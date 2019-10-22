/**
 ****************************************************************************************
 *
 * @file i2c.c
 *
 * @brief Implementation of the I2C Low Level Driver for DA1585, DA14586, DA14531 devices.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "i2c.h"
#include "datasheet.h"
#include "ll.h"
#include "dma.h"

typedef struct
{
    const uint8_t           *data;
    uint16_t                len;
    uint16_t                num;
    i2c_complete_cb_t       cb;
    void                    *cb_data;
    uint32_t                flags;
}tx_state_t;

typedef struct
{
    uint8_t                 *data;
    uint16_t                len;
    uint16_t                num;
    uint16_t                rr;
    i2c_complete_cb_t       cb;
    void                    *cb_data;
    uint32_t                flags;
}rx_state_t;

#if defined (CFG_I2C_DMA_SUPPORT)
typedef struct
{
    void                     *cb_data;
    i2c_complete_cb_t        cb;
    i2c_dma_channel_pair_t   channel;
}dma_state_t;
#endif

typedef struct
{
    tx_state_t      tx_state;
    rx_state_t      rx_state;
#if defined (CFG_I2C_DMA_SUPPORT)
    dma_state_t     dma_state;
#endif
    i2c_interrupt_cb_t   intr_cb;
}i2c_env_tag_t;

/* I2C data are not retained. The user must ensure that they are updated after exiting sleep. */
static i2c_env_tag_t i2c_env;

#if !defined (__EXCLUDE_I2C_HANDLER__)
/// Interrupt handler used to serve I2C interrupts
static void intr_handler(uint16_t mask);
#endif

/// Interrupt handler used for Master transmit buffer operation
static void intr_master_transmit_buffer_handler(uint16_t mask);

/// Interrupt handler used for Master receive buffer operation
static void intr_master_receive_buffer_handler(uint16_t mask);

/// Interrupt handler used for Slave receive buffer operation
static void intr_slave_receive_buffer_handler(uint16_t mask);

/// Interrupt handler used for Slave transmit buffer operation
static void intr_slave_transmit_buffer_handler(uint16_t mask);

/// Reply tx for transmit completion
static void tx_reply(bool success);

/// Reply rx for receive completion
static void rx_reply(bool success);

#if defined (CFG_I2C_DMA_SUPPORT)
/// Interrupt handler used to handle STOP and ABORT for DMA writes
static void intr_write_buffer_dma_handler(uint16_t mask);

/// Interrupt handler used to to handle ABORT for DMA writes
static void intr_write_buffer_dma_no_stop_handler(uint16_t mask);

/// Callback for the DMA end of read
static void notify_on_dma_read_end_cb(void *user_data, uint16_t len);
#if defined (__DA14531__)
/// Callback for the DMA last read request
static void notify_on_dma_read_request_end_cb(void *user_data, uint16_t len);
#endif

/// DMA Reply tx for transmit completion
static void dma_tx_reply(bool success);

/// DMA Reply rx for receive completion
static void dma_rx_reply(bool success);

