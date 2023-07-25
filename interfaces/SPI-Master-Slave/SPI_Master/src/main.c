/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief SPI Master Slave demo
 *
 * Copyright (C) 2012-2022 Renesas Electronics Corporation and/or its affiliates
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

#include <stdio.h>
#include <stdint.h>
#include "arch_system.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "uart.h"
#include "uart_utils.h"
#include "spi.h"
#include "spi_flash.h"

// Settings
#define SPI_DEMO_MESSAGES_LENGTH (12)

/* buffer with data to send to SPI*/
const uint8_t spi_master_demo_out_buffer[SPI_DEMO_MESSAGES_LENGTH] = {'D','i','a','l','o','g',' ','d','e','m','o',0};

/* buffer with data to receive from SPI */
uint8_t spi_demo_in_buffer[SPI_DEMO_MESSAGES_LENGTH];
const uint8_t spi_demo_expected_response[SPI_DEMO_MESSAGES_LENGTH] = {'R','e','s','p','o','n','s','e',' ','s','l',0};


/**
 ****************************************************************************************
 * @brief SPI demo function
 ****************************************************************************************
 */
static void spi_demo(void);

/**
 ****************************************************************************************
 * @brief Main routine of the SPI demo
 ****************************************************************************************
 */
int main (void)
{
	  /* System initialization */
    system_init();
    spi_demo();
}

/**
 ****************************************************************************************
 * @brief Callback to return in interrupt/DMA-driven transferring operations
 * @param[in] spi_cb_t  SPI callback type. 
 ****************************************************************************************
 */

void spi_transfer_user_callback(uint16_t spi_transaction_length)
{
    uint8_t i;
    
    spi_cs_high(); 
      

    if (strcmp((const char*)spi_demo_in_buffer, (const char*)spi_demo_expected_response) == 0)
    {
        printf_string(UART, "\n\rAll bytes received OK\r\n");
    }
    else
    {
        printf_string(UART, "\n\rInvalid byte(s) received\r\n\r\n");
    }
        
    printf_string(UART, "\n\rBytes received:\r\n  In hex:");

    for (i=0; i<spi_transaction_length; i++)
    {
        printf_string(UART, "0x");
        printf_byte(UART, spi_demo_in_buffer[i]); 
        printf_string(UART, ", ");        
    }
    
    printf_string(UART, "\n\r  String:");
    printf_string(UART, (char*)spi_demo_in_buffer);
    


}

/**
 ****************************************************************************************
 * @brief SPI demo function
 ****************************************************************************************
 */

static void spi_demo(void)
{
    printf_string(UART, "\n\r*******************");   

    printf_string(UART, "\n\r* SPI MASTER DEMO *");       

    printf_string(UART, "\n\r*******************\n\r");   
    
    spi_register_transfer_cb(spi_transfer_user_callback);

    spi_cs_low();
    spi_cs_high();
    spi_cs_low();    

	  /* Start sending Data*/
    spi_transfer(spi_master_demo_out_buffer, spi_demo_in_buffer, SPI_DEMO_MESSAGES_LENGTH, SPI_OP_DMA);
		
	  while(1);

}
