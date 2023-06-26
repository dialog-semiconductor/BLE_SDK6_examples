/**
 ****************************************************************************************
 *
 * @file user_custs1_def.h
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
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
#define DEF_SVC1_UUID_128                       {0x59, 0x5a, 0x08, 0xe4, 0x86, 0x2a, 0x9e, 0x8f, 0xe9, 0x11, 0xbc, 0x7c, 0x98, 0x43, 0x42, 0x18}

#define DEF_SVC1_CURRENT_TIME_UUID_128          {0x17, 0xB9, 0x67, 0x98, 0x4C, 0x66, 0x4C, 0x01, 0x96, 0x33, 0x31, 0xB1, 0x91, 0x59, 0x00, 0x15}
#define DEF_SVC1_SET_CURRENT_TIME_UUID_128      {0x17, 0xB9, 0x67, 0x98, 0x4C, 0x66, 0x4C, 0x01, 0x96, 0x33, 0x31, 0xB1, 0x91, 0x59, 0x00, 0x15}
#define DEF_SVC1_ALARM_UUID_128                 {0x23, 0x68, 0xEC, 0x52, 0x1E, 0x62, 0x44, 0x74, 0x9A, 0x1B, 0xD1, 0x8B, 0xAB, 0x75, 0xB6, 0x6E}

#define DEF_SVC1_CURRENT_TIME_CHAR_LEN          11
#define DEF_SVC1_SET_CURRENT_TIME_CHAR_LEN      11
#define DEF_SVC1_ALARM_CHAR_LEN                 8

#define DEF_SVC1_CURRENT_TIME_USER_DESC         "Current Time"
#define DEF_SVC1_SET_CURRENT_TIME_USER_DESC     "Set Current Time"
#define DEF_SVC1_ALARM_USER_DESC                "Alarm"

/// Custom1 Service Data Base Characteristic enum
enum
{
    // Custom Service 1
    SVC1_IDX_SVC = 0,

    SVC1_IDX_CURRENT_TIME_CHAR,
    SVC1_IDX_CURRENT_TIME_VAL,
    SVC1_IDX_CURRENT_TIME_NTF_CFG,
    SVC1_IDX_CURRENT_TIME_USER_DESC,
    
    SVC1_IDX_SET_CURRENT_TIME_CHAR,
    SVC1_IDX_SET_CURRENT_TIME_VAL,
    SVC1_IDX_SET_CURRENT_TIME_NTF_CFG,
    SVC1_IDX_SET_CURRENT_TIME_USER_DESC,

    SVC1_IDX_ALARM_CHAR,
    SVC1_IDX_ALARM_VAL,
    SVC1_IDX_ALARM_NTF_CFG,
    SVC1_IDX_ALARM_USER_DESC,

    CUSTS1_IDX_NB
};

/// @} USER_CONFIG

#endif // _USER_CUSTS1_DEF_H_