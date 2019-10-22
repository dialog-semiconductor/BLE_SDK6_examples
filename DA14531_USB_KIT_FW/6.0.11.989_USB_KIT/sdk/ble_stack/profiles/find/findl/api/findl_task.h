/**
 ****************************************************************************************
 *
 * @file findl_task.h
 *
 * @brief Header file - FINDLTASK.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef FINDL_TASK_H_
#define FINDL_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup FINDLTASK Find Me Locator Task
 * @ingroup FINDL
 * @brief Find Me Locator Task
 *
 * The FINDLTASK is responsible for handling the API messages coming from the application
 * or internal tasks.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#if (BLE_FINDME_LOCATOR)
#include "ke_task.h"
#include "co_error.h"
#include "findl.h"
#include "prf_types.h"
#include "prf_utils.h"

/*
 * DEFINES
 ****************************************************************************************
 */

///Maximum number of Find ME Locator task instances
#define FINDL_IDX_MAX    (BLE_CONNECTION_MAX)

/// Possible states of the FINDL task
enum findl_state
{
    /// Not Connected State
    FINDL_FREE,
    /// Idle state
    FINDL_IDLE,
    /// Discovering state
    FINDL_DISCOVERING,
    /// Busy State
    FINDL_BUSY,

    /// Number of defined states.
    FINDL_STATE_MAX
};

///Find Me Locator API messages
enum findl_msg_id
{
    /// Start the find me locator profile - at connection
    FINDL_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_FINDL),
    /// Enable confirm message, containing IAS attribute details if discovery connection type
    FINDL_ENABLE_RSP,

    /// Alert level set request
    FINDL_SET_ALERT_REQ,
    /// Alert level set response
    FINDL_SET_ALERT_RSP,
};

/// Alert levels
enum
{
    /// No alert
    FINDL_ALERT_NONE    = 0x00,
    /// Mild alert
    FINDL_ALERT_MILD,
    /// High alert
    FINDL_ALERT_HIGH
};

/// Parameters of the @ref FINDL_ENABLE_REQ message
struct findl_enable_req
{
    ///Connection type
    uint8_t con_type;
    ///Discovered IAS details if any
    struct ias_content ias;
};

/// Parameters of the @ref FINDL_ENABLE_RSP message
struct findl_enable_rsp
{
    ///Status
    uint8_t status;
    ///Discovered IAS details if any
    struct ias_content ias;
};

/// Parameters of the @ref FINDL_SET_ALERT_REQ message
struct findl_set_alert_req
{
    /// Alert level
    uint8_t  alert_lvl;
};

///Parameters of the @ref FINDL_SET_ALERT_RSP message
struct findl_set_alert_rsp
{
    /// Status
    uint8_t  status;
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler findl_default_handler;

#endif //BLE_FINDME_LOCATOR

/// @} FINDLTASK

#endif // FINDL_TASK_H_
