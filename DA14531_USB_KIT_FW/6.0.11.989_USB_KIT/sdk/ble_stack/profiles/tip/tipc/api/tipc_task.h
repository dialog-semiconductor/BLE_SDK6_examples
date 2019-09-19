/**
 ****************************************************************************************
 *
 * @file tipc_task.h
 *
 * @brief Header file - Time Profile Client Role Task.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef _TIPC_TASK_H_
#define _TIPC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup TIPCTASK Time Profile Client Task
 * @ingroup TIPC
 * @brief Time Profile Client Task
 *
 * The TIPCTASK is responsible for handling the messages coming in and out of the
 * @ref TIPC monitor block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "tip_common.h"

#if (BLE_TIP_CLIENT)
#include "tipc.h"
#include "ke_task.h"
#include "gattc_task.h"
#include "co_error.h"
#include "prf_types.h"
#include "prf_utils.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the TIPC task
enum
{
    /// Free state
    TIPC_FREE,
    /// IDLE state
    TIPC_IDLE,
    /// Discovering Blood Pressure SVC and CHars
    TIPC_DISCOVERING,
    /// Busy state
    TIPC_BUSY,

    /// Number of defined states.
    TIPC_STATE_MAX
};

enum
{
    /// Start the time profile - at connection
    TIPC_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_TIPC),
    ///Confirm that cfg connection has finished with discovery results, or that normal cnx started
    TIPC_ENABLE_RSP,

    /// Generic message to read a CTS, NDCS or RTUS characteristic value
    TIPC_RD_CHAR_REQ,
    /// Received read value
    TIPC_RD_CHAR_RSP,

    ///Generic message for configuring the Current Time Characteristic on the Server
    TIPC_CT_NTF_CFG_REQ,
    ///Current Time Characteristic configuration response
    TIPC_CT_NTF_CFG_RSP,

    /// Generic message for writing the Time Update Control Point Characteristic Value on a peer device
    TIPC_WR_TIME_UPD_CTNL_PT_REQ,
    ///Generic message for writing characteristic response status to APP
    TIPC_WR_TIME_UPD_CTNL_PT_RSP,

    /// Received Current Time value (Notification)
    TIPC_CT_IND,
};


/// Parameters of the @ref TIPC_ENABLE_REQ message
struct tipc_enable_req
{
    ///Connection type
    uint8_t con_type;

    ///Existing handle values cts
    struct tipc_cts_content cts;
    ///Existing handle values ndcs
    struct tipc_ndcs_content ndcs;
    ///Existing handle values rtus
    struct tipc_rtus_content rtus;
};

/// Parameters of the @ref TIPC_ENABLE_RSP message
struct tipc_enable_rsp
{
    ///status
    uint8_t status;

    ///Existing handle values cts
    struct tipc_cts_content cts;
    ///Existing handle values ndcs
    struct tipc_ndcs_content ndcs;
    ///Existing handle values rtus
    struct tipc_rtus_content rtus;
};

///Parameters of the @ref TIPC_CT_NTF_CFG_REQ message
struct tipc_ct_ntf_cfg_req
{
    ///Stop/notify/indicate value to configure into the peer characteristic
    uint16_t cfg_val;
};

///Parameters of the @ref TIPC_CT_NTF_CFG_RSP message
struct tipc_ct_ntf_cfg_rsp
{
    ///Status
    uint8_t status;
};

///Parameters of the @ref TIPC_RD_CHAR_REQ message
struct tipc_rd_char_req
{
    ///Characteristic value code
    uint8_t  char_code;
};

///Parameters of the @ref TIPC_RD_CHAR_RSP message
struct tipc_rd_char_rsp
{
    /// Operation code
    uint8_t op_code;
    /// Status
    uint8_t status;

    union tipc_rd_value_tag
    {
        ///Notification Configuration
        uint16_t ntf_cfg;
        ///Current Time
        struct tip_curr_time curr_time;
        ///Local Time Information
        struct tip_loc_time_info loc_time_info;
        ///Reference Time Information
        struct tip_ref_time_info ref_time_info;
        ///Time With DST
        struct tip_time_with_dst time_with_dst;
        ///Time Update State
        struct tip_time_upd_state time_upd_state;
    } value;
};

///Parameters of the @ref TIPC_WR_TIME_UPD_CTNL_PT_REQ message
struct tipc_wr_time_udp_ctnl_pt_req
{
    ///Value
    uint8_t value;
};

///Parameters of the @ref TIPC_WR_TIME_UPD_CTNL_PT_RSP message
struct tipc_wr_time_upd_ctnl_pt_rsp
{
    ///Status
    uint8_t  status;
};

///Parameters of the @ref TIPC_CT_IND message
struct tipc_ct_ind
{
    ///Current Time Value
    struct tip_curr_time ct_val;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler tipc_default_handler;

#endif //(BLE_TIP_CLIENT)

/// @} TIPCTASK

#endif /* _TIPC_TASK_H_ */
