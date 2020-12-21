## Example Description

Purpose of the current example is to demonstrate the WiFi coexistance feature available in the SDK 6.0.14.1114. The example will provide guidelines on how the feature can be enabled on the SDK as well as the signal behaviour that should be expected on the configured pins under the various states and priority configuration of the device. 

The WLAN coexistence feature on DA14585/586/531 is designed to allow multiple 2.4 GHz devices to operate without signals from one radio interfering with adjacent radios. This is done by a handshaking protocol between the BLE device and the other 2.4 GHz device using the following signals:

 - **WLAN_COEX_BLE_EIP**: Output signal of the device. The DA14585/586/531 can communicate its radio status in advance using the above mentioned signal. WLAN_COEX_BLE_EIP is an envelop of the scheduled TX and RX radio activity of the BLE Radio and it starts before the actual radio activity so it can upfront inform the coexisting system of scheduled RF activity. The signal is de-asserted synchronously directly after radio activity is finished.
 - **WLAN_COEX_BLE_PRIO**: Output signal of the device. The DA14585/586/531 can enable a priority line to indicate that it can not be interrupted for a defined activity. The activity for which BLE needs priority can be defined through the an SDK API. The API which controls the signal enables the application to assign a priority to certain packet types on specific connections. The user should define the type of the packet and the conditions via the **wlan_coex_prio_criteria_add(packet_type, connection, num_of_packets)** API.
 - **WLAN_COEX_24G_EIP**: Input signal of the device. External 2.4GHz device event in progress indication. Asserting the pin will force the BLE device to supress its radio activity if no higher priority for the upcomming packet is defined.
 - **DEBUG A**: Debugging output signal of the device. Asserted when the BLE radio is overrulled from 24G external request.
 - **DEBUG B**: Debugging output signal of the device. Asserted when a 24G request arrives.

## HW and SW Configuration
Example requirements:
- A DA14531 or DA14585/586 Smartbond daughterboard evaluation kit.
- The DA145xx Pro Development Kit is proposed for this example since it provides the proper headers for attaching a logic analyzer for signal observation.
- A logic analyzer for monitoring the signals.

### Hardware Configuration for the DA14585/586 and DA14531 daughter board evaluation kit

