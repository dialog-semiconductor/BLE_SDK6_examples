/**
 ****************************************************************************************
 *
 * @file spi_58x.c
 *
 * @brief Implementation of the SPI Low Level Driver for DA14585 and DA14586 devices.
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
#include "gpio.h"

/*
 * DEFINES
 *****************************************************************************************
 */

/// SPI environment type
typedef struct
{
    /// SPI CS configuration Pad configuration
    SPI_Pad_t                       cs_pad;

#if defined (CFG_SPI_DMA_SUPPORT)
    /// SPI send callback
    spi_cb_t                        send_cb;

    /// SPI receive callback
    spi_cb_t                        receive_cb;

    /// SPI transfer callback
    spi_cb_t                        transfer_cb;

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
        .src_address =  (uint32_t) SPI_RX_TX_REG0,
        .irq_nr_of_trans = 0,
    };
    // Initialize Rx DMA Channel for SPI
    dma_initialize(spi_env.spi_dma_rx_channel, &dma_spi_cfg);

    // Configure Tx DMA Channel for SPI
    spi_env.spi_dma_tx_channel = spi_cfg->spi_dma_channel == SPI_DMA_CHANNEL_01 ? DMA_CHANNEL_1 : DMA_CHANNEL_3;
    dma_spi_cfg.src_inc = DMA_INC_TRUE;
    dma_spi_cfg.dst_inc = DMA_INC_FALSE;
    dma_spi_cfg.dst_address = (uint32_t) SPI_RX_TX_REG0;
    // Initialize Tx DMA Channel for SPI
    dma_initialize(spi_env.spi_dma_tx_channel, &dma_spi_cfg);
}

/**
 ****************************************************************************************
 * @brief SPI DMA Master Send Callback function.
 * @param[in] user_data     User data (Not used by SPI LLD).
 * @param[in] length        Number of words transferred.
 * @return void
 ****************************************************************************************
 */
static void spi_dma_send_cb(void *user_data, uint16_t length)
{
    // Wait while SPI is busy with a transfer
    while (GetBits16(&spi->SPI_CTRL_REG1F, SPI_BUSY) == 1);
    if (spi_env.send_cb != NULL)
        spi_env.send_cb(length);
}

/**
 ****************************************************************************************
 * @brief SPI DMA Master Receive Callback function.
 * @param[in] user_data     User data (Not used by SPI LLD).
 * @param[in] length        Number of data items transferred.
 * @return void
 ****************************************************************************************
 */
static void spi_dma_receive_cb(void *user_data, uint16_t length)
{
    // Wait while SPI is busy with a transfer
    while (GetBits16(&spi->SPI_CTRL_REG1F, SPI_BUSY) == 1);
    if (spi_env.receive_cb != NULL)
        spi_env.receive_cb(length);
}

/**
 ****************************************************************************************
 * @brief SPI DMA Master Transfer Callback function.
 * @param[in] user_data     User data (Not used by SPI LLD).
 * @param[in] length        Number of data items transferred.
 * @return void
 ****************************************************************************************
 */
static void spi_dma_transfer_cb(void *user_data, uint16_t length)
{
    // Wait while SPI is busy with a transfer
    while (GetBits16(&spi->SPI_CTRL_REG1F, SPI_BUSY) == 1);
    if (spi_env.transfer_cb != NULL)
        spi_env.transfer_cb(length);
}

/**
 ****************************************************************************************
 * @brief SPI send data via DMA.
 * @param[in] tx_buf    Pointer to buffer with data to send to SPI.
 * @param[in] length    Number of data items to send.
 * @param[in] cb        Callback to return upon completion.
 * @return void
 ****************************************************************************************
 */
