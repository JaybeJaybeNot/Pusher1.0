#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/i2c_master.h"
#include "esp_err.h"

//—-----I2C_Constants—---

#define I2C_SCL_GPIO 47
#define I2C_SDA_GPIO 48
#define I2C_FREQ_HZ   100000

#define C1_ADDR 0x58
#define C2_ADDR 0x59
#define C3_ADDR 0x5A
#define C4_ADDR 0x5B

static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t c1_handle;
static i2c_master_dev_handle_t c2_handle;
static i2c_master_dev_handle_t c3_handle;
static i2c_master_dev_handle_t c4_handle;

// -------------------- Pins --------------------

#define SDA_PIN   GPIO_NUM_48
#define SCL_PIN   GPIO_NUM_47
#define BUTTON1   GPIO_NUM_14
#define BUTTON2   GPIO_NUM_9   // currently unused, kept for later

// -------------------- ADC_channels --------------------

#define JOY_X_CHANNEL  ADC_CHANNEL_0
#define JOY_Y_CHANNEL  ADC_CHANNEL_1

//—------I2C Setup
static void i2c_init_four_devices(void)
{
    i2c_master_bus_config_t bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = I2C_SCL_GPIO,
        .sda_io_num = I2C_SDA_GPIO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };

  ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .scl_speed_hz = I2C_FREQ_HZ,
        .scl_wait_us = 0,
    };

    dev_cfg.device_address = C1_ADDR;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &c1_handle));

    dev_cfg.device_address = C2_ADDR;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &c2_handle));

    dev_cfg.device_address = C3_ADDR;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &c3_handle));

    dev_cfg.device_address = C4_ADDR;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &c4_handle));
}


// -------------------- Motion / IK --------------------
static float pos[3]   = {0.2f, 0.2f, 0.2f};   // x, y, z
static float joint[3]  = {0.0f, 0.0f, 0.0f};   // base yaw, shoulder, elbow/wrist placeholder
static float elbow     = 0.0f;
static float epsilon   = 0.0f;                // wrist rotation target

static float L1 = 0.50f;  // arm link 1 length
static float L2 = 0.40f;  // arm link 2 length
static bool elbowUp = true;

static const int minCap = 100;               // joystick deadzone half-width
static const float maxReach = 0.50f + 0.40f; // L1 + L2
static const float POSITION_EPS = 1e-4f;

static const float MAX_LINEAR_SPEED = 0.25f;  // m/s
static const float MAX_Z_SPEED      = 0.10f;   // m/s
static const float MAX_WRIST_SPEED  = 0.78539816339f; // rad/s = pi/4

static const char *TAG = "JOY";

static adc_oneshot_unit_handle_t adc1_handle;
static int64_t last_time_us = 0;

// -------------------- helpers --------------------
static inline float clampf_local(float v, float lo, float hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static float joystickDirectionConvert(int raw)
{
    const int center = 2048;
    int delta = raw - center;

    if (abs(delta) <= minCap) {
        return 0.0f;
    }

    float out = (float)delta / 2048.0f;

    // Clamp to [-1, 1]
    out = clampf_local(out, -1.0f, 1.0f);
    return out;
}

static void limitRadius(void)
{
    float radius = sqrtf(pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2]);

    if (radius > maxReach && radius > 0.0f) {
        float scale = maxReach / radius;
        pos[0] *= scale;
        pos[1] *= scale;
        pos[2] *= scale;
    }
}

static void inverseKinematics(void)
{
    // Treat pos[0], pos[1] as base plane x/y, pos[2] as vertical z.
    float x = pos[0];
    float y = pos[1];
    float z = pos[2];

    float r_xy = sqrtf(x * x + y * y);
    float d = sqrtf(r_xy * r_xy + z * z);

    if (d < POSITION_EPS) {
        d = POSITION_EPS;
    }

    float cos_elbow = (L1 * L1 + L2 * L2 - d * d) / (2.0f * L1 * L2);
    cos_elbow = clampf_local(cos_elbow, -1.0f, 1.0f);
    elbow = acosf(cos_elbow);

    float cos_shoulder = (L1 * L1 + d * d - L2 * L2) / (2.0f * L1 * d);
    cos_shoulder = clampf_local(cos_shoulder, -1.0f, 1.0f);
    float shoulder_offset = acosf(cos_shoulder);

    float base_yaw = atan2f(y, x);
    float elevation = atan2f(z, r_xy);

    joint[0] = base_yaw;
    joint[1] = elbowUp ? (elevation + shoulder_offset) : (elevation - shoulder_offset);
    joint[2] = epsilon;
}



