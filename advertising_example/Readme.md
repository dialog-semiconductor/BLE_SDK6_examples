# DA14585 - Changing advertising parameters using GPIO interrupts and/or timers

---


## Example description

This SDK6 DA14585 example provides a implementation of a advertising state machine, changing between non-connectible advertising, undirected advertising and sleep state. The advertising state is dependent upon button events by default, but with a simple hash define the user can configure it to be dependent on timers instead. The timings of the states and advertising intervals are pre-compilation configurable as well. The example includes UART messages to a serial console (i.e. Tera Term) and LED indication by turning the LED on during advertising. Both of these options can be turned off if desired.
## HW and SW configuration


* **Hardware configuration**

	- This example runs on The DA14585 Bluetooth Smart SoC devices.
	- The Basic or pro Development kit is needed for this example.
	- Connect the USB Development kit to the host computer.
	- Button is configured to P1_1 corresponding to SW3 on the Pro Development kit.
		- For the Basic development kit, an active-low switch should be connected to P1_1 as displayed in the schematic underneath
	- The jumper settings are displayed in pink in the schematic underneath. These settings are the same for both the pro and basic development kit.

	**DA14585 Schematic**

	![breadboard](assets/advertising_example_schematic.png)


* **Software configuration**

	- This example requires:
    * Smartsnippets Studio 1.6.3.
    * SDK6.0.10
	- **SEGGER’s J-Link** tools should be downloaded and installed.


## How to run the example

For initial setup of the example please refer to [this](https://support.dialog-semiconductor.com/resource/da1458x-example-setup) section of the dialog support portal.
The example is running from SRAM. For programming to Flash, see chapter 11 in the [SmartSnippets Toolbox User Manual](https://support.dialog-semiconductor.com/resource/um-b-083-smartsnippets-toolbox-user-manual).

### Initial Setup

 - Start Keil
 - [Optional] Configure the following parameters at the top of the da1458x_config_basic.h file.
 	- Make sure *ADV_EXAMPLE* is defined.
 	- Make sure either *ADV_BUTTON_EXAMPLE* **OR** *ADV_TIMER_EXAMPLE* is defined based on which example is preferred.
 	- Make sure *LED_INDICATION* is defined if this is you preference.
 	- Make sure *CFG_PRINTF*  is defined if UART messages are desired.


 - [Optional] Configure the following parameters in the advertising_example.h files
 	- Set *UNDIRECT_ADV_INTERVAL* to the desired value in milliseconds to configure the undirected advertising interval.
 	- Set *NONCONN_ADV_INTERVAL* to the desired value in milliseconds to configure the non-connectible advertising interval.
 	- Set *UNDIRECT_ADV_DURATION* to the desired value multiplied by 10 milliseconds to determine the duration of the undirected advertising in the timer based example.
 	- Set *NONCONN_ADV_DURATION* to the desired value multiplied by 10 milliseconds to determine the duration of the non-connectible advertising in the timer based example.
 	- Set *SLEEP_DURATION* to the desired value multiplied by 10 milliseconds to determine the sleep duration in the timer based example.


 - Compile and launch the example

 - [Optional] Open the serial terminal of your choice (i.e. Tera Term) with the following parameters.
	- baud rate: 115200
	- data: 8 bits
	- stop: 1 bit
	- parity: None
	- flow  control: none

- If *ADV_BUTTON_EXAMPLE* is defined (default), the example will switch states based on button events like shown in the diagram below. In order to differentiate from a long and a short button press in this example a three second timer is started on the positive edge. If the timer expires, a flag will be set, indicating a long button press. Upon button release, the timer will be terminated if it is still running. After this a check will detect if the flag is set or not and determine the state accordingly.

![adv_button_diagram](assets/adv_button_diagram.png)

- if *ADV_TIMER_EXAMPLE* is defined, the example will switch states based on timer events like shown in the diagram below. Upon initiating advertising, a timer is started with a registered callback to update the advertising parameters. In this example however, the callback, instead of updating the parameters, will activate the next state.

![adv_timer_diagram](assets/adv_timer_diagram.png)


### About this example
Starting point for this example is the BLE barebone project from SDK6. This provides the right framework for the advertising events used in this example.
- The user_barebone.h and user_barebone.c files were renamed to user_adv_example.h and user_adv_example.c. Some changes and additions were made here. These changes can be easily traced as they are marked with the ADV_EXAMPLE flag.
- Some minor changes were made to the user_peripheral_setup files to accommodate the button functionality and led indication.
- The user_callback_config.h has been slightly modified. Callback is added for non-connectible advertising as well as a callback to check the last BLE event for the sake of advertising LED indication.
- The user_button.h and user_button.c were added. These files accommodate the button functionality used in this example.


## Known Limitations


- There are No known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations](https://support.dialog-semiconductor.com/system/files/resources/DA1458x-KnownLimitations_2018_02_06.pdf "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums).
- you can Refer also for the Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.


## License


**************************************************************************************
Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs included in this package ("Software") include confidential, proprietary information of Dialog Semiconductor. All Rights Reserved.

THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR (‘DIALOG’) AND MAY ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND.  THIS SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS.  RECIPIENT SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT DIALOG’S PRIOR WRITTEN PERMISSION.

UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED BY THE FCC OR OTHER LIKE AGENCIES.

IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**************************************************************************************
