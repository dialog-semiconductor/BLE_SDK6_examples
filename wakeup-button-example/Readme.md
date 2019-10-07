
# DA14585/DA14586 and DA14531 Button Press with Wakeup from Sleep

---


## Example description

This SDK6 example shows how to configure
multiple buttons that will wakeup the system from
sleep. In the wakeup handler it will detect the source
of the wakeup. When button SW2 is the source of the
wakeup, the LED P1_0 will turn on. Alternatively, when 
button SW3 is the source of the wakeup, the LED will 
remain off. In both cases a message will 
be send to the serial terminal (i.e. Tera Term, PuTTy or any 
other of your own preference) displaying the wakeup source.

## HW and SW configuration

This example runs on The DA14585 and DA14531 Bluetooth Smart SoC devices.
The GPIO can be configured in *user_periph_setup.h*.

* **Hardware configuration DA14585 and DA14586**
	- Jumper configuration (see red boxes in image below): 
		- SWD
		- UART: P0_4 to Tx pin P0_5
		- LED: P1_0 to LED
		- SW2: P0_6 to SW2 
		- SW3: P1_1 to SW3 

![connections DA14585](assets/DA14585_jumper_configuration.PNG)

* **Hardware configuration DA14531**
	- Jumper configuration (see red boxes in the image below): 
		- SWD
		- LED: P1_0 to LED, P1_0 is for the DA14531 connected to P0_9 that is why the LED is controlled in the software with port 0 and pin 9

	- Jumper wire configuration (see green, blue and purple lines in the image below):
		- UART: P0_6 (P26) to Tx pin P0_5 (see purple line) 
		- SW2: P0_7 (P27) to SW2 (see green line) 
		- SW3: P0_8 (P28) to SW3 (see blue line) 

![connections DA14531](assets/DA14531_jumper(wire)_configuration.PNG)

* **Software configuration**

	- This example requires:
    * SDK6.0.11
	- **SEGGER’s J-Link** tools should be downloaded and installed.


## How to run the example

For initial setup of the example please refer to [this section of the dialog support portal](https://support.dialog-semiconductor.com/resource/da1458x-example-setup). 
The example is running from SRAM. To run the program from flash refer to chapter 11 of the [SmartSnippets Toolbox User Manual](https://support.dialog-semiconductor.com/resource/um-b-083-smartsnippets-toolbox-user-manual).

### Initial Setup

 - Start Keil
 - (optional) See power usage, good to see the difference between advertising and sleeping.  
 - Compile and launch the example
 - Open the development kit serial port with the following parameters

		- baudrate: 115200
		- data: 8 bits
		- stop: 1 bit
		- parity: None
		- flow  control: none

Image below shows a capture of SmartSnippets
![SmartSnippets DA14531](assets/DA14531_SmartSnippets_Capture_wakeup.PNG)
		
## About this Example
This example was built on top of the Barebone project from SDK6. The code can be found in *user_wakeup_example.c*.
The function, to initialize the parameters of the example, **user_wakeup_example_init(void)**, is called from the **user_app_init(void)** in *the user_barebone.c* file.
In the *da1458x_config_basic.h* file the SLEEP_WITHOUT_ADVERTISING flag is undefined by default. When sleep without advertising is required, defining this flag will ensure this.
Mind that *da1458x_config_basic.h* has a section for DA14531 and a shared section for DA14585 and DA14586. 

To setup the wakeup interrupt, wkupct_enable_irq(uint32_t sel_pins, uint32_t pol_pins, uint16_t events_num, uint16_t deb_time) from the wkupct_quadec driver is used with the following paramters:

 - 	sel_pins: (WKUPCT_PIN_SELECT(GPIO_BUTTON2_PORT, GPIO_BUTTON2_PIN) | WKUPCT_PIN_SELECT(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN)). WKUPCT_PIN_SELECT uses the GPIO Port and GPIO Pin to create the correct
	bitmask for setting the wakeup interrupt in the corresponding register(s). If more than one interrupt source is to be set, as is the case here, use an OR operation to combine all the bitmasks.
 - 	pol_pins: (WKUPCT_PIN_POLARITY(GPIO_BUTTON2_PORT, GPIO_BUTTON2_PIN, 1) | WKUPCT_PIN_POLARITY(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, 1)). WKUPCT_PIN_POLARITY uses the GPIO Port, GPIO Pin and polarity parameter to create the correct
	bitmask for setting the polarity of the wakeup source in the corresponding register(s). If more than one interrupt source is to be set, as is the case here, use an OR operation to combine all the bitmasks.
 - 	events_num: (1). The number of wakeup events before an interrupt is triggered ranging from 0 - 255.
 - 	deb_time: (30). The debounce time in miliseconds ranging from 0 - 63 (0x00 - 0x3F).
 
 The wakeup callback is set using the wkupct_register_callback(wakeup_handler_function_t callback) function from the wkupct_quadec driver. In this case it is set to the app_wakeup_press_cb(void).
 
 In the app_wakeup_press_cb(void) the following is done:
 * When the device isn't sleeping, put the device to sleep by calling the following functions:
 
	- app_easy_gap_advertise_stop(); This function will stop the advertising. (Function is only called when the SLEEP_WITHOUT_ADVERTISING is defined)
	- arch_set_sleep_mode(ARCH_EXT_SLEEP_ON); This function will change the default sleepstate.	
	- arch_ble_ext_wakeup_on(); This function prevents the BLE core from waking up the system. Only an external event (i.e. buttonpress) can wake up the system.
	
* When the device is sleeping, wakeup the device by calling the following functions:

	- arch_set_sleep_mode(ARCH_SLEEP_OFF); This will change the default sleepstate.
    	- arch_ble_force_wakeup(); This will wakeup the BLE core when only external event can wake up the system. 
    	- arch_ble_ext_wakeup_off(); This will enable the BLE core to wakeup the system from sleep.
	- Furthermore, the source of the interrupt will be checked and displayed accordingly. 
	
With the wakeup interrupt there isn't a good way do differentiate between level detection and edge detection 
as is the case with normal GPIO interrupts by setting the correct value in the GPIO_INT_LEVEL_CTRL_REG. 
For the wakeup interrupt there is no similiar register to achieve this result.

## Known Limitations


- There are No known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations](https://support.dialog-semiconductor.com/system/files/resources/DA1458x-KnownLimitations_2018_02_06.pdf "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums).
- you can Refer also for the Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.


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