/// Read command for DMA
static volatile uint16_t i2c_prepare_dma_read_cmd;
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void i2c_init(const i2c_cfg_t *cfg)
{
    // Disable interrupts
    GLOBAL_INT_DISABLE();

    //Enable i2c clock to R/W i2c registers
    i2c_set_clock_status(I2C_CLOCK_ENABLE);

    // Enable interrupts
    GLOBAL_INT_RESTORE();

    i2c_set_controller_status(I2C_CONTROLLER_DISABLE);

    // There is a two ic_clk delay when enabling or disabling the controller
    while(i2c_get_controller_status() != I2C_CONTROLLER_DISABLE);

    i2c_set_int_mask(0x0000);

    // SCL clock settings
    SetWord16(&i2c->I2C_SS_SCL_HCNT_REGF, cfg->clock_cfg.ss_hcnt);
    SetWord16(&i2c->I2C_SS_SCL_LCNT_REGF, cfg->clock_cfg.ss_lcnt);

    SetWord16(&i2c->I2C_FS_SCL_HCNT_REGF, cfg->clock_cfg.fs_hcnt);
    SetWord16(&i2c->I2C_FS_SCL_LCNT_REGF, cfg->clock_cfg.fs_lcnt);

    // Set speed mode
    SetBits16(&i2c->I2C_CON_REGF, I2C_SPEED, cfg->speed);

    // Set fifo levels
    ASSERT_WARNING(cfg->tx_fifo_level != 0);
    ASSERT_WARNING(cfg->rx_fifo_level != 0);
    SetBits16(&i2c->I2C_TX_TL_REGF, RX_TL, cfg->tx_fifo_level - 1);
    SetBits16(&i2c->I2C_RX_TL_REGF, RX_TL, cfg->rx_fifo_level - 1);

    if (cfg->mode == I2C_MODE_MASTER)
    {
        // Setup I2C master
        i2c_setup_master(cfg->restart_en, cfg->addr_mode, cfg->address);
    }
    else
    {
        // Setup I2C slave
        i2c_setup_slave(cfg->addr_mode, cfg->address);
    }

    i2c_set_controller_status(I2C_CONTROLLER_ENABLE);
    // There is a two ic_clk delay when enabling or disabling the controller
    while ((i2c_get_controller_status() != I2C_CONTROLLER_ENABLE));

    // Set the priority of I2C interrupt to level 2
    NVIC_SetPriority(I2C_IRQn, 2);
    // Enable I2C interrupt
    NVIC_EnableIRQ(I2C_IRQn);
}

void i2c_release(void)
{
    i2c_set_controller_status(I2C_CONTROLLER_DISABLE);
    // There is a two ic_clk delay when enabling or disabling the controller
    while(i2c_get_controller_status() != I2C_CONTROLLER_DISABLE);

    i2c_set_clock_status(I2C_CLOCK_DISABLE);
}

void i2c_setup_master(i2c_restart_t restart, i2c_addressing_t addr_mode, uint16_t target_addr)
{
    // Set Master mode
    i2c_set_mode(I2C_MODE_MASTER);

    /// Force enable Restart condition with a 10-bit target address.
    i2c_set_restart_status(addr_mode == I2C_ADDRESSING_10B ? I2C_RESTART_ENABLE : restart);

    i2c_set_target_addressing_mode(addr_mode);

    i2c_set_target_address(target_addr);
}

void i2c_setup_slave(i2c_addressing_t addr_mode, uint16_t slave_addr)
{
    // Set Slave mode
    i2c_set_mode(I2C_MODE_SLAVE);

    i2c_set_slave_addressing_mode(addr_mode);

    i2c_set_slave_address(slave_addr);
}

uint16_t i2c_master_transmit_buffer_sync(const uint8_t *data, uint16_t len, i2c_abort_t *abrt_code, uint32_t flags)
{
    i2c_abort_t ret = I2C_ABORT_NONE;
    uint16_t bytes_written = 0;

#if defined (__DA14531__)
    if (flags & I2C_F_WAIT_FOR_STOP)
    {
      flags |= I2C_F_ADD_STOP;
    }
#endif

    while (len--)
    {
        while (!i2c_is_tx_fifo_not_full());
        // Write byte to tx fifo
#if defined (__DA14531__)
        i2c_write_byte(data[bytes_written] | ((len == 0 && (flags & I2C_F_ADD_STOP)) ? I2C_STOP : 0));
#else
        i2c_write_byte(data[bytes_written]);
#endif
        bytes_written++;
        // Read tx abort source
        ret = i2c_get_abort_source();
        if (ret)
        {
            // Clear tx abort
            i2c_reset_int_tx_abort();

            break;
        }
    }

    if ((!ret) && (flags & I2C_F_WAIT_FOR_STOP))
    {
        // Wait until TX fifo is empty
        while (!i2c_is_tx_fifo_empty());

        // Wait until no master activity
        while (i2c_is_master_busy());
    }

    if (abrt_code)
    {
        *abrt_code = ret;
    }

    return bytes_written;
}

