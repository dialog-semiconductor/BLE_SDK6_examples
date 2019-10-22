/**
 ****************************************************************************************
 *
 * @file glpc_task.h
 *
 * @brief Header file - Glucose Profile Collector Role Task.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef _GLPC_TASK_H_
#define _GLPC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup GLPCTASK Glucose Profile Collector Task
 * @ingroup GLPC
 * @brief Glucose Profile Collector Task
 *
 * The GLPCTASK is responsible for handling the messages coming in and out of the
 * @ref GLPC monitor block of the BLE Host.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_GL_COLLECTOR)
#include "ke_task.h"
#include "gattc_task.h"
#include "co_error.h"
#include "glpc.h"
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

/// Possible states of the GLPC task
enum glpc_state
{
    /// Not Connected state
    GLPC_FREE,
    /// IDLE state
    GLPC_IDLE,
    /// Discovering Glucose SVC and Chars
    GLPC_DISCOVERING,

    /// Number of defined states.
    GLPC_STATE_MAX
};

enum glpc_msg_id
{
    /// Start the Glucose Profile - at connection
    GLPC_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_GLPC),
    /// Confirm that cfg connection has finished with discovery results, or that normal cnx started
    GLPC_ENABLE_RSP,

    /// Register to Glucose sensor notifications - request
    GLPC_REGISTER_REQ,
    /// Register to Glucose sensor notifications - confirmation
    GLPC_REGISTER_RSP,

    /// Read Glucose sensor features - request
    GLPC_READ_FEATURES_REQ,
    /// Read Glucose sensor features - response
    GLPC_READ_FEATURES_RSP,

    /// Record Access Control Point - Request
    GLPC_RACP_REQ,
    /// Record Access Control Point - Resp
    GLPC_RACP_RSP,

    /// Glucose measurement value received
    GLPC_MEAS_IND,

    /// Glucose measurement context received
    GLPC_MEAS_CTX_IND,


    /// Glucose Record Access Control Point Request Timeout
    GLPC_RACP_REQ_TIMEOUT,
};


/// Parameters of the @ref GLPC_ENABLE_REQ message
struct glpc_enable_req
{
    /// Connection type
    uint8_t con_type;
    /// Existing handle values gls
    struct gls_content gls;
};

/// Parameters of the @ref GLPC_ENABLE_RSP message
struct glpc_enable_rsp
{
    /// status
    uint8_t status;
    /// Existing handle values gls
    struct gls_content gls;
};

/// Parameters of the @ref GLPC_REGISTER_REQ message
struct glpc_register_req
{
    /// Register or not Glucose measurement context notifications
    bool meas_ctx_en;
};
/// Parameters of the @ref GLPC_REGISTER_RSP message
struct glpc_register_rsp
{
    /// Status
    uint8_t  status;
};

/// Parameters of the @ref GLPC_READ_FEATURES_RSP message
struct glpc_read_features_rsp
{
    /// Glucose sensor features
    uint16_t features;
    /// Status
    uint8_t  status;
};

/// Parameters of the @ref GLPC_RACP_REQ message
struct glpc_racp_req
{
    /// Record Access Control Point Request
    struct glp_racp_req racp_req;
};
/// Parameters of the @ref GLPC_RACP_RSP message
struct glpc_racp_rsp
{
    ///record access control response
    struct glp_racp_rsp racp_rsp;
    /// Status
    uint8_t  status;
};

/// Parameters of the @ref GLPC_MEAS_IND message
struct glpc_meas_ind
{
    /// Sequence Number
    uint16_t seq_num;
    /// Glucose measurement
    struct glp_meas meas_val;
};

/// Parameters of the @ref GLPC_MEAS_CTX_IND message
struct glpc_meas_ctx_ind
{
    /// Sequence Number
    uint16_t seq_num;
    /// Glucose measurement
    struct glp_meas_ctx ctx;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler glpc_default_handler;

#endif //(BLE_GL_COLLECTOR)

/// @} GLPCTASK

#endif /* _GLPC_TASK_H_ */
