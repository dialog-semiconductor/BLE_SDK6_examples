
# DA14585/DA14586 Active scanner

---


## Example description

The device is setup in active scanning mode. On advertising data report, the
data is formatted and pushed on the UART. The user can read the peripheral
advertising data in a friendly way on its serial terminal.

## HW and SW configuration


* **Hardware configuration**

	- This example runs on The DA14585/DA14586 Bluetooth Smart SoC devices.
	- A Basic or Pro Development kit is needed for this example.
	- Connect the USB Development kit to the host computer.

* **Software configuration**

	- This example requires:
       * Smartsnippets Studio 1.6.3.
       * SDK6.0.10
	- **SEGGER’s J-Link** tools should be downloaded and installed.
	- Python 3.5 or greater


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

![Scan_results](assets/markdown-img-paste-20180917114832644.png)



## Known Limitations


- There are No known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations](https://support.dialog-semiconductor.com/system/files/resources/DA1458x-KnownLimitations_2018_02_06.pdf "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums).
- you can Refer also for the Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.

