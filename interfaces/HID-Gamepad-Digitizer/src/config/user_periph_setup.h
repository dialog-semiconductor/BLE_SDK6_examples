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

#include "gpio.h"
#include "uart.h"
#include "spi.h"
#include "spi_flash.h"
#include "i2c.h"
#include "i2c_eeprom.h"



/*
 * DEFINES
 ****************************************************************************************
 */

/****************************************************************************************/
/* UART2 configuration                                                                  */
/****************************************************************************************/
// Define UART2 Tx Pad
#if defined (__DA14531__)
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_6
#else
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_4
#endif


/****************************************************************************************/
/* Debug define                                                                         */
/****************************************************************************************/
// The DA14531 doesn't have a lot of pins. As one of the four ADC inputs is shared with SWD
// It's impossible to use SWD together with four analog inputs.
// If this is defined, both joysticks will share the same x axis input. And SWD is not disabled  
#define DEBUGGING

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
    #define GPIO_LED_PORT           GPIO_PORT_0
    #define GPIO_LED_PIN            GPIO_PIN_9
#else
    #define GPIO_LED_PORT           GPIO_PORT_1
    #define GPIO_LED_PIN            GPIO_PIN_0
#endif

/****************************************************************************************/
/* SPI configuration                                                                    */
/****************************************************************************************/
// Define SPI Pads
#if defined (__DA14531__)
    #define SPI_EN_PORT             GPIO_PORT_0
    #define SPI_EN_PIN              GPIO_PIN_1

    #define SPI_CLK_PORT            GPIO_PORT_0
    #define SPI_CLK_PIN             GPIO_PIN_4

    #define SPI_DO_PORT             GPIO_PORT_0
    #define SPI_DO_PIN              GPIO_PIN_0

    #define SPI_DI_PORT             GPIO_PORT_0
    #define SPI_DI_PIN              GPIO_PIN_3

#elif !defined (__DA14586__)
    #define SPI_EN_PORT             GPIO_PORT_2
    #define SPI_EN_PIN              GPIO_PIN_3

    #define SPI_CLK_PORT            GPIO_PORT_0
    #define SPI_CLK_PIN             GPIO_PIN_0

    #define SPI_DO_PORT             GPIO_PORT_0
    #define SPI_DO_PIN              GPIO_PIN_6

    #define SPI_DI_PORT             GPIO_PORT_0
    #define SPI_DI_PIN              GPIO_PIN_5
#endif

/****************************************************************************************/
/* Gamepad buttons configuration                                                        */
/****************************************************************************************/
#if defined (__DA14531__)
    #define GAMEPAD_BUTTON_A_PORT   GPIO_PORT_0
    #define GAMEPAD_BUTTON_A_PIN    GPIO_PIN_3
		#define GAMEPAD_BUTTON_B_PORT   GPIO_PORT_0
    #define GAMEPAD_BUTTON_B_PIN    GPIO_PIN_4
		#define GAMEPAD_BUTTON_STA_PORT GPIO_PORT_0
    #define GAMEPAD_BUTTON_STA_PIN  GPIO_PIN_11
		#define GAMEPAD_BUTTON_SEL_PORT GPIO_PORT_0
    #define GAMEPAD_BUTTON_SEL_PIN  GPIO_PIN_9
#else
    #define GAMEPAD_BUTTON_A_PORT   GPIO_PORT_2
    #define GAMEPAD_BUTTON_A_PIN    GPIO_PIN_0
		#define GAMEPAD_BUTTON_B_PORT   GPIO_PORT_2
    #define GAMEPAD_BUTTON_B_PIN    GPIO_PIN_1
		#define GAMEPAD_BUTTON_STA_PORT GPIO_PORT_1
    #define GAMEPAD_BUTTON_STA_PIN  GPIO_PIN_1
		#define GAMEPAD_BUTTON_SEL_PORT GPIO_PORT_0
    #define GAMEPAD_BUTTON_SEL_PIN  GPIO_PIN_6
#endif

/****************************************************************************************/
/* Gamepad joysticks configuration                                                      */
/****************************************************************************************/
#if defined (__DA14531__)
    #define GAMEPAD_LS_X_PORT   GPIO_PORT_0
    #define GAMEPAD_LS_X_PIN    GPIO_PIN_1
    #define GAMEPAD_LS_Y_PORT   GPIO_PORT_0
    #define GAMEPAD_LS_Y_PIN    GPIO_PIN_6
		#define GAMEPAD_RS_X_PORT   GPIO_PORT_0
#ifdef DEBUGGING
    #define GAMEPAD_RS_X_PIN    GPIO_PIN_1
#else
    #define GAMEPAD_RS_X_PIN    GPIO_PIN_2
#endif
		#define GAMEPAD_RS_Y_PORT   GPIO_PORT_0
    #define GAMEPAD_RS_Y_PIN    GPIO_PIN_7
#else
    #define GAMEPAD_LS_X_PORT   GPIO_PORT_0
    #define GAMEPAD_LS_X_PIN    GPIO_PIN_0
    #define GAMEPAD_LS_Y_PORT   GPIO_PORT_0
    #define GAMEPAD_LS_Y_PIN    GPIO_PIN_1
		#define GAMEPAD_RS_X_PORT   GPIO_PORT_0
    #define GAMEPAD_RS_X_PIN    GPIO_PIN_2
		#define GAMEPAD_RS_Y_PORT   GPIO_PORT_0
    #define GAMEPAD_RS_Y_PIN    GPIO_PIN_3
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
 * @return  void
 ****************************************************************************************
 */
void GPIO_reservations(void);
#endif

/**
 ****************************************************************************************
 * @brief   Sets the functionality of application pads
 * @details i.e. to set P0_1 as Generic purpose Output:
 *          GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
 * @return  void
 ****************************************************************************************
 */
void set_pad_functions(void);

/**
 ****************************************************************************************
 * @brief   Initializes application's peripherals and pins
 * @return  void
 ****************************************************************************************
 */
void periph_init(void);


#endif // _USER_PERIPH_SETUP_H_