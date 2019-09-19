/**
 ****************************************************************************************
 *
 * @file gatt_client.c
 *
 * @brief C file - Generic Attribute Profile Service Client Implementation.
 *
 * Copyright (c) 2018 Dialog Semiconductor. All rights reserved.
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
 * @addtogroup GATT_CLIENT
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_GATT_CLIENT)

#include <string.h>

#include "gatt_client.h"
#include "gatt_client_task.h"

/**
 ****************************************************************************************
 * @brief Initialization of the GATT Client module.
 * This function performs the initialization of the Profile module:
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application:
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    env        Client or Service allocated environment data.
 * @param[in,out] start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     app_task   Application task number.
 * @param[in]     sec_lvl    Security level (AUTH, EKS and MI field of @see enum attm_value_perm_mask)
 * @param[in]     param      Configuration parameters of profile collector or service (32-bit aligned)
 *
 * @return status code to know if profile initialization succeeds
 ****************************************************************************************
 */
static uint8_t gatt_client_init(struct prf_task_env *env, uint16_t *start_hdl, uint16_t app_task,
                                uint8_t sec_lvl, void *params)
{
    uint8_t idx;

    struct gatt_client_env_tag *gattc_env =
            (struct gatt_client_env_tag *) ke_malloc(sizeof(struct gatt_client_env_tag), KE_MEM_ATT_DB);
    memset(gattc_env, 0, sizeof(struct gatt_client_env_tag));

    // Allocate GATT client required environment variable
    env->env = (prf_env_t*) gattc_env;

    gattc_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
    gattc_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);

    // Initialize environment variable
    env->id                     = TASK_ID_GATT_CLIENT;
    env->desc.idx_max           = GATT_CLIENT_IDX_MAX;
    env->desc.state             = gattc_env->state;
    env->desc.state_max         = GATT_CLIENT_STATE_MAX;
    env->desc.state_handler     = NULL;
    env->desc.default_handler   = &gatt_client_default_handler;

    for (idx = 0; idx < GATT_CLIENT_IDX_MAX ; idx++)
    {
        gattc_env->env[idx] = NULL;
        // Service is ready, go into the Free state
        ke_state_set(KE_BUILD_ID(env->task, idx), GATT_CLIENT_FREE);
    }

    return GAP_ERR_NO_ERROR;
}

/**
 ****************************************************************************************
 * @brief Clean-up of connection specific environment parameters
 * This function performs clean-up of ongoing operations
 * @param[in,out]    env        Client or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 * @return void
 ****************************************************************************************
 */
static void gatt_client_cleanup(struct prf_task_env *env, uint8_t conidx, uint8_t reason)
{
    struct gatt_client_env_tag *gattc_env = (struct gatt_client_env_tag *) env->env;

    // Clean up environment variable allocated for task instance
    if (gattc_env->env[conidx] != NULL)
    {
        ke_free(gattc_env->env[conidx]);
        gattc_env->env[conidx] = NULL;
    }

    // Put GATT Client in Free state
    ke_state_set(KE_BUILD_ID(env->task, conidx), GATT_CLIENT_FREE);
}

/**
 ****************************************************************************************
 * @brief Destruction of the GATT client module - due to a reset for instance.
 * This function cleans up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in,out]    env        Client or Service allocated environment data.
 * @return void
 ****************************************************************************************
 */
static void gatt_client_destroy(struct prf_task_env *env)
{
    uint8_t idx;
    struct gatt_client_env_tag *gattc_env = (struct gatt_client_env_tag *) env->env;

    // Cleanup environment variable for each task instances
    for (idx = 0; idx < GATT_CLIENT_IDX_MAX ; idx++)
    {
        if (gattc_env->env[idx] != NULL)
        {
            gatt_client_cleanup(env, idx, 0);
        }
    }

    // Free profile environment variables
    env->env = NULL;
    ke_free(gattc_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in,out]    env        Task allocated environment data.
 * @param[in]        conidx     Connection index
 * @return void
 ****************************************************************************************
 */
static void gatt_client_create(struct prf_task_env *env, uint8_t conidx)
{
    // Put GATT Client in Idle state
    ke_state_set(KE_BUILD_ID(env->task, conidx), GATT_CLIENT_IDLE);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// GATT Client Task interface required by profile manager
const struct prf_task_cbs gatt_client_itf =
{
    (prf_init_fnct)gatt_client_init,
    gatt_client_destroy,
    gatt_client_create,
    gatt_client_cleanup,
};

/*
 * GLOBAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs *gatt_client_prf_itf_get(void)
{
   return &gatt_client_itf;
}

void gatt_client_enable_rsp_send(struct gatt_client_env_tag *gatt_env,
                                 uint8_t conidx,
                                 uint8_t status)
{
    // Send APP the details of the discovered attributes on GATT
    struct gatt_client_enable_rsp *rsp = KE_MSG_ALLOC(GATT_CLIENT_ENABLE_RSP,
                                               prf_dst_task_get(&(gatt_env->prf_env), conidx),
                                               prf_src_task_get(&(gatt_env->prf_env), conidx),
                                               gatt_client_enable_rsp);

    rsp->status = status;

    if (status == GAP_ERR_NO_ERROR)
    {
        rsp->gatt = gatt_env->env[conidx]->gatt;
        // Register the GATT Client task in GATT-level indications/notifications
        prf_register_atthdl2gatt(&gatt_env->prf_env, conidx, &gatt_env->env[conidx]->gatt.svc);
        // Go to connected state
        ke_state_set(prf_src_task_get(&(gatt_env->prf_env), conidx), GATT_CLIENT_IDLE);
    }
    ke_msg_send(rsp);
}

void gatt_client_rd_cfg_ind_rsp_send(struct gatt_client_env_tag *gatt_env,
                                     uint8_t conidx,
                                     uint16_t cfg_val,
                                     uint8_t status)
{
    struct gatt_client_rd_cfg_ind_rsp *rsp = KE_MSG_ALLOC(GATT_CLIENT_RD_CFG_IND_RSP,
                                                prf_dst_task_get(&(gatt_env->prf_env), conidx),
                                                prf_src_task_get(&(gatt_env->prf_env), conidx),
                                                gatt_client_rd_cfg_ind_rsp);

    rsp->status = status;
    rsp->cfg_val = cfg_val;

    ke_msg_send(rsp);
}

void gatt_client_wr_cfg_ind_rsp_send(struct gatt_client_env_tag *gatt_env,
                                     uint8_t conidx,
                                     uint8_t status)
{
    struct gatt_client_wr_cfg_ind_rsp *rsp = KE_MSG_ALLOC(GATT_CLIENT_WR_CFG_IND_RSP,
                                                prf_dst_task_get(&(gatt_env->prf_env), conidx),
                                                prf_src_task_get(&(gatt_env->prf_env), conidx),
                                                gatt_client_wr_cfg_ind_rsp);

    rsp->status = status;
    ke_msg_send(rsp);
}
#endif // (BLE_GATT_CLIENT)
/// @} GATT_CLIENT