static void spi_dma_send(const uint32_t *tx_buf, uint16_t length, dma_cb_t cb)
{
    // Update Tx DMA to normal copy mode
    dma_set_ainc(spi_env.spi_dma_tx_channel, DMA_INC_TRUE);

    // Update DMA bus data width
    dma_set_bw(spi_env.spi_dma_tx_channel,
               GetBits16(&spi->SPI_CTRL_REGF, SPI_WORD) == SPI_MODE_8BIT ? DMA_BW_BYTE : DMA_BW_HALFWORD);

    // Update DMA source address
    dma_set_src(spi_env.spi_dma_tx_channel, (uint32_t) tx_buf);

    // Update Tx DMA INT
    dma_set_int(spi_env.spi_dma_tx_channel, length);

    // Update Tx DMA length
    dma_set_len(spi_env.spi_dma_tx_channel, length);

    // Update DMA callback
    dma_register_callback(spi_env.spi_dma_tx_channel, cb, NULL);

    // Start DMA
    dma_channel_start(spi_env.spi_dma_tx_channel, DMA_IRQ_STATE_ENABLED);

    return;
}

/**
 ****************************************************************************************
 * @brief SPI receive data via DMA.
 * @param[in] rx_buf    Pointer to buffer where received data from SPI will be stored.
 * @param[in] length    Number of data items to receive.
 * @param[in] cb        Callback to return upon completion
 * @return void
 ****************************************************************************************
 */
static void spi_dma_receive(const uint32_t *rx_buf, uint16_t length, dma_cb_t cb)
{
    uint8_t tx_data[1] = {0};
    DMA_BW_CFG wsz = GetBits16(&spi->SPI_CTRL_REGF, SPI_WORD) == SPI_MODE_8BIT ? DMA_BW_BYTE : DMA_BW_HALFWORD;

    // Update Rx DMA bus data width
    dma_set_bw(spi_env.spi_dma_rx_channel, wsz);

    // Update Rx DMA destination address
    dma_set_dst(spi_env.spi_dma_rx_channel, (uint32_t) rx_buf);

    // Update Rx DMA INT
    dma_set_int(spi_env.spi_dma_rx_channel, length);

    // Update Rx DMA length
    dma_set_len(spi_env.spi_dma_rx_channel, length);

    // Update Rx DMA callback
    dma_register_callback(spi_env.spi_dma_rx_channel, cb, NULL);


    // Do not increment src addrss, we want to send dummy zeroes
    dma_set_ainc(spi_env.spi_dma_tx_channel, DMA_INC_FALSE);

    // Update Tx DMA bus data width
    dma_set_bw(spi_env.spi_dma_tx_channel, wsz);

    // Update Tx DMA source address
    dma_set_src(spi_env.spi_dma_tx_channel, (uint32_t) tx_data);

    // Update Tx DMA INT
    dma_set_int(spi_env.spi_dma_tx_channel, length);

    // Update Tx DMA length
    dma_set_len(spi_env.spi_dma_tx_channel, length);

    // Update Tx DMA callback
    dma_register_callback(spi_env.spi_dma_tx_channel, NULL, NULL);

    // Start DMA
    dma_channel_start(spi_env.spi_dma_tx_channel, DMA_IRQ_STATE_DISABLED);
    dma_channel_start(spi_env.spi_dma_rx_channel, DMA_IRQ_STATE_ENABLED);
}

/**
 ****************************************************************************************
 * @brief SPI transfer data via DMA.
 * @param[in] tx_buf    Pointer to buffer with data to send to SPI.
 * @param[in] rx_buf    Pointer to buffer where received data from SPI will be stored.
 * @param[in] length    Number of data items to receive.
 * @param[in] cb        Callback to return upon completion.
 * @return void
 ****************************************************************************************
 */
