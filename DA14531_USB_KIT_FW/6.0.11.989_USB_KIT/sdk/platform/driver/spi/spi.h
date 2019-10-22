/**
 ****************************************************************************************
 *
 * @file spi.h
 *
 * @brief spi interface driver header file.
 *
 * Copyright (c) 2012-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef _SPI_H_
#define _SPI_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#if defined (__DA14531__)
    #include "spi_531.h"
#else
    #include "spi_58x.h"
#endif


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Enable clock for SPI peripheral.
 * @return void
 ****************************************************************************************
 */
void spi_enable(void);

/**
 ****************************************************************************************
 * @brief Disable SPI block and clock for SPI peripheral. Force CS to inactive state.
 * @return void
 ****************************************************************************************
 */
void spi_disable(void);

/**
 ****************************************************************************************
 * @brief Set SPI word size (on-the-fly).
 * @param[in] spi_wsz           SPI_WSZ_MODE_CFG (8, 16, 32 or 9 bits width)
 * @return void
 ****************************************************************************************
 */
void spi_set_bitmode(SPI_WSZ_MODE_CFG spi_wsz);

/**
 ****************************************************************************************
 * @brief Register SPI callback (on-the-fly). Callback to return in interrupt/DMA-driven
 * sending operations.
 * @param[in] cb                pointer to callback of spi_cb_t type
 * @return void
 ****************************************************************************************
 */
void spi_register_send_cb(spi_cb_t cb);

/**
 ****************************************************************************************
 * @brief Register SPI callback (on-the-fly). Callback to return in interrupt/DMA-driven
 * receiving operations.
 * @param[in] cb                pointer to callback of spi_cb_t type
 * @return void
 ****************************************************************************************
 */
void spi_register_receive_cb(spi_cb_t cb);

/**
 ****************************************************************************************
 * @brief Register SPI callback (on-the-fly). Callback to return in interrupt/DMA-driven
 * transferring operations.
 * @param[in] cb                pointer to callback of spi_cb_t type
 * @return void
 ****************************************************************************************
 */
void spi_register_transfer_cb(spi_cb_t cb);

/**
 ****************************************************************************************
 * @brief Initialize SPI peripheral with selected configuration.
 *        If DMA is supported, it will also initialize the DMA engine for the SPI peripheral
 * @param[in] spi_cfg       SPI configuration struct of spi_cfg_t type
 * @return void
 ****************************************************************************************
 */
int8_t spi_initialize(const spi_cfg_t *spi_cfg);

/**
 ****************************************************************************************
 * @brief Activate the configured Chip Select line
 * @return void
 ****************************************************************************************
 */
void spi_cs_low(void);

/**
 ****************************************************************************************
 * @brief Deactivate the configured Chip Select line
 * @return void
 ****************************************************************************************
 */
void spi_cs_high(void);

/**
 ****************************************************************************************
 * @brief Start sending data to SPI
 * @param[in] data      Pointer to buffer with data to send to SPI. It can be a pointer to
 *                      uint8_t, uint16_t or uint32_t data type depending on SPI
 *                      configuration.
 * @param[in] num       Number of data items to send. (Must be non-zero)
 * @param[in] op        Blocking, interrupt-driven or DMA-driven operation.
 * @note In 16 and 32 bitmodes data are transferred in big endian format.
 * @note DMA operation is supported only for 8 and 16 bitmode transfers.
 ****************************************************************************************
 */
int8_t spi_send(const void *data, uint16_t num, SPI_OP_CFG op);

/**
 ****************************************************************************************
 * @brief Start receiving data from SPI
 * @param[in] data      Pointer to buffer with data to receive from SPI. It can be a
 *                      pointer to uint8_t, uint16_t or uint32_t data type depending on SPI
 *                      configuration.
 * @param[in] num       Number of data items to receive. (Must be non-zero)
 * @param[in] op        Blocking, interrupt-driven or DMA-driven operation.
 * @note In 16 and 32 bitmodes data are transferred in big endian format.
 * @note DMA operation is supported only for 8 and 16 bitmode transfers.
 ****************************************************************************************
 */
int8_t spi_receive(void *data, uint16_t num, SPI_OP_CFG op);

/**
 ****************************************************************************************
 * @brief Start sending/receiving data to/from SPI - Full Duplex.
 * @param[in] data_out  Pointer to buffer with data to send to SPI. It can be a pointer to
 *                      uint8_t, uint16_t or uint32_t data type depending on SPI
 *                      configuration.
 * @param[in] data_in   Pointer to buffer with data to receive from SPI. It can be a
 *                      pointer to uint8_t, uint16_t or uint32_t data type depending on SPI
 *                      configuration.
 * @param[in] num       Number of data items to transfer. (Must be non-zero)
 * @param[in] op        Blocking, interrupt-driven or DMA-driven operation.
 * @note In 16 and 32 bitmodes data are transferred in big endian format.
 * @note DMA operation is supported only for 8 and 16 bitmode transfers.
 ****************************************************************************************
 */
int8_t spi_transfer(const void *data_out, void *data_in, uint16_t num, SPI_OP_CFG op);

// Legacy API support

/**
 ****************************************************************************************
 * @brief SPI master access to slave without acting on CS (works in all bit modes).
 * @param[in] dataToSend Data to send
 * @return data read
 ****************************************************************************************
 */
uint32_t spi_access(uint32_t dataToSend);

/**
 ****************************************************************************************
 * @brief SPI master transaction to slave with CS handling (works in all bit modes).
 * @param[in] dataToSend Data to send
 * @return data read
 ****************************************************************************************
 */
uint32_t spi_transaction(uint32_t dataToSend);

/**
 ****************************************************************************************
 * @brief Releases SPI.
 * @return void
 ****************************************************************************************
 */
#define spi_release() spi_disable()


#endif // _SPI_H_
