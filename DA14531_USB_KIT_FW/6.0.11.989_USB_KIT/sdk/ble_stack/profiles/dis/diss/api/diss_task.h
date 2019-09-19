/**
 ****************************************************************************************
 *
 * @file diss_task.h
 *
 * @brief Header file - DISSTASK.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef DISS_TASK_H_
#define DISS_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup DISSTASK Task
 * @ingroup DISS
 * @brief Device Information Service Server Task
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_DIS_SERVER)
#include <stdint.h>
#include "ke_task.h"
#include "diss.h"
#include "prf_types.h"

/*
 * DEFINES
 ****************************************************************************************
 */


/// Possible states of the DISS task
enum
{
    /// Idle state
    DISS_IDLE,
    /// Busy state
    DISS_BUSY,
    /// Number of defined states.
    DISS_STATE_MAX
};

/// Messages for Device Information Service Server
enum
{
    ///Set the value of an attribute - Request
    DISS_SET_VALUE_REQ = KE_FIRST_MSG(TASK_ID_DISS),
    ///Set the value of an attribute - Response
    DISS_SET_VALUE_RSP,
    
    /// Peer device request to get profile attribute value
    DISS_VALUE_REQ_IND,
    /// Peer device confirm value of requested attribute
    DISS_VALUE_CFM,
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

/// Parameters for the database creation
struct diss_db_cfg
{
    /// Database configuration @see enum diss_features
    uint16_t features;
};


///Set the value of an attribute - Request
struct diss_set_value_req
{
    /// Value to Set
    uint8_t value;
    /// Value length
    uint8_t length;
    /// Value data
    uint8_t data[__ARRAY_EMPTY];
};

///Set the value of an attribute - Response
struct diss_set_value_rsp
{
    /// Value Set
    uint8_t value;
    /// status of the request
    uint8_t status;
};

/// Peer device request to get profile attribute value
struct diss_value_req_ind
{
    /// Requested value
    uint8_t value;
};

/// Peer device  value of requested attribute
struct diss_value_cfm
{
    /// Requested value
    uint8_t value;
    /// Value length
    uint8_t length;
    /// Value data
    uint8_t data[__ARRAY_EMPTY];
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
extern const struct ke_state_handler diss_default_handler;

#endif //BLE_DIS_SERVER

/// @} DISSTASK
#endif // DISS_TASK_H_
