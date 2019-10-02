------

------

# DA14531 - DA14585/6 Simple Beacon Example 

------

## Example description

The main purpose of this SW example is to demonstrate how to create a **non-connectable** advertising application example. To make non-connectable advertising more efficient the flag type field inside the advertising packet is optional (please check **Core Specification Supplement v6 by Bluetooth SIG**). If the flag type field is not used it will free up to 3 Bytes of space which can be used to send more advertising data.

## Introduction

The following figure (Fig.1) demonstrates the luetooth Low Energy Protocol Stack

<center>
 <figure>
  <img src="assets\ble_protocol_stack.PNG" >
  <figcaption><i> <b>Fig.1</b> - Bluetooth Low Energy Protocol Stack</i></figcaption>
</figure>
</center>

### Advertising Concept

Bluetooth low energy can broadcast small packets of data containing advertisements to peer devices. 

- An Advertising packet is small and has a well-defined format. As a result, only a restricted amount of user data can be carried.
- The Advertising mode also supports transmission of a secondary scan Response packet which contains additional data. This data can be requested by a potential client using a Scan Request without establishing a permanent connection to the device.
- An Advertising packet is made up of a number of fields which typically includes:
- The name of the device
- Some or all of the services supported by the device
- Advertising packets may also contain proprietary manufacturer-specific data and flags declaring the capabilities of the device.

### Advertising with Respect to Bluetooth Low Energy

Bluetooth low energy implements two communication methods:

- Advertisement: A Bluetooth low energy peripheral device broadcasts packets to every device around it. The receiving device can then act on this information without establishing any connection (scan request) or it may also connect to receive further information.
- Connected: Communication is setup to receive packets using a physical connection link, where both the peripheral and central devices send packets.

### BLE States during Communication

<table>
  <caption>  <i><b>Table 1</b> - BLE States during Communication</i></caption>
<thead>
  <tr class="header">
  <th style="text-align: left;">State</th>
  <th style="text-align: left;">Description</th>
  </tr>
</thead>
<tbody>
  <tr class="odd">
  <td style="text-align: left;">Standby</td>
  <td style="text-align: left;">Does not transmit or receive packets</td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">Advertising</td>
  <td style="text-align: left;">Broadcasts advertisements in advertising channels</td>
  </tr>
    <tr class="odd">
  <td style="text-align: left;">Scanning</td>
  <td style="text-align: left;">Looks for advertisers</td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">Initiating</td>
  <td style="text-align: left;">Initiates connection to advertiser</td>
  </tr>
    <tr class="odd">
  <td style="text-align: left;">Connection : Master Role</td>
  <td style="text-align: left;">Communicates with device in the Slave role, defines timings of
transmissions</td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">Connection : Slave Role</td>
  <td style="text-align: left;">Communicates with single device in Master Role</td>
  </tr>
</tbody>
</table>



<center>
 <figure>
  <img src="assets\ble_states.PNG" >
  <figcaption><i> <b>Fig.2</b> - BLE States during Communication</i></figcaption>
</figure>
</center>

### Advertising Packets

A packet can be **80** to **376** bits in length, and has the following fields:

1. Preamble** : Used for internal protocol management. Advertising packets have 0xAA as preamble.
2. **Access Address**: This is always **0x8E89BED6** for advertising packets.
3. **PDU** :There are two PDU formats, one for advertising packets and one for data packets. The Advertising PDU consists of the 16-bit PDU header and, depending on the type of advertising, the device address (6 bytes) and up to 31 bytes of information. If the advertising mode allows it, the active scanner (through scan request) may request up to 31 bytes of additional information from the advertiser. This means that a sizeable portion of data can be received from the advertising device even without establishing a connection.

<center>
 <figure>
  <img src="assets\adv_packets.PNG" >
  <figcaption><i> <b>Fig.3</b> - Advertising Packets</i></figcaption>
</figure>
</center>

## HW and SW configuration

- **Hardware configuration**
  - This example runs on the DA14531, DA14585, DA14586 Bluetooth Smart SoC devices.
  - The Basic / Pro Development kit is needed for this example.
  - Connect the USB Development kit to the host computer.
- **Software configuration**
  - This example requires:
    - Smartsnippets Studio v2.0.8.
    - SDK6.0.1x
  - **SEGGER’s J-Link** tools should be downloaded and installed.

