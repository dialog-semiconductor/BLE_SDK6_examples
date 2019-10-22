/**
 ****************************************************************************************
 *
 * @file spi_531.h
 *
 * @brief Definition of API for the SPI Low Level Driver for DA14531 devices.
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

#ifndef SPI_531_H_
#define SPI_531_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>       // integer definition
#include <stdbool.h>      // boolean definition
#include "datasheet.h"
#include "gpio.h"
#include "dma.h"

// Define supported features/operations

// Enable support of SPI Slave role
#ifdef CFG_SPI_SLAVE
    #define SPI_SLAVE_SUPPORT           1
#else
    #define SPI_SLAVE_SUPPORT           0
#endif

/*
 * DEFINITIONS
 *****************************************************************************************
 */

/// SPI Status No Error
#define SPI_STATUS_ERR_OK           (0)
/// SPI Status Config Error
#define SPI_STATUS_CFG_ERR          (-1)
/// SPI Status Slave Overflow Error
#define SPI_STATUS_OVFL_ERR         (-2)


/// SPI registers (@ 0x50001200)
typedef struct
{
    /// SPI Control Register (@ 0x00000000)
    volatile uint16_t SPI_CTRL_REGF;
    uint16_t  RESERVED;
    /// SPI Configuration Register (@ 0x00000004)
    volatile uint16_t SPI_CONFIG_REGF;
    uint16_t  RESERVED1;
    /// SPI Clock Register (@ 0x00000008)
    volatile uint16_t SPI_CLOCK_REGF;
    uint16_t  RESERVED2;
    /// SPI FIFO Configuration Register (@ 0x0000000C)
    volatile uint16_t SPI_FIFO_CONFIG_REGF;
    uint16_t  RESERVED3;
    /// SPI IRQ Mask Register (@ 0x00000010)
    volatile uint16_t SPI_IRQ_MASK_REGF;
    uint16_t  RESERVED4;
    /// SPI Status Register (@ 0x00000014)
    volatile uint16_t SPI_STATUS_REGF;
    uint16_t  RESERVED5;
    /// SPI FIFO Status Register (@ 0x00000018)
    volatile uint16_t SPI_FIFO_STATUS_REGF;
    uint16_t  RESERVED6;
    /// SPI FIFO Read Register (@ 0x0000001C)
    volatile uint16_t SPI_FIFO_READ_REGF;
    uint16_t  RESERVED7;
    /// SPI FIFO Write Register (@ 0x00000020)
    volatile uint16_t SPI_FIFO_WRITE_REGF;
    uint16_t  RESERVED8;
    /// SPI CS Configuration Register (@ 0x00000024)
    volatile uint16_t SPI_CS_CONFIG_REGF;
    uint16_t  RESERVED9;
    /// SPI FIFO High Register (@ 0x00000028)
    volatile uint16_t SPI_FIFO_HIGH_REGF;
    uint16_t  RESERVED10;
    /// SPI TXBUFFER_FORCE Low Register (@ 0x0000002C)
    volatile uint16_t SPI_TXBUFFER_FORCE_L_REGF;
    uint16_t  RESERVED11;
    /// SPI TXBUFFER_FORCE High Register (@ 0x00000030)
    volatile uint16_t SPI_TXBUFFER_FORCE_H_REGF;
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
    SPI_BIT_DIS     = 0,

    /// Generic Enable bit.
    SPI_BIT_EN      = 1,
} SPI_BIT_CFG;

/// @brief Defines the SPI master/slave mode.
typedef enum {
    /// SPI Master mode
    SPI_MS_MODE_MASTER  = 0,

#if (SPI_SLAVE_SUPPORT)
    /// SPI Slave mode
    SPI_MS_MODE_SLAVE   = 1,
#endif
} SPI_MS_MODE_CFG;

