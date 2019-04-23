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

	- This example runs on The DA14485/DA14586 Bluetooth Smart SoC devices.
	- A Basic or PRO Development kits is needed for this example.
	
* **Software configuration**

	- This example requires:
        * Smartsnippets Studio 2.0.8 (or later)
        * SDK6.0.10 (or later)

## How to run the example

For initial setup of the example please refer to [this section of the dialog support portal](https://support.dialog-semiconductor.com/resource/da1458x-example-setup).

### Initial Setup

1.  Build and download the example using the Keil IDE. 
2.  Run the example using the Keil debugger.
3.  Observe busrts of advertising events using of the following:
        * Montor UART debug output.
        * Monitor current consumption using the Power Profiler found within the SmartSnippets Toolbox.
        * Montior transmitted advertising events using an Smart Phone App such as LightBlue.

## Known Limitations


- There are No known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations](https://support.dialog-semiconductor.com/system/files/resources/DA1458x-KnownLimitations_2018_02_06.pdf "known hardware limitations").
- Dialog Software [Forum Link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software "Forum Link").
- you can Refer also for the Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.


## License


**************************************************************************************

 Copyright (c) 2018 Dialog Semiconductor. All rights reserved.

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
