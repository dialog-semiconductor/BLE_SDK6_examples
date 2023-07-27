/**
 ****************************************************************************************
 *
 * @file user_periph_setup.h
 *
 * @brief Peripherals setup header file.
 *
 * Copyright (C) 2015-2023 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

#ifndef _USER_PERIPH_SETUP_H_
#define _USER_PERIPH_SETUP_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "arch.h"
#include "gpio.h"
#include "uart.h"


/*
 * DEFINES
 ****************************************************************************************
 */
#if defined (__DA14531__)
    #define LED_PORT      GPIO_PORT_0
    #define LED_PIN       GPIO_PIN_9
#else
    #define LED_PORT      GPIO_PORT_1
    #define LED_PIN       GPIO_PIN_0
#endif

/****************************************************************************************/
/* UART2 configuration to use with arch_console print messages                          */
/****************************************************************************************/
// Define UART2 Tx Pad
// Define UART2 Tx Pad
#if defined (__DA14531__)
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_5
#else
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_4
#endif
// Define UART2 Settings
#define UART2_BAUDRATE              UART_BAUDRATE_115200
#define UART2_DATABITS              UART_DATABITS_8
#define UART2_PARITY                UART_PARITY_NONE
#define UART2_STOPBITS              UART_STOPBITS_1
#define UART2_AFCE                  UART_AFCE_DIS
#define UART2_FIFO                  UART_FIFO_EN
#define UART2_TX_FIFO_LEVEL         UART_TX_FIFO_LEVEL_0
#define UART2_RX_FIFO_LEVEL         UART_RX_FIFO_LEVEL_0
/****************************************************************************************/
/* I2C configuration                                                                  	*/
/****************************************************************************************/
#define I2C_SLAVE_ADDRESS           0x44                  // Set slave device address
#define I2C_SPEED_MODE              I2C_SPEED_FAST        // Speed mode: 			I2C_SPEED_FAST (400 kbits/s)
#define I2C_ADDRESS_MODE            I2C_ADDRESSING_7B     // Addressing mode: I2C_ADDRESSING_7B
#define I2C_ADDRESS_SIZE            I2C_1BYTES_ADDR       // Address width: 	I2C_1BYTE_ADDR 

// Define I2C Pins
#if defined (__DA14531__)
#define HS3001_SCL_PORT            GPIO_PORT_0
#define HS3001_SCL_PIN	           GPIO_PIN_6
#define HS3001_SDA_PORT            GPIO_PORT_0
#define HS3001_SDA_PIN             GPIO_PIN_7
	#else
#define HS3001_SCL_PORT            GPIO_PORT_0
#define HS3001_SCL_PIN	            GPIO_PIN_7
#define HS3001_SDA_PORT            GPIO_PORT_0
#define HS3001_SDA_PIN             GPIO_PIN_4
#endif


/***************************************************************************************/
/* Production debug output configuration                                               */
/***************************************************************************************/
#if PRODUCTION_DEBUG_OUTPUT
#if defined (__DA14531__)
    #define PRODUCTION_DEBUG_PORT   GPIO_PORT_0
    #define PRODUCTION_DEBUG_PIN    GPIO_PIN_11
#else
    #define PRODUCTION_DEBUG_PORT   GPIO_PORT_2
    #define PRODUCTION_DEBUG_PIN    GPIO_PIN_5
#endif
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */



#if DEVELOPMENT_DEBUG
/**
 ****************************************************************************************
 * @brief   Reserves application's specific GPIOs
 * @details Used only in Development mode (#if DEVELOPMENT_DEBUG)
 *          i.e. to reserve P0_1 as Generic Purpose I/O:
 *          RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
 ****************************************************************************************
 */
void GPIO_reservations(void);
#endif

/**
 ****************************************************************************************
 * @brief   Sets the functionality of application pads
 * @details i.e. to set P0_1 as Generic purpose Output:
 *          GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
 ****************************************************************************************
 */
void set_pad_functions(void);

/**
 ****************************************************************************************
 * @brief   Initializes application's peripherals and pins
 ****************************************************************************************
 */
void periph_init(void);


#endif // _USER_PERIPH_SETUP_H_
