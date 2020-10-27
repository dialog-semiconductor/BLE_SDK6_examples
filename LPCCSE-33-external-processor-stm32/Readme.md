
# Booting the DA14531 with Codeless through a STM32

---

## Example description

The DA14531 has an integrated ARM Cortex M0+ and can be used for programming and controlling the SoC. Due to the small form factor the ARM Cortex has its limitations. A solution for the limitations of the integrated microprocessor can be to use an external microprocessor. To control the DA14531 with this microprocessor some code is still needed on the DA14531. Fortunately the DA14531 can execute code from RAM that can be loaded in during its bootsequence. This way a microcontroller can load a program onto the DA14531.

The goal of this example is to show how to load a program into the RAM of the DA14531 via a STM32 microcontroller. This example shows the flow of the code and how it can be configured. The program that is booted on the DA14531 in this example is called Codeless. The interface that is used for booting Codeless is 1 wire UART. Codeless is a solution by Dialog to interface with the DA14531 with AT commands. More info on Codeless and its use can be found on [here](https://www.dialog-semiconductor.com/products/smartbond-codeless-commands). In this example it will only be used to verify that the program has loaded correctly.

## HW and SW configuration

### Required hardware

- DA14531 daughter board + DA145xxDEVKT-P PRO-Motherboard.
- A pc workstation
- Clicker 2 for STM32 Developmentboard by MikroE [(Link to Clicker 2 for STM32)](https://www.mikroe.com/clicker-2-stm32f4)
- J-Link SEGGER

The user manual for the development kits can be found:

- [Here](https://www.dialog-semiconductor.com/products/da14531-development-kit-pro) for the DA145xxDEVKT-P PRO-Motherboard.

### General Hardware configuration using the DA145xxDEVKT-P PRO-Motherboard and Clicker 2 for STM32

- Connect the DA145xxDEVKT-P PRO-Motherboard to the work station through USB1 connector (only needed for power).
- Connect the headers on the PRO-Motherboard as on the image below
- Connect the J-Link Segger to the Clicker 2 for STM32 J-TAG header using the ARM-debug wiring
- Connect the Clicker 2 for STM32 to the PRO-Motherboard:
  * Clicker 2 pins PD8 and PD9 together with a 1k resistor (to create a 1 wire UART interface)
  * Clicker 2 pin PD9 to pin P25 on the PRO-Motherboard
  * Clicker 2 pin PA3 to pin P20 on the PRO-Motherboard
  * Add a 10k pullup resistor to pin P23 on the PRO-Motherboard

![Clicker_2_for_STM32_to_DA14531_Connections](assets/uart.png)

### Software configuration

**This example requires:**

- Keil 5
- **SEGGER’s J-Link** tools should be downloaded and installed.
- BLE scanner for your smartphone (in this example BLE Scanner for Android is used which can be found [here](https://play.google.com/store/apps/details?id=com.macdom.ble.blescanner&hl=nl&gl=US))


**Flow of the code**
![code_flow_of_boot](assets/boot_flow.png)

**Running the code**
Open the Keil project file located in **project environment** folder and once it has opened click the **Options for target...** Button. In this screen goto Debug and set the debugger to **J-LINK / J-TRACE Cortex**
after that click on the **Settings** Button. Within this screen select the J-link module that is connected and set the **Port:** to **SW**. Save these settings.

![CubeMX_debug](assets/debug.png)
![CubeMX_debug_settings](assets/debug_settings.png)

After all the settings have been set the project can be build. Press the **Build** button or the **F7** key to start building the project.
When the build is done press the **Start/Stop Debug Session** button or the key combination **Ctrl + F5** to load to program into the STM32.

![Keil_compile](assets/compile.png)

In the debug screen press the **Run** button or the **F5** key to start the program and the boot process should start.

![Keil_run](assets/run.png)

## Expected Result

After about 5 seconds when the run button is pressed the DA14531 should start advertising itself as **CLv2**. Down below a screenshot can be seen from BLE scanner for Android after the boot was succesful.

![BLE_result](assets/result.png)

## Code settings

There are settings within the code that can be changed if needed. The first one being which interface to use,
either 1 wire UART or 2 wire UART can be chosen as a parameter of the boot functions.

![keil_one_wire_uart](assets/uart_config.png)

The second one being, which version of Codeless to boot the DA14531 with. This settings can be found in the **codeless_config.h** file. At the moment the three versions of Codeless are **Standalones 1, Standalone 2 and Datapump**. More detailed info on each version of Codesless can be found [here](http://lpccs-docs.dialog-semiconductor.com/UM-140-DA145x-CodeLess/atcommands.html).The other settings in this file are the attempt amount to boot and the timeout time of uart communication. The boot attempts are the amount of tries the STM32 takes to boot codeless. The timeout time specifies the amount of time in milliseconds how long the STM32 waits for a response from the DA14531.

![keil_boot_config](assets/boot.png)

The project is setup using STM32CubeMX and the .ioc is provided with the project so changes can be made should they be necesarry for the user. The settings used in STM32CubeMX are as follows:

![CubeMX_pinout](assets/pinout.png)

## Known Limitations

- Refer to the following application note for DA14531 known [hardware limitations](https://www.dialog-semiconductor.com/da14531_HW_Limitation)
- Also refer to Dialog Software [Forum Link](https://support.dialog-semiconductor.com/forum)

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
