/**
 ****************************************************************************************
 *
 * @file disc_task.h
 *
 * @brief Header file - DISCTASK.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef DISC_TASK_H_
#define DISC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup DISCTASK Device Information Service Client Task
 * @ingroup DISC
 * @brief Device Information Service Client Task
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#if (BLE_DIS_CLIENT)
#include "ke_task.h"
#include "gattc_task.h"
#include "disc.h"
#include "prf_types.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Possible states of the DISC task
enum
{
    /// Not Connected State
    DISC_FREE,
    /// Idle state
    DISC_IDLE,
    /// Discovering State
    DISC_DISCOVERING,
    /// Busy State
    DISC_BUSY,

    ///Number of defined states.
    DISC_STATE_MAX
};

enum
{
    /// Start the find me locator profile - at connection
    DISC_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_DISC),
    /// Confirm that cfg connection has finished with discovery results, or that normal cnx started
    DISC_ENABLE_RSP,

    /// Generic message to read a DIS characteristic value
    DISC_RD_CHAR_REQ,
    /// Generic message for read responses for APP
    DISC_RD_CHAR_RSP,
};

/// Parameters of the @ref DISC_ENABLE_REQ message
struct disc_enable_req
{
    ///Connection type
    uint8_t con_type;

    /// Existing handle values dis
    struct disc_dis_content dis;
};

/// Parameters of the @ref DISC_ENABLE_RSP message
struct disc_enable_rsp
{
    ///status
    uint8_t status;

    /// DIS handle values and characteristic properties
    struct disc_dis_content dis;
};

///Parameters of the @ref DISC_RD_CHAR_REQ message
struct disc_rd_char_req
{
    ///Characteristic value code
    uint8_t char_code;
};

///Parameters of the @ref DISC_RD_CHAR_RSP message
struct disc_rd_char_rsp
{
    /// Attribute data information
    struct prf_att_info info;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler disc_default_handler;

#endif //BLE_DIS_CLIENT

/// @} DISCTASK

#endif // DISC_TASK_H_
