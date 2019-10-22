/**
 ****************************************************************************************
 *
 * @file bcsc.c
 *
 * @brief C file - Body Composition Service Client Implementation.
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
 * @addtogroup BCSC
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "bcs_common.h"
#if (BLE_BCS_CLIENT)

#include <string.h>

#include "bcsc.h"
#include "bcsc_task.h"

/**
 ****************************************************************************************
 * @brief Initialization of the BCSC module.
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
static uint8_t bcsc_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                         uint8_t sec_lvl, void* params)
{
    uint8_t idx;

    struct bcsc_env_tag* bcsc_env =
            (struct bcsc_env_tag* ) ke_malloc(sizeof(struct bcsc_env_tag), KE_MEM_ATT_DB);
    memset(bcsc_env, 0, sizeof(struct bcsc_env_tag));

    // Allocate BCSC required environment variable
    env->env = (prf_env_t*) bcsc_env;

    bcsc_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
    bcsc_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);

    // Initialize environment variable
    env->id                     = TASK_ID_BCSC;
    env->desc.idx_max           = BCSC_IDX_MAX;
    env->desc.state             = bcsc_env->state;
    env->desc.state_handler     = NULL;
    env->desc.default_handler   = &bcsc_default_handler;

    for(idx = 0; idx < BCSC_IDX_MAX; idx++)
    {
        bcsc_env->env[idx] = NULL;
        // Service is ready, go into the Free state
        ke_state_set(KE_BUILD_ID(env->task, idx), BCSC_FREE);
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
static void bcsc_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct bcsc_env_tag* bcsc_env = (struct bcsc_env_tag*) env->env;

    // Clean-up environment variable allocated for task instance
    if(bcsc_env->env[conidx] != NULL)
    {
        ke_free(bcsc_env->env[conidx]);
        bcsc_env->env[conidx] = NULL;
    }

    // Put BCS Client in Free state
    ke_state_set(KE_BUILD_ID(env->task, conidx), BCSC_FREE);
}

/**
 ****************************************************************************************
 * @brief Destruction of the BCSC module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Client or Service allocated environment data.
 ****************************************************************************************
 */