uint16_t i2c_master_receive_buffer_sync(uint8_t *data, uint16_t len, i2c_abort_t *abrt_code, uint32_t flags)
{
    i2c_abort_t ret = I2C_ABORT_NONE;
    uint16_t bytes_read = 0;
    uint16_t read_req = 0;

    while (bytes_read < len)
    {
        while ((read_req < len) && (i2c_is_tx_fifo_not_full()))
        {
            read_req++;
            // Write read command to tx fifo
#if defined (__DA14531__)
            i2c_write_byte(I2C_CMD | ((read_req == len && (flags & I2C_F_ADD_STOP)) ? I2C_STOP : 0));
#else
            i2c_write_byte(I2C_CMD);
#endif
        }

        while ((bytes_read < len) && (i2c_get_rx_fifo_level()))
        {
            // Get received byte from rx fifo
            data[bytes_read] = i2c_read_byte();

            bytes_read++;
        }

        // Read tx abort source
        ret = i2c_get_abort_source();
        if (ret)
        {
            // Clear tx abort
            i2c_reset_int_tx_abort();

            break;
        }
    }

    if (abrt_code)
    {
        *abrt_code = ret;
    }

    return bytes_read;
}

static void tx_reply(bool success)
{
    // Unregister previous installed interrupts
    i2c_unregister_int();

    i2c_env.tx_state.data = NULL;

    // Fire user callback
    if (i2c_env.tx_state.cb)
    {
        i2c_env.tx_state.cb(i2c_env.tx_state.cb_data, i2c_env.tx_state.num, success);
    }
}

static void intr_master_transmit_buffer_handler(uint16_t mask)
{
    tx_state_t *txs = &i2c_env.tx_state;

    if (mask & I2C_INT_TX_ABORT)
    {
        tx_reply(false);

        // Clear tx abort
        i2c_reset_int_tx_abort();
        return;
    }

    if (mask & I2C_INT_STOP_DETECTED)
    {
        tx_reply(txs->num == txs->len);

        // Clear stop detected
        i2c_reset_int_stop_detected();
        return;
    }

    while ((txs->num < txs->len) && (i2c_is_tx_fifo_not_full()))
    {
        // Write byte to tx fifo
#if defined (__DA14531__)
        i2c_write_byte(txs->data[txs->num] | (((txs->num + 1 == txs->len) && (txs->flags & I2C_F_ADD_STOP)) ? I2C_STOP : 0));
#else
        i2c_write_byte(txs->data[txs->num]);
#endif
        txs->num++;
    }

    // Trigger reply when all data were written to TX FIFO
    if (txs->num == txs->len)
    {
        if (txs->flags & I2C_F_WAIT_FOR_STOP)
        {
            i2c_set_int_mask(i2c_get_int_mask() & ~I2C_INT_TX_EMPTY);
            return;
        }
        else
        {
            tx_reply(true);
        }
    }
}

void i2c_master_transmit_buffer_async(const uint8_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data, uint32_t flags)
{
    uint16_t mask = I2C_INT_TX_EMPTY | I2C_INT_TX_ABORT;

#if defined (__DA14531__)
    if (flags & I2C_F_WAIT_FOR_STOP)
    {
        flags |= I2C_F_ADD_STOP;
    }
#endif

    // Initialize I2C tx configuration in the environment
    i2c_env.tx_state.data = data;
    i2c_env.tx_state.len = len;
    i2c_env.tx_state.num = 0;
    i2c_env.tx_state.cb = cb;
    i2c_env.tx_state.cb_data = cb_data;
    i2c_env.tx_state.flags = flags;

    // Clear tx abort
    i2c_reset_int_tx_abort();

    if (flags & I2C_F_WAIT_FOR_STOP)
    {
        // Clear stop detected
        i2c_reset_int_stop_detected();

        mask |= I2C_INT_STOP_DETECTED;
    }

    // Register the interrupt handler
    i2c_register_int(intr_master_transmit_buffer_handler, mask);
}

