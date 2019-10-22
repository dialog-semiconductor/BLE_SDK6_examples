/**
 ****************************************************************************************
 *
 * @file app_wsss.c
 *
 * @brief Weight Scale Profile Server application.
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

#if (BLE_WSS_SERVER)
#include "app.h"
#include "app_task.h"
#include "app_wsss.h"
#include "app_prf_perm_types.h"
#include "prf_utils.h"
#include "user_callback_config.h"

#include "wsss.h"
#include "wsss_task.h"

// Define some default features when flag is not defined
#ifndef APP_WSSS_FEATURES
#define APP_WSSS_FEATURES WSSS_FEAT_TIME_STAMP_SUPPORTED | \
                          WSSS_FEAT_BMI_SUPPORTED | \
                          WSSS_FEAT_HT_DISPLAY_1MM_ACC | \
                          WSSS_FEAT_WT_DISPLAY_500G_ACC
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

void app_wsss_init(void)
{
    return;
}

void app_wsss_create_db(void)
{
    struct wsss_db_cfg* db_cfg;
    const struct att_incl_desc *incl = NULL;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct wsss_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_WSSS);
    req->prf_task_id = TASK_ID_WSSS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;
    db_cfg = (struct wsss_db_cfg* ) req->param;
    db_cfg->ws_feature = APP_WSSS_FEATURES;

    if(user_app_wsss_cb.on_wsss_bcs_reference_req)
    {
        incl = user_app_wsss_cb.on_wsss_bcs_reference_req();
    }

    if(incl != NULL)
    {
        db_cfg->include_bcs_instance = true;
        memcpy(&db_cfg->bcs_ref, incl, sizeof(db_cfg->bcs_ref));
    }

    // Send the message
    ke_msg_send(req);
}

void app_wsss_enable(uint8_t conidx)
{
    struct wsss_env_tag *wsss_env = PRF_ENV_GET(WSSS, wsss);
    // Allocate the message
    struct wsss_enable_req * req = KE_MSG_ALLOC(WSSS_ENABLE_REQ,
                                                prf_src_task_get(&wsss_env->prf_env, conidx),
                                                TASK_APP,
                                                wsss_enable_req);

    req->wt_meas_ind_en = measurement_ind_en ? GATT_CCC_START_IND : GATT_CCC_STOP_NTFIND;

    ke_msg_send(req);
}

void app_wsss_send_measurement(uint8_t conidx, const struct wss_wt_meas *meas)
{
    struct wsss_env_tag *wsss_env = PRF_ENV_GET(WSSS, wsss);
    // Indicate the measurement
    struct wsss_meas_send_req *req = KE_MSG_ALLOC(WSSS_MEAS_SEND_REQ,
                                                  prf_src_task_get(&wsss_env->prf_env, conidx),
                                                  TASK_APP,
                                                  wsss_meas_send_req);

    memcpy(&req->meas_val, meas, sizeof(*meas));

    ke_msg_send(req);

}

void app_wsss_set_initial_measurement_ind_cfg(bool enabled)
{
    measurement_ind_en = enabled;
}

#endif // (BLE_WSS_SERVER)

#endif // BLE_APP_PRESENT

/// @} APP
