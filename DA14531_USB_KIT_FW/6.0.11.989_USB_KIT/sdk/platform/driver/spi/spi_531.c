/**
 ****************************************************************************************
 *
 * @file spi_531.c
 *
 * @brief Implementation of the SPI Low Level Driver for DA14531 devices.
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

#include <stdint.h>
#include <string.h>
#include "spi.h"

/*
 * DEFINES
 *****************************************************************************************
 */

// SPI Master clock frequency
#define SPI_MASTER_CLK_FREQ_32      (32000)

/// SPI environment type
typedef struct
{
    /// SPI master/slave mode
    SPI_MS_MODE_CFG                 spi_ms:1;

    /// SPI word size (1, 2, 4 Bytes)
    SPI_WSZ_MODE_CFG                spi_wsz:4;

    /// SPI CS
    SPI_CS_MODE_CFG                 spi_cs:3;

    /// SPI send callback
    spi_cb_t                        send_cb;

    /// SPI receive callback
    spi_cb_t                        receive_cb;

    /// SPI transfer callback
    spi_cb_t                        transfer_cb;

    /// Increment according to wsz
    uint8_t                         incr;
#if defined (CFG_SPI_DMA_SUPPORT)
    /// SPI DMA RX Channel
    DMA_ID                          spi_dma_rx_channel;

    /// SPI DMA TX Channel
    DMA_ID                          spi_dma_tx_channel;

#endif
} spi_env_t;

/// SPI retained environment
static spi_env_t spi_env            __SECTION_ZERO("retention_mem_area0");

#if defined (CFG_SPI_DMA_SUPPORT)
/**
 ****************************************************************************************
 * @brief Initialize DMA for SPI
 * @param[in] spi_cfg       SPI configuration struct
 * @return void
 ****************************************************************************************
 */
static void spi_dma_init(const spi_cfg_t *spi_cfg)
{
    // Configure DMA Channel for SPI Rx operations
    spi_env.spi_dma_rx_channel = spi_cfg->spi_dma_channel == SPI_DMA_CHANNEL_01 ? DMA_CHANNEL_0 : DMA_CHANNEL_2;

    // Configure Rx DMA Channel for SPI
    dma_cfg_t dma_spi_cfg =
    {
        .bus_width =    DMA_BW_BYTE,
        .irq_enable =   DMA_IRQ_STATE_ENABLED,
        .dreq_mode =    DMA_DREQ_TRIGGERED,
        .src_inc =      DMA_INC_FALSE,
        .dst_inc =      DMA_INC_TRUE,
        .circular =     DMA_MODE_NORMAL,
        .dma_prio =     spi_cfg->spi_dma_priority,
        .dma_idle =     DMA_IDLE_BLOCKING_MODE,
        .dma_init =     DMA_INIT_AX_BX_AY_BY,
        .dma_sense =    DMA_SENSE_LEVEL_SENSITIVE,
        .dma_req_mux =  DMA_TRIG_SPI_RXTX,
        .src_address =  (uint32_t) SPI_FIFO_READ_REG,
        .irq_nr_of_trans = 0,
    };
    // Initialize Rx DMA Channel for SPI
    dma_initialize(spi_env.spi_dma_rx_channel, &dma_spi_cfg);

    // Configure Tx DMA Channel for SPI
    spi_env.spi_dma_tx_channel = spi_cfg->spi_dma_channel == SPI_DMA_CHANNEL_01 ? DMA_CHANNEL_1 : DMA_CHANNEL_3;
    dma_spi_cfg.src_inc = DMA_INC_TRUE;
    dma_spi_cfg.dst_inc = DMA_INC_FALSE;
    dma_spi_cfg.dst_address = (uint32_t) SPI_FIFO_WRITE_REG;
    // Initialize Tx DMA Channel for SPI
    dma_initialize(spi_env.spi_dma_tx_channel, &dma_spi_cfg);
}

/**
 ****************************************************************************************
 * @brief SPI DMA Send Callback function
 * @param[in] user_data     User data
 * @param[in] length        Number of words transferred
 * @return void
 ****************************************************************************************
 */
static void spi_dma_send_cb(void *user_data, uint16_t length)
{
    if (spi_env.spi_ms == SPI_MS_MODE_MASTER)
    {
        while (spi_transaction_status_getf() == SPI_TRANSACTION_IS_ACTIVE);
    }
    if (spi_env.send_cb != NULL)
        spi_env.send_cb(length);
}

/**
 ****************************************************************************************
 * @brief SPI DMA Receive Callback function
 * @param[in] user_data     User data
 * @param[in] length        Number of data items transferred
 * @return void
 ****************************************************************************************
 */
static void spi_dma_receive_cb(void *user_data, uint16_t length)
{
    if (spi_env.spi_ms == SPI_MS_MODE_MASTER)
    {
        while (spi_transaction_status_getf() == SPI_TRANSACTION_IS_ACTIVE);
    }
    if (spi_env.receive_cb != NULL)
        spi_env.receive_cb(length);
}

/**
 ****************************************************************************************
 * @brief SPI DMA Transfer Callback function
 * @param[in] user_data     User data
 * @param[in] length        Number of data items transferred
 * @return void
 ****************************************************************************************
 */
static void spi_dma_transfer_cb(void *user_data, uint16_t length)
{
    if (spi_env.spi_ms == SPI_MS_MODE_MASTER)
    {
        while (spi_transaction_status_getf() == SPI_TRANSACTION_IS_ACTIVE);
    }
    if (spi_env.transfer_cb != NULL)
        spi_env.transfer_cb(length);
}

/**
 ****************************************************************************************
 * @brief SPI send data via DMA
 * @param[in] tx_buf    Pointer to buffer with data to send to SPI.
 * @param[in] length    Number of data items to send.
 * @param[in] cb        Callback to return upon completion
 * @return void
 ****************************************************************************************
 */