static void rx_reply(bool success)
{
    // Unregister interrupts
    i2c_unregister_int();

    i2c_env.rx_state.data = NULL;

    // Fire user callback
    if (i2c_env.rx_state.cb)
    {
        i2c_env.rx_state.cb(i2c_env.rx_state.cb_data, i2c_env.rx_state.num, success);
    }
}

static void intr_master_receive_buffer_handler(uint16_t mask)
{
    rx_state_t *rxs = &i2c_env.rx_state;

    if (mask & I2C_INT_TX_ABORT)
    {
        rx_reply(false);

        // Clear abort
        i2c_reset_int_tx_abort();
        return;
    }

    while ((rxs->rr < rxs->len) && i2c_is_tx_fifo_not_full())
    {
        rxs->rr++;
#if defined (__DA14531__)
        i2c_write_byte(I2C_CMD | ((rxs->rr == rxs->len && (rxs->flags & I2C_F_ADD_STOP)) ? I2C_STOP : 0));
#else
        i2c_write_byte(I2C_CMD);
#endif
    }

    while ((rxs->num < rxs->len) && i2c_get_rx_fifo_level())
    {
        // Get received byte from rx fifo
        rxs->data[rxs->num] = i2c_read_byte();
        rxs->num++;
    }

    if (rxs->num == rxs->len)
    {
        rx_reply(true);

        return;
    }

    if (rxs->rr < rxs->len)
    {
        return;
    }
}

void i2c_master_receive_buffer_async(uint8_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data, uint32_t flags)
{
    uint16_t mask = I2C_INT_TX_EMPTY | I2C_INT_RX_FULL | I2C_INT_TX_ABORT;

    // Initialize I2C rx configuration in the environment
    i2c_env.rx_state.data = data;
    i2c_env.rx_state.len = len;
    i2c_env.rx_state.num = 0;
    i2c_env.rx_state.rr = 0;
    i2c_env.rx_state.cb = cb;
    i2c_env.rx_state.cb_data = cb_data;
    i2c_env.rx_state.flags = flags;

    // Clear tx abort
    i2c_reset_int_tx_abort();

    // Register the interrupt handler
    i2c_register_int(intr_master_receive_buffer_handler, mask);
}

uint16_t i2c_slave_receive_buffer_sync(uint8_t *data, uint16_t len)
{
    uint16_t bytes_read = 0;

    while (bytes_read < len)
    {
        if (i2c_get_rx_fifo_level())
        {
            // Get received byte from rx fifo
            data[bytes_read] = i2c_read_byte();

            bytes_read++;
        }
    }

    return bytes_read;
}

uint16_t i2c_slave_transmit_buffer_sync(const uint8_t *data, uint16_t len, i2c_abort_t *abrt_code)
{
    i2c_abort_t ret = I2C_ABORT_NONE;
    uint16_t bytes_written = 0;

    while (len--)
    {
        while (!i2c_is_tx_fifo_not_full());

        // Write byte to tx fifo
        i2c_write_byte(data[bytes_written++]);

        // Read tx abort source
        ret = i2c_get_abort_source();

        if (ret)
        {
            // Clear tx abort
            i2c_reset_int_tx_abort();
            break;
        }
    }

    if (!ret)
    {
        while (!i2c_is_tx_fifo_empty());

        while (i2c_is_slave_busy());
    }

    if (abrt_code)
    {
        *abrt_code = ret;
    }

    return bytes_written;
}

