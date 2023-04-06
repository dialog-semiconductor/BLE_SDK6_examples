# ble_burst_adv

Burst Advertising with Bluetooth LE

## Example description

Simple example showing how to implement 'burst' advertising on the DA14531 and DA14585/6 devices. Burst 
advertising is the process of sending a number of advertising events and then having 
a delay before sending another group of advertising events. This example allows the 
user to configure the number of advertising events to be transmitted per burst and 
the time between bursts. 
	
## HW and SW configuration

- This example runs on the DA14585, DA14586 and DA14531 Bluetooth Smart SoC devices.
- DA14585/DA14586 or DA14531 daughter board + DA145xxDEVKT-P PRO-Motherboard are required.

For the UART to see the communication logs, check the setup shown below for DA14531 and DA14585/586

### Hardware configuration for the DA14531

![DA14531 Jumper setting](assets/da14531.svg)

### Hardware configuration for the DA14585/586

 
![DA14585 Jumper setting](assets/da14585.svg)


The user manuals for the development kits can be found:

- [Here](https://www.dialog-semiconductor.com/products/da14531-development-kit-pro) for the DA145xxDEVKT-P PRO-Motherboard.

	
### Software configuration

- This example requires:
    * Smartsnippets Studio 2.0.16 (or later)
    * [SDK6.0.14](https://www.dialog-semiconductor.com/da14531_sdk_latest).

## How to run the example

For the initial setup of the project that involves linking the SDK to this SW example, please follow the Readme [here](../../Readme.md).

### Initial Setup

1.  Build and download the example using the Keil IDE. 
2.  Run the example using the Keil debugger.
3.  Observe bursts of advertising events using Power Profiler found within the SmartSnippets Toolbox, monitor UART debug output using a terminal such as Teraterm etc as following:
		   
  		    - baudrate: 115200
		    - data: 8 bits
		    - stop: 1 bit
		    - parity: None
		    - flow control: none
				
4. Undirected connecatable advertising is used for this demonstration. Monitor transmitted advertising events using a Smart Phone App such as LightBlue.

The device should display the advertising data as shown in the picture below:

![Output](assets/output.jpg)

## Known Limitations


- There are no known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations for DA1458x devices](https://www.dialog-semiconductor.com/sites/default/files/da1458x-knownlimitations_2019_01_07.pdf) or [known hardware limitations for DA14531 devices](https://www.dialog-semiconductor.com/da14531_HW_Limitation).
- Dialog Software [Forum link](https://www.dialog-semiconductor.com/forum).
- you can also refer to the [DA14585/DA14586 Getting Started Guide with the PRO-Development Kit](http://lpccs-docs.dialog-semiconductor.com/da14585_getting_started/index.html) or the [DA14531 Getting Started guide](https://www.dialog-semiconductor.com/da14531-getting-started).

