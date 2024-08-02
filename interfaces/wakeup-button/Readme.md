# DA14585/DA14586 and DA1453x Multiple button press wake up callback

## Example description

This example shows:
- How to wake up using two possible sources, button SW2 or button SW3.
- How to detect the source, button SW2 or button SW3.

The expected result of the example can be verified by:
- Connecting a serial terminal on the work station to the MB (Mother Board) using UART.
- Status of the LED.
- While awake, the system is advertising (BLE). 

## HW and SW configuration

This example runs on the BLE Smart SoC (System on Chip) devices:
- DA14585/DA14586 or DA1453x daughter board + DA14531 or DA1453x DEVKT-P PRO-Motherboard.
- DA14585/DA14586 daughter board + Basic dev Kit mother board.
	
The user manuals for the development kits can be found:
- [here](https://www.renesas.com/us/en/products/wireless-connectivity/bluetooth-low-energy/da14531-00fxdevkt-p-smartbond-tiny-da14531-bluetooth-low-energy-51-system-chip-development-kit-pro) for the DA14531DEVKT-P PRO-Motherboard.
- [here](https://www.renesas.com/us/en/products/wireless-connectivity/bluetooth-low-energy/da14535-00fxdevkt-p-smartbond-tiny-da14535-bluetooth-low-energy-53-soc-development-kit-pro) for the   DA1453x DEVKT-PPRO-Motherboard.
- [here](https://www.dialog-semiconductor.com/sites/default/files/um-b-048_da14585da14586_getting_started_guide_v2.0_0.pdf) for the Basic Development Kit.

The example is running from SRAM. To run the program from flash please visit chapter 13 of the [SmartSnippets Toolbox User Manual](http://lpccs-docs.dialog-semiconductor.com/SmartSnippetsToolbox5.0.8_UM/tools/SPIFlashProgrammer.html).

* __Hardware configuration DA14531 using DA145xxDEVKT-P PRO-Motherboard__

	- UART TX: connect P21 on J2 to UTX pin 17 on J1 as shown in the image below (the blue line).
	- LED jumper on J8 is configured to P0_9 (red box 2).
	- SW2 (button 2) is configured to P3_1, located on J19 (red box 1)
	- SW3 (button 3) on J19 is configured to P0_7 (P27) on J2, see green line in the image below
	- Connect the DA14531 or DA1453x DEVKT-P PRO-Motherboard to the working station through USB1 connector. 

	![Motherboard_Hardware_Configuration_DA14531](assets/Motherboard_Hardware_Configuration_DA14531.svg)
	![Motherboard_Hardware_Configuration_DA14535](assets/Motherboard_Hardware_Configuration_DA14535.svg)
	
* **Hardware configuration DA14585 using the DA145xxDEVKT-P PRO-Motherboard**

	- UART TX jumper on P0_4, located on J1 (red box 1).
	- LED jumper is configured to P1_0, located on J8 (red box 3).
	- SW2 (button 2) is configured to P0_6, located on J19 (red box 4)
	- SW3 (button 3) is configured to P1_1, located on J19 (red box 2)
	- Connect the DA145xxDEVKT-P PRO-Motherboard to the working station through USB1 connector.

	![Motherboard_Hardware_Configuration_DA14585](assets/Motherboard_Hardware_Configuration_DA14585.svg)

* **Hardware configuration DA14585 using the Basic Development Kit**

	- UART TX/RX jumper on P0_4/P0_5, located on J4
	- LED jumper is configured to P1_0, located on J9
	- An active-low switch should be connected to P1_1, located on J4, (this switch will be SW3) as displayed in the following schematic
	- An active-low switch should be connected to P0_6, located on J4, (this switch will be SW2) as displayed in the following schematic
	
	![connections](assets/connections.svg)

* **Software configuration**

	- This example requires:
	* (optional) SmartSnippets Toolbox v5.0.10
    * [SDK6 latest version](https://www.renesas.com/eu/en/document/swo/sdk601811821-da1453x-da145856)
	* Keil5
	- **SEGGER’s J-Link** tools should be downloaded and installed.

## How to run the example

### Setup

For the initial setup of the project that involves linking the SDK to this SW example, please follow the Readme [here](https://github.com/dialog-semiconductor/BLE_SDK6_examples).

1. Start Keil using the `ble_button_wakeup.uvprojx` Keil project file.
 
2. Expand the dialog shown in the red box in the image below.

![Expand_Select_Device](assets/Expand_Select_Device.png)

3. Select your device: DA14531, DA14586 or DA14585.
		
![Select_Device](assets/Select_Device.png)

4. Open a serial terminal on the work station using for example Tera Term/PuTTY with the following parameters:
```
- baud rate: 115200
- data: 8 bits
- stop: 1 bit
- parity: None
- flow  control: none
```

5. (Optional) open and connect your device in SmartSnippets Toolbox 



## Expected Results

### DA14585/DA14586/DA14531 with DA145xxDEVKT-P PRO-Motherboard
1. The LED controlled in this example is `D5` and the color is `orange`.

2. Have the serial UART monitor active: status messages will be displayed here. See step `4` in __How to run the example__ for setting up the serial monitor.
	- Note that UART messages will only be send to your serial monitor if *CFG_PRINTF_UART2* is defined, the definition can be found in *user_periph_setup.h* 

3. (Optional) initialize and start SmartSnippets toolbox.
	- __Note if SmartSnippets toolbox is used remove the jumper connected to P1_3 on J8__ 
		  
	Image below shows a capture of SmartSnippets
	![DA1453x_SmartSnippets_Capture_wakeup](assets/DA14531_SmartSnippets_Capture_wakeup.png)

4. After launching the example, the system will start advertising. 
	- Note that at start up __no__ message is send via UART.

5. Pressing button SW2 or SW3 will make the system go to sleep.
	- "System going to sleep" is printed on your serial monitor.
	- If the LED was on it will switch off.
	- System will stop advertising.

6. Pressing button SW2:
	- "Wakeup source: SW2" is printed on the serial monitor.
	- The LED is turned on.
	- System will start advertising.

7. See step 5.

8. Pressing button SW3:
	- "Wakeup source: SW3" is printed on the serial monitor.
	- LED will remain off because SW3 is the source of wake up.
	- System will start advertising. 

### DA14585/DA14586 Basic Dev Kit

1. The LED controlled in this example is `USR` and the color is `green`.

2. Identify button:
	- The button connected to P1_1 is SW3.
	- The button connected to P0_6 is SW2.

3. Follow the steps from step 2 in __DA14585/DA14586/DA14531 with DA145xxDEVKT-P PRO-Motherboard__
		
## About this Example

### Configuration

This example can be modified:
- By changing the pin and other configurations in *user_periph_setup.h*.
- By changing configurations in *user_wakeup.h*
- By changing the configurations in one or more of the **user_config** files.

Configuration of the wake up callback (`wkupct_enable_irq`):
- sel_pins: using `WKUPCT_PIN_SELECT` and an or operator SW2 and SW3 is set as wake up callback.
- pol_pins: using `WKUPCT_PIN_POLARITY`, an or operator and WKUPCT_PIN_POLARITY_LOW SW2 and SW3 is set to wake up the system at button press.
	- Button press is equal to polarity low because an internal pull up is used.
- events_num: is set to 1 event (button press) to wake up the system.
- deb_time: is set to 30 to prevent button debounce to trigger interrupt/callback.

Configure the callback function:
- With `wkupct_register_callback` a wake up callback function is set, in this case `user_app_wakeup_press_cb`
 
### How the system goes to sleep

1. `GPIO_SetInactive(LED_PORT, LED_PIN)` sets the LED inactive.

2. `arch_set_sleep_mode(ARCH_EXT_SLEEP_ON)` this function will change the default sleep state to ARCH_EXT_SLEEP_ON.

3. `arch_ble_ext_wakeup_on()` this function prevents the BLE core from waking up, the system can only wake up from an external source, a button press in this example.

### How the system wakes up

1. `arch_set_sleep_mode(ARCH_SLEEP_OFF)` this function will change the default sleep state to ARCH_SLEEP_OFF.

2. `arch_ble_force_wakeup()` this function will wake up the BLE core.

3. `arch_ble_ext_wakeup_off()` this function will disable external wake up, the BLE core can wake up the system again.

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

