/**
 ****************************************************************************************
 *
 * @file i2c.c
 *
 * @brief Definition of API for the I2C Low Level Driver for DA14585 and DA14586 devices.
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


typedef struct
{
    tx_state_t      tx_state;
    rx_state_t      rx_state;
    i2c_interrupt_cb_t   intr_cb;
}i2c_env_tag_t;

/* I2C data are not retained. The user must ensure that they are updated after exiting sleep. */
static i2c_env_tag_t i2c_env;

/// Interrupt handler used to serve I2C interrupts
static void intr_handler(uint16_t mask);

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

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void i2c_init(const i2c_cfg_t *cfg)
{
    IRQn_Type irq_type = I2C_IRQn;

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

    // Enable NVIC interrupts
    NVIC_EnableIRQ(irq_type);
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


    while (len--)
    {
        while (!i2c_is_tx_fifo_not_full());
        // Write byte to tx fifo
        i2c_write_byte(data[bytes_written]);
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
            i2c_write_byte(I2C_CMD);
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
        i2c_write_byte(txs->data[txs->num]);
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
        i2c_write_byte(I2C_CMD);
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