/// @brief Defines the SPI mode (CPOL, CPHA).
typedef enum {
    /// New data on falling, capture on rising, Clk low in idle state.
    SPI_CP_MODE_0   = 0,

    /// New data on rising, capture on falling, Clk low in idle state.
    SPI_CP_MODE_1   = 1,

    /// New data on rising, capture on falling, Clk high in idle state.
    SPI_CP_MODE_2   = 2,

    /// New data on falling, capture on rising, Clk high in idle state.
    SPI_CP_MODE_3   = 3,
} SPI_CP_MODE_CFG;

/// @brief Defines the SPI speed mode.
typedef enum {
    /// SPI master clock frequency 2MHz.
    SPI_SPEED_MODE_2MHz     = 2000,

    /// SPI master clock frequency 4MHz.
    SPI_SPEED_MODE_4MHz     = 4000,

    /// SPI master clock frequency 8MHz.
    SPI_SPEED_MODE_8MHz     = 8000,

    /// SPI master clock frequency 16MHz.
    SPI_SPEED_MODE_16MHz    = 16000,

    /// SPI master clock frequency 32MHz.
    SPI_SPEED_MODE_32MHz    = 32000,
} SPI_SPEED_MODE_CFG;

/// @brief Word Size Configuration
typedef enum {
    /// Word Size 8 bits
    SPI_MODE_8BIT       = 0,

    /// Word Size 16 bits
    SPI_MODE_16BIT      = 1,

    /// Word Size 32 bits
    SPI_MODE_32BIT      = 2,
} SPI_WSZ_MODE_CFG;

/// @brief Control the CS output in master mode.
typedef enum {
    /// None slave device selected.
    SPI_CS_NONE         = 0,

    /// Selected slave device connected to GPIO with FUNC_MODE = SPI_CS0.
    SPI_CS_0            = 1,

    /// Selected slave device connected to GPIO with FUNC_MODE = SPI_CS1.
    SPI_CS_1            = 2,

    /// Selected slave device connected to GPIO with FUNC_MODE = GPIO.
    SPI_CS_GPIO         = 4,
} SPI_CS_MODE_CFG;

/// @brief Define the SPI IRQ masked/enabled mode.
typedef enum {
    /// IRQ is masked.
    SPI_IRQ_DISABLED    = 0,

    /// IRQ is enabled.
    SPI_IRQ_ENABLED     = 1,
} SPI_IRQ_CFG;

/// @brief Mode of operation
typedef enum {
    /// Blocking operation (no interrupts - no DMA)
    SPI_OP_BLOCKING     = 0,

#if (SPI_INTR_SUPPORT)
    /// Interrupt driven operation (Currently not supported)
    SPI_OP_INTR         = 1,
#endif // SPI_INTR_SUPPORT

#if defined (CFG_SPI_DMA_SUPPORT)
    /// DMA driven operation
    SPI_OP_DMA          = 2,
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

/// @brief Define the SPI master edge capture type.
typedef enum {
    /// SPI master captures data at current clock edge
    SPI_MASTER_EDGE_CAPTURE         = 0,

    /// SPI master captures data at next clock edge. (only for high clock configurations)
    SPI_MASTER_EDGE_CAPTURE_NEXT    = 1,
} SPI_MASTER_EDGE_CAPTURE_CFG;

/// @brief SPI RX FIFO Empty status.
typedef enum {
    /// RX FIFO is not empty.
    SPI_RX_FIFO_IS_NOT_EMPTY        = 0,

    /// RX FIFO is empty.
    SPI_RX_FIFO_IS_EMPTY            = 1,
} SPI_RX_FIFO_EMPTY_STATUS;

/// @brief SPI RX FIFO Full status.
typedef enum {
    /// RX FIFO level is less than SPI_RX_TL + 1.
    SPI_RX_FIFO_IS_NOT_FULL         = 0,

    /// RX FIFO level is more or equal to SPI_RX_TL + 1.
    SPI_RX_FIFO_IS_FULL             = 1,
} SPI_RX_FIFO_FULL_STATUS;

/// @brief SPI TX FIFO Empty status.
typedef enum {
    /// TX FIFO level is larger than SPI_TX_TL.
    SPI_TX_FIFO_IS_NOT_EMPTY        = 0,

    /// TX FIFO level is less or equal to SPI_TX_TL.
    SPI_TX_FIFO_IS_EMPTY            = 1,
} SPI_TX_FIFO_EMPTY_STATUS;

/// @brief SPI TX FIFO Full status.
typedef enum {
    /// TX FIFO is not full.
    SPI_TX_FIFO_IS_NOT_FULL        = 0,

    /// TX FIFO is full.
    SPI_TX_FIFO_IS_FULL            = 1,
} SPI_TX_FIFO_FULL_STATUS;

/// @brief SPI transaction status.
typedef enum {
    /// SPI transaction is inactive.
    SPI_TRANSACTION_IS_INACTIVE     = 0,

    /// SPI transaction is active.
    SPI_TRANSACTION_IS_ACTIVE       = 1,
} SPI_TRANSACTION_STATUS;

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
    SPI_SPEED_MODE_CFG              spi_speed;

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

    /// SPI Capture at current or next edge.
    SPI_MASTER_EDGE_CAPTURE_CFG spi_capture:1;
} spi_cfg_t;


