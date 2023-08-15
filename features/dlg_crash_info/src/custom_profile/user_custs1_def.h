/**
 ****************************************************************************************
 *
 * @file user_custs1_def.h
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (C) 2016-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _USER_CUSTS1_DEF_H_
#define _USER_CUSTS1_DEF_H_

/**
 ****************************************************************************************
 * @defgroup USER_CONFIG
 * @ingroup USER
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "attm_db_128.h"

/*
 * DEFINES
 ****************************************************************************************
 */
 
// TODO update comments

// Debug Crash Info Service UUID
//21ce31fc-da27-11ed-afa1-0242ac120002
#define DCI_SVC_UUID_128                 {0x02, 0x00, 0x12, 0xac, 0x42, 0x02, 0xa1, 0xaf, 0xed, 0x11, 0x27, 0xda, 0xfc, 0x31, 0xce, 0x21}

// Debug Crash Info Service Rx characteristic UUID
//8a8b791b-82f3-4ecf-9ce4-7d422c371a01
#define DCI_SVC_RX_UUID_128     			   {0x01, 0x1a, 0x37, 0x2c, 0x42, 0x7d, 0xe4, 0x9c, 0xcf, 0x4e, 0xf3, 0x82, 0x1b, 0x79, 0x8b, 0x8a}

// Debug Crash Info Service Tx characteristic UUID
//17738e00-54f9-4a2c-a6ed-1ee67e00f323
#define DCI_SVC_TX_UUID_128							 {0x23, 0xf3, 0x00, 0x7e, 0xe6, 0x1e, 0xed, 0xa6, 0x2c, 0x4a, 0xf9, 0x54, 0x00, 0x8e, 0x73, 0x17}

#define DCI_SVC_RX_CHAR_LEN     				 1
#define DCI_SVC_TX_CHAR_LEN      				 20  

#define DCI_SVC_RX_USER_DESC     				"Debug Crash Info RX"
#define DCI_SVC_TX_USER_DESC         		"Debug Crash Info TX"


/// Deubg Crash Info Service enum. Used to index into the table created in user_custs1_def.c
enum
{
    DEBUG_CRASH_INFO_SVC_IDX = 0,
		DEBUG_CRASH_INFO_SVC_START = DEBUG_CRASH_INFO_SVC_IDX,

    DEBUG_CRASH_INFO_RX_CHAR,
    DEBUG_CRASH_INFO_RX_VAL,
    DEBUG_CRASH_INFO_RX_USER_DESC,
	
		DEBUG_CRASH_INFO_TX_CHAR,
    DEBUG_CRASH_INFO_TX_VAL,
		DEBUG_CRASH_INFO_TX_CCC,
    DEBUG_CRASH_INFO_TX_USER_DESC,
	
		DEBUG_CRASH_INFO_SVC_END = DEBUG_CRASH_INFO_TX_USER_DESC,

    CUSTS1_IDX_NB
};

/// @} USER_CONFIG

#endif // _USER_CUSTS1_DEF_H_
