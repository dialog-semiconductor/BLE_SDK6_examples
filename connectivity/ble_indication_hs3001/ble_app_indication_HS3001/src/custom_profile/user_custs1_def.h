/**
 ****************************************************************************************
 *
 * @file user_custs1_def.h
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (c) 2016-2019 Dialog Semiconductor. All rights reserved.
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

#define DEF_CUST1_SVC_UUID_128        {0x2F, 0x2A, 0x93, 0xA6, 0xBD, 0xD8, 0x41, 0x52, 0xAC, 0x0B, 0x10, 0x99, 0x2E, 0xC6, 0xFE, 0xED } /* Displayed as EDFEC62E99100BAC5241D8BDA6932A2F */

#define DEF_CUST1_LONG_VALUE_UUID_128 {0x8C, 0x09, 0xE0, 0xD1, 0x81, 0x54, 0x42, 0x40, 0x8E, 0x4F, 0xD2, 0xB3, 0x77, 0xE3, 0x2A, 0x77 }


#define USER_IDX_TEMPERATURE_VAL_UUID_128		{0x56, 0x34, 0xE0, 0xD1, 0x81, 0x54, 0x42, 0x10, 0x8E, 0xFF, 0xD2, 0xB3, 0x77, 0xE3, 0x2A, 0x77}
#define USER_IDX_HUMIDITY_VAL_UUID_128		  {0x4F, 0x43, 0x31, 0x3C, 0x93, 0x92, 0x42, 0xE6, 0xA8, 0x76, 0xFA, 0x3B, 0xEF, 0xB4, 0x87, 0x5A}


#define USER_IDX_TEMPERATURE_VAL_CHAR_LEN 	9
#define USER_IDX_HUMIDITY_VAL_CHAR_LEN 	9

#if defined (CFG_USE_INTERNAL_TEMP_SENSOR) && (__DA14531__)
#define USER_IDX_TEMPERATURE_VAL_USER_DESC	"Internal sensor temperature data"
#else
#define USER_IDX_TEMPERATURE_VAL_USER_DESC	"HS3001 temperature data"
#define USER_IDX_HUMIDITY_VAL_USER_DESC	    "HS3001 humidity data"
#endif

//// Service 1 of the custom server 1
#define DEF_SVC1_UUID_128                {0x59, 0x5a, 0x08, 0xe4, 0x86, 0x2a, 0x9e, 0x8f, 0xe9, 0x11, 0xbc, 0x7c, 0x98, 0x43, 0x42, 0x18}


/// Custom1 Service Data Base Characteristic enum
enum
{
//    // Custom Service 1
    SVC1_IDX_SVC = 0,// Must be the first line because your characteristics must between two values in this case its between SVC1_IDX_SVC and CUSTS1_IDX_NB 
		
		USER_IDX_TEMPERATURE_VAL_CHAR,
		USER_IDX_TEMPERATURE_VAL_VAL,
		SVC1_IDX_TEMPERATURE_VAL_NTF_CFG,
		USER_IDX_TEMPERATURE_VAL_DESC,
	
	
	  // Custom Service 2
//    SVC2_IDX_SVC,
		USER_IDX_HUMIDITY_VAL_CHAR,
		USER_IDX_HUMIDITY_VAL_VAL,
		SVC1_IDX_HUMIDITY_VAL_NTF_CFG,
		USER_IDX_HUMIDITY_VAL_DESC,

    CUSTS1_IDX_NB// must be the last line of the enum because it indicates a size
	
	
};


/// @} USER_CONFIG

#endif // _USER_CUSTS1_DEF_H_
