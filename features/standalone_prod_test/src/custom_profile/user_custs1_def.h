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

// Service 1 of the custom server 1
#define DEF_SVC1_UUID_128                	{0x59, 0x5a, 0x08, 0xe4, 0x86, 0x2a, 0x9e, 0x8f, 0xe9, 0x11, 0xbc, 0x7c, 0x98, 0x43, 0x42, 0x18}

#define DEF_SVC1_CTRL_POINT_UUID_128     	{0x20, 0xEE, 0x8D, 0x0C, 0xE1, 0xF0, 0x4A, 0x0C, 0xB3, 0x25, 0xDC, 0x53, 0x6A, 0x68, 0x86, 0x2D}

#if defined (REMOTE_MODE)
#define DEF_SVC1_RESPONSE_UUID_128      	{0x17, 0xB9, 0x67, 0x98, 0x4C, 0x66, 0x4C, 0x01, 0x96, 0x33, 0x31, 0xB1, 0x91, 0x59, 0x00, 0x15}
#endif // BATCH_REMOTE_MODE

#if defined (BATCH_REMOTE_MODE)
#define DEF_SVC1_CTRL_POINT_CHAR_LEN     	1
#else
#define DEF_SVC1_CTRL_POINT_CHAR_LEN     	7
#endif // BATCH_REMOTE_MODE

#if defined (REMOTE_MODE)
#define DEF_SVC1_RESPONSE_CHAR_LEN     		1
#endif // REMOTE_MODE

#define DEF_SVC1_CONTROL_POINT_USER_DESC	"Control Point"

#if defined (REMOTE_MODE)
#define DEF_SVC1_RESPONSE_USER_DESC			"Command Response"
#endif // REMOTE_MODE

/// Custom1 Service Data Base Characteristic enum
enum
{
    // Custom Service 1
    SVC1_IDX_SVC = 0,

    SVC1_IDX_CONTROL_POINT_CHAR,
    SVC1_IDX_CONTROL_POINT_VAL,
    SVC1_IDX_CONTROL_POINT_USER_DESC,
#if defined (REMOTE_MODE)
	SVC1_IDX_RESPONSE_CHAR,
    SVC1_IDX_RESPONSE_VAL,
    SVC1_IDX_RESPONSE_NTF_CFG,
    SVC1_IDX_RESPONSE_USER_DESC,
#endif // REMOTE_MODE
    CUSTS1_IDX_NB
};

/// @} USER_CONFIG

#endif // _USER_CUSTS1_DEF_H_
