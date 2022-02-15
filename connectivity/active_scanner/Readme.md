-----
# DA14531 and DA14585/586 Active scanner
-----

## Example description

The device is setup in active scanning mode. On advertising data report, the data is formatted and pushed on the UART. 
The user can read the peripheral advertising data in a friendly way on its serial terminal.

## HW and SW configuration

* **Hardware configuration**

    - This example runs on the DA14585, DA14586 and DA14531 Bluetooth Smart SoC devices.
    - DA14585/DA14586 or DA14531 daughter board + DA145xxDEVKT-P PRO-Motherboard are required.

* **Software configuration**

    - This example requires:
        * [SDK6.0.14](https://www.dialog-semiconductor.com/da14531_sdk_latest).
        * SEGGER’s J-Link tools should be downloaded and installed.

For the UART to see the communication logs, check the setup shown below for DA14531 and DA14585/586

### Hardware configuration for the DA14531

![DA14531 Jumper setting](assets/da14531.jpg)

### Hardware configuration for the DA14585/586

 
![DA14585 Jumper setting](assets/da14585.jpg)


The user manuals for the development kits can be found:

- [Here](https://www.dialog-semiconductor.com/products/da14531-development-kit-pro) for the DA145xxDEVKT-P PRO-Motherboard.


## How to run the example

For the initial setup of the project that involves linking the SDK to this SW example, please follow the Readme [here](../../Readme.md).

### Initial Setup

 - Start Keil
 - Compile and launch the example
 - Open the development kit serial port with the following parameters

		- baudrate: 115200
		- data: 8 bits
		- stop: 1 bit
		- parity: None
		- flow  control: none

The device should display the advertising data as shown in the picture below

![Scan_results](assets/active_scanner.png)


## Known Limitations

- There are No known limitations for this example. But you can check and refer to: 

	- [SDK6 Known Limitations](http://lpccs-docs.dialog-semiconductor.com/sdk6_kll/index.html).
	- [known hardware limitations for DA14531 devices](https://www.dialog-semiconductor.com/da14531_HW_Limitation).
	- [DA14531 Getting Started guide](https://www.dialog-semiconductor.com/da14531-getting-started).
	- [DA14585 hardware limitations](https://www.dialog-semiconductor.com/sites/default/files/da1458x-knownlimitations_2019_01_07.pdf)
	- [DA14585/DA14586 Getting Started with the Development Kit UM-B-049](http://lpccs-docs.dialog-semiconductor.com/da14585_getting_started/index.html).
	- [Dialog BLE Support Forum](https://www.dialog-semiconductor.com/BLE_Support).


