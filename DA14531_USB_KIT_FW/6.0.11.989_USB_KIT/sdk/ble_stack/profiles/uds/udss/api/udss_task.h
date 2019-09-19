/**
 ****************************************************************************************
 *
 * @file udss_task.h
 *
 * @brief Header file - User Data Service Server Role Task Declaration.
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

#ifndef UDSS_TASK_H_
#define UDSS_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup UDSSTASK Task
 * @ingroup UDSS
 * @brief User Data Service Task.
 *
 * The UDSS_TASK is responsible for handling the messages coming in and out of the
 * @ref UDSS block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_UDS_SERVER)

#include "ke_task.h"
#include "uds_common.h"
#include "udss.h"

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Possible states of the UDSS task
enum
{
    /// Idle state
    UDSS_IDLE,
    /// Busy state - characteristics are updated
    UDSS_BUSY,
    /// User Control Point operation is ongoing
    UDSS_UCP_OP_ONGOING,
    /// Number of defined states
    UDSS_STATE_MAX,
};

/// Messages for User Data Service Server
enum
{
    /// Request char value from the app
    UDSS_CHAR_VAL_REQ = KE_FIRST_MSG(TASK_ID_UDSS),
    /// Response with the requested char value
    UDSS_CHAR_VAL_RSP,
    /// Inform Host about the new char value set by peer device
    UDSS_SET_CHAR_VAL_IND,
    /// Confirm storing the new value
    UDSS_SET_CHAR_VAL_CFM,
    /// User Control Point Request from peer device
    UDSS_UCP_REQ_IND,
    /// User Control Point Response
    UDSS_UCP_REQ_CFM,
    /// Informs Host that peer has confirmed UCP operation result indication
    UDSS_UCP_CMP_EVT,
    /// Notify peer about the new db_change_incr_val (new values populated in data base)
    UDSS_DB_UPDATED_NTF_REQ,
    /// Error indication to Host
    UDSS_ERROR_IND,
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the User Data Service database
struct udss_db_cfg
{
    /// Features of UDS instance
    uint32_t enabled_chars;
    /// Wheter characteristic values can be updated by the Host - adds notifications to database changed increment characteristics
    bool local_char_update_enabled;
};

/// Parameters of the @ref UDSS_CHAR_VAL_REQ message
struct udss_char_val_req
{
    /// Characteristic Code
    uint8_t char_code;
};

/// Parameters of the @ref UDSS_CHAR_VAL_RSP and @ref UDSS_SET_CHAR_VAL_IND message - shall be dynamically allocated
struct udss_char_val
{
    /// Characteristic Code
    uint8_t char_code;
    /// Value length
    uint8_t length;
    /// Value
    uint8_t value[__ARRAY_EMPTY];
};

/// Parameters of the @ref UDSS_SET_CHAR_VAL_CFM
struct udss_char_val_cfm
{
    /// Characteristic Code
    uint8_t char_code;
    /// Status
    uint8_t status;
};

/// Parameters of the @ref UDSS_UCP_REQ_IND message
struct udss_ucp_req_ind
{
    /// User control point (UCP) request structure
    struct uds_ucp_req ucp_req;
};

/// Parameters of the @ref UDSS_UCP_REQ_CFM message
struct udss_ucp_req_cfm
{
    /// User control point (UCP) response structure
    struct uds_ucp_rsp ucp_rsp;
};

/// Parameters of the @ref UDSS_DB_UPDATED_NTF_REQ message
struct udss_db_updated_ntf_req
{
    /// Database changed increment value
    uint32_t db_change_incr_val;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler udss_default_handler;

#endif // BLE_UDS_SERVER

/// @} UDSSTASK

#endif // UDSS_TASK_H_
