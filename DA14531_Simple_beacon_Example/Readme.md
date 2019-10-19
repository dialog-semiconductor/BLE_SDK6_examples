
------

# DA14531 - DA14585/586 Simple Beacon Example 

------

## Example description

The main purpose of this Software Εxample is to demonstrate creating a **Non-Connectable Advertising** application example. To make Non-Connectable advertising more efficient the ``flag type`` field of an advertising packet is optional (please check **Core Specification Supplement v6 by Bluetooth SIG**). If the ``flag type`` field is not used, it can free up to 3 bytes of space which can then be used for sending more advertising data.

## What is a Beacon?

Beacons are battery powered devices that advertise a particular Bluetooth Low Energy payload with identifying information. 

<p align="center">
  <img src="assets\what_is_beacon.PNG"/>
</p>

This Software Example is just to demonstrate creating a Simple Beacon project with **Non-Connectable Advertising**. However, it can be customized for for more advanced use cases, such as:
  * Adaptive modification of advertising data
  * Choosing from various beacon formats
  * Interleaving connectable advertising events
  * Software Updates Over The Air (SUOTA)

## Non-Connectable Advertising 

Non-connectable advertising allows users to advertise data with the lowest power consumption.

<p align="center">
  <img src="assets\non_conn_adv.PNG"/>
</p>

The SDK includes a non-connectable advertising application example. The project is in the SDK directory:

    <sdk_root_directory>\projects\target_apps\misc\.

The Keil v5 project file is:

    <sdk_root_directory>\projects\target_apps\misc\ble_app_noncon\Keil_5\ble_app_noncon.uvprojx

The **main difference** between Simple Beacon SW Example and Non-Connectable example of the SDK is that in this implementation, user can define the data which will be appended into the Advertising or Scan Response data. In the SDK Example, there is not this possibility, as the data which is stored in the adverting or Scan response packet was fixed (0xA5). To do so, Beacons will advertise a particular Bluetooth Low Energy payload with identifying information selected by the user. 

The appropriate APIs should be used are the listed below :

 - **app_easy_gap_non_connectable_advertise_get_active()** : Get non connectable advertising message with filled parameters.
 - **app_easy_gap_non_connectable_advertise_start()** : Start advertising for non-connectable peripheral.

For getting more information about the Non-Connectable Advertising, please refer to [Advertising Tutorial](https://www.dialog-semiconductor.com/sites/default/files/advertising_concept.pdf) from our [support website](https://www.dialog-semiconductor.com/bluetooth-low-energy).

## Hardware & Software Configuration
    
This example runs on DA14531 and DA14585/586 Bluetooth Smart SoC devices. 
  - Connect the Development Kit with the host computer over the USB connector. 
  - Hardware configurations are different between DA14585/586 and DA14531. Please find bellow the approprate the hardware configuration.

### DA14585/586 Hardware Configuration

In case of DA14585/586, the DA1458x Pro Development Kit is required with default jumper configuration. Please see the illustration below:

<p align="center">
  <img src="assets\585_basic_DK.PNG"/>
</p>

Refer to the getting started guide of [DA14585 Basic Development Kit](https://s3.eu-west-2.amazonaws.com/lpccs-docs.dialog-semiconductor.com/DA14585_Getting_Started_basic/index.html).

### DA14531 Hardware Configuration

In case of DA14531, either  **DA145xx Pro Development Kit** or **DA14531 USB Kit** is required for this Software Example with default jumper configuration. Please see the illustrations below:

**DA145xx Pro Development Kit**

Jumpers are placed in default configurations. Refer to the getting started guide of [DA145xx Pro Development Kit](https://www.dialog-semiconductor.com/da14531-getting-started).
<p align="center">
  <img src="assets\531_pro_dk.PNG"/>
</p>

**DA14531 USB Kit** 

Refer to the getting started guide of DA145xx USB Kit
<p align="center">
  <img src="assets\531_usb_kit.PNG"/>
</p>

### Software configuration
  - SDK6.0.12 or later
  - **SEGGER’s J-Link** tools should be downloaded and installed.
  - A smartphone with a BLE scanning app (for example **BLE scanner** on Android or **Lightblue** on IOS).
  - A BLE Sniffing tool is also useful; though not mandatory .

## How to run the example

### Initial Setup

- For the initial setup, please refer to [this section](https://www.dialog-semiconductor.com/sites/default/files/sw-example-da145x-example-setup.pdf).

- For the DA14585/586 getting started guide you can refer to this [link](http://lpccs-docs.dialog-semiconductor.com/da14585_getting_started/index.html).
- For the DA14531 Getting started guide you can refer to this [link](https://www.dialog-semiconductor.com/da14531-getting-started).

### Compile & Run

- Navigate to ``project_environment`` folder and open the **Keil **project.
- Compile and then launch the demonstration example. You can download the firmware either into System-RAM or SPI Flash. To download the firmware into SPI Flash, the  SPI Flash programmer from SmartSnippets Toolbox should be used. 
- Define the data that is to append into ``Adverting`` or ``Scan Response`` data packet by configuring the **USER_DATA** macro in `user_simple_beacon.h`. By default, **USER_DATA** is defined as follow :

```c
#define USER_DATA  ("DA14585/586 & DA14531 Simple Beacon Software Example")
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


To verify please take a look at the BLE sniffer log data

1. Initially only the the Device Name is placed into Adverising Data.
  <p align="center">
    <img src="assets\device_name.PNG"/>
  </p>

2. Advertising Data are now full.
  <p align="center">
    <img src="assets\non_conn.PNG"/>
  </p>

3. Switching from ADV_NONCONN_IND to ADV_SCAN_IND.
  <p align="center">
    <img src="assets\adv_ind.PNG"/>
  </p>

4. Central sends a scan request (SCAN_REQ) in order to receive a scan response (SCAN_RESP) from the advertiser.
  <p align="center">
    <img src="assets\scan_req.PNG"/>
  </p>

5. Full Scan Response Data.
  <p align="center">
    <img src="assets\scan_resp.PNG"/>
  </p>

## Known Limitations

- Refer to the following application note for [DA1458x known hardware limitations](https://www.dialog-semiconductor.com/sites/default/files/da1458x-knownlimitations_2019_01_07.pdf  "known hardware limitations").
- Refer to the following application note for [DA14531 known hardware limitations](https://www.dialog-semiconductor.com/da14531_HW_Limitation  "known hardware limitations"). 
- If you have any question, please contact us in our [public Bluetooth forums](https://www.dialog-semiconductor.com/forum).

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