- The USB1 connector of the DA145xx Pro Development Kit should be connected to the host computer
- The jumpers should be connected as shown in the graphic for downloading the binary to the RAM via JTAG (using Keil or Smart Snippets Toolbox). For downloading the fw via UART using the P05 single wire UART configuration (only via the Smart Snippets Toolbox) alternative jumper configuration should be applied, see [AN-B-072 DA14531 Booting from OTP and Serial Interfaces](https://www.dialog-semiconductor.com/sites/default/files/an-b-072-da14531_booting_options_v1.4.pdf).
- The current example traces the signals on the assigned COEX pins. By default the signals are exposed on specific pins thus the probes of the analyzer should be attached on the following pins:
	- WLAN_COEX_24G_EIP exposed on P05 (for DA14531) or P00 (for DA14585/586)
	- WLAN_COEX_BLE_EIP exposed on P06 (for DA14531) or P03 (for DA14585/586)
	- WLAN_COEX_BLE_PRIO exposed on P07 (for DA14531) or P02 (for DA14585/586)
	- DEBUG A exposed on P09 (for DA14531) or P01 (for DA14585/586)
	- DEBUG B exposed on P08 (for DA14531) or P13 (for DA14585/586)
- In order to emulate an external device requesting radio transmition a simple button can be used connected on pin J2:1 (V3) of the motherboard and on WLAN_COEX_24G_EIP pin (default at P05 for DA14531 or P00 for DA14585/586).

<figure>
<img src="media/DevKit531.png" alt="motherboard_with_DA14531_DB_eval_kit">
	<figcaption>Fig. 1: DA14531 Daughter Board along with Pro Dev Kit</figcaption>

<img src="media/DevKit585_586.png" alt="motherboard_with_DA14585/586_DB_eval_kit">
	<figcaption>Fig. 2: DA14585/586 Daughter Board along with Pro Dev Kit</figcaption>
</figure>

### Software Configuration

 Example requirements:
- SDK v6.0.14 or later
- **SEGGER’s J-Link** tools downloaded and installed
- Logic Analyzer software.

## Enable the COEX feature in a project

The following guidelines enable the COEX feature on the template example located in the default SDK.
1. Open the template example located under the following directory **C:\sdk_dir\DA145xx_SDK\6.0.14.1114\projects\target_apps\template\empty_peripheral_template\Keil_5\empty_peripheral_template.uvprojx**.
2. The wlan_coex.c file contains the APIs of the COEX feature, thus it should be added in the project. For adding the source file right click on the sdk_driver in Keil's project tree and select the "Add existing files to Group 'sdk_driver'. In the explorer window navigate into the following directory **sdk_root\sdk\platform\driver\wifi** select the **wlan_coex.c** file and click add in the explorer to include the file into the project.
3. To include the corresponding header file of the API you will have to select the **options for target** button and navigate to the **C/C++** tab. Select the **Include paths** and add the header file **wlan_coex.h** from the following directory **sdk_root\sdk\platform\driver\wifi**.

<figure>
<img src="media/include_header_file.png" alt="wlan_coex.h_include_header_file">
	<figcaption>Fig. 3: Add wlan_coex.h in Keil project</figcaption>
</figure>

4. In da1458x_config_advanced.h, at the corresponding sections for DA14531 or DA14585/586, use the following preprocessor definitions in order to enable WLAN coexistence mode with or without debugging signals and set the prefered polarity of the BLE event in progress signal. The CFG_WLAN_COEX_BLE_EVENT_INV is only used in case the application requires the signal **WLAN_COEX_BLE_EIP** inverted (de-asserted high, asserted low).
```c
/****************************************************************************************************************/
/* WLAN coexistence mode: Enable/disable the mode.                                                              */
/****************************************************************************************************************/
#define CFG_COEX

/****************************************************************************************************************/
/*  WLAN coexistence mode: Enable/disable the debugging signals.                                                */
/****************************************************************************************************************/
#define CFG_WLAN_COEX_DEBUG

/****************************************************************************************************************/
/* WLAN coexistence mode: Controls the polarity of the BLE event in progress signal.                            */
/* - defined: BLE event in progress signal is active low.                                                       */
/* - undefined: BLE event in progress signal is active high.                                                    */
/****************************************************************************************************************/
#undef CFG_WLAN_COEX_BLE_EVENT_INV
```
5. In the **user_periph_setup.h** file the following block of definitions should be added for WLAN coexistence pin mapping. All COEX signals but the **WLAN_COEX_BLE_EIP** can be assigned to any pins. The **WLAN_COEX_BLE_EIP** signal is an internal HW signal and it can be only mapped to specific pins. For both DA14531 and DA14585/586 the WLAN_COEX_BLE_EIP can be mapped to pins from P00 to P07.
```c
/****************************************************************************************/
/* WLAN COEX pin configuration                                                          */
/****************************************************************************************/

#if (WLAN_COEX_ENABLED)
#if defined (__DA14531__)

   /// Input signal to device: 2.4GHz external device event in progress indication.
   #define WLAN_COEX_24G_EIP_PORT      GPIO_PORT_0
   #define WLAN_COEX_24G_EIP_PIN       GPIO_PIN_5

   /// Output signal from device: BLE event in progress indication.
   #define WLAN_COEX_BLE_EIP_PORT      GPIO_PORT_0
   #define WLAN_COEX_BLE_EIP_PIN       GPIO_PIN_6

   /// Output signal from device: BLE priority indication.
   #define WLAN_COEX_BLE_PRIO_PORT     GPIO_PORT_0
   #define WLAN_COEX_BLE_PRIO_PIN      GPIO_PIN_7

#if defined (CFG_WLAN_COEX_DEBUG)
   /// BLE radio overruled signal pin definition.
   /// This signal goes high when the BLE radio is forced to be off due to external 2.4GHz device activity.
   #define WLAN_COEX_DEBUG_A_PORT      GPIO_PORT_0
   #define WLAN_COEX_DEBUG_A_PIN       GPIO_PIN_9

   /// External 2.4GHz device EIP handler signal pin definition.
   /// This signal indicates when an external 2.4GHz device wants to start or stop sending data.
   #define WLAN_COEX_DEBUG_B_PORT      GPIO_PORT_0
   #define WLAN_COEX_DEBUG_B_PIN       GPIO_PIN_8
#endif

#else

   /// Input signal to device: 2.4GHz external device event in progress indication.
   #define WLAN_COEX_24G_EIP_PORT      GPIO_PORT_0
   #define WLAN_COEX_24G_EIP_PIN       GPIO_PIN_0

   /// Output signal from device: BLE event in progress indication.
   #define WLAN_COEX_BLE_EIP_PORT      GPIO_PORT_0
   #define WLAN_COEX_BLE_EIP_PIN       GPIO_PIN_3

   /// Output signal from device: BLE priority indication.
   #define WLAN_COEX_BLE_PRIO_PORT     GPIO_PORT_0
   #define WLAN_COEX_BLE_PRIO_PIN      GPIO_PIN_2

#if defined (CFG_WLAN_COEX_DEBUG)
   /// BLE radio overruled signal pin definition.
   //  This signal goes high when the BLE radio is forced to be off due to external 2.4GHz device activity.
   #define WLAN_COEX_DEBUG_A_PORT      GPIO_PORT_0
   #define WLAN_COEX_DEBUG_A_PIN       GPIO_PIN_1

   /// External 2.4GHz device EIP handler signal pin definition.
   /// This signal indicates when an external 2.4GHz device wants to start or stop sending data.
   #define WLAN_COEX_DEBUG_B_PORT      GPIO_PORT_1
   #define WLAN_COEX_DEBUG_B_PIN       GPIO_PIN_3
#endif

#endif

// GPIO IRQ number. Interrupt is fired when 2.4GHz external device event in progress signal is activated.
#define WLAN_COEX_IRQ            4

#endif // WLAN_COEX_ENABLED
```
6. In user_periph_setup.c we should apply the following changes in order to enable properly WLAN coexistence feature.
	- Add the required include file. Include wlan_coex.h in the Include Files section.
	```c
	#if (WLAN_COEX_ENABLED)
	#include "wlan_coex.h"
	#endif
	```
	- Add the following global variable definition (configuration struct for WLAN coexistence).
	```c
	#if (WLAN_COEX_ENABLED)
	// Configuration struct for WLAN coexistence
	const wlan_coex_cfg_t wlan_coex_cfg = {
	.ext_24g_eip_port = WLAN_COEX_24G_EIP_PORT,
	.ext_24g_eip_pin = WLAN_COEX_24G_EIP_PIN,
	.ble_eip_port = WLAN_COEX_BLE_EIP_PORT,
	.ble_eip_pin = WLAN_COEX_BLE_EIP_PIN,
	.ble_prio_port = WLAN_COEX_BLE_PRIO_PORT,
	.ble_prio_pin = WLAN_COEX_BLE_PRIO_PIN,
	#if defined (CFG_WLAN_COEX_DEBUG)
	.debug_a_port = WLAN_COEX_DEBUG_A_PORT,
	.debug_a_pin = WLAN_COEX_DEBUG_A_PIN,
	.debug_b_port = WLAN_COEX_DEBUG_B_PORT,
	.debug_b_pin = WLAN_COEX_DEBUG_B_PIN,
	#endif
	.irq = WLAN_COEX_IRQ,
	};
	#endif
	```
	- Reserve GPIOs used by the module by adding the following block of code inside GPIO_reservations() function.
	
	```c
	#if (WLAN_COEX_ENABLED)
    RESERVE_GPIO(COEX_EIP, wlan_coex_cfg.ble_eip_port, wlan_coex_cfg.ble_eip_pin, PID_GPIO);
    RESERVE_GPIO(COEX_PRIO, wlan_coex_cfg.ble_prio_port, wlan_coex_cfg.ble_prio_pin, PID_GPIO);
    RESERVE_GPIO(COEX_REQ, wlan_coex_cfg.ext_24g_eip_port, wlan_coex_cfg.ext_24g_eip_pin, PID_GPIO);
	#endif

	#if defined (CFG_WLAN_COEX_DEBUG)
	RESERVE_GPIO(DEBUGPIN1, wlan_coex_cfg.debug_b_port, wlan_coex_cfg.debug_b_pin, PID_GPIO);
	RESERVE_GPIO(DEBUGPIN2, wlan_coex_cfg.debug_a_port, wlan_coex_cfg.debug_a_pin, PID_GPIO);
	#endif
	```
	- Call wlan_coex_gpio_cfg() function at the end of set_pad_functions() to initialize GPIOs used by the module and to configure 2.4GHz external device event in progress signal to act as a GPIO interrupt.
	```c
	#if (WLAN_COEX_ENABLED)
   	wlan_coex_gpio_cfg();
	#endif
	```
7. Now the COEX feature is enabled in the project. A final step is required in order to access the API's from application level. The following modifications are required in the user_empty_peripheral_template.c file or the corresponding user file that holds the user application.
	- Add the required include files, wlan_coex.h and lld.h in the Include Files section.
	```c
	#if (WLAN_COEX_ENABLED)
	#include "wlan_coex.h"
	#include "lld.h"
	#endif
	```
	- Invoke through the .app_on_init callback, in this example's case the user_app_on_init() function, the following functions in order to initialize the COEX functionality and assign the desired rules for the coexistance. The ``` wlan_coex_prio_criteria_add(WLAN_COEX_BLE_PRIO_ADV, LLD_ADV_HDL, 0);``` rule specifies that priority over the 24G signal have the BLE advertising packets, the LLD_ADV_HDL indicates that the specific packet is not associated with a connection and the last parameter is only used when priority is assigned for data packets.
	```c
	#if (WLAN_COEX_ENABLED)
   	wlan_coex_init();

   	// Adds priority case for a specific connection
   	wlan_coex_prio_criteria_add(WLAN_COEX_BLE_PRIO_ADV, LLD_ADV_HDL, 0);
	#endif
	```
	- Invoke through the .app_validate_sleep, in our case is the user_app_on_validate_sleep() function the following function in order to prepare the COEX for going to sleep.
	```c
	#if (WLAN_COEX_ENABLED)
   	wlan_coex_going_to_sleep();
	#endif
	```
	
<figure>
<img src="media/user_empty_peripheral_template.png" alt="user_empty_peripheral_template.c_modifications">
	<figcaption>Fig. 4: Modifications in user_empty_peripheral template</figcaption>
</figure>

## Run the Example

The example is delivered with the latest SDK6 release. The project is located under the following directory: ``DA145xx_SDK\6.0.14.1114\projects\target_apps\Coexistance_example``

### Initial Setup

1. Start Keil.
2. Compile the example by pressing the **Build** button. 
3. As soon as the build is complete download the fw directly to RAM via Keil using JTAG or via the Smart Snippets Toolbox on a DA14531 or a DA14585/586 daughtercard via JTAG or UART.
4. Start the Logic Analyzer software for probing the COEX signals as the fw executes.
 
### WLAN COEX API

As allready mentioned the COEX API allows the application to set certain rules regarding what type of packets will have priority over 24G packets. If there are no rules defined from application level the 24G device will always take priority over the BLE device.
The function that sets the rules for the BLE priorities is the ``` void wlan_coex_prio_criteria_add(wlan_coex_ble_prio_t priority, uint16_t conhdl, uint16_t missed)``` function. Its parameters specify:
- **priority**: type of packet that has priority over the 24G device. Available values for the specific parameter are indicated at the Table 1.
- **conhdl**: the connection that is associated with the packet that will have priority over the 24G packet.
- **missed**: if "missed" packets are lost during a connection, the next BLE packet will be prioritized over the next 24G.
<p>
<table><caption>Table 1</caption>
	<tr><th>Rule</th> <th>Command</th> <th>Priority Type</th><th>conhdl</th><th>missed</th></tr>
	<tr><th>1</th> <th>BLEMPRIO_SCAN</th> <th>Active scan</th><th>always set to LLD_ADV_HDL</th><th>ignored</th></tr>
	<tr><th>2</th> <th>BLEMPRIO_ADV</th> <th>Advertise</th><th>always set to LLD_ADV_HDL</th><th>ignored</th></tr>
	<tr><th>3</th> <th>BLEMPRIO_CONREQ</th> <th>Connection request</th><th>always set to LLD_ADV_HDL</th><th>ignored</th></tr> 
	<tr><th>4</th> <th>BLEMPRIO_LLCP</th> <th>Control packet</th><th>handle of the corresponding connection</th><th>ignored</th></tr>
	<tr><th>5</th> <th>BLEMPRIO_DATA</th> <th>Data packet</th><th>handle of the corresponding connection</th><th>ignored</th></tr>
	<tr><th>6</th> <th>BLEMPRIO_MISSED</th> <th>Missed events</th><th>handle of the corresponding connection</th><th>amount of missed BLE events</th></tr>
</table>
</p>

### Monitor the signals through the Logic Analyzer

At this point you should have the executable running on the DA14531 and the Logic Analyzer connected to the proper pins while the analyzer software runs. Depending on the packet rules applied the below traces indicate the behaviour of the signals.

1. #### Priority on BLE advertising packets.
The current trace demonstrates the state of the signals when BLE advertising packets are prioritized over 24G requests.
In the ```.app_on_init callback``` the ```wlan_coex_prio_criteria_add(WLAN_COEX_BLE_PRIO_ADV, LLD_ADV_HDL, 0);``` function is applied to set the rule for prioritizing advertising packets.

<figure>
<img src="media/Advertising_prio_with24G_activity.png" alt="Advertising_prio_with24G_activity">
	<figcaption>Fig. 5: Priority on BLE advertising events along 24G activity</figcaption>
</figure>

Since we have applied the rule for prioritizing BLE advertising events and asserted the **24G EIP** (emulating a request from the 24G device) the **BLE PRIO** pin is asserted to indicate to the 24G not to transmit since an advertsing BLE event is ongoing (the **BLE PRIO** signal will be asserted regardless if the **24G_EIP** is asserted or not). The **BLE PRIO** signal remaines asserted as long as the Radio events are ongoing and de-asserts as soon as the BLE device finishes with the radio events. The **BLE EIP** signal always asserts during a radio event regardless the rules appied and envelopes the entire BLE radio activity.
As far as the debugging singnals, **DEBUG A** signal is asserted when there is a request from the 24G device and de-asserted if the BLE radio is **NOT** overrulled. The **Events** trace describes an indication of the current consumed from the device while advertising (the 3 bumps on the trace describe the TX/RX activity of the device while advertising). **DEBUG B** signal indicates that there is a request from an external 24G device in order to send data. 

2. #### Priority on 24G packets while advertising
The current trace demonstrates the state of signals when there is no rule for prioritizing BLE advertising events. This means that either any data packets are pioritized but advertising packets or no rule is applied.

<figure>
<img src="media/no_adv_priority_with24G_request.png" alt="No advertising priority along with 24G request">
	<figcaption>Fig. 6: No advertising priority along with 24G request</figcaption>
</figure>

Since there is no priority on the advertising events the **BLE PRIO** signal remains de-asserted while the **BLE EIP** signal asserts regardless of the rules applied. **DEBUG A** pin asserts when waking up and remains asserted since the BLE radio is overruled from the 24G device. **DEBUG B** signal indicates the request from the 24G device. From the **Events** indication signal is clear that the BLE device supresses its radio activity in order for the 24G to aquire the antenna.

3. #### Priority on BLE data packets
To apply the rule for prioritizing the data packets on a specific connection the ```wlan_coex_prio_criteria_add(WLAN_COEX_BLE_PRIO_DATA, app_env[0].conhdl, 0);``` function should be invoked through the ```.app_on_init callback``` to set the rule.
The current trace pictures the case where the data packets are prioritized over 24G activity. It cleary noticable that the **BLE PRIO** signal always asserts for an upcomming BLE event. The connection interval of the specific trace is 45ms.

<figure>
<img src="media/data_packets_prio_with24G_activity.png" alt="BLE data packets priority along with 24G request">
	<figcaption>Fig. 7: BLE data packets priority along with 24G request</figcaption>
</figure>

4. #### Priority on BLE missed events
The COEX API also provides a missed events rule, available only for data packets, where the application is able to specify the amount of missed events which can be tolerated before asserting the priority signal. If for example during a connection the device doesn't get an ACK for a specific amount of connection events the **BLE PRIO** signal will be asserted in order to indicate to the 24G that there should be BLE communication on the next event.
To apply the rule for prioritizing based on the missed data packets on a specific connection the ```wlan_coex_prio_criteria_add(WLAN_COEX_BLE_PRIO_MISSED, app_env[0].conhdl, 5);``` function should be invoked through the ```.app_on_init callback``` to set the rule. The above function specifies that up to 5 connection events might be missed, on the 6th event the **BLE PRIO** signal will be asserted for the BLE to aquire the antenna.

<figure>
<img src="media/data_packets_missed_packets_prio_with24G_activity.png" alt="5 BLE missed data packets priority along with 24G request">
	<figcaption>Fig. 8: 5 BLE missed data packets priority along with 24G request</figcaption>
</figure>

Zooming in at Fig. 8 is clear that there is Tx/Rx activity on the 1st depicted connection event while the following events up to connection event 6 there is no radio activity. On connection event 7 the **BLE PRIO** signal is asserted indicating to the 24G device that BLE should aquire the antenna. 

<figure>
<img src="media/data_packets_missed_packets_prio_with24G_activity_zoomin.png" alt="5 BLE missed data packets priority along with 24G request zoomed in">
	<figcaption>Fig. 9: 5 BLE missed data packets priority along with 24G request zoomed in</figcaption>
</figure>

## Known Limitations

- When programing DA14531 SMARTBOND TINY™ MODULE using Keil, an error stating "No Cortex-M SW Device found" may occur when the user tries to open the debugger session. User must lower the JTAG clock speed to 2 MHz. For more information, see the section [KEIL Installation](http://lpccs-docs.dialog-semiconductor.com/UM-B-139-Getting-Started-with-DA14531-TINY-Module/05_Software_Development_Tools/Software_Development_Tools.html#keil-installation) in the UM-B-139 Getting Started with DA14531 TINY Module document
- No known hardware limitations for this example, but it is recommended to see the following:
  - Application note for [known hardware limitations for DA14531 devices](https://www.dialog-semiconductor.com/da14531_HW_Limitation)
  - Dialog Software [Forum link](https://www.dialog-semiconductor.com/forum)
  - [DA14531 Getting Started guide](https://www.dialog-semiconductor.com/da14531-getting-started)


## License


**************************************************************************************

 Copyright (c) 2020 Dialog Semiconductor. All rights reserved.

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
