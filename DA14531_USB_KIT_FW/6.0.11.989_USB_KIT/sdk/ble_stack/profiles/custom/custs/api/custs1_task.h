/**
 ****************************************************************************************
 *
 * @file custs1_task.h
 *
 * @brief Custom Server 1 (CUSTS1) profile task header file.
 *
 * Copyright (c) 2017-2019 Dialog Semiconductor. All rights reserved.
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

#ifndef _CUSTS1_TASK_H_
#define _CUSTS1_TASK_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_CUSTOM1_SERVER)

#include <stdint.h>
#include "ke_task.h"
#include "prf_types.h"
#include "compiler.h"        // compiler definition
#include "att.h"
#include "attm_db_128.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the CUSTOMS task
enum custs1_state
{
    /// Idle state
    CUSTS1_IDLE,
    /// Busy state
    CUSTS1_BUSY,
    /// Number of defined states.
    CUSTS1_STATE_MAX,
};

/// Messages for CUSTS1
enum
{
    /// Add a CUSTOMS instance into the database
    CUSTS1_CREATE_DB_REQ = KE_FIRST_MSG(TASK_ID_CUSTS1),
    /// Inform APP of database creation status
    CUSTS1_CREATE_DB_CFM,
    /// Start the Custom Service Task - at connection
    CUSTS1_ENABLE_REQ,
    /// Set/update characteristic value
    CUSTS1_VAL_SET_REQ,
    /// Peer device request to get a non-database value (RI enabled)
    CUSTS1_VALUE_REQ_IND,
    /// Response to non-database value request
    CUSTS1_VALUE_REQ_RSP,
    /// Inform profile task that the peer has subscribed/unsubscribed to notification
    CUSTS1_VAL_NTF_REQ,
    /// Response after receiving a CUSTS1_VAL_NTF_REQ message and a notification is triggered
    CUSTS1_VAL_NTF_CFM,
    /// Inform profile task that the peer has subscribed/unsubscribed to indication
    CUSTS1_VAL_IND_REQ,
    /// Response after receiving a CUSTS1_VAL_IND_REQ message and an indication is triggered
    CUSTS1_VAL_IND_CFM,
    /// Indicate that the characteristic value has been written
    CUSTS1_VAL_WRITE_IND,
    /// Inform the application that the profile service role task has been disabled after a disconnection
    CUSTS1_DISABLE_IND,
    /// Profile error report
    CUSTS1_ERROR_IND,
    /// Inform the application that there is an attribute info request that shall be processed
    CUSTS1_ATT_INFO_REQ,
    /// Inform back that the attribute info request has been processed
    CUSTS1_ATT_INFO_RSP,
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

/// Parameters for the database creation
struct custs1_db_cfg
{
    ///max number of casts1 service characteristics
    uint8_t max_nb_att;
    uint8_t *att_tbl;
    uint8_t *cfg_flag;

    ///Database configuration
    uint16_t features;
};

/// Parameters of the @ref CUSTS1_CREATE_DB_CFM message
struct custs1_create_db_cfm
{
    ///Status
    uint8_t status;
};

/// Parameters of the @ref CUSTS1_ENABLE_REQ message
struct custs1_enable_req
{
    /// Connection index
    uint8_t conidx;
    /// security level: b0= nothing, b1=unauthenticated, b2=authenticated, b3=authorized; b1 or b2 and b3 can go together
    uint8_t sec_lvl;
    /// Type of connection
    uint8_t con_type;
};

/// Parameters of the @ref CUSTS1_DISABLE_IND message
struct custs1_disable_ind
{
    /// Connection index
    uint8_t conidx;
};

/// Parameters of the @ref CUSTS1_VAL_WRITE_IND message
struct custs1_val_write_ind
{
    /// Connection index
    uint8_t  conidx;
    /// Handle of the attribute that has to be written
    uint16_t handle;
    /// Data length to be written
    uint16_t length;
    /// Data to be written in attribute database
    uint8_t  value[__ARRAY_EMPTY];
};

/// Parameters of the @ref CUSTS1_VAL_NTF_CFM message
struct custs1_val_ntf_cfm
{
    /// Connection index
//    uint8_t  conidx;
    /// Handle of the attribute that has been updated
    uint16_t handle;
    /// Confirmation status
    uint8_t status;
};

/// Parameters of the @ref CUSTS1_VAL_IND_CFM message
struct custs1_val_ind_cfm
{
    /// Connection index
    uint8_t  conidx;
    /// Handle of the attribute that has been updated
    uint16_t handle;
    /// Confirmation status
    uint8_t status;
};

/// Parameters of the @ref CUSTS1_VAL_SET_REQ message
struct custs1_val_set_req
{
    /// Connection index
    uint8_t  conidx;
    /// Handle of the attribute that has to be written
    uint16_t handle;
    /// Data length to be written
    uint16_t length;
    /// Data to be written in attribute database
    uint8_t  value[__ARRAY_EMPTY];
};

/// Parameters of the @ref CUSTS1_VAL_REQ_IND message
struct custs1_value_req_ind
{
    /// Connection index
    uint8_t  conidx;
    /// Index of the attribute for which value has been requested
    uint16_t att_idx;
};

/// Parameters of the @ref CUSTS1_VAL_REQ_RSP message
struct custs1_value_req_rsp
{
    /// Connection index
    uint8_t  conidx;
    /// Index of the attribute for which value has been requested
    uint16_t att_idx;
    /// Current length of that attribute
    uint16_t length;
    /// ATT error code
    uint8_t  status;
    /// Data value
    uint8_t  value[__ARRAY_EMPTY];
};

/// Parameters of the @ref CUSTS1_VAL_NTF_REQ message
struct custs1_val_ntf_ind_req
{
    /// Connection index
    uint8_t  conidx;
    /// Notificatin/indication
    bool     notification;
    /// Handle of the attribute that has to be written
    uint16_t handle;
    /// Data length to be written
    uint16_t length;
    /// Data to be written in attribute database
    uint8_t  value[__ARRAY_EMPTY];
};

/// Parameters of the @ref CUSTS1_VAL_IND_REQ message
struct custs1_val_ind_req
{
    /// Connection index
    uint8_t  conidx;
    /// Handle of the attribute that has to be written
    uint16_t handle;
    /// Data length to be written
    uint16_t length;
    /// Data to be written in attribute database
    uint8_t  value[__ARRAY_EMPTY];
};

/// Parameters of the @ref CUSTS1_ATT_INFO_REQ message
struct custs1_att_info_req
{
    /// Connection index
    uint8_t  conidx;
    /// Index of the attribute for which info has been requested
    uint16_t att_idx;
};

/// Parameters of the @ref CUSTS1_ATT_INFO_RSP message
struct custs1_att_info_rsp
{
    /// Connection index
    uint8_t  conidx;
    /// Index of the attribute for which info has been requested
    uint16_t att_idx;
    /// Current length of that attribute
    uint16_t length;
    /// ATT error code
    uint8_t  status;
};

/**
 ****************************************************************************************
 * @brief Initialize Client Characteristic Configuration fields.
 * @details Function initializes all CCC fields to default value.
 * @param[in] att_db         Id of the message received.
 * @param[in] max_nb_att     Pointer to the parameters of the message.
 ****************************************************************************************
 */
void custs1_init_ccc_values(const struct attm_desc_128 *att_db, int max_nb_att);

/**
 ****************************************************************************************
 * @brief Set per connection CCC value for attribute
 * @details Function sets CCC for specified connection.
 * @param[in] conidx         Connection index.
 * @param[in] att_idx        Attribute index.
 * @param[in] ccc            Value of ccc.
 ****************************************************************************************
 */
void custs1_set_ccc_value(uint8_t conidx, uint8_t att_idx, uint16_t ccc);

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
extern const struct ke_state_handler custs1_state_handler[CUSTS1_STATE_MAX];
extern const struct ke_state_handler custs1_default_handler;

#endif // BLE_CUSTOM1_SERVER
#endif // _CUSTS1_TASK_H_
