/**
 ****************************************************************************************
 *
 * @file spi_58x.h
 *
 * @brief Definition of API for the SPI Low Level Driver for DA14585 and DA14586 devices.
 *
 * Copyright (c) 2018-2019 Dialog Semiconductor. All rights reserved.
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

#ifndef SPI_585_H_
#define SPI_585_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>       // integer definition
#include <stdbool.h>      // boolean definition
#include "datasheet.h"
#include "gpio.h"
#include "dma.h"

/*
 * DEFINITIONS
 *****************************************************************************************
 */

 /// SPI Status No Error
#define SPI_STATUS_ERR_OK           (0)
/// SPI Status Config Error
#define SPI_STATUS_CFG_ERR          (-1)

/// SPI registers (@ 0x50001200)
typedef struct
{
    /// SPI control register 0 (@ 0x00000000)
    volatile uint16_t SPI_CTRL_REGF;

    /// SPI RX/TX register0 (@ 0x00000002)
    volatile uint16_t SPI_RX_TX_REG0F;

    /// SPI RX/TX register1 (@ 0x00000004)
    volatile uint16_t SPI_RX_TX_REG1F;

    /// SPI clear interrupt register (@ 0x00000006)
    volatile uint16_t SPI_CLEAR_INT_REGF;

    /// SPI control register 1 (@ 0x0000008)
    volatile uint16_t SPI_CTRL_REG1F;
} spi_t;

/// SPI Instantiation
static spi_t * const spi = (spi_t *) (SPI_CTRL_REG);

/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */

/// @brief Generic enable/disable enum for SPI driver.
typedef enum {
    /// Generic Disable bit.
    SPI_BIT_DIS,

    /// Generic Enable bit.
    SPI_BIT_EN,
} SPI_BIT_CFG;

/// @brief Defines the SPI master/slave mode.
typedef enum {
    /// SPI Master mode
    SPI_MS_MODE_MASTER,

#if (SPI_SLAVE_SUPPORT)
    /// SPI Slave mode
    SPI_MS_MODE_SLAVE,
#endif
} SPI_MS_MODE_CFG;

/// @brief Defines the SPI mode (CPOL, CPHA).
typedef enum {
    /// New data on falling, capture on rising, Clk low in idle state.
    SPI_CP_MODE_0 = 0,

    /// New data on rising, capture on falling, Clk low in idle state.
    SPI_CP_MODE_1 = 1,

    /// New data on falling, capture on rising, Clk high in idle state.
    SPI_CP_MODE_2 = 2,

    /// New data on rising, capture on falling, Clk high in idle state.
    SPI_CP_MODE_3 = 3,
} SPI_CP_MODE_CFG;

/// @brief Defines the SPI speed mode.
typedef enum {
    /// SPI master clock frequency 2MHz.
    SPI_SPEED_MODE_2MHz,

    /// SPI master clock frequency 4MHz.
    SPI_SPEED_MODE_4MHz,

    /// SPI master clock frequency 8MHz.
    SPI_SPEED_MODE_8MHz,

    /// SPI master clock frequency 1M14Hz.
    SPI_SPEED_MODE_1M14Hz,
} SPI_SPEED_MODE_CFG;

/// @brief Word Size Configuration
typedef enum {
    /// Word Size 8 bits
    SPI_MODE_8BIT = 0,

    /// Word Size 16 bits
    SPI_MODE_16BIT = 1,

    /// Word Size 32 bits
    SPI_MODE_32BIT = 2,

    /// Word Size 9 bits
    SPI_MODE_9BIT = 3,

} SPI_WSZ_MODE_CFG;

/// @brief Control the CS output in master mode.
typedef enum {
    /// Selected slave device connected to GPIO with FUNC_MODE = SPI_CS0.
    SPI_CS_0,
} SPI_CS_MODE_CFG;

/// @brief Define the SPI IRQ masked/enabled mode.
typedef enum {
    /// IRQ is masked.
    SPI_IRQ_DISABLED,

    /// IRQ is enabled.
    SPI_IRQ_ENABLED,
} SPI_IRQ_CFG;

/// @brief Mode of operation
typedef enum {
    /// Blocking operation (no interrupts - no DMA)
    SPI_OP_BLOCKING,

#if defined (CFG_SPI_DMA_SUPPORT)
    /// DMA driven operation
    SPI_OP_DMA,
#endif
} SPI_OP_CFG;

#if defined (CFG_SPI_DMA_SUPPORT)
/// @brief SPI DMA channel configuration.
typedef enum {
    /// Rx = Channel 0, Tx = Channel 1
    SPI_DMA_CHANNEL_01,

    /// Rx = Channel 2, Tx = Channel 3
    SPI_DMA_CHANNEL_23,
} SPI_DMA_CHANNEL_CFG;
#endif

/*
 * TYPE DEFINITIONS
 *****************************************************************************************
 */
/// SPI Pad configuration
typedef struct {
    /// SPI Port
    GPIO_PORT port;

    /// SPI Pin
    GPIO_PIN pin;
}SPI_Pad_t;

/// SPI callback type
typedef void (*spi_cb_t)(uint16_t length);

/// SPI configuration structure definition
typedef struct
{
    /// SPI master/slave mode
    SPI_MS_MODE_CFG                 spi_ms:1;

    /// SPI clock mode (CPOL, CPHA)
    SPI_CP_MODE_CFG                 spi_cp:3;

    /// SPI master clock frequency
    SPI_SPEED_MODE_CFG              spi_speed:4;

    /// SPI word size (1, 2, 4 Bytes or 9 bits)
    SPI_WSZ_MODE_CFG                spi_wsz:3;

    /// SPI master CS mode
    SPI_CS_MODE_CFG                 spi_cs:3;

    /// SPI Interrupt mode
    SPI_IRQ_CFG                     spi_irq:1;

    /// SPI CS Pad
    SPI_Pad_t                       cs_pad;

#if defined (CFG_SPI_DMA_SUPPORT)
    /// SPI send callback
    spi_cb_t                        send_cb;

    /// SPI receive callback
    spi_cb_t                        receive_cb;

    /// SPI transfer callback
    spi_cb_t                        transfer_cb;

    /// SPI DMA Channel Pair Configuration
    SPI_DMA_CHANNEL_CFG             spi_dma_channel:1;

    /// SPI DMA Priority Configuration
    DMA_PRIO_CFG                    spi_dma_priority;

#endif
} spi_cfg_t;

/**
 ****************************************************************************************
 * @brief Configure SPI chip select pad
 * @param[in] cs_pad    SPI chip select pad (port and pin)
 * @return void
 ****************************************************************************************
 */
void spi_set_cs_pad(SPI_Pad_t cs_pad);

#endif // SPI_585_H_
