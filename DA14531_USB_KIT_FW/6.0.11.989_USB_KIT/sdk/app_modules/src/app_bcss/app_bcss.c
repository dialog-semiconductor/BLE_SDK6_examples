/**
 ****************************************************************************************
 *
 * @file app_bcss.c
 *
 * @brief Body Composition Service Server application.
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
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_PRESENT)

#if (BLE_BCS_SERVER)
#include "app.h"
#include "app_task.h"
#include "app_bcss.h"
#include "app_prf_perm_types.h"

#include "bcs_common.h"
#include "bcss.h"
#include "bcss_task.h"

// Define some default features when flag is not defined
#ifndef APP_BCSS_FEATURES
#define APP_BCSS_FEATURES       BCS_FEAT_TIME_STAMP | \
                                BCS_FEAT_MULTIPLE_USERS | \
                                BCS_FEAT_BASAL_METABOLISM | \
                                BCS_FEAT_MUSCLE_PERCENTAGE | \
                                BCS_FEAT_MUSCLE_MASS | \
                                BCS_FEAT_FAT_FREE_MASS | \
                                BCS_FEAT_SOFT_LEAN_MASS | \
                                BCS_FEAT_BODY_WATER_MASS | \
                                BCS_FEAT_IMPEDANCE | \
                                BCS_FEAT_WEIGHT | \
                                BCS_FEAT_HEIGHT | \
                                BCS_FEAT_MASS_RES_05_KG | \
                                BCS_FEAT_HEIGHT_RES_001_M
#endif // APP_BCSS_FEATURES

#ifndef APP_BCSS_IS_PRIMARY
#define APP_BCSS_IS_PRIMARY     (1)
#endif

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

static bool measurement_ind_en __SECTION_ZERO("retention_mem_area0");

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_bcss_init(void)
{
    return;
}

void app_bcss_create_db(void)
{
    struct bcss_db_cfg* db_cfg;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct bcss_db_cfg));

    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_BCSS);
    req->prf_task_id = TASK_ID_BCSS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    db_cfg = (struct bcss_db_cfg* ) req->param;
    db_cfg->is_primary = APP_BCSS_IS_PRIMARY;
    db_cfg->features = APP_BCSS_FEATURES;

    ke_msg_send(req);
}

void app_bcss_enable(uint8_t conidx)
{
    // Allocate the message
    struct bcss_env_tag *bcss_env = PRF_ENV_GET(BCSS, bcss);
    struct bcss_enable_req * req = KE_MSG_ALLOC(BCSS_ENABLE_REQ,
                                                prf_src_task_get(&bcss_env->prf_env, conidx),
                                                TASK_APP,
                                                bcss_enable_req);

    req->meas_ind_en = measurement_ind_en ? GATT_CCC_START_IND : GATT_CCC_STOP_NTFIND;

    ke_msg_send(req);
}

void app_bcss_send_measurement(uint8_t conidx, const bcs_meas_t *meas)
{

    struct bcss_env_tag *bcss_env = PRF_ENV_GET(BCSS, bcss);

    struct bcss_meas_val_ind_req* ind = KE_MSG_ALLOC(BCSS_MEAS_VAL_IND_REQ,
                                                     prf_src_task_get(&bcss_env->prf_env, conidx),
                                                     TASK_APP,
                                                     bcss_meas_val_ind_req);
    ind->meas = *meas;
    ke_msg_send(ind);

}

void app_bcss_set_initial_measurement_ind_cfg(bool enabled)
{
    measurement_ind_en = enabled;
}

#endif // (BLE_BCS_SERVER)

#endif // BLE_APP_PRESENT

/// @} APP
