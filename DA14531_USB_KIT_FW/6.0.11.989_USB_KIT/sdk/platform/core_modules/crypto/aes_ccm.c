/**
 ****************************************************************************************
 *
 * @file aes_ccm.c
 *
 * @brief AES-CCM Encryption/Decryption implementation.
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

/**
 ****************************************************************************************
 * @addtogroup aes_ccm
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <string.h>

#include "aes_cbc.h"
#include "aes_ccm.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define HW_ENCODE 1
#define HW_DECODE 0

// Flags for input data formatting
// [Sec. A.2.1 in NIST_SP_800-38C]
#define CCM_FLAG_Q
#define CCM_FLAG_Q_OFFSET       0
#define CCM_FLAG_Q_MASK         7

#define CCM_FLAG_T
#define CCM_FLAG_T_OFFSET       3
#define CCM_FLAG_T_MASK         7

#define CCM_FLAG_ADATA
#define CCM_FLAG_ADATA_OFFSET   6
#define CCM_FLAG_ADATA_MASK     1

#define CCM_FLAG_RES
#define CCM_FLAG_RES_OFFSET     7
#define CCM_FLAG_RES_MASK       1

// Flags for counter block formatting
// [Sec. A.3 in NIST_SP_800-38C]
#define CTR_FLAG_Q
#define CTR_FLAG_Q_OFFSET       0
#define CTR_FLAG_Q_MASK         7

#define CTR_FLAG_ZERO
#define CTR_FLAG_ZERO_OFFSET    3
#define CTR_FLAG_ZERO_MASK      7

#define CTR_FLAG_RES1
#define CTR_FLAG_RES1_OFFSET    6
#define CTR_FLAG_RES1_MASK      1

#define CTR_FLAG_RES2
#define CTR_FLAG_RES2_OFFSET    7
#define CTR_FLAG_RES2_MASK      1

#define CCM_FLAG_SET(dst, flag, value) (dst = (dst & ~(flag##_MASK << flag##_OFFSET)) | ((value & flag##_MASK) << flag##_OFFSET))

/*
 * STRUCTURES
 ****************************************************************************************
 */

// Associated data encoding struct
// [Sec. A.2.2 in NIST_SP_800-38C]
struct Alen_encoding2 {
    uint8_t data[2];
};

struct Alen_encoding6 {
    uint8_t magic[2];
    uint8_t data[4];
};

struct Alen_encoding10 {
    uint8_t magic[2];
    uint8_t data[8];
};

