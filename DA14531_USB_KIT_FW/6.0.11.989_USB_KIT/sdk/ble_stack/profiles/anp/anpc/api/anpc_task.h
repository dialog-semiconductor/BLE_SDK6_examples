/**
 ****************************************************************************************
 *
 * @file anpc_task.h
 *
 * @brief Header file - Alert Notification Profile Client Role Task.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 * $ Rev $
 *
 ****************************************************************************************
 */

#ifndef _ANPC_TASK_H_
#define _ANPC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup ANPCTASK Alert Notification Profile Client Task
 * @ingroup ANPC
 * @brief Alert Notification Profile Client Task
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "anp_common.h"

#if (BLE_AN_CLIENT)
#include "anpc.h"
#include "ke_task.h"
#include "gattc_task.h"
#include "co_error.h"
#include "prf_types.h"
#include "prf_utils.h"


/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Possible states of the ANPC task
enum anpc_states
{
    /// Free state
    ANPC_FREE,
    /// Idle state
    ANPC_IDLE,
    /// Connected state
    ANPC_DISCOVERING,
    /// Busy state
    ANPC_BUSY,

    /// Number of defined states.
    ANPC_STATE_MAX
};

/// Message IDs
enum anpc_msg_ids
{
    /// Enable the Alert Notification Profile Client task - at connection
    ANPC_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_ANPC),
    /// Enable the Alert Notification Profile Client task - at connection
    ANPC_ENABLE_RSP,

    /// Read the value of an attribute in the peer device database
    ANPC_READ_CMD,
    /// Write the value of an attribute in the peer device database
    ANPC_WRITE_CMD,

    /// Indicate that an attribute value has been received either upon notification or read response
    ANPC_VALUE_IND,

    /// Complete Event Information
    ANPC_CMP_EVT,
};

/// Operation Codes
enum anpc_op_codes
{
    /// Reserved operation code
    ANPC_RESERVED_OP_CODE  = 0x00,

    /**
     * EXTERNAL OPERATION CODES
     */

    /// Discovery Procedure
    ANPC_ENABLE_OP_CODE,
    /// Read attribute value Procedure
    ANPC_READ_OP_CODE,
    /// Write attribute value Procedure
    ANPC_WRITE_OP_CODE,

    /**
     * INTERNAL OPERATION CODES
     */

    /// Discovery Procedure - Read Supported New Alert Category
    ANPC_ENABLE_RD_NEW_ALERT_OP_CODE,
    /// Discovery Procedure - Read Supported Unread Alert Category
    ANPC_ENABLE_RD_UNREAD_ALERT_OP_CODE,
    /// Discovery Procedure - Enable New Alert Categories for Notifications
    ANPC_ENABLE_WR_NEW_ALERT_OP_CODE,
    /// Discovery Procedure - Ask to be notified immediately for New Alert Values
    ANPC_ENABLE_WR_NTF_NEW_ALERT_OP_CODE,
    /// Discovery Procedure - Enable Unread Alert Categories for Notifications
    ANPC_ENABLE_WR_UNREAD_ALERT_OP_CODE,
    /// Discovery Procedure - Ask to be notified immediately for Unread Alert Values
    ANPC_ENABLE_WR_NTF_UNREAD_ALERT_OP_CODE,
};

/*
 * API MESSAGE STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the @ref ANPC_ENABLE_REQ message
struct anpc_enable_req
{
    /// Connection type
    uint8_t con_type;

    /// New Alert Category to Enable for Notifications
    struct anp_cat_id_bit_mask new_alert_enable;
    /// Unread Alert Category to Enable for Notifications
    struct anp_cat_id_bit_mask unread_alert_enable;

    /// Existing handle values ANS
    struct anpc_ans_content ans;
};

/// Parameters of the @ref ANPC_ENABLE_RSP message
struct anpc_enable_rsp
{
    /// status
    uint8_t status;
    /// Existing handle values ANS
    struct anpc_ans_content ans;
};

/// Parameters of the @ref ANPC_READ_CMD message
struct anpc_read_cmd
{
    /// Operation Code
    uint8_t operation;
    /// Read code
    uint8_t read_code;
};

/// Parameters of the @ref ANPC_WRITE_CMD message
struct anpc_write_cmd
{
    /// Operation Code
    uint8_t operation;
    /// Write code
    uint8_t write_code;
    /// Value
    union anpc_write_value_tag
    {
        /// Alert Notification Control Point
        struct anp_ctnl_pt ctnl_pt;
        /// New Alert Notification Configuration
        uint16_t new_alert_ntf_cfg;
        /// Unread Alert Status Notification Configuration
        uint16_t unread_alert_status_ntf_cfg;
    } value;
};

/// Parameters of the @ref ANPC_VALUE_IND message
struct anpc_value_ind
{
    /// Attribute Code
    uint8_t att_code;
    /// Value
    union anpc_value_tag
    {
        /// List of supported categories
        struct anp_cat_id_bit_mask supp_cat;
        /// New Alert
        struct anp_new_alert new_alert;
        /// Unread Alert
        struct anp_unread_alert unread_alert;
        /// Client Characteristic Configuration Descriptor Value
        uint16_t ntf_cfg;
    } value;
};

/// Parameters of the @ref ANPC_CMP_EVT message
struct anpc_cmp_evt
{
    /// Operation code
    uint8_t operation;
    /// Status
    uint8_t status;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler anpc_default_handler;

#endif //(BLE_AN_CLIENT)

/// @} ANPCTASK

#endif //(_ANPC_TASK_H_)
