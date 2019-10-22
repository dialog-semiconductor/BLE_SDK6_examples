/**
 ****************************************************************************************
 *
 * @file bmsc_task.h
 *
 * @brief Header file - Bond Management Service Client Role Task Declaration.
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

#ifndef BMSC_TASK_H_
#define BMSC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup BMSCTASK Bond Management Service Client Task
 * @ingroup BMSC
 * @brief Bond Management Service Client Task
 *
 * The BMSCTASK is responsible for handling the messages coming in and out of the
 * @ref BMSC monitor block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_BMS_CLIENT)
#include "ke_task.h"
#include "gattc_task.h"
#include "co_error.h"
#include "bmsc.h"
#include "bms_common.h"
#include "prf_types.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the BMSC task
enum bmsc_state
{
    /// Free state
    BMSC_FREE,
    /// IDLE state
    BMSC_IDLE,
    /// Discovering Bond Management SVC and CHars
    BMSC_DISCOVERING,
    /// Busy state
    BMSC_BUSY,
    /// Number of defined states
    BMSC_STATE_MAX
};

enum bmsc_msg_id
{
    /// Start the Bond Management Service Client - at connection
    BMSC_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_BMSC),
    /// Confirm that cfg connection has finished with discovery results, or that normal cnx started
    BMSC_ENABLE_RSP,
    /// Generic error message
    BMSC_ERROR_IND,
    /// APP request for control point write
    BMSC_WR_CNTL_POINT_REQ,
    /// Write response to APP
    BMSC_WR_CNTL_POINT_RSP,
    /// APP request for features read
    BMSC_RD_FEATURES_REQ,
    /// Read response to APP
    BMSC_RD_FEATURES_RSP,
};

/// Parameters of the @ref BMSC_ENABLE_REQ message
struct bmsc_enable_req
{
    /// Connection type
    uint8_t con_type;
    /// Existing handle values bms
    struct bms_content bms;
};

/// Parameters of the @ref BMSC_ENABLE_CFM message
struct bmsc_enable_rsp
{
    /// status
    uint8_t status;
    /// Existing handle values bms
    struct bms_content bms;
};

///Parameters of the @ref BMSC_ERROR_IND message
struct bmsc_error_ind
{
    /// Status
    uint8_t  status;
};

/// Parameters of the @ref BMSC_RD_FEATURES_RSP message - dynamically allocated
struct bmsc_rd_features_rsp
{
    /// Connection index
    uint8_t conidx;
    /// Status
    uint8_t status;
    /// Bond Management server features length
    uint8_t length;
    /// Bond Management server features
    uint8_t features[__ARRAY_EMPTY];
};

/// Parameters of the @ref BMSC_WR_CNTL_POINT_REQ message - dynamically allocated
struct bmsc_wr_cntl_point_req
{
    /// Operation code and operand
    struct bms_cntl_point_op operation;
};

/// Parameters of the @ref BMSC_WR_CNTL_POINT_RSP message
struct bmsc_wr_cntl_point_rsp
{
    /// Status
    uint8_t  status;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler bmsc_default_handler;
extern ke_state_t bmsc_state[BMSC_IDX_MAX];

#endif // (BLE_BMS_CLIENT)

/// @} BMSCTASK

#endif /* BMSC_TASK_H_ */
