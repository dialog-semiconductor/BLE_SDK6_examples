/**
 ****************************************************************************************
 *
 * @file da1458x_config_basic.h
 *
 * @brief Basic compile configuration file.
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
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
#if defined (REMOTE_MODE) || defined (BATCH_REMOTE_MODE)
#define CFG_APP
#else
#undef CFG_APP
#endif

/****************************************************************************************************************/
/* Enables the BLE security functionality in TASK_APP. If not defined BLE security related code is compiled out.*/
/****************************************************************************************************************/
#undef CFG_APP_SECURITY

/****************************************************************************************************************/
/* Enables WatchDog timer.                                                                                      */
/****************************************************************************************************************/
#undef CFG_WDOG

/****************************************************************************************************************/
/* Watchdog timer behavior in production mode:                                                                  */
/*     Flag is not defined: Watchdog timer generates NMI at value 0.                                            */
/*     Flag is defined    : Watchdog timer generates a WDOG (SYS) reset at value 0.                             */
/****************************************************************************************************************/
#undef CFG_WDG_TRIGGER_HW_RESET_IN_PRODUCTION_MODE

/****************************************************************************************************************/
/* Must be defined for any project that uses the HCI.                                                           */
/****************************************************************************************************************/
#define CFG_HCI

/****************************************************************************************************************/
/* Enables vendor specific HCI command support.                                                                  */
/****************************************************************************************************************/
#define CFG_DBG_TASK

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
#undef CFG_PRINTF
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
#undef CFG_SPI_FLASH_ENABLE
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

#if defined (STANDALONE_MODE) || defined (BATCH_REMOTE_MODE)

#if defined (BATCH_REMOTE_MODE)
    #define STANDALONE_START_IMMEDIATELY
#endif // BATCH_REMOTE_MODE
    #define STANDALONE_UNMOD_TX_TEST        // Unmodulated Tx test
    #define STANDALONE_UNMOD_RX_TEST        // Unmodulated Rx test
    #define STANDALONE_LE_TX_TEST           // BLE packet Tx test
    #define STANDALONE_LE_RX_TEST           // BLE packet Rx test
    #define STANDALONE_CONT_MOD_TX_TEST     // Modulated Tx test
    #define STANDALONE_GO_TO_SLEEP_TEST     // Goto sleep mode

    #define LOW_CHANNEL  0      // 2402 MHz (BLE channel 37)
    #define MID_CHANNEL  12     // 2426 MHz (BLE channel 38)
    #define HIGH_CHANNEL 39     // 2480 MHz (BLE channel 39)

    #if defined(STANDALONE_CONT_MOD_TX_TEST) || defined(STANDALONE_LE_TX_TEST)
        #define PAYLOAD_TYPE     PAYL_PSEUDO_RAND_9     // Payload types are defined in co_bt.h
    #endif
	
    #ifdef STANDALONE_LE_TX_TEST
        #define PAYLOAD_LENGTH   37                     // Payload length for STANDALONE_LE_TX_TEST
    #endif
	
#endif // STANDALONE_MODE || BATCH_REMOTE_MODE

/**************************************************************************************************/
/* Amount of time each test will last */
/**************************************************************************************************/
#if defined (BATCH_REMOTE_MODE) || defined (REMOTE_MODE)
#define END_OF_TEST_TIME		500
#endif

#else


/***************************************************************************************************************/
/* Integrated or external processor configuration                                                              */
/*    -defined      Integrated processor mode. Host application runs in DA14585 processor. Host application    */
/*                  is the TASK_APP kernel task.                                                               */
/*    -undefined    External processor mode. Host application runs on an external processor. Communicates with */
/*                  BLE application through GTL protocol over a signalling iface (UART, SPI etc)               */
/***************************************************************************************************************/
#if defined (REMOTE_MODE) || defined (BATCH_REMOTE_MODE)
#define CFG_APP
#else
#undef CFG_APP
#endif

/****************************************************************************************************************/
/* Enables WatchDog timer.                                                                                      */
/****************************************************************************************************************/
#undef CFG_WDOG

/****************************************************************************************************************/
/* Watchdog timer behavior in production mode:                                                                  */
/*     Flag is not defined: Watchdog timer generates NMI at value 0.                                            */
/*     Flag is defined    : Watchdog timer generates a WDOG (SYS) reset at value 0.                             */
/****************************************************************************************************************/
#undef CFG_WDG_TRIGGER_HW_RESET_IN_PRODUCTION_MODE

/****************************************************************************************************************/
/* Must be defined for any project that uses the HCI.                                                           */
/****************************************************************************************************************/
#define CFG_HCI

/****************************************************************************************************************/
/* Enables vendor specific HCI command support.                                                                  */
/****************************************************************************************************************/
#define CFG_DBG_TASK

/****************************************************************************************************************/
/* Determines maximum concurrent connections supported by application. It configures the heap memory allocated  */
/* to service multiple connections. It is used for GAP central role applications. For GAP peripheral role it    */
/* should be set to 1 for optimizing memory utilization.                                                        */
/*      - MAX value for DA14531: 3                                                                              */
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
#undef CFG_PRINTF
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
#undef CFG_SPI_FLASH_ENABLE
#undef CFG_I2C_EEPROM_ENABLE

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

#if defined (STANDALONE_MODE) || defined (BATCH_REMOTE_MODE)

#if defined (BATCH_REMOTE_MODE)
    #define STANDALONE_START_IMMEDIATELY    // Start first test immediately after reset
#else
	#undef STANDALONE_START_IMMEDIATELY		// if not defined in STANDALONE device will go directly to sleep
#endif // BATCH_REMOTE_MODE

    #define STANDALONE_UNMOD_TX_TEST        // Unmodulated Tx test
    #define STANDALONE_UNMOD_RX_TEST        // Unmodulated Rx test
    #define STANDALONE_LE_TX_TEST           // BLE packet Tx test
    #define STANDALONE_LE_RX_TEST           // BLE packet Rx test
    #define STANDALONE_CONT_MOD_TX_TEST     // Modulated Tx test
    #define STANDALONE_GO_TO_SLEEP_TEST     // Goto sleep mode

    #define LOW_CHANNEL  0      // 2402 MHz (BLE channel 37)
    #define MID_CHANNEL  12     // 2426 MHz (BLE channel 38)
    #define HIGH_CHANNEL 39     // 2480 MHz (BLE channel 39)

    #if defined(STANDALONE_CONT_MOD_TX_TEST) || defined(STANDALONE_LE_TX_TEST)
        #define PAYLOAD_TYPE     PAYL_PSEUDO_RAND_9     // Payload types are defined in co_bt.h
    #endif
	
    #ifdef STANDALONE_LE_TX_TEST
        #define PAYLOAD_LENGTH   37                     // Payload length for STANDALONE_LE_TX_TEST
    #endif
	
#endif // STANDALONE_MODE || BATCH_REMOTE_MODE

/****************************************************************************************************************/
/* Default Tx power level                                                                                       */
/****************************************************************************************************************/
#define RADIO_INIT_TX_POWER RF_TX_PWR_LVL_0d0     // Tx power levels are defined in rf_531.h

/**************************************************************************************************/
/* Amount of time each test will last */
/**************************************************************************************************/
#if defined (BATCH_REMOTE_MODE) || defined (REMOTE_MODE)
#define END_OF_TEST_TIME		500
#endif

#endif

#endif // _DA1458X_CONFIG_BASIC_H_
