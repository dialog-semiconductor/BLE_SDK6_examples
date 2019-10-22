/**
 ****************************************************************************************
 *
 * @file aes_cbc.h
 *
 * @brief AES-CBC header file.
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

#ifndef AES_CBC_H_
#define AES_CBC_H_

/*
 * DEFINES
 ****************************************************************************************
 */

// Input block size (128 bits)
#define AES_CBC_BLK_SIZE (16)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

// Status codes
enum
{
    AES_CBC_ERR_NO_ERR,
    AES_CBC_ERR_INVALID_PARAM,
};

/*
 * PUBLIC FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief CBC encryption using AES cipher
 * @param[in] blocks_128        Input data blocks
 * @param[in] block_count       Number of input blocks
 * @param[out] out_block_128    Output block for CBC mode result
 * @param[in] block_count       Number of output blocks
 * @param[in] key               Key
 * @param[in] key_size          Key length (in bytes)
 * @param[in] IV_128            Input vector for 128-bit data block cypher
 * @return                      error code if something goes wrong, 0 otherwise
 * @note Blocks-128 should point to a memory ares which is a multiple of 128-bit
 * data blocks in size. Additionally if there are fewer output blocks than the number of
 * input blocks, only the last ciphertext block will be stored in the first output
 * block (CBC-MAC mode).
 ****************************************************************************************
 */
uint8_t aes_cbc_encrypt(const uint8_t *blocks_128, const uint8_t block_count,
                        uint8_t *out_block_128, const uint8_t out_block_count,
                        const uint8_t *key, const uint8_t key_size, const uint8_t *IV_128);

/**
 ****************************************************************************************
 * @brief CBC decryption using AES cipher
 * @param[in] blocks_128        Input data blocks
 * @param[in] block_count       Number of input blocks
 * @param[out] out_block_128    Output block for CBC mode result
 * @param[in] out_block_count   Number of output blocks
 * @param[in] key               Key
 * @param[in] key_size          Key length (in bytes)
 * @param[in] IV_128            Input vector for 128-bit data block cypher
 * @return                      error code if something goes wrong, 0 otherwise
 * @note Blocks-128 should point to a memory area whose size is a multiple of
 * 128-bit data blocks.
 ****************************************************************************************
 */
uint8_t aes_cbc_decrypt(const uint8_t *blocks_128, uint8_t *out_blocks_128,
                        const uint8_t block_count, const uint8_t *key,
                        const uint8_t key_size, const uint8_t *IV_128);

/**
 ****************************************************************************************
 * @brief Calculate number of blocks given the payload size.
 * @param[in] size                  payload size in bytes,
 * @return                          number of blocks of size ENC_DATA_LEN
 ****************************************************************************************
 */
extern inline uint8_t aes_get_blk_num(uint16_t size);

/**
 ****************************************************************************************
 * @brief Xor data buffers
 * @param[in] s1        1st data buffer
 * @param[in] s2        2nd data buffer
 * @param[in] size      number of bytes to be xored
 * @param[out] des      buffer for the result
 ****************************************************************************************
 */
void aes_array_xor(const uint8_t *s1, const uint8_t *s2, uint16_t size, uint8_t *des);

#endif // AES_CBC_H_
