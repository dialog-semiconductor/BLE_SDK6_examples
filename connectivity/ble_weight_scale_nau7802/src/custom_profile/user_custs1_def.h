/**
 ****************************************************************************************
 *
 * @file user_custs1_def.h
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
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

// Service 1 of the custom server 1: Zero the scale
#define DEF_SVC1_UUID_128                	{0x59, 0x5a, 0x08, 0xe4, 0x86, 0x2a, 0x9e, 0x8f, 0xe9, 0x11, 0xbc, 0x7c, 0x98, 0x43, 0x42, 0x18}

#define DEF_SVC1_ZERO_UUID_128      			{0x21, 0x95, 0x71, 0x60, 0x40, 0x63, 0x79, 0x45, 0xC8, 0x43, 0x99, 0x0B, 0x60, 0x7D, 0x0D, 0x10}
#define DEF_SVC1_CALIB_UUID_128      			{0x22, 0xC6, 0x75, 0x3c, 0x66, 0x25, 0x8d, 0xf3, 0x1d, 0xaa, 0xd9, 0xe8, 0xec, 0x3f, 0x43, 0x2c}

#define DEF_SVC1_ZERO_CHAR_LEN     	 			1
#define DEF_SVC1_CALIB_CHAR_LEN      			4

#define DEF_SVC1_ZERO_USER_DESC         	"Zero"
#define DEF_SVC1_CALIB_USER_DESC         	"Calibrate"


/// Custom1 Service Data Base Characteristic enum
enum
{
    // Custom Service 1
    SVC1_IDX_SVC = 0,

    SVC1_IDX_ZERO_CHAR,
    SVC1_IDX_ZERO_VAL,
    SVC1_IDX_ZERO_USER_DESC,

    SVC1_IDX_CALIB_CHAR,
    SVC1_IDX_CALIB_VAL,
    SVC1_IDX_CALIB_USER_DESC,

    CUSTS1_IDX_NB
};

/// @} USER_CONFIG

#endif // _USER_CUSTS1_DEF_H_