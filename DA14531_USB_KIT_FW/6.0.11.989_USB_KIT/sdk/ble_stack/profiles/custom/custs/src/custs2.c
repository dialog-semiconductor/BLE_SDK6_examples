/**
 ****************************************************************************************
 *
 * @file custs2.c
 *
 * @brief Custom Server 2 (CUSTS2) profile source file.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"              // SW configuration

#if (BLE_CUSTOM2_SERVER)
#include <string.h>

#include "custs2.h"
#include "custs2_task.h"
#include "attm_db.h"
#include "attm_db_128.h"
#include "gapc.h"
#include "prf.h"

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern const uint8_t custs2_services[];
extern const uint8_t custs2_services_size;
extern const uint16_t custs2_att_max_nb;
extern struct attm_desc_128 custs2_att_db[];

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the CUSTS2 module.
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
static uint8_t custs2_init(struct prf_task_env *env, uint16_t *start_hdl, uint16_t app_task, uint8_t sec_lvl, struct custs2_db_cfg *params)
{
    //------------------ create the attribute database for the profile -------------------

    // DB Creation Status
    uint8_t status = ATT_ERR_NO_ERROR;

    //-------------------- allocate memory required for the profile  ---------------------
    struct custs2_env_tag *custs2_env = (struct custs2_env_tag *) ke_malloc(sizeof(struct custs2_env_tag), KE_MEM_ATT_DB);
    memset(custs2_env, 0, sizeof(struct custs2_env_tag));

    // allocate CUSTS1 required environment variable
    env->env = (prf_env_t *)custs2_env;

    // Loop through the user defined services to generate the database
    for (uint8_t i = 0; i < custs2_services_size; i++)
    {
        // Create the service
        status = attm_svc_create_db_128(custs2_services[i],
                                        start_hdl,
                                        NULL,
                                        custs2_services[i+1],
                                        NULL,
                                        env->task,
                                        custs2_att_db,
                                        (sec_lvl & PERM_MASK_SVC_AUTH) | (sec_lvl & PERM_MASK_SVC_EKS) | PERM(SVC_PRIMARY, ENABLE));

        // Retain the handle of the first service in the profile
        if (i == 0)
        {
            custs2_env->shdl = *start_hdl;
        }

        // Prepare for the next service in the profile
        *start_hdl += custs2_services[i+1] - custs2_services[i];

        // Exit the for() loop if an error occurred while generating the service
        if (status != ATT_ERR_NO_ERROR)
        {
            break;
        }
    }

    if (status == ATT_ERR_NO_ERROR)
    {
        custs2_env->max_nb_att = custs2_att_max_nb;
        custs2_env->prf_env.app_task = app_task | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        custs2_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id                     = TASK_ID_CUSTS2;
        env->desc.idx_max           = CUSTS2_IDX_MAX;
        env->desc.state             = custs2_env->state;
        env->desc.default_handler   = &custs2_default_handler;
        co_list_init(&(custs2_env->values));
        custs2_init_ccc_values(custs2_att_db, custs2_att_max_nb);

        // profile is ready, go into an Idle state
        ke_state_set(env->task, CUSTS2_IDLE);
    }

    return status;
}
/**
 ****************************************************************************************
 * @brief Destruction of the CUSTS2 module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 ****************************************************************************************
 */
static void custs2_destroy(struct prf_task_env *env)
{
    struct custs2_env_tag *custs2_env = (struct custs2_env_tag *)env->env;

    // remove all values present in list
    while (!co_list_is_empty(&(custs2_env->values)))
    {
        struct co_list_hdr *hdr = co_list_pop_front(&(custs2_env->values));
        ke_free(hdr);
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(custs2_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void custs2_create(struct prf_task_env *env, uint8_t conidx)
{
    int att_idx;
    ASSERT_ERROR(conidx < BLE_CONNECTION_MAX);

    // Find all ccc fields and clean them
    for (att_idx = 1; att_idx < custs2_att_max_nb; att_idx++)
    {
        // Find only CCC characteristics
        if (custs2_att_db[att_idx].uuid_size == ATT_UUID_16_LEN &&
            *(uint16_t *)custs2_att_db[att_idx].uuid == ATT_DESC_CLIENT_CHAR_CFG)
        {
            // Clear CCC value
            custs2_set_ccc_value(conidx, att_idx, 0);
        }
    }
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
static void custs2_cleanup(struct prf_task_env *env, uint8_t conidx, uint8_t reason)
{
    int att_idx;
    struct custs2_env_tag *custs2_env = (struct custs2_env_tag *)env->env;

    // Find all ccc fields and clean them
    for (att_idx = 1; att_idx < custs2_att_max_nb; att_idx++)
    {
        // Find only CCC characteristics
        if (custs2_att_db[att_idx].uuid_size == ATT_UUID_16_LEN &&
            *(uint16_t *)custs2_att_db[att_idx].uuid == ATT_DESC_CLIENT_CHAR_CFG)
        {
            // Clear CCC value
            custs2_set_ccc_value(conidx, att_idx, 0);
        }
    }

    ke_state_set(prf_src_task_get(&(custs2_env->prf_env), conidx), CUSTS2_IDLE);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// CUSTS2 Task interface required by profile manager
const struct prf_task_cbs custs2_itf =
{
        (prf_init_fnct) custs2_init,
        custs2_destroy,
        custs2_create,
        custs2_cleanup,
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* custs2_prf_itf_get(void)
{
    return &custs2_itf;
}

#endif // (BLE_CUSTOM2_SERVER)