static void spi_dma_transfer(const uint32_t *tx_buf, uint32_t *rx_buf, uint16_t length, dma_cb_t cb)
{
    // Non-blocking mode. Interrupt driven + DMA
    if (GetBits16(&spi->SPI_CTRL_REGF, SPI_WORD) <= SPI_MODE_16BIT)
    {
        DMA_BW_CFG wsz = GetBits16(&spi->SPI_CTRL_REGF, SPI_WORD) == SPI_MODE_8BIT ? DMA_BW_BYTE : DMA_BW_HALFWORD;

        dma_set_bw(spi_env.spi_dma_rx_channel, wsz);

        // Update Rx DMA destination address
        dma_set_dst(spi_env.spi_dma_rx_channel, (uint32_t) rx_buf);

        // Update Rx DMA INT
        dma_set_int(spi_env.spi_dma_rx_channel, length);

        // Update Rx DMA length
        dma_set_len(spi_env.spi_dma_rx_channel, length);

        // Update Rx DMA callback
        dma_register_callback(spi_env.spi_dma_rx_channel, cb, NULL);


        // Update Tx DMA bus data width
        dma_set_bw(spi_env.spi_dma_tx_channel, wsz);

        // Update Tx DMA source address
        dma_set_src(spi_env.spi_dma_tx_channel, (uint32_t) tx_buf);

        // Update Tx DMA INT
        dma_set_int(spi_env.spi_dma_tx_channel, length);

        // Update Tx DMA length
        dma_set_len(spi_env.spi_dma_tx_channel, length);

        // Update Tx DMA callback
        dma_register_callback(spi_env.spi_dma_tx_channel, NULL, NULL);

        // Start DMA
        dma_channel_start(spi_env.spi_dma_tx_channel, DMA_IRQ_STATE_DISABLED);
        dma_channel_start(spi_env.spi_dma_rx_channel, DMA_IRQ_STATE_ENABLED);
    }
}
#endif

/**
 ****************************************************************************************
 * @brief Waits for SPI to finish the current transfer.
 * @return void
 ****************************************************************************************
 */
