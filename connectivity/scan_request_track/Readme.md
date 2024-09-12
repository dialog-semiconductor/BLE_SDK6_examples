# DA1453x/DA1458x Scan Request Track

## Example description

This SDK6 DA14585 / DA14531 example demonstrates how a peripheral device can track if it is scanned and which central device 
performs the scanning procedure.
The central device needs to scan in active mode since the peripheral device will track if it is scanned via
the scan request from central.
The example doesn't require any change in the SDK 6.0.22 code as in previous SDK releases, the scan request is tracked in every BLE END event.

- Devices naming:
    - DA1453x is refering to DA14531-00, DA14531-01, DA14530 and DA14535.
    - DA1458x is refering to DA14585 and DA14586.
    - The DA14531-00 is the main DA14531 device. The -00 is just a new naming to introduce the variant DA14531-01. The DA14531-01 is a ROM variant of the main DA14531-00.
    - The DA14535 is a DA14531 upgrade.
	
## HW and SW configuration


* **Hardware configuration**

	- This example runs on The DA1453x/DA1458x Bluetooth Smart SoC devices.
	- The Basic / Pro Development kit is needed for this example.
	- Connect the DA145xx Pro Development Kit to the host computer.
	- UART TX on P0_4 for DA14585/DA14586 (place jumper between J1:17 and J1:18)
		![DA14585_connection_wires](assets/DA14585_connection_wires.svg)
	
	- UART TX on P0_6 for DA14531 (Place wire between J1:17 and J2:27)
		![DA14531_connection_wires](assets/DA14531_connection_wires.svg)

	- UART TX on P0_6 for DA14535 (Place wire between J1:17 and J2:27)
		![DA14535_connection_wires](assets/DA14535_connection_wires.svg)

	- For the cursor to operate on DA14531 on the Pro Development kit a fly wire is needed 
	connecting J2:24 with J8.2 pin (default jumper on J8 should be removed).
	- For the cursor to operate on DA14585 on the Pro Development kit a jumper is needed connecting J8:1 with J8.2 pin (default jumper connection).

- For the DA14531 getting started guide you can refer to [UM-B-117](https://lpccs-docs.renesas.com/UM-B-117-DA14531-Getting-Started-With-The-Pro-Development-Kit/index.html)

- For the DA14535 getting started guide you can refer to this [UM-B-165](https://lpccs-docs.renesas.com/DA14535/UM-B-165-DA14531-Getting-Started-With-The-Pro-Development-Kit/index.html#device-family-getting-started-with-the-pro-development-kits)

- For the DA14531 Module getting started guide you can refer to this [UM-B-139](https://lpccs-docs.renesas.com/UM-B-139-Getting-Started-with-DA14531-TINY-Module/index.html)
- For the DA14585/586 getting started guide you can refer to this [UM-B-049](https://lpccs-docs.renesas.com/da14585_getting_started/index.html).
* **Software configuration**

	- This example requires:
    * Smartsnippets Toolbox 5.0.24 or later.
    * [SDK6 latest version](https://www.renesas.com/sdk6_latest).
	- **SEGGER’s J-Link** tools should be downloaded and installed.
	- **A simple serial terminal** should be installed on the PC


## How to run the example

For the initial setup of the project that involves linking the SDK to this SW example, please follow the Readme [here](https://github.com/dialog-semiconductor/BLE_SDK6_examples).

### Initial Setup

 - Compile and launch the example
 - Open the development kit serial port with the following parameters

		- baudrate: 115200
		- data: 8 bits
		- stop: 1 bit
		- parity: None
		- flow  control: none
 - If the device is actively scanned the FW will print the BD address that scanned the device.
 
	![scan_requests_print](assets/scan_requests_print.png)
 - In the case of a scan request, a cursor will also appear in the power profiler of Smart Snippets.

	![scan_request_cursor](assets/scan_request_cursor.png)


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

