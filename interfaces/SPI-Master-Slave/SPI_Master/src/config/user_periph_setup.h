/**
 ****************************************************************************************
 *
 * @file user_periph_setup.h
 *
 * @brief Peripherals setup header file.
 *
 * Copyright (C) 2012-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _USER_PERIPH_SETUP_H_
#define _USER_PERIPH_SETUP_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "uart.h"
#include "gpio.h"
#include "spi.h"
#include "spi_flash.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/****************************************************************************************/
/* UART2 configuration to print messages                                                */
/****************************************************************************************/
#define UART                        UART2
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
/* SPI configuration                                                                    */
/****************************************************************************************/
// Define SPI Pads for DA14531 target

    #define SPI_EN_PORT             GPIO_PORT_0
    #define SPI_EN_PIN              GPIO_PIN_8

    #define SPI_CLK_PORT            GPIO_PORT_0
    #define SPI_CLK_PIN             GPIO_PIN_6

    #define SPI_DO_PORT             GPIO_PORT_0
    #define SPI_DO_PIN              GPIO_PIN_9

    #define SPI_DI_PORT             GPIO_PORT_0
    #define SPI_DI_PIN              GPIO_PIN_11

// Define SPI Configuration

    #define SPI_MS_MODE             SPI_MS_MODE_MASTER
    #define SPI_CP_MODE             SPI_CP_MODE_0
    #define SPI_WSZ                 SPI_MODE_8BIT
    #define SPI_CS                  SPI_CS_0

    #define SPI_SPEED_MODE          SPI_SPEED_MODE_2MHz
    #define SPI_EDGE_CAPTURE        SPI_MASTER_EDGE_CAPTURE

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief   Initializes application's peripherals and pins
 ****************************************************************************************
 */
void periph_init(void);

/**
 ****************************************************************************************
 * @brief Set gpio port function mode
 ****************************************************************************************
 */
void set_pad_functions(void);

#endif // _USER_PERIPH_SETUP_H_
