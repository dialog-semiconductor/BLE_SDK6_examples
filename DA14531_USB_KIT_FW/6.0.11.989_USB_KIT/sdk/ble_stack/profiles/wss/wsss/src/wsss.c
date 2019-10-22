/**
 ****************************************************************************************
 *
 * @file wsss.c
 *
 * @brief Weight Scale Service Server implementation.
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
 * @addtogroup wsss
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_WSS_SERVER)
#include <string.h>

#include "attm.h"
#include "wsss.h"
#include "wsss_task.h"
#include "prf_utils.h"
#include "prf.h"

/*
 * WSS ATTRIBUTES
 ****************************************************************************************
 */

/// Full WSS Database Description - Used to add attributes into the database
struct attm_desc wsss_att_db[WSS_IDX_NB] =
{
    // Weight Scale Service Declaration
    [WSS_IDX_SVC]             =   {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0},
    // WSS Included Service Declaration
    [WSS_IDX_INCL_SVC]        =   {ATT_DECL_INCLUDE, PERM(RD, ENABLE), 0},
    // Weight Scale Feature Characteristic Declaration
    [WSS_IDX_FEATURE_CHAR]    =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    // Weight Scale Feature Characteristic Value
    [WSS_IDX_FEATURE_VAL]     =   {ATT_CHAR_WS_FEAT, PERM(RD, ENABLE), PERM(RI, ENABLE)},
    // Weight Measurement Characteristic Declaration
    [WSS_IDX_WT_MEAS_CHAR]    =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
    // Weight Measurement Characteristic Value
    [WSS_IDX_WT_MEAS_VAL]     =   {ATT_CHAR_WS_MEAS, PERM(IND, ENABLE), 0},
    // Weight Measurement Characteristic - Client Characteristic Configuration Descriptor
    [WSS_IDX_WT_MEAS_IND_CFG] =   {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE)|PERM(WR, ENABLE)|PERM(WRITE_REQ, ENABLE), 0},
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the WSSS module.
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
static uint8_t wsss_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                            uint8_t sec_lvl, struct wsss_db_cfg* params)
{
    // Service content flag
    uint8_t cfg_flag = ~0;
    //DB Creation Status
    uint8_t status;

    // Turn the BCS inclusion declaration off if not desired
    if (!params->include_bcs_instance)
    {
        cfg_flag &= ~(1 << WSS_IDX_INCL_SVC);
    }
    else
    {
        wsss_att_db[WSS_IDX_INCL_SVC].max_size = params->bcs_ref.start_hdl;
    }

    struct wsss_env_tag* wsss_env =
                (struct wsss_env_tag* ) ke_malloc(sizeof(struct wsss_env_tag), KE_MEM_ATT_DB);
    memset(wsss_env, 0, sizeof(struct wsss_env_tag));

    status = attm_svc_create_db(start_hdl, ATT_SVC_WEIGHT_SCALE, &cfg_flag, WSS_IDX_NB,
                wsss_env->att_tbl, env->task, &wsss_att_db[0],
                (sec_lvl & PERM_MASK_SVC_AUTH) | (sec_lvl & PERM_MASK_SVC_EKS) | PERM(SVC_PRIMARY, ENABLE) | PERM(SVC_MI, ENABLE));

    if (status == ATT_ERR_NO_ERROR)
    {
        env->env = (prf_env_t*)wsss_env;
        wsss_env->shdl = *start_hdl;
        wsss_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        wsss_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);
        wsss_env->wss_features = params->ws_feature;

        memset(wsss_env->ind_cfg, 0 , sizeof(wsss_env->ind_cfg));

        // initialize environment variable
        env->id                     = TASK_ID_WSSS;
        env->desc.idx_max           = WSSS_IDX_MAX;
        env->desc.state             = wsss_env->state;
        env->desc.state_handler     = NULL;
        env->desc.default_handler   = &wsss_default_handler;

        ke_state_set(env->task, WSSS_IDLE);
    }
    else
    {
        ke_free(wsss_env);
    }
    return status;

}

/**
 ****************************************************************************************
 * @brief Destruction of the WSSS module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 ****************************************************************************************
 */