static void intr_slave_receive_buffer_handler(uint16_t mask)
{
    rx_state_t *rxs = &i2c_env.rx_state;

    if (mask & I2C_INT_RX_OVERFLOW)
    {
        rx_reply(false);

        // Clear rx overflow
        i2c_reset_int_rx_overflow();
    }

    while (i2c_get_rx_fifo_level() && (rxs->num < rxs->len))
    {
        // Get received byte from rx fifo
        rxs->data[rxs->num] = i2c_read_byte();

        rxs->num++;
    }

    if ((rxs->num == rxs->len) || (mask & (I2C_INT_STOP_DETECTED)))
    {
        rx_reply(true);

        // Clear stop detected
        i2c_reset_int_stop_detected();
        return;
    }
}

void i2c_slave_receive_buffer_async(uint8_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data)
{
    uint16_t mask = I2C_INT_STOP_DETECTED | I2C_INT_RX_FULL | I2C_INT_RX_OVERFLOW;

    // Initialize I2C rx configuration in the environment
    i2c_env.rx_state.data = data;
    i2c_env.rx_state.len = len;
    i2c_env.rx_state.num = 0;
    i2c_env.rx_state.cb = cb;
    i2c_env.rx_state.cb_data = cb_data;

    // Register the interrupt handler
    i2c_register_int(intr_slave_receive_buffer_handler, mask);
}

static void intr_slave_transmit_buffer_handler(uint16_t mask)
{
    tx_state_t *txs = &i2c_env.tx_state;

    if (mask & I2C_INT_TX_ABORT)
    {
        tx_reply(false);

        // Clear tx abort
        i2c_reset_int_tx_abort();
    }

    if (mask & I2C_INT_RX_OVERFLOW)
    {
        tx_reply(false);

        // Clear rx overflow
        i2c_reset_int_rx_overflow();
    }

    if (mask & I2C_INT_STOP_DETECTED)
    {
        tx_reply(txs->num == txs->len);

        // Clear stop detected
        i2c_reset_int_stop_detected();
        return;
    }

    if (mask & I2C_INT_RX_DONE)
    {
        tx_reply(txs->num == txs->len);

        // Clear rx_done
        i2c_reset_int_rx_done();
        return;
    }

    if (mask & I2C_INT_READ_REQUEST)
    {
        if (txs->num < txs->len && i2c_is_tx_fifo_not_full())
        {
            // Write byte to tx fifo
            i2c_write_byte(txs->data[txs->num++]);
        }
        else
        {
            tx_reply(false);
        }

        // Clear read request
        i2c_reset_int_read_request();
    }
}

void i2c_slave_transmit_buffer_async(const uint8_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data)
{
    uint16_t mask = I2C_INT_READ_REQUEST | I2C_INT_RX_FULL | I2C_INT_RX_OVERFLOW | I2C_INT_TX_ABORT | I2C_INT_RX_DONE | I2C_INT_STOP_DETECTED;

    // Initialize I2C tx configuration in the environment
    i2c_env.tx_state.data = data;
    i2c_env.tx_state.len = len;
    i2c_env.tx_state.num = 0;
    i2c_env.tx_state.cb = cb;
    i2c_env.tx_state.cb_data = cb_data;

    // Clear tx abort
    i2c_reset_int_tx_abort();

    // Register the interrupt handler
    i2c_register_int(intr_slave_transmit_buffer_handler, mask);
}

#if defined (CFG_I2C_DMA_SUPPORT)
void i2c_master_transmit_buffer_dma(i2c_dma_channel_pair_t dma_channel_pair, const uint16_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data, uint32_t flags)
{
    // Prepare DMA for Master transmit
    i2c_prepare_dma(dma_channel_pair, (void *)data, len, I2C_DMA_TRANSFER_WRITE, cb, cb_data, flags);

    // Start I2C DMA
    i2c_dma_start();
}

