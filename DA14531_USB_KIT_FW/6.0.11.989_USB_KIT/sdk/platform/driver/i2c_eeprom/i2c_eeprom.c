/**
 ****************************************************************************************
 *
 * @file i2c_eeprom.c
 *
 * @brief I2C EEPROM driver.
 *
 * Copyright (c) 2012-2019 Dialog Semiconductor. All rights reserved.
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
#include <string.h>
#include "compiler.h"
#include "ll.h"
#include "i2c_eeprom.h"
#include "i2c.h"

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// I2C EEPROM environment
typedef struct
{
    /// I2C configuration
    i2c_cfg_t           i2c;

    /// I2C EEPROM configuration
    i2c_eeprom_cfg_t    eeprom;
} i2c_eeprom_env_t;

static i2c_eeprom_env_t i2c_eeprom_env      __SECTION_ZERO("retention_mem_area0");

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void i2c_eeprom_configure(const i2c_cfg_t *i2c_cfg, const i2c_eeprom_cfg_t *i2c_eeprom_cfg)
{
    memcpy(&i2c_eeprom_env.eeprom, i2c_eeprom_cfg, sizeof(i2c_eeprom_cfg_t));
    memcpy(&i2c_eeprom_env.i2c, i2c_cfg, sizeof(i2c_cfg_t));
}

void i2c_eeprom_get_configuration(i2c_cfg_t *i2c_cfg, i2c_eeprom_cfg_t *i2c_eeprom_cfg)
{
    memcpy(i2c_eeprom_cfg, &i2c_eeprom_env.eeprom, sizeof(i2c_eeprom_cfg_t));
    memcpy(i2c_cfg, &i2c_eeprom_env.i2c, sizeof(i2c_cfg_t));
}

void i2c_eeprom_initialize(void)
{
    i2c_init(&i2c_eeprom_env.i2c);
}

void i2c_eeprom_release(void)
{
    i2c_release();
}

void i2c_eeprom_update_slave_address(uint16_t slave_addr)
{
    i2c_release();
    i2c_eeprom_env.i2c.address = slave_addr;
}

/**
 ****************************************************************************************
 * @brief Write last byte.
 * @param[in] data  16-bit word (data + STOP bit in case of DA14531)
 * @return void
 ****************************************************************************************
 */
static void i2c_write_last_byte(uint16_t data)
{
#if defined (__DA14531__)
    i2c_write_byte(data | I2C_STOP);
#else
    i2c_write_byte(data);
#endif
}

i2c_error_code i2c_wait_until_eeprom_ready(void)
{
    uint16_t tx_abrt_source;

    // Check if ACK is received
    for (uint32_t i = 0; i < I2C_MAX_RETRIES; i++)
    {
        i2c_write_last_byte(0x08);                              // Make a dummy access
        while (i2c_is_tx_fifo_empty() == 0);                    // Wait until Tx FIFO is empty
        while (i2c_is_master_busy());                           // Wait until no master activity
        tx_abrt_source = i2c_get_abort_source();                // Read the I2C_TX_ABRT_SOURCE_REG register
        i2c_reset_int_tx_abort();                               // Clear I2C_TX_ABRT_SOURCE register
        if ((tx_abrt_source & ABRT_7B_ADDR_NOACK) == 0)
        {
            return I2C_NO_ERROR;
        }
    }
    return I2C_7B_ADDR_NOACK_ERROR;
}

/**
 ****************************************************************************************
 * @brief Send I2C EEPROM memory address.
 * @param[in] address The I2C EEPROM memory address
 * @return void
 ****************************************************************************************
 */
