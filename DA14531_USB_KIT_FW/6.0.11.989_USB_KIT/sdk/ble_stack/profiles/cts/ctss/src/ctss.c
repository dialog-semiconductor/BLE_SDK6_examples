/**
 ****************************************************************************************
 *
 * @file ctss.c
 *
 * @brief Current Time Service Server Implementation.
 *
 * Copyright (c) 2017-2018 Dialog Semiconductor. All rights reserved.
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
 * @addtogroup CTSS
 * @{
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_CTS_SERVER)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <string.h>

#include "cts_common.h"
#include "ctss_task.h"
#include "ctss.h"

#include "prf_utils.h"

/*
 * CTS ATTRIBUTES DEFINITION
 ****************************************************************************************
 */

/// Current Time Service
const att_svc_desc_t ctss_svc                           = ATT_SVC_CURRENT_TIME;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Current Time Service Server environment variable
struct ctss_env_tag ctss_env __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the CTSS module.
 * This function performs all the initializations of the Profile module.
 *  - Creation of database (if it's a service)
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    env        Collector or Service allocated environment data.
 * @param[in|out] start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     app_task   Application task number.
 * @param[in]     sec_lvl    Security level (AUTH, EKS and MI field of @enum attm_svc_perm_mask)
 * @param[in]     param      Configuration parameters of profile collector or service (32 bits aligned)
 *
 * @return status code to know if profile initialization succeed or not.
 ****************************************************************************************
 */
static uint8_t ctss_init(struct prf_task_env *env, uint16_t *start_hdl, uint16_t app_task, uint8_t sec_lvl, struct ctss_db_cfg *params)
{
    //------------------ create the attribute database for the profile -------------------

    // Service content flag
    uint8_t cfg_flag;
    // DB Creation Status
    uint8_t status = ATT_ERR_NO_ERROR;

    /// CTS Database Description - Used to add attributes into the database
    const struct attm_desc att_db[CTSS_IDX_NB] =
    {
        // Current Time Service Declaration
        [CTSS_IDX_SVC]                              = {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0},

        // Current Time Characteristic Declaration
        [CTSS_IDX_CURRENT_TIME_CHAR]                = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), CTSS_IDX_CURRENT_TIME_VAL},
        // Current Time Characteristic Value
        [CTSS_IDX_CURRENT_TIME_VAL]                 = {ATT_CHAR_CT_TIME, PERM(RD, ENABLE) | PERM(NTF, ENABLE) |
                                                        ((params->features & CTSS_CURRENT_TIME_WRITE_SUP) ? PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE) : 0),
                                                        PERM(RI, ENABLE) | CTSS_CURRENT_TIME_VAL_LEN},
        // Current Time Characteristic - Client Char. Configuration Descriptor
        [CTSS_IDX_CURRENT_TIME_CFG]                 = {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), 0},

        // Local Time Information Characteristic Declaration
        [CTSS_IDX_LOCAL_TIME_INFO_CHAR]             = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), CTSS_IDX_LOCAL_TIME_INFO_VAL},
        // Local Time Information Characteristic Value
        [CTSS_IDX_LOCAL_TIME_INFO_VAL]              = {ATT_CHAR_LOCAL_TIME_INFO, PERM(RD, ENABLE) |
                                                        ((params->features & CTSS_LOC_TIME_WRITE_SUP) ? PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE) : 0),
                                                        PERM(RI, ENABLE) | sizeof(struct cts_loc_time_info)},

        // Reference Time Information Characteristic Declaration
        [CTSS_IDX_REF_TIME_INFO_CHAR]               = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), CTSS_IDX_REF_TIME_INFO_VAL},
        // Reference Time Info Characteristic Value
        [CTSS_IDX_REF_TIME_INFO_VAL]                = {ATT_CHAR_REFERENCE_TIME_INFO, PERM(RD, ENABLE), PERM(RI, ENABLE) | sizeof(struct cts_ref_time_info)},
    };
    struct ctss_env_tag *ctss_env =
        (struct ctss_env_tag *)ke_malloc(sizeof(struct ctss_env_tag), KE_MEM_ATT_DB);
    memset(ctss_env, 0, sizeof(struct ctss_env_tag));

    // Service, current time characteristics
    cfg_flag = CTSS_CURRENT_TIME_MASK;
    // Add Local Time Info if requested
    if (params->features & CTSS_LOC_TIME_INFO_SUP)
        cfg_flag |= CTSS_LOC_TIME_INFO_MASK;
    // Add Ref Time Info if requested
    if (params->features & CTSS_REF_TIME_INFO_SUP)
        cfg_flag |= CTSS_REF_TIME_INFO_MASK;

    status = attm_svc_create_db(start_hdl, ATT_SVC_CURRENT_TIME, &cfg_flag,
            CTSS_IDX_NB, ctss_env->att_tbl, env->task, &att_db[0],
            (sec_lvl & PERM_MASK_SVC_AUTH) | (sec_lvl & PERM_MASK_SVC_EKS) | PERM(SVC_PRIMARY, ENABLE));

    //-------------------- allocate memory required for the profile  ---------------------
    if (status == ATT_ERR_NO_ERROR)
    {
        // allocate CTSS required environment variable
        env->env = (prf_env_t *) ctss_env;
        ctss_env->start_hdl = *start_hdl;
        ctss_env->features  = params->features;
        ctss_env->prf_env.app_task = app_task
                | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        ctss_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id                     = TASK_ID_CTSS;
        env->desc.idx_max           = CTSS_IDX_MAX;
        env->desc.state             = &ctss_env->state;
        env->desc.state_handler     = NULL;
        env->desc.default_handler   = &ctss_default_handler;

        // service is ready, go into an Idle state
        ke_state_set(env->task, CTSS_IDLE);
    }
    else
    {
        ke_free(ctss_env);
    }

    return status;
}