static void spi_dma_send(const uint32_t *tx_buf, uint16_t length, dma_cb_t cb, DMA_IRQ_CFG irq_en)
{
    // Update DMA bus data width
    dma_set_bw(spi_env.spi_dma_tx_channel, spi_env.spi_wsz == SPI_MODE_8BIT ? DMA_BW_BYTE : DMA_BW_HALFWORD);
    // Update DMA source address
    dma_set_src(spi_env.spi_dma_tx_channel, (uint32_t) tx_buf);
    // Update Tx DMA INT
    dma_set_int(spi_env.spi_dma_tx_channel, length);
    // Update Tx DMA length
    dma_set_len(spi_env.spi_dma_tx_channel, length);
    // Update DMA callback
    dma_register_callback(spi_env.spi_dma_tx_channel, cb, NULL);
    // Enable Tx DMA
    dma_channel_start(spi_env.spi_dma_tx_channel, irq_en);
    // Enable SPI DMA Tx Path
    spi_ctrl_reg_spi_dma_tx_en_setf(SPI_BIT_EN);
}

/**
 ****************************************************************************************
 * @brief SPI receive data via DMA
 * @param[in] rx_buf    Pointer to buffer where received data from SPI will be stored.
 * @param[in] length    Number of data items to receive.
 * @param[in] cb        Callback to return upon completion
 * @return void
 ****************************************************************************************
 */
static void spi_dma_receive(const uint32_t *rx_buf, uint16_t length, dma_cb_t cb, DMA_IRQ_CFG irq_en)
{
    // Update Rx DMA bus data width
    dma_set_bw(spi_env.spi_dma_rx_channel, spi_env.spi_wsz == SPI_MODE_8BIT ? DMA_BW_BYTE : DMA_BW_HALFWORD);
    // Update Rx DMA destination address
    dma_set_dst(spi_env.spi_dma_rx_channel, (uint32_t) rx_buf);
    // Update Rx DMA INT
    dma_set_int(spi_env.spi_dma_rx_channel, length);
    // Update Rx DMA length
    dma_set_len(spi_env.spi_dma_rx_channel, length);
    // Update Rx DMA callback
    dma_register_callback(spi_env.spi_dma_rx_channel, cb, NULL);
    // Enable Rx DMA
    dma_channel_start(spi_env.spi_dma_rx_channel, irq_en);
    // Enable SPI DMA Rx Path
    spi_ctrl_reg_spi_dma_rx_en_setf(SPI_BIT_EN);
}
#endif

#if (SPI_SLAVE_SUPPORT)
/**
 ****************************************************************************************
 * @brief Receive data of 8b word length from SPI FIFO
 * @details It can be used only in slave mode. Reads the data available in the READ FIFO.
 * It also checks the status of FIFO in case of overflow.
 * @param[in] rx_buf        Pointer to the buffer where the read data will be stored
 * @param[in] length        Number of data items to receive
 * @return status           No error or overflow
 ****************************************************************************************
 */
static int8_t spi_slave_receive8(uint8_t *rx_buf, uint16_t length)
{
    uint16_t spi_fifo_status;

    for (uint16_t i = 0; i < length; i++)
    {
        do
        {
            // Read FIFO Status
            spi_fifo_status = spi_fifo_status_getf();
            // Check for overflows
            if ((spi_fifo_status & SPI_RX_FIFO_OVFL) !=0)
            {
                return SPI_STATUS_OVFL_ERR;
            }
        // Wait while Rx FIFO is empty
        } while ((spi_fifo_status & (SPI_STATUS_RX_EMPTY)) != 0);

        // Read data
        *rx_buf = spi_rx_fifo_read() & 0xFF;
        rx_buf++;
    }
    return SPI_STATUS_ERR_OK;
}

/**
 ****************************************************************************************
 * @brief Receive data of 16b word length from SPI FIFO
 * @details It can be used only in slave mode. Reads the data available in the READ FIFO.
 * It also checks the status of FIFO in case of overflow.
 * @param[in] rx_buf        Pointer to the buffer where the read data will be stored
 * @param[in] length        Number of data items to receive
 * @return status           No error or overflow
 ****************************************************************************************
 */
static int8_t spi_slave_receive16(uint16_t *rx_buf, uint16_t length)
{
    uint16_t spi_fifo_status;
    for (uint16_t i = 0; i < length; i++)
    {
        do
        {
            // Read FIFO Status
            spi_fifo_status = spi_fifo_status_getf();
            // Check for overflows
            if ((spi_fifo_status & SPI_RX_FIFO_OVFL) !=0)
            {
                return SPI_STATUS_OVFL_ERR;
            }
        // Wait while Rx FIFO is empty
        } while ((spi_fifo_status & (SPI_STATUS_RX_EMPTY)) != 0);

        // Read data
        *rx_buf = spi_rx_fifo_read() & 0xFFFF;
        rx_buf++;
    }
    return SPI_STATUS_ERR_OK;
}

/**
 ****************************************************************************************
 * @brief Receive data of 32b word length from SPI FIFO
 * @details It can be used only in slave mode. Reads the data available in the READ FIFO.
 * It also checks the status of FIFO in case of overflow.
 * @param[in] rx_buf        Pointer to the buffer where the read data will be stored
 * @param[in] length        Number of data items to receive
 * @return status           No error or overflow
 ****************************************************************************************
 */
