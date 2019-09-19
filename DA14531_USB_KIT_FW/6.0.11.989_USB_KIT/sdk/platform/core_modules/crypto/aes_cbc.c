/**
 ****************************************************************************************
 *
 * @file aes_cbc.c
 *
 * @brief AES-CBC implementation.
 *
 * Copyright (c) 2018 Dialog Semiconductor. All rights reserved.
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

/**
 ****************************************************************************************
 * @addtogroup aes_cbc
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "aes_cbc.h"
#include "aes_api.h"

/*
 * STATIC FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Encrypt single block
 * @param[in] input             Input data blocks
 * @param[in] output            Output data blocks
 * @param[in] key               Key
 * @param[in] key_size          Key length (in bytes)
 * @param[in] endec             Encrypt/Decrypt flag
 ****************************************************************************************
 */
static inline void cbc_blk_aes_op(const uint8_t *input, uint8_t *output,
                                  const uint8_t *key, uint8_t key_size,
                                  const uint8_t encdec)
{
    AES_KEY aes_key;

    // Perform aes calculation for the requested block
    aes_set_key(key, 128, &aes_key, encdec);
    aes_enc_dec((uint8_t *)input, output, &aes_key, encdec, 0);
}

/**
 ****************************************************************************************
 * @brief Encrypt single block xored with the initial input vector
 * @param[in] block_128         Input data blocks
 * @param[in] out_block_128     Output data blocks
 * @param[in] key               Key
 * @param[in] key_size          Key length (in bytes)
 * @param[in] IV_128            Initial input vector
 ****************************************************************************************
 */
static inline void aes_cbc_blk_encr(const uint8_t *block_128, uint8_t *out_block_128,
                                    const uint8_t *key, const uint8_t key_size,
                                    const uint8_t *IV_128)
{
    aes_array_xor(IV_128, block_128, AES_CBC_BLK_SIZE, out_block_128);
    cbc_blk_aes_op(out_block_128, out_block_128, key, key_size, AES_ENCRYPT);
}

/*
 * PUBLIC FUNCTION DEFINITIONS
 ****************************************************************************************
 */

uint8_t aes_cbc_encrypt(const uint8_t *blocks_128, const uint8_t block_count,
                        uint8_t *out_blocks_128, const uint8_t out_block_count,
                        const uint8_t *key, const uint8_t key_size,
                        const uint8_t *IV_128)
{
    uint16_t i;
    uint8_t *prev_blk = &out_blocks_128[0];
    uint8_t *out_blk = prev_blk;

    if ((block_count == 0) || (out_block_count == 0) ||
        (blocks_128 == NULL) || (out_blocks_128 == NULL) ||
        (key == NULL) || (key_size == 0))
    {
        return AES_CBC_ERR_INVALID_PARAM;
    }

    // Apply Initialisation Vector (if provided) to the first
    // payload block, then encrypt.
    if (IV_128)
    {
        aes_cbc_blk_encr(blocks_128, out_blk, key, key_size, IV_128);
    } else {
        cbc_blk_aes_op(blocks_128, out_blk, key, key_size, AES_ENCRYPT);
    }

    for (i = AES_CBC_BLK_SIZE; i < (block_count * AES_CBC_BLK_SIZE);
         i += AES_CBC_BLK_SIZE)
    {
        if (out_block_count == block_count)
        {
            // Move In/Out pointers if not in MAC-only mode
            prev_blk = out_blk;
            out_blk = &out_blocks_128[i];
        }

        // Use previous out block as an IV for the current block
        aes_cbc_blk_encr(&blocks_128[i], out_blk, key, key_size, prev_blk);
    }

    return AES_CBC_ERR_NO_ERR;
}

uint8_t aes_cbc_decrypt(const uint8_t *blocks_128, uint8_t *out_blocks_128,
                        const uint8_t block_count, const uint8_t *key,
                        const uint8_t key_size, const uint8_t *IV_128)
{
    uint8_t i = (block_count - 1) * AES_CBC_BLK_SIZE;

    if ((block_count == 0) || (key == NULL) || (key_size == 0))
    {
        return AES_CBC_ERR_INVALID_PARAM;
    }

    // Compute all blocks except the first one
    while (i)
    {
        cbc_blk_aes_op(&blocks_128[i], &out_blocks_128[i], key, key_size,
                       AES_DECRYPT);
        aes_array_xor(&out_blocks_128[i], &blocks_128[i - AES_CBC_BLK_SIZE],
                      AES_CBC_BLK_SIZE, &out_blocks_128[i]);

        i -= AES_CBC_BLK_SIZE;
    }

    // Compute first block
    cbc_blk_aes_op(blocks_128, out_blocks_128, key, key_size, AES_DECRYPT);

    // Xor with IV if provided
    if (IV_128)
    {
        aes_array_xor(out_blocks_128, IV_128, AES_CBC_BLK_SIZE, out_blocks_128);
    }

    return AES_CBC_ERR_NO_ERR;
}

inline uint8_t aes_get_blk_num(uint16_t size)
{
    return (size + ENC_DATA_LEN - 1) / ENC_DATA_LEN;
}

void aes_array_xor(const uint8_t *s1, const uint8_t *s2, uint16_t size, uint8_t *des)
{
    uint32_t i;

    for (i = 0; i < size; i++)
    {
        des[i] = s1[i] ^ s2[i];
    }
}
/// @} aes_cbc
