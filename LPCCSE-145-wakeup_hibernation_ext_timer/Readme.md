# BLE Eddystone beacon with hibernation or deep sleep mode

## Example description

This example configures a DA14531 device to be used as an Eddystone beacon. The beacon advertises with a random static device address for a set amount of time, after which it drops to hibernation mode or deep-sleep mode. While advertising, the device will utilize the extended-sleep mode in between advertising events. Previous advertising data are retained in RAM to be used in subsequent advertising. The system wakes up from the real-time clock in deep-sleep mode, or from an external timer upon a GPIO toggle in hibernation mode.

## HW and SW configuration
- This example runs on the DA14531 Bluetooth Smart SoC devices.	
- The DA145xx Pro Development Kit is needed for this example, and it can be used with the DA14531 daughterboard evaluation kits.

### Hardware configuration for use in deep-sleep mode

- Connect the USB1 connector of the DA145xx Pro Development Kit to the host computer.
- Connect the SPI flash and JTAG debugger with the appropriate jumpers on header J1.
	
	![deep_sleep_board_cfg](assets/deep_sleep_cfg.png)


### Hardware configuration for use in hibernation mode

- Connect the USB1 connector of the DA145xx Pro Development Kit to the host computer.
- Configure your external timer according to the manufacturer's instructions and connect the wake-up signal of your external timer to P25 of header J2. 

	For this example we used Texas Instruments' TPL5010EVM, in which case you would have to connect the WAKE pin of the module to P25, as well as the DONE pin of the module to P24 of header J2 on the motherboard. If you prefer to supply your external timer from the motherboard, you have to connect the V3 pin of J2 header to AUX_VDD, and the ground pin of the timer to any ground pin (marked with a dash) on the motherboard. Trim the variable resistance to your preferred wake-up time interval.

	You can see the jumper configuration below.

	![hibernation_board_cfg](assets/hibernation_cfg.png)

### Software configuration

 This example requires:
 - SDK v6.0.12 or later
 - **SEGGER’s J-Link** tools should be downloaded and installed.
 - Additionaly, an application like BLE Scanner from Bluepixel Technologies should be used to check advertising of our beacon. It can be found on [Google Play](https://play.google.com/store/apps/details?id=com.macdom.ble.blescanner) or on the [App Store](https://apps.apple.com/us/app/ble-scanner-4-0/id1221763603).

## How to run the example

For initial setup of the example please refer to [this section of the dialog support portal](https://www.dialog-semiconductor.com/sites/default/files/sw-example-da145x-example-setup.pdf).

### Initial Setup

 - Start Keil
 - Click on the "Select target" drop-down box and choose the DA14531 target.
	![da14531-target](assets/da14531_target.png)
 - Open the header file ``src/user_hibernation_timer.h`` and define either the ``CFG_HIBERNATION_MODE`` or ``CFG_DEEP_SLEEP_MODE`` guard to use the sleep mode you prefer. Make sure to leave the other one undefined.
 - Compile the example. In the deep-sleep or hibernation mode, we can't have the debugger attached, so we can't run the example in a debug session. 
 
	In deep sleep mode, the DA14531 will initiate a hardware reset on wake-up, and for this reason we would need to load the executable in the SPI flash. You can find more information on how to use the SmartSnippets Toolbox and the SPI Flash Programmer at http://lpccs-docs.dialog-semiconductor.com/SmartSnippetsToolbox5.0.8_UM/index.html.

 	For the hibernation mode, follow the next steps to load your executable and run it without the debugger attached. 

    Start a debugging session.
	![keil-start-debug](assets/keil-start-debug-session.png)

	Stop the debug session. The debugger will detach and your code will run on the DA14531 in the background.
	![keil-stop-debug](assets/keil-stop-debug-session.png)

 ### Inspecting the beacon data
 The following instructions are using the BLE Scanner app.
 - Start the BLE Scanner. The application will list all the Bluetooth devices that are advertising. 
 - Find the Eddystone beacon, advertising with the name ``DLG-HIBER-TIMER``.
 	
	 ![ble-scanner-eddystone-beacon](assets/ble-scanner-eddystone-beacon.jpg)

   You can inspect in the beacon's telemetry data the device's battery level, the simulated temperature, and the advertising counter.

### Configuring the example
The configuration options are local to the ``user_hibernation_timer.c`` file and you can adjust them with the appropriate defines.

- ``PREV_TEMPER_COUNT``: The count of the previous temperatures that will be stored in RAM and will be used by the moving-average filter to compute the next temperature.
- ``PREV_TEMPER_INIT_VALUE``: The temperature that will be used as an initialization value on power-on for the previous stored temperatures. Units are in milli-degrees of Celsius.
- ``PREV_TEMPER_SET_VALUE``: The temperature set value. A random number in the range of +/-500 milli-degrees of Celsius will be added to simulate noise in measurements. Units are in milli-degrees of Celsius.
- ``ADV_TIMER_CANCEL_TIMEOUT``: The duration of advertising. The device will drop to extended-sleep mode in between advertising events.
- ``DONE_TIMER_TIMEOUT``: Used to deassert the DONE signal of the TPL5010 in hibernation mode.
- ``RTC_TIMEOUT``: The time after which the real-time clock will wake the system up from deep-sleep mode.

## Known Limitations


- There are no known limitations for this example. But you can check and refer to the following application note for [known hardware limitations for DA14531 devices](https://www.dialog-semiconductor.com/da14531_HW_Limitation).
- Dialog Software [Forum link](https://www.dialog-semiconductor.com/forum).
- You can also refer to the [DA14531 Getting Started guide](https://www.dialog-semiconductor.com/da14531-getting-started).


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
