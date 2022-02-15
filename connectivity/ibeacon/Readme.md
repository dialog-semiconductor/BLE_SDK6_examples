------
# ibeacon
------

iBeacon implementation for the DA14531, DA14585/DA14586

## Example description

Simple example showing how to implement an iBeacon on the DA14531, DA14585/586. All beacon payload
parameters (advertising interval, UUID etc.) are easily configurable from within the user
application (user_app.c).

The example is configured to use a random static Bluetooth Device address (BD address). This 
is generated upon power on or reset and will remain static throughout the life of the device. 
It is also guaranteed to be unique on each DA14531 or DA14585/586 device (i.e. a different BD address will be 
generated on each DA14531 or DA14585/586 on which this is example is run).

By default the output power is set to 0dBm. This can be increased to +2.5dBm (only for the DA14531) by defining the
macro TX_POWER_2d5Bm (see the macro definitions at the start of the user_app.c file).
 	
## HW and SW configuration


* **Hardware configuration**

	- This example runs on the DA14531 or DA14585/DA14586 Bluetooth Smart SoC device.
	- For running the example on DA14531, a [USB Kit](https://www.dialog-semiconductor.com/products/da14531-development-kit-usb) or [PRO](https://www.dialog-semiconductor.com/products/da14531-development-kit-pro) Development kit along with a DA14531 Daughter Board is needed.
  - For running the example on DA14585/586, a [PRO](https://www.dialog-semiconductor.com/products/da14531-development-kit-pro) Development kit along with a DA14585 or a DA14586 Daughter Board is needed.
	
* **Software configuration**

	- This example requires:
        * Smartsnippets Studio 2.0.10 (or later)
        * SDK6.0.14 (or later)
		* SEGGER’s J-Link tools should be downloaded and installed.

## How to run the example

For the initial setup of the project that involves linking the SDK to this SW example, please follow the Readme [here](../../Readme.md).

### Initial Setup

1.  Build and download the example using the Keil IDE. 
2.  Run the example using the Keil debugger.
3.  Use a Smart Device running an App such as Locate to view the beacons transmitted by the device.

## Known Limitations

- There are No known limitations for this example. But you can check and refer to: 

	- [SDK6 Known Limitations](http://lpccs-docs.dialog-semiconductor.com/sdk6_kll/index.html).
	- [known hardware limitations for DA14531 devices](https://www.dialog-semiconductor.com/da14531_HW_Limitation).
	- [DA14531 Getting Started guide](https://www.dialog-semiconductor.com/da14531-getting-started).
	- [DA14585 hardware limitations](https://www.dialog-semiconductor.com/sites/default/files/da1458x-knownlimitations_2019_01_07.pdf)
	- [DA14585/DA14586 Getting Started with the Development Kit UM-B-049](http://lpccs-docs.dialog-semiconductor.com/da14585_getting_started/index.html).
	- [Dialog BLE Support Forum](https://www.dialog-semiconductor.com/BLE_Support).


