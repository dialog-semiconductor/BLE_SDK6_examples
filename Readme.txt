# Example name

Button functionalities and advertising


## Example description

Code snippet displaying GPIO functionalities (button, timer, LED) as well as changing advertising modes (connecable / non-connectable) and advertising intervals
	
## HW and SW configuration


* **Hardware configuration**
	- This example runs on the DA14585 Pro Dev kit connected to host computer via USB

* **Software configuration**

	- This example requires:
    	* Smartsnippets Toolbox v4.8.3.1804
    	* SDK 6.0.10.511


## How to run the example

Use the python script (in the project folder) to link the SDK path to the example.

step 1 locate your untouched SDK6, available from the customer support website

step 2 Run the script from the command line with the extension -sdkpath "<untouched SDK location>"

step 3 when finished, you should be able to run the example


### Initial Setup

1. 	BLE application starts advertising with 100ms connectable advertising interval
2. 	Press SW3
3. 	BLE application updates the advertising interval to 1000ms and keeps advertising 
	but now this time non-connectable advertising
4. 	Press the same button go back to step 1
5. 	If the same button is pressed for 3 seconds the device goes to sleep
6. 	If the same button is pressed for 3 seconds the device wakes up and starts advertising 
	with the last advertising interval was used and the last known mode it was advertising with.
7. 	You can check the state using serial port messages
8.	Using the power profiler from the smartsnippet toolbox you can check the state by measuring 
	the power consumption


## Known Limitations


- There are No known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations](https://support.dialog-semiconductor.com/system/files/resources/DA1458x-KnownLimitations_2018_02_06.pdf "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software "Forum link").
- You can Refer also for the Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.


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