static int8_t spi_slave_receive32(uint32_t *rx_buf, uint16_t length)
{
    uint16_t spi_fifo_status;
    for (uint16_t i = 0; i < length; i++)
    {
        do
        {
            // Read FIFO Status
            spi_fifo_status = spi_fifo_status_getf();
            // Check for overflows
            if ((spi_fifo_status & SPI_RX_FIFO_OVFL) !=0)
            {
                return SPI_STATUS_OVFL_ERR;
            }
        // Wait while Rx FIFO is empty
        } while ((spi_fifo_status & (SPI_STATUS_RX_EMPTY)) != 0);

        // Read data
        *rx_buf = spi_rx_fifo_read();
        *rx_buf += (spi_rx_fifo_high_read() << 16);
        rx_buf++;
    }
    return SPI_STATUS_ERR_OK;
}

/**
 ****************************************************************************************
 * @brief Transfer data of 8b word length from/to SPI FIFO
 * @details It can be used only in slave mode. Writes all available data to be transmitted
 * in the WRITE FIFO, until it is full. Then it waits to receive clock from the SPI master
 * to beging transmiiting the data from WRITE FIFO and stores the received data in the READ
 * FIFO. It also checks the status of FIFO in case of overflow.
 * @param[in] tx_buf        Pointer to the buffer with the data to write
 * @param[in] rx_buf        Pointer to the buffer where the read data will be stored
 * @param[in] length        Number of data items to transfer
 * @return status           No error or overflow
 ****************************************************************************************
 */
static int8_t spi_slave_transfer8(const uint8_t *tx_buf, uint8_t *rx_buf, uint16_t length)
{
    // Set index to 1, since first word has been written to SPI_TXBUFFER_FORCE_L_REG
    uint16_t tx_index = 1;
    uint16_t spi_fifo_status = 0;

    // Write TX FIFO until it is full. Don't wait for SPI Master clock.
    while ((tx_index < length) && (spi_tx_fifo_full_status_getf() != SPI_TX_FIFO_IS_FULL))
    {
        spi_tx_fifo_write(*tx_buf);
        tx_buf++;
        tx_index++;
    }

    // Write rest of the data to FIFO, when SPI Master starts reading
    for (uint16_t i = 0; i < length; i++)
    {
        // If there are remaining tx data, write them in Tx FIFO, if there is space.
        if ((tx_index < length) && (spi_tx_fifo_full_status_getf() != SPI_TX_FIFO_IS_FULL))
        {
            spi_tx_fifo_write(*tx_buf);
            tx_buf++;
            tx_index++;
        }
        do
        {
            spi_fifo_status = spi_fifo_status_getf();
            // Check for overflows
            if ((spi_fifo_status & SPI_RX_FIFO_OVFL) !=0)
            {
                return SPI_STATUS_OVFL_ERR;
            }
        // Wait while Rx FIFO is empty
        } while ((spi_fifo_status & (SPI_STATUS_RX_EMPTY)) != 0);

        // Read data
        *rx_buf = spi_rx_fifo_read() & 0xFF;
        rx_buf++;
    }
    return SPI_STATUS_ERR_OK;
}

/**
 ****************************************************************************************
 * @brief Transfer data of 16b word length from/to SPI FIFO
 * @details It can be used only in slave mode. Writes all available data to be transmitted
 * in the WRITE FIFO, until it is full. Then it waits to receive clock from the SPI master
 * to beging transmiiting the data from WRITE FIFO and stores the received data in the READ
 * FIFO. It also checks the status of FIFO in case of overflow.
 * @param[in] tx_buf        Pointer to the buffer with the data to write
 * @param[in] rx_buf        Pointer to the buffer where the read data will be stored
 * @param[in] length        Number of data items to transfer
 * @return status           No error or overflow
 ****************************************************************************************
 */
static int8_t spi_slave_transfer16(const uint16_t *tx_buf, uint16_t *rx_buf, uint16_t length)
{
    // Set index to 1, since first word has been written to SPI_TXBUFFER_FORCE_L_REG
    uint16_t tx_index = 1;
    uint16_t spi_fifo_status = 0;

    // Write TX FIFO until it is full. Don't wait for SPI Master clock.
    while ((tx_index < length) && (spi_tx_fifo_full_status_getf() != SPI_TX_FIFO_IS_FULL))
    {
        spi_tx_fifo_write(*tx_buf);
        tx_buf++;
        tx_index++;
    }

    // Write rest of the data to FIFO, when SPI Master starts reading
    for (uint16_t i = 0; i < length; i++)
    {
        // If there are remaining tx data, write them in Tx FIFO, if there is space.
        if ((tx_index < length) && (spi_tx_fifo_full_status_getf() != SPI_TX_FIFO_IS_FULL))
        {
            spi_tx_fifo_write(*tx_buf);
            tx_buf++;
            tx_index++;
        }
        do
        {
            spi_fifo_status = spi_fifo_status_getf();
            // Check for overflows
            if ((spi_fifo_status & SPI_RX_FIFO_OVFL) !=0)
            {
                return SPI_STATUS_OVFL_ERR;
            }
        // Wait while Rx FIFO is empty
        } while ((spi_fifo_status & (SPI_STATUS_RX_EMPTY)) != 0);

        // Read data
        *rx_buf = spi_rx_fifo_read() & 0xFFFF;
        rx_buf++;
    }
    return SPI_STATUS_ERR_OK;
}

/**
 ****************************************************************************************
 * @brief Transfer data of 32b word length from/to SPI FIFO
 * @details It can be used only in slave mode. Writes all available data to be transmitted
 * in the WRITE FIFO, until it is full. Then it waits to receive clock from the SPI master
 * to beging transmiiting the data from WRITE FIFO and stores the received data in the READ
 * FIFO. It also checks the status of FIFO in case of overflow.
 * @param[in] tx_buf        Pointer to the buffer with the data to write
 * @param[in] rx_buf        Pointer to the buffer where the read data will be stored
 * @param[in] length        Number of data items to transfer
 * @return status           No error or overflow
 ****************************************************************************************
 */