static int read_joystick_axis(adc_channel_t channel)
{
    int raw = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, channel, &raw));
    return raw;
}

static void parseJoystick(float dt)
{
    int xRaw = read_joystick_axis(JOY_X_CHANNEL);
    int yRaw = read_joystick_axis(JOY_Y_CHANNEL);

    float xAxis = joystickDirectionConvert(xRaw);
    float yAxis = joystickDirectionConvert(yRaw);

    // Button pressed = active-low with pull-up enabled.
    bool pressed = (gpio_get_level(BUTTON1) == 0);

    if (pressed) {
        // While pressed: use stick for z + wrist
        pos[2] += yAxis * MAX_Z_SPEED * dt;
        epsilon += xAxis * MAX_WRIST_SPEED * dt;
    } else {
        // Normal mode: move x/y
        pos[0] += xAxis * MAX_LINEAR_SPEED * dt;
        pos[1] += yAxis * MAX_LINEAR_SPEED * dt;
    }

    limitRadius();
    inverseKinematics();
}

static void init_gpio(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON1),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
}

static void init_adc(void)
{
    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc1_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, JOY_X_CHANNEL, &chan_cfg));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, JOY_Y_CHANNEL, &chan_cfg));
}

static void joystick_task(void *arg)
{
    last_time_us = esp_timer_get_time();

    while (1) {
        int64_t now_us = esp_timer_get_time();
        float dt = (now_us - last_time_us) / 1000000.0f;
        last_time_us = now_us;

        if (dt < 0.0f) dt = 0.0f;
        if (dt > 0.1f) dt = 0.1f;

        // 1. Read joysticks and calculate Inverse Kinematics
        parseJoystick(dt);

        // 2. Transmit the freshly computed angles to your 4 motor controllers
        send_to_all_four();

        // 3. Log the status updates
        ESP_LOGI(TAG,
                 "pos=(%.3f, %.3f, %.3f)  joint=(%.3f, %.3f, %.3f)  elbow=%.3f",
                 pos[0], pos[1], pos[2],
                 joint[0], joint[1], joint[2],
                 elbow);

        // 4. Yield CPU control for 10ms (Allows background tasks to breathe)
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
void send_motor_command(
    i2c_master_dev_handle_t dev,
    float angle_rad,
    uint8_t speed)
{
    uint8_t tx_buf[4];

    float angle_deg = angle_rad * 180.0f / M_PI;

    uint8_t direction;

    if (angle_deg >= 0.0f)
    {
        direction = 1;
    }
    else
    {
        direction = 0;
        angle_deg = -angle_deg;
    }

    // clamp to representable range
    if (angle_deg > 359.99f)
    {
        angle_deg = 359.99f;
    }

    uint16_t angle_encoded = (uint16_t)(angle_deg * 100.0f);

    tx_buf[0] = (angle_encoded >> 8) & 0xFF;
    tx_buf[1] = angle_encoded & 0xFF;
    tx_buf[2] = direction;
    tx_buf[3] = speed;

    ESP_ERROR_CHECK(
        i2c_master_transmit(
            dev,
            tx_buf,
            sizeof(tx_buf),
            pdMS_TO_TICKS(100)
        )
    );
}




void app_main(void) { // Initialize Hardware Peripherals

 init_gpio();
 init_adc(); 
i2c_init_four_devices(); // Create the infinite cyclic control task 
xTaskCreate(joystick_task, "joystick_task", 4096, NULL, 5, NULL); 

// app_main can safely finish or delete itself; // FreeRTOS keeps the spawned task alive. vTaskDelete(NULL); }

