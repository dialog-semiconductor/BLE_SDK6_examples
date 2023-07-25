/**
 ****************************************************************************************
 *
 * @file user_custs1_def.h
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (c) 2016-2021 Dialog Semiconductor
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
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
#define DEF_SVC1_UUID_128                {0x59, 0x5a, 0x08, 0xe4, 0x86, 0x2a, 0x9e, 0x8f, 0xe9, 0x11, 0xbc, 0x7c, 0x98, 0x43, 0x42, 0x18}

#define DEF_SVC1_CTRL_POINT_UUID_128     {0x20, 0xEE, 0x8D, 0x0C, 0xE1, 0xF0, 0x4A, 0x0C, 0xB3, 0x25, 0xDC, 0x53, 0x6A, 0x68, 0x86, 0x2D}
#define DEF_SVC1_RESET_SOURCE_UUID_128   {0x23, 0x68, 0xEC, 0x52, 0x1E, 0x62, 0x44, 0x74, 0x9A, 0x1B, 0xD1, 0x8B, 0xAB, 0x75, 0xB6, 0x6E}

#define DEF_SVC1_CTRL_POINT_CHAR_LEN     1
#define DEF_SVC1_RESET_SOURCE_CHAR_LEN   2

#define DEF_SVC1_CONTROL_POINT_USER_DESC     "Control Point"
#define DEF_SVC1_RESET_SOURCE_USER_DESC     	 "Reset detection"

/// Custom1 Service Data Base Characteristic enum
enum
{
    // Custom Service 1
    SVC1_IDX_SVC = 0,

    SVC1_IDX_CONTROL_POINT_CHAR,
    SVC1_IDX_CONTROL_POINT_VAL,
    SVC1_IDX_CONTROL_POINT_USER_DESC,
	
	SVC1_IDX_RESET_SOURCE_CHAR,
    SVC1_IDX_RESET_SOURCE_VAL,
    SVC1_IDX_RESET_SOURCE_USER_DESC,

    CUSTS1_IDX_NB
};

/// @} USER_CONFIG

#endif // _USER_CUSTS1_DEF_H_
