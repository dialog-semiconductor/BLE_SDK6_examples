/**
 ****************************************************************************************
 *
 * @file user_custs1_def.c
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (C) 2016-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup USER_CONFIG
 * @ingroup USER
 * @brief Custom server 1 (CUSTS1) profile database definitions.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "co_utils.h"
#include "prf_types.h"
#include "attm_db_128.h"
#include "user_custs1_def.h"

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */		

// DCI Service UUIDs
static const att_svc_desc128_t custs1_svc1                     			  = DCI_SVC_UUID_128;
static const uint8_t DEBUG_CRASH_INFO_RX_UUID_128[ATT_UUID_128_LEN]   = DCI_SVC_RX_UUID_128;
static const uint8_t DEBUG_CRASH_INFO_TX_UUID_128[ATT_UUID_128_LEN]   = DCI_SVC_TX_UUID_128;

// Attribute specifications
static const uint16_t att_decl_svc       = ATT_DECL_PRIMARY_SERVICE;
static const uint16_t att_decl_char      = ATT_DECL_CHARACTERISTIC;
static const uint16_t att_desc_cfg       = ATT_DESC_CLIENT_CHAR_CFG;
static const uint16_t att_desc_user_desc = ATT_DESC_CHAR_USER_DESCRIPTION;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

const uint8_t custs1_services[]  = {DEBUG_CRASH_INFO_SVC_IDX, CUSTS1_IDX_NB};
const uint8_t custs1_services_size = ARRAY_LEN(custs1_services) - 1;
const uint16_t custs1_att_max_nb = CUSTS1_IDX_NB;

/// Full CUSTS1 Database Description - Used to add attributes into the database
const struct attm_desc_128 custs1_att_db[CUSTS1_IDX_NB] =
{
    /*************************
     * Debug Crash Info Service configuration
     *************************
     */

    // Debug Crash Info Service  Declaration
    [DEBUG_CRASH_INFO_SVC_IDX]         = {(uint8_t*)&att_decl_svc, ATT_UUID_128_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_svc1), sizeof(custs1_svc1), (uint8_t*)&custs1_svc1},
		
    // RX Characteristic Declaration
    [DEBUG_CRASH_INFO_RX_CHAR]      	 = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                           0, 0, NULL},

    // RX Characteristic Value
    [DEBUG_CRASH_INFO_RX_VAL]       	 = {DEBUG_CRASH_INFO_RX_UUID_128, ATT_UUID_128_LEN, PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                          DCI_SVC_RX_CHAR_LEN, 0, NULL},

    // RX Characteristic User Description
    [DEBUG_CRASH_INFO_RX_USER_DESC] 	 = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
																					sizeof(DCI_SVC_RX_USER_DESC) - 1, sizeof(DCI_SVC_RX_USER_DESC) - 1,
																					(uint8_t *) DCI_SVC_RX_USER_DESC},

    // TX Characteristic Declaration
    [DEBUG_CRASH_INFO_TX_CHAR]         = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                           0, 0, NULL},

    // TX Characteristic Value
    [DEBUG_CRASH_INFO_TX_VAL]          = {DEBUG_CRASH_INFO_TX_UUID_128, ATT_UUID_128_LEN, PERM(NTF, ENABLE),
                                          DCI_SVC_TX_CHAR_LEN, 0, NULL},
		
		// TX Client Characteristic Configuration Descriptor
    [DEBUG_CRASH_INFO_TX_CCC]    			 = {(uint8_t*)&att_desc_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                           sizeof(uint16_t), 0, NULL},

    // TX Characteristic User Description
    [DEBUG_CRASH_INFO_TX_USER_DESC]    = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                          sizeof(DCI_SVC_TX_USER_DESC) - 1, sizeof(DCI_SVC_TX_USER_DESC) - 1,
                                          (uint8_t *) DCI_SVC_TX_USER_DESC},

};

/// @} USER_CONFIG
