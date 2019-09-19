/**
 ****************************************************************************************
 *
 * @file app_udss_task.c
 *
 * @brief User Data Service application task.
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
 * @addtogroup APPTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_APP_PRESENT)

#if (BLE_UDS_SERVER)

#include "udss_task.h"
#include "app_entry_point.h"
#include "user_callback_config.h"

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles UDS Characteristic value request message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udss_char_val_req_handler(ke_msg_id_t const msgid,
                                     struct udss_char_val_req const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_udss_cb.on_udss_char_val_req, KE_IDX_GET(src_id),
                    param->char_code)

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles UDS Control Point request message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udss_ucp_req_ind_handler(ke_msg_id_t const msgid,
                                    struct udss_ucp_req_ind const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_udss_cb.on_udss_cntl_point_write_ind, KE_IDX_GET(src_id),
                    &param->ucp_req)

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles UDS Characteristic value indication message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udss_char_val_ind_handler(ke_msg_id_t const msgid,
                                     struct udss_char_val const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    CALLBACK_ARGS_4(user_app_udss_cb.on_udss_char_val_ind, KE_IDX_GET(src_id),
                    param->char_code, param->length, param->value);

    return (KE_MSG_CONSUMED);
}

/*
 * LOCAL VARIABLES DEFINITION
 ****************************************************************************************
 */

static const struct ke_msg_handler app_udss_process_handlers[] =
{
    {UDSS_CHAR_VAL_REQ,         (ke_msg_func_t)udss_char_val_req_handler},
    {UDSS_UCP_REQ_IND,          (ke_msg_func_t)udss_ucp_req_ind_handler},
    {UDSS_SET_CHAR_VAL_IND,     (ke_msg_func_t)udss_char_val_ind_handler},
};

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_udss_process_handler(ke_msg_id_t const msgid,
                                                     void const *param,
                                                     ke_task_id_t const dest_id,
                                                     ke_task_id_t const src_id,
                                                     enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param,src_id, dest_id, msg_ret, app_udss_process_handlers,
                                 sizeof(app_udss_process_handlers) / sizeof(struct ke_msg_handler));
}

#endif // (BLE_UDS_SERVER)

#endif // (BLE_APP_PRESENT)

/// @} APPTASK
