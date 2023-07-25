/**
 ****************************************************************************************
 *
 * @file user_custs1_def.c
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
 * OR OTHER DEALINGS IN THE SOFTWARE.E.
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

static const uint8_t CUST1_SERVER_TX_UUID_128[ATT_UUID_128_LEN]       = DEF_CUST1_SERVER_TX_UUID_128;
static const uint8_t CUST1_SERVER_RX_UUID_128[ATT_UUID_128_LEN]        = DEF_CUST1_SERVER_RX_UUID_128;

static struct att_char128_desc custs1_server_rx_char        = {ATT_CHAR_PROP_WR_NO_RESP,
                                                              {0, 0},
                                                              DEF_CUST1_SERVER_RX_UUID_128};

static struct att_char128_desc custs1_server_tx_char        = {ATT_CHAR_PROP_NTF,
                                                              {0, 0},
                                                              DEF_CUST1_SERVER_TX_UUID_128};

// Attribute specifications
static const uint16_t att_decl_svc       = ATT_DECL_PRIMARY_SERVICE;
static const uint16_t att_decl_char      = ATT_DECL_CHARACTERISTIC;
static const uint16_t att_desc_cfg       = ATT_DESC_CLIENT_CHAR_CFG;
static const uint16_t att_desc_user_desc = ATT_DESC_CHAR_USER_DESCRIPTION;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

const uint8_t custs1_services[]  = {SVC1_IDX_SVC, CUSTS1_IDX_NB};
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

		// Server RX Characteristic Declaration
    [CUST1_IDX_SERVER_RX_CHAR]          = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_server_rx_char), sizeof(custs1_server_rx_char), (uint8_t*)&custs1_server_rx_char},

    // Server RX Characteristic Value
    [CUST1_IDX_SERVER_RX_VAL]           = {CUST1_SERVER_RX_UUID_128, ATT_UUID_128_LEN, PERM(WR, ENABLE) | PERM(WRITE_COMMAND, ENABLE),
                                            DEF_CUST1_SERVER_RX_CHAR_LEN, 0, NULL},

    // Server RX Characteristic User Description
    [CUST1_IDX_SERVER_RX_USER_DESC]     = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(CUST1_SERVER_RX_USER_DESC) - 1, sizeof(CUST1_SERVER_RX_USER_DESC) - 1, (uint8_t *)CUST1_SERVER_RX_USER_DESC},

    // Server TX Characteristic Declaration
    [CUST1_IDX_SERVER_TX_CHAR]          = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_server_tx_char), sizeof(custs1_server_tx_char), (uint8_t*)&custs1_server_tx_char},

    // Server TX Characteristic Value
    [CUST1_IDX_SERVER_TX_VAL]           = {CUST1_SERVER_TX_UUID_128, ATT_UUID_128_LEN, PERM(NTF, ENABLE),
                                            DEF_CUST1_SERVER_TX_CHAR_LEN, 0, NULL},

    // Server TX Client Characteristic Configuration Descriptor
    [CUST1_IDX_SERVER_TX_NTF_CFG]       = {(uint8_t*)&att_desc_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE) | PERM(WRITE_COMMAND, ENABLE),
                                            sizeof(uint16_t), 0, NULL},

    // Server TX Characteristic User Description
    [CUST1_IDX_SERVER_TX_USER_DESC]     = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(CUST1_SERVER_TX_USER_DESC) - 1, sizeof(CUST1_SERVER_TX_USER_DESC) - 1, (uint8_t *)CUST1_SERVER_TX_USER_DESC},
};

/// @} USER_CONFIG
