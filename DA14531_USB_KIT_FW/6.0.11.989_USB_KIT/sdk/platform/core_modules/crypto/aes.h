/**
 ****************************************************************************************
 *
 * @file aes.h
 *
 * @brief AES header file.
 *
 * Copyright (c) 2017-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef AES_H_
#define AES_H_

/**
 ****************************************************************************************
 * @addtogroup aes Security Manager Protocol Manager
 * @ingroup SMP
 * @brief Security Manager Protocol Manager.
 *
 * This Module allows the 1-instanced modules to communicate with multi-instanced SMPC module.
 * It is only an intermediary between the actual SMPC handling SM behavior, and
 * LLM, GAP, or GATT which only indicate the index of the connection for which
 * SMPC actions are necessary.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "co_bt.h"
#include "ke_msg.h"
#include "sw_aes.h"

/*
 * STRUCTURES
 ****************************************************************************************
 */
typedef AES_CTX AES_KEY;
/**
 * aes environment structure
 */
struct aes_env_tag
{
    /// Request operation Kernel message
    void *operation;
    
    /// Operation requester task id
    ke_task_id_t requester;

    /// Encryption/Decryption operation mode selection
    uint8_t enc_dec;
    
    /// Key length
    uint32_t key_len;
    
    /// Key
    uint8_t *key;
    
    AES_KEY aes_key;
    
    /// Plaintext
    uint8_t *in;
    
    /// Plaintext length
    uint32_t in_len;
    
    /// Plaintext index
    uint32_t in_cur;
    
    /// Ciphered text
    uint8_t *out;
    
    /// Ciphered text length
    uint32_t out_len;
    
    /// Ciphered text index
    uint32_t out_cur;

    /// Callback function to be called upon encryption/decryption completion
    void (*aes_done_cb)(uint8_t status);

    /// Synchronous/asynchronous mode selection
    uint8_t ble_flags;
 };

/*
 * GLOBAL VARIABLES DECLARATION
 ****************************************************************************************
 */

extern struct aes_env_tag aes_env;
extern uint8_t aes_out[ENC_DATA_LEN];

/*
 * PUBLIC FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the AES task.
 * @param[in] reset       FALSE will create the task, TRUE will just reset the environment.
 * @param[in] aes_done_cb Callback function to be called upon encryption/decryption completion
 ****************************************************************************************
 */
void aes_init(bool reset, void (*aes_done_cb)(uint8_t status));

/**
 ****************************************************************************************
 * @brief AES encrypt/decrypt operation.
 * @param[in] key           The key data.
 * @param[in] key_len       The key data length in bytes (should be 16).
 * @param[in] in            The input data block.
 * @param[in] in_len        The input data block length.
 * @param[in] out           The output data block.
 * @param[in] out_len       The output data block length.
 * @param[in] enc_dec       AES_ENCRYPT for encryption, AES_DECRYPT for decryption.
 * @param[in] aes_done_cb   The callback to be called at the end of each operation.
 * @param[in] ble_flags     Used to specify whether the encryption/decryption
 *                          will be performed synchronously or asynchronously (message based)
 *                          also if ble_safe is specified in ble_flags rwip_schedule() will be called
 *                          to avoid missing any ble events.
 * @return                   0 if successful,
 *                          -1 if userKey or key are NULL,
 *                          -2 if AES task is busy,
 *                          -3 if enc_dec not 0/1,
 *                          -4 if key_len not 16.
 ****************************************************************************************
 */
int aes_operation(uint8_t *key,
                  uint8_t key_len,
                  uint8_t *in,
                  uint32_t in_len,
                  uint8_t *out,
                  uint32_t out_len,
                  uint8_t enc_dec,
                  void (*aes_done_cb)(uint8_t status),
                  uint8_t ble_flags);

/**
 ****************************************************************************************
 * @brief Send a aes_CMP_EVT message to the task which sent the request.
 *
 * @param[in] cmd_src_id            Command source ID
 * @param[in] operation             Command operation code
 * @param[in] status                Status of the request
 ****************************************************************************************
 */
void aes_send_cmp_evt(ke_task_id_t cmd_src_id, uint8_t operation, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send an encryption request to the LLM.
 ****************************************************************************************
 */
void aes_send_encrypt_req(uint8_t *operand_1, uint8_t *operand_2);

#endif // (AES_H_)

/// @} aes
