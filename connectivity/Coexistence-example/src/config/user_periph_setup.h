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


/*
 * DEFINES
 ****************************************************************************************
 */
 
 /****************************************************************************************/
/* WLAN COEX pin configuration                                                          */
/****************************************************************************************/

#if (WLAN_COEX_ENABLED)
#if defined (__DA14531__)

   /// Input signal to device: 2.4GHz external device event in progress indication.
   #define WLAN_COEX_24G_EIP_PORT      GPIO_PORT_0
   #define WLAN_COEX_24G_EIP_PIN       GPIO_PIN_5

   /// Output signal from device: BLE event in progress indication.
 #define WLAN_COEX_BLE_EIP_PORT      GPIO_PORT_0
 #define WLAN_COEX_BLE_EIP_PIN       GPIO_PIN_6

   /// Output signal from device: BLE priority indication.
   #define WLAN_COEX_BLE_PRIO_PORT     GPIO_PORT_0
   #define WLAN_COEX_BLE_PRIO_PIN      GPIO_PIN_7

#if defined (CFG_WLAN_COEX_DEBUG)
   /// BLE radio overruled signal pin definition.
   /// This signal goes high when the BLE radio is forced to be off due to external 2.4GHz device activity.
   #define WLAN_COEX_DEBUG_A_PORT      GPIO_PORT_0
   #define WLAN_COEX_DEBUG_A_PIN       GPIO_PIN_9

   /// External 2.4GHz device EIP handler signal pin definition.
   /// This signal indicates when an external 2.4GHz device wants to start or stop sending data.
   #define WLAN_COEX_DEBUG_B_PORT      GPIO_PORT_0
   #define WLAN_COEX_DEBUG_B_PIN       GPIO_PIN_8
#endif

#else

   /// Input signal to device: 2.4GHz external device event in progress indication.
   #define WLAN_COEX_24G_EIP_PORT      GPIO_PORT_0
   #define WLAN_COEX_24G_EIP_PIN       GPIO_PIN_0

   /// Output signal from device: BLE event in progress indication.
   #define WLAN_COEX_BLE_EIP_PORT      GPIO_PORT_0
   #define WLAN_COEX_BLE_EIP_PIN       GPIO_PIN_3

   /// Output signal from device: BLE priority indication.
   #define WLAN_COEX_BLE_PRIO_PORT     GPIO_PORT_0
   #define WLAN_COEX_BLE_PRIO_PIN      GPIO_PIN_2

#if defined (CFG_WLAN_COEX_DEBUG)
   /// BLE radio overruled signal pin definition.
   //  This signal goes high when the BLE radio is forced to be off due to external 2.4GHz device activity.
   #define WLAN_COEX_DEBUG_A_PORT      GPIO_PORT_0
   #define WLAN_COEX_DEBUG_A_PIN       GPIO_PIN_1

   /// External 2.4GHz device EIP handler signal pin definition.
   /// This signal indicates when an external 2.4GHz device wants to start or stop sending data.
   #define WLAN_COEX_DEBUG_B_PORT      GPIO_PORT_1
   #define WLAN_COEX_DEBUG_B_PIN       GPIO_PIN_3
#endif

#endif

// GPIO IRQ number. Interrupt is fired when 2.4GHz external device event in progress signal is activated.
#define WLAN_COEX_IRQ            4

#endif // WLAN_COEX_ENABLED


/****************************************************************************************/
/* UART2 configuration to use with arch_console print messages                          */
/****************************************************************************************/
// Define UART2 Tx Pad
#if defined (__DA14531__)
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_6
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