static void wsss_destroy(struct prf_task_env* env)
{
    struct wsss_env_tag* wsss_env = (struct wsss_env_tag*) env->env;

    // free profile environment variables
    env->env = NULL;
    ke_free(wsss_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void wsss_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Nothing to do */
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
static void wsss_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    ke_state_set(KE_BUILD_ID(env->task, conidx), WSSS_IDLE);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// WSSS Task interface required by profile manager
const struct prf_task_cbs wsss_itf =
{
    (prf_init_fnct) wsss_init,
    wsss_destroy,
    wsss_create,
    wsss_cleanup,
};


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* wsss_prf_itf_get(void)
{
   return &wsss_itf;
}

void wsss_pack_meas_value(uint8_t* packed_meas,
                          const struct wss_wt_meas* pmeas_val)
{
    // Clear packet data
    memset(packed_meas, 0, WSS_MEAS_VAL_MAX_LEN);

    // Weight measurement flags
    *packed_meas++ = pmeas_val->flags;

    // Weight measurement
    if (pmeas_val->weight < 0)
        co_write16p(packed_meas, 0xFFFF);
    else
        co_write16p(packed_meas, (pmeas_val->flags & WSS_MEAS_FLAG_UNIT_IMPERIAL)
                                ? pmeas_val->weight / 0.01
                                : pmeas_val->weight / 0.005);
    packed_meas += sizeof(uint16_t);

    if (pmeas_val->flags & WSS_MEAS_FLAG_TIME_STAMP)
        // Time Stamp
        packed_meas += prf_pack_date_time(packed_meas, &pmeas_val->datetime);

    if (pmeas_val->flags & WSS_MEAS_FLAG_USERID_PRESENT)
        // User ID
        *packed_meas++ = pmeas_val->userid;

    if ((pmeas_val->flags & WSS_MEAS_FLAG_BMI_HT_PRESENT) && (pmeas_val->weight >= 0))
    {   // BMI
        co_write16p(packed_meas, pmeas_val->bmi / 0.1);
        packed_meas += sizeof(uint16_t);

        // Height
        co_write16p(packed_meas, (pmeas_val->flags & WSS_MEAS_FLAG_UNIT_IMPERIAL)
                               ? pmeas_val->height / 0.1
                               : pmeas_val->height / 0.001);
        packed_meas += sizeof(uint16_t);
    }
}

void wsss_meas_send_cfm_send(uint8_t conidx, uint8_t status)
{
    struct wsss_env_tag *wsss_env = PRF_ENV_GET(WSSS, wsss);
    // Send CFM to APP that value has been sent or not
    struct wsss_meas_send_cfm *cfm = KE_MSG_ALLOC(WSSS_MEAS_SEND_CFM,
                                                  prf_dst_task_get(&wsss_env->prf_env, conidx),
                                                  prf_src_task_get(&wsss_env->prf_env, conidx),
                                                  wsss_meas_send_cfm);

    cfm->status = status;

    // Go back to IDLE state
    ke_state_set(prf_src_task_get(&(wsss_env->prf_env), conidx), WSSS_IDLE);

    ke_msg_send(cfm);
}

uint8_t wsss_calc_meas_value_size(uint8_t flags)
{
    // Flags & Weight always present
    uint8_t size = sizeof(int8_t) + sizeof(uint16_t);

    if (flags & WSS_MEAS_FLAG_TIME_STAMP)
    {   // Time Stamp present
        struct prf_date_time datetime;
        size += sizeof(datetime.year) + sizeof(datetime.month) + sizeof(datetime.day)
              + sizeof(datetime.hour) + sizeof(datetime.min) + sizeof(datetime.sec);
    }

    if (flags & WSS_MEAS_FLAG_USERID_PRESENT)
        // User ID present
        size += sizeof(uint8_t);

    if (flags & WSS_MEAS_FLAG_BMI_HT_PRESENT)
        // BMI & Height present
        size += sizeof(uint16_t) * 2;

    return size;
}

#endif //BLE_WSS_SERVER
/// @} WSSS
