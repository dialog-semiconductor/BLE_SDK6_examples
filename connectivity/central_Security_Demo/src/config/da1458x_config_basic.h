/**
 ****************************************************************************************
 *
 * @file da1458x_config_basic.h
 *
 * @brief Basic compile configuration file.
 *
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
 */

#ifndef _DA1458X_CONFIG_BASIC_H_
#define _DA1458X_CONFIG_BASIC_H_

#include "da1458x_stack_config.h"
#include "user_profiles_config.h"

#if !defined (__DA14531__)

/***************************************************************************************************************/
/* Integrated or external processor configuration                                                              */
/*    -defined      Integrated processor mode. Host application runs in DA14585 processor. Host application    */
/*                  is the TASK_APP kernel task.                                                               */
/*    -undefined    External processor mode. Host application runs on an external processor. Communicates with */
/*                  BLE application through GTL protocol over a signalling iface (UART, SPI etc)               */
/***************************************************************************************************************/
#define CFG_APP

/****************************************************************************************************************/
/* Enables the BLE security functionality in TASK_APP. If not defined BLE security related code is compiled out.*/
/****************************************************************************************************************/
#define CFG_APP_SECURITY

/****************************************************************************************************************/
/* Enables WatchDog timer.                                                                                      */
/****************************************************************************************************************/
#define CFG_WDOG

/****************************************************************************************************************/
/* Watchdog timer behavior in production mode:                                                                  */
/*     Flag is not defined: Watchdog timer generates NMI at value 0.                                            */
/*     Flag is defined    : Watchdog timer generates a WDOG (SYS) reset at value 0.                             */
/****************************************************************************************************************/
#undef CFG_WDG_TRIGGER_HW_RESET_IN_PRODUCTION_MODE

/****************************************************************************************************************/
/* Determines maximum concurrent connections supported by application. It configures the heap memory allocated  */
/* to service multiple connections. It is used for GAP central role applications. For GAP peripheral role it    */
/* should be set to 1 for optimizing memory utilization.                                                        */
/*      - MAX value for DA14585: 8                                                                              */
/****************************************************************************************************************/
#define CFG_MAX_CONNECTIONS     (1)

/****************************************************************************************************************/
/* Enables development/debug mode. For production mode builds it must be disabled.                              */
/* When enabled the following debugging features are enabled                                                    */
/*      -   Allows the emulation of the OTP mirroring to System RAM. No actual writing to RAM is done, but the  */
/*          exact same amount of time is spend as if the mirroring would take place. This is to mimic the       */
/*          behavior as if the System Code is already in OTP, and the mirroring takes place after waking up,    */
/*          but the (development) code still resides in an external source.                                     */
/*      -   Validation of GPIO reservations.                                                                    */
/*      -   Enables Debug module and sets code execution in breakpoint in Hardfault and NMI (Watchdog) handlers.*/
/*          It allows developer to hot attach debugger and get debug information                                */
/****************************************************************************************************************/
#define CFG_DEVELOPMENT_DEBUG

/****************************************************************************************************************/
/* UART Console Print. If CFG_PRINTF is defined, serial interface logging mechanism will be enabled.            */
/* If CFG_PRINTF_UART2 is defined, then serial interface logging mechanism is implented using UART2, else UART1 */
/* will be used.                                                                                                */
/****************************************************************************************************************/
#define CFG_PRINTF
#ifdef CFG_PRINTF
    #define CFG_PRINTF_UART2
#endif

/****************************************************************************************************************/
/* UART1 Driver Implementation. If CFG_UART1_SDK is defined, UART1 ROM driver will be overriden and UART SDK    */
/* driver will be used, else ROM driver will be used for UART1 module.                                          */
/****************************************************************************************************************/
#undef CFG_UART1_SDK


