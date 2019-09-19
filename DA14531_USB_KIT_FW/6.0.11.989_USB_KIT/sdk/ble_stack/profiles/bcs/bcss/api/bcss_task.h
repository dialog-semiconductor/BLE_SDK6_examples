/**
 ****************************************************************************************
 *
 * @file bcss_task.h
 *
 * @brief Header file - Body Composition Service Server Task.
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

#ifndef BCSS_TASK_H_
#define BCSS_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup BCSSTASK Body Composition Service Server
 * @ingroup BCSS
 * @brief Body Composition Service Server
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_BCS_SERVER)

#include "bcs_common.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Possible states of the DISS task
enum
{
    /// Idle state
    BCSS_IDLE,
    /// Connected state
    BCSS_BUSY,
    /// Number of defined states.
    BCSS_STATE_MAX
};

/// Messages for Body Composition Service Server
enum
{
    /// Start the Body Composition Service Server - at connection
    BCSS_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_BCSS),
    /// BCS enable response
    BCSS_ENABLE_RSP,
    /// Body Composition Measurement Value Indication Request
    BCSS_MEAS_VAL_IND_REQ,
    /// Body Composition Measurement Value Indication Response
    BCSS_MEAS_VAL_IND_RSP,
    /// Inform APP the if Notification Configuration has been changed
    BCSS_MEAS_VAL_IND_CFG_IND,
};

/*
 * APIs Structures
 ****************************************************************************************
 */

/// Parameters of the @ref BCSS_CREATE_DB_REQ message
struct bcss_db_cfg
{
    /// Type of service to instantiate (primary or secondary)
    uint8_t is_primary;
    /// Body Composition features
    uint32_t features;
};

/// Parameters of the @ref BCSS_ENABLE_REQ message
struct bcss_enable_req
{
    /// Body Composition Indication configuration
    uint16_t meas_ind_en;
};

/// Parameters of the @ref BCSS_ENABLE_RSP message
struct bcss_enable_rsp
{
    /// CCC initial configuration
    uint16_t init_cfg_ind;
    /// Operation status
    uint8_t status;
};

/// Parameters of the @ref BCSS_MEAS_VAL_IND_REQ message
struct bcss_meas_val_ind_req
{
    /// Measurement value
    bcs_meas_t meas;
};

/// Parameters of the @ref BCSS_MEAS_VAL_IND_RSP message
struct bcss_meas_val_ind_rsp
{
    /// Status
    uint8_t status;
};

/// Parameters of the @ref BCSS_MEAS_VAL_IND_CFG_IND message
struct bcss_meas_val_ind_cfg_ind
{
    /// Notification Configuration
    uint16_t ind_cfg;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler bcss_default_handler;

#endif // (BLE_BCS_SERVER)

/// @} BCSSTASK

#endif // BCSS_TASK_H_