/*
 * Low Level Register Access Functions
 *****************************************************************************************
 */


/***************** SPI_CTRL_REG Functions *****************/

/**
 ****************************************************************************************
 * @brief Set SPI Control Register Value.
 * @param[in] spi_ctrl_reg      SPI Control Register Value
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_ctrl_reg_setf(uint8_t spi_ctrl_reg){
    SetWord16(&spi->SPI_CTRL_REGF, spi_ctrl_reg);
}

/**
 ****************************************************************************************
 * @brief Get SPI Control Register Value.
 * @return uint8_t              SPI Control Register Value
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t spi_ctrl_reg_getf(void){
    return GetWord16(&spi->SPI_CTRL_REGF);
}

/**
 ****************************************************************************************
 * @brief Set SPI Control Register Value.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_ctrl_reg_clear_enable_setf(void){
    SetBits16(&spi->SPI_CTRL_REGF, SPI_EN | SPI_TX_EN | SPI_RX_EN| SPI_DMA_TX_EN |
              SPI_DMA_RX_EN, 0);
}

/**
 ****************************************************************************************
 * @brief Set SPI_EN in Control Register.
 * @param[in] spi_enable        SPI_BIT_DIS: Disable, SPI_BIT_EN: Enable
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_ctrl_reg_spi_en_setf(SPI_BIT_CFG spi_enable){
    SetBits16(&spi->SPI_CTRL_REGF, SPI_EN, spi_enable);
}

/**
 ****************************************************************************************
 * @brief Get SPI_EN from Control Register.
 * @return SPI_BIT_DIS: Disabled, SPI_BIT_EN: Enabled
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_BIT_CFG spi_ctrl_reg_spi_en_getf(void){
    return (SPI_BIT_CFG) GetBits16(&spi->SPI_CTRL_REGF, SPI_EN);
}

/**
 ****************************************************************************************
 * @brief Set SPI_TX_EN in Control Register.
 * @param[in] spi_tx_enable     SPI_BIT_DIS: Disable, SPI_BIT_EN: Enable
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_ctrl_reg_spi_tx_en_setf(SPI_BIT_CFG spi_tx_enable){
    SetBits16(&spi->SPI_CTRL_REGF, SPI_TX_EN, spi_tx_enable);
}

/**
 ****************************************************************************************
 * @brief Get SPI_TX_EN from Control Register.
 * @return SPI_BIT_DIS: Disabled, SPI_BIT_EN: Enabled
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_BIT_CFG spi_ctrl_reg_spi_tx_en_getf(void){
    return (SPI_BIT_CFG) GetBits16(&spi->SPI_CTRL_REGF, SPI_TX_EN);
}

/**
 ****************************************************************************************
 * @brief Set SPI_RX_EN in Control Register.
 * @param[in] spi_rx_enable     SPI_BIT_DIS: Disable, SPI_BIT_EN: Enable
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_ctrl_reg_spi_rx_en_setf(SPI_BIT_CFG spi_rx_enable){
    SetBits16(&spi->SPI_CTRL_REGF, SPI_RX_EN, spi_rx_enable);
}

/**
 ****************************************************************************************
 * @brief Get SPI_RX_EN from Control Register.
 * @return SPI_BIT_DIS: Disabled, SPI_BIT_EN: Enabled
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_BIT_CFG spi_ctrl_reg_spi_rx_en_getf(void){
    return (SPI_BIT_CFG) GetBits16(&spi->SPI_CTRL_REGF, SPI_RX_EN);
}

/**
 ****************************************************************************************
 * @brief Set SPI_DMA_TX_EN in Control Register.
 * @param[in] spi_dma_tx_enable SPI_BIT_DIS: Disable, SPI_BIT_EN: Enable
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_ctrl_reg_spi_dma_tx_en_setf(SPI_BIT_CFG spi_dma_tx_enable){
    SetBits16(&spi->SPI_CTRL_REGF, SPI_DMA_TX_EN, spi_dma_tx_enable);
}

/**
 ****************************************************************************************
 * @brief Get SPI_DMA_TX_EN from Control Register.
 * @return SPI_BIT_DIS: Disabled, SPI_BIT_EN: Enabled
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_BIT_CFG spi_ctrl_reg_spi_dma_tx_en_getf(void){
    return (SPI_BIT_CFG) GetBits16(&spi->SPI_CTRL_REGF, SPI_DMA_TX_EN);
}

/**
 ****************************************************************************************
 * @brief Set SPI_DMA_RX_EN in Control Register.
 * @param[in] spi_dma_rx_enable SPI_BIT_DIS: Disable, SPI_BIT_EN: Enable
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_ctrl_reg_spi_dma_rx_en_setf(SPI_BIT_CFG spi_dma_rx_enable){
    SetBits16(&spi->SPI_CTRL_REGF, SPI_DMA_RX_EN, spi_dma_rx_enable);
}

/**
 ****************************************************************************************
 * @brief Get SPI_DMA_RX_EN from Control Register.
 * @return SPI_BIT_DIS: Disabled, SPI_BIT_EN: Enabled
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_BIT_CFG spi_ctrl_reg_spi_dma_rx_en_getf(void){
    return (SPI_BIT_CFG) GetBits16(&spi->SPI_CTRL_REGF, SPI_DMA_RX_EN);
}

/**
 ****************************************************************************************
 * @brief Set SPI_FIFO_RESET in Control Register.
 * @param[in] spi_fifo_reset SPI_BIT_DIS: Disable, SPI_BIT_EN: Enable
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_ctrl_reg_spi_fifo_reset_setf(SPI_BIT_CFG spi_fifo_reset){
    SetBits16(&spi->SPI_CTRL_REGF, SPI_FIFO_RESET, spi_fifo_reset);
}

/**
 ****************************************************************************************
 * @brief Get SPI_FIFO_RESET from Control Register.
 * @return SPI_BIT_DIS: Disabled, SPI_BIT_EN: Enabled
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_BIT_CFG spi_ctrl_reg_spi_fifo_reset_getf(void){
    return (SPI_BIT_CFG) GetBits16(&spi->SPI_CTRL_REGF, SPI_FIFO_RESET);
}

/**
 ****************************************************************************************
 * @brief Set SPI_CAPTURE_AT_NEXT_EDGE in Control Register.
 * @param[in] capture_next_edge
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_ctrl_reg_capture_next_edge_setf(SPI_MASTER_EDGE_CAPTURE_CFG
                                                       capture_next_edge){
    SetBits16(&spi->SPI_CTRL_REGF, SPI_CAPTURE_AT_NEXT_EDGE, capture_next_edge);
}

/**
 ****************************************************************************************
 * @brief Get SPI_CAPTURE_AT_NEXT_EDGE from Control Register.
 * @return SPI_MASTER_EDGE_CAPTURE_CFG
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_MASTER_EDGE_CAPTURE_CFG spi_ctrl_reg_capture_next_edge_getf(void){
    return (SPI_MASTER_EDGE_CAPTURE_CFG) GetBits16(&spi->SPI_CTRL_REGF, SPI_CAPTURE_AT_NEXT_EDGE);
}


/***************** SPI_CONFIG_REG Functions *****************/