static int8_t spi_slave_transfer32(const uint32_t *tx_buf, uint32_t *rx_buf, uint16_t length)
{
    // Set index to 1, since first word has been written to SPI_TXBUFFER_FORCE_L_REG
    uint16_t tx_index = 1;
    uint16_t spi_fifo_status = 0;

    // Write TX FIFO until it is full. Don't wait for SPI Master clock.
    while ((tx_index < length) && (spi_tx_fifo_full_status_getf() != SPI_TX_FIFO_IS_FULL))
    {
        spi_tx_fifo_high_write(*tx_buf >> 16);
        spi_tx_fifo_write(*tx_buf & 0xFFFF);
        tx_buf++;
        tx_index++;
    }

    // Write rest of the data to FIFO, when SPI Master starts reading
    for (uint16_t i = 0; i < length; i++)
    {
        // If there are remaining tx data, write them in Tx FIFO, if there is space.
        if ((tx_index < length) && (spi_tx_fifo_full_status_getf() != SPI_TX_FIFO_IS_FULL))
        {
            spi_tx_fifo_high_write(*tx_buf >> 16);
            spi_tx_fifo_write(*tx_buf & 0xFFFF);
            tx_buf++;
            tx_index++;
        }
        do
        {
            spi_fifo_status = spi_fifo_status_getf();
            // Check for overflows
            if ((spi_fifo_status & SPI_RX_FIFO_OVFL) !=0)
            {
                return SPI_STATUS_OVFL_ERR;
            }
        // Wait while Rx FIFO is empty
        } while ((spi_fifo_status & (SPI_STATUS_RX_EMPTY)) != 0);

        // Read data
        *rx_buf = spi_rx_fifo_read();
        *rx_buf += (spi_rx_fifo_high_read() << 16);
        rx_buf++;
    }
    return SPI_STATUS_ERR_OK;
}

/**
 ****************************************************************************************
 * @brief SPI Slave send data
 * @param[in] tx_buf    Pointer to buffer where received data from SPI will be stored.
 * @param[in] length    Number of data items to receive.
 * @param[in] op        Blocking, interrupt-driven or DMA-driven operation.
 * @return status
 ****************************************************************************************
 */
static int8_t spi_slave_send(const void *tx_buf, uint16_t length, SPI_OP_CFG op)
{
    const uint8_t *tx_data = (uint8_t *) tx_buf;

    // Clear Tx, Rx and DMA enable paths in Control Register
    spi_ctrl_reg_clear_enable_setf();
    // Enable TX path
    spi_ctrl_reg_spi_tx_en_setf(SPI_BIT_EN);
    // Enable SPI
    spi_ctrl_reg_spi_en_setf(SPI_BIT_EN);

    // Write Force Register
    if (spi_env.spi_wsz > SPI_MODE_16BIT)
    {
        spi_txbuffer_high_force_write((*(uint32_t *) tx_data) >> 16);
    }
    spi_txbuffer_low_force_write(*((uint16_t *) tx_data));
    tx_data += spi_env.incr;

    if (op == SPI_OP_BLOCKING)
    {
        // Write FIFO
        for (uint16_t i = 0; i < length - 1; i++)
        {
            // Wait until TX-FIFO is not full
            while (spi_tx_fifo_full_status_getf() == SPI_TX_FIFO_IS_FULL);

            if (spi_env.spi_wsz > SPI_MODE_16BIT)
            {
                spi_tx_fifo_high_write((*(uint32_t *) tx_data) >> 16);
            }
            if (spi_env.spi_wsz > SPI_MODE_8BIT)
            {
                spi_tx_fifo_write(* (uint16_t *) tx_data & 0xFFFF);
            }
            else
            {
                spi_tx_fifo_write(* (uint8_t *) tx_data & 0xFF);
            }
            tx_data += spi_env.incr;
        }

        // Wait while TX FIFO is not empty
        while (spi_tx_fifo_level_getf() != 0);
        return SPI_STATUS_ERR_OK;
    }

#if defined (CFG_SPI_DMA_SUPPORT)
    // Enable Tx DMA
    spi_dma_send((uint32_t *) tx_data, length - 1, spi_dma_send_cb, DMA_IRQ_STATE_ENABLED);
#endif // SPI_DMA_SUPPORT

    return SPI_STATUS_ERR_OK;
}

/**
 ****************************************************************************************
 * @brief SPI Slave receive data
 * @param[in] rx_buf    Pointer to buffer with data to receive from SPI. It can be a
 *                      pointer to uint8_t, uint16_t or uint32_t data type depending on SPI
 *                      configuration.
 * @param[in] length    Number of data items to receive
 * @param[in] op        Blocking, interrupt-driven or DMA-driven operation.
 * @return status
 * @note In 16 and 32 bitmodes data are transferred in big endian format.
 * @note DMA operation is supported only for 8 and 16 bitmode transfers.
 ****************************************************************************************
 */
static int8_t spi_slave_receive(void *rx_buf, uint16_t length, SPI_OP_CFG op)
{
    // Clear Tx, Rx and DMA enable paths in Control Register
    spi_ctrl_reg_clear_enable_setf();
    // Enable RX path
    spi_ctrl_reg_spi_rx_en_setf(SPI_BIT_EN);

    // Blocking mode. No interrupts, no DMA
    if (op == SPI_OP_BLOCKING || spi_env.spi_wsz == SPI_MODE_32BIT)
    {
        // Enable SPI
        spi_ctrl_reg_spi_en_setf(SPI_BIT_EN);

        // Read
        switch (spi_env.spi_wsz)
        {
            case (SPI_MODE_8BIT):
                return spi_slave_receive8((uint8_t *) rx_buf, length);
            case (SPI_MODE_16BIT):
                return spi_slave_receive16((uint16_t *) rx_buf, length);
            case (SPI_MODE_32BIT):
                return spi_slave_receive32((uint32_t *) rx_buf, length);
            default:
                return SPI_STATUS_CFG_ERR;
        }
    }

#if defined (CFG_SPI_DMA_SUPPORT)
    else
    {
        // Enable Rx DMA
        spi_dma_receive((uint32_t *) rx_buf, length, spi_dma_receive_cb, DMA_IRQ_STATE_ENABLED);
        // Enable SPI
        spi_ctrl_reg_spi_en_setf(SPI_BIT_EN);
        return SPI_STATUS_ERR_OK;
    }
#else
    return SPI_STATUS_CFG_ERR;
#endif
}

