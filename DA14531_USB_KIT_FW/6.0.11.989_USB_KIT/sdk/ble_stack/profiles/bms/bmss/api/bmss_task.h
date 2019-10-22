/**
 ****************************************************************************************
 *
 * @file bmss_task.h
 *
 * @brief Header file - Bond Management Service Server Role Task Declaration.
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

#ifndef BMSS_TASK_H_
#define BMSS_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup BMSSTASK Task
 * @ingroup BMSS
 * @brief Bond Management 'Profile' Task.
 *
 * The BMSS_TASK is responsible for handling the messages coming in and out of the
 * @ref BMSS block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
*/

#if (BLE_BMS_SERVER)

#include "ke_task.h"
#include "bms_common.h"
#include "bmss.h"

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Possible states of the BMSS task
enum
{
    /// Idle state
    BMSS_IDLE,
    /// Busy state
    BMSS_BUSY,
    /// Number of defined states
    BMSS_STATE_MAX,
};

/// Messages for Bond Management Server
enum
{
    /// Error Indication
    BMSS_ERROR_IND = KE_FIRST_MSG(TASK_ID_BMSS),
    /// Delete Bond Request Indication
    BMSS_DEL_BOND_REQ_IND,
    /// Delete Bond Write Response
    BMSS_DEL_BOND_CFM,
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

///Parameters of the Bond Management service database
struct bmss_db_cfg
{
    /// Features of BMS instance
    uint32_t features;
    /// Extended properties - reliable writes (supported or not)
    uint8_t reliable_writes;
};

/// Parameters of the @ref BMSS_DEL_BOND_REQ_IND message - dynamically allocated
struct bmss_del_bond_req_ind
{
    /// Operation code and operand
    struct bms_cntl_point_op operation;
};

/// Parameters of the @ref BMSS_DEL_BOND_CFM message
struct bmss_del_bond_cfm
{
    /// Bond deletion result status
    uint8_t status;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler bmss_state_handler[BMSS_STATE_MAX];
extern const struct ke_state_handler bmss_default_handler;

#endif // BLE_BMS_SERVER

/// @} BMSSTASK

#endif // BMSS_TASK_H_
