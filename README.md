<h1 align="center"><u>Pusher 1.0</u></h1>

<p align="center"><i>June 8, 2026</i></p>

**Affiliation:** Fallout, Hackclub

Pusher is a **4 Degree of Freedom wearable robotic arm** based on the anatomical structure of the human arm, with a maximum reach of approximately **0.65 meters** and a weight of **5 kilograms**. It is designed for **modularity**, aiming to serve as an affordable, general-purpose **research platform** for Human-Computer Interfacing, such as studies involving **EMG, EEG, and Computer Vision**, to assist in the **control of supernumerary arms**. As such, this project is intended for both undergraduate researchers and hobbyists looking for an introduction to the study of HCI robotics, with potential to be used in graduate-school prototyping due to the ease of production and low cost.

<h1 align="center"><u>Table of Contents</u></h1>

1. [Bill of Materials](#bill-of-materials)
2. [System Diagram](#system-diagram)
3. [Building the Project](#building-the-project)
   1. [Prerequisites](#prerequisites)
   2. [Assembling Mechanical Design](#assembling-mechanical-design)
   3. [Assembling Electrical Design](#assembling-electrical-design)
   4. [Flashing Firmware](#flashing-firmware)
4. [Author's Notes](#authors-notes)
   1. [Commentary on BOM](#commentary-on-bom)
   2. [Guide to Adding Implementations (Taking Advantage of Modularity)](#guide-to-adding-implementations-taking-advantage-of-modularity)
   3. [Future Implementations by Author](#future-implementations-by-author)

<h1 align="center"><u>Bill of Materials</u></h1>

### Electronics

| Index | LCSC # | Quantity | Value | Link |
|---|---:|---:|---:|---|
| 1 | C146170 | 2 | 1.81 | [LCSC](https://www.lcsc.com/product-detail/C146170.html) |
| 2 | C22396880 | 6 | 10.34 | [LCSC](https://www.lcsc.com/product-detail/C22396880.html) |
| 3 | C49678 | 100 | 0.84 | [LCSC](https://www.lcsc.com/product-detail/C49678.html) |
| 4 | C72487 | 20 | 0.57 | [LCSC](https://www.lcsc.com/product-detail/C72487.html) |
| 5 | C24123 | 20 | 0.30 | [LCSC](https://www.lcsc.com/product-detail/C24123.html) |
| 6 | C3869051 | 20 | 0.57 | [LCSC](https://www.lcsc.com/product-detail/C3869051.html) |
| 7 | C8465 | 5 | 0.67 | [LCSC](https://www.lcsc.com/product-detail/C8465.html) |
| 8 | C72334 | 5 | 1.01 | [LCSC](https://www.lcsc.com/product-detail/C72334.html) |
| 9 | C86513 | 4 | 6.79 | [LCSC](https://www.lcsc.com/product-detail/C86513.html) |
| 10 | C114548 | 100 | 0.36 | [LCSC](https://www.lcsc.com/product-detail/C114548.html) |
| 11 | C2930231 | 100 | 0.19 | [LCSC](https://www.lcsc.com/product-detail/C2930231.html) |
| 12 | C90964 | 5 | 3.70 | [LCSC](https://www.lcsc.com/product-detail/C90964.html) |
| 13 | C58159 | 1 | 0.38 | [LCSC](https://www.lcsc.com/product-detail/C58159.html) |
| 14 | C7467397 | 50 | 0.65 | [LCSC](https://www.lcsc.com/product-detail/C7467397.html) |
| 15 | C2930266 | 100 | 0.19 | [LCSC](https://www.lcsc.com/product-detail/C2930266.html) |
| 16 | C2907315 | 100 | 0.22 | [LCSC](https://www.lcsc.com/product-detail/C2907315.html) |
|  |  | **Total** | **28.59** |  |

### Mechanical

| Part name | Quantity | Value | Link |
|---|---:|---:|---|
| PLA Filament | 1 | 7.36 | [AliExpress](https://www.aliexpress.us/item/3256806054300015.html?spm=a2g0o.productlist.main.3.6f78379d5OwR1X&algo_pvid=498c2f52-b8c2-4add-bea6-f9bda40c924e&algo_exp_id=498c2f52-b8c2-4add-bea6-f9bda40c924e-2&pdp_ext_f=%7B%22order%22%3A%22488%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21USD%2122.79%217.36%21%21%2122.79%217.36%21%402140c1c317807875296987000e9684%2112000058209259440%21sea%21US%210%21ABX%211%210%21n_tag%3A-29910%3Bd%3Ae1bbef76%3Bm03_new_user%3A-29895%3BpisId%3A5000000208023469&curPageLogUid=Qx4M7j8exyPx&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005006240614767%7C_p_origin_prod%3A) |
| motors | 4 | 30 | [Precision Mini Drives](https://precisionminidrives.com/product/shaft-diameter-8mm-10mm-micro-dc-gear-motor-model-nfp-42-775-ce-12v-24v?attribute_option=12V%2C+80RPM&attribute_shaft-diameter=10mm&utm_source=GoogleAds&utm_campaign=PMax&utm_medium=Google%20Shopping&utm_term=adtribes&utm_content=%7Bproduct_id%7D&gad_source=1&gad_campaignid=18478080173&gbraid=0AAAAAomY7RXK-6ddvY5v_P-wvX2-wvXLt&gclid=CjwKCAjw5NvPBhAoEiwA_2egfvITADIqACLw4gyQt7JJ5pv2qg0MlFNwTrXCpuVo1aJEWzca6XUyrBoCjcwQAvD_BwE) |
| RU-28 | 4 | 19.91 | [AliExpress](https://www.aliexpress.us/item/3256809392312866.html?src=google&src=google&albch=shopping&acnt=708-803-3821&isdl=y&slnk=&plac=&mtctp=&albbt=Google_7_shopping&aff_platform=google&aff_short_key=UneMJZVf&gclsrc=aw.ds&albagn=888888&ds_e_adid=&ds_e_matchtype=&ds_e_device=c&ds_e_network=x&ds_e_product_group_id=&ds_e_product_id=en3256809392312866&ds_e_product_merchant_id=107675220&ds_e_product_country=US&ds_e_product_language=en&ds_e_product_channel=online&ds_e_product_store_id=&ds_url_v=2&albcp=20123152476&albag=&isSmbAutoCall=false&needSmbHouyi=false&gad_source=1&gad_campaignid=20127768206&gbraid=0AAAAAD6I-hH5MzbM-Qht5WI26YJI1jQ7x&gclid=CjwKCAjw5NvPBhAoEiwA_2egfknGl0bGc7tc0ydw4pmODKzmqiRzPcj2s5EQ8m0HpaPSL7tY9xceahoCLmEQAvD_BwE&gatewayAdapt=glo2usa) |
| M5 Screws (x50) | 1 | 15 | [Amazon Japan](https://www.amazon.co.jp/-/en/Button-Socket-Screws-Finish-BHSSM5X10B-50-SL/dp/B07DZN3FP5) |
| M3 screws and nuts (x20) | 1 | 9 | [Amazon Japan](https://www.amazon.co.jp/-/en/Phillips-Machine-Washers-Plastic-Countersunk/dp/B0B9YGNKV6) |
| m5 nuts (x50) | 1 | 13 | [Amazon Japan](https://www.amazon.co.jp/-/en/Thin-Nuts-Stainless-Steel-Flat/dp/B0CQ55XWQX?th=1) |
|  |  | **Total** | **244** |

**Total project cost:** **272.59**

<h1 align="center"><u>System Diagram</u></h1>


<h1 align="center"><u>Building the Project</u></h1>

## Prerequisites

This source document does not provide a completed prerequisites list.

## Assembling Mechanical Design

For assembly, please see the `FINAL ASSEMBLY` file in the `mechanical design` folder, opened in CAD software such as Fusion 360.

The material modeled for stress testing and manufacturability is PLA plastic, intended for 3D printers such as the Bambu models.

In a slicer such as Cura, print the following:

- 1x `Bicep_And_Cap`
- 1x `Bicep_RU_28_Connector` (in `Bicep_And_Cap`)
- 1x `Forearm`
- 3x `Joint_Cover`
- 1x `Joint1`
- 2x `Joints_2_and_3`
- 3x `RU_28_Connector`

Approximately ~2 kg in PLA.

To assemble the physical design, it is recommended to have 30x M5 screws (10 mm) and 30x matching nuts. Then, use 15x M3 screws (10 mm) with 15x matching nuts.

## Assembling Electrical Design

Trusted vendors such as [JLCPCB](https://jlcpcb.com/) and [LCSC](https://www.lcsc.com/).

After ordering your PCB and components from JLCPCB and LCSC, hand-solder them as follows:

- 4x motor driver PCBs
- 1x main controller PCB

Motor driver PCBs are secured in a dedicated mount, which is fastened to the joints themselves.

The controller PCB is left separate from the rest of the arm. The recommended option is to keep it on a desk during assembly and strap it to the arm during operation.

The wiring diagram is as follows:

- Red — 24 V and 3.3 V power supply bus
- Yellow — I2C communication bus

Connections:

- From Joint 1 to Joint 2
- From Joint 2 to Joint 3
- From Joint 3 to Bicep

The firmware includes a 360-degree clamp to prevent wires from excessive twisting and strain.

## Flashing Firmware

### Flashing STM32F103C8T6

Note: The firmware should be placed in the `Brushed_DC_Motor_Controller_STM32` folder. This is the folder you select when creating a project from an existing STM32CubeIDE project.

1. Connect `SWDIO`, `SWCLK`, `GND`, and `3V3` on the motor driver STM32 to the corresponding pins on your ST-LINK. A STM32 Nucleo board can be used to avoid third-party flashers that do not work.
2. Remove the ST-Link isolation jumpers on CN2 of the Nucleo board so that you program the DC motor driver rather than the Nucleo itself.
3. Import `Brushed_DC_Motor_Controller_STM32` as an **Existing STM32 project** into STM32CubeIDE.
4. Open the project, find the build tool on the top toolbar, and click it. Verify that there are no errors. If workspace configuration causes errors, close STM32CubeIDE, reopen it, and select the `Software` folder as the workspace.
5. Click the debug tool.
6. Click Run.

If flashing succeeds, STM32CubeIDE should display a successful download message in the console.

### Flashing ESP32

1. Follow the steps in the AKP's IoT Tech video to download and set up ESP32 IDF. Stop around 4:20, after creating a project folder.

YouTube video: https://www.youtube.com/watch?v=xQ-Zk0FaK-c

2. At 4:20, replace the code in `hello_world_main.c` with the contents of `main.c` located in the `ESP32 Joystick Controller` folder.
3. The code does not use external libraries, so no additional imports are needed.
4. In the ESP32-IDF terminal, type:

```bash
idf.py set-target esp32
idf.py build
```

5. Connect the ESP32 to your computer via USB.

On Windows, select the ESP32 model and the communication port. Ports usually appear as `COMx`.

Then run:

```bash
idf.py -p COM3 flash
```

Replace `3` in `COM3` with the corresponding COM number.

#### Mac

On Mac, ports usually appear as `/dev/ttyUSBx`.

You can identify the port with:

```bash
ls -la /dev | grep -E "cu\.(SLAB|usbserial)"
```

Then follow these steps:

```bash
source ~/esp/esp-idf/export.sh
idf.py set-target esp32
idf.py -p /dev/cu.SLAB_USBtoUART flash
```

#### Linux

On Linux, ports usually appear as `/dev/ttyACMx`.

First, add your user to the hardware access group:

Ubuntu/Debian:

```bash
sudo usermod -aG dialout $USER
```

Arch Linux:

```bash
sudo usermod -aG uucp $USER
```

Then, find the device path with:

```bash
ls /dev/ttyUSB* /dev/ttyACM*
```

Then navigate into your project directory and select the model:

```bash
idf.py set-target esp32
```

Finally, flash and monitor:

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

Replace `/dev/ttyUSB0` with the actual device path identified above.

<h1 align="center"><u>Author's Notes</u></h1>

## Commentary on BOM

## Guide to Adding Implementations (Taking Advantage of Modularity)

## Future Implementations by Author