/**
 ****************************************************************************************
 * @brief SPI Slave transfer data.
 * @param[in] tx_buf    Pointer to buffer with data to send to SPI. It can be a pointer to
 *                      uint8_t, uint16_t or uint32_t data type depending on SPI
 *                      configuration.
 * @param[in] rx_buf    Pointer to buffer with data to receive from SPI. It can be a
 *                      pointer to uint8_t, uint16_t or uint32_t data type depending on SPI
 *                      configuration.
 * @param[in] length    Number of data items to transfer
 * @param[in] op        Blocking, interrupt-driven or DMA-driven operation.
 * @return status
 * @note In 16 and 32 bitmodes data are transferred in big endian format.
 * @note DMA operation is supported only for 8 and 16 bitmode transfers.
 ****************************************************************************************
 */
static int8_t spi_slave_transfer(const void *tx_buf, void *rx_buf, uint16_t length, SPI_OP_CFG op)
{
    const uint8_t *tx_data = tx_buf;

    // Clear Tx, Rx and DMA enable paths in Control Register
    spi_ctrl_reg_clear_enable_setf();
    // Enable TX path
    spi_ctrl_reg_spi_tx_en_setf(SPI_BIT_EN);
    // Enable RX path
    spi_ctrl_reg_spi_rx_en_setf(SPI_BIT_EN);
    // Enable SPI
    spi_ctrl_reg_spi_en_setf(SPI_BIT_EN);

    // Force write the first data
    switch (spi_env.spi_wsz)
    {
        case (SPI_MODE_8BIT):
        {
            spi_txbuffer_low_force_write(*tx_data);
            tx_data += 1;
            if (op == SPI_OP_BLOCKING)
            {
                return spi_slave_transfer8((uint8_t *) tx_data, (uint8_t *) rx_buf, length);
            }
            break;
        }
        case (SPI_MODE_16BIT):
        {
            spi_txbuffer_low_force_write(*((uint16_t *) tx_data));
            tx_data += 2;
            if (op == SPI_OP_BLOCKING)
            {
                return spi_slave_transfer16((uint16_t *) tx_data, (uint16_t *) rx_buf, length);
            }
            break;
        }
        case (SPI_MODE_32BIT):
        {
            spi_txbuffer_high_force_write((*(uint32_t *) tx_data) >> 16);
            spi_txbuffer_low_force_write(*((uint16_t *) tx_data));
            tx_data += 4;
            if (op == SPI_OP_BLOCKING)
            {
                return spi_slave_transfer32((uint32_t *) tx_data, (uint32_t *) rx_buf, length);
            }
            break;
        }
        default:
            return SPI_STATUS_CFG_ERR;
    }

#if defined (CFG_SPI_DMA_SUPPORT)
    // Enable Rx DMA
    spi_dma_receive((uint32_t *) rx_buf, length, spi_dma_transfer_cb, DMA_IRQ_STATE_ENABLED);
    // Enable Tx DMA
    spi_dma_send((uint32_t *) tx_data, length - 1, NULL, DMA_IRQ_STATE_DISABLED);
#endif
    return SPI_STATUS_ERR_OK;
}

#endif // SPI_SLAVE_SUPPORT

/**
 ****************************************************************************************
 * @brief SPI Master send data
 * @param[in] tx_buf    Pointer to buffer where received data from SPI will be stored.
 * @param[in] length    Number of data items to receive.
 * @param[in] op        Blocking, interrupt-driven or DMA-driven operation.
 * @return status
 ****************************************************************************************
 */
static int8_t spi_master_send(const void *tx_buf, uint16_t length, SPI_OP_CFG op)
{
    // Clear Tx, Rx and DMA enable paths in Control Register
    spi_ctrl_reg_clear_enable_setf();

    // Enable TX path
    spi_ctrl_reg_spi_tx_en_setf(SPI_BIT_EN);

#if defined (CFG_SPI_DMA_SUPPORT)
    // Non-blocking mode. Interrupt driven + DMA
    if ((op == SPI_OP_DMA) && (spi_env.spi_wsz != SPI_MODE_32BIT))
    {
        dma_set_ainc(spi_env.spi_dma_tx_channel, DMA_INC_TRUE);
        spi_dma_send((uint32_t *) tx_buf, length, spi_dma_send_cb, DMA_IRQ_STATE_ENABLED);
        // Enable SPI
        spi_ctrl_reg_spi_en_setf(SPI_BIT_EN);
    }
    else
#endif
    // Blocking mode. No interrupts, no DMA
    {
        const uint8_t *tx_data = (uint8_t *) tx_buf;

        // Enable SPI
        spi_ctrl_reg_spi_en_setf(SPI_BIT_EN);

        for (uint16_t i = 0; i < length; i++)
        {
            // Wait until TX-FIFO is not full
            while (spi_tx_fifo_full_status_getf() == SPI_TX_FIFO_IS_FULL);

            if (spi_env.spi_wsz > SPI_MODE_16BIT)
            {
                spi_tx_fifo_high_write((*(uint32_t *) tx_data) >> 16);
            }
            if (spi_env.spi_wsz > SPI_MODE_8BIT)
            {
                spi_tx_fifo_write(*(uint16_t *) tx_data & 0xFFFF);
            }
            else
            {
                spi_tx_fifo_write(*(uint8_t *) tx_data & 0xFF);
            }
            tx_data += spi_env.incr;
        }

        // Wait until transaction is finished and SPI is not busy
        while (spi_transaction_status_getf() == SPI_TRANSACTION_IS_ACTIVE);
    }
    return SPI_STATUS_ERR_OK;
}

