/**
 ****************************************************************************************
 *
 * @file user_custs1_def.h
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (c) 2016-2018 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
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
#define DEF_SVC1_UUID_128                			    	{0x3b, 0x0f, 0x28, 0xa6, 0x46, 0xa5, 0x4d, 0xff, 0x9a, 0x74, 0x28, 0xdc, 0xe8, 0x78, 0x26, 0x50}
																							    
#define DEF_SVC1_CAPACITANCE_VALUE_UUID_128         {0xe0, 0x41, 0x5e, 0xe2, 0x8d, 0xa7, 0x4b, 0xd2, 0x90, 0x9c, 0x78, 0xba, 0xfe, 0x31, 0x3d, 0x5b}
																							    
#define DEF_SVC1_CAPACITANCE_VALUE_CHAR_LEN         50

#define DEF_SVC1_CAPACITANCE_VALUE_CHAR_USER_DESC   "Capacitance Value"


/// Custom1 Service Data Base Characteristic enum
enum
{
    // Custom Service 1
    SVC1_IDX_SVC = 0,


    SVC1_IDX_CAPACITANCE_VALUE_CHAR,
    SVC1_IDX_CAPACITANCE_VALUE_VAL,
    SVC1_IDX_CAPACITANCE_VALUE_NTF_CFG,
    SVC1_IDX_CAPACITANCE_VALUE_USER_DESC,

    CUSTS1_IDX_NB
};

/// @} USER_CONFIG

#endif // _USER_CUSTS1_DEF_H_