void i2c_master_receive_buffer_dma(i2c_dma_channel_pair_t dma_channel_pair, uint8_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data, uint32_t flags)
{
    // Prepare DMA for Master receive
    i2c_prepare_dma(dma_channel_pair, (void *)data, len, I2C_DMA_TRANSFER_MASTER_READ , cb, cb_data, flags);

    // Start I2C DMA
    i2c_dma_start();
}

void i2c_slave_transmit_buffer_dma(i2c_dma_channel_pair_t dma_channel_pair, const uint16_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data)
{
    // Prepare DMA for Slave transmit
    i2c_prepare_dma(dma_channel_pair, (void *)data, len, I2C_DMA_TRANSFER_WRITE, cb, cb_data, I2C_F_WAIT_FOR_STOP);

    // Start I2C DMA
    i2c_dma_start();
}

void i2c_slave_receive_buffer_dma(i2c_dma_channel_pair_t dma_channel_pair, uint8_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data)
{
    // Prepare DMA for Slave receive
    i2c_prepare_dma(dma_channel_pair, (void *)data, len, I2C_DMA_TRANSFER_SLAVE_READ , cb, cb_data, I2C_F_NONE);

    // Start I2C DMA
    i2c_dma_start();
}

static void dma_rx_reply(bool success)
{
    // Unregister interrupts
    i2c_unregister_int();

    i2c_env.rx_state.data = NULL;

    // Fire user callback
    if (i2c_env.dma_state.cb)
    {
        i2c_env.dma_state.cb(i2c_env.dma_state.cb_data, i2c_env.rx_state.num, success);
    }
}

static void dma_tx_reply(bool success)
{
    // Unregister interrupts
    i2c_unregister_int();

    i2c_env.tx_state.data = NULL;

    // Fire user callback
    if (i2c_env.dma_state.cb)
    {
        i2c_env.dma_state.cb(i2c_env.dma_state.cb_data, i2c_env.tx_state.num, success);
    }
}

static void notify_on_dma_write_end_no_stop_cb(void *user_data, uint16_t len)
{
    tx_state_t *txs = &i2c_env.tx_state;

    txs->num = len;

    // Disable I2C DMA
    i2c_dma_stop();

    dma_tx_reply(txs->num == txs->len);
}

static void notify_on_dma_write_end_cb(void *user_data, uint16_t len)
{
    // Disable I2C DMA
    i2c_dma_stop();

    // Store len, to pass to user's cb when STOP/ABORT is detected
    i2c_env.tx_state.num = len;
}

#if defined (__DA14531__)
static void notify_on_dma_read_request_end_cb(void *user_data, uint16_t len)
{
    // Wait if I2C TX FIFO is full
    while (!i2c_is_tx_fifo_not_full());
    // Add STOP condition to the last read request
    i2c_write_byte(i2c_prepare_dma_read_cmd | I2C_STOP);
}
#endif

static void notify_on_dma_read_end_cb(void *user_data, uint16_t len)
{
    rx_state_t *rxs = &i2c_env.rx_state;

    rxs->num = len;

    // Disable I2C DMA
    i2c_dma_stop();

    dma_rx_reply(rxs->num == rxs->len);
}


