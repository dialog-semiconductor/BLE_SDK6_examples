/**
 ****************************************************************************************
 *
 * @file user_custs1_def.c
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (C) 2016-2021 Dialog Semiconductor
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

// Service 1 of the custom server 1
static const att_svc_desc128_t custs1_svc1                      = DEF_SVC1_UUID_128;

static const uint8_t SVC1_CTRL_POINT_UUID_128[ATT_UUID_128_LEN]       = DEF_SVC1_CTRL_POINT_UUID_128;
static const uint8_t SVC1_LED_STATE_UUID_128[ATT_UUID_128_LEN]        = DEF_SVC1_LED_STATE_UUID_128;
static const uint8_t SVC1_ADC_VAL_1_UUID_128[ATT_UUID_128_LEN]        = DEF_SVC1_ADC_VAL_1_UUID_128;
static const uint8_t SVC1_INDICATEABLE_UUID_128[ATT_UUID_128_LEN]     = DEF_SVC1_INDICATEABLE_UUID_128;
static const uint8_t SVC1_LONG_VALUE_UUID_128[ATT_UUID_128_LEN]       = DEF_SVC1_LONG_VALUE_UUID_128;

// Service 2 of the custom server 1
static const att_svc_desc128_t custs1_svc2                            = DEF_SVC2_UUID_128;

static const uint8_t SVC2_WRITE_VAL_1_UUID_128[ATT_UUID_128_LEN]      = DEF_SVC2_WRITE_VAL_1_UUID_128;

// Service 3 of the custom server 1
static const att_svc_desc128_t custs1_svc3                            = DEF_SVC3_UUID_128;

static const uint8_t SVC3_READ_VAL_1_UUID_128[ATT_UUID_128_LEN]       = DEF_SVC3_READ_VAL_1_UUID_128;

// Attribute specifications
static const uint16_t att_decl_svc       = ATT_DECL_PRIMARY_SERVICE;
static const uint16_t att_decl_char      = ATT_DECL_CHARACTERISTIC;
static const uint16_t att_desc_cfg       = ATT_DESC_CLIENT_CHAR_CFG;
static const uint16_t att_desc_user_desc = ATT_DESC_CHAR_USER_DESCRIPTION;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

const uint8_t custs1_services[]  = {SVC1_IDX_SVC, SVC2_IDX_SVC, SVC3_IDX_SVC, CUSTS1_IDX_NB};
const uint8_t custs1_services_size = ARRAY_LEN(custs1_services) - 1;
const uint16_t custs1_att_max_nb = CUSTS1_IDX_NB;

/// Full CUSTS1 Database Description - Used to add attributes into the database
const struct attm_desc_128 custs1_att_db[CUSTS1_IDX_NB] =
{
    /*************************
     * Service 1 configuration
     *************************
     */

    // Service 1 Declaration
    [SVC1_IDX_SVC]                     = {(uint8_t*)&att_decl_svc, ATT_UUID_128_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_svc1), sizeof(custs1_svc1), (uint8_t*)&custs1_svc1},

    // Control Point Characteristic Declaration
    [SVC1_IDX_CONTROL_POINT_CHAR]      = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            0, 0, NULL},

    // Control Point Characteristic Value
    [SVC1_IDX_CONTROL_POINT_VAL]       = {SVC1_CTRL_POINT_UUID_128, ATT_UUID_128_LEN, PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            DEF_SVC1_CTRL_POINT_CHAR_LEN, 0, NULL},

    // Control Point Characteristic User Description
    [SVC1_IDX_CONTROL_POINT_USER_DESC] = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(DEF_SVC1_CONTROL_POINT_USER_DESC) - 1, sizeof(DEF_SVC1_CONTROL_POINT_USER_DESC) - 1,
                                            (uint8_t *) DEF_SVC1_CONTROL_POINT_USER_DESC},

    // LED State Characteristic Declaration
    [SVC1_IDX_LED_STATE_CHAR]          = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            0, 0, NULL},

    // LED State Characteristic Value
    [SVC1_IDX_LED_STATE_VAL]           = {SVC1_LED_STATE_UUID_128, ATT_UUID_128_LEN, PERM(WR, ENABLE) | PERM(WRITE_COMMAND, ENABLE),
                                            PERM(RI, ENABLE) | DEF_SVC1_LED_STATE_CHAR_LEN, 0, NULL},

    // LED State Characteristic User Description
    [SVC1_IDX_LED_STATE_USER_DESC]     = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(DEF_SVC1_LED_STATE_USER_DESC) - 1, sizeof(DEF_SVC1_LED_STATE_USER_DESC) - 1,
                                            (uint8_t *) DEF_SVC1_LED_STATE_USER_DESC},

    // ADC Value 1 Characteristic Declaration
    [SVC1_IDX_ADC_VAL_1_CHAR]          = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            0, 0, NULL},

    // ADC Value 1 Characteristic Value
    [SVC1_IDX_ADC_VAL_1_VAL]           = {SVC1_ADC_VAL_1_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(NTF, ENABLE),
                                            DEF_SVC1_ADC_VAL_1_CHAR_LEN, 0, NULL},

    // ADC Value 1 Client Characteristic Configuration Descriptor
    [SVC1_IDX_ADC_VAL_1_NTF_CFG]       = {(uint8_t*)&att_desc_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            sizeof(uint16_t), 0, NULL},

    // ADC Value 1 Characteristic User Description
    [SVC1_IDX_ADC_VAL_1_USER_DESC]     = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(DEF_SVC1_ADC_VAL_1_USER_DESC) - 1, sizeof(DEF_SVC1_ADC_VAL_1_USER_DESC) - 1,
                                            (uint8_t *) DEF_SVC1_ADC_VAL_1_USER_DESC},

    // Indicateable Characteristic Declaration
    [SVC1_IDX_INDICATEABLE_CHAR]       = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            0, 0, NULL},

    // Indicateable Characteristic Value
    [SVC1_IDX_INDICATEABLE_VAL]        = {SVC1_INDICATEABLE_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(IND, ENABLE),
                                            DEF_SVC1_INDICATEABLE_CHAR_LEN, 0, NULL},

    // Indicateable Client Characteristic Configuration Descriptor
    [SVC1_IDX_INDICATEABLE_IND_CFG]    = {(uint8_t*)&att_desc_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            sizeof(uint16_t), 0, NULL},

    // Indicateable Characteristic User Description
    [SVC1_IDX_INDICATEABLE_USER_DESC]  = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(DEF_SVC1_INDICATEABLE_USER_DESC) - 1, sizeof(DEF_SVC1_INDICATEABLE_USER_DESC) - 1,
                                            (uint8_t *) DEF_SVC1_INDICATEABLE_USER_DESC},

    // Long Value Characteristic Declaration
    [SVC1_IDX_LONG_VALUE_CHAR]         = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            0, 0, NULL},

    // Long Value Characteristic Value
    [SVC1_IDX_LONG_VALUE_VAL]          = {SVC1_LONG_VALUE_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(NTF, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            DEF_SVC1_LONG_VALUE_CHAR_LEN, 0, NULL},

    // Long Value Client Characteristic Configuration Descriptor
    [SVC1_IDX_LONG_VALUE_NTF_CFG]      = {(uint8_t*)&att_desc_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            sizeof(uint16_t), 0, NULL},

    // Long Value Characteristic User Description
    [SVC1_IDX_LONG_VALUE_USER_DESC]    = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(DEF_SVC1_LONG_VALUE_CHAR_USER_DESC) - 1, sizeof(DEF_SVC1_LONG_VALUE_CHAR_USER_DESC) - 1,
                                            (uint8_t *) DEF_SVC1_LONG_VALUE_CHAR_USER_DESC},

    /*************************
     * Service 2 configuration
     *************************
     */

    // Service 2 Declaration
    [SVC2_IDX_SVC]                     = {(uint8_t*)&att_decl_svc, ATT_UUID_128_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_svc2), sizeof(custs1_svc2), (uint8_t*)&custs1_svc2},

    // Write 1 Characteristic Declaration
    [SVC2_WRITE_1_CHAR]                = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE), 0, 0, NULL},

    // Write 1 Characteristic Value
    [SVC2_WRITE_1_VAL]                 = {SVC2_WRITE_VAL_1_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            PERM(RI, ENABLE) | DEF_SVC2_WRITE_VAL_1_CHAR_LEN, 0, NULL},

    // Write 1 Characteristic User Description
    [SVC2_WRITE_1_USER_DESC]           = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(DEF_SVC2_WRITE_VAL_1_USER_DESC) - 1, sizeof(DEF_SVC2_WRITE_VAL_1_USER_DESC) - 1,
                                            (uint8_t *) DEF_SVC2_WRITE_VAL_1_USER_DESC},

    /*************************
     * Service 3 configuration
     *************************
     */

    // Service 3 Declaration
    [SVC3_IDX_SVC]                     = {(uint8_t*)&att_decl_svc, ATT_UUID_128_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_svc3), sizeof(custs1_svc3), (uint8_t*)&custs1_svc3},
    
    // Read 1 Characteristic Declaration
    [SVC3_IDX_READ_1_CHAR]             = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE), 0, 0, NULL},

    // Read 1 Characteristic Value
    [SVC3_IDX_READ_1_VAL]              = {SVC3_READ_VAL_1_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE),
                                            PERM(RI, ENABLE) | DEF_SVC3_READ_VAL_1_CHAR_LEN, 0, NULL},

    // Read 1 Characteristic User Description
    [SVC3_IDX_READ_1_USER_DESC]        = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(DEF_SVC3_READ_VAL_1_USER_DESC) - 1, sizeof(DEF_SVC3_READ_VAL_1_USER_DESC) - 1,
                                            (uint8_t *) DEF_SVC3_READ_VAL_1_USER_DESC},
};

/// @} USER_CONFIG
