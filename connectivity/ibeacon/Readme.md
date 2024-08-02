# ibeacon

## Example description

Simple example showing how to implement an iBeacon on the DA14531, DA14585/586. All beacon payload
parameters (advertising interval, UUID etc.) are easily configurable from within the user
application (user_app.c).

The example is configured to use a random static Bluetooth Device address (BD address). This 
is generated upon power on or reset and will remain static throughout the life of the device. 
It is also guaranteed to be unique on each DA1453x or DA14585/586 device (i.e. a different BD address will be 
generated on each DA1453x or DA14585/586 on which this is example is run).

By default the output power is set to 0dBm. This can be increased to +2.5dBm (only for the DA1453x) by defining the
macro TX_POWER_2d5Bm (see the macro definitions at the start of the user_app.c file).
 	
## HW and SW configuration


* **Hardware configuration**

	- This example runs on the DA1453x or DA14585/DA14586 Bluetooth Smart SoC device.
	- For running the example on DA1453x, a [USB Kit](https://www.dialog-semiconductor.com/products/da14531-development-kit-usb) , [PRO_531](https://www.renesas.com/us/en/products/wireless-connectivity/bluetooth-low-energy/da14531-00fxdevkt-p-smartbond-tiny-da14531-bluetooth-low-energy-51-system-chip-development-kit-pro) or [PRO_53x](https://www.renesas.com/us/en/products/wireless-connectivity/bluetooth-low-energy/da14535-00fxdevkt-p-smartbond-tiny-da14535-bluetooth-low-energy-53-soc-development-kit-pro) Development kit along with a DA1453x Daughter Board is needed.
  - For running the example on DA14585/586, a [Basic](https://www.renesas.com/us/en/products/wireless-connectivity/bluetooth-low-energy/da14585-00atdevkt-b-smartbond-da14585-bluetooth-low-energy-basic-development-kit) Development kit along with a DA14585 or a DA14586 Daughter Board is needed.
	
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

## Further reading

- [Wireless Connectivity Forum](https://lpccs-docs.renesas.com/lpc_docs_index/DA145xx.html)



## Known Limitations

- There are no known limitations for this example. But you can check and refer to the following application note for
[SDK6 known limitations](https://lpccs-docs.renesas.com/sdk6_kll/index.html)

## Feedback and support ?

If you have any comments or suggestions about this document, you can contact us through:

- [Wireless Connectivity Forum](https://community.renesas.com/wireles-connectivity)

- [Contact Technical Support](https://www.renesas.com/eu/en/support?nid=1564826&issue_type=technical)

- [Contact a Sales Representative](https://www.renesas.com/eu/en/buy-sample/locations)