/**
 ****************************************************************************************
 * @brief Set SPI Configuration Register Value.
 * @param[in] spi_config_reg      SPI Configuration Register Value
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_config_reg_setf(uint8_t spi_config_reg){
    SetWord16(&spi->SPI_CONFIG_REGF, spi_config_reg);
}

/**
 ****************************************************************************************
 * @brief Get SPI Configuration Register Value.
 * @return uint8_t              SPI Configuration Register Value
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t spi_config_reg_getf(void){
    return GetWord16(&spi->SPI_CONFIG_REGF);
}

/**
 ****************************************************************************************
 * @brief Set SPI_MODE in Configuration Register.
 * @param[in] spi_cp            SPI_CP_MODE_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_config_reg_spi_mode_setf(SPI_CP_MODE_CFG spi_cp){
    SetBits16(&spi->SPI_CONFIG_REGF, SPI_MODE, spi_cp);
}

/**
 ****************************************************************************************
 * @brief Get SPI_MODE from Configuration Register.
 * @return SPI_CP_MODE_CFG
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_CP_MODE_CFG spi_config_reg_spi_mode_getf(void){
    return (SPI_CP_MODE_CFG) GetBits16(&spi->SPI_CONFIG_REGF, SPI_MODE);
}

/**
 ****************************************************************************************
 * @brief Set SPI_WORD_LENGTH in Configuration Register.
 * @param[in] spi_wsz           SPI Word size in bits
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_config_reg_wsz_setf(uint8_t spi_wsz){
    SetBits16(&spi->SPI_CONFIG_REGF, SPI_WORD_LENGTH, spi_wsz - 1);
}

/**
 ****************************************************************************************
 * @brief Get SPI_WORD_LENGTH from Configuration Register.
 * @return uint8_t              SPI Word size in bits
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t spi_config_reg_wsz_getf(void){
    return (uint8_t) ((GetBits16(&spi->SPI_CONFIG_REGF, SPI_WORD_LENGTH) + 1));
}

/**
 ****************************************************************************************
 * @brief Set SPI_SLAVE_EN in Configuration Register.
 * @param[in] spi_ms
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_config_reg_ms_mode_setf(SPI_MS_MODE_CFG spi_ms){
    SetBits16(&spi->SPI_CONFIG_REGF, SPI_SLAVE_EN, spi_ms);
}

/**
 ****************************************************************************************
 * @brief Get SPI Master/Slave mode from Configuration Register.
 * @return SPI_MS_MODE_CFG
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_MS_MODE_CFG spi_config_reg_ms_mode_getf(void){
    return (SPI_MS_MODE_CFG) GetBits16(&spi->SPI_CONFIG_REGF, SPI_SLAVE_EN);
}


/***************** SPI_CLOCK_REG Functions *****************/

