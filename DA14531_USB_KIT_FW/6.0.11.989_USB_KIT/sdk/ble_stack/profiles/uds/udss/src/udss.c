/**
 ****************************************************************************************
 *
 * @file udss.c
 *
 * @brief C file - User Data Service Server Implementation.
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
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

/**
 ****************************************************************************************
 * @addtogroup UDSS
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_UDS_SERVER)

#include <string.h>

#include "udss.h"
#include "udss_task.h"

/*
 * UDS Attributes
 ****************************************************************************************
 */

/// Database structure deifinition
const struct attm_desc uds_att_db[UDS_IDX_NB] =
{
    [UDS_IDX_SVC] = {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0},

    [UDS_IDX_USER_FIRST_NAME_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_FIRST_NAME_VAL]  = {ATT_CHAR_UDS_USER_FIRST_NAME, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), UDS_VAL_MAX_LEN | PERM(RI, ENABLE)},

    [UDS_IDX_USER_LAST_NAME_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_LAST_NAME_VAL]  = {ATT_CHAR_UDS_USER_LAST_NAME, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), UDS_VAL_MAX_LEN | PERM(RI, ENABLE)},

    [UDS_IDX_USER_EMAIL_ADDRESS_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_EMAIL_ADDRESS_VAL]  = {ATT_CHAR_UDS_USER_EMAIL_ADDR, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), UDS_VAL_MAX_LEN | PERM(RI, ENABLE)},

    [UDS_IDX_USER_AGE_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_AGE_VAL]  = {ATT_CHAR_UDS_USER_AGE, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_DATE_OF_BIRTH_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_DATE_OF_BIRTH_VAL]  = {ATT_CHAR_UDS_USER_DATE_OF_BIRTH, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(struct uds_date) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_GENDER_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_GENDER_VAL]  = {ATT_CHAR_UDS_USER_GENDER, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_WEIGHT_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_WEIGHT_VAL]  = {ATT_CHAR_UDS_USER_WEIGHT, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint16_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_HEIGHT_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_HEIGHT_VAL]  = {ATT_CHAR_UDS_USER_HEIGHT, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint16_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_VO2_MAX_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_VO2_MAX_VAL]  = {ATT_CHAR_UDS_USER_VO2_MAX, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_HEART_RATE_MAX_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_HEART_RATE_MAX_VAL]  = {ATT_CHAR_UDS_USER_HEART_RATE_MAX, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_RESTING_HEART_RATE_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_RESTING_HEART_RATE_VAL]  = {ATT_CHAR_UDS_USER_RESTING_HEART_RATE, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_MAX_RECOMMENDED_HEART_RATE_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_MAX_RECOMMENDED_HEART_RATE_VAL]  = {ATT_CHAR_UDS_USER_MAX_REC_HEART_RATE, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_AEROBIC_THRESHOLD_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_AEROBIC_THRESHOLD_VAL]  = {ATT_CHAR_UDS_USER_AEROBIC_THRESHOLD, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_ANAEROBIC_THRESHOLD_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_ANAEROBIC_THRESHOLD_VAL]  = {ATT_CHAR_UDS_USER_ANAEROBIC_THRESHOLD, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_SPORT_TYPE_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_SPORT_TYPE_VAL]  = {ATT_CHAR_UDS_USER_THRESHOLDS_SPORT_TYPE, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_DATE_OF_THRESHOLD_ASSESSMENT_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_DATE_OF_THRESHOLD_ASSESSMENT_VAL]  = {ATT_CHAR_UDS_USER_DATE_OF_THRESHOLD_ASS, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint32_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_WAIST_CIRCUMFERENCE_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_WAIST_CIRCUMFERENCE_VAL]  = {ATT_CHAR_UDS_USER_WAIST_CIRCUMFERENCE, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint16_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_HIP_CIRCUMFERENCE_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_HIP_CIRCUMFERENCE_VAL]  = {ATT_CHAR_UDS_USER_HIP_CIRCUMFERENCE, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint16_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_FAT_BURN_HEART_RATE_LOW_LIMIT_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_FAT_BURN_HEART_RATE_LOW_LIMIT_VAL]  = {ATT_CHAR_UDS_USER_FAT_BURN_HR_LOW_LIM, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_FAT_BURN_HEART_RATE_UP_LIMIT_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_FAT_BURN_HEART_RATE_UP_LIMIT_VAL]  = {ATT_CHAR_UDS_USER_FAT_BURN_HR_UP_LIM, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_AEROBIC_HEART_RATE_LOW_LIMIT_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_AEROBIC_HEART_RATE_LOW_LIMIT_VAL]  = {ATT_CHAR_UDS_USER_AEROBIC_HR_LOW_LIM, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_AEROBIC_HEART_RATE_UP_LIMIT_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_AEROBIC_HEART_RATE_UP_LIMIT_VAL]  = {ATT_CHAR_UDS_USER_AEROBIC_HR_UP_LIM, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_ANAEROBIC_HEART_RATE_LOW_LIMIT_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_ANAEROBIC_HEART_RATE_LOW_LIMIT_VAL]  = {ATT_CHAR_UDS_USER_ANAEROBIC_HR_LOW_LIM, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_ANAEROBIC_HEART_RATE_UP_LIMIT_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_ANAEROBIC_HEART_RATE_UP_LIMIT_VAL]  = {ATT_CHAR_UDS_USER_ANAEROBIC_HR_UP_LIM, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_FIVE_ZONE_HEART_RATE_LIMITS_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_FIVE_ZONE_HEART_RATE_LIMITS_VAL]  = {ATT_CHAR_UDS_USER_5ZONE_HR_LIM, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint32_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_THREE_ZONE_HEART_RATE_LIMITS_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_THREE_ZONE_HEART_RATE_LIMITS_VAL]  = {ATT_CHAR_UDS_USER_3ZONE_HR_LIM, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint16_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_TWO_ZONE_HEART_RATE_LIMIT_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_TWO_ZONE_HEART_RATE_LIMIT_VAL]  = {ATT_CHAR_UDS_USER_2ZONE_HR_LIM, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_LANGUAGE_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_LANGUAGE_VAL]  = {ATT_CHAR_UDS_USER_LANGUAGE, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint16_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_DB_CHANGE_INCR_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_DB_CHANGE_INCR_VAL]  = {ATT_CHAR_UDS_USER_DB_CHANGE_INCR, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), sizeof(uint32_t) | PERM(RI, ENABLE)},
    [UDS_IDX_USER_DB_CHANGE_INCR_CFG]  = {ATT_DESC_CLIENT_CHAR_CFG,  PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), PERM(RI, ENABLE)},

    [UDS_IDX_USER_INDEX_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_INDEX_VAL]  = {ATT_CHAR_UDS_USER_INDEX, PERM(RD, ENABLE), sizeof(uint8_t) | PERM(RI, ENABLE)},

    [UDS_IDX_USER_CNTL_POINT_CHAR] = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    [UDS_IDX_USER_CNTL_POINT_VAL]  = {ATT_CHAR_UDS_USER_CTRL_PT, PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE) | PERM(IND, ENABLE), 19},
    [UDS_IDX_USER_CNTL_POINT_CFG]  = {ATT_DESC_CLIENT_CHAR_CFG,  PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), PERM(RI, ENABLE)}
};

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the UDSS module.
 * This function performs all the initializations of the Profile module.
 *  - Creation of database (if it's a service)
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    env        Client or Service allocated environment data.
 * @param[in|out] start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     app_task   Application task number.
 * @param[in]     sec_lvl    Security level (AUTH, EKS and MI field of @enum attm_svc_perm_mask)
 * @param[in]     param      Configuration parameters of profile collector or service (32 bits aligned)
 *
 * @return status code to know if profile initialization succeed or not.
 ****************************************************************************************
 */
static uint8_t udss_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                         uint8_t sec_lvl, struct udss_db_cfg* params)
{
    uint8_t status = ATT_ERR_NO_ERROR;
    uint64_t enabled_chars_mask = 1;
    uint8_t db_cfg_mask[9]; // We need 65 bits for all attributes
    uint8_t i = 0;

    // Mask for all attributes - start with 0x01 for service declaration
    memset(db_cfg_mask, 0, 9);
    db_cfg_mask[0] = 0x01;

    // Check params->fearures and prepare optional attributes configuration mask
    for (i = UDS_IDX_USER_FIRST_NAME_CHAR; i < UDS_IDX_USER_DB_CHANGE_INCR_CHAR; i += 2)
    {
        if ((params->enabled_chars & enabled_chars_mask) == enabled_chars_mask)
        {
            // Unmask Char Declaration Attribute
            db_cfg_mask[i / 8] |= (0x01 << (i % 8));
            // Unmask Char Value Attribute
            db_cfg_mask[(i + 1) / 8] |= (0x01 << ((i + 1) % 8));
        }

        enabled_chars_mask = enabled_chars_mask << 1;
    }

    // Unmask optional CCC descriptor
    if (i == UDS_IDX_USER_DB_CHANGE_INCR_CFG)
    {
        if (params->local_char_update_enabled)
        {
            db_cfg_mask[i / 8] |= (0x01 << (i % 8));
        }

        i++;
    }

    // Unmask Mandatory Attributes
    for (; i < UDS_IDX_NB; ++i)
    {
        db_cfg_mask[i / 8] |= (0x01 << (i % 8));
    }

    // Allocate UDSS required environment variable
    struct udss_env_tag* udss_env =
            (struct udss_env_tag*) ke_malloc(sizeof(struct udss_env_tag), KE_MEM_ENV);
    memset(udss_env, 0, sizeof(struct udss_env_tag));

    status = attm_svc_create_db(start_hdl, ATT_SVC_USER_DATA,
                                (uint8_t *)&db_cfg_mask, UDS_IDX_NB,
                                udss_env->att_tbl, env->task, &uds_att_db[0],
                                (sec_lvl & PERM_MASK_SVC_AUTH) | \
                                (sec_lvl & PERM_MASK_SVC_EKS) | \
                                PERM(SVC_PRIMARY, ENABLE) | PERM(SVC_MI, DISABLE));

    // Set optional notification attribute permissions
    if (params->local_char_update_enabled)
    {
        att_perm_type perm = uds_att_db[UDS_IDX_USER_DB_CHANGE_INCR_VAL].perm | PERM(NTF, ENABLE);

        attmdb_att_set_permission(udss_env->att_tbl[UDS_IDX_USER_DB_CHANGE_INCR_VAL], perm);
    }

    if (status == ATT_ERR_NO_ERROR)
    {
        udss_env->shdl              = *start_hdl;
        udss_env->enabled_chars     = params->enabled_chars;
        udss_env->prf_env.app_task  = app_task
                    | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        udss_env->prf_env.prf_task  = env->task | PERM(PRF_MI, ENABLE);
        memset(udss_env->consent, 0, UDS_IDX_MAX);

        // Initialize environment variable
        env->id                     = TASK_ID_UDSS;
        env->desc.idx_max           = UDS_IDX_MAX;
        env->desc.state             = udss_env->state;
        env->desc.state_max         = UDSS_STATE_MAX;
        env->desc.state_handler     = NULL;
        env->desc.default_handler   = &udss_default_handler;
        env->env = (prf_env_t*) udss_env;

        // Set start handle to first allocated service value
        *start_hdl = udss_env->shdl;

        ke_state_set(env->task, UDSS_IDLE);
    }
    else
    {
        ke_free(udss_env);
    }

    return status;
}

