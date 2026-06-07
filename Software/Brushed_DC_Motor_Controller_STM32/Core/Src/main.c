/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
void parseI2C();
void fail();
float Proportional(int err);
float Derivative(int err);
float Integral(int err);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);
void PID_Response(float output);
void turnReverse(float duty);
void turnForward(float duty);

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern I2C_HandleTypeDef hi2c1;
float int absPosition=0;

int var = 0;
int rot = 0;
int speed = 0;
int direction = 1; //left is 0, right is 1
int currentPosition = 0;
int targetPosition= 0;
float previousTarget = 0;
int error = 0;

int accum = 0; //Accumulated integral. Will reset when desired position changes
int tempProp = 0; //the previous error, used to calculate derivative

float Kd= 1; // derivative coefficient
float Kp= 1; // proportional coefficient
float Ki= 1; //integral coefficient

float sum = 0; //sum of all three PID parts
uint8_t RX_Buffer [4]; //buffer for [2 bytes for angle number (0 to 359.9945 degrees)], 1 byte number (0 for left or 1 for right), [1 byte 0-255 (speed)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  parseI2C();
	  if (targetPosition != previousTarget){
		  accum = 0;
		  previousTarget = targetPosition;
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void PID_Response(float output){

if (output < 0 ){
turnReverse(-output);
}

else {
turnForward(output);
 }
}

void turnForward(float duty){
TIM3 -> CCR2 = duty; //INA1
TIM3 -> CCR1 =0 ; //INA2
}

void turnReverse(float duty){
TIM3 -> CCR2 = 0; //INA1
TIM3 -> CCR1 = duty; //INA2
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim){
error = targetPosition - currentPosition; //measured in angles
if (error > 180){
error = error - 360;
}
if (error < -180){
error = error + 360;
}
absPosition += error;
if (absPosition >= 180){
absPosition = 180;
}
if (absPosition <= -180){
absPosition = -180;
}

sum = Proportional(error) + Integral(error) + Derivative(error);
if (sum > TIM3->ARR) {
sum = TIM3->ARR;
}
if (sum < -1 * TIM3->ARR) {
sum = -1* TIM3->ARR; //clamping PID Response
}
PID_Response(sum);
}
//SUM RANGES FROM -1279 to 1279



void parseI2C(){
HAL_StatusTypeDef Ret;
Ret = HAL_I2C_Slave_Receive(&hi2c1 ,RX_Buffer, 4,1000);
if (Ret != HAL_OK){
fail();
}
else{
HAL_I2C_Slave_Transmit(&hi2c1,RX_Buffer, 4, 1000);
}

targetPosition = ((RX_Buffer[0] << 8) | RX_Buffer[1]) * 360 / 65535;

direction = RX_Buffer[2];

speed = RX_Buffer[3];}
void fail(){
printf("Oopsies! Look like there’s a problem =<");

}

float Proportional(int err){

return Kp * err;
}

float Integral(int err){
accum += err;
if (accum >= 1000000){
	  accum = 1000000;
	  }
if (accum <= -1000000){
	  accum = -1000000;
	  }
return (Ki * accum);

}

float Derivative(int err){
int temp = tempProp;
tempProp = err; //also make sure this updates the global variable tempProp
return ((tempProp - temp)/(0.01) * Kd);
}

//Okay, now Var should hold the values transmitted by I2C. Example, [0x00][0xB4][0x01][0x80]

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
