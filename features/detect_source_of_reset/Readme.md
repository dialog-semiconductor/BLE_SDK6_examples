# Detect the source of the Reset 

------

## Example description

The main purpose of this SW example is to demonstrate how to detect the source of a reset event and explain how to store data into or/and read data from **retention RAM**.  This demonstration example **requires some SDK modifications** and is based on top of **SDK6.0.10**. The user can update the Bluetooth advertising data with the source of the reset or print the reason of the reset on a serial console.

## Features

- Store data into and/or read data from retention RAM
- Detect reset caused by Hardfault
- Detect reset caused by NMI
- Detect software reset
- Detect Power-on-Reset (POR)
- Store values in Retention-RAM
- Interacting with the UART serial port terminal
- Interacting with advertising data

## Custom profile
A 128-bit UUID custom profile us included with 1 custom service. 

<table>
  <caption> <b> <i> Custom Service 1 Characteristic Values and Properties</i></b></caption>
<thead>
  <tr class="header">
  <th style="text-align: left;">Name</th>
  <th style="text-align: left;">Properties</th>
  <th style="text-align: left;">Length(B)</th>
  <th style="text-align: left;">Description</th>
  </tr>
</thead>
<tbody>
  <tr class="odd">
  <td style="text-align: left;">Control Point</td>
  <td style="text-align: left;">Write</td>
  <td style="text-align: left;">1</td>
  <td style="text-align: left;">Accept commands from peer</td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">Reset detection</td>
  <td style="text-align: left;">Read</td>
  <td style="text-align: left;">20</td>
  <td style="text-align: left;">Update tte source of the reset in the peer device</td>
  </tr>
</tbody>
</table>

The implementation code of the Custom service is included in  ``user_custs1_impl.c``. The **user_svc1_ctrl_wr_ind_handler** is implemented,  which is the **Control Point** characteristic write indication handler.

## APIs brief description 

- **user_app_connection**: Connection function
- **user_app_adv_undirect_complete**: Undirect advertising completion function
- **user_app_disconnect**: Disconnection function
- **user_catch_rest_hndl**: Handles the messages that are not handled by the SDK internal mechanisms
- **append_data**: Append data into adverting data
- **user_app_init**: Application initialization function
- **user_power_on_reset**: Explicitly cause power on reset upon system initialization
- **ret_ram_data_init**: Store USER_RETRAM_DATA into the RetRAM upon system initialization

## How it works

### SDK modifications
-  Modifications in **Hardfault handler** (`sdk_root/hardfault_handler.c`)
  1. Include the following header files
      ```c
      #include "user_custs1_impl.h"
      ``` 
  2. Add detect_rst_flag variable
        ```c
      extern uint8_t detect_rst_flag;
      ```
  3. Add detect_rst_flag in **HardFault_HandlerC**
      ```c
      uint8_t detect_rst_flag = CUSTS1_HARDFAULT; 
      ```
-  Modifications in **NMI handler** (`sdk_root/nmi_handler.c`)
  1. Include the following header files
      ```c
      #include "user_custs1_impl.h"
      ```
  2. Add detect_rst_flag variable
      ```c
      extern uint8_t detect_rst_flag;
      ```
  3. Add detect_rst_flag in **NMI_HandlerC**
      ```c
      uint8_t detect_rst_flag = CUSTS1_NMI; 
      ```

### Store data into the Retention-RAM

- Define the maximum uninitialized retained data required by the application. By default, 64 bytes of uninitialized retained data are allocated. 
  - **#define CFG_RET_DATA_UNINIT_SIZE (64)** in `da1458x_config_advanced.h`
- Use the **USER_RETRAM_DATA** definition in `user_application.h`  to define the initialized data in Retention-RAM
  - **#define USER_RETRAM_DATA**  ("stored data")
- In order to store the data into the uninitialized area of the retention RAM, the **retention_mem_area_uninit** should be used, and so the initialized data should be defined as follow:
  - `uint8_t uninit_store_data[USER_RETRAM_DATA_LEN] __attribute__((section("retention_mem_area_uninit") , zero_init));  //@RETENTION MEMORY`
- **ret_ram_data_init** is used to store user's data into Retention-RAM.
- A flag, named **detect_rst_flag**, is stored in Retention-RAM which is used to detect the reason of the reset. 



### Reset detection