static void wait_spi_to_finish(void)
{
    // Wait until SPI becomes ready for a transfer
    while (GetBits16(&spi->SPI_CTRL_REG1F, SPI_BUSY) == 1);

    // Clear the interrupt flag
    while (GetBits16(&spi->SPI_CTRL_REGF, SPI_INT_BIT) == 1)
    {
        GetWord16(&spi->SPI_RX_TX_REG0F);
        SetWord16(&spi->SPI_CLEAR_INT_REGF, 0x01);
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
    // Switch off SPI block
    SetBits16(&spi->SPI_CTRL_REGF, SPI_ON, SPI_BIT_DIS);
    SetBits16(CLK_PER_REG, SPI_ENABLE, SPI_BIT_DIS);
    spi_cs_high();

    // Enable interrupts
    GLOBAL_INT_RESTORE();
}

void spi_set_cp_mode(SPI_CP_MODE_CFG spi_cp)
{
    // Close SPI block
    SetBits16(&spi->SPI_CTRL_REGF, SPI_ON, 0);

    // Set Clock Phase
    SetBits16(&spi->SPI_CTRL_REGF, SPI_PHA, spi_cp & 0x1);
    // Set Clock Polarity
    SetBits16(&spi->SPI_CTRL_REGF, SPI_POL, (spi_cp >> 1) & 0x1);

    // Open SPI block
    SetBits16(&spi->SPI_CTRL_REGF, SPI_ON, 1);
}

void spi_set_cs_pad(SPI_Pad_t cs_pad)
{
    spi_env.cs_pad.port = cs_pad.port;
    spi_env.cs_pad.pin = cs_pad.pin;
}

void spi_set_speed(SPI_SPEED_MODE_CFG spi_speed)
{
    // Close SPI block
    SetBits16(&spi->SPI_CTRL_REGF, SPI_ON, 0);

    // Set speed
    SetBits16(&spi->SPI_CTRL_REGF, SPI_CLK, spi_speed);

    // Open SPI block
    SetBits16(&spi->SPI_CTRL_REGF, SPI_ON, 1);
}

void spi_set_bitmode(SPI_WSZ_MODE_CFG spi_wsz)
{
    // Close SPI block
    SetBits16(&spi->SPI_CTRL_REGF, SPI_ON, 0);

    // Set word size
    SetBits16(&spi->SPI_CTRL_REGF, SPI_WORD, spi_wsz);

    // Open SPI block
    SetBits16(&spi->SPI_CTRL_REGF, SPI_ON, 1);
}

void spi_set_irq_mode(SPI_IRQ_CFG irq_mode)
{
    // Close SPI block
    SetBits16(&spi->SPI_CTRL_REGF, SPI_ON, 0);

    // Set IRQ mode
    SetBits16(&spi->SPI_CTRL_REGF, SPI_MINT, irq_mode);

    // Open SPI block
    SetBits16(&spi->SPI_CTRL_REGF, SPI_ON, 1);
}

#if defined (CFG_SPI_DMA_SUPPORT)
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
#endif

int8_t spi_initialize(const spi_cfg_t *spi_cfg)
{
    // Set CS Pad in environment
    spi_env.cs_pad.port = spi_cfg->cs_pad.port;
    spi_env.cs_pad.pin = spi_cfg->cs_pad.pin;

    // Force CS line to inactive state
    spi_cs_high();

    // Enable Clock for SPI
    spi_enable();

    // Close SPI block and write control register with selected configuration
    SetWord16(&spi->SPI_CTRL_REGF,  (spi_cfg->spi_ms << 6) | (spi_cfg->spi_cp << 1) |
                                    (spi_cfg->spi_speed << 3) | (spi_cfg->spi_wsz << 7) |
                                    (spi_cfg->spi_irq << 14));

    // Set FIFO Bidirectional mode
    SetBits16(&spi->SPI_CTRL_REG1F, SPI_FIFO_MODE, 0);

    // Clear the interrupt flag
    SetWord16(&spi->SPI_CLEAR_INT_REGF, 0x01);

#if defined (CFG_SPI_DMA_SUPPORT)
    // Register callbacks
    spi_env.send_cb = spi_cfg->send_cb;
    spi_env.receive_cb = spi_cfg->receive_cb;
    spi_env.transfer_cb = spi_cfg->transfer_cb;

    // Initialize DMA for SPI
    spi_dma_init(spi_cfg);
#endif

    // Enable SPI block
    SetBits16(&spi->SPI_CTRL_REGF, SPI_ON, 1);

    return SPI_STATUS_ERR_OK;
}

void spi_cs_low(void)
{
    GPIO_SetInactive(spi_env.cs_pad.port, spi_env.cs_pad.pin);
}

void spi_cs_high(void)
{
    GPIO_SetActive(spi_env.cs_pad.port, spi_env.cs_pad.pin);
}

int8_t spi_send(const void *data, uint16_t num, SPI_OP_CFG op)
{
    // Get word size configuration
    SPI_WSZ_MODE_CFG wsz = (SPI_WSZ_MODE_CFG) GetBits16(&spi->SPI_CTRL_REGF, SPI_WORD);

    // Set Write only mode
    SetBits16(&spi->SPI_CTRL_REG1F, SPI_FIFO_MODE, 2);

    if (GetBits16(&spi->SPI_CTRL_REGF, SPI_SMN) == SPI_MS_MODE_MASTER)
    {
#if defined (CFG_SPI_DMA_SUPPORT)
        // Non-blocking mode. Interrupt driven + DMA
        if (op == SPI_OP_DMA && wsz <= SPI_MODE_16BIT)
        {
            spi_dma_send(data, num, spi_dma_send_cb);
        }
        else
#endif
        {
            const uint8_t *tx_data = (uint8_t *) data;

            // While SPI is not busy
            for (uint16_t i = 0; i < num; i++)
            {
                // Wait until TX-FIFO is not full
                while (GetBits16(&spi->SPI_CTRL_REGF, SPI_TXH) == 1);

                if (wsz == SPI_MODE_8BIT)
                {
                    // Send 8bit data
                    SetWord16(&spi->SPI_RX_TX_REG0F, *(uint8_t *) tx_data & 0xFF);
                    tx_data++;
                }
                else if (wsz == SPI_MODE_16BIT)
                {
                    // Send 16bit data
                    SetWord16(&spi->SPI_RX_TX_REG0F, *(uint16_t *) tx_data & 0xFFFF);
                    tx_data+=2;
                }
                else
                {
                    // Send 16bit MSB data
                    SetWord16(&spi->SPI_RX_TX_REG1F, (*(uint32_t *) tx_data) >> 16);
                    // Send 16bit LSB data
                    SetWord16(&spi->SPI_RX_TX_REG0F, *(uint16_t *) tx_data & 0xFFFF);
                    tx_data+=4;
                }

                // Clear the interrupt flag
                SetWord16(&spi->SPI_CLEAR_INT_REGF, 0x01);
            }
            wait_spi_to_finish();
        }
    }
    return SPI_STATUS_ERR_OK;
}

int8_t spi_receive(void *data, uint16_t num, SPI_OP_CFG op)
{
    // Get word size configuration
    SPI_WSZ_MODE_CFG wsz = (SPI_WSZ_MODE_CFG) GetBits16(&spi->SPI_CTRL_REGF, SPI_WORD);

    // Disable Read only mode. Read is performed with dummy writes
    SetBits16(&spi->SPI_CTRL_REG1F, SPI_FIFO_MODE, 0);

    if (GetBits16(&spi->SPI_CTRL_REGF, SPI_SMN) == SPI_MS_MODE_MASTER)
    {
#if defined (CFG_SPI_DMA_SUPPORT)
        if (op == SPI_OP_DMA && wsz <= SPI_MODE_16BIT)
        {
            spi_dma_receive(data, num, spi_dma_receive_cb);
        }
        else
#endif
        {
            uint8_t *rx_data8 = (uint8_t *) data;
            uint16_t *rx_data16 = (uint16_t *) data;
            uint32_t *rx_data32 = (uint32_t *) data;

            // Read the remaining words
            for (uint16_t read_cnt = 0; read_cnt < num; read_cnt++)
            {
                // Dummy write
                SetWord16(&spi->SPI_RX_TX_REG0F, 0);
                // Wait while Rx FIFO is empty
                while (GetBits16(&spi->SPI_CTRL_REGF, SPI_INT_BIT) == 0);

                if (wsz == SPI_MODE_8BIT)
                {
                    *rx_data8 = GetWord16(&spi->SPI_RX_TX_REG0F) & 0xFF;
                    rx_data8++;
                }
                else if (wsz == SPI_MODE_16BIT)
                {
                    *rx_data16 = GetWord16(&spi->SPI_RX_TX_REG0F);
                    rx_data16++;
                }
                else
                {
                    *rx_data32 = GetWord16(&spi->SPI_RX_TX_REG1F) << 16
                               | GetWord16(&spi->SPI_RX_TX_REG0F);
                    rx_data32++;
                }

                // Clear the interrupt flag
                SetWord16(&spi->SPI_CLEAR_INT_REGF, 0x01);
            }
            wait_spi_to_finish();
        }
    }
    return SPI_STATUS_ERR_OK;
}

int8_t spi_transfer(const void *data_out, void *data_in, uint16_t num, SPI_OP_CFG op)
{
    // Get word size configuration
    SPI_WSZ_MODE_CFG wsz = (SPI_WSZ_MODE_CFG) GetBits16(&spi->SPI_CTRL_REGF, SPI_WORD);

    // Set FIFO Bidirectional mode
    SetBits16(&spi->SPI_CTRL_REG1F, SPI_FIFO_MODE, 0);

    if (GetBits16(&spi->SPI_CTRL_REGF, SPI_SMN) == SPI_MS_MODE_MASTER)
    {
#if defined (CFG_SPI_DMA_SUPPORT)
        if (op == SPI_OP_DMA && wsz <= SPI_MODE_16BIT)
        {
            spi_dma_transfer(data_out, data_in, num, spi_dma_transfer_cb);
        }
        else
#endif
        {
            const uint8_t *tx_data = (uint8_t *) data_out;
            uint8_t *rx_data8 = (uint8_t *) data_in;
            uint16_t *rx_data16 = (uint16_t *) data_in;
            uint32_t *rx_data32 = (uint32_t *) data_in;

            for (uint16_t i = 0; i < num; i++)
            {
                // Wait until TX-FIFO is not full
                while (GetBits16(&spi->SPI_CTRL_REGF, SPI_TXH) == 1);

                if (wsz == SPI_MODE_8BIT)
                {
                    // Send 8bit data
                    SetWord16(&spi->SPI_RX_TX_REG0F, *(uint8_t *) tx_data & 0xFF);
                    tx_data++;

                    // Wait while Rx FIFO is empty
                    while (GetBits16(&spi->SPI_CTRL_REGF, SPI_INT_BIT) == 0);

                    // Receive 8bit data
                    *rx_data8 = GetWord16(&spi->SPI_RX_TX_REG0F) & 0xFF;
                    rx_data8++;
                }
                else if (wsz == SPI_MODE_16BIT)
                {
                    // Send 16bit data
                    SetWord16(&spi->SPI_RX_TX_REG0F, *(uint16_t *) tx_data & 0xFFFF);
                    tx_data+=2;

                    // Wait while Rx FIFO is empty
                    while (GetBits16(&spi->SPI_CTRL_REGF, SPI_INT_BIT) == 0);

                    // Receive 16bit data
                    *rx_data16 = GetWord16(&spi->SPI_RX_TX_REG0F);
                    rx_data16++;
                }
                else
                {
                    // Send 16bit MSB data
                    SetWord16(&spi->SPI_RX_TX_REG1F, (*(uint32_t *) tx_data) >> 16);
                    // Send 16bit LSB data
                    SetWord16(&spi->SPI_RX_TX_REG0F, *(uint16_t *) tx_data & 0xFFFF);
                    tx_data+=4;

                    // Wait while Rx FIFO is empty
                    while (GetBits16(&spi->SPI_CTRL_REGF, SPI_INT_BIT) == 0);

                    // Receive 32bit data
                    *rx_data32 = GetWord16(&spi->SPI_RX_TX_REG1F) << 16
                               | GetWord16(&spi->SPI_RX_TX_REG0F);
                    rx_data32++;
                }

                // Clear the interrupt flag
                SetWord16(&spi->SPI_CLEAR_INT_REGF, 0x01);
            }
            wait_spi_to_finish();
        }
    }
    return SPI_STATUS_ERR_OK;
}


uint32_t spi_access(uint32_t dataToSend)
{
    uint32_t dataRead = 0;

    // Set FIFO Bidirectional mode
    SetBits16(&spi->SPI_CTRL_REG1F, SPI_FIFO_MODE, 2);

    // Check if bitmode is set in 9-bit or 32-bit
    if (GetBits16(&spi->SPI_CTRL_REGF, SPI_WORD) > 1)
    {
        // Write high part of dataToSend
        SetWord16(&spi->SPI_RX_TX_REG1F, (uint16_t)(dataToSend >> 16));
    }

    // Write (low part of) dataToSend
    SetWord16(&spi->SPI_RX_TX_REG0F, (uint16_t)dataToSend);

    // Polling to wait for spi transmission
    do
    {
    } while (GetBits16(&spi->SPI_CTRL_REGF, SPI_INT_BIT) == 0);

    // Clear pending flag
    SetWord16(&spi->SPI_CLEAR_INT_REGF, 0x01);

    // Read from Registers
    // Check if bitmode is set in 9-bit or 32-bit
    if (GetBits16(&spi->SPI_CTRL_REGF, SPI_WORD) > 1)
    {
        // Read high part of data from spi slave
        dataRead = GetWord16(&spi->SPI_RX_TX_REG1F) << 16;
    }

    // Read (low part of) data from spi slave
    dataRead += GetWord16(&spi->SPI_RX_TX_REG0F);
    // Return data read from spi slave
    return dataRead;
}

uint32_t spi_transaction(uint32_t dataToSend)
{
    uint32_t dataRead = 0;

    spi_cs_low();

    dataRead = spi_access(dataToSend);                      // basic SPI access

    spi_cs_high();

    return dataRead;                                        // return data read from spi slave
}
