/**
 ****************************************************************************************
 *
 * @file user_spi.h
 *
 * @brief Barebone application header file.
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

#ifndef _USER_SPI_H_
#define _USER_SPI_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup
 *
 * @brief
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
	

/*
 * DEFINES
 ****************************************************************************************
 */

/***************************************************************************************/
/* User defines  											                                                 */
/***************************************************************************************/
#define RESET_VALUE		0
#define TEST_SIZE			100 

/***************************************************************************************/
/* DMA defines  											                                                 */
/***************************************************************************************/
#if  defined(CFG_SPI_DMA_SUPPORT)
	#define SPI_OP SPI_OP_DMA
#elif defined(SPI_INTR_SUPPORT) 
	#define SPI_OP SPI_OP_INTR
#else
	#define SPI_OP SPI_OP_BLOCKING		
#endif

#if USER_SPI_DMA_CHANNEL == SPI_DMA_CHANNEL_01
	#define SPI_DMA_CHANNEL_RX DMA_CHANNEL_0
	#define SPI_DMA_CHANNEL_TX DMA_CHANNEL_1
#else
	#define SPI_DMA_CHANNEL_RX DMA_CHANNEL_2
	#define SPI_DMA_CHANNEL_TX DMA_CHANNEL_3
#endif

/***************************************************************************************/
/* Flash defines  											                                               */
/***************************************************************************************/
#define SOME_POINT_IN_FLASH   0x10000

/***************************************************************************************/
/* Debug pin 														                                               */
/***************************************************************************************/
#if defined(__DA14531__)
#define DEBUG_GPIO_PORT GPIO_PORT_0
#define DEBUG_GPIO_PIN 	GPIO_PIN_9
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void demo_spi(void);

#if defined(CFG_SPI_DMA_SUPPORT)
void wait_for_DMA(void);
#endif

/// @} APP

#endif //_USER_SPI_H_
