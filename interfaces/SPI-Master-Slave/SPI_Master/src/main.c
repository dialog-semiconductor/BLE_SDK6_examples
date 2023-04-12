/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief SPI Master Slave demo
 *
 * Copyright (C) 2012-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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
