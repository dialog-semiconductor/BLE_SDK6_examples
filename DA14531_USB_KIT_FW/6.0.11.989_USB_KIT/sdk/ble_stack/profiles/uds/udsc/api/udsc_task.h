/**
 ****************************************************************************************
 *
 * @file udsc_task.h
 *
 * @brief Header file - User Data Service Client Role Task Declaration.
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef UDSC_TASK_H_
#define UDSC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup UDSCTASK Client Task
 * @ingroup UDSC
 * @brief User Data Service Client Task.
 *
 * The UDSC_TASK is responsible for handling the messages coming in and out of the
 * @ref UDSC block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
*/

#include "rwip_config.h"

#if (BLE_UDS_CLIENT)

#include "ke_task.h"
#include "gattc_task.h"
#include "co_error.h"

#include "uds_common.h"
#include "udsc.h"
#include "prf_types.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the UDSC task
enum
{
    /// Free state
    UDSC_FREE,
    /// Idle state
    UDSC_IDLE,
    /// Discovering service, characteristics and descriptors
    UDSC_DISCOVERING,
    /// Busy state
    UDSC_BUSY,

    /// Number of defined states
    UDSC_STATE_MAX,
};

/// Messages for User Data Service Client
enum udsc_msg_id
{
    /// Start the User Data Service Client - at connection
    UDSC_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_UDSC),
    /// Confirm that cfg connection has finished with discovery results, or that normal cnx started
    UDSC_ENABLE_RSP,
    /// Perform certain database operation using User Control Point opcodes
    UDSC_UCP_OP_REQ,
    /// Result of the requested database operation
    UDSC_UCP_OP_CFM,
    /// Start writing char cteristic values
    UDSC_SET_CHAR_VAL_BEGIN_REQ,
    /// Message to set certain Characteristic value on the remote device
    UDSC_SET_CHAR_VAL_REQ,
    /// Result of the database write
    UDSC_SET_CHAR_VAL_CFM,
    /// Finish writing char cteristic values
    UDSC_SET_CHAR_VAL_END_REQ,
    /// Read database entry
    UDSC_READ_CHAR_VAL_REQ,
    /// Result of database entry read
    UDSC_READ_CHAR_VAL_CFM,
    /// Message for configuring DB Change Incr. or User Control Point descr.
    UDSC_NTFIND_CFG_REQ,
    /// Completeness indication
    UDSC_CMP_EVT_IND,
    /// Notification that server has updated it's database
    UDSC_DB_CHANGE_NTF,
    /// Procedure Timer Timeout
    UDSC_TIMEOUT_TIMER_IND
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the @ref UDSC_ENABLE_REQ message
struct udsc_enable_req
{
    /// Connection type
    uint8_t con_type;
    /// Existing handle values
    struct uds_content uds;
};

/// Parameters of the @ref UDSC_ENABLE_RSP message
struct udsc_enable_rsp
{
    /// status
    uint8_t status;
    /// Existing handle values
    struct uds_content uds;
};

/// Parameters of the @ref UDSC_UCP_OP_REQ message
struct udsc_ucp_op_req
{
    /// User control point request
    struct uds_ucp_req req;
};

/// Parameters of the @ref UDSC_UCP_OP_CFM message
struct udsc_ucp_op_cfm
{
    /// status
    uint8_t status;
    /// User control point response
    struct uds_ucp_rsp cfm;
};

/// Parameters of the @ref UDSC_SET_CHAR_VAL_REQ message - shall be dynamically allocated
struct udsc_set_char_val_req
{
    /// Characteristic Code
    uint8_t char_code;
    /// Value length
    uint8_t val_len;
    /// Value
    uint8_t val[__ARRAY_EMPTY];
};

/// Parameters of the @ref UDSC_SET_CHAR_VAL_CFM message
struct udsc_set_char_val_cfm
{
    /// Status
    uint8_t status;
};

/// Parameters of the @ref UDSC_READ_CHAR_VAL_REQ message
struct udsc_read_char_val_req
{
    /// Characteristic Code
    uint8_t char_code;
};

/// Parameters of the @ref UDSC_READ_CHAR_VAL_CFM message - shall be dynamically allocated
struct udsc_read_char_val_cfm
{
    /// Characteristic Code
    uint8_t char_code;
    /// Status of request
    uint8_t  status;
    /// Value length
    uint8_t val_len;
    /// Value
    uint8_t val[__ARRAY_EMPTY];
};

/// Parameters of the @ref UDSC_NTFIND_CFG_REQ message
struct udsc_ntfind_cfg_req
{
    /// Stop/notify/indicate value to configure into the peer characteristic
    uint16_t cfg_val;
    /// Descriptor to configure
    uint8_t desc_code;
};

/// Parameters of the @ref UDSC_CMP_EVT_IND message
struct udsc_cmp_evt_ind
{
    /// Status
    uint8_t status;
};

/// Parameters of the @ref UDSC_DB_CHANGE_NTF message
struct udsc_db_change_ntf
{
    /// Value of the database change increment characteristic
    uint32_t incr_val;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler udsc_default_handler;
extern ke_state_t udsc_state[UDSC_IDX_MAX];

#endif // BLE_UDS_CLIENT

/// @} UDSCTASK

#endif // UDSC_TASK_H_