/**
 ****************************************************************************************
 * @brief Destruction of the UDSS module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Client or Service allocated environment data.
 ****************************************************************************************
 */
static void udss_destroy(struct prf_task_env* env)
{
    struct udss_env_tag* udss_env = (struct udss_env_tag*) env->env;

    // Free profile environment variables
    env->env = NULL;
    ke_free(udss_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void udss_create(struct prf_task_env* env, uint8_t conidx)
{
    struct udss_env_tag* udss_env = (struct udss_env_tag*) env->env;
    ASSERT_ERROR(conidx < BLE_CONNECTION_MAX);

    // Set initial CCC descriptor states
    udss_env->db_change_ccc_val[conidx] = GATT_CCC_STOP_NTFIND;
    udss_env->ucp_ccc_val[conidx] = GATT_CCC_STOP_NTFIND;

    // Set initial consent procedure result
    udss_env->consent[conidx] = false;

    ke_state_set(KE_BUILD_ID(env->task, conidx), UDSS_IDLE);
}

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void udss_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    // Do nothing
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// UDSS Task interface required by profile manager
const struct prf_task_cbs udss_itf =
{
    (prf_init_fnct) udss_init,
    udss_destroy,
    udss_create,
    udss_cleanup,
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* udss_prf_itf_get(void)
{
   return &udss_itf;
}

uint8_t udss_unpack_ucp_req(const uint8_t *packed_val, uint16_t length,
                            struct uds_ucp_req* ucp_req)
{
    uint8_t cursor = 0;

    // Verify that enough data present
    if(length < 1)
    {
        return PRF_APP_ERROR;
    }

    // Retrieve function op code
    ucp_req->op_code = packed_val[cursor];
    cursor++;

    memset(&(ucp_req->parameter), 0, sizeof(ucp_req->parameter));

    if((ucp_req->op_code < UDS_REQ_REG_NEW_USER)
            || (ucp_req->op_code > UDS_REQ_DEL_USER_DATA))
    {
        return UDS_RSP_OP_CODE_NOT_SUP;
    }

    if(ucp_req->op_code == UDS_REQ_DEL_USER_DATA)
    {
        return ATT_ERR_NO_ERROR;
    }

    if(ucp_req->op_code == UDS_REQ_REG_NEW_USER)
    {
        // Check sufficient data available
        if((length - cursor) < 2)
        {
            return UDS_RSP_INVALID_PARAMETER;
        }

        // Retrieve consent code
        ucp_req->parameter.reg_new_user.consent_code = co_read16p(packed_val + cursor);
        cursor +=2;

        if(ucp_req->parameter.reg_new_user.consent_code > UDS_CONSENT_CODE_MAX_VAL)
        {
            return UDS_RSP_INVALID_PARAMETER;
        }
    }

    if(ucp_req->op_code == UDS_REQ_CONSENT)
    {
        // Check sufficient data available
        if((length - cursor) < 3)
        {
            return UDS_RSP_INVALID_PARAMETER;
        }

        // Retrieve user index
        ucp_req->parameter.consent.user_idx = *(packed_val + cursor);
        cursor++;

        // Retrieve consent code
        ucp_req->parameter.consent.consent_code = co_read16p(packed_val + cursor);
        cursor +=2;

        if(ucp_req->parameter.consent.consent_code > UDS_CONSENT_CODE_MAX_VAL)
        {
            return UDS_RSP_INVALID_PARAMETER;
        }
    }

    return ATT_ERR_NO_ERROR;
}

uint8_t udss_pack_ucp_rsp(uint8_t *packed_val, const struct uds_ucp_rsp* ucp_rsp)
{
    uint8_t cursor = 0;

    // Set response op code
    packed_val[cursor] = ucp_rsp->op_code;
    cursor++;

    // Set request op code
    packed_val[cursor] = ucp_rsp->req_op_code;
    cursor++;

    // Set response value
    packed_val[cursor] = ucp_rsp->rsp_val;
    cursor++;

    // Fill in parameter with user index for register new user function
    if((ucp_rsp->req_op_code == UDS_REQ_REG_NEW_USER)
       &&(ucp_rsp->rsp_val == UDS_RSP_SUCCESS))
    {
        *(packed_val+cursor) = ucp_rsp->parameter.reg_new_user.user_idx;
        cursor ++;
    }

    return cursor;
}

void udss_send_ucp_rsp(const struct uds_ucp_rsp *ucp_rsp, uint8_t conidx)
{
    struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);

    struct gattc_send_evt_cmd *evt = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                                                      KE_BUILD_ID(TASK_GATTC, conidx),
                                                      prf_src_task_get(&(udss_env->prf_env), conidx),
                                                      gattc_send_evt_cmd, UDS_UCP_NTF_MAX_LEN);

    evt->operation = GATTC_INDICATE;
    evt->handle = udss_env->att_tbl[UDS_IDX_USER_CNTL_POINT_VAL];
    evt->length = udss_pack_ucp_rsp(evt->value, ucp_rsp);

    ke_msg_send(evt);
}

#endif // (BLE_UDS_SERVER)

/// @} UDSS