/**
 ****************************************************************************************
 * @brief SPI Master receive data
 * @param[in] rx_buf    Pointer to buffer with data to receive from SPI. It can be a
 *                      pointer to uint8_t, uint16_t or uint32_t data type depending on SPI
 *                      configuration.
 * @param[in] length    Number of data items to receive
 * @param[in] op        Blocking, interrupt-driven or DMA-driven operation.
 * @return status
 * @note In 16 and 32 bitmodes data are transferred in big endian format.
 * @note DMA operation is supported only for 8 and 16 bitmode transfers.
 ****************************************************************************************
 */
static int8_t spi_master_receive(void *rx_buf, uint16_t length, SPI_OP_CFG op)
{
    uint8_t *rx_data8 = (uint8_t *) rx_buf;
    uint16_t *rx_data16 = (uint16_t *) rx_buf;
    uint32_t *rx_data32 = (uint32_t *) rx_buf;

    // Clear Tx, Rx and DMA enable paths in Control Register
    spi_ctrl_reg_clear_enable_setf();
    // Enable TX path
    spi_ctrl_reg_spi_tx_en_setf(SPI_BIT_EN);
    // Enable RX path
    spi_ctrl_reg_spi_rx_en_setf(SPI_BIT_EN);

#if defined (CFG_SPI_DMA_SUPPORT)
    // Non-blocking mode. Interrupt driven + DMA
    if ((op == SPI_OP_DMA) && (spi_env.spi_wsz != SPI_MODE_32BIT))
    {
        uint8_t tx_data[1] = {0};

        // Enable Rx DMA
        spi_dma_receive((uint32_t *) rx_buf, length, spi_dma_receive_cb, DMA_IRQ_STATE_ENABLED);

        // Enable Tx DMA (Set AINC to false, to keep DO line to 0)
        dma_set_ainc(spi_env.spi_dma_tx_channel, DMA_INC_FALSE);
        spi_dma_send((uint32_t *) tx_data, length, NULL, DMA_IRQ_STATE_DISABLED);

        // Enable SPI
        spi_ctrl_reg_spi_en_setf(SPI_BIT_EN);
    }
    else
#endif
    // Non-blocking mode. Interrupt driven + DMA.
    {
        // Enable SPI
        spi_ctrl_reg_spi_en_setf(SPI_BIT_EN);

        for (uint16_t i = 0; i < length; i++)
        {
            // Wait until TX-FIFO is not full
            while (spi_tx_fifo_full_status_getf() == SPI_TX_FIFO_IS_FULL);

            // Dummy write 0
            spi_tx_fifo_high_write(0);
            spi_tx_fifo_write(0);

            // Wait while RX FIFO is empty
            while (spi_rx_fifo_empty_status_getf() == SPI_RX_FIFO_IS_EMPTY);
            // Read data
            if (spi_env.spi_wsz == SPI_MODE_8BIT)
            {
                *rx_data8++ = spi_rx_fifo_read() & 0xFF;
            }
            else if (spi_env.spi_wsz == SPI_MODE_16BIT)
            {
                *rx_data16++ = spi_rx_fifo_read();
            }
            else
            {
                *rx_data32 = spi_rx_fifo_read();
                *rx_data32 += (spi_rx_fifo_high_read() << 16);
                rx_data32++;
            }
        }

        // Wait until transaction is finished and SPI is not busy
        while (spi_transaction_status_getf() == SPI_TRANSACTION_IS_ACTIVE);
    }
    return SPI_STATUS_ERR_OK;
}

/**
 ****************************************************************************************
 * @brief SPI Master transfer data.
 * @param[in] tx_buf    Pointer to buffer with data to send to SPI. It can be a pointer to
 *                      uint8_t, uint16_t or uint32_t data type depending on SPI
 *                      configuration.
 * @param[in] rx_buf    Pointer to buffer with data to receive from SPI. It can be a
 *                      pointer to uint8_t, uint16_t or uint32_t data type depending on SPI
 *                      configuration.
 * @param[in] length    Number of data items to transfer
 * @param[in] op        Blocking, interrupt-driven or DMA-driven operation.
 * @return status
 * @note In 16 and 32 bitmodes data are transferred in big endian format.
 * @note DMA operation is supported only for 8 and 16 bitmode transfers.
 ****************************************************************************************
 */
