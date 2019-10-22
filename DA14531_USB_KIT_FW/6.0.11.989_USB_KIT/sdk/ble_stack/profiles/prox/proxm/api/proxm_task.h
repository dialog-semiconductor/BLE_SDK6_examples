/**
 ****************************************************************************************
 *
 * @file proxm_task.h
 *
 * @brief Header file - PROXMTASK.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef PROXM_TASK_H_
#define PROXM_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup PROXMTASK Proximity Monitor Task
 * @ingroup PROXM
 * @brief Proximity Monitor Task
 *
 * The Proximity Monitor Task is responsible for handling the API messages received from
 * either the Application or internal tasks.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_PROX_MONITOR)

#include "ke_task.h"
#include "co_error.h"
#include "proxm.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Possible states of the PROXM task
enum proxm_state
{
    /// FREE state
    PROXM_FREE,
    /// IDLE state
    PROXM_IDLE,
    /// Busy state
    PROXM_BUSY,
    ///Discovering
    PROXM_DISCOVERING,

    /// Number of defined states.
    PROXM_STATE_MAX
};

///Proximity Monitor API messages
enum
{
    ///Proximity Monitor role enable request from application.
    PROXM_ENABLE_REQ = KE_FIRST_MSG(TASK_ID_PROXM),
    /// Proximity Monitor role enable confirmation to application.
    PROXM_ENABLE_RSP,

    /// Read LLS Alert or TX Power Level - request
    PROXM_RD_REQ,
    /// Read LLS Alert or TX Power Level - response
    PROXM_RD_RSP,

    ///Set Alert level
    PROXM_WR_ALERT_LVL_REQ,
    ///Set Alert level response
    PROXM_WR_ALERT_LVL_RSP,
};

/*
 * API Messages Structures
 ****************************************************************************************
 */

/// Proximity monitor enable command structure
struct proxm_enable_req
{
    /// Connection type
    uint8_t con_type;

    /// Reporter LLS details kept in APP
    struct svc_content lls;
    /// Reporter IAS details kept in APP
    struct svc_content ias;
    /// Reporter TPS details kept in APP
    struct svc_content txps;
};

/// Proximity monitor enable confirm structure
struct proxm_enable_rsp
{
    /// Status
    uint8_t status;

    /// Reporter LLS details to keep in APP
    struct svc_content lls;
    /// Reporter IAS details to keep in APP
    struct svc_content ias;
    /// Reporter TPS details to keep in APP
    struct svc_content txps;
};


///Parameters of the @ref PROXM_WR_ALERT_LVL_REQ message
struct proxm_wr_alert_lvl_req
{
    /// 0=LLS or 1=IAS, code for the service in which the alert level should be written
    uint8_t  svc_code;
    /// Alert level
    uint8_t  lvl;
};

///Parameters of the @ref PROXM_WR_ALERT_LVL_RSP message
struct proxm_wr_alert_lvl_rsp
{
    /// Write characteristic response status code, may be GATT code or ATT error code.
    uint8_t  status;
};

///Parameters of the @ref PROXM_RD_REQ message
struct proxm_rd_req
{
    /// 0=LLS or 1=TXPS, code for the service in which the alert level should be read
    uint8_t  svc_code;
};

/// Parameters of the @ref PROXM_RD_RSP message
struct proxm_rd_rsp
{
    /// 0=LLS or 1=TXPS, code for the service in which the alert level should be read
    uint8_t  svc_code;
    /// Status
    uint8_t  status;
    /// Value
    uint8_t value;
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler proxm_default_handler;

#endif //BLE_PROX_MONITOR

/// @} PROXMTASK

#endif // PROXM_TASK_H_