union Alen_encoding {
    struct Alen_encoding2 alen2;
    struct Alen_encoding6 alen6;
    struct Alen_encoding10 alen10;
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

// Data block
struct aes_ccm_env_tag AES128_ccm_blk;

// Memory block allocation
uint8_t blk_mem_type        __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

// Length of Auth Bytes (TAG/MAC), options are 4,6,8,10,12,14 and 16
uint8_t CCM_T               __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
// Length of the encoded Plain Text length field, options are 2~8
uint8_t CCM_Q               __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
// The nonce length
uint8_t CCM_N               __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

// Key variable
uint8_t key_aes[KEY_LEN]    __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/*
 * STATIC FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief AES Encryption
 * @param[in] input     Plain data to be encrypted
 * @param[out] output   Encrypted Data
 * @param[in] size      Data length
 ****************************************************************************************
 */
static void aes_encrypt(uint8_t *input, uint8_t *output, uint16_t size)
{
    uint8_t num_blk = aes_get_blk_num(size);

    aes_cbc_encrypt(input, num_blk, output, num_blk, key_aes, sizeof(key_aes), NULL);
}

/**
 ****************************************************************************************
 * @brief Set AES key for later use
 * @param[in] key   key to be used, should be 16 bytes
 ****************************************************************************************
 */
static inline void aes_key_init(uint8_t *key)
{
    memcpy(&key_aes, key, 16);
}

/**
 ****************************************************************************************
 * @brief Allocate data blocks
 * @param[out] dest             pointer to store allocated blocks
 * @param[in] old_blk_count     current number of blocks
 * @param[in] new_blk_count     new number of blocks
 ****************************************************************************************
 */
static void blk_malloc(uint8_t (**dest)[CCM_BLK_SIZE], uint8_t old_blk_count,
                       uint8_t new_blk_count)
{
    // Free old buffer if larger one is needed
    if (*dest != NULL)
    {
        if (old_blk_count < new_blk_count)
        {
            ke_free(*dest);
            *dest = ke_malloc(new_blk_count * CCM_BLK_SIZE, blk_mem_type);
        }
    }
    else
    {
        *dest = ke_malloc(new_blk_count * CCM_BLK_SIZE, blk_mem_type);
    }

    memset(*dest, 0, new_blk_count * CCM_BLK_SIZE);
}

/**
 ****************************************************************************************
 * @brief Format associated data block
 * @param[in] blk           Predefined AES_CCM structure,
 * @param[in] adata         Associated data,
 * @param[in] adata_len     Associated data length.
 ****************************************************************************************
 */
static inline void aes_format_ccm_adata(struct aes_ccm_env_tag *blk, const uint8_t *adata,
                                        const uint16_t adata_len)
{
    union Alen_encoding *alenc;
    uint8_t enclen;

    // Length for adata length encoding
    // [Sec. A.2.2 in NIST_SP_800-38C]
    enclen = adata_len < 65280 ? 2 : adata_len < 4294967296 ? 6 : 10;

    // Encode the Adata length
    // [Sec. A.2.2 in NIST_SP_800-38C]
    switch (enclen) {
    case 2:
        blk_malloc(&(blk->adata_blk), blk->adata_blk_cnt,
                   aes_get_blk_num(adata_len + enclen));
        alenc = (union Alen_encoding *) blk->adata_blk;

        // Encoding in 2 bytes
        memcpy(&(alenc->alen2.data[0]), (uint8_t *) &adata_len + 1, 1);
        memcpy(&(alenc->alen2.data[1]), &adata_len, 1);
        break;

    case 6:
        ASSERT_ERROR(0);
        break;

    case 10:
        ASSERT_ERROR(0);
        break;

#if 0 // left for reference only - we support up to 255 blocks -> 4080 bytes
    case 6:
        blk_malloc(&(blk->adata_blk), blk->adata_blk_cnt,
                   aes_get_blk_num(adata_len + enclen));
        alenc = (union Alen_encoding *) blk->adata_blk;

        // 2 Magic bytes
        SetWord8(&(alenc->alen6.magic[0]), 0xff);
        SetWord8(&(alenc->alen6.magic[1]), 0xfe);

        // Encoding in 4 bytes
        memcpy(&(alenc->alen6.data[0]), (uint8_t *) &adata_len + 3, 1);
        memcpy(&(alenc->alen6.data[1]), (uint8_t *) &adata_len + 2, 1);
        memcpy(&(alenc->alen6.data[2]), (uint8_t *) &adata_len + 1, 1);
        memcpy(&(alenc->alen6.data[3]), &adata_len, 1);
        break;

    case 10:
        blk_malloc(&(blk->adata_blk), blk->adata_blk_cnt,
                   aes_get_blk_num(adata_len + enclen));
        alenc = (union Alen_encoding *) blk->adata_blk;

        // 2 Magic bytes
        SetWord8(&(alenc->alen10.magic[0]), 0xff);
        SetWord8(&(alenc->alen10.magic[1]), 0xff);

        // Encoding in 8 bytes
        memcpy(&(alenc->alen10.data[0]), (uint8_t *) &adata_len + 7, 1);
        memcpy(&(alenc->alen10.data[1]), (uint8_t *) &adata_len + 6, 1);
        memcpy(&(alenc->alen10.data[2]), (uint8_t *) &adata_len + 5, 1);
        memcpy(&(alenc->alen10.data[3]), (uint8_t *) &adata_len + 4, 1);
        memcpy(&(alenc->alen10.data[4]), (uint8_t *) &adata_len + 3, 1);
        memcpy(&(alenc->alen10.data[5]), (uint8_t *) &adata_len + 2, 1);
        memcpy(&(alenc->alen10.data[6]), (uint8_t *) &adata_len + 1, 1);
        memcpy(&(alenc->alen10.data[7]), &adata_len, 1);
        break;
#endif
    }

    // Concatenate Adata
    memcpy(((uint8_t*) blk->adata_blk) + enclen, adata, adata_len);

    // Update data length and block count
    blk->adata_len = adata_len + enclen;
    blk->adata_blk_cnt = aes_get_blk_num(blk->adata_len);
}

/**
 *****************************************************************************************
 * @brief Set up AES CCM operation parameters
 * @param[in] blk           Predefined AES_CCM structure
 * @param[in] payload       Data to be encrypted/decrypted
 * @param[in] payload_len   payload length in bytes,up to 32
 * @param[in] Nonce         Nonce array, should be unique for each AES-CCM operation
 * @param[in] Adata         Adata, or header, up to 32 bytes
 * @param[in] Adata_len     Adata length in bytes, up to 32
 * @param[in] type          choose ENCRYPT_PROCESS or DECRYPT_PROCESS
 ****************************************************************************************
 */
static void aes_set_ccm_param(struct aes_ccm_env_tag *blk, uint8_t *payload,
                              uint16_t payload_len, uint8_t *Nonce, uint8_t *Adata,
                              uint16_t Adata_len, uint8_t type)
{
    // Set payload and nonce const pointers
    blk->payload_input = payload;
    blk->Nonce = Nonce;

    if (Adata)
    {
        aes_format_ccm_adata(blk, Adata, Adata_len);
    }

    if (type == ENCRYPT_PROCESS)
    {
        blk->payload_len = payload_len;
        blk->payload_blk_cnt = aes_get_blk_num(payload_len);
    }
    else
    {
        blk->payload_len = payload_len - CCM_T;
        blk->payload_blk_cnt = aes_get_blk_num(blk->payload_len);
    }
}

/**
 ****************************************************************************************
 * @brief Format payload length in Q number of bytes
 * @param[out] dest         Destination buffer,
 * @param[in] source        Payload length,
 * @param[in] bytecount     Number of bytes to encode the payload length.
 ****************************************************************************************
 */
static void aes_format_qfield(uint8_t *dest, const uint8_t *source, uint8_t bytecount)
{
    // Set padding bytes to 0
    memset(dest, 0, CCM_Q - bytecount);

    while (bytecount)
    {
        // Reverse byte order
        memcpy(dest + CCM_Q - bytecount, source + bytecount - 1, 1);
        bytecount--;
    }
}

/**
 ****************************************************************************************
 * @brief Generate first B block with flags and formatting info.
 * @param[in,out] blk   Predefined AES_CCM structure
 ****************************************************************************************
 */
static void aes_gen_ccm_bblk0(struct aes_ccm_env_tag *blk)
{
    uint8_t bytecount = 0;

    // Get minimum number of octets to encode blk->payload_len
    while (blk->payload_len >> (bytecount * 8))
    {
        bytecount++;
    }

    // Check if there is enough space for all significant bytes
    ASSERT_ERROR(CCM_Q >= bytecount);

    // Set flags [according to Sec. A.2.1 in NIST_SP_800-38C]
    if (blk->adata_len)
    {
        CCM_FLAG_SET(blk->b_blk[0][0], CCM_FLAG_ADATA, 1);
    }

    CCM_FLAG_SET(blk->b_blk[0][0], CCM_FLAG_T, (CCM_T - 2) / 2);
    CCM_FLAG_SET(blk->b_blk[0][0], CCM_FLAG_Q, CCM_Q - 1);
    CCM_FLAG_SET(blk->b_blk[0][0], CCM_FLAG_RES, 0);

    // Put Nounce after the flags
    memcpy(&blk->b_blk[0][1], blk->Nonce, CCM_N);

    // Encode payload length in CCM_Q bytes after the nonce (BE order)
    aes_format_qfield(&blk->b_blk[0][1] + CCM_N,
                      (const uint8_t *) &blk->payload_len, bytecount);
}

/**
 ****************************************************************************************
 * @brief Generate B block for AES CCM operation
 * @details Prepares all B blocks for the CBC-MAC pass
 * @param[in,out] blk   Predefined AES_CCM structure
 * @param[in] type      choose ENCRYPT_PROCESS or DECRYPT_PROCESS
 ****************************************************************************************
 */
static void aes_gen_ccm_bblk(struct aes_ccm_env_tag *blk, uint8_t type)
{
    uint8_t blk_cnt = 1 + (blk->adata_blk_cnt) + (blk->payload_blk_cnt);
    uint8_t* s;

    // Allocate blocks for formatted data and the CBC-MAC results
    blk_malloc(&(blk->b_blk), blk->b_blk_cnt, blk_cnt);
    blk_malloc(&(blk->x_blk), blk->b_blk_cnt, blk_cnt);
    blk->b_blk_cnt = blk_cnt;

    // Put flags, Nounce and payload length into the very first block
    aes_gen_ccm_bblk0(blk);

    // Put Associated Data in consecutive blocks. Additionally append
    // it's encoded length [according to Sec. A.2.2 in NIST_SP_800-38C].
    if (blk->adata_len)
    {
        // Adata length should already be encoded by Aes_Set_Ccm_Param(...)
        s = blk->b_blk[1];
        memcpy(s, blk->adata_blk, blk->adata_len);
    }

    // Put Input Payload in consecutive blocks
    if (type == ENCRYPT_PROCESS)
    {
        s = blk->b_blk[1 + blk->adata_blk_cnt];
        memcpy(s, blk->payload_input, blk->payload_len);
    }
}

/**
 ****************************************************************************************
 * @brief Generate A block for AES CCM operation
 * @details Prepares all A blocks for the CTR pass
 * @param[in,out] blk   Predefined AES_CCM structure
 ****************************************************************************************
 */
static void aes_gen_ccm_ablk(struct aes_ccm_env_tag *blk)
{
    uint8_t bytecount = 0;

    // Allocate blocks for counter formatted data and the CTR results
    blk_malloc(&(blk->a_blk), blk->a_blk_cnt, blk->b_blk_cnt);
    blk_malloc(&(blk->s_blk), blk->a_blk_cnt, blk->b_blk_cnt);
    blk->a_blk_cnt = blk->b_blk_cnt;

    // Get minimum number of octets to encode counter for for A blocks
    while (blk->a_blk_cnt >> (bytecount * 8))
    {
        bytecount++;
    }

    // Check if there is enough space for all significant bytes
    ASSERT_ERROR(CCM_Q >= bytecount);

    for (int i = 0; i < blk->a_blk_cnt; i++)
    {
        // Set flags [according to Sec. A.3 in NIST_SP_800-38C]
        CCM_FLAG_SET(blk->a_blk[i][0], CTR_FLAG_Q, CCM_Q - 1);
        CCM_FLAG_SET(blk->a_blk[i][0], CTR_FLAG_ZERO, 0);
        CCM_FLAG_SET(blk->a_blk[i][0], CTR_FLAG_RES1, 0);
        CCM_FLAG_SET(blk->a_blk[i][0], CTR_FLAG_RES2, 0);

        // Put Nounce after the flags
        memcpy(&blk->a_blk[i][1], blk->Nonce, CCM_N);

        // Put counter after nonce
        aes_format_qfield(&blk->a_blk[i][1] + CCM_N,
                          (const uint8_t *) &i, bytecount);
    }
}

/**
 ****************************************************************************************
 * @brief Generate S block for AES CCM operation
 * @details Calculates S blocks in CTR mode
 * @param[in,out] blk   Predefined AES_CCM structure
 ****************************************************************************************
 */
static void aes_gen_ccm_sblk(struct aes_ccm_env_tag *blk)
{
    for (int i = 0; i < blk->a_blk_cnt; i++)
    {
        aes_encrypt(blk->a_blk[i], blk->s_blk[i], CCM_BLK_SIZE);
    }
}

/**
 ****************************************************************************************
 * @brief Generate X block for AES CCM operation
 * @details Calculates tag in CBC-MAC mode (fills the X block)
 * @param[in,out] blk   Predefined AES_CCM structure
 ****************************************************************************************
 */
static inline void aes_gen_ccm_xblk(struct aes_ccm_env_tag *blk)
{
    aes_cbc_encrypt(blk->b_blk[0], blk->b_blk_cnt,      // Input
                    blk->x_blk[0], blk->b_blk_cnt,      // Output
                    key_aes, sizeof(key_aes), NULL);    // Key, IV=0
}

/**
 ****************************************************************************************
 * @brief Generate Output Array for AES CCM operation
 * @details Combines CBC-MAC with the CTR results into the final cyphertext
 * @param[in,out] blk       Predefined AES_CCM structure
 * @param[out] output       where output array to be placed
 ****************************************************************************************
 */
static void aes_gen_ccm_output(struct aes_ccm_env_tag *blk, uint8_t *output)
{
    uint8_t index = blk->b_blk_cnt - 1;
    uint8_t payload_index = blk->adata_blk_cnt + 1;

    aes_array_xor(blk->x_blk[index], blk->s_blk[0], CCM_T, output + blk->payload_len);
    aes_array_xor(blk->b_blk[payload_index], blk->s_blk[1], blk->payload_len, output);
}

/**
 ****************************************************************************************
 * @brief Check if decrypted data match up with the authenticate data and provide the result
 * @param[in,out] blk       Predefined AES_CCM structure
 * @param[out] output       Decrypted data
 * @return                  0 if auth data matches up. 1 if something goes wrong
 ****************************************************************************************
 */
static uint8_t aes_check_ccm_payload(struct aes_ccm_env_tag *blk, uint8_t *output)
{
    uint8_t index = blk->b_blk_cnt - 1;
    uint8_t payload_index = blk->adata_blk_cnt + 1;
    uint8_t *temp_mic = ke_malloc(CCM_T, KE_MEM_NON_RETENTION);
    uint8_t ret = 1;

    memset(temp_mic, 0, CCM_T);

    aes_array_xor(blk->payload_input, blk->s_blk[1], blk->payload_len,
              blk->b_blk[payload_index]);
    aes_gen_ccm_xblk(blk);
    aes_array_xor(blk->x_blk[index], blk->s_blk[0], CCM_T, temp_mic);

    if (!memcmp(blk->payload_input + blk->payload_len, temp_mic, CCM_T))
    {
        memcpy(output, blk->b_blk[payload_index], blk->payload_len);
        ret = 0;
    }

    ke_free(temp_mic);
    return ret;
}

/*
 * PUBLIC FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void aes_ccm_init(uint8_t *key, uint8_t T, uint8_t N, uint8_t ke_mem_type)
{
    struct aes_ccm_env_tag *blk = &AES128_ccm_blk;

    aes_key_init(key);

    blk_mem_type = ke_mem_type;

    CCM_T = T;      // Length of Auth Bytes (TAG/MAC), options are 4,6,8,10,12,14 and 16
    CCM_Q = 15 - N; // Length of the encoded Plain Text length field, options are 2~8
    CCM_N = N;      // The nonce length

    blk->adata_blk = 0;
    blk->adata_len = 0;
    blk->adata_blk_cnt = 0;
    blk->b_blk = 0;
    blk->b_blk_cnt = 0;
    blk->x_blk = 0;
    blk->a_blk = 0;
    blk->a_blk_cnt = 0;
    blk->s_blk = 0;
}

void aes_ccm_cleanup(void)
{
    struct aes_ccm_env_tag *blk = &AES128_ccm_blk;

    if (blk->adata_blk)
    {
        ke_free(blk->adata_blk);
        blk->adata_blk = 0;
        blk->adata_len = 0;
        blk->adata_blk_cnt = 0;
    }

    if (blk->b_blk)
    {
        ke_free(blk->b_blk);
        blk->b_blk = 0;
        blk->b_blk_cnt = 0;
    }

    if (blk->x_blk)
    {
        ke_free(blk->x_blk);
        blk->x_blk = 0;
    }

    if (blk->a_blk)
    {
        ke_free(blk->a_blk);
        blk->a_blk = 0;
        blk->a_blk_cnt = 0;
    }

    if (blk->s_blk)
    {
        ke_free(blk->s_blk);
        blk->s_blk = 0;
    }
}

void aes_ccm_encrypt(uint8_t *payload, uint16_t payload_len, uint8_t *Nonce,
                     uint8_t *Adata, uint16_t Adata_len, uint8_t *output)
{
    struct aes_ccm_env_tag *blk = &AES128_ccm_blk;

    aes_set_ccm_param(blk, payload, payload_len, Nonce, Adata, Adata_len, ENCRYPT_PROCESS);
    aes_gen_ccm_bblk(blk, ENCRYPT_PROCESS);

    aes_gen_ccm_ablk(blk);
    aes_gen_ccm_sblk(blk);
    aes_gen_ccm_xblk(blk);
    aes_gen_ccm_output(blk, output);
}

uint8_t aes_ccm_decrypt(uint8_t *payload, uint16_t payload_len, uint8_t *Nonce,
                        uint8_t *Adata, uint16_t Adata_len, uint8_t *output)
{
    struct aes_ccm_env_tag *blk = &AES128_ccm_blk;

    aes_set_ccm_param(blk, payload, payload_len, Nonce, Adata, Adata_len, DECRYPT_PROCESS);
    aes_gen_ccm_bblk(blk, DECRYPT_PROCESS);

    aes_gen_ccm_ablk(blk);
    aes_gen_ccm_sblk(blk);

    return aes_check_ccm_payload(blk, output);
}
/// @} aes_ccm
