/**
 ****************************************************************************************
 *
 * @file wssc.c
 *
 * @brief C file - Weight Scale Service Client Implementation.
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
 * @addtogroup WSSC
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_WSS_COLLECTOR)

#include <string.h>

#include "wssc.h"
#include "wssc_task.h"

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the WSSC module.
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
static uint8_t wssc_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                          uint8_t sec_lvl, void* params)
{
    uint8_t idx;

    struct wssc_env_tag* wssc_env =
            (struct wssc_env_tag* ) ke_malloc(sizeof(struct wssc_env_tag), KE_MEM_ATT_DB);
    memset(wssc_env, 0, sizeof(struct wssc_env_tag));

    // allocate WSSC required environment variable
    env->env = (prf_env_t*) wssc_env;

    wssc_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
    wssc_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);

    // initialize environment variable
    env->id                     = TASK_ID_WSSC;
    env->desc.idx_max           = WSSC_IDX_MAX;
    env->desc.state             = wssc_env->state;
    env->desc.state_handler     = NULL;
    env->desc.default_handler   = &wssc_default_handler;

    for(idx = 0; idx < WSSC_IDX_MAX ; idx++)
    {
        wssc_env->env[idx] = NULL;
        // service is ready, go into the Free state
        ke_state_set(KE_BUILD_ID(env->task, idx), WSSC_FREE);
    }

    return GAP_ERR_NO_ERROR;
}

/**
 ****************************************************************************************
 * @brief Clean-up connection dedicated environment parameters
 * This function performs cleanup of ongoing operations
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void wssc_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct wssc_env_tag* wssc_env = (struct wssc_env_tag*) env->env;

    // clean-up environment variable allocated for task instance
    if(wssc_env->env[conidx] != NULL)
    {
        ke_free(wssc_env->env[conidx]);
        wssc_env->env[conidx] = NULL;
    }

    /* Put WSS Client in Free state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), WSSC_FREE);
}

/**
 ****************************************************************************************
 * @brief Destruction of the WSSC module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Client or Service allocated environment data.
 ****************************************************************************************
 */
static void wssc_destroy(struct prf_task_env* env)
{
    uint8_t idx;
    struct wssc_env_tag* wssc_env = (struct wssc_env_tag*) env->env;

    // cleanup environment variable for each task instances
    for(idx = 0; idx < WSSC_IDX_MAX ; idx++)
    {
        if(wssc_env->env[idx] != NULL)
        {
            wssc_cleanup(env, idx, 0);
        }
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(wssc_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void wssc_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Put WSS Client in Idle state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), WSSC_IDLE);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// WSSC Task interface required by profile manager
const struct prf_task_cbs wssc_itf =
{
    (prf_init_fnct) wssc_init,
    wssc_destroy,
    wssc_create,
    wssc_cleanup,
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* wssc_prf_itf_get(void)
{
   return &wssc_itf;
}

void wssc_enable_rsp_send(struct wssc_env_tag *wssc_env, uint8_t conidx, uint8_t status)
{
    struct wssc_enable_rsp * rsp = KE_MSG_ALLOC(WSSC_ENABLE_RSP,
                                                prf_dst_task_get(&(wssc_env->prf_env), conidx),
                                                prf_src_task_get(&(wssc_env->prf_env), conidx),
                                                wssc_enable_rsp);

    rsp->status = status;

    if (status == GAP_ERR_NO_ERROR)
    {
        rsp->wss = wssc_env->env[conidx]->wss;

        // Register WSSC task in gatt for indication/notifications
        prf_register_atthdl2gatt(&wssc_env->prf_env, conidx, &(wssc_env->env[conidx]->wss.svc));

        // Go to IDLE state
        ke_state_set(prf_src_task_get(&(wssc_env->prf_env), conidx), WSSC_IDLE);
    }
    else
    {
        if(wssc_env->env[conidx] != NULL)
        {
            // clean-up environment variable allocated for task instance
            ke_free(wssc_env->env[conidx]);
            wssc_env->env[conidx] = NULL;
        }
    }

    ke_msg_send(rsp);
}

void wssc_unpack_meas_value(struct wssc_env_tag *wssc_env, uint8_t conidx, uint8_t *packed_data, uint8_t lenght)
{
    uint8_t cursor = 0;

    if(lenght >= WSSC_PACKED_MEAS_MIN_LEN)
    {
        struct wssc_ws_ind *ind = KE_MSG_ALLOC(WSSC_WS_IND,
                                               prf_dst_task_get(&(wssc_env->prf_env), conidx),
                                               prf_src_task_get(&(wssc_env->prf_env), conidx),
                                               wssc_ws_ind);

        // Unpack flags
        ind->meas_val.flags = *(packed_data + cursor);
        cursor += 1;

        // Unpack weight value
        ind->meas_val.weight = co_read16p(packed_data + cursor) *
                               ((ind->meas_val.flags & WSS_MEAS_FLAG_UNIT_IMPERIAL) ? 0.01 : 0.005);

        cursor += 2;

        // Unpack Time Stampt if present
        if(ind->meas_val.flags & WSS_MEAS_FLAG_TIME_STAMP)
        {
            cursor += prf_unpack_date_time(packed_data + cursor, &(ind->meas_val.datetime));
        }

        // Unpack user ID if present
        if(ind->meas_val.flags & WSS_MEAS_FLAG_USERID_PRESENT)
        {
            ind->meas_val.userid = *(packed_data + cursor);
            cursor += 1;
        }
        // Unpack BMI and height if present
        if(ind->meas_val.flags & WSS_MEAS_FLAG_BMI_HT_PRESENT)
        {   // BMI
            ind->meas_val.bmi = co_read16p(packed_data + cursor) * 0.1;
            cursor += 2;

            // Height
            ind->meas_val.height = co_read16p(packed_data + cursor) *
                                   ((ind->meas_val.flags & WSS_MEAS_FLAG_UNIT_IMPERIAL) ? 0.1 : 0.001);
        }

        ke_msg_send(ind);

    }
}

void wssc_err_ind_send(struct wssc_env_tag *wssc_env, uint8_t conidx, uint8_t status)
{
    struct wssc_error_ind *ind = KE_MSG_ALLOC(WSSC_ERROR_IND,
                                              prf_dst_task_get(&(wssc_env->prf_env), conidx),
                                              prf_src_task_get(&(wssc_env->prf_env), conidx),
                                              wssc_error_ind);

    ind->status = status;

    // Send the message
    ke_msg_send(ind);
}

#endif // (BLE_WSS_COLLECTOR)

/// @} WSSC