uint8_t ctss_get_att_idx(const struct ctss_env_tag *ctss_env, uint16_t handle)
{
    uint8_t att_ix = 0;

    for (att_ix = 0; att_ix < CTSS_IDX_NB; ++att_ix)
    {
        if (ctss_env->att_tbl[att_ix] == (uint8_t) handle)
            return att_ix;
    }

    return 0;
}

uint16_t ctss_get_att_handle(const struct ctss_env_tag *ctss_env, uint8_t att_idx)
{
    return ctss_env->att_tbl[att_idx];
}

/**
 ****************************************************************************************
 * @brief  Trigger time notification
 *
 * @param ctss_env profile environment
 * @param conidx   peer destination connection index
 * @param svc_idx  Service index
 ****************************************************************************************
 */
static void ctss_notify_time(struct ctss_env_tag *ctss_env, uint8_t conidx)
{
    // Allocate the GATT notification message
    struct gattc_send_evt_cmd *cur_time = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
            KE_BUILD_ID(TASK_GATTC, conidx), prf_src_task_get(&(ctss_env->prf_env),0),
            gattc_send_evt_cmd, sizeof(struct cts_curr_time));

    // Fill in the parameter structure
    cur_time->operation = GATTC_NOTIFY;
    cur_time->handle = ctss_get_att_handle(ctss_env, CTSS_IDX_CURRENT_TIME_VAL);
    // pack measured value in database
    cur_time->length = sizeof(ctss_env->pckd_time);
    memcpy(cur_time->value, ctss_env->pckd_time, sizeof(ctss_env->pckd_time));

    // send notification to peer device
    ke_msg_send(cur_time);
}

