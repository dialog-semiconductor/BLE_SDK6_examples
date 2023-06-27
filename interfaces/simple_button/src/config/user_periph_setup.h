/**
 ****************************************************************************************
 *
 * @file user_periph_setup.h
 *
 * @brief Peripherals setup header file.
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

#ifndef _USER_PERIPH_SETUP_H_
#define _USER_PERIPH_SETUP_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "uart.h"
#include "gpio.h"

/****************************************************************************************/
/* UART2 configuration to print messages                                                */
/****************************************************************************************/
#define UART                        UART2

#if defined (__DA14531__)
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_1
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
/* LED configuration                                                                    */
/****************************************************************************************/
#if defined (__DA14531__)
    #define LED_PORT                GPIO_PORT_0
    #define LED_PIN                 GPIO_PIN_9
#else
    #define LED_PORT                GPIO_PORT_1
    #define LED_PIN                 GPIO_PIN_0
#endif
	
	#if defined (__DA14531__)
		#define GPIO_SW_PORT			 GPIO_PORT_0 
		#define GPIO_SW_PIN			 	 GPIO_PIN_11
#else
		#define GPIO_SW_PORT			 GPIO_PORT_1 
		#define GPIO_SW_PIN			 	 GPIO_PIN_1 
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief   Initializes application's peripherals and pins.
 * @return  void
 ****************************************************************************************
 */
void periph_init(void);

/**
****************************************************************************************
* @brief	Deinitializes application's peripherals and pins. For future use.
* @return	void
****************************************************************************************
*/
void periph_deinit(void);

#endif // _USER_PERIPH_SETUP_H_