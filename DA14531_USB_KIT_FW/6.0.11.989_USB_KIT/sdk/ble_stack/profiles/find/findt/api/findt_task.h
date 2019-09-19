/**
 ****************************************************************************************
 *
 * @file findt_task.h
 *
 * @brief Header file - FINDTTASK.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef FINDT_TASK_H_
#define FINDT_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup FINDTTASK Find Me Target Task
 * @ingroup FINDT
 * @brief Find Me Target Task
 *
 * The FINDTTASK is responsible for handling the APi messages from the application or
 * internal tasks.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_FINDME_TARGET)
#include <stdint.h>
#include "ke_task.h"
#include "findt.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Possible states of the FINDT task
enum findt_state
{
    /// Idle state
    FINDT_IDLE,

    /// Number of defined states.
    FINDT_STATE_MAX
};

/// Messages for Find Me Target
enum findt_msg_id
{
    /// Alert indication
    FINDT_ALERT_IND = KE_FIRST_MSG(TASK_ID_FINDT),
};


/// Parameters of the @ref FINDT_ALERT_IND message
struct findt_alert_ind
{
    /// Alert level
    uint8_t alert_lvl;
    /// Connection index
    uint8_t conidx;
};


/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */
/// Parameters for the database creation
struct findt_db_cfg
{
    /// Database configuration
    uint8_t dummy;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler findt_default_handler;

#endif //BLE_FINDME_TARGET

/// @} FINDTTASK
#endif // FINDT_TASK_H_
