# DA14585/DA14586/DA14531 DLE and L2CAP Packet Optimization

---


## Example description

This project is intended to illustrate to the user:
- How to do a peer feature request for determining peer DLE capability
- How to request DLE and utilize larger packets to enhance throughput
- Basic principles of L2CAP 100% packet utilization
- Packet optimization for non DLE devices

This functionality can be verified by:
- Using a BLE handset
- Reading serial output from the device.
	

## HW and SW configuration
This example runs on the BLE Smart SoC (System on Chip) devices:
- DA14585/DA14586 or DA14531 daughter board + DA145xxDEVKT-P PRO-Motherboard.
- DA14585/DA14586 daughter board + Basic development Kit mother board.
- DA14531 USB Kit

The user manuals for the development kits can be found:
- [here](https://www.dialog-semiconductor.com/products/da14531-development-kit-pro) for the DA145xxDEVKT-P PRO-Motherboard.
- [here](https://www.dialog-semiconductor.com/sites/default/files/um-b-048_da14585da14586_getting_started_guide_v2.0_0.pdf) for the Basic Development Kit.

* **Special Hardware configuration setup**

* __Hardware configuration using DA145xxDEVKT-P PRO-Motherboard__
	- When using DA14585 make sure a jumper is across UTX.
	- When using DA14531, Jumper P2_6 from J2 to UTX (Pin 18 on J1) for serial UART communication.
	![Motherboard_Hardware_Configuration_DA14531](assets/Motherboard_Hardware_Configuration_DA14531.png).
	
__Hardware configuration using LEGACY DA145xxDEVKT-P PRO-Motherboard__
	- When using DA14585, make sure the UTX is jumpered across J5 (Default).
	- When using DA14531, Jumper P2_6 from J7 to UTX (Pin 10).

* **Software configuration**

	- This example requires:
    * SDK6.0.12 or Newer (Older versions of SDK6 require minimal changes)
	- **SEGGER’s J-Link** tools should be downloaded and installed.


## How to run the example
### Setup


Before launching the Keil project, make sure to link the SDK and project environment using the Python linker script `dlg_make_keil_vx.xxx`. More information [here](https://www.dialog-semiconductor.com/sites/default/files/sw-example-da145x-example-setup.pdf).
1. Start Keil using the `tput_optimization.uvprojx` Keil project file.

2. Expand the dialog shown in the red box in the image below.
	![Expand_Select_Device](assets/Expand_Select_Device.png)

3. Select your target device: DA14531, DA14585, DA14586

4. Open your favorite serial terminal application, e.g. TeraTerm, RealTerm, Putty - use 115200, 8, N, 1 as your settings.

5. Compile (F7) and launch (ctrl + F5) the example.\
If the warning (shown below) pops up press OK.

### Running the Example

1. Open a BLE application on your handset and look for "DIALOG-TPUT"

2. Connect to the device.
	![Connect_Device](assets/Connect_Device.png)
	
3. Subscribe to notifications on the Log Transfer characteristic.
	![Log_Characteristic](assets/Log_Characteristic.png)

4. Write the value to 0xAA to the Control Point Characteristic. 
	![CP_Characteristic](assets/CP_Characteristic.png)

5. This will begin a log transfer of static bytes. 

6. Check your terminal for debug output and throughput calculation.
	![Terminal_Ouptut](assets/Terminal_Output.png)

## How it works
### General Logic
The example uses a successive approach for optimization throughput.  In the Link Layer, we are limited to two basic buffer sizes depending on
The support for DLE: 27, 251.  First, we do a peer feature request to see if DLE is supported - If supported, we utilize
the additional packet space.  From here, we do a connection parameter update, and following the result of this we do an MTU Exchange. 
The MTU exchange is intended to grant a larger payload for non-DLE devices and provide adequate buffer space for DLE devices.
Following the MTU exchange, the optimal packet size is determined.  We calculate the required overhead of the L2CAP and ATT and force
our payload then to utilize the the rest of the space.  For non-DLE devices that have MTUs greater than 104, we statically set the payloadsize to
101.  This number is has been emperically determined to provide good throughput in a noisier environment. Full packet utilization provides
a good enhancement on throughput if otherwised not considered. 

### Note
This example does not consider a more complex connection parameter scheme for throughput optimization.  This would be an excercise to the user.
Note that DSPS does include this functionality for reference and provides optimal connection parameters for DLE and non-DLE devices.


## Known Limitations

- There are No known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations](https://www.dialog-semiconductor.com/bluetooth-low-energy "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software"Forum link").
- you can Refer also for the Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.


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
