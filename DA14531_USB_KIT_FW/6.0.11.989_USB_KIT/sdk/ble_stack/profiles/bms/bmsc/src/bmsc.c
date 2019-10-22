/**
 ****************************************************************************************
 *
 * @file bmsc.c
 *
 * @brief C file - Bond Management Service Client Implementation.
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
 * @addtogroup BMSC
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_BMS_CLIENT)

#include <string.h>

#include "bmsc.h"
#include "bmsc_task.h"

static void bmsc_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason);

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the BMSC module.
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
static uint8_t bmsc_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                            uint8_t sec_lvl, void* params)
{
    uint8_t idx;

    struct bmsc_env_tag* bmsc_env =
            (struct bmsc_env_tag* ) ke_malloc(sizeof(struct bmsc_env_tag), KE_MEM_ATT_DB);
    memset(bmsc_env, 0, sizeof(struct bmsc_env_tag));

    // allocate BMSC required environment variable
    env->env = (prf_env_t*) bmsc_env;

    bmsc_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
    bmsc_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);

    // initialize environment variable
    env->id                     = TASK_ID_BMSC;
    env->desc.idx_max           = BMSC_IDX_MAX;
    env->desc.state             = bmsc_env->state;
    env->desc.default_handler   = &bmsc_default_handler;

    for(idx = 0; idx < BMSC_IDX_MAX ; idx++)
    {
        bmsc_env->env[idx] = NULL;
        // service is ready, go into the Free state
        ke_state_set(KE_BUILD_ID(env->task, idx), BMSC_FREE);
    }

    return GAP_ERR_NO_ERROR;
}

/**
 ****************************************************************************************
 * @brief Destruction of the BMSC module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Client or Service allocated environment data.
 ****************************************************************************************
 */
static void bmsc_destroy(struct prf_task_env* env)
{
    uint8_t idx;
    struct bmsc_env_tag* bmsc_env = (struct bmsc_env_tag*) env->env;

    // cleanup environment variable for each task instances
    for(idx = 0; idx < BMSC_IDX_MAX ; idx++)
    {
        if(bmsc_env->env[idx] != NULL)
        {
            bmsc_cleanup(env, idx, 0);
        }
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(bmsc_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Client or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void bmsc_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Put BMS Client in Idle state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), BMSC_IDLE);
}

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in|out]    env        Client or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void bmsc_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct bmsc_env_tag* bmsc_env = (struct bmsc_env_tag*) env->env;

    // clean-up environment variable allocated for task instance
    if(bmsc_env->env[conidx] != NULL)
    {
        ke_free(bmsc_env->env[conidx]);
        bmsc_env->env[conidx] = NULL;
    }

    /* Put BMS Client in Free state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), BMSC_FREE);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// BMSC Task interface required by profile manager
const struct prf_task_cbs bmsc_itf =
{
    (prf_init_fnct) bmsc_init,
    bmsc_destroy,
    bmsc_create,
    bmsc_cleanup,
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* bmsc_prf_itf_get(void)
{
   return &bmsc_itf;
}

void bmsc_enable_rsp_send(struct bmsc_env_tag *bmsc_env, uint8_t conidx, uint8_t status)
{
    //format response to app
    struct bmsc_enable_rsp * rsp = KE_MSG_ALLOC(BMSC_ENABLE_RSP,
                                                prf_dst_task_get(&(bmsc_env->prf_env), conidx),
                                                prf_src_task_get(&(bmsc_env->prf_env), conidx),
                                                bmsc_enable_rsp);

    rsp->status = status;

    if (status == GAP_ERR_NO_ERROR)
    {
        rsp->bms = bmsc_env->env[conidx]->bms;

        // Register BMSC task in gatt for indication/notifications
        prf_register_atthdl2gatt(&bmsc_env->prf_env, conidx, &(bmsc_env->env[conidx]->bms.svc));

        // Go to IDLE state
        ke_state_set(prf_src_task_get(&(bmsc_env->prf_env), conidx), BMSC_IDLE);
    }
    else
    {
        // clean-up environment variable allocated for task instance
        ke_free(bmsc_env->env[conidx]);
        bmsc_env->env[conidx] = NULL;
    }

    ke_msg_send(rsp);
}

uint8_t bmsc_validate_request(struct bmsc_env_tag *bmsc_env, uint8_t conidx, uint8_t char_code)
{
    uint8_t status = GAP_ERR_NO_ERROR;

    // check if feature val characteristic exists
    if (bmsc_env->env[conidx]->bms.chars[char_code].val_hdl == ATT_INVALID_HANDLE)
    {
        status = PRF_ERR_INEXISTENT_HDL;
    }

    return status;
}

void bmsc_error_ind_send(struct bmsc_env_tag *bmsc_env, uint8_t conidx, uint8_t status)
{
    struct bmsc_error_ind *ind = KE_MSG_ALLOC(BMSC_ERROR_IND,
                                              prf_dst_task_get(&(bmsc_env->prf_env), conidx),
                                              prf_src_task_get(&(bmsc_env->prf_env), conidx),
                                              bmsc_error_ind);

    // It will be a BMSC status code
    ind->status = status;

    // Send the message
    ke_msg_send(ind);
}

#endif // (BLE_BMS_CLIENT)

/// @} BMSC
