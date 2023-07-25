/**
 ****************************************************************************************
 *
 * @file user_custs1_def.h
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (c) 2016-2021 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
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
#define DEF_SVC1_UUID_128                			{0x2F, 0x2A, 0x93, 0xA6, 0xBD, 0xD8, 0x41, 0x52, 0xAC, 0x0B, 0x10, 0x99, 0x2E, 0xC6, 0xFE, 0xED}

#define DEF_SVC1_KBD_DATA_UUID_128      			{0x4F, 0x43, 0x31, 0x3C, 0x93, 0x92, 0x42, 0xE6, 0xA8, 0x76, 0xFA, 0x3B, 0xEF, 0xB4, 0x87, 0x5A}

#define DEF_SVC1_KBD_DATA_CHAR_LEN       			4	

#define DEF_SVC1_KBD_DATA_USER_DESC          	"Keyboard data"

/// Custom1 Service Data Base Characteristic enum
enum
{
    // Custom Service 1
    SVC1_IDX_SVC = 0,

    SVC1_IDX_KBD_DATA_STATE_CHAR,
    SVC1_IDX_KBD_DATA_STATE_VAL,
    SVC1_IDX_KBD_DATA_STATE_USER_DESC,
		SVC1_IDX_KBD_DATA_DATA_NTF_CFG,

    CUSTS1_IDX_NB,
};

/// @} USER_CONFIG

#endif // _USER_CUSTS1_DEF_H_
