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

#include "arch.h"
#include "gpio.h"
#include "uart.h"
#include "da1458x_config_basic.h"
/*
 * DEFINES
 ****************************************************************************************
 */

/****************************************************************************************/
/* UART2 configuration to use with arch_console print messages                          */
/****************************************************************************************/
#if defined (__DA14531__)
// Define UART2 Tx Pad
#define UART2_TX_PORT           GPIO_PORT_0
#define UART2_TX_PIN            GPIO_PIN_5

#else
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_4
#endif

// Define UART2 Settings
#define UART2_BAUDRATE          UART_BAUDRATE_115200
#define UART2_DATABITS          UART_DATABITS_8
#define UART2_PARITY            UART_PARITY_NONE
#define UART2_STOPBITS          UART_STOPBITS_1
#define UART2_AFCE              UART_AFCE_DIS
#define UART2_FIFO              UART_FIFO_EN
#define UART2_TX_FIFO_LEVEL     UART_TX_FIFO_LEVEL_0
#define UART2_RX_FIFO_LEVEL     UART_RX_FIFO_LEVEL_0

/***************************************************************************************/
/* GPIO configuration to use with BMP388 SPI interface                                 */
/***************************************************************************************/
#if defined (__DA14531__)

#define BMP388_SPI_CLK_PORT     GPIO_PORT_0
#define BMP388_SPI_CLK_PIN      GPIO_PIN_4

#define BMP388_SPI_MOSI_PORT    GPIO_PORT_0
#define BMP388_SPI_MOSI_PIN     GPIO_PIN_0

#define BMP388_SPI_MISO_PORT    GPIO_PORT_0
#define BMP388_SPI_MISO_PIN     GPIO_PIN_3

#define BMP388_SPI_CS_PORT      GPIO_PORT_0
#define BMP388_SPI_CS_PIN       GPIO_PIN_7

#else

#define BMP388_SPI_CLK_PORT     GPIO_PORT_0
#define BMP388_SPI_CLK_PIN      GPIO_PIN_0 

#define BMP388_SPI_MOSI_PORT    GPIO_PORT_0
#define BMP388_SPI_MOSI_PIN     GPIO_PIN_6 

#define BMP388_SPI_MISO_PORT    GPIO_PORT_0
#define BMP388_SPI_MISO_PIN     GPIO_PIN_5 

#define BMP388_SPI_CS_PORT      GPIO_PORT_1
#define BMP388_SPI_CS_PIN       GPIO_PIN_3 

#endif

/***************************************************************************************/
/* GPIO configuration to use with BMP388 I2C interface                                 */
/***************************************************************************************/
#if defined (__DA14531__)
#define BMP388_I2C_SCL_PORT     GPIO_PORT_0
#define BMP388_I2C_SCL_PIN      GPIO_PIN_2
#define BMP388_I2C_SDA_PORT     GPIO_PORT_0
#define BMP388_I2C_SDA_PIN      GPIO_PIN_8
#else

#define BMP388_I2C_SCL_PORT     GPIO_PORT_0
#define BMP388_I2C_SCL_PIN      GPIO_PIN_7
#define BMP388_I2C_SDA_PORT     GPIO_PORT_0
#define BMP388_I2C_SDA_PIN      GPIO_PIN_4

#endif
/***************************************************************************************/
/* GPIO configuration to use with BMP388 INT pin                                       */
/***************************************************************************************/
#if defined (__DA14531__)

#define BMP388_INT_PORT         GPIO_PORT_0
#define BMP388_INT_PIN          GPIO_PIN_8 

#else

#define BMP388_INT_PORT         GPIO_PORT_1
#define BMP388_INT_PIN          GPIO_PIN_2

#endif

/***************************************************************************************/
/* Production debug output configuration                                               */
/***************************************************************************************/
#if PRODUCTION_DEBUG_OUTPUT
  #define PRODUCTION_DEBUG_PORT GPIO_PORT_0
  #define PRODUCTION_DEBUG_PIN  GPIO_PIN_11
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