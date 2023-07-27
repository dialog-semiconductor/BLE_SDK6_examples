/**
 ****************************************************************************************
 *
 * @file user_custs1_def.c
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (c) 2016-2023 Renesas Electronics Corporation and/or its affiliates
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
 *****************************************************
 ***********************************
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

// Service 1 of the custom server 1
static const att_svc_desc128_t custs1_svc1	= DEF_SVC1_UUID_128;



// Attribute specifications
static const uint16_t att_decl_svc       		= ATT_DECL_PRIMARY_SERVICE;
static const uint16_t att_decl_char      		= ATT_DECL_CHARACTERISTIC;
static const uint16_t att_desc_cfg       		= ATT_DESC_CLIENT_CHAR_CFG;
static const uint16_t att_desc_user_desc 		= ATT_DESC_CHAR_USER_DESCRIPTION;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
static const uint8_t USER_TEMPERATURE_VAL_UUID_128[ATT_UUID_128_LEN] = USER_IDX_TEMPERATURE_VAL_UUID_128;
static const uint8_t USER_HUMIDITY_VAL_UUID_128[ATT_UUID_128_LEN] = USER_IDX_HUMIDITY_VAL_UUID_128;


const uint8_t custs1_services[]  		= {SVC1_IDX_SVC,CUSTS1_IDX_NB};
const uint8_t custs1_services_size 	= ARRAY_LEN(custs1_services) - 1;
const uint16_t custs1_att_max_nb 		= CUSTS1_IDX_NB;


/* Full CUSTS1 Database Description - Used to add attributes into the database.
we are managing to have two services in one profile*/


const struct attm_desc_128 custs1_att_db[CUSTS1_IDX_NB] =
{
	
	
	
/*
************************
  Service 1 configuration
*************************/

    // Service 1 Declaration
    [SVC1_IDX_SVC]                     			 = {(uint8_t*)&att_decl_svc, ATT_UUID_128_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_svc1), sizeof(custs1_svc1), (uint8_t*)&custs1_svc1},

		// Temperature Value Characteristic Declaration
    [USER_IDX_TEMPERATURE_VAL_CHAR]          = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            0, 0, NULL},

    // Temperature Characteristic Value
    [USER_IDX_TEMPERATURE_VAL_VAL]           = {USER_TEMPERATURE_VAL_UUID_128, ATT_UUID_128_LEN, PERM(IND, ENABLE),
                                                USER_IDX_TEMPERATURE_VAL_CHAR_LEN, 0, NULL},

    // Temperature Client Characteristic Configuration Descriptor
    [SVC1_IDX_TEMPERATURE_VAL_IND_CFG]       = {(uint8_t*)&att_desc_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            sizeof(uint16_t), 0, NULL},

    // Temperature Characteristic User Description
    [USER_IDX_TEMPERATURE_VAL_DESC]     = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(USER_IDX_TEMPERATURE_VAL_USER_DESC) - 1, sizeof(USER_IDX_TEMPERATURE_VAL_USER_DESC) - 1,
                                            (uint8_t *) USER_IDX_TEMPERATURE_VAL_USER_DESC},
		
																						
																						
/*
************************
  Service 2 configuration
*************************/

		// humidity Value Characteristic Declaration
    [USER_IDX_HUMIDITY_VAL_CHAR]          = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            0, 0, NULL},

    // humidity Characteristic Value
    [USER_IDX_HUMIDITY_VAL_VAL]           = {USER_HUMIDITY_VAL_UUID_128, ATT_UUID_128_LEN, PERM(IND, ENABLE),
                                            USER_IDX_HUMIDITY_VAL_CHAR_LEN, 0, NULL},

    // humidity Client Characteristic Configuration Descriptor
    [SVC1_IDX_HUMIDITY_VAL_IND_CFG]       = {(uint8_t*)&att_desc_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            sizeof(uint16_t), 0, NULL},

    // humidity Characteristic User Description
    [USER_IDX_HUMIDITY_VAL_DESC]     = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(USER_IDX_HUMIDITY_VAL_USER_DESC) - 1, sizeof(USER_IDX_HUMIDITY_VAL_USER_DESC) - 1,
                                            (uint8_t *) USER_IDX_HUMIDITY_VAL_USER_DESC},																				
																						
																						

};

/// @} USER_CONFIG
