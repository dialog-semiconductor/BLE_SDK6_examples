/**
 ****************************************************************************************
 *
 * @file user_periph_setup.h
 *
 * @brief Peripherals setup header file.
 *
 * Copyright (C) 2015-2023 Renesas Electronics Corporation and/or its affiliates.
 * All rights reserved. Confidential Information.
 *
 * This software ("Software") is supplied by Renesas Electronics Corporation and/or its
 * affiliates ("Renesas"). Renesas grants you a personal, non-exclusive, non-transferable,
 * revocable, non-sub-licensable right and license to use the Software, solely if used in
 * or together with Renesas products. You may make copies of this Software, provided this
 * copyright notice and disclaimer ("Notice") is included in all such copies. Renesas
 * reserves the right to change or discontinue the Software at any time without notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS". RENESAS DISCLAIMS ALL WARRANTIES OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. TO THE
 * MAXIMUM EXTENT PERMITTED UNDER LAW, IN NO EVENT SHALL RENESAS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE, EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES. USE OF THIS SOFTWARE MAY BE SUBJECT TO TERMS AND CONDITIONS CONTAINED IN
 * AN ADDITIONAL AGREEMENT BETWEEN YOU AND RENESAS. IN CASE OF CONFLICT BETWEEN THE TERMS
 * OF THIS NOTICE AND ANY SUCH ADDITIONAL LICENSE AGREEMENT, THE TERMS OF THE AGREEMENT
 * SHALL TAKE PRECEDENCE. BY CONTINUING TO USE THIS SOFTWARE, YOU AGREE TO THE TERMS OF
 * THIS NOTICE.IF YOU DO NOT AGREE TO THESE TERMS, YOU ARE NOT PERMITTED TO USE THIS
 * SOFTWARE.
 *
 ****************************************************************************************
 */

#ifndef _USER_PERIPH_SETUP_H_
#define _USER_PERIPH_SETUP_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gpio.h"
#include "uart.h"


/*
 * DEFINES
 ****************************************************************************************
 */


/****************************************************************************************/
/* UART1 configuration                                                                  */
/****************************************************************************************/
// Define UART1 Pads
#if defined (__DA14531__)
#if defined (__FPGA__)
    #define UART1_TX_PORT           GPIO_PORT_0
    #define UART1_TX_PIN            GPIO_PIN_4

    #define UART1_RX_PORT           GPIO_PORT_0
    #define UART1_RX_PIN            GPIO_PIN_3

    #define UART1_RTSN_PORT         GPIO_PORT_0
    #define UART1_RTSN_PIN          GPIO_PIN_5

    #define UART1_CTSN_PORT         GPIO_PORT_0
    #define UART1_CTSN_PIN          GPIO_PIN_6
#else
    #define UART1_TX_PORT           GPIO_PORT_0
    #define UART1_TX_PIN            GPIO_PIN_6

    #define UART1_RX_PORT           GPIO_PORT_0
    #define UART1_RX_PIN            GPIO_PIN_5

    #define UART1_RTSN_PORT         GPIO_PORT_0
    #define UART1_RTSN_PIN          GPIO_PIN_7

    #define UART1_CTSN_PORT         GPIO_PORT_0
    #define UART1_CTSN_PIN          GPIO_PIN_8
#endif
#else
    #define UART1_TX_PORT           GPIO_PORT_0
    #define UART1_TX_PIN            GPIO_PIN_4

    #define UART1_RX_PORT           GPIO_PORT_0
    #define UART1_RX_PIN            GPIO_PIN_5

    #define UART1_RTSN_PORT         GPIO_PORT_0
    #define UART1_RTSN_PIN          GPIO_PIN_7

    #define UART1_CTSN_PORT         GPIO_PORT_0
    #define UART1_CTSN_PIN          GPIO_PIN_6
#endif

// Define UART1 Settings
#define UART1_BAUDRATE              UART_BAUDRATE_115200
#define UART1_DATABITS              UART_DATABITS_8

/* The following UART1 settings can be used if the SDK driver is
   selected for UART1.
*/
#define UART1_PARITY                UART_PARITY_NONE
#define UART1_STOPBITS              UART_STOPBITS_1
#define UART1_AFCE                  UART_AFCE_EN
#define UART1_FIFO                  UART_FIFO_EN
#define UART1_TX_FIFO_LEVEL         UART_TX_FIFO_LEVEL_0
#define UART1_RX_FIFO_LEVEL         UART_RX_FIFO_LEVEL_0


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