void i2c_prepare_dma(i2c_dma_channel_pair_t dma_channel_pair, void *data, uint16_t len, i2c_dma_transfer_t type, i2c_complete_cb_t cb, void *cb_data, uint32_t flags)
{
    i2c_prepare_dma_read_cmd = I2C_CMD;
    dma_cfg_t dma_cfg;

   // Disable I2C DMA
    i2c_dma_stop();

    // Initialize I2C dma configuration in the environment
    i2c_env.dma_state.cb = cb;
    i2c_env.dma_state.cb_data = cb_data;
    i2c_env.dma_state.channel = dma_channel_pair;

    // Prepare DMA and I2C for Master read or Slave read
    if (type != I2C_DMA_TRANSFER_WRITE)
    {
        // Configure DMA for I2C Rx
        dma_cfg.bus_width = DMA_BW_BYTE;
        dma_cfg.irq_enable = DMA_IRQ_STATE_ENABLED;
        dma_cfg.irq_nr_of_trans = 0;
        dma_cfg.dreq_mode = DMA_DREQ_TRIGGERED;
        dma_cfg.src_inc = DMA_INC_FALSE;
        dma_cfg.dst_inc = DMA_INC_TRUE;
        dma_cfg.circular = DMA_MODE_NORMAL;
        dma_cfg.dma_prio = DMA_PRIO_7;
        dma_cfg.dma_idle = DMA_IDLE_INTERRUPTING_MODE;
        dma_cfg.dma_init =  DMA_INIT_AX_BX_AY_BY;
        dma_cfg.dma_req_mux = DMA_TRIG_I2C_RXTX;
        dma_cfg.src_address = I2C_DATA_CMD_REG;
        dma_cfg.dst_address = (uint32_t) data;

        // Update Rx DMA length
        dma_cfg.length = len;

        // Register DMA callback
        dma_cfg.cb = notify_on_dma_read_end_cb;

        // Initialize I2C rx configuration in the environment
        i2c_env.rx_state.num = 0;
        i2c_env.rx_state.len = len;

        // Initialize DMA Channel for rx
        dma_initialize(DMA_ID_GET(dma_channel_pair), &dma_cfg);

        // Start DMA
        dma_channel_start(DMA_ID_GET(dma_channel_pair), DMA_IRQ_STATE_ENABLED);
    }

    // Prepare DMA and I2C for Master read - write or Slave write.
    // TX channel used also when reading as master since we need to trigger read by writing read command to TX FIFO
    if (type != I2C_DMA_TRANSFER_SLAVE_READ)
    {
        // Check if we have Master read
        bool is_rx = (type != I2C_DMA_TRANSFER_WRITE);

        // Configure DMA for I2C Tx
        dma_cfg.bus_width = DMA_BW_HALFWORD;
        dma_cfg.irq_enable = DMA_IRQ_STATE_ENABLED;
        dma_cfg.irq_nr_of_trans = 0;
        dma_cfg.dreq_mode = DMA_DREQ_TRIGGERED;
        dma_cfg.src_inc = is_rx ? DMA_INC_FALSE : DMA_INC_TRUE;
        dma_cfg.dst_inc = DMA_INC_FALSE;
        dma_cfg.circular = DMA_MODE_NORMAL;
        dma_cfg.dma_prio = DMA_PRIO_7;
        dma_cfg.dma_idle = DMA_IDLE_INTERRUPTING_MODE;
        dma_cfg.dma_init =  DMA_INIT_AX_BX_AY_BY;
        dma_cfg.dma_req_mux = DMA_TRIG_I2C_RXTX;

        // For Master read we store read command separately
        dma_cfg.src_address = (uint32_t) (is_rx ? &i2c_prepare_dma_read_cmd : data);
        dma_cfg.dst_address = I2C_DATA_CMD_REG;

        // Update Tx DMA length
#if defined (__DA14531__)
        if (is_rx && (flags & I2C_F_ADD_STOP))
        {
            dma_cfg.length = len - 1;
        }
        else
        {
            dma_cfg.length = len;
        }
#else
        dma_cfg.length = len;
#endif

        // Prepare I2C for Master write or Slave write.
        if (type == I2C_DMA_TRANSFER_WRITE)
        {
            uint16_t mask = I2C_INT_TX_ABORT;

            // Clear tx abort
            i2c_reset_int_tx_abort();

            // Initialize I2C tx configuration in the environment
            i2c_env.tx_state.num = 0;
            i2c_env.tx_state.len = len;
            i2c_env.tx_state.data = data;
            i2c_env.tx_state.flags = flags;

#if defined (__DA14531__)
            if ((flags & (I2C_F_ADD_STOP | I2C_F_WAIT_FOR_STOP)) != 0)
            {
                // Set STOP condition bit in last data packet when DMA is used
                ((uint16_t *)data)[len - 1] |= I2C_STOP;
            }
#endif
            if (flags & I2C_F_WAIT_FOR_STOP)
            {
                mask |= I2C_INT_STOP_DETECTED;

                // Clear stop detected
                i2c_reset_int_stop_detected();

                // Register DMA callback
                dma_cfg.cb = notify_on_dma_write_end_cb;

                // Register the interrupt handler
                i2c_register_int(intr_write_buffer_dma_handler, mask);
            }
            else
            {
                // Register DMA callback
                dma_cfg.cb = notify_on_dma_write_end_no_stop_cb;

                // Register the interrupt handler
                i2c_register_int(intr_write_buffer_dma_no_stop_handler, mask);
            }
        }
        else
        {

            // Register DMA callback
#if defined (__DA14531__)
            dma_cfg.cb = (flags & (I2C_F_ADD_STOP)) ? notify_on_dma_read_request_end_cb : NULL;
#else
            dma_cfg.cb = NULL;
#endif
        }

    // Initialize DMA Channel for tx
    dma_initialize(DMA_ID_GET(dma_channel_pair + 1), &dma_cfg);

    // Start DMA
    dma_channel_start(DMA_ID_GET(dma_channel_pair + 1), DMA_IRQ_STATE_ENABLED);
    }
}

