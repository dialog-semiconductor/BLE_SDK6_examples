/**
 ****************************************************************************************
 *
 * @file user_periph_setup.h
 *
 * @brief Peripherals setup header file.
 *
 * Copyright (c) 2015-2021 Dialog Semiconductor
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */

#ifndef _USER_PERIPH_SETUP_H_
#define _USER_PERIPH_SETUP_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "arch.h"
#include "da1458x_periph_setup.h"
#include "i2c_eeprom.h"

/*
 * DEFINES
 ****************************************************************************************
 */

//*** <<< Use Configuration Wizard in Context Menu >>> ***

// <o> DK selection <0=> As in da1458x_periph_setup.h <1=> Basic <2=> Pro <3=> Expert
#define HW_CONFIG (2)

#define HW_CONFIG_BASIC_DK  ((HW_CONFIG==0 && SDK_CONFIG==1) || HW_CONFIG==1)
#define HW_CONFIG_PRO_DK    ((HW_CONFIG==0 && SDK_CONFIG==2) || HW_CONFIG==2)
#define HW_CONFIG_EXPERT_DK ((HW_CONFIG==0 && SDK_CONFIG==3) || HW_CONFIG==3)

//*** <<< end of configuration section >>>    ***

/****************************************************************************************/
/* I2C EEPROM configuration                                                             */
/****************************************************************************************/

#define I2C_EEPROM_SIZE   0x20000         // EEPROM size in bytes
#define I2C_EEPROM_PAGE   256             // EEPROM page size in bytes
#define I2C_SPEED_MODE    I2C_FAST        // 1: standard mode (100 kbits/s), 2: fast mode (400 kbits/s)
#define I2C_ADDRESS_MODE  I2C_7BIT_ADDR   // 0: 7-bit addressing, 1: 10-bit addressing
#define I2C_ADDRESS_SIZE  I2C_2BYTES_ADDR // 0: 8-bit memory address, 1: 16-bit memory address, 3: 24-bit memory address

/****************************************************************************************/
/* SPI FLASH configuration                                                              */
/****************************************************************************************/

#define SPI_FLASH_DEFAULT_SIZE  (256 * 1024)    // SPI Flash memory size in bytes
#define SPI_FLASH_DEFAULT_PAGE  256
#define SPI_SECTOR_SIZE         4096

#ifndef __DA14586__
    #define SPI_EN_GPIO_PORT    GPIO_PORT_0
    #define SPI_EN_GPIO_PIN     GPIO_PIN_3

    #define SPI_CLK_GPIO_PORT   GPIO_PORT_0
    #define SPI_CLK_GPIO_PIN    GPIO_PIN_0

    #define SPI_DO_GPIO_PORT    GPIO_PORT_0
    #define SPI_DO_GPIO_PIN     GPIO_PIN_6

    #define SPI_DI_GPIO_PORT    GPIO_PORT_0
#define SPI_DI_GPIO_PIN     GPIO_PIN_5
#else // DA14586
    #define SPI_EN_GPIO_PORT    GPIO_PORT_2
    #define SPI_EN_GPIO_PIN     GPIO_PIN_3

    #define SPI_CLK_GPIO_PORT   GPIO_PORT_2
    #define SPI_CLK_GPIO_PIN    GPIO_PIN_0

    #define SPI_DO_GPIO_PORT    GPIO_PORT_2
    #define SPI_DO_GPIO_PIN     GPIO_PIN_9

    #define SPI_DI_GPIO_PORT    GPIO_PORT_2
    #define SPI_DI_GPIO_PIN     GPIO_PIN_4
#endif

/****************************************************************************************/
/* UART2 pin configuration (debug print console)                                        */
/****************************************************************************************/

#ifdef CFG_PRINTF_UART2
    #if HW_CONFIG_BASIC_DK
        #define UART2_TX_GPIO_PORT  GPIO_PORT_2
        #define UART2_TX_GPIO_PIN   GPIO_PIN_6

        #define UART2_RX_GPIO_PORT  GPIO_PORT_2
        #define UART2_RX_GPIO_PIN   GPIO_PIN_7

    #elif HW_CONFIG_PRO_DK
        #define UART2_TX_GPIO_PORT  GPIO_PORT_2
        #define UART2_TX_GPIO_PIN   GPIO_PIN_6

        #define UART2_RX_GPIO_PORT  GPIO_PORT_2
        #define UART2_RX_GPIO_PIN   GPIO_PIN_7

    #elif HW_CONFIG_EXPERT_DK
        #define UART2_TX_GPIO_PORT  GPIO_PORT_0
        #define UART2_TX_GPIO_PIN   GPIO_PIN_4

        #define UART2_RX_GPIO_PORT  GPIO_PORT_0
        #define UART2_RX_GPIO_PIN   GPIO_PIN_5

    #else // (e.g. HW_CONFIG_USB_DONGLE)
        #define UART2_TX_GPIO_PORT  GPIO_PORT_0
        #define UART2_TX_GPIO_PIN   GPIO_PIN_4

        #define UART2_RX_GPIO_PORT  GPIO_PORT_0
        #define UART2_RX_GPIO_PIN   GPIO_PIN_5
    #endif
#endif

/****************************************************************************************/
/* LED configuration                                                                    */
/****************************************************************************************/

#if HW_CONFIG_BASIC_DK
    #define GPIO_LED_PORT     GPIO_PORT_1
    #define GPIO_LED_PIN      GPIO_PIN_0

#elif HW_CONFIG_PRO_DK
    #define GPIO_LED_PORT     GPIO_PORT_1
    #define GPIO_LED_PIN      GPIO_PIN_0

#elif HW_CONFIG_EXPERT_DK
    #define GPIO_LED_PORT     GPIO_PORT_1
    #define GPIO_LED_PIN      GPIO_PIN_0

#else // (other configuration)
#endif

/***************************************************************************************/
/* Production debug output configuration                                               */
/***************************************************************************************/
#if PRODUCTION_DEBUG_OUTPUT
    #define PRODUCTION_DEBUG_PORT GPIO_PORT_2
    #define PRODUCTION_DEBUG_PIN GPIO_PIN_5
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Enable pad and peripheral clocks assuming that peripheral power domain is down.
 *        The UART and SPI clocks are set.
 * @return void
 ****************************************************************************************
 */
void periph_init(void);

/**
 ****************************************************************************************
 * @brief Map port pins. The UART and SPI port pins and GPIO ports are mapped.
 * @return void
 ****************************************************************************************
 */
void set_pad_functions(void);

/**
 ****************************************************************************************
 * @brief Each application reserves its own GPIOs here.
 * @return void
 ****************************************************************************************
 */
void GPIO_reservations(void);

#endif // _USER_PERIPH_SETUP_H_