static void i2c_eeprom_send_address(uint32_t address)
{
    switch(i2c_eeprom_env.eeprom.address_size)
    {
        case I2C_2BYTES_ADDR:
        {
            i2c_set_controller_status(I2C_CONTROLLER_DISABLE);
            // The programmer supports I2C memories with an address range between 0x00000 and 0x3FFFF (maximum size is 256KB)
            i2c_set_target_address(i2c_eeprom_env.i2c.address | ((address & 0x30000) >> 16));  // Set Slave device address
            i2c_set_controller_status(I2C_CONTROLLER_ENABLE);
            while (i2c_is_master_busy());               // Wait until no master activity
            i2c_write_byte((address >> 8) & 0xFF);      // Set address MSB, write access
            break;
        }
        case I2C_3BYTES_ADDR:
        {
            i2c_write_byte((address >> 16) & 0xFF);     // Set address MSB, write access
            i2c_write_byte((address >> 8) & 0xFF);      // Set address MSB, write access
            break;
        }
    }

    i2c_write_byte(address & 0xFF);                      // Set address LSB, write access
}

i2c_error_code i2c_eeprom_read_byte(uint32_t address, uint8_t *byte)
{
    if (i2c_wait_until_eeprom_ready() != I2C_NO_ERROR)
    {
        return I2C_7B_ADDR_NOACK_ERROR;
    }

    // Critical section
    GLOBAL_INT_DISABLE();

    i2c_eeprom_send_address(address);

    i2c_write_last_byte(I2C_CMD);                   // Set R/W bit to 1 (read access)

    // End of critical section
    GLOBAL_INT_RESTORE();

    while (i2c_get_rx_fifo_level() == 0);           // Wait for received data
    *byte = i2c_read_byte();                        // Get received byte

    while (i2c_is_tx_fifo_empty() == 0);            // Wait until Tx FIFO is empty
    while (i2c_is_master_busy());                   // wait until no master activity

    return I2C_NO_ERROR;
}

/**
 ****************************************************************************************
 * @brief Read single series of bytes from I2C EEPROM (for driver's internal use)
 * @param[in] p    Memory address to read the series of bytes from (all in the same page)
 * @param[in] size count of bytes to read (must not cross page)
 ****************************************************************************************
 */
static void read_data_single(uint8_t **p, uint32_t address, uint32_t size)
{
    uint32_t j;

    // Critical section
    GLOBAL_INT_DISABLE();

    i2c_eeprom_send_address(address);

    for (j = 0; j < size - 1 ; j++)
    {
        while (i2c_is_tx_fifo_not_full() == 0);     // Wait if Tx FIFO is full
        i2c_write_byte(I2C_CMD);
    }

    while (i2c_is_tx_fifo_not_full() == 0);         // Wait if Tx FIFO is full
    i2c_write_last_byte(I2C_CMD);

    // End of critical section
    GLOBAL_INT_RESTORE();

    // Get the received data
    for (j = 0; j < size; j++)
    {
        while (i2c_get_rx_fifo_level() == 0);       // Wait for received data
        **p = i2c_read_byte();                      // Get the received byte
        (*p)++;
    }

    while (i2c_is_tx_fifo_empty() == 0);            // Wait until Tx FIFO is empty
    while (i2c_is_master_busy());                   // wait until no master activity
}

i2c_error_code i2c_eeprom_read_data(uint8_t *rd_data_ptr, uint32_t address, uint32_t size, uint32_t *bytes_read)
{
    uint32_t tmp_size;

    if (size == 0)
    {
        *bytes_read = 0;
        return I2C_NO_ERROR;
    }

    // Check for max bytes to be read from a (MAX_SIZE x 8) I2C EEPROM
    if (size > i2c_eeprom_env.eeprom.size - address)
    {
        tmp_size = i2c_eeprom_env.eeprom.size - address;
        *bytes_read = tmp_size;
    }
    else
    {
        tmp_size = size;
        *bytes_read = size;
    }

    if (i2c_wait_until_eeprom_ready() != I2C_NO_ERROR)
    {
        return I2C_7B_ADDR_NOACK_ERROR;
    }

    // Read 32 bytes at a time
    while (tmp_size >= 32)
    {
        read_data_single(&rd_data_ptr, address, 32);

        address += 32;                              // Update base address for read
        tmp_size -= 32;                             // Update tmp_size for bytes remaining to be read
    }

    if (tmp_size)
    {
        read_data_single(&rd_data_ptr, address, tmp_size);
    }

    return I2C_NO_ERROR;
}