/**
 ****************************************************************************************
 * @brief Set SPI Clock Register Value.
 * @param[in] spi_clock_reg      SPI Clock Register Value
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_clock_reg_setf(uint8_t spi_clock_reg){
    SetWord16(&spi->SPI_CLOCK_REGF, spi_clock_reg);
}

/**
 ****************************************************************************************
 * @brief Get SPI Clock Register Value.
 * @return uint8_t              SPI Clock Register Value
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t spi_clock_reg_getf(void){
    return GetWord16(&spi->SPI_CLOCK_REGF);
}

/**
 ****************************************************************************************
 * @brief Set SPI_CLK_DIV in Clock Register. Applicable only in master mode. Defines
 *        the spi clock frequency in master only mode.
 * @param[in] spi_clk_div        SPI_CLK = module_clk / 2 * (SPI_CLK_DIV + 1)
 *                               when SPI_CLK_DIV is not 0x7F.
 *                               If SPI_CLK_DIV = 0x7F then SPI_CLK = module_clk
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_clock_reg_clk_div_setf(uint8_t spi_clk_div){
    SetBits16(&spi->SPI_CLOCK_REGF, SPI_CLK_DIV, spi_clk_div);
}

/**
 ****************************************************************************************
 * @brief Get SPI_CLK_DIV from Configuration Register.
 * @return spi_clk_div
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t spi_clock_reg_clk_div_getf(void){
    return GetBits16(&spi->SPI_CLOCK_REGF, SPI_CLK_DIV);
}

/**
 ****************************************************************************************
 * @brief Set SPI_MASTER_CLK_MODE in Clock Register.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_clock_reg_master_clk_mode_setf(void){
    SetBits16(&spi->SPI_CLOCK_REGF, SPI_MASTER_CLK_MODE, 1);
}

/***************** SPI_FIFO_CONFIG_REG Functions *****************/

