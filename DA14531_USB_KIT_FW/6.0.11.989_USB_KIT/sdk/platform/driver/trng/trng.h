/**
 ****************************************************************************************
 *
 * @file trng.h
 *
 * @brief True Random Number Generator header file.
 *
 * Copyright (c) 2014-2019 Dialog Semiconductor. All rights reserved.
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

#ifndef _TRNG_H_
#define _TRNG_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>

#if defined (__DA14531__)
#include "iid_irng.h"
#endif

/*
 * DEFINITIONS
 *****************************************************************************************
 */

#if defined (__DA14531__)

/// TRNG seed size in bytes
#define TRNG_SEED_SIZE      (IRNG_RANDOM_SEED_SIZE_BYTES)

/// Number of uninitialized RAM blocks used to generate random data
#define TRNG_NUM_BLOCKS     (IRNG_MINIMUM_SRAM_PUF_BLOCKS)

/// Size of one block of uninitialized RAM
#define TRNG_BLOCK_SIZE     (IRNG_BLOCK_SIZE_BYTES)

#define TRNG_RAM_SIZE_USED  (TRNG_NUM_BLOCKS * TRNG_BLOCK_SIZE)

#else

/// TRNG seed size in bytes
#define TRNG_SEED_SIZE      (16)

#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if defined (__DA14531__)

/**
 ****************************************************************************************
 * @brief  Acquires 128 random bits using uninitialized RAM data.
 *
 * @param[out] trng_bits_ptr    Pointer to the buffer where random data will be stored.
 * @note Its address must be aligned to 32 bits.
 * @param[in] sram_puf          Pointer to the base address of the uninitialized SRAM area.
 * @note Its address must be aligned to 32 bits.
 * @param[in] sram_puf_blocks   Number of 16byte blocks to use (28 minimum).
 * @return Status code
 ****************************************************************************************
 */
uint8_t trng_acquire(uint8_t *trng_bits_ptr, uint8_t *sram_puf, const uint16_t sram_puf_blocks);

#else

/**
 ****************************************************************************************
 * @brief  Acquires 128 bits, saves them in trng_bits[] and restores the modified regs.
 * @param[in] trng_bits_ptr Pointer to the buffer where random data will be stored
 * @return void
 ****************************************************************************************
 */
void trng_acquire(uint8_t *trng_bits_ptr);

#endif

/**
 ****************************************************************************************
 * @brief Initialize random number generator seed using random bits acquired from TRNG
 * @return void
 ****************************************************************************************
 */
void init_rand_seed_from_trng(void);

#endif // _TRNG_H_
