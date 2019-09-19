/**
 ****************************************************************************************
 *
 * @file aes_ccm.h
 *
 * @brief AES-CCM Encryption/Decryption implementation header file.
 *
 * Copyright (C) 2017-2018 Dialog Semiconductor.
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

#ifndef AES_CCM_H_
#define AES_CCM_H_

/*
 * DEFINES
 ****************************************************************************************
 */

#define ENCRYPT_PROCESS 0
#define DECRYPT_PROCESS 1

// Block size
#define CCM_BLK_SIZE    (16)

// Formatting block + adata + formatted adata_len + payload
#define OUT_BUFFER_SIZE(payload_len, CCM_T) (payload_len + CCM_T)

// Formatting function parameters
enum {
    AES_CCM_T4  = 4,
    AES_CCM_T6  = 6,
    AES_CCM_T8  = 8,
    AES_CCM_T10 = 10,
    AES_CCM_T12 = 12,
    AES_CCM_T14 = 14,
    AES_CCM_T16 = 16,
};

enum {
    AES_CCM_Q2 = 2,
    AES_CCM_Q3 = 3,
    AES_CCM_Q4 = 4,
    AES_CCM_Q5 = 5,
    AES_CCM_Q6 = 6,
    AES_CCM_Q7 = 7,
    AES_CCM_Q8 = 8,
};

enum {
    AES_CCM_N7  = 15 - AES_CCM_Q8,
    AES_CCM_N8  = 15 - AES_CCM_Q7,
    AES_CCM_N9  = 15 - AES_CCM_Q6,
    AES_CCM_N10 = 15 - AES_CCM_Q5,
    AES_CCM_N11 = 15 - AES_CCM_Q4,
    AES_CCM_N12 = 15 - AES_CCM_Q3,
    AES_CCM_N13 = 15 - AES_CCM_Q2,
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/**
 * aes_ccm environment structure
 */
struct aes_ccm_env_tag
{
    const uint8_t *Nonce;

    // Payload reserved, array length may change
    uint16_t payload_len;
    uint8_t payload_blk_cnt;
    const uint8_t *payload_input;

    // Adata blocks
    uint16_t adata_len;
    uint8_t adata_blk_cnt;
    uint8_t (*adata_blk)[CCM_BLK_SIZE];

    // CBC-MAC blocks
    uint8_t b_blk_cnt;
    uint8_t (*b_blk)[CCM_BLK_SIZE];
    uint8_t (*x_blk)[CCM_BLK_SIZE];

    // CTR blocks
    uint32_t a_blk_cnt;
    uint8_t (*a_blk)[CCM_BLK_SIZE];
    uint8_t (*s_blk)[CCM_BLK_SIZE];
};

/*
 * PUBLIC FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief AES CCM encryption
 * @param[in] payload      Data to be encrypted/decrypted
 * @param[in] payload_len  payload length in bytes, up to 32
 * @param[in] Nonce        Nonce array, should be unique for each AES-CCM operation
 * @param[in] Adata        Adata, or header, up to 32 bytes
 * @param[in] Adata_len    Adata length in bytes, up to 32
 * @param[out] output      where encrypted cipher to be placed. Header is not included
 ****************************************************************************************
 */
void aes_ccm_encrypt(uint8_t *payload, uint16_t payload_len, uint8_t *Nonce,
                     uint8_t *Adata, uint16_t Adata_len, uint8_t *output);

/**
 ****************************************************************************************
 * @brief AES CCM decryption
 * @param[in] payload      Data to be encrypted/decrypted
 * @param[in] payload_len  payload length in bytes, up to 32
 * @param[in] Nonce        Nonce array, should be unique for each AES-CCM operation
 * @param[in] Adata        Adata, or header, up to 32 bytes
 * @param[in] Adata_len    Adata length in bytes, up to 32
 * @param[out] output      where encrypted cipher to be placed. Header is not included
 * @return    0 if auth data matches up. 1 if something goes wrong
 ****************************************************************************************
 */
uint8_t aes_ccm_decrypt(uint8_t *payload, uint16_t payload_len, uint8_t *Nonce,
                        uint8_t *Adata, uint16_t Adata_len, uint8_t *output);

/**
 ****************************************************************************************
 * @brief Set AES key and memory pool type
 * @param[in] key           key to be used, should be 16 bytes
 * @param[in] T             Tag length
 * @param[in] N             Nonce length
 * @param[in] ke_mem_type   memory pool type used for buffer allocation
 ****************************************************************************************
 */
void aes_ccm_init(uint8_t *key, uint8_t T, uint8_t N, uint8_t ke_mem_type);

/**
 ****************************************************************************************
 * @brief Deinitialize AES-CCM data block
 ****************************************************************************************
 */
void aes_ccm_cleanup(void);

#endif // AES_CCM_H_
