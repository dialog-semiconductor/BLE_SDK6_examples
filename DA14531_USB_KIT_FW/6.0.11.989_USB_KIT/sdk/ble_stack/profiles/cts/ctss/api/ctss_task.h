/**
 ****************************************************************************************
 *
 * @file ctss_task.h
 *
 * @brief Header file - Current Time Service.
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

#ifndef CTSS_TASK_H_
#define CTSS_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup CTSSTASK Task
 * @ingroup CTSS
 * @brief Time Profile Server Task
 *
 * The CTSSTASK is responsible for handling the messages coming in and out of the
 * @ref CTS reporter block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#if (BLE_CTS_SERVER)
#include "ke_task.h"
#include "ctss.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of Time Server task instances
#define CTSS_IDX_MAX     (1)

// CTS Configuration Flag Masks
#define CTSS_CURRENT_TIME_MASK      (0x0F)
#define CTSS_LOC_TIME_INFO_MASK     (0x30)
#define CTSS_REF_TIME_INFO_MASK     (0xC0)

/// Possible states of the CTSS task
enum
{
    /// idle state
    CTSS_IDLE,
    /// connected state
    CTSS_BUSY,

    /// Number of defined states.
    CTSS_STATE_MAX
};

/// Messages for Current Time Server
enum
{
    /// Request Curent Time from APP
    CTSS_READ_REQ_IND,
    /// Application response to CTSS_READ_REQ_IND
    CTSS_READ_RSP,
    ///
    CTSS_WRITE_CUR_TIME_REQ_IND,
    CTSS_WRITE_LOC_TIME_REQ_IND,
    CTSS_WRITE_RSP,
    /// Notify Current Time Request from APP
    CTSS_NTF_CURR_TIME_REQ,
    /// Notify Current Time Response to APP
    CTSS_NTF_CURR_TIME_CFM,
    /// Update Local Time Information Request from APP
    CTSS_UPD_LOCAL_TIME_INFO_REQ,
    /// Update Local Time Information Response to APP
    CTSS_UPD_LOCAL_TIME_INFO_RSP,
    /// Inform APP about modification of Current Time Characteristic Client. Charact. Cfg
    CTSS_CURRENT_TIME_CCC_IND,
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the @ref CTSS_NTF_CURR_TIME_REQ message
struct ctss_ntf_curr_time_req
{
    /// Current Time
    struct cts_curr_time current_time;
};

/// Parameters of the @ref CTSS_NTF_CURR_TIME_RSP message
struct ctss_ntf_curr_time_rsp
{
    ///status
    uint8_t status;
};

/// Parameters of the @ref CTSS_UPD_LOCAL_TIME_INFO_REQ message
struct ctss_upd_local_time_info_req
{
    /// Local Time Information
    struct cts_loc_time_info loc_time_info;
};

/// Parameters of the @ref CTSS_UPD_LOCAL_TIME_INFO_RSP message
struct ctss_upd_local_time_info_rsp
{
    ///status
    uint8_t status;
};

/// Parameters of the @ref CTSS_UPD_REF_TIME_INFO_REQ message
struct ctss_upd_ref_time_info_req
{
    /// Reference Time Information
    struct cts_ref_time_info ref_time_info;
};

/// Parameters of the @ref CTSS_CURRENT_TIME_CCC_IND message
struct ctss_current_time_ccc_ind
{
    /// Configuration Value
    uint16_t cfg_val;
};

struct ctss_read_req_ind
{
    /// Requested value
    uint8_t value;
};

/// Value for requested attribute for peer device
struct ctss_read_rsp
{
    /// Requested value
    uint8_t value;
    /// Value length
    uint8_t length;
    /// Value data
    uint8_t data[__ARRAY_EMPTY];
};

/// Value for requested attribute for peer device
struct ctss_write_rsp
{
    /// Requested value
    uint8_t value;
    /// Status
    uint8_t status;
};

struct ctss_cur_time_write_ind
{
    /// Requested value
    uint8_t value;
    /// Current Time
    struct cts_curr_time current_time;
};

struct ctss_loc_time_info_write_ind
{
    /// Requested value
    uint8_t value;
    /// Current Time
    struct cts_loc_time_info local_time_info;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
extern const struct ke_state_handler ctss_default_handler;

#endif //BLE_CTS_SERVER

/// @} CTSSTASK
#endif // CTSS_TASK_H_