int8_t spi_master_transfer(const void *tx_buf, void *rx_buf, uint16_t length, SPI_OP_CFG op)
{
    const uint8_t * tx_data = (uint8_t *) tx_buf;
    uint8_t *rx_data8 = (uint8_t *) rx_buf;
    uint16_t *rx_data16 = (uint16_t *) rx_buf;
    uint32_t *rx_data32 = (uint32_t *) rx_buf;

    // Clear Tx, Rx and DMA enable paths in Control Register
    spi_ctrl_reg_clear_enable_setf();
    // Enable TX path
    spi_ctrl_reg_spi_tx_en_setf(SPI_BIT_EN);
    // Enable RX path
    spi_ctrl_reg_spi_rx_en_setf(SPI_BIT_EN);

#if defined (CFG_SPI_DMA_SUPPORT)
    // Non-blocking mode. Interrupt driven + DMA
    if ((op == SPI_OP_DMA) && (spi_env.spi_wsz != SPI_MODE_32BIT))
    {
        // Enable Rx DMA
        spi_dma_receive((uint32_t *) rx_buf, length, spi_dma_transfer_cb, DMA_IRQ_STATE_ENABLED);
        // Enable Tx DMA
        dma_set_ainc(spi_env.spi_dma_tx_channel, DMA_INC_TRUE);
        spi_dma_send((uint32_t *) tx_buf, length, NULL, DMA_IRQ_STATE_DISABLED);
        // Enable SPI
        spi_ctrl_reg_spi_en_setf(SPI_BIT_EN);
    }
    else
#endif
    // Non-blocking mode. Interrupt driven + DMA.
    {
        // Enable SPI
        spi_ctrl_reg_spi_en_setf(SPI_BIT_EN);

        for (uint16_t i = 0; i < length; i++)
        {
            // Wait until TX-FIFO is not full
            while (spi_tx_fifo_full_status_getf() == SPI_TX_FIFO_IS_FULL);

            if (spi_env.spi_wsz > SPI_MODE_16BIT)
            {
                spi_tx_fifo_high_write((*(uint32_t *) tx_data) >> 16);
            }
            if (spi_env.spi_wsz > SPI_MODE_8BIT)
            {
                spi_tx_fifo_write(* (uint16_t *) tx_data & 0xFFFF);
            }
            else
            {
                spi_tx_fifo_write(* (uint8_t *) tx_data & 0xFF);
            }
            tx_data += spi_env.incr;

            // Wait while RX FIFO is empty
            while (spi_rx_fifo_empty_status_getf() == SPI_RX_FIFO_IS_EMPTY);
            // Read data
            if (spi_env.spi_wsz == SPI_MODE_8BIT)
            {
                *rx_data8++ = spi_rx_fifo_read() & 0xFF;
            }
            else if (spi_env.spi_wsz == SPI_MODE_16BIT)
            {
                *rx_data16++ = spi_rx_fifo_read();
            }
            else
            {
                *rx_data32 = spi_rx_fifo_read();
                *rx_data32 += (spi_rx_fifo_high_read() << 16);
                rx_data32++;
            }
        }

        // Wait until transaction is finished and SPI is not busy
        while (spi_transaction_status_getf() == SPI_TRANSACTION_IS_ACTIVE);
    }
    return SPI_STATUS_ERR_OK;
}

/**
 ****************************************************************************************
 * @brief Set SPI speed.
 * @param[in] spi_speed             SPI_SPEED_MODE_CFG
 * @param[in] spi_master_clk_mode   SPI_MASTER_CLK_MODE_CFG
 * @return void
 ****************************************************************************************
 */
static void spi_set_speed(SPI_SPEED_MODE_CFG spi_speed)
{
    // Set SPI master clock speed
    if (spi_speed == SPI_SPEED_MODE_32MHz)
    {
        spi_clock_reg_clk_div_setf(0x7F);
    }
    else
    {
        spi_clock_reg_clk_div_setf((SPI_MASTER_CLK_FREQ_32 / ( 2 * spi_speed))-1);
    }
}


/********************** Exposed API **********************/
void spi_enable(void)
{
    // Disable interrupts
    GLOBAL_INT_DISABLE();
    // Enable clock for SPI peripheral
    SetBits16(CLK_PER_REG, SPI_ENABLE, SPI_BIT_EN);
    // Enable interrupts
    GLOBAL_INT_RESTORE();
}

void spi_disable(void)
{
    // Disable interrupts
    GLOBAL_INT_DISABLE();
    // Disable clock for SPI peripheral
    SetBits16(CLK_PER_REG, SPI_ENABLE, SPI_BIT_DIS);
    // Enable interrupts
    GLOBAL_INT_RESTORE();
}

void spi_set_cp_mode(SPI_CP_MODE_CFG spi_cp)
{
    SetBits16(&spi->SPI_CONFIG_REGF, SPI_MODE, spi_cp);
}

void spi_set_bitmode(SPI_WSZ_MODE_CFG spi_wsz)
{
    switch (spi_wsz)
    {
        case(SPI_MODE_8BIT):
        {
            SetBits16(&spi->SPI_CONFIG_REGF, SPI_WORD_LENGTH, 7);
            spi_env.incr = 1;
            break;
        }
        case(SPI_MODE_16BIT):
        {
            SetBits16(&spi->SPI_CONFIG_REGF, SPI_WORD_LENGTH, 15);
            spi_env.incr = 2;
            break;
        }
        case(SPI_MODE_32BIT):
        {
            SetBits16(&spi->SPI_CONFIG_REGF, SPI_WORD_LENGTH, 31);
            spi_env.incr = 4;
            break;
        }
        default:
        {
            SetBits16(&spi->SPI_CONFIG_REGF, SPI_WORD_LENGTH, 7);
            spi_env.incr = 1;
            break;
        }
    }
    spi_env.spi_wsz = spi_wsz;
}

void spi_set_irq_mode(SPI_IRQ_CFG irq_mode)
{
}

void spi_set_cs_mode(SPI_CS_MODE_CFG spi_cs)
{
    spi_env.spi_cs = spi_cs;
}

void spi_register_send_cb(spi_cb_t cb)
{
    spi_env.send_cb = cb;
}

void spi_register_receive_cb(spi_cb_t cb)
{
    spi_env.receive_cb = cb;
}

void spi_register_transfer_cb(spi_cb_t cb)
{
    spi_env.transfer_cb = cb;
}

