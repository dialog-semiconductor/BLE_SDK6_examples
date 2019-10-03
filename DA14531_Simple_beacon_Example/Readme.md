------

# DA14531 - DA14585/6 Simple Beacon Example 

------

## Example description

The main purpose of this SW example is to demonstrate creating a **Non-Connectable Advertising** application example. To make Non-Connectable advertising more efficient the ``flag type`` field of an advertising packet is optional (please check **Core Specification Supplement v6 by Bluetooth SIG**). If the ``flag type`` field is not used, it can free up to 3 bytes of space which can then be used for sending more advertising data.

## Non-Connectable Advertising

For getting more information about the Non-Connectable Advertising, please refer to [Advertising Tutorial](https://www.dialog-semiconductor.com/sites/default/files/advertising_concept.pdf) from our [support website](https://www.dialog-semiconductor.com/bluetooth-low-energy).

## HW and SW configuration

- **Hardware configuration**
  - This example runs on the DA14531, DA14585, DA14586 Bluetooth Smart SoC devices.
  - The Basic / Pro Development kit is needed for this example with default jumper configuration..
  - Connect the USB Development kit to the host computer.
- **Software configuration**
  - SmartSnippets Studio v2.0.x.
  - SDK6.0.1x.
  - **SEGGER’s J-Link** tools should be downloaded and installed.
  - A smartphone with a BLE scanning app (for example **BLE scanner** on Android or **Lightblue** on IOS).
  - A BLE Sniffing tool is also useful; though not mandatory .

## How to run the example

### Initial Setup

For the initial setup, please refer to [this section](https://www.dialog-semiconductor.com/sites/default/files/sw-example-da1458x-example-setup.pdf").

### Compile & Run

- Navigate to ``project_environment`` folder and open the **Keil **project.
- Compile and then launch the demonstration example. You can download the firmware either into System-RAM or SPI Flash. To download the firmware into SPI Flash, the  SPI Flash programmer from SmartSnippets Toolbox should be used. 
- Define the data that is to append into ``Adverting`` or ``Scan Response`` data packet by configuring the **USER_DATA** macro in `user_simple_beacon.h`. By default, **USER_DATA** is defined as follow :

```c
#define USER_DATA  ("DA14585/6 & DA14531 Simple Beacon Software Example")
```

***Note:***
**USER_DATA** are stored in the **uninitialized section of the Retention-RAM**

```c
uint8_t user_store_data[USER_DATA_LEN]            __attribute__((section("retention_mem_area_uninit") , zero_init));  //@RETENTION MEMORY
```

and copied into the `user_store_data[]` array:

```c
memcpy(user_store_data, USER_DATA ,USER_DATA_LEN );
```

- Initially, the device initiates non-connectable advertising (**ADV_NONCONN_IND**) and then proceeds with sending an advertising scan indication (**ADV_SCAN_IND**).  Finally, the device is switched back to non-connectable advertising mode. This is a full loop and is repeated all the time. 
- Initially, the code checks if `user_store_data[]` is not empty. 
- Every time the SW timer elapses, the `adv_data_update_timer_cb()` callback routine is triggered and each element of `user_store_data[]` is stored in the Advertising Data or Scan Response data packet.
- The firmware checks whether the advertising data are full or not. If not, the next item from ``user_store_data[]`` will be appended into them. 
- Once the Advertising data packet is full and given that more data are present in ``user_store_data[]` , the next user data will be stored into the Scan Response data packet (**SCAN_RESP**).

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