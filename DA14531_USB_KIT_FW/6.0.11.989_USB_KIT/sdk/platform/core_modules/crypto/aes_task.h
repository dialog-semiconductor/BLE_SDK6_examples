/**
 ****************************************************************************************
 *
 * @file aes_task.h
 *
 * @brief AES task implementation header file.
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

#ifndef AES_TASK_H_
#define AES_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup aes_TASK Security Manager Protocol Manager Task
 * @ingroup aes
 * @brief Handles ALL messages to/from aes block.
 *
 * The aes task is responsible for all security related functions not related to a
 * specific connection with a peer.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "rwip_config.h"
#include "gapm_task.h"          // GAP Manager Task Definitions
#include "gap.h"                // GAP Definitions

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum aes instance number
#define AES_IDX_MAX                    (0x01)

/**
 * Operation Codes (Mapped on GAPM Operation Codes)
 */
/// Use Encryption Block
#define AES_OP_USE_ENC_BLOCK           (GAPM_USE_ENC_BLOCK)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/**
 * States of aes Task
 */
enum aes_state_id
{
    /// IDLE state
    AES_IDLE,
    /// BUSY state: Communication with LLM ongoing state
    AES_BUSY,

    // Number of defined states.
    AES_STATE_MAX
};

/**
 * aes block API message IDs
 */
enum aes_msg_id
{
    ///Encryption Toolbox Access
    AES_USE_ENC_BLOCK_CMD          = KE_FIRST_MSG(TASK_AES),

    /// Command Complete Event
    AES_CMP_EVT
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// AES Encrypt complete event structure
struct aes_cmp_evt
{
    /// Status of the command reception
    uint8_t status;
    ///Encrypted data to return to command source.
    uint8_t encrypted_data[16];
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler aes_state_handler[AES_STATE_MAX];
extern const struct ke_state_handler aes_default_handler;
extern ke_state_t aes_state[AES_IDX_MAX];

#endif // (AES_TASK_H_)

/// @} aes_TASK