/****************************************************************************************************************/
/* Select external memory device for data storage                                                               */
/* SPI FLASH  (#define CFG_SPI_FLASH_ENABLE)                                                                    */
/* I2C EEPROM (#define CFG_I2C_EEPROM_ENABLE)                                                                   */
/****************************************************************************************************************/
#define CFG_SPI_FLASH_ENABLE
#undef CFG_I2C_EEPROM_ENABLE

/****************************************************************************************************************/
/* Enables/Disables the DMA Support for the following interfaces:                                               */
/*     - UART                                                                                                   */
/*     - SPI                                                                                                    */
/*     - I2C                                                                                                    */
/****************************************************************************************************************/
#undef CFG_UART_DMA_SUPPORT
#undef CFG_SPI_DMA_SUPPORT
#undef CFG_I2C_DMA_SUPPORT



#else



/***************************************************************************************************************/
/* Integrated or external processor configuration                                                              */
/*    -defined      Integrated processor mode. Host application runs in DA14585 processor. Host application    */
/*                  is the TASK_APP kernel task.                                                               */
/*    -undefined    External processor mode. Host application runs on an external processor. Communicates with */
/*                  BLE application through GTL protocol over a signalling iface (UART, SPI etc)               */
/***************************************************************************************************************/
#define CFG_APP

/****************************************************************************************************************/
/* Enables the BLE security functionality in TASK_APP. If not defined BLE security related code is compiled out.*/
/****************************************************************************************************************/
#define CFG_APP_SECURITY

/****************************************************************************************************************/
/* Enables WatchDog timer.                                                                                      */
/****************************************************************************************************************/
#define CFG_WDOG

/****************************************************************************************************************/
/* Watchdog timer behavior in production mode:                                                                  */
/*     Flag is not defined: Watchdog timer generates NMI at value 0.                                            */
/*     Flag is defined    : Watchdog timer generates a WDOG (SYS) reset at value 0.                             */
/****************************************************************************************************************/
#undef CFG_WDG_TRIGGER_HW_RESET_IN_PRODUCTION_MODE

/****************************************************************************************************************/
/* Determines maximum concurrent connections supported by application. It configures the heap memory allocated  */
/* to service multiple connections. It is used for GAP central role applications. For GAP peripheral role it    */
/* should be set to 1 for optimizing memory utilization.                                                        */
/*      - MAX value for DA14585: 8                                                                              */
/****************************************************************************************************************/
#define CFG_MAX_CONNECTIONS     (1)

/****************************************************************************************************************/
/* Enables development/debug mode. For production mode builds it must be disabled.                              */
/* When enabled the following debugging features are enabled                                                    */
/*      -   Allows the emulation of the OTP mirroring to System RAM. No actual writing to RAM is done, but the  */
/*          exact same amount of time is spend as if the mirroring would take place. This is to mimic the       */
/*          behavior as if the System Code is already in OTP, and the mirroring takes place after waking up,    */
/*          but the (development) code still resides in an external source.                                     */
/*      -   Validation of GPIO reservations.                                                                    */
/*      -   Enables Debug module and sets code execution in breakpoint in Hardfault and NMI (Watchdog) handlers.*/
/*          It allows developer to hot attach debugger and get debug information                                */
/****************************************************************************************************************/
#define CFG_DEVELOPMENT_DEBUG

/****************************************************************************************************************/
/* UART Console Print. If CFG_PRINTF is defined, serial interface logging mechanism will be enabled.            */
/* If CFG_PRINTF_UART2 is defined, then serial interface logging mechanism is implented using UART2, else UART1 */
/* will be used.                                                                                                */
/****************************************************************************************************************/
#define CFG_PRINTF
#ifdef CFG_PRINTF
    #define CFG_PRINTF_UART2
#endif

/****************************************************************************************************************/
/* UART1 Driver Implementation. If CFG_UART1_SDK is defined, UART1 ROM driver will be overriden and UART SDK    */
/* driver will be used, else ROM driver will be used for UART1 module.                                          */
/****************************************************************************************************************/
#undef CFG_UART1_SDK


