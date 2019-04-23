------

# Detect the source of the Reset 

------

## Example description

The main purpose of this SW example is to demonstrate how to detect the source of a reset event and explain how to store data into or/and read data from **retention RAM**.  This demonstration example does not require SDK modifications and is based on top of **SDK6.0.10**. The user can update the Bluetooth advertising data with the source of the reset or print the reason of the reset on a serial console.

## Features

- Store data into and/or read data from retention RAM
- Detect reset caused by Hardfault
- Detect reset caused by NMI
- Detect software reset
- Detect Power-on-Reset (POR)
- Store values in Retention-RAM
- Interacting with the UART serial port terminal
- Interacting with advertising data

## How it works

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

An 128-bit UUID custom service is utilized and includes a characteristic attribute named **Control Point**. This attribute is used to indicate the reason of reset as described in `Features` section. The declaration of the characteristic is located in `user_custs1_def.h /.c`. If you are not familiar with custom profile declaration, please refer to [training material](https://www.dialog-semiconductor.com/sites/default/files/training_02_custom_profile_example_v1.2.pdf) from [support website](https://www.dialog-semiconductor.com/bluetooth-low-energy). Valid values of **Control Point** at:

- **Byte 0x00 : Hardfault** 
  - A hardfault is occurred by executing `*(uint32_t *)0x01 = 0x90;`
  - Once the hardfault is issued, the **HardFault_HandlerC** exception handler is triggered, located in `hardfault_handler.c`  
  - The **detect_rst_flag** value is assigned to **CUSTS1_HARDFAULT (=0x00)**
- **Byte 0x01 : NMI**
  - An endless loop will run, so that the WDOG will expire and an NMI will be fired.
  - Once the NMI is issued, the **NMI_HandlerC** exception handler is triggered, located in `nmi_handler.c`
  - The **detect_rst_flag** value is assigned to **CUSTS1_NMI (=0x01)**
- **Byte 0x02 : Software Reset** 
  - The **SYS_CTRL_REG** is used to trigger a *Software Reset* to the device
  - According to the datasheet, the SW reset is triggered by writing the **SYS_CTRL_REG[SW_RESET]** bit field.
  - The **detect_rst_flag** value is assigned to **CUSTS1_SW_RESET (=0x02)**

**Power-on-Reset (POR)** 
There is no dedicated register on the DA14585 that will allow us determine when a POR event is issued. However, it is of paramount importance to distinguish between the  production and actual operation of the device. This functionality, can be implemented using the *7-bit* **POR_TIMER_REG**. The default value is **0x18**. The above register retains its value between deep sleep, hardware and software resets and resets to the default value only after POR. To detect that device started operating following a POR event, application should probe **POR_TIMER_REG** looking for the default value and changing the value to a different one. 

**user_power_on_reset()** is used to explicitly cause POR upon system initialization. This API should be applied as soon as possible after the device boots. Both **POR_TIMER_REG** and **POR_PIN_REG*** are the only registers that will retain their values between resets and during any type of sleep modes and can be used to store application data. If the condition is true, then the value of the register is changed and **detect_rst_flag** is assigned to **CUSTS1_POR (=0x03)**. After POR, the device will start advertising again. **CFG_POWER_ON_RESET**, located in `user_custs1_impl.h`,  should be used to activate/deactivate POR. 

​   

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

 ![Pro-DK Jumper Placement](C:\Release_sw_examples_585\DA14585_detect_source_of_reset\assets\Board_setup.png)

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
3. Run it in **debug mode**. To learn basic debugging techniques, please refer to training material](https://www.dialog-semiconductor.com/sites/default/files/training_08_debugging_v1.1.pdf).
   ![Debug mode](C:\Release_sw_examples_585\DA14585_detect_source_of_reset\assets\Debug_mode.png)
4. Connect to  **DETECT-RESET** 
   ![ble_scanner_1](C:\Release_sw_examples_585\DA14585_detect_source_of_reset\assets\ble_scanner_1.png)
5. A custom service with the **Control Point** characteristic should be appeared.
   ![ble_scanner_2](C:\Release_sw_examples_585\DA14585_detect_source_of_reset\assets\ble_scanner_2.png)
6. Write **0x00** to trigger a hardfault
   ![ble_scanner_3](C:\Release_sw_examples_585\DA14585_detect_source_of_reset\assets\ble_scanner_3.png)
7. A hardfault is occured
   ![hardfault](C:\Release_sw_examples_585\DA14585_detect_source_of_reset\assets\hardfault.png)
8. Stop the debug procedure and reset the Pro-DK. detect_rst_flag** is stored into Retention-RAM, and so its value is retained.
9. Add a break point into ``user_app_adv_start()`` function and run it again in debug mode.
10. Step over (press F10) and the "Hardfault" is appended into the  advertising data. 
    ![hardfault_1](C:\Release_sw_examples_585\DA14585_detect_source_of_reset\assets\hardfault_1.png)
11. Stop the debug procedure
12. Open the Serial terminal (TerTerm) with the following COM port configurations
    ![comport](C:\Release_sw_examples_585\DA14585_detect_source_of_reset\assets\comport.png)
13. Download the firmware into System-RAM again and  the reason of the reset should be appeared on the monitor. 
    ![teraterm](C:\Release_sw_examples_585\DA14585_detect_source_of_reset\assets\teraterm.png)



## Known Limitations

- The sectors of the Keil are 4 bytes aligned, so the maximum uninitialized retained data that are required by the application (*CFG_RET_DATA_UNINIT_SIZE*) **must be multiple of 4  and greater or equal to the sum of the total data**. If the *CFG_RET_DATA_UNINIT_SIZE* is not multiple of 4, the Keil will truncate it into the smallest multiple of 4, otherwise if the data cannot be fit then the hole section will be deleted. 
- Refer to the following application note for [known hardware limitations](https://support.dialog-semiconductor.com/system/files/resources/DA1458x-KnownLimitations_2018_02_06.pdf "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software"Forum link").
- You can refer to Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.

## License

------

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

------