/**
 ****************************************************************************************
 * @brief Set SPI_TX_TL in FIFO Configuration Register.
 * @param[in] spi_tx_tl         Transmit FIFO threshold level in bytes. Control the level
 *                              of bytes in fifo that triggers the TX_EMPTY interrupt.
 *                              IRQ is occurred when fifo level is less or equal to
 *                              spi_tx_tl.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_fifo_config_tx_tl_setf(uint8_t spi_tx_tl){
    SetBits16(&spi->SPI_FIFO_CONFIG_REGF, SPI_TX_TL, spi_tx_tl);
}

/**
 ****************************************************************************************
 * @brief Get SPI_TX_TL from FIFO Configuration Register.
 * @return spi_tx_tl
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t spi_fifo_config_tx_tl_getf(void){
    return GetBits16(&spi->SPI_FIFO_CONFIG_REGF, SPI_TX_TL);
}

/**
 ****************************************************************************************
 * @brief Set SPI_RX_TL in FIFO Configuration Register.
 * @param[in] spi_rx_tl         Receive FIFO threshold level in bytes. Control the level
 *                              of bytes in fifo that triggers the RX_FULL interrupt.
 *                              IRQ is occurred when fifo level is less or equal to
 *                              spi_rx_tl + 1.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_fifo_config_rx_tl_setf(uint8_t spi_rx_tl){
    SetBits16(&spi->SPI_FIFO_CONFIG_REGF, SPI_RX_TL, spi_rx_tl);
}

/**
 ****************************************************************************************
 * @brief Get SPI_RX_TL from FIFO Configuration Register.
 * @return spi_rx_tl
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t spi_fifo_config_rx_tl_getf(void){
    return GetBits16(&spi->SPI_FIFO_CONFIG_REGF, SPI_RX_TL);
}


/***************** SPI_IRQ_MASK_REG Functions *****************/

