
# DA14531 and DA14585/586 Active scanner

---


## Example description

The device is setup in active scanning mode. On advertising data report, the data is formatted and pushed on the UART. 
The user can read the peripheral advertising data in a friendly way on its serial terminal.

## HW and SW configuration

* **Hardware configuration**

    - This example runs on the DA14585, DA14586 and DA14531 Bluetooth Smart SoC devices.
    - DA14585/DA14586 or DA14531 daughter board + DA145xxDEVKT-P PRO-Motherboard are required.

* **Software configuration**

    - This example requires:
        * [SDK6 latest version](https://www.dialog-semiconductor.com/da14531_sdk_latest).
        * SEGGER’s J-Link tools should be downloaded and installed.

For the UART to see the communication logs, check the setup shown below for DA14531 and DA14585/586

# Hardware configuration for the DA14531:

![DA14531 Jumper setting](assets/da14531.svg)

# Hardware configuration for the DA14585/586:

 
![DA14585 Jumper setting ](assets/da14585.svg width="256" height="256" )


The user manuals for the development kits can be found:

- [Here](http://lpccs-docs.renesas.com/UM-B-117-DA14531-Getting-Started-With-The-Pro-Development-Kit/02_Introduction/Introduction.html) for the DA145xxDEVKT-P PRO-Motherboard.


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


- There are no known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations for DA1458x devices](https://www.dialog-semiconductor.com/sites/default/files/da1458x-knownlimitations_2019_01_07.pdf) or [known hardware limitations for DA14531 devices](https://www.dialog-semiconductor.com/da14531_HW_Limitation).
- Dialog Software [Forum link](https://www.dialog-semiconductor.com/forum).
- you can also refer to the [DA14585/DA14586 Getting Started Guide with the PRO-Development Kit](http://lpccs-docs.dialog-semiconductor.com/da14585_getting_started/index.html) or the [DA14531 Getting Started guide](https://www.dialog-semiconductor.com/da14531-getting-started).

