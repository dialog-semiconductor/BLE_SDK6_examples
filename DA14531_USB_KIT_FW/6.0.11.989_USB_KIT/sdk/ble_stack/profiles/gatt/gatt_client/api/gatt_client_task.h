/**
 ****************************************************************************************
 *
 * @file gatt_client_task.h
 *
 * @brief Header file - Generic Attribute Profile Service Client Role Task Declaration.
 *
 * Copyright (c) 2018 Dialog Semiconductor. All rights reserved.
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

#ifndef GATT_CLIENT_TASK_H_
#define GATT_CLIENT_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup GATTCLIENT Generic Attribute Profile Service Client Task
 * @ingroup GATT_CLIENT
 * @brief Generic Attribute Profile Service Client
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_GATT_CLIENT)

#include "ke_task.h"
#include "gattc_task.h"
#include "co_error.h"
#include "gatt_client.h"
#include "prf_types.h"


/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// Possible states of the GATT client task
enum gatt_client_state
{
    /// Free state
    GATT_CLIENT_FREE,
    /// IDLE state
    GATT_CLIENT_IDLE,
    /// Discovering GATT Service
    GATT_CLIENT_DISCOVERING,
    /// Busy state
    GATT_CLIENT_BUSY,

    /// Number of defined states
    GATT_CLIENT_STATE_MAX
};

/// Message IDs
enum
{
    /// Enable the GATT Client task - at connection
    GATT_CLIENT_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_GATT_CLIENT),
    /// Enable response
    GATT_CLIENT_ENABLE_RSP,
    /// Write Indication Configuration Request
    GATT_CLIENT_WR_CFG_IND_REQ,
    /// Write Indication Configuration Response
    GATT_CLIENT_WR_CFG_IND_RSP,
    /// Read Notification Configuration Request
    GATT_CLIENT_RD_CFG_IND_REQ,
    /// Read Notification Configuration Response
    GATT_CLIENT_RD_CFG_IND_RSP,
    /// Indicate that a indication of the Service Changed characteristic has been received
    GATT_CLIENT_SVC_CHANGED_IND,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Parameters of the @ref GATT_CLIENT_ENABLE_REQ message
struct gatt_client_enable_req
{
    /// Connection type
    uint8_t con_type;
    /// GATT handle values and characteristic properties
    struct gatt_client_content gatt;
};

/// Parameters of the @ref ANCC_ENABLE_RSP message
struct gatt_client_enable_rsp
{
    /// Status
    uint8_t status;
    /// GATT handle values and characteristic properties
    struct gatt_client_content gatt;
};

/// Parameters of the @ref GATT_CLIENT_WR_CFG_IND_REQ message
struct gatt_client_wr_cfg_ind_req
{
    /// Indication value
    uint16_t cfg_val;
};

/// Parameters of the @ref ANCC_WR_CFG_NTF_RSP message
struct gatt_client_wr_cfg_ind_rsp
{
    /// Operation status
    uint8_t status;
};

/// Parameters of the @ref GATT_CLIENT_RD_CFG_IND_REQ message
struct gatt_client_rd_cfg_ind_req
{
    /// Unused member
    uint8_t dummy;
};

/// Parameters of the @ref GATT_CLIENT_RD_CFG_IND_RSP message
struct gatt_client_rd_cfg_ind_rsp
{
    /// Operation status
    uint8_t status;
    uint16_t cfg_val;
};

/// Parameters of the @ref GATT_CLIENT_SVC_CHANGE_IND message
struct gatt_client_svc_changed_ind
{
    /// Service changed value
    struct prf_svc val;
};

extern const struct ke_state_handler gatt_client_default_handler;
extern ke_state_t gatt_client_state[GATT_CLIENT_IDX_MAX];

#endif // (BLE_GATT_CLIENT)

/// @} GATTCLIENT

#endif // GATT_CLIENT_TASK_H_