/**
 ****************************************************************************************
 * @brief Set SPI_IRQ_MASK_TX_EMPTY in IRQ Mask Register.
 * @param[in] irq_tx_empty_en   SPI_IRQ_CFG.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_irq_tx_empty_en_setf(SPI_IRQ_CFG irq_tx_empty_en){
    SetBits16(&spi->SPI_IRQ_MASK_REGF, SPI_IRQ_MASK_TX_EMPTY, irq_tx_empty_en);
}

/**
 ****************************************************************************************
 * @brief Get SPI_IRQ_MASK_TX_EMPTY from IRQ Mask Register.
 * @return SPI_IRQ_CFG.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_IRQ_CFG spi_irq_tx_empty_en_getf(void){
    return (SPI_IRQ_CFG) GetBits16(&spi->SPI_IRQ_MASK_REGF, SPI_IRQ_MASK_TX_EMPTY);
}

/**
 ****************************************************************************************
 * @brief Set SPI_IRQ_MASK_RX_FULL in IRQ Mask Register.
 * @param[in] irq_rx_full_en   SPI_IRQ_CFG.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_irq_rx_full_en_setf(SPI_IRQ_CFG irq_rx_full_en){
    SetBits16(&spi->SPI_IRQ_MASK_REGF, SPI_IRQ_MASK_RX_FULL, irq_rx_full_en);
}

/**
 ****************************************************************************************
 * @brief Get SPI_IRQ_MASK_RX_FULL from IRQ Mask Register.
 * @return SPI_IRQ_CFG.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_IRQ_CFG spi_irq_rx_full_en_getf(void){
    return (SPI_IRQ_CFG) GetBits16(&spi->SPI_IRQ_MASK_REGF, SPI_IRQ_MASK_RX_FULL);
}


/***************** SPI_STATUS_REG and SPI_FIFO_STATUS_REG Functions *****************/

/**
 ****************************************************************************************
 * @brief Get SPI FIFO status.
 * @return SPI FIFO STATUS.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t spi_fifo_status_getf(void){
    return  GetWord16(&spi->SPI_FIFO_STATUS_REGF);
}

/**
 ****************************************************************************************
 * @brief Get SPI transaction status from Status Register.
 * @return SPI_TRANSACTION_STATUS.
 ****************************************************************************************
 */

__STATIC_FORCEINLINE SPI_TRANSACTION_STATUS spi_transaction_status_getf(void){
    return (SPI_TRANSACTION_STATUS) GetBits16(&spi->SPI_FIFO_STATUS_REGF, SPI_TRANSACTION_ACTIVE);
}

/**
 ****************************************************************************************
 * @brief Get SPI TX FIFO Full status from FIFO Status Register.
 * @return SPI_TX_FIFO_FULL_STATUS.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_TX_FIFO_FULL_STATUS spi_tx_fifo_full_status_getf(void){
    return (SPI_TX_FIFO_FULL_STATUS) GetBits16(&spi->SPI_FIFO_STATUS_REGF, SPI_STATUS_TX_FULL);
}

/**
 ****************************************************************************************
 * @brief Get SPI TX FIFO Empty status from Status Register.
 * @return SPI_TX_FIFO_EMPTY_STATUS.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_TX_FIFO_EMPTY_STATUS spi_tx_fifo_empty_status_getf(void){
    return (SPI_TX_FIFO_EMPTY_STATUS) GetBits16(&spi->SPI_STATUS_REGF, SPI_STATUS_TX_EMPTY);
}

/**
 ****************************************************************************************
 * @brief Get SPI TX FIFO level from FIFO Status Register.
 * @return Number of bytes in TX FIFO.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t spi_tx_fifo_level_getf(void){
    return GetBits16(&spi->SPI_FIFO_STATUS_REGF, SPI_TX_FIFO_LEVEL);
}

/**
 ****************************************************************************************
 * @brief Get SPI RX FIFO Empty status from FIFO Status Register.
 * @return SPI_RX_FIFO_EMPTY_STATUS.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_RX_FIFO_EMPTY_STATUS spi_rx_fifo_empty_status_getf(void){
    return (SPI_RX_FIFO_EMPTY_STATUS) GetBits16(&spi->SPI_FIFO_STATUS_REGF, SPI_STATUS_RX_EMPTY);
}

/**
 ****************************************************************************************
 * @brief Get SPI RX FIFO Full status from Status Register.
 * @return SPI_RX_FIFO_FULL_STATUS.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_RX_FIFO_FULL_STATUS spi_rx_fifo_full_status_getf(void){
    return (SPI_RX_FIFO_FULL_STATUS) GetBits16(&spi->SPI_STATUS_REGF, SPI_STATUS_RX_FULL);
}

/**
 ****************************************************************************************
 * @brief Get SPI RX FIFO level from FIFO Status Register.
 * @return Number of bytes in RX FIFO.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t spi_rx_fifo_level_getf(void){
    return GetBits16(&spi->SPI_FIFO_STATUS_REGF, SPI_RX_FIFO_LEVEL);
}

/**
 ****************************************************************************************
 * @brief Get SPI RX FIFO overflow status from FIFO Status Register.
 * @return 0 = no overflow. 1 = receive data is not written to FIFO because FIFO was full.
           It clears with SPI_CTRL_REG.SPI_FIFO_RESET.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t spi_rx_fifo_overflow_status_getf(void){
    return GetBits16(&spi->SPI_FIFO_STATUS_REGF, SPI_RX_FIFO_OVFL);
}


/***************** SPI_FIFO_READ_REG Functions *****************/