int8_t spi_initialize(const spi_cfg_t *spi_cfg)
{
    // Enable Clock for SPI
    spi_enable();

    // Disable SPI / Reset FIFO in SPI Control Register
    spi_ctrl_reg_setf(SPI_FIFO_RESET);

    // Set SPI Word length
    spi_set_bitmode(spi_cfg->spi_wsz);

    // Set SPI Mode (CPOL, CPHA)
    spi_set_cp_mode(spi_cfg->spi_cp);

    // Set SPI Master/Slave mode
    spi_config_reg_ms_mode_setf(spi_cfg->spi_ms);
    spi_env.spi_ms = spi_cfg->spi_ms;

    // Set SPI FIFO threshold levels to 0
    SetWord16(&spi->SPI_FIFO_CONFIG_REGF, 0);

    // Set SPI clock in async mode (mandatory)
    spi_clock_reg_master_clk_mode_setf();

    if (spi_cfg->spi_ms == SPI_MS_MODE_MASTER)
    {
        // Set CS mode
        spi_env.spi_cs = spi_cfg->spi_cs;

        // Set SPI master clock speed
        spi_set_speed(spi_cfg->spi_speed);

        // Set SPI clock edge capture data
        spi_ctrl_reg_capture_next_edge_setf(spi_cfg->spi_capture);
    }

#if defined (CFG_SPI_DMA_SUPPORT)
    // Register callbacks
    spi_env.send_cb = spi_cfg->send_cb;
    spi_env.receive_cb = spi_cfg->receive_cb;
    spi_env.transfer_cb = spi_cfg->transfer_cb;

    // Initialize DMA for SPI
    spi_dma_init(spi_cfg);
#endif

    // Disable FIFO reset
    spi_ctrl_reg_spi_fifo_reset_setf(SPI_BIT_DIS);

    return SPI_STATUS_ERR_OK;
}

void spi_cs_low(void)
{
    spi_ctrl_reg_spi_fifo_reset_setf(SPI_BIT_DIS);
    spi_cs_mode_setf(spi_env.spi_cs);
}

void spi_cs_high(void)
{
    spi_cs_mode_setf(SPI_CS_NONE);
    spi_ctrl_reg_spi_fifo_reset_setf(SPI_BIT_EN);
}

int8_t spi_send(const void *data, uint16_t num, SPI_OP_CFG op)
{
    if (spi_env.spi_ms == SPI_MS_MODE_MASTER)
    {
        return spi_master_send(data, num, op);
    }

#if (SPI_SLAVE_SUPPORT)
    if (spi_env.spi_ms == SPI_MS_MODE_SLAVE)
    {
        return spi_slave_send(data, num, op);
    }
#endif // SPI_SLAVE_SUPPORT
    return SPI_STATUS_CFG_ERR;
}

int8_t spi_receive(void *data, uint16_t num, SPI_OP_CFG op)
{
    if (spi_env.spi_ms == SPI_MS_MODE_MASTER)
    {
        return spi_master_receive(data, num, op);
    }

#if (SPI_SLAVE_SUPPORT)
    if (spi_env.spi_ms == SPI_MS_MODE_SLAVE)
    {
        return spi_slave_receive(data, num, op);
    }
#endif // SPI_SLAVE_SUPPORT
    return SPI_STATUS_CFG_ERR;
}

int8_t spi_transfer(const void *data_out, void *data_in, uint16_t num, SPI_OP_CFG op)
{
    if (spi_env.spi_ms == SPI_MS_MODE_MASTER)
    {
        return spi_master_transfer(data_out, data_in, num, op);
    }

#if (SPI_SLAVE_SUPPORT)
    if (spi_env.spi_ms == SPI_MS_MODE_SLAVE)
    {
        return spi_slave_transfer(data_out, data_in, num, op);
    }
#endif // SPI_SLAVE_SUPPORT
    return SPI_STATUS_CFG_ERR;
}

uint32_t spi_access(uint32_t dataToSend)
{
    uint32_t dataRead = 0;

    // Clear Tx, Rx and DMA enable paths in Control Register
    spi_ctrl_reg_clear_enable_setf();
    // Enable TX path
    spi_ctrl_reg_spi_tx_en_setf(SPI_BIT_EN);
    // Enable RX path
    spi_ctrl_reg_spi_rx_en_setf(SPI_BIT_EN);
    // Enable SPI
    spi_ctrl_reg_spi_en_setf(SPI_BIT_EN);

    // Check if bitmode is set in 9-bit or 32-bit
    if (spi_env.spi_wsz >= SPI_MODE_32BIT)
    {
        // Write high part of dataToSend
        spi_tx_fifo_high_write((uint16_t) (dataToSend >> 16));
    }

    // Write (low part of) dataToSend
    spi_tx_fifo_write((uint16_t) dataToSend);

    // Wait while RX FIFO is empty
    while (spi_rx_fifo_empty_status_getf() == SPI_RX_FIFO_IS_EMPTY);

    // Check if bitmode is > 16 bit
    if (spi_env.spi_wsz > SPI_MODE_16BIT)
    {
        // Read high part of data from spi slave
        dataRead = (spi_rx_fifo_high_read() << 16);
    }

    // Read (low part of) data from spi slave
    if (spi_env.spi_wsz == SPI_MODE_8BIT)
    {
        dataRead = spi_rx_fifo_read() & 0xFF;
    }
    else
    {
        dataRead += spi_rx_fifo_read() & 0xFFFF;
    }

    // Wait until transaction is finished and SPI is not busy
    while (spi_transaction_status_getf() == SPI_TRANSACTION_IS_ACTIVE);
    return dataRead;
}

uint32_t spi_transaction(uint32_t dataToSend)
{
    uint32_t dataRead = 0;

    spi_cs_low();
    // Basic SPI access
    dataRead = spi_access(dataToSend);

    spi_cs_high();
    // Return data read from spi slave
    return dataRead;
}
