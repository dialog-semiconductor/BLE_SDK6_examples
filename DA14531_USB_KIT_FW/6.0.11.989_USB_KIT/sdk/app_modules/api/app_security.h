/**
 ****************************************************************************************
 *
 * @file app_security.h
 *
 * @brief Application Security Entry Point
 *
 * Copyright (C) RivieraWaves 2009-2013
 * Copyright (C) 2017 Modified by Dialog Semiconductor
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP_SECURITY
 * @{
 ****************************************************************************************
 */

#ifndef _APP_SECURITY_H_
#define _APP_SECURITY_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_APP_SEC)

#include "co_bt.h"
#include "gap.h"
#include "app.h"
#include "app_utils.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Type with wich bond database will be searched
enum bdb_search_by_type
{
    /// Search bond database by EDIV
    SEARCH_BY_EDIV_TYPE,
    /// Search bond database by Bluetooth Device Address
    SEARCH_BY_BDA_TYPE,
    /// Search bond database by Identity Resolving Key
    SEARCH_BY_IRK_TYPE,
    /// Search bond database by Identity Address
    SEARCH_BY_ID_TYPE,
    /// Search by slot
    SEARCH_BY_SLOT_TYPE,
    /// Search bond database by custom value
    SEARCH_BY_CUSTOM_TYPE,
    /// Search bond database by custom value
    NO_SEARCH_TYPE,
};

/// Remove operation type in bond database
enum bdb_remove_type
{
    /// Remove this entry
    REMOVE_THIS_ENTRY,
    /// Remove all other entries
    REMOVE_ALL_BUT_THIS_ENTRY,
    /// Remove all
    REMOVE_ALL,
};

/// Flags for valid bonding keys
enum keys_present
{
    /// No present key
    NOKEY_PRESENT   = 0,
    /// LTK present, used for link encryption
    LTK_PRESENT     = (1 << 0),
    /// Remote LTK present
    RLTK_PRESENT    = (1 << 1),
    /// Remote IRK present
    RIRK_PRESENT    = (1 << 2),
    /// Local CSRK present
    LCSRK_PRESENT   = (1 << 3),
    /// Remote CSRK present
    RCSRK_PRESENT   = (1 << 4),
};

/// Application Security Bond Data environment structure
struct app_sec_bond_data_env_tag
{
    /// Valid bonding keys
    enum keys_present valid_keys;
    /// LTK (LTK, EDIV, RANDNB, Key size)
    /// Key to use for link encryption
    struct gapc_ltk ltk;
    /// Remote LTK (LTK, EDIV, RANDNB, Key size),
    /// Key sent by initiator for future reversed-role usage
    struct gapc_ltk rltk;
    /// Remote IRK (IRK, Identity Address, Identity Address Type)
    /// Key to resolve Resolvable Private Address address of peer
    struct gapc_irk rirk;
    /// Local CSRK
    /// Key used by local device to sign data
    struct gap_sec_key lcsrk;
    /// Remote CSRK
    /// Key used by remote device to sign data
    struct gap_sec_key rcsrk;
    /// BD address of peer (BD address, BD address type)
    struct gap_bdaddr peer_bdaddr;
    /// Authentication level (BOND, MITM, SEC, KEY)
    uint8_t auth;
    /// slot in database
    uint8_t bdb_slot;
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/// Application Security Environment
extern struct app_sec_bond_data_env_tag app_sec_env[APP_EASY_MAX_ACTIVE_CONNECTION];

/*
 * FUNCTIONS DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Generate Temporary Key (pin code).
 * @return pin code
 ****************************************************************************************
 */
uint32_t app_sec_gen_tk(void);

/**
 ****************************************************************************************
 * @brief Generate Long Term Key (LTK) and store it in #app_sec_env.
 * @param[in] conidx        Connection index
 * @param[in] key_size      Key size
 * @return void
 ****************************************************************************************
 */
void app_sec_gen_ltk(uint8_t conidx, uint8_t key_size);

/**
 ****************************************************************************************
 * @brief Generate Connection Signature Resolving Key (CSRK) and store it in #app_sec_env.
 * @param[in] conidx    Connection index
 * @return void
 ****************************************************************************************
 */
void app_sec_gen_csrk(uint8_t conidx);

#endif // (BLE_APP_SEC)

/// @} APP_SECURITY

#endif // _APP_SECURITY_H_