/**
 ****************************************************************************************
 * @brief Read RX FIFO. Read access is permitted only if SPI_RX_FIFO_EMPTY = 0.
 * @return 16 LSbits of RX FIFO.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t spi_rx_fifo_read(void){
    return GetWord16(&spi->SPI_FIFO_READ_REGF);
}


/***************** SPI_FIFO_WRITE_REG Functions *****************/

/**
 ****************************************************************************************
 * @brief Write to TX FIFO. Write access is permitted only if SPI_TX_FIFO_FULL is 0.
 * @param[in] tx_data   16 bits.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_tx_fifo_write(uint16_t tx_data){
    SetWord16(&spi->SPI_FIFO_WRITE_REGF, tx_data);
}


/***************** SPI_CS_CONFIG_REG Functions *****************/

/**
 ****************************************************************************************
 * @brief Set CS output in master mode.
 * @param[in] cs_mode       CS output in master mode.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_cs_mode_setf(SPI_CS_MODE_CFG cs_mode){
    SetWord16(&spi->SPI_CS_CONFIG_REGF, cs_mode);
}

/**
 ****************************************************************************************
 * @brief Get CS output in master mode.
 * @return SPI_CS_MODE_CFG.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE SPI_CS_MODE_CFG spi_cs_mode_getf(void){
    return (SPI_CS_MODE_CFG) GetWord16(&spi->SPI_CS_CONFIG_REGF);
}


/***************** SPI_CS_CONFIG_REG Functions *****************/

/**
 ****************************************************************************************
 * @brief Write SPI_FIFO_HIGH Register.
 *        This register has to be written before the SPI_FIFO_WRITE_REG.
 * @param[in] tx_data       16 MSb when spi word is larger than 16bits.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_tx_fifo_high_write(uint16_t tx_data){
    SetWord16(&spi->SPI_FIFO_HIGH_REGF, tx_data);
}

/**
 ****************************************************************************************
 * @brief Read from SPI_FIFO_HIGH Register.
 * @return 16 MSb when spi word is larger than 16bits.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t spi_rx_fifo_high_read(void){
    return GetWord16(&spi->SPI_FIFO_HIGH_REGF);
}


/***************** SPI_TXBUFFER_FORCE_REG Functions *****************/

/**
 ****************************************************************************************
 * @brief Write SPI_TXBUFFER_FORCE_L_REG Register.
 * @param[in] tx_data       Write directly the tx buffer (2 LSB). It must to be used only
 *                          in slave mode.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_txbuffer_low_force_write(uint16_t tx_data){
    SetWord16(&spi->SPI_TXBUFFER_FORCE_L_REGF, tx_data);
}

/**
 ****************************************************************************************
 * @brief Write SPI_TXBUFFER_FORCE_H_REG Register.
 * @param[in] tx_data       Write directly the tx buffer (2 MSB). It must to be used only
 *                          in slave mode.
 *                          This register has to be written before the SPI_FIFO_WRITE_REG.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void spi_txbuffer_high_force_write(uint16_t tx_data){
    SetWord16(&spi->SPI_TXBUFFER_FORCE_H_REGF, tx_data);
}

#endif // SPI_531_H_