/****************************************************************************************************************/
/* Select external memory device for data storage                                                               */
/* SPI FLASH  (#define CFG_SPI_FLASH_ENABLE)                                                                    */
/* I2C EEPROM (#define CFG_I2C_EEPROM_ENABLE)                                                                   */
/****************************************************************************************************************/
#define CFG_SPI_FLASH_ENABLE
#undef CFG_I2C_EEPROM_ENABLE

/****************************************************************************************************************/
/* Enables the SPI flash memory extensions feature. When enabled, the DA14531 controls, through a dedicated     */
/* GPIO, the activation/deactivation of the SPI flash memory device. This feature is mainly used when the       */
/* DA14531 is in boost mode and the SPI flash memory is supplied by VBAT_H (VBAT_L is the input voltage and the */
/* DCDC converter generates a programmable output voltage at VBAT_H). In boost mode the DCDC converter has a    */
/* limit of 50uA in VBAT_H during the DA14531 power-up. Due to this limitation some SPI flash memories may fail */
/* to start-up for certain values of VBAT_L. Therefore, their power voltage must be externally controlled using */
/* the SPI flash memory extensions feature. Refer to SDK6 documentation for further details.                    */
/****************************************************************************************************************/
#undef CFG_SPI_FLASH_EXTENSIONS

/****************************************************************************************************************/
/* The GPIO pin may be directly connected to a capacitor. If the pin status changes, the capacitor may be       */
/* discharged abruptly through the GPIO pin. If the macro is defined, the pull-down resistor will be enabled    */
/* first before enabling the pin push-pull operation.                                                           */
/* Note: The feature is used in the CFG_SPI_FLASH_EXTENSIONS context when the dedicated pin is connected        */
/* directly to the SPI flash memory voltage input.                                                              */
/****************************************************************************************************************/
#define CFG_SPI_FLASH_CAP_SLOW_DISCHARGE

/****************************************************************************************************************/
/* The GPIO pin may be directly connected to a capacitor. If the pin status changes abruptly, the GPIO pin may  */
/* be stressed since a large current may flow through the pin to charge the capacitor. Therefore, initially the */
/* internal pull-up resistor will be enabled.                                                                   */
/* Note: The feature is used in the CFG_SPI_FLASH_EXTENSIONS context when the dedicated pin is connected        */
/* directly to the SPI flash memory voltage input.                                                              */
/****************************************************************************************************************/
#define CFG_SPI_FLASH_CAP_SLOW_CHARGE

/****************************************************************************************************************/
/* Enables/Disables the DMA Support for the following interfaces:                                               */
/*     - UART                                                                                                   */
/*     - SPI                                                                                                    */
/*     - I2C                                                                                                    */
/*     - ADC                                                                                                    */
/****************************************************************************************************************/
#undef CFG_UART_DMA_SUPPORT
#undef CFG_SPI_DMA_SUPPORT
#undef CFG_I2C_DMA_SUPPORT
#undef CFG_ADC_DMA_SUPPORT

/****************************************************************************************************************/
/* Notify the SDK about the fixed power mode (currently used only for Bypass):                                  */
/*     - CFG_POWER_MODE_BYPASS = Bypass mode                                                                    */
/****************************************************************************************************************/
#undef CFG_POWER_MODE_BYPASS

#endif

/****************************************************************************************************************/
/* Enable extra wait time after release from Ultra Deep Power-Down (UDPD). Mainly required for the              */
/* Adesto/Dialog SPI flash memories which support the UDPD command.                                             */
/****************************************************************************************************************/
#undef CFG_SPI_FLASH_ADESTO_UDPD

/****************************************************************************************************************/
/* Control the memory protection feature of the SPI flash chip through its Status Register 1.                   */
/****************************************************************************************************************/
#undef CFG_SPI_FLASH_MEM_PROTECT_USING_STATUS_REG1

#endif // _DA1458X_CONFIG_BASIC_H_