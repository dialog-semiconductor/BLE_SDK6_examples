/**
 ****************************************************************************************
 *
 * @file user_custs1_def.c
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
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
static const att_svc_desc128_t custs1_svc1                                  = DEF_SVC1_UUID_128;

static const uint8_t SVC1_REMOTE_WRITE_UUID_128[ATT_UUID_128_LEN]           = DEF_SVC1_REMOTE_WRITE_128;

// Attribute specifications
static const uint16_t att_decl_svc                                          = ATT_DECL_PRIMARY_SERVICE;
static const uint16_t att_decl_char                                         = ATT_DECL_CHARACTERISTIC;
static const uint16_t att_desc_user_desc                                    = ATT_DESC_CHAR_USER_DESCRIPTION;

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
    [SVC1_IDX_SVC]                      = {(uint8_t*)&att_decl_svc, ATT_UUID_128_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_svc1), sizeof(custs1_svc1), (uint8_t*)&custs1_svc1},

    // Remote Write Characteristic Declaration
    [SVC1_IDX_REMOTE_WRITE_CHAR]        = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            0, 0, NULL},

    // Remate Write Characteristic Value
    [SVC1_IDX_REMOTE_WRITE_VAL]         = {SVC1_REMOTE_WRITE_UUID_128, ATT_UUID_128_LEN, PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            DEF_SVC1_REMOTE_WRITE_CHAR_LEN, 0, NULL},

    // Remote Write Characteristic User Description
    [SVC1_IDX_REMOTE_WRITE_USER_DESC]   = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(DEF_SVC1_REMOTE_WRITE_DESC) - 1, sizeof(DEF_SVC1_REMOTE_WRITE_DESC) - 1,
                                            (uint8_t *) DEF_SVC1_REMOTE_WRITE_DESC},
};

/// @} USER_CONFIG