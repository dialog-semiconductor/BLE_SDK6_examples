/**
 ****************************************************************************************
 *
 * @file paspc.h
 *
 * @brief Header file - Phone Alert Status Profile - Client Role.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 * $ Rev $
 *
 ****************************************************************************************
 */

#ifndef _PASPC_H_
#define _PASPC_H_

/**
 ****************************************************************************************
 * @addtogroup PASPC Phone Alert Status Profile Client
 * @ingroup PASP
 * @brief Phone Alert Status Profile Client
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "pasp_common.h"

#if (BLE_PAS_CLIENT)

#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of Phone Alert Status Client task instances
#define PASPC_IDX_MAX        (BLE_CONNECTION_MAX)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Phone Alert Status Service Characteristics
enum
{
    /// Alert Status
    PASPC_CHAR_ALERT_STATUS,
    /// Ringer Setting
    PASPC_CHAR_RINGER_SETTING,
    /// Ringer Control Point
    PASPC_CHAR_RINGER_CTNL_PT,

    PASPC_CHAR_MAX,
};

/// Phone Alert Status Service Characteristic Descriptors
enum
{
    /// Alert Status Char. - Client Characteristic Configuration
    PASPC_DESC_ALERT_STATUS_CL_CFG,
    /// Ringer Setting Char. - Client Characteristic Configuration
    PASPC_DESC_RINGER_SETTING_CL_CFG,

    PASPC_DESC_MAX,

    PASPC_DESC_MASK = 0x10,
};

/// Internal codes for reading/writing a PAS characteristic with one single request
enum
{
    /// Read PAS Alert Status
    PASPC_RD_ALERT_STATUS              = PASPC_CHAR_ALERT_STATUS,
    /// Read PAS Ringer Setting
    PASPC_RD_RINGER_SETTING            = PASPC_CHAR_RINGER_SETTING,
    /// Write PAS Ringer Control Point
    PASPC_WR_RINGER_CTNL_PT            = PASPC_CHAR_RINGER_CTNL_PT,

    /// Read/Write PAS Alert Status Client Characteristic Configuration Descriptor
    PASPC_RD_WR_ALERT_STATUS_CFG       = (PASPC_DESC_ALERT_STATUS_CL_CFG | PASPC_DESC_MASK),
    /// Read PAS Ringer Setting Client Characteristic Configuration Descriptor
    PASPC_RD_WR_RINGER_SETTING_CFG     = (PASPC_DESC_RINGER_SETTING_CL_CFG | PASPC_DESC_MASK),
};



/*
 * STRUCTURES
 ****************************************************************************************
 */

/**
 * Structure containing the characteristics handles, value handles and descriptors for
 * the Phone Alert Status Service
 */
struct paspc_pass_content
{
    /// Service info
    struct prf_svc svc;

    /// Characteristic info:
    ///  - Alert Status
    ///  - Ringer Setting
    ///  - Ringer Control Point
    struct prf_char_inf chars[PASPC_CHAR_MAX];

    /// Descriptor handles:
    ///  - Alert Status Client Cfg
    ///  - Ringer Setting Client Cfg
    struct prf_char_desc_inf descs[PASPC_DESC_MAX];
};

struct paspc_cnx_env
{
    /// Last requested UUID(to keep track of the two services and char)
    uint16_t last_uuid_req;
    /// Last characteristic code used in a read or a write request
    uint16_t last_char_code;
    /// Counter used to check service uniqueness
    uint8_t nb_svc;
    /// Current operation code
    uint8_t operation;
    /// Phone Alert Status Service Characteristics
    struct paspc_pass_content pass;
};

/// Phone Alert Status Profile Client environment variable
struct paspc_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct paspc_cnx_env* env[PASPC_IDX_MAX];
    /// State of different task instances
    ke_state_t state[PASPC_IDX_MAX];
};

/*
 * GLOBAL FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve PASP client profile interface
 * @return PASP client profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* paspc_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Send Phone Alert Status ATT DB discovery results to PASPC host.
 * @param[in] paspc_env environment variable
 * @param[in] conidx Connection index
 * @param[in] status Satus
 ****************************************************************************************
 */
void paspc_enable_rsp_send(struct paspc_env_tag *paspc_env, uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send a PASPC_CMP_EVT message to the task which enabled the profile
 ****************************************************************************************
 */
void paspc_send_cmp_evt(struct paspc_env_tag *paspc_env, uint8_t conidx,
                        uint8_t operation, uint8_t status);

#endif //(BLE_PAS_CLIENT)

/// @} PASPC

#endif //(_PASPC_H_)
