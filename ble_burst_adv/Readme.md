# ble_burst_adv

Burst Advertising with Bluetooth LE

## Example description

Simple example showing how to implement 'burst' advertising on the DA14585/6. Burst 
advertising is the process of sending a number of advertising events and then having 
a delay before sending another group of advertising events. This example allows the 
user to configure the number of advertising events to be transmitted per burst and 
the time between bursts. 
	
## HW and SW configuration


* **Hardware configuration**

	- This example runs on the DA14585/DA14586 Bluetooth Smart SoC devices.
	- A Basic or PRO Development kits is needed for this example.
	
* **Software configuration**

	- This example requires:
        * Smartsnippets Studio 2.0.8 (or later)
        * SDK6.0.10 (or later)

## How to run the example

For initial setup of the example please refer to [this section of the dialog support portal](https://www.dialog-semiconductor.com/sites/default/files/sw-example-da1458x-example-setup.pdf).

### Initial Setup

1.  Build and download the example using the Keil IDE. 
2.  Run the example using the Keil debugger.
3.  Observe bursts of advertising events using of the following:
        * Monitor UART debug output.
        * Monitor current consumption using the Power Profiler found within the SmartSnippets Toolbox.
        * Monitor transmitted advertising events using a Smart Phone App such as LightBlue.

## Known Limitations


- There are No known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations](https://www.dialog-semiconductor.com/sites/default/files/da1458x-knownlimitations_2019_01_07.pdf "known hardware limitations").
- Dialog Software [Forum Link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software "Forum Link").
- You can also refer to the troubleshooting section in the [Getting Started Link](https://www.dialog-semiconductor.com/sites/default/files/um-b-049_da14585da14586_getting_started_guide_v2.1_0.pdf "DA1585x Getting Started with the Development Kit UM-B-049").


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