void ctss_exe_operation(void)
{
    struct ctss_env_tag *ctss_env = PRF_ENV_GET(CTSS, ctss);
    ASSERT_ERROR(ctss_env->operation != NULL);
    bool quit_busy_state = true;

    // Restoring connection information requested
    if (ctss_env->operation->id == CTSS_NTF_CURR_TIME_REQ)
    {
        struct ctss_upd_curr_time_req *req = (struct ctss_upd_curr_time_req *) ke_msg2param(ctss_env->operation);
        // loop on all connections
        while (ctss_env->cursor < BLE_CONNECTION_MAX)
        {
            if (ctss_env->ntf_cfg[ctss_env->cursor])
            {
                // trigger notification
                ctss_notify_time(ctss_env, ctss_env->cursor);

                quit_busy_state = false;
                ctss_env->cursor++;
                break;
            }
            ctss_env->cursor++;
        }
    }
    // default, should not happen
    else
    {
        ASSERT_ERROR(0);
    }

    // check if operation is finished
    if (quit_busy_state)
    {
        // trigger response message
        if (ctss_env->operation->id == CTSS_NTF_CURR_TIME_REQ)
        {
            struct ctss_ntf_curr_time_rsp *rsp = KE_MSG_ALLOC(CTSS_NTF_CURR_TIME_CFM,
                ctss_env->operation->src_id,
                ctss_env->operation->dest_id, ctss_ntf_curr_time_rsp);

            rsp->status = GAP_ERR_NO_ERROR;
            ke_msg_send(rsp);
        }

        // free operation
        ke_free(ctss_env->operation);
        ctss_env->operation = NULL;
        // go back to idle state
        ke_state_set(prf_src_task_get(&(ctss_env->prf_env), 0), CTSS_IDLE);
    }
}

void ctss_pack_curr_time_value(const struct cts_curr_time *p_curr_time_val, uint8_t *p_pckd_time)
{
    // Date-Time
    prf_pack_date_time(p_pckd_time, &(p_curr_time_val->exact_time_256.day_date_time.date_time));

    // Day of Week
    *(p_pckd_time + 7) = p_curr_time_val->exact_time_256.day_date_time.day_of_week;

    // Fraction 256
    *(p_pckd_time + 8) = p_curr_time_val->exact_time_256.fraction_256;

    // Adjust Reason
    *(p_pckd_time + 9) = p_curr_time_val->adjust_reason;
}

void ctss_unpack_curr_time_value(const uint8_t *p_pckd_time, struct cts_curr_time *p_curr_time_val)
{
    // Date-Time
    p_pckd_time += prf_unpack_date_time((uint8_t *) p_pckd_time, &p_curr_time_val->exact_time_256.day_date_time.date_time);

    // Day of Week
    p_curr_time_val->exact_time_256.day_date_time.day_of_week = *p_pckd_time++;

    // Fraction 256
    p_curr_time_val->exact_time_256.fraction_256 = *p_pckd_time++;

    // Adjust Reason
    p_curr_time_val->adjust_reason = *p_pckd_time++;
}

/**
 ****************************************************************************************
 * @brief Destruction of the CTSS module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 ****************************************************************************************
 */
static void ctss_destroy(struct prf_task_env *env)
{
    struct ctss_env_tag *ctss_env = (struct ctss_env_tag *)env->env;

    // clear on-going operation
    if (ctss_env->operation != NULL)
    {
        ke_free(ctss_env->operation);
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(ctss_env);
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
static void ctss_cleanup(struct prf_task_env *env, uint8_t conidx, uint8_t reason)
{
    struct ctss_env_tag *ctss_env = (struct ctss_env_tag *)env->env;

    ASSERT_ERROR(conidx < BLE_CONNECTION_MAX);
    // force notification config to zero when peer device is disconnected
    ctss_env->ntf_cfg[conidx] = 0;
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void ctss_create(struct prf_task_env *env, uint8_t conidx)
{
    struct ctss_env_tag *ctss_env = (struct ctss_env_tag *) env->env;
    ASSERT_ERROR(conidx < BLE_CONNECTION_MAX);

    // force notification config to zero when peer device is connected
    ctss_env->ntf_cfg[conidx] = 0;
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// CTSS Task interface required by profile manager
const struct prf_task_cbs ctss_itf =
{
        (prf_init_fnct) ctss_init,
        ctss_destroy,
        ctss_create,
        ctss_cleanup,
};

/*
 * GLOBAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs *ctss_prf_itf_get(void)
{
   return &ctss_itf;
}

#endif // (BLE_CTS_SERVER)

/// @} CTSS
