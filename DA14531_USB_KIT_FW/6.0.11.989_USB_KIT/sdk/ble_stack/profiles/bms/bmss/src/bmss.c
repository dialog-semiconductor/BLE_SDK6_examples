/**
 ****************************************************************************************
 *
 * @file bmss.c
 *
 * @brief C file - Bond Management Service Server Implementation.
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
 * @addtogroup BMSS
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_BMS_SERVER)

#include <string.h>

#include "bmss.h"
#include "bmss_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define BMS_FEAT_MAX_LEN    (4)

#define BMS_FEAT_CROP_MASK  (0xFF000000)
#define BMS_ALL_FEAT_MASK   (BMS_FEAT_DEL_BOND | \
                             BMS_FEAT_DEL_BOND_AUTHZ | \
                             BMS_FEAT_DEL_ALL_BOND | \
                             BMS_FEAT_DEL_ALL_BOND_AUTHZ | \
                             BMS_FEAT_DEL_ALL_BOND_BUT_PEER | \
                             BMS_FEAT_DEL_ALL_BOND_BUT_PEER_AUTHZ)

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the BMSS module.
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
 * @param[in]     params     Configuration parameters of profile collector or service (32 bits aligned)
 *
 * @return status code to know if profile initialization succeed or not.
 ****************************************************************************************
 */
static uint8_t bmss_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                            uint8_t sec_lvl, struct bmss_db_cfg* params)
{
    // DB Creation Status
    uint8_t status = ATT_ERR_NO_ERROR;
    // Mask for all attributes
    const uint32_t db_cfg_flag = 0x3F;

    // Database structure deifinition
    const struct attm_desc bms_att_db[BMS_IDX_NB] =
    {
        // Bond Management Service Declaration
        [BMS_IDX_SVC]                 = {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0},

        // Bond Managment Control Point Characteristic Declaration
        [BMS_IDX_BM_CNTL_PT_CHAR]     = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), BMS_IDX_BM_CNTL_PT_VAL},
        // Bond Managment Control Point Characteristic Value
        [BMS_IDX_BM_CNTL_PT_VAL]      = {ATT_CHAR_BM_CNTL_PT, PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE) | PERM(EXT, ENABLE), BMS_CNTL_PT_MAX_LEN},
        // Bond Management Control Point Characteristic Extended Properties
        [BMS_IDX_BM_CNTL_PT_EXT_PROP] = {ATT_DESC_CHAR_EXT_PROPERTIES, PERM(RD, ENABLE), (params->reliable_writes & 0x01) ? ATT_EXT_RELIABLE_WRITE : 0},

        // Bond Management Feature Characteristic Declaration
        [BMS_IDX_BM_FEAT_CHAR]        = {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), BMS_IDX_BM_FEAT_VAL},
        // Bond Management Feature Characteristic Value
        [BMS_IDX_BM_FEAT_VAL]         = {ATT_CHAR_BM_FEAT, PERM(RD, ENABLE), BMS_FEAT_MAX_LEN},
    };

    status = attm_svc_create_db(start_hdl, ATT_SVC_BOND_MANAGEMENT,
                                (uint8_t *)&db_cfg_flag, BMS_IDX_NB,
                                NULL, env->task, &bms_att_db[0],
                                (sec_lvl & PERM_MASK_SVC_AUTH) | \
                                (sec_lvl & PERM_MASK_SVC_EKS) | \
                                PERM(SVC_PRIMARY, ENABLE) | PERM(SVC_MI, DISABLE));

    if (status == ATT_ERR_NO_ERROR)
    {
        uint32_t bmss_feat_val = 0;

        // Allocate BMSS required environment variable
        struct bmss_env_tag* bmss_env =
                (struct bmss_env_tag*) ke_malloc(sizeof(struct bmss_env_tag),
                KE_MEM_ENV);
        memset(bmss_env, 0, sizeof(struct bmss_env_tag));

        bmss_env->shdl              = *start_hdl;        // start_hdl should now be altered
        bmss_env->features          = params->features;
        bmss_env->reliable_writes   = params->reliable_writes;
        bmss_env->prf_env.app_task  = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        bmss_env->prf_env.prf_task  = env->task | PERM(PRF_MI, ENABLE);

        // Initialize environment variable
        env->id                     = TASK_ID_BMSS;
        env->desc.idx_max           = BMSS_IDX_MAX;
        env->desc.state             = bmss_env->state;
        env->desc.state_handler     = bmss_state_handler;
        env->desc.default_handler   = &bmss_default_handler;
        env->env = (prf_env_t*) bmss_env;

        // Set start handle to first allocated service value
        *start_hdl = bmss_env->shdl;

        // Prepare features characteristic value and filter out unsupported feature bits
        co_write32p(&bmss_feat_val, (params->features & BMS_ALL_FEAT_MASK));
        if (bmss_feat_val != 0)
        {
            uint32_t bms_feat_len = 1;
            uint8_t index;

            // Check how many feature octets should be saved in DB
            for (index = 0; index < BMS_FEAT_MAX_LEN; index++)
            {
                if (bmss_feat_val & (BMS_FEAT_CROP_MASK >> (8 * index)))
                {
                    bms_feat_len = BMS_FEAT_MAX_LEN - index;
                    break;
                }
            }

            // Set initial value for features characteristic
            status = attmdb_att_set_value(bmss_env->shdl + BMS_IDX_BM_FEAT_VAL,
                                            bms_feat_len, 0, (uint8_t *) &bmss_feat_val);

            // Service is ready, go into the Idle state
            ke_state_set(env->task, BMSS_IDLE);
        }
        else
        {
            status = PRF_ERR_INVALID_PARAM;
        }
    }

    return status;
}

/**
 ****************************************************************************************
 * @brief Destruction of the BMSS module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Client or Service allocated environment data.
 ****************************************************************************************
 */
static void bmss_destroy(struct prf_task_env* env)
{
    struct bmss_env_tag* bmss_env = (struct bmss_env_tag*) env->env;

    // free profile environment variables
    env->env = NULL;
    ke_free(bmss_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void bmss_create(struct prf_task_env* env, uint8_t conidx)
{
    // Nothing to do here
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
static void bmss_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    ke_state_set(KE_BUILD_ID(env->task, conidx), BMSS_IDLE);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// BMSS Task interface required by profile manager
const struct prf_task_cbs bmss_itf =
{
    (prf_init_fnct) bmss_init,
    bmss_destroy,
    bmss_create,
    bmss_cleanup,
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* bmss_prf_itf_get(void)
{
   return &bmss_itf;
}

#endif // (BLE_BMS_SERVER)

/// @} BMSS