static void intr_write_buffer_dma_no_stop_handler(uint16_t mask)
{
    if (mask & I2C_INT_TX_ABORT)
    {
        // Disable I2C DMA
        i2c_dma_stop();

        dma_tx_reply(false);

        // Clear tx abort
        i2c_reset_int_tx_abort();

        return;
    }
}

static void intr_write_buffer_dma_handler(uint16_t mask)
{
    tx_state_t *txs = &i2c_env.tx_state;

    // Must provide a valid (> 0) mask
    ASSERT_WARNING(mask != 0);

    if (mask & I2C_INT_TX_ABORT)
    {
        // Disable I2C DMA
        i2c_dma_stop();

        dma_tx_reply(false);

        // Clear tx abort
        i2c_reset_int_tx_abort();

        return;
    }

    if (mask & I2C_INT_STOP_DETECTED)
    {
        if (GetWord16(I2C_DMA_CR_REG) != 0)
        {
            // Clear stop detected
            i2c_reset_int_stop_detected();

            // A STOP while DMA is still enabled is caused by a NACK from the slave.
            // While servicing the STOP_DETECTED interrupt we don't need to call the
            // reply callback. This will be done when servicing the TX_ABORT interrupt
            // that will follow.
            return;
        }

        dma_tx_reply(txs->num == txs->len);

        // Clear stop detected
        i2c_reset_int_stop_detected();
        return;
    }
}
#endif

void i2c_register_int(i2c_interrupt_cb_t cb, uint16_t mask)
{
    // Register I2C callback
    i2c_env.intr_cb = cb;

    // Set interrupt mask
    i2c_set_int_mask(mask);
}

void i2c_unregister_int(void)
{
    i2c_register_int(NULL, 0);

    // Clear I2C interrupt requests
    NVIC_ClearPendingIRQ(I2C_IRQn);
}

#if !defined (__EXCLUDE_I2C_HANDLER__)
static void intr_handler(uint16_t mask)
{
    if (i2c_env.intr_cb)
    {
        // Fire I2C callback
        i2c_env.intr_cb(mask);
    }
    else
    {
        // Clear I2C interrupt requests
        NVIC_ClearPendingIRQ(I2C_IRQn);
    }
}

void I2C_Handler(void)
{
    // Read interrupt state
    uint16_t mask = i2c_get_int_state();

    intr_handler(mask);
}
#endif
