/**
 ****************************************************************************************
 *
 * @file user_spi.c
 *
 * @brief Barebone project source code.
 *
 * Copyright (c) 2015-2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "user_spi.h"
#include "spi_flash.h"

void wait_for_DMA(void){
#if defined(CFG_SPI_DMA_SUPPORT)
	while(dma_get_channel_state(SPI_DMA_CHANNEL_RX) || dma_get_channel_state(SPI_DMA_CHANNEL_TX)){}
#endif
}

void demo_spi(void){
	 uint8_t tx_buffer[TEST_SIZE];
	 uint8_t rx_buffer[TEST_SIZE];
	 uint16_t actual_size = TEST_SIZE;
	 int8_t	error_code = SPI_FLASH_ERR_OK;
	 
	 for(uint8_t i = 0; i <= TEST_SIZE && i<= 0xFF; i++){//0xFF = 255 = (2^8)-1 = the max value of an uint8_t
		 tx_buffer[i] = i;
	 }
	 
	 
	 #if defined(__DA14531__)
			arch_set_pxact_gpio(DEBUG_GPIO_PORT,DEBUG_GPIO_PIN);//Place a software cursor in the Smartsnippets toolbox power analyser 
	 #else                                                  //
			arch_set_pxact_gpio();															//
	 #endif

	 spi_cs_low();
	 error_code = spi_send(tx_buffer,TEST_SIZE,SPI_OP);
	 if(error_code != SPI_FLASH_ERR_OK){
			//Insert error handler
	 }
	 wait_for_DMA();
	 spi_cs_high();
	 
	 #if defined(CFG_SPI_DMA_SUPPORT)
	 error_code = spi_flash_read_data_dma(rx_buffer,SOME_POINT_IN_FLASH,TEST_SIZE,&actual_size);	 
	 #else
	 error_code = spi_flash_read_data(rx_buffer,SOME_POINT_IN_FLASH,TEST_SIZE,&actual_size);
	 #endif
	 if(error_code != SPI_FLASH_ERR_OK){
			//Insert error handler
	 }
	 
	 memset(tx_buffer,RESET_VALUE,TEST_SIZE);
	 tx_buffer[0] = SPI_FLASH_OP_READ;

	 spi_cs_low();
	 error_code = spi_transfer(tx_buffer,rx_buffer,TEST_SIZE,SPI_OP);
	 if(error_code != SPI_FLASH_ERR_OK){
			//Insert error handler
	 }
	 wait_for_DMA();
	 spi_cs_high();
	 
}

/// @} APP