static void bcsc_destroy(struct prf_task_env* env)
{
    uint8_t idx;
    struct bcsc_env_tag* bcsc_env = (struct bcsc_env_tag*) env->env;

    // Cleanup environment variable for each task instances
    for(idx = 0; idx < BCSC_IDX_MAX ; idx++)
    {
        if(bcsc_env->env[idx] != NULL)
        {
            bcsc_cleanup(env, idx, 0);
        }
    }

    // Free profile environment variables
    env->env = NULL;
    ke_free(bcsc_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void bcsc_create(struct prf_task_env* env, uint8_t conidx)
{
    // Put ANP Client in Idle state
    ke_state_set(KE_BUILD_ID(env->task, conidx), BCSC_IDLE);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// WSSC Task interface required by profile manager
const struct prf_task_cbs bcsc_itf =
{
    (prf_init_fnct) bcsc_init,
    bcsc_destroy,
    bcsc_create,
    bcsc_cleanup,
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* bcsc_prf_itf_get(void)
{
   return &bcsc_itf;
}

void bcsc_enable_rsp_send(struct bcsc_env_tag *bcsc_env, uint8_t conidx, uint8_t status)
{
    // Format response to app
    struct bcsc_enable_rsp * rsp = KE_MSG_ALLOC(BCSC_ENABLE_RSP,
                                                prf_dst_task_get(&(bcsc_env->prf_env), conidx),
                                                prf_src_task_get(&(bcsc_env->prf_env), conidx),
                                                bcsc_enable_rsp);

    rsp->status = status;

    if (status == GAP_ERR_NO_ERROR)
    {
        rsp->bcs = bcsc_env->env[conidx]->bcs;

        // Register BCSC task in gatt for indication/notifications
        prf_register_atthdl2gatt(&bcsc_env->prf_env, conidx, &(bcsc_env->env[conidx]->bcs.svc));

        // Go to IDLE state
        ke_state_set(prf_src_task_get(&(bcsc_env->prf_env), conidx), BCSC_IDLE);
    }
    else
    {
        // Clean-up environment variable allocated for task instance
        ke_free(bcsc_env->env[conidx]);
        bcsc_env->env[conidx] = NULL;
    }

    ke_msg_send(rsp);
}

void bcsc_bc_meas_cfg_ind_rsp_send(struct bcsc_env_tag * bcsc_env, uint8_t conidx, uint8_t status)
{
    struct bcsc_bc_meas_cfg_ind_rsp* rsp = KE_MSG_ALLOC(BCSC_BC_MEAS_CFG_IND_RSP,
                                                        prf_dst_task_get(&(bcsc_env->prf_env), conidx),
                                                        prf_src_task_get(&(bcsc_env->prf_env), conidx),
                                                        bcsc_bc_meas_cfg_ind_rsp);

    rsp->status = status;
    ke_msg_send(rsp);
}

void bcsc_unpack_meas_value(bcs_meas_t *pmeas_val, uint16_t *flags, uint8_t *packed_bp)
{
    uint8_t *ptr = packed_bp;

    // Get the flags
    *flags = co_read16p(ptr);
    ptr += sizeof(*flags);

    // Set Measurment Unit
    pmeas_val->measurement_unit = (*flags & BCM_FLAG_UNIT_IMPERIAL) ? BCS_UNIT_IMPERIAL : BCS_UNIT_SI;

    // Get the body fat percentage
    pmeas_val->body_fat_percentage = co_read16p(ptr);
    ptr += sizeof(pmeas_val->body_fat_percentage);

    if ((*flags & BCM_FLAG_TIME_STAMP) == BCM_FLAG_TIME_STAMP)
    {
        ptr += prf_unpack_date_time(ptr, &pmeas_val->time_stamp);
    }

    if ((*flags & BCM_FLAG_USER_ID) == BCM_FLAG_USER_ID)
    {
        pmeas_val->user_id = *ptr;
        ptr += sizeof(pmeas_val->user_id);
    }

    if ((*flags & BCM_FLAG_BASAL_METABOLISM) == BCM_FLAG_BASAL_METABOLISM)
    {
        pmeas_val->basal_metabolism = co_read16p(ptr);
        ptr += sizeof(pmeas_val->basal_metabolism);
    }

    if ((*flags & BCM_FLAG_MUSCLE_PERCENTAGE) == BCM_FLAG_MUSCLE_PERCENTAGE)
    {
        pmeas_val->muscle_percentage = co_read16p(ptr);
        ptr += sizeof(pmeas_val->muscle_percentage);
    }

    if ((*flags & BCM_FLAG_MUSCLE_MASS) == BCM_FLAG_MUSCLE_MASS)
    {
        pmeas_val->muscle_mass = co_read16p(ptr);
        ptr += sizeof(pmeas_val->muscle_mass);
    }

    if ((*flags & BCM_FLAG_FAT_FREE_MASS) == BCM_FLAG_FAT_FREE_MASS)
    {
        pmeas_val->fat_free_mass = co_read16p(ptr);
        ptr += sizeof(pmeas_val->fat_free_mass);
    }

    if ((*flags & BCM_FLAG_SOFT_LEAN_MASS) == BCM_FLAG_SOFT_LEAN_MASS)
    {
        pmeas_val->soft_lean_mass = co_read16p(ptr);
        ptr += sizeof(pmeas_val->soft_lean_mass);
    }

    if ((*flags & BCM_FLAG_BODY_WATER_MASS) == BCM_FLAG_BODY_WATER_MASS)
    {
        pmeas_val->body_water_mass = co_read16p(ptr);
        ptr += sizeof(pmeas_val->body_water_mass);
    }

    if ((*flags & BCM_FLAG_IMPEDANCE) == BCM_FLAG_IMPEDANCE)
    {
        pmeas_val->impedance = co_read16p(ptr);
        ptr += sizeof(pmeas_val->impedance);
    }

    if ((*flags & BCM_FLAG_WEIGHT) == BCM_FLAG_WEIGHT)
    {
        pmeas_val->weight = co_read16p(ptr);
        ptr += sizeof(pmeas_val->weight);
    }

    if ((*flags & BCM_FLAG_HEIGHT) == BCM_FLAG_HEIGHT)
    {
        pmeas_val->height = co_read16p(ptr);
        ptr += sizeof(pmeas_val->height);
    }
}

#endif // (BLE_BCS_CLIENT)

/// @} BCSC