/****************************************************************************************/
/* External CPU to DA1458x wake-up pin selection                                        */
/****************************************************************************************/
#if defined (CFG_EXTERNAL_WAKEUP)
/* Auto select the external GPIO wakeup signal according to the
   HCI_EIF_SELECT_PORT / HCI_EIF_SELECT_PIN configuration
*/
    #define EIF_WAKEUP_GPIO                             (1)

    #if EIF_WAKEUP_GPIO
        #if defined (CFG_HCI_BOTH_EIF)
            #define EXTERNAL_WAKEUP_GPIO_PORT                                           \
                    (GPIO_GetPinStatus(HCI_EIF_SELECT_PORT, HCI_EIF_SELECT_PIN) == 1)   \
                    ? UART1_CTSN_PORT : SPI_EN_PORT

            #define EXTERNAL_WAKEUP_GPIO_PIN                                            \
                    (GPIO_GetPinStatus(HCI_EIF_SELECT_PORT, HCI_EIF_SELECT_PIN) == 1)   \
                    ? UART1_CTSN_PIN : SPI_EN_PIN

            #define EXTERNAL_WAKEUP_GPIO_POLARITY                                       \
                    (GPIO_GetPinStatus(HCI_EIF_SELECT_PORT, HCI_EIF_SELECT_PIN) == 1)   \
                    ? 1 : 0
        #else
            #if defined (CFG_HCI_SPI) || defined (CFG_GTL_SPI)
                #define EXTERNAL_WAKEUP_GPIO_PORT       SPI_EN_PORT
                #define EXTERNAL_WAKEUP_GPIO_PIN        SPI_EN_PIN
                #define EXTERNAL_WAKEUP_GPIO_POLARITY   (0)
            #else // UART
                #define EXTERNAL_WAKEUP_GPIO_PORT       UART1_CTSN_PORT
                #define EXTERNAL_WAKEUP_GPIO_PIN        UART1_CTSN_PIN
                #define EXTERNAL_WAKEUP_GPIO_POLARITY   (1)
            #endif
        #endif
    #else
#if defined (__DA14531__)
        #define EXTERNAL_WAKEUP_GPIO_PORT               GPIO_PORT_0
        #define EXTERNAL_WAKEUP_GPIO_PIN                GPIO_PIN_9
#else
        #define EXTERNAL_WAKEUP_GPIO_PORT               GPIO_PORT_1
        #define EXTERNAL_WAKEUP_GPIO_PIN                GPIO_PIN_1
#endif
        #define EXTERNAL_WAKEUP_GPIO_POLARITY           (1)
    #endif
#endif

/****************************************************************************************/
/* DA1458x to external CPU wake-up pin selection                                        */
/****************************************************************************************/
#if defined (CFG_WAKEUP_EXT_PROCESSOR)
#if defined (__DA14531__)
    #define EXT_WAKEUP_PORT         GPIO_PORT_0
    #define EXT_WAKEUP_PIN          GPIO_PIN_7
#else
    #define EXT_WAKEUP_PORT         GPIO_PORT_1
    #define EXT_WAKEUP_PIN          GPIO_PIN_2
#endif
#endif

/****************************************************************************************/
/* DA1453x POR pin configuration                                                        */
/****************************************************************************************/
#define POR_RESET_PORT          (GPIO_PORT_0)
#define POR_RESET_PIN           (GPIO_PIN_0)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// GTL FSP pad configuration structure
typedef struct {
    uint8_t gtl_tx_pad_config_tag[2];
    uint8_t tx_pad_port;
    uint8_t tx_pad_pin;
    uint8_t gtl_rx_pad_config_tag[2];
    uint8_t rx_pad_port;
    uint8_t rx_pad_pin;
    uint8_t gtl_rts_pad_config_tag[3];
    uint8_t rts_pad_port;
    uint8_t rts_pad_pin;
    uint8_t gtl_cts_pad_config_tag[3];
    uint8_t cts_pad_port;
    uint8_t cts_pad_pin;
    uint8_t por_pads_config_tag[3];
    uint8_t por_pad_port;
    uint8_t por_pad_pin;
    uint8_t por_pad_polarity;
} gtl_pads_config_t;

typedef union {
    gtl_pads_config_t config;
    uint8_t config_array[32];
} gtl_pads_config_u;

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