The **Control Point** attribute is used to indicate the reason of reset as described in `Features` section. The declaration of the characteristic is located in `user_custs1_def.h /.c`. If you are not familiar with custom profile declaration, please refer to [training material](https://www.dialog-semiconductor.com/sites/default/files/training_02_custom_profile_example_v1.2.pdf) from [support website](https://www.dialog-semiconductor.com/bluetooth-low-energy). Valid values of **Control Point** at:

- **Byte 0x01 : Hardfault** 
  - A hardfault is occurred by executing `*(uint32_t *)0x01 = 0x90;`
  - Once the hardfault is issued, the **HardFault_HandlerC** exception handler is triggered, located in `hardfault_handler.c`  
  - The **detect_rst_flag** value is assigned to **CUSTS1_HARDFAULT (=0x00)**
- **Byte 0x02 : NMI**
  - An endless loop will run, so that the WDOG will expire and an NMI will be fired.
  - Once the NMI is issued, the **NMI_HandlerC** exception handler is triggered, located in `nmi_handler.c`
  - The **detect_rst_flag** value is assigned to **CUSTS1_NMI (=0x01)**
- **Byte 0x03 : Software Reset** 
  - The **SYS_CTRL_REG** is used to trigger a *Software Reset* to the device
  - According to the datasheet, the SW reset is triggered by writing the **SYS_CTRL_REG[SW_RESET]** bit field.
  - The **detect_rst_flag** value is assigned to **CUSTS1_SW_RESET (=0x02)**

**Power-on-Reset (POR)** 
There is no dedicated register on the DA14585 that will allow us determine when a POR event is issued. However, it is of paramount importance to distinguish between the  production and actual operation of the device. This functionality, can be implemented using the *7-bit* **POR_TIMER_REG**. The default value is **0x18**. The above register retains its value between deep sleep, hardware and software resets and resets to the default value only after POR. To detect that device started operating following a POR event, application should probe **POR_TIMER_REG** looking for the default value and changing the value to a different one. 

**user_power_on_reset()** is used to explicitly cause POR upon system initialization. This API should be applied as soon as possible after the device boots. Both **POR_TIMER_REG** and **POR_PIN_REG*** are the only registers that will retain their values between resets and during any type of sleep modes and can be used to store application data. If the condition is true, then the value of the register is changed and **detect_rst_flag** is assigned to **CUSTS1_POR (=0x03)**. After POR, the device will start advertising again. **CFG_POWER_ON_RESET**, located in `user_custs1_impl.h`,  should be used to activate/deactivate POR. 

   

```c
void user_power_on_reset(void)
{
    // Check for POR_TIMER_REG default value
  if (GPIO_GetPorTime() == 0x18)  
    {
        // Change value to other than defaultdetect_rst_flag = CUSTS1_POR;
      GPIO_SetPorTime(0x19);

    //Restart advertising after the power on reset
    user_app_adv_start();
    }
}
```



### Update advertising data and print out the reason of the reset

- `user_app_adv_start()` is modified for this scope. According to **detect_rst_flag**, after the reset, the advertising data will be updated with the reason of the reset `append_data()` is used for appending the appropriate data. 
- The user has the option to print out the reason of the reset on a serial terminal. To do so, **CFG_PRINTF** should be defined in `da1458x_config_basic.h`.

## HW and SW configuration

- **Hardware configuration**
  - This example runs on DA14585/DA14586 Bluetooth Smart SoC devices.
  - The Basic or Pro Development kit is required for this example.
  - Connect the Development kit to the host computer.
  - Jumpers are placed on standard SPI flash setup, plus extra jumpers for UTX on **P04 - JL_RxD**,and jumping wiring for **JL_TxD on P02**. See illustration below:

 ![Pro-DK Jumper Placement](assets\Board_setup.png)

- **Software configuration**
  - This example requires:
    - Smartsnippets Studio 2.0.8.
    - SDK6.0.10
    - A smartphone with a BLE scanning app (for example BLE scanner on Android or Lightblue on IOS)
  - **SEGGER’s J-Link** tools should be downloaded and installed.
  - Python 3.5 or greater

## How to run the example

### Initial Setup

For the initial setup, please refer to [this section](https://support.dialog-semiconductor.com/resource/da1458x-example-setup).

### How it works

### Compile and run

- Open the project via Keil µVision 5
- Compile and run the project
- Open a `BLE scanner` App and search for **DETECT-RESET**
- Connect to the device
- You should see the **Control Point** Bluetooth characteristic



### Hardfault example

1. Open the project via Keil µVision 5

2. Build the project 

3. Run it in **debug mode**. To learn basic debugging techniques, please refer to training material](https://www.dialog-semiconductor.com/sites/default/files/training_08_debugging_v1.1.pdf).&nbsp;
   ![Debug mode](assets\Debug_mode.PNG)

4. Connect to  **DETECT-RESET** &nbsp;
   ![ble_scanner_1](assets\ble_scanner_1.png)

5. A custom service with the **Control Point** characteristic should be appeared.&nbsp;
   ![ble_scanner_2](assets\ble_scanner_2.png)

6. &nbsp;Write **0x00** to trigger a hardfault&nbsp;
   ![ble_scanner_3](assets\ble_scanner_3.png)

7. &nbsp;A hardfault is occured&nbsp;
   ![hardfault](assets\hardfault.PNG)

8. Stop the debug procedure and reset the Pro-DK. detect_rst_flag** is stored into Retention-RAM, and so its value is retained.

9. Add a break point into ``user_app_adv_start()`` function and run it again in debug mode.

10. Step over (press F10) and the "Hardfault" is appended into the  advertising data.&nbsp;
    ![hardfault_1](assets\hardfault_1.PNG)

11. Stop the debug procedure

12. &nbsp;Open the Serial terminal (TerTerm) with the following COM port configurations&nbsp;
    ![comport](assets\comport.PNG)
    
13. &nbsp;Download the firmware into System-RAM again and  the reason of the reset should be appeared on the monitor.&nbsp;
    ![teraterm](assets\teraterm.PNG)



## Known Limitations

- The sectors of the Keil are 4 bytes aligned, so the maximum uninitialized retained data that are required by the application (*CFG_RET_DATA_UNINIT_SIZE*) **must be multiple of 4  and greater or equal to the sum of the total data**. If the *CFG_RET_DATA_UNINIT_SIZE* is not multiple of 4, the Keil will truncate it into the smallest multiple of 4, otherwise if the data cannot be fit then the hole section will be deleted. 
- Refer to the following application note for [known hardware limitations](https://support.dialog-semiconductor.com/system/files/resources/DA1458x-KnownLimitations_2018_02_06.pdf "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software"Forum link").
- You can refer to Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.

