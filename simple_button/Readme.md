# DA14585/DA14586/DA14531 Peripheral GPIO - Simple Button press

---


## Example description

This SDK6 example shows how to configure a button for short press and long press (>3 seconds)
from the applicaiton layer.
As a proof of concept it can be verified by transmitting UART messages and/or the LED status

## HW and SW configuration

	- This example runs on The DA14585 and the DA14531 Bluetooth Smart SoC devices.	
	- The Basic / Pro Development kit can be used for the DA14585.
	- The DA14531 requires the Pro Development kit
	
	User manual for the Pro Development kit can be found [here](https://www.dialog-semiconductor.com/products/da14531-development-kit-pro)
	
	Follow the hardware configuration according to your Daughterboard (DA14585 or DA14531) and Development Kit, Pro or Basic.

* **Hardware configuration DA14531 using Pro Development Kit**

	- Connect the USB Development kit to the host computer.
	- UART TX: connect P21 on J2 to UTX pin 17 on J1 as shown in the image below (the blue line)
	- LED jumper is configured to P1_0 (red box 2)
	- Button jumper is configured from SW2 pin to P3_1 (red box 1)
	  The image below shows the Motherboard with jumper (wire) configuration for the DA14531

		![Motherboard_Hardware_Configuration_DA14531](assets/Motherboard_Hardware_Configuration_DA14531.png)
		
* **Hardware configuration DA14585 using the Pro Development Kit**

	- Connect the USB Development kit to the host computer.
	- UART TX jumper on P0_4 (red box 1)
	- LED jumper is configured to P1_0 (red box 3)
	- Button jumper is configured from SW3 pin to P1_1 (red box 2)
	  The image below shows the Motherboard with jumper configuration for the DA14585

		![Motherboard_Hardware_Configuration_DA14585](assets/Motherboard_Hardware_Configuration_DA14585.png)

* **Hardware configuration DA14585 using the Basic Development Kit**

	- Connect the USB Development kit to the host computer.
	- UART TX/RX jumper on P0_4/P0_5
	- LED jumper is configured to P10
	- An active-low switch should be connected to P1_1 as displayed in the following schematic

	   ![ADXL_Fritzing](assets/ADXL_Fritzing.png)

* **Software configuration**
 
	- This example requires:
    * SDK6.0.12
	* Keil5
	- **SEGGER’s J-Link** tools should be downloaded and installed.

## How to run the example

For initial setup of the example please visit [initial project setup](https://www.dialog-semiconductor.com/sites/default/files/sw-example-da145x-example-setup.pdf).

If not done already, please visit the getting started guide accourding to your device [DA14585](http://lpccs-docs.dialog-semiconductor.com/da14585_getting_started/index.html) or [DA14531](NO LINK AVAILABLE YET)

### Initial Setup

 - Start Keil
 
 - Expand the dialog shown in the red box in the image below

	  ![Expand_Select_Device](assets/Expand_Select_Device.png)

 - Select your device DA14531, DA14586 or DA14585.
		
	  ![Select_Device](assets/Select_Device.png)
 
 - Compile (F7) and launch (ctrl F5) the example, if the warning (shown below) pops up press OK
 
	  ![warning](assets/warning.png)

 - A screen similar to the image below is shown, press F5 to run the example

	  ![Start_Example](assets/Start_Example.png)
	
 - Open the development kit serial port with the following parameters

		- baudrate: 115200
		- data: 8 bits
		- stop: 1 bit
		- parity: None
		- flow  control: none


 - Identify the corresponding led
	- For the Pro Development Kit the led is called *D5* and the colour of the led is *orange*.
 	- For the Basic Development Kit the led is called *USR* and the colour of the led is *green*. 
 
 
 - Pressing the button (SW3 for the DA14585 and SW2 for the DA14531) for less than 3 seconds (i.e. short press) to toggle the led status
 - Pressing the button (SW3 for the DA14585 and SW2 for the DA14531) for 3 seconds or more (i.e. long press) will start LED blinking ON/OFF
 - Pressing the button (SW3 for the DA14585 and SW2 for the DA14531) for less than 3 seconds (i.e. short press) again will stop LED blinking or toggle the led status

	![simple_button_uart](assets/uart_data.png)


## Known Limitations


- There are No known limitations for this example.
- For the DA14585 and the DA14586 known hardware limitations please visit [known hardware limitations DA1458x](https://www.dialog-semiconductor.com/sites/default/files/da1458x-knownlimitations_2019_01_07.pdf "known hardware limitations DA1458x").
- For the DA14531 known hardware limitations please visit [DA14531 known hardware limitations](https://www.dialog-semiconductor.com/da14531_HW_Limitation)


## License


**************************************************************************************

 Copyright (c) 2019 Dialog Semiconductor. All rights reserved.

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
