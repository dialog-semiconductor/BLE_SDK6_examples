/**
 ****************************************************************************************
 *
 * @file app_ctss.c
 *
 * @brief Current Time Service Application entry point
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

#if (BLE_CTS_SERVER)
#include "app_ctss.h"                // Current Time Service Application Definitions
#include "app.h"                     // Application Definitions
#include "app_task.h"                // Application Task Definitions
#include "ctss_task.h"               // Health Thermometer Functions
#include "app_prf_perm_types.h"
#include "user_profiles_config.h"

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_ctss_init(void)
{
    // Nothing to do
}

void app_ctss_create_db(void)
{
    struct ctss_db_cfg *db_cfg;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct ctss_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_CTSS);
    req->prf_task_id = TASK_ID_CTSS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct ctss_db_cfg *) req->param;
    db_cfg->features = APP_CTS_FEATURES;

    // Send the message
    ke_msg_send(req);
}

void app_ctss_notify_current_time(const struct cts_curr_time *curr_time)
{
    struct ctss_ntf_curr_time_req *req = KE_MSG_ALLOC(CTSS_NTF_CURR_TIME_REQ,
                                                      prf_get_task_from_id(TASK_ID_CTSS),
                                                      TASK_APP,
                                                      ctss_ntf_curr_time_req);

    // Fill message
    req->current_time = *curr_time;

    // Send the message
    ke_msg_send(req);
}

void app_ctss_set_local_time_info(const struct cts_loc_time_info *local_time_info)
{
    struct ctss_upd_local_time_info_req *req = KE_MSG_ALLOC(CTSS_UPD_LOCAL_TIME_INFO_REQ,
                                                            prf_get_task_from_id(TASK_ID_CTSS),
                                                            TASK_APP,
                                                            ctss_upd_local_time_info_req);

    // Fill message
    req->loc_time_info = *local_time_info;

    // Send the message
    ke_msg_send(req);
}

#endif // (BLE_CTS_SERVER)

#endif // BLE_APP_PRESENT

/// @} APP