## Analyzing The Simple Beacon Example

This section analyzes the source code of the application example. 

### Setting the Bluetooth Address and Device Name

1. ***Setting the BD Address*:** The public address of the device is set in the **CFG_NVDS_TAG_BD_ADDRESS** flag in `da1458x_config_advanced.h` .

```c
#define CFG_NVDS_TAG_BD_ADDRESS             {0x00, 0x11, 0x70, 0xCA, 0xEA, 0x80}
```

To change the BD address you have only to change this define.

2. ***Setting the Advertised Device Name***: User is free to change the Device Name as well by modifying the **USER_DEVICE_NAME** macro in `user_config.h`.

```c
/// Device name
#define USER_DEVICE_NAME        "SIMPLE-BEACON"

/// Device name length
#define USER_DEVICE_NAME_LEN    (sizeof(USER_DEVICE_NAME)-1)
```

3. ***Changing the Advertising Data***: User can also change the Advertising Data or the Scan Response Data in `user_config.h` by modifying the `USER_ADVERTISE_DATA` or `USER_ADVERTISE_SCAN_RESPONSE_DATA`. For this software example, both of them empty because the application will update them on run time. Upon system initialization, the Device Name will be placed into Advertising Data. 

```c
/// Advertising data
#define USER_ADVERTISE_DATA                   ""

/// Advertising data length - maximum 28 bytes, 3 bytes are reserved to set
#define USER_ADVERTISE_DATA_LEN               (sizeof(USER_ADVERTISE_DATA)-1)

/// Scan response data
#define USER_ADVERTISE_SCAN_RESPONSE_DATA     ""

/// Scan response data length- maximum 31 bytes
#define USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN (sizeof(USER_ADVERTISE_SCAN_RESPONSE_DATA)-1)
```

### Non-Connectable Advertising

The advertising mode is customizable by calling an of the APIs from `app.c` file which is located under `sdk/app_modules/src/app_common` SDK path. For non-connectable advertising the following APIs should be used:

1. Get non connectable advertising message with filled parameters.

```c
struct gapm_start_advertise_cmd* app_easy_gap_non_connectable_advertise_get_active(void);
```

2. Start advertising for non-connectable peripheral.

```c
void app_easy_gap_non_connectable_advertise_start(void);
```

## How to run the example

### Initial Setup

For initial setup of the example please refer to [this section of the dialog support portal](https://www.dialog-semiconductor.com/sites/default/files/sw-example-da1458x-example-setup.pdf").

### Complile & Run

1. Navigate to project_environment folder and open the Keil project
2. Compile and launch the example. You could download the firmware either into System-RAM or into SPI Flash with the usage of SPI Flash programmer from SmartSnippets Toolbox. 
3. Define the data that you would like to store in the Advertinf and Scan Response Data by configuring the USER_DATA macro in `user_simple_beacon.h`. By default the USER_DATA macro is defined as follow and it's just for demonstration purposes:

```c
#define USER_DATA  ("DA14585/6 & DA14531 Simple Beacon Software Example")
```

***Note:***
*The **USER_DATA** are stored in the **uninitialized section of the Retention-RAM***  

```c
uint8_t user_store_data[USER_DATA_LEN]            __attribute__((section("retention_mem_area_uninit") , zero_init));  //@RETENTION MEMORY
```

*and copied into the `user_store_data[]` array in `data_init()`:*

```c
memcpy(user_store_data, USER_DATA ,USER_DATA_LEN );
```

4. The device will start non-connectable advertising (**ADV_NONCONN_IND**) and then it will start sending a advertising scan indication (**ADV_SCAN_IND**)and again it will switch back to non-connectable advertising. 
5. If the Advertising data are full and if there is a remaining element in the `user_store_data[]` , the next user data will be stored into the Scan Response data (**SCAN_RESP**)
6. Every time time that the timer elapses and the `adv_data_update_timer_cb()` is triggered, each element of `user_store_data[]` will be stored in the Advertising Data or in Scan Response data if it is possible. 

## Known Limitations

- Refer to the following application note for [known hardware limitations](https://support.dialog-semiconductor.com/system/files/resources/DA1458x-KnownLimitations_2018_02_06.pdf "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software"Forum link").


## License

------

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

------