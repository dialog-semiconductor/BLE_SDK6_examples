/**
 ****************************************************************************************
 *
 * @file user_custs1_def.c
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs included in this package ("Software") include confidential, proprietary information of Dialog Semiconductor. All Rights Reserved.
 *
 * THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR (‘DIALOG’) AND MAY ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND.  THIS SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS.  RECIPIENT SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT DIALOG’S PRIOR WRITTEN PERMISSION.
 *
 * UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED BY THE FCC OR OTHER LIKE AGENCIES.
 *
 * IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
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
static const att_svc_desc128_t custs1_svc2                      = DEF_SVC2_UUID_128;

static const uint8_t SVC1_ACCEL_X_DATA_UUID_128[ATT_UUID_128_LEN]       = DEF_SVC1_ACCEL_X_DATA_UUID_128;
static const uint8_t SVC1_ACCEL_Y_DATA_UUID_128[ATT_UUID_128_LEN]       = DEF_SVC1_ACCEL_Y_DATA_UUID_128;
static const uint8_t SVC1_ACCEL_Z_DATA_UUID_128[ATT_UUID_128_LEN]       = DEF_SVC1_ACCEL_Z_DATA_UUID_128;
static const uint8_t SVC2_G_DATA_UUID_128[ATT_UUID_128_LEN]             = DEF_SVC2_G_DATA_UUID_128;

// Attribute specifications
static const uint16_t att_decl_svc       = ATT_DECL_PRIMARY_SERVICE;
static const uint16_t att_decl_char      = ATT_DECL_CHARACTERISTIC;
static const uint16_t att_desc_cfg       = ATT_DESC_CLIENT_CHAR_CFG;
static const uint16_t att_desc_user_desc = ATT_DESC_CHAR_USER_DESCRIPTION;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

const uint8_t custs1_services[]  = {SVC1_IDX_SVC, SVC2_IDX_SVC, CUSTS1_IDX_NB};
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

    // Accelerometer X axis Characteristic Declaration
    [SVC1_IDX_ACCEL_X_DATA_CHAR]      = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            0, 0, NULL},

    // Accelerometer X axis Characteristic Value
    [SVC1_IDX_ACCEL_X_DATA_VAL]       = {SVC1_ACCEL_X_DATA_UUID_128, ATT_UUID_128_LEN, PERM(NTF, ENABLE),
                                            DEF_SVC1_ACCEL_X_DATA_CHAR_LEN, 0, NULL},

    // Accelerometer X axis Client Characteristic Configuration
    [SVC1_IDX_ACCEL_X_NTF_CFG]      = {(uint8_t*)&att_desc_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            sizeof(uint16_t), 0, NULL},

    // Accelerometer X axis Characteristic User Description
    [SVC1_IDX_ACCEL_X_DATA_USER_DESC] = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(DEF_SVC1_ACCEL_X_DATA_USER_DESC) - 1, sizeof(DEF_SVC1_ACCEL_X_DATA_USER_DESC) - 1, (uint8_t*)DEF_SVC1_ACCEL_X_DATA_USER_DESC},

    // Accelerometer Y axis Characteristic Declaration
    [SVC1_IDX_ACCEL_Y_DATA_CHAR]      = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            0, 0, NULL},

    // Accelerometer Y axis Characteristic Value
    [SVC1_IDX_ACCEL_Y_DATA_VAL]       = {SVC1_ACCEL_Y_DATA_UUID_128, ATT_UUID_128_LEN, PERM(NTF, ENABLE),
                                            DEF_SVC1_ACCEL_Y_DATA_CHAR_LEN, 0, NULL},

    // Accelerometer Y axis Client Characteristic Configuration
    [SVC1_IDX_ACCEL_Y_NTF_CFG]      = {(uint8_t*)&att_desc_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            sizeof(uint16_t), 0, NULL},

    // Accelerometer Y axis Characteristic User Description
    [SVC1_IDX_ACCEL_Y_DATA_USER_DESC] = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(DEF_SVC1_ACCEL_Y_DATA_USER_DESC) - 1, sizeof(DEF_SVC1_ACCEL_Y_DATA_USER_DESC) - 1, (uint8_t*)DEF_SVC1_ACCEL_Y_DATA_USER_DESC},

    // Accelerometer Z axis Characteristic Declaration
    [SVC1_IDX_ACCEL_Z_DATA_CHAR]      = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            0, 0, NULL},

    // Accelerometer Z axis Characteristic Value
    [SVC1_IDX_ACCEL_Z_DATA_VAL]       = {SVC1_ACCEL_Z_DATA_UUID_128, ATT_UUID_128_LEN, PERM(NTF, ENABLE),
                                            DEF_SVC1_ACCEL_Z_DATA_CHAR_LEN, 0, NULL},

    // Accelerometer Z axis Client Characteristic Configuration
    [SVC1_IDX_ACCEL_Z_NTF_CFG]      = {(uint8_t*)&att_desc_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            sizeof(uint16_t), 0, NULL},

    // Accelerometer Z axis Characteristic User Description
    [SVC1_IDX_ACCEL_Z_DATA_USER_DESC] = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(DEF_SVC1_ACCEL_Z_DATA_USER_DESC) - 1, sizeof(DEF_SVC1_ACCEL_Z_DATA_USER_DESC) - 1, (uint8_t*)DEF_SVC1_ACCEL_Z_DATA_USER_DESC},

    // Service 2 Declaration
    [SVC2_IDX_SVC]                     = {(uint8_t*)&att_decl_svc, ATT_UUID_128_LEN, PERM(RD, ENABLE),
                                            sizeof(custs1_svc2), sizeof(custs1_svc2), (uint8_t*)&custs1_svc2},

    // Accelerometer X axis Characteristic Declaration
    [SVC2_IDX_G_DATA_CHAR]      = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            0, 0, NULL},

    // Accelerometer X axis Characteristic Value
    [SVC2_IDX_G_DATA_VAL]       = {SVC2_G_DATA_UUID_128, ATT_UUID_128_LEN, PERM(NTF, ENABLE),
                                            DEF_SVC2_G_DATA_CHAR_LEN, 0, NULL},

    // Accelerometer X axis Client Characteristic Configuration
    [SVC2_IDX_G_NTF_CFG]      = {(uint8_t*)&att_desc_cfg, ATT_UUID_16_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE),
                                            sizeof(uint16_t), 0, NULL},

    // Accelerometer X axis Characteristic User Description
    [SVC2_IDX_G_DATA_USER_DESC] = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE),
                                            sizeof(DEF_SVC2_G_DATA_USER_DESC) - 1, sizeof(DEF_SVC2_G_DATA_USER_DESC) - 1, (uint8_t*)DEF_SVC2_G_DATA_USER_DESC},

};
/// @} USER_CONFIG
