/**
 ****************************************************************************************
 *
 * @file i2c_eeprom.h
 *
 * @brief I2C EEPROM driver header file.
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

/*******************************************************************************************/
/* Configuration of EEPROM IC's that have been tested                                      */
/* Define one of the following set of directives in your application                       */
/* (not in the body of the driver)                                                         */
/* a) Microchip 24LC02 (2 kBit - 8 byte page)                                              */
/*      #define I2C_EEPROM_DEV_SIZE   0x100        // EEPROM size in bytes                 */
/*      #define I2C_EEPROM_PAGE_SIZE   8           // EEPROM page size in bytes            */
/*      #define I2C_SPEED_MODE    I2C_FAST         // fast mode (400 kbits/s)              */
/*      #define I2C_ADDRESS_MODE  I2C_7BIT_ADDR    // 7-bit addressing                     */
/*      #define I2C_ADDRESS_SIZE  I2C_1BYTE_ADDR   // 8-bit memory address                 */
/* b) Microchip 24LC64 (64 kBit - 32 byte page)                                            */
/*      #define I2C_EEPROM_DEV_SIZE   0x2000       // EEPROM size in bytes                 */
/*      #define I2C_EEPROM_PAGE_SIZE   32          // EEPROM page size in bytes            */
/*      #define I2C_SPEED_MODE    I2C_FAST         // fast mode (400 kbits/s)              */
/*      #define I2C_ADDRESS_MODE  I2C_7BIT_ADDR    // 7-bit addressing                     */
/*      #define I2C_ADDRESS_SIZE  I2C_2BYTES_ADDR  // 16-bit memory address                */
/* c) ST M24M01 (1 MBit - 256 byte page)                                                   */
/*      #define I2C_EEPROM_DEV_SIZE   0x20000       // EEPROM size in bytes                */
/*      #define I2C_EEPROM_PAGE_SIZE   256          // EEPROM page size in bytes           */
/*      #define I2C_SPEED_MODE    I2C_FAST          // fast mode (400 kbits/s)             */
/*      #define I2C_ADDRESS_MODE  I2C_7BIT_ADDR     // 7-bit addressing                    */
/*      #define I2C_ADDRESS_SIZE  I2C_2BYTES_ADDR   // 16-bit memory address               */
/*                                                                                         */
/*******************************************************************************************/

#ifndef _I2C_EEPROM_H_
#define _I2C_EEPROM_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "i2c.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define I2C_MAX_RETRIES                 (200000)

enum I2C_ADDRESS_BYTES_COUNT
{
    I2C_1BYTE_ADDR,
    I2C_2BYTES_ADDR,
    I2C_3BYTES_ADDR,
};

typedef enum
{
    I2C_NO_ERROR,
    I2C_7B_ADDR_NOACK_ERROR,
    I2C_INVALID_EEPROM_ADDRESS
} i2c_error_code;

typedef struct
{
    /// EEPROM size
    uint32_t size;

    /// EEPROM page size
    uint16_t page_size;

    /// Address size
    uint8_t address_size;

} i2c_eeprom_cfg_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

 /**
 ****************************************************************************************
 * @brief Set I2C Serial EEPROM driver environment
 * @param[in] i2c_cfg           I2C Configuration
 * @param[in] i2c_eeprom_cfg    EEPROM Configuration
 * @retun void
 ****************************************************************************************
 */
void i2c_eeprom_configure(const i2c_cfg_t *i2c_cfg, const i2c_eeprom_cfg_t *i2c_eeprom_cfg);

 /**
 ****************************************************************************************
 * @brief Get I2C Serial EEPROM driver environment
 * @param[out] i2c_cfg           I2C Configuration
 * @param[out] i2c_eeprom_cfg    EEPROM Configuration
 * @retun void
 ****************************************************************************************
 */
void i2c_eeprom_get_configuration(i2c_cfg_t *i2c_cfg, i2c_eeprom_cfg_t *i2c_eeprom_cfg);

 /**
 ****************************************************************************************
 * @brief Initialize I2C Serial EEPROM
 * @retun void
 ****************************************************************************************
 */
void i2c_eeprom_initialize(void);

/**
 ****************************************************************************************
 * @brief Disable I2C controller and clock.
 * @return void
 ****************************************************************************************
 */
void i2c_eeprom_release(void);

/**
 ****************************************************************************************
 * @brief Update I2C target address
 * @param[in] slave_addr           slave address
 * @note This function disables also the I2C peripheral
 * @retun void
 ****************************************************************************************
 */
void i2c_eeprom_update_slave_address(uint16_t slave_addr);

/**
 ****************************************************************************************
 * @brief Poll until I2C eeprom is ready.
 * @return i2c_error_code Enumeration type that defines the returned error code
 ****************************************************************************************
 */
i2c_error_code i2c_wait_until_eeprom_ready(void);

/**
 ****************************************************************************************
 * @brief Read single byte from I2C EEPROM.
 * @param[in] address     Memory address to read the byte from
 * @param[in|out] byte    Pointer to the read byte
 * @return i2c_error_code Enumeration type that defines the returned error code
 ****************************************************************************************
 */
i2c_error_code i2c_eeprom_read_byte(uint32_t address, uint8_t *byte);

/**
 ****************************************************************************************
 * @brief Reads data from I2C EEPROM to memory position of given pointer.
 * @param[in|out] rd_data_ptr Read data pointer
 * @param[in] address         Starting memory address
 * @param[in] size            Size of the data to be read
 * @param[in|out] bytes_read  Bytes that were actually read (due to memory size limitation)
 * @return i2c_error_code     Enumeration type that defines the returned error code
 ****************************************************************************************
 */
i2c_error_code i2c_eeprom_read_data(uint8_t *rd_data_ptr, uint32_t address, uint32_t size, uint32_t *bytes_read);

/**
 ****************************************************************************************
 * @brief Write single byte to I2C EEPROM.
 * @param[in] address     Memory position to write the byte to
 * @param[in] byte        Byte to be written
 * @return i2c_error_code Enumeration type that defines the returned error code
 ****************************************************************************************
 */
i2c_error_code i2c_eeprom_write_byte(uint32_t address, uint8_t byte);

/**
 ****************************************************************************************
 * @brief Writes page to I2C EEPROM.
 * @param[in] address           Starting address of memory page (MUST BE MULTIPLE OF I2C_EEPROM_PAGE_SIZE)
 * @param[in] wr_data_ptr       Pointer to the first of the bytes to be written
 * @param[in] size              Size of the data to be written (MUST BE LESS OR EQUAL TO I2C_EEPROM_PAGE_SIZE)
 * @param[in|out] bytes_written Bytes that were actually written (due to memory size limitation)
 * @return i2c_error_code       Enumeration type that defines the returned error code
 ****************************************************************************************
 */
i2c_error_code i2c_eeprom_write_page(uint8_t *wr_data_ptr, uint32_t address, uint16_t size, uint32_t *bytes_written);

/**
 ****************************************************************************************
 * @brief Writes data to I2C EEPROM.
 * @param[in] wr_data_ptr       Pointer to the first of the bytes to be written
 * @param[in] address           Starting address of the write process
 * @param[in] size              Size of the data to be written
 * @param[in|out] bytes_written Bytes that were actually written (due to memory size limitation)
 * @return i2c_error_code       Enumeration type that defines the returned error code
 ****************************************************************************************
 */
i2c_error_code i2c_eeprom_write_data(uint8_t *wr_data_ptr, uint32_t address, uint32_t size, uint32_t *bytes_written);

#endif // _I2C_EEPROM_H_
