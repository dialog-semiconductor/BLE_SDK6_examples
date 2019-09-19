/**
 ****************************************************************************************
 *
 * @file ctsc_task.h
 *
 * @brief Header file - CTSCTASK.
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

#ifndef CTSC_TASK_H_
#define CTSC_TASK_H_

/**
************************************************************************************
* @addtogroup CTSC Task
* @ingroup CTSC
* @brief Current Time Service Client Task
* @{
************************************************************************************
*/

/*
* INCLUDE FILES
************************************************************************************
*/

#if (BLE_CTS_CLIENT)
#include "prf_types.h"
#include "cts_common.h"
#include "ctsc.h"

/*
* DEFINES
****************************************************************************************
*/

/// Maximum number of Current Time Client task instances
#define CTSC_IDX_MAX        (BLE_CONNECTION_MAX)

/*
* TYPE DEFINITIONS
****************************************************************************************
*/

enum
{
    /// Not Connected State
    CTSC_FREE,
    /// Idle state
    CTSC_IDLE,
    /// Discovering State
    CTSC_DISCOVERING,
    /// Busy State
    CTSC_BUSY,

    /// Number of defined states.
    CTSC_STATE_MAX
};

enum ctsc_msg_id
{
    // Start the Current Time Service Client
    CTSC_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_CTSC),
    /// Confirm that cfg connection has finished with discovery results, or that normal cnx started
    CTSC_ENABLE_RSP,

    /// Message to read Current Time
    CTSC_READ_CT_REQ,
    /// Response for Current Time read request @ref CTSC_READ_CT_REQ
    CTSC_READ_CT_RSP,

    /// Message to read Local Time Information
    CTSC_READ_LTI_REQ,
    /// Response for Local Time read request @ref CTSC_READ_LTI_REQ
    CTSC_READ_LTI_RSP,

    /// Message to read Reference Time Information
    CTSC_READ_RTI_REQ,
    /// Response for Reference Time Information read request @ref CTSC_READ_RTI_REQ
    CTSC_READ_RTI_RSP,

    /// Message to read Characteristic Descriptor
    CTSC_READ_DESC_REQ,
    /// Response for Characteristic Descriptor read request @ref CTSC_READ_DESC_REQ
    CTSC_READ_DESC_RSP,

    /// Message to write Current Time
    CTSC_WRITE_CT_REQ,
    /// Response for Current Time write request @ref CTSC_WRITE_CT_REQ
    CTSC_WRITE_CT_RSP,

    /// Message to write Local Time Information
    CTSC_WRITE_LTI_REQ,
    /// Response for Local Time Information write request @ref CTSC_WRITE_LTI_REQ
    CTSC_WRITE_LTI_RSP,

    /// Message to write Characteristic Descriptor
    CTSC_WRITE_DESC_REQ,
    /// Response for Descriptor write request @ref CTSC_WRITE_DESC_REQ
    CTSC_WRITE_DESC_RSP,

    /// Current Time value send to APP
    CTSC_CT_IND,
};

/// Parameters of the @ref CTSC_ENABLE_REQ message
struct ctsc_enable_req
{
    /// Connection type
    uint8_t con_type;
    /// CTSS handle values and characteristic properties
    struct ctsc_cts_content cts;
};

/// Parameters of the @ref CTSC_ENABLE_RSP message
struct ctsc_enable_rsp
{
    /// Connection type
    uint8_t status;
    /// CTS handle values and characteristic properties
    struct ctsc_cts_content cts;
};

/// Parameters for the @ref CTSC_READ_CT_REQ,
//                     @ref CTSC_READ_LTI_REQ,
//                     @ref CTSC_READ_RTI_REQ,
//                     @ref CTSC_READ_DESC_REQ
struct ctsc_read_req
{
    /// Characteristic value code
    uint8_t  char_code;
};

/// Parameters of the @ref CTSC_READ_xxx_RSP and CTSC_WRITE_xxx_RSP
struct ctsc_rsp
{
    uint8_t status;
    uint8_t char_code;        /// Characteristic or descriptor
};


/// Parameters of the @ref CTSC_READ_CT_RSP message
struct ctsc_read_ct_rsp
{
    struct ctsc_rsp rsp;
    struct cts_curr_time ct_val;
};

/// Parameters of the @ref CTSC_READ_LTI_RSP message
struct ctsc_read_lti_rsp
{
    struct ctsc_rsp rsp;
    struct cts_loc_time_info lti_val;
};

/// Parameters of the @ref CTSC_READ_RTI_RSP message
struct ctsc_read_rti_rsp
{
    struct ctsc_rsp rsp;
    struct cts_ref_time_info rti_val;
};

/// Parameters for the @ref CTSC_READ_DESC_RSP message
struct ctsc_read_desc_rsp
{
    struct ctsc_rsp rsp;
    uint16_t ntf_cfg;
};

/// Parameters of the @ref CTSC_WRITE_xxx_REQ, message
struct ctsc_write_req
{
    uint8_t char_code;
};

/// Parameters of the @ref CTSC_WRITE_DESC_REQ message
struct ctsc_write_desc_req
{
    struct ctsc_write_req desc;
    uint16_t value;
};

/// Parameters of the @ref CTSC_WRITE_CT_REQ message
struct ctsc_write_ct_req
{
    struct ctsc_write_req chr;
    struct cts_curr_time ct_val;
};

/// Parameters of the @ref CTSC_WRITE_LTI_REQ message
struct ctsc_write_lti_req
{
    struct ctsc_write_req chr;
    struct cts_loc_time_info lti_val;
};

/// Parameters for the @ref CTSC_CT_IND message
struct ctsc_ct_ind
{
    uint8_t status;
    /// Current Time Value
    struct cts_curr_time ct_val;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
extern const struct ke_state_handler ctsc_default_handler;

#endif // BLE_CTS_CLIENT

/// @} CTSCTASK

#endif // CTSC_TASK_H_
