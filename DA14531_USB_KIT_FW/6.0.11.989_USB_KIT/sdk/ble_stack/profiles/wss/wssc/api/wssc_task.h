/**
 ****************************************************************************************
 *
 * @file wssc_task.h
 *
 * @brief Header file - Weight Scale Service Client Role Task Declaration.
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

#ifndef WSSC_TASK_H_
#define WSSC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup WSSCTASK Weight Scale Service Client Task
 * @ingroup WSSC
 * @brief Weight Scale Service Client Task
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_WSS_COLLECTOR)
#include "ke_task.h"
#include "gattc_task.h"
#include "co_error.h"
#include "wssc.h"
#include "wss_common.h"
#include "prf_types.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the WSSC task
enum wssc_state
{
    /// Free state
    WSSC_FREE,
    /// IDLE state
    WSSC_IDLE,
    /// Discovering Weight Scale SVC and CHars
    WSSC_DISCOVERING,
    /// Busy state
    WSSC_BUSY,

    /// Number of defined states
    WSSC_STATE_MAX
};

enum
{
    /// Start the Weight Scale Collector profile - at connection
    WSSC_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_WSSC),
    /// Confirm that cfg connection has finished with discovery results, or that normal cnx started
    WSSC_ENABLE_RSP,
    /// Generic error message
    WSSC_ERROR_IND,

    /// Generic message to read WSS characteristics
    WSSC_RD_CHAR_REQ,
    /// GEneric message for read response
    WSSC_RD_CHAR_RSP,
    /// Message for configuration
    WSSC_CFG_INDNTF_REQ,
    /// Configuration response
    WSSC_CFG_INDNTF_RSP,

    /// Weight Scale value send to APP
    WSSC_WS_IND,
};

/// Codes for reading WSS characteristics
enum
{
    // Read Wight Scale Feature
    WSSC_RD_WSS_WS_FEAT,

    // Read Weight Measurment Client Cfg Descriptor
    WSSC_RD_WSS_WM_CLI_CFG
};

/// Parameters of the @ref WSSC_ENABLE_REQ message
struct wssc_enable_req
{
    /// Connection type
    uint8_t con_type;
    /// WSS handle values and characteristic properties
    struct wss_content wss;
};

/// Parameters of the @ref WSSC_ENABLE_CFM message
struct wssc_enable_rsp
{
    /// Connection type
    uint8_t status;
    /// WSS handle values and characteristic properties
    struct wss_content wss;
};

/// Parameters of the @ref WSSC_ERROR_IND message
struct wssc_error_ind
{
    /// Status
    uint8_t  status;
};

/// Parameters of the @ref WSSC_RD_CHAR_REQ message
struct wssc_rd_char_req
{
    /// Characteristic value code
    uint8_t char_code;
};

/// Parameters of the @ref WSSC_RD_CHAR_RSP message
struct wssc_rd_char_rsp
{
    /// Attribute data information
    struct prf_att_info info;
};

/// Parameters of the @ref WSSC_CFG_INDNTF_REQ message
struct wssc_cfg_indntf_req
{
    /// Stop/notify/indicate value to configure into the peer characteristic
    uint16_t cfg_val;
};

/// Parameters of the @ref WSSC_WR_CHAR_RSP message
struct wssc_cfg_indntf_rsp
{
    /// Status
    uint8_t  status;
};

/// Parameters of the @ref WSSC_WS_IND message
struct wssc_ws_ind
{
    /// Weight measurment value
    struct wss_wt_meas meas_val;
};


/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler wssc_default_handler;
extern ke_state_t wssc_state[WSSC_IDX_MAX];

#endif // (BLE_WSS_COLLECTOR)

/// @} WSSCTASK

#endif /* WSSC_TASK_H_ */
