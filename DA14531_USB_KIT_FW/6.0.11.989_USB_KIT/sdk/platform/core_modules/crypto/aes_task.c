/**
 ****************************************************************************************
 *
 * @file aes_task.c
 *
 * @brief AES task implementation.
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

/**
 ****************************************************************************************
 * @addtogroup aes_TASK
 * @ingroup aes
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <string.h>
#include "ke_msg.h"
#include "smp_common.h"
#include "gapm_task.h"
#include "aes.h"
#include "aes_task.h"

/*
 * FUNCTION DEFINITONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] req       Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int aes_use_enc_block_cmd_handler(ke_msg_id_t const msgid,
                                         struct gapm_use_enc_block_cmd const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
    // Message Status
    uint8_t msg_status = KE_MSG_CONSUMED;

    // Check if the aes task is not busy
    if (ke_state_get(dest_id) == AES_IDLE)
    {
        // Check the operation code and the address type
        if (param->operation == AES_OP_USE_ENC_BLOCK || param->operation == AES_OP_USE_ENC_BLOCK+1)
        {
            // Keep the command message until the end of the request
            aes_env.operation = (void *)param;
            aes_env.requester = src_id;
            // Inform the kernel that this message cannot be unallocated
            msg_status = KE_MSG_NO_FREE;

            // Ask for encryption/decryption
            aes_send_encrypt_req((uint8_t *)&param->operand_1[0], (uint8_t *)&param->operand_2[0]);

            // Go in a busy state
            ke_state_set(TASK_AES, AES_BUSY);
        }
        else
        {
            aes_send_cmp_evt(src_id,
                             AES_OP_USE_ENC_BLOCK,
                             SMP_GEN_PAIR_FAIL_REASON(SMP_PAIR_FAIL_REASON_MASK, SMP_ERROR_INVALID_PARAM));
        }
    }
    else
    {
        // Push the message in the message queue so that it can be handled later
        msg_status = KE_MSG_SAVED;
    }

    return (int)msg_status;
}

/**
 ****************************************************************************************
 * @brief Encrypt command complete event handler.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return Received kernel message status (consumed or not).
 ****************************************************************************************
 */
static int aes_cmp_evt_handler(ke_msg_id_t const msgid,
                               struct aes_cmp_evt const *param,
                               ke_task_id_t const dest_id,
                               ke_task_id_t const src_id)
{
    // Check if a command is currently handled, drop the message if not the case.
    if (ke_state_get(dest_id) == AES_BUSY)
    {
        ASSERT_ERROR(aes_env.operation != NULL);
        switch (((struct smp_cmd *)(aes_env.operation))->operation)
        {
            case (AES_OP_USE_ENC_BLOCK):
            {
                uint8_t status = SMP_ERROR_NO_ERROR;

                // Check the returned status
                if (param->status == SMP_ERROR_NO_ERROR)
                {
                }
                else
                {
                    status = SMP_ERROR_LL_ERROR;
                }

                // Send the command complete event message
                aes_send_cmp_evt(aes_env.requester, AES_OP_USE_ENC_BLOCK, status);
            } break;
            case (AES_OP_USE_ENC_BLOCK + 1):
            {
                uint8_t status = SMP_ERROR_NO_ERROR;
                // Send the command complete event message
                aes_send_cmp_evt(aes_env.requester, AES_OP_USE_ENC_BLOCK + 1, status);
            } break;
            default:
            {
                // Drop the message
                ASSERT_ERROR(0);
            } break;
        }
    }

    return KE_MSG_CONSUMED;
}

/*
 * TASK DESCRIPTOR DEFINITIONS
 ****************************************************************************************
 */

/// The default message handlers
const struct ke_msg_handler aes_default_state[] =
{
    {AES_USE_ENC_BLOCK_CMD,        (ke_msg_func_t)aes_use_enc_block_cmd_handler},
    {AES_CMP_EVT,                  (ke_msg_func_t)aes_cmp_evt_handler},
};

/// State handlers table
const struct ke_state_handler aes_state_handler[AES_STATE_MAX] =
{
    [AES_IDLE]     = KE_STATE_HANDLER_NONE,
    [AES_BUSY]     = KE_STATE_HANDLER_NONE,
};

/// Default state handlers table
const struct ke_state_handler aes_default_handler = KE_STATE_HANDLER(aes_default_state);

/// Defines the place holder for the states of all the task instances.
ke_state_t aes_state[AES_IDX_MAX] __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY;

/// @} aes_TASK
