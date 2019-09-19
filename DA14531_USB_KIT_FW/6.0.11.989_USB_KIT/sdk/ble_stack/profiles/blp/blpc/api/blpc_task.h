/**
 ****************************************************************************************
 *
 * @file blpc_task.h
 *
 * @brief Header file - Blood Pressure Profile Collector Role Task.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef _BLPC_TASK_H_
#define _BLPC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup BLPCTASK Blood Pressure Profile Collector Task
 * @ingroup BLPC
 * @brief Blood Pressure Profile Collector Task
 *
 * The BLPCTASK is responsible for handling the messages coming in and out of the
 * @ref BLPC monitor block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_BP_COLLECTOR)
#include "ke_task.h"
#include "gattc_task.h"
#include "co_error.h"
#include "blpc.h"
#include "prf_types.h"
#include "prf_utils.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the BLPC task
enum
{
    /// Free state
    BLPC_FREE,
    /// IDLE state
    BLPC_IDLE,
    /// Discovering Blood Pressure SVC and CHars
    BLPC_DISCOVERING,
    /// Busy state
    BLPC_BUSY,

    /// Number of defined states.
    BLPC_STATE_MAX
};

enum
{
    /// Start the blood pressure profile - at connection
    BLPC_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_BLPC),
    ///Confirm that cfg connection has finished with discovery results, or that normal cnx started
    BLPC_ENABLE_RSP,

    /// Generic message to read a BPS or DIS characteristic value
    BLPC_RD_CHAR_REQ,
    ///Generic message for read responses for APP
    BLPC_RD_CHAR_RSP,

    ///Generic message for configuring the 2 characteristics that can be handled
    BLPC_CFG_INDNTF_REQ,
    ///Generic message for write characteristic response status to APP
    BLPC_CFG_INDNTF_RSP,

    /// Blood pressure value send to APP
    BLPC_BP_MEAS_IND,
};

/// Parameters of the @ref BLPC_ENABLE_REQ message
struct blpc_enable_req
{
    ///Connection type
    uint8_t con_type;
    ///Existing handle values bps
    struct bps_content bps;
};

/// Parameters of the @ref BLPC_ENABLE_RSP message
struct blpc_enable_rsp
{
    ///status
    uint8_t status;
    ///Existing handle values bps
    struct bps_content bps;
};


///Parameters of the @ref BLPC_RD_CHAR_REQ message
struct blpc_rd_char_req
{
    ///Characteristic value code
    uint8_t  char_code;
};

///Parameters of the @ref BLPC_RD_CHAR_RSP message
struct blpc_rd_char_rsp
{
    /// Attribute data information
    struct prf_att_info info;
};

///Parameters of the @ref BLPC_CFG_INDNTF_REQ message
struct blpc_cfg_indntf_req
{
    ///Own code for differentiating between blood pressure and
    ///intermediate cuff pressure measurements
    uint8_t char_code;
    ///Stop/notify/indicate value to configure into the peer characteristic
    uint16_t cfg_val;
};

///Parameters of the @ref BLPC_CFG_INDNTF_RSP message
struct blpc_cfg_indntf_rsp
{
    ///Status
    uint8_t  status;
};

///Parameters of the @ref BLPC_BP_MEAS_IND message
struct blpc_bp_meas_ind
{
    /// Flag indicating if it is a intermediary cuff pressure measurement (0) or
    /// stable blood pressure measurement (1).
    uint16_t flag_interm_cp;
    ///Blood Pressure measurement
    struct bps_bp_meas meas_val;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler blpc_default_handler;

#endif //(BLE_BP_COLLECTOR)

/// @} BLPCTASK

#endif /* _BLPC_TASK_H_ */
