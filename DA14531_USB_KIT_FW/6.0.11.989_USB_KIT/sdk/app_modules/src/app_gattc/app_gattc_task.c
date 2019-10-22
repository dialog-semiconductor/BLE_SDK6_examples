/**
 ****************************************************************************************
 *
 * @file app_gattc_task.c
 *
 * @brief Generic Attribute Profile Service Application Task
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
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"        // SW Configuration

#if (BLE_APP_PRESENT)

#if (BLE_GATT_CLIENT)

#include <stdint.h>
#include "gatt_client_task.h"
#include "gatt_client.h"
#include "app_gattc.h"
#include "app_gattc_task.h"
#include "app_task.h"
#include "app_entry_point.h"
#include "app.h"
#include "user_profiles_config.h"
#include "user_callback_config.h"

/**
 ****************************************************************************************
 * @brief Handles GATT Client enable response message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_gattc_enable_handler(const ke_msg_id_t msgid,
                                    const struct gatt_client_enable_rsp *param,
                                    const ke_task_id_t dest_id,
                                    const ke_task_id_t src_id)
{
    CALLBACK_ARGS_3(user_app_gattc_cb.on_gattc_enable, KE_IDX_GET(src_id), param->status, &param->gatt);

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GATT Client notification configuration write response message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_gattc_wr_cfg_ind_rsp_handler(const ke_msg_id_t msgid,
                                            const struct gatt_client_wr_cfg_ind_rsp *param,
                                            const ke_task_id_t dest_id,
                                            const ke_task_id_t src_id)
{
    CALLBACK_ARGS_2(user_app_gattc_cb.on_gattc_wr_cfg_ind_rsp, KE_IDX_GET(src_id), param->status);

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GATT Client notification configuration read response message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_gattc_rd_cfg_ind_rsp_handler(const ke_msg_id_t msgid,
                                            const struct gatt_client_rd_cfg_ind_rsp *param,
                                            const ke_task_id_t dest_id,
                                            const ke_task_id_t src_id)
{
    bool read_val = (param->cfg_val == PRF_CLI_START_IND) ? true : false;

    CALLBACK_ARGS_3(user_app_gattc_cb.on_gattc_rd_cfg_ind_rsp, KE_IDX_GET(src_id), param->status, read_val);

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GATT Clinet indication of the Service changed characteristic message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_gattc_svc_changed_ind_handler(const ke_msg_id_t msgid,
                                             struct gatt_client_svc_changed_ind *param,
                                             const ke_task_id_t dest_id,
                                             const ke_task_id_t src_id)
{
    CALLBACK_ARGS_2(user_app_gattc_cb.on_gattc_svc_changed_ind, KE_IDX_GET(src_id), &(param->val));
    return KE_MSG_CONSUMED;
}

/*
 * LOCAL VARIABLES DEFINITION
 ****************************************************************************************
 */

static const struct ke_msg_handler app_gattc_process_handlers[] =
{
    {GATT_CLIENT_ENABLE_RSP,        (ke_msg_func_t)app_gattc_enable_handler},
    {GATT_CLIENT_WR_CFG_IND_RSP,    (ke_msg_func_t)app_gattc_wr_cfg_ind_rsp_handler},
    {GATT_CLIENT_RD_CFG_IND_RSP,    (ke_msg_func_t)app_gattc_rd_cfg_ind_rsp_handler},
    {GATT_CLIENT_SVC_CHANGED_IND,   (ke_msg_func_t)app_gattc_svc_changed_ind_handler},
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_gattc_process_handler(ke_msg_id_t const msgid,
                                                      void const *param,
                                                      ke_task_id_t const dest_id,
                                                      ke_task_id_t const src_id,
                                                      enum ke_msg_status_tag *msg_ret)
{
    return (app_std_process_event(msgid, param, src_id, dest_id, msg_ret, app_gattc_process_handlers,
                                  sizeof(app_gattc_process_handlers) / sizeof(struct ke_msg_handler)));
}

#endif // (BLE_GATT_CLIENT)

#endif // BLE_APP_PRESENT

/// @} APP