i2c_error_code i2c_eeprom_write_byte(uint32_t address, uint8_t byte)
{
    if (i2c_wait_until_eeprom_ready() != I2C_NO_ERROR)
    {
        return I2C_7B_ADDR_NOACK_ERROR;
    }

    // Critical section
    GLOBAL_INT_DISABLE();

    i2c_eeprom_send_address(address);

    i2c_write_last_byte(byte & 0xFF);                // Send write byte

    // End of critical section
    GLOBAL_INT_RESTORE();

    while (i2c_is_tx_fifo_empty() == 0);            // Wait until Tx FIFO is empty
    while (i2c_is_master_busy());                   // Wait until no master activity

    return I2C_NO_ERROR;
}

i2c_error_code i2c_eeprom_write_page(uint8_t *wr_data_ptr, uint32_t address, uint16_t size, uint32_t *bytes_written)
{
    uint16_t feasible_size;
    *bytes_written = 0;

    if (address < i2c_eeprom_env.eeprom.size)
    {
        // max possible write size without crossing page boundary
        feasible_size = i2c_eeprom_env.eeprom.page_size - (address % i2c_eeprom_env.eeprom.page_size);

        if (size < feasible_size)
        {
            feasible_size = size;                   // adjust limit accordingly
        }

        if (i2c_wait_until_eeprom_ready() != I2C_NO_ERROR)
        {
            return I2C_7B_ADDR_NOACK_ERROR;
        }

        // Critical section
        GLOBAL_INT_DISABLE();

        i2c_eeprom_send_address(address);

        do
        {
            while (i2c_is_tx_fifo_not_full() == 0); // Wait if I2C Tx FIFO is full
            if (feasible_size == 1)
            {
                i2c_write_last_byte(*wr_data_ptr & 0xFF);
            }
            else
            {
                i2c_write_byte(*wr_data_ptr & 0xFF);          // Send write data
            }

            wr_data_ptr++;
            feasible_size--;
            (*bytes_written)++;
        }
        while (feasible_size != 0);

        // End of critical section
        GLOBAL_INT_RESTORE();

        while (i2c_is_tx_fifo_empty() == 0);        // Wait until Tx FIFO is empty
        while (i2c_is_master_busy());               // Wait until no master activity
    }

    return I2C_NO_ERROR;
}

i2c_error_code i2c_eeprom_write_data(uint8_t *wr_data_ptr, uint32_t address, uint32_t size, uint32_t *bytes_written)
{
    *bytes_written = 0;
    uint32_t feasible_size = size;
    uint32_t bytes_left_to_send;
    uint32_t cnt = 0;

    if (address >= i2c_eeprom_env.eeprom.size)
    {
        return I2C_INVALID_EEPROM_ADDRESS;          // address is not it the EEPROM address space
    }

    // limit to the maximum count of bytes that can be written to an (I2C_EEPROM_DEV_SIZE x 8) EEPROM
    if (size > i2c_eeprom_env.eeprom.size - address)
    {
        feasible_size = i2c_eeprom_env.eeprom.size - address;
    }

    bytes_left_to_send = feasible_size;

    while (bytes_left_to_send)
    {
        if (i2c_eeprom_write_page(wr_data_ptr + (*bytes_written), address + (*bytes_written), bytes_left_to_send, &cnt) != I2C_NO_ERROR)
        {
            return I2C_7B_ADDR_NOACK_ERROR;
        }
        (*bytes_written) += cnt;
        bytes_left_to_send -= cnt;
    }

    return I2C_NO_ERROR;
}
