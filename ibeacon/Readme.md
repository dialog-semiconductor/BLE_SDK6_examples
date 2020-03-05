# ibeacon

iBeacon implementation for the DA14531

## Example description

Simple example showing how to implement an iBeacon on the DA14531. All beacon payload
parameters (advertising interval, UUID etc.) are easily configurable from within the user
application (user_app.c).

The example is configured to use a random static Bluetooth Device address (BD address). This 
is generated upon power on or reset and will remain static throughout the life of the device. 
It is also guaranteed to be unique on each DA14531 device (i.e. a different BD address will be 
generated on each DA14531 on which this is example is run).

By default the output power is set to 0dBm. This can be increased to +2.5dBm by defining the
macro TX_POWER_2d5Bm (see the macro definitions at the start of the user_app.c file).
 	
## HW and SW configuration


* **Hardware configuration**

	- This example runs on the DA14531 Bluetooth Smart SoC device.
	- A DA14531 [USB](https://www.dialog-semiconductor.com/products/da14531-development-kit-usb) or [PRO](https://www.dialog-semiconductor.com/products/da14531-development-kit-pro) Development kit is needed for this example.
	
* **Software configuration**

	- This example requires:
        * Smartsnippets Studio 2.0.10 (or later)
        * SDK6.0.12 (or later)
		* SEGGER’s J-Link tools should be downloaded and installed.

## How to run the example

For initial setup of the example please refer to [this section of the dialog support portal](http://lpccs-docs.dialog-semiconductor.com/Software_Example_Setup/index.html).



### Initial Setup

1.  Build and download the example using the Keil IDE. 
2.  Run the example using the Keil debugger.
3.  Use a Smart Device running an App such as Locate to view the beacons transmitted by the DA14531.

## Known Limitations

- There are no known limitations for this example. But you can check and refer to the following 
  application note for known hardware limitations for DA14531 devices:
  https://www.dialog-semiconductor.com/sites/default/files/da14531_errata_1v0.pdf
- Dialog Software [Forum Link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software "Forum Link").
- You can also refer to the troubleshooting section in the [Getting Started with the DA14531 PRO Development Kit](http://lpccs-docs.dialog-semiconductor.com/UM-B-117-DA14531-Getting-Started-With-The-Pro-Development-Kit/index.html).


## License


**************************************************************************************

 Copyright (c) 2020 Dialog Semiconductor. All rights reserved.

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
