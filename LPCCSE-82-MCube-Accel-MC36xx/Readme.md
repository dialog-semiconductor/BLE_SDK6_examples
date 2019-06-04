
# DA14585/586 interface with mCube Accelerometer over I2C


## Example description

Sample software application to interface Dialog DA14585/586 BLE SoC's with mCube MC36xx (MC3672/35) accelerometers.

This DA14585 example shows how to communicate with a mCube accelerometer sensor over I2C interface. The sensor data gets sent over BLE with a notification when connected to a mobile application. Features in this example includes the implementation code for sensor sniff mode for low-power application and FIFO as sensor buffer.


## HW and SW configuration

### Hardware configuration

The current example focuses on the implementation of Accel connected via I2C and sending sensor data over BLE notifications using the DA14585 & MC3672. For SPI interface implementation refer to [MC3672 Quick Start Guide](https://mcubemems.com/wp-content/uploads/2018/05/MC3672-Quick-Start-Guide-APS-045-0017v1.2.pdf) which demos the MC3672 Arduino implementation or [mCube-MC36XX-Programming Guide](https://github.com/mcubemems/Accelerometer_mc36xx_mcu_driver/blob/master/MC36XX_MCU_3.1.0/mCube%20-%20MC36XX%20-%20Programming%20Guide%20%20v1.1.1.pdf) for general platforms.

**Note:** Both the SPI and I2C interfaces cannot be active at the same time as the clock (SCK) and data (SDA) are shared between the two protocols.

- This SW example runs on The [DA14585/DA14586](https://www.dialog-semiconductor.com/products/connectivity/bluetooth-low-energy/smartbond-da14585-and-da14586) Bluetooth Smart SoC devices.
- The Basic or Pro Development kit is needed for this example.
- Connect the SCL pin of the [EV3672A](https://mcubemems.com/product/mc3672-3-axis-accelerometer) to pin 1-0 of the development board.
- Connect the SDA pin of the [EV3672A](https://mcubemems.com/product/mc3672-3-axis-accelerometer) to pin 1-2 of the development board.
- These pins are defined in the m_drv_interface.h file

<p align="center">
	<img src="assets/Hardware_setup_DA14585_MC36xx.jpg">
</p>

### Software configuration

Before user starts to load this DEMO. The following software tools has to be setup.
- This example requires:
	- Keil μVision 5
	- Dialog SDK 6.0.10
	- Smartphone with a BLE scanning app (for example BLE Scanner on Android or Lightblue on IOS)
	- **SEGGER’s J-Link** tools should be downloaded and installed.


## How to run the example

The implementation of mCube accelerometer driver is described in the [mCube-MC36XX-Programming Guide](https://github.com/mcubemems/Accelerometer_mc36xx_mcu_driver/blob/master/MC36XX_MCU_3.1.0/mCube%20-%20MC36XX%20-%20Programming%20Guide%20%20v1.1.1.pdf). Customers using mCube sensors for the first time, refer to this document for understanding the driver implementation. The repository also includes the driver source code if needed.

[Accelerometer mc36xx mcu driver](https://github.com/mcubemems/Accelerometer_mc36xx_mcu_driver)

### Compile and run

To compile and run this program, user can follow the steps below.
- Open the project mCube_mc36xx_data_notifcation in Keil μVision 5.
- Optionally, change the parameters in m_drv_mc36xx.c
- Compile and run the project
- Open the BLE scanner app and look for DLG-ACCL
- Connect to the device
- Subscribe to the notifications

<p align="center"> 
	<img src="assets/BLE_Notif_DA14585_MC36xx.jpg">
</p>

- To enable the Sniff and/or FIFO modes, modify the configurations as show below. The default FIFO threshold is 5 samples but it can set to a maximum of 32 samples or just set FIFO to FIFO_FULL mode.

>
	//#define SNIFF_DEMO //Remove this comment to show the sniff DEMO

	//#define FIFO_DEMO //Remove this comment to show the FIFO DEMO

	#define FIFO_DEEP 32

	#define FIFO_THRESHOLD 5


## Reference

- Please refer to the document from the link below to set up this software example on your machine.

 [How to setup software Example](https://www.dialog-semiconductor.com/sites/default/files/sw-example-da1458x-example-setup.pdf)

## Known Limitations

- There are No known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations](https://www.dialog-semiconductor.com/sites/default/files/da1458x-knownlimitations_2019_01_07.pdf).
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software).
- You can also refer to the Troubleshooting section in the DA1585x Getting Started Guide with the Development Kit [UM-B-049](https://www.dialog-semiconductor.com/sites/default/files/um-b-049_da14585da14586_getting_started_guide_v2.1_0.pdf).


## License


**************************************************************************************

 Copyright (c) 2018 Dialog Semiconductor. All rights reserved.

 This software ("Software") is owned by Dialog Semiconductor. By using this Software
 you agree that Dialog Semiconductor retains all intellectual property and proprietary
 rights in and to this Software and any use, reproduction, disclosure or distribution
 of the Software without express written permission or a license agreement from Dialog
 Semiconductor is strictly prohibited. This Software is solely for use on or in
 conjunction with Dialog Semiconductor products.

 EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.

**************************************************************************************
