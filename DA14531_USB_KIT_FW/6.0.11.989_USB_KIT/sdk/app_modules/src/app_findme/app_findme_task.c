/**
 ****************************************************************************************
 *
 * @file app_findme_task.c
 *
 * @brief Findme locator and target application task.
 *
 * Copyright (c) 2012-2018 Dialog Semiconductor. All rights reserved.
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

#include "rwip_config.h"               // SW configuration

#if (BLE_APP_PRESENT)

#if (BLE_FINDME_TARGET) || (BLE_FINDME_LOCATOR)
#include "findl_task.h"                 // Application Task API
#include "findt_task.h"                 // Application Task API
#include "app_findme_task.h"            // Application Task API
#include "app_findme.h"                 // Application Task API
#include "app_task.h"                   // Application Task API
#include "user_callback_config.h"
#include "app_entry_point.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if (BLE_FINDME_TARGET)
/**
 ****************************************************************************************
 * @brief Handles Alert indication from findme target.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int findt_alert_ind_handler(ke_msg_id_t const msgid,
                                   struct findt_alert_ind const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_findt_cb.on_findt_alert_ind, param->conidx, param->alert_lvl)

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles APP_FINDT_TIMER message.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_findt_timer_handler(ke_msg_id_t const msgid,
                                   void const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    if (alert_state.blink_toggle)
    {
        GPIO_SetInactive(alert_state.port, alert_state.pin);
        alert_state.blink_toggle = 0;
    }
    else
    {
        GPIO_SetActive(alert_state.port, alert_state.pin);
        alert_state.blink_toggle = 1;
    }

    ke_timer_set(APP_FINDT_TIMER, dest_id, alert_state.blink_timeout);

    return (KE_MSG_CONSUMED);
}
#endif // BLE_FINDME_TARGET

#if (BLE_FINDME_LOCATOR)
/**
 ****************************************************************************************
 * @brief Handles FINDL_ENABLE_RSP from findme locator.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int findl_enable_rsp_handler(ke_msg_id_t const msgid,
                                    struct findl_enable_rsp const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    CALLBACK_ARGS_1(user_app_findt_cb.on_findl_enable_rsp, param)

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles FINDL_SET_ALERT_RSP from findme locator.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int findl_set_alert_rsp_handler(ke_msg_id_t const msgid,
                                       struct findl_set_alert_rsp const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    CALLBACK_ARGS_1(user_app_findt_cb.on_findl_set_alert_rsp, param->status)

    return (KE_MSG_CONSUMED);
}
#endif // BLE_FINDME_LOCATOR

#if (BLE_FINDME_TARGET || BLE_FINDME_LOCATOR)

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

static const struct ke_msg_handler app_findme_process_handlers[]=
{
#if (BLE_FINDME_TARGET)
    {FINDT_ALERT_IND,                       (ke_msg_func_t)findt_alert_ind_handler},
    {APP_FINDT_TIMER,                       (ke_msg_func_t)app_findt_timer_handler},
#endif

#if (BLE_FINDME_LOCATOR)
    {FINDL_ENABLE_RSP,                      (ke_msg_func_t)findl_enable_rsp_handler},
    {FINDL_SET_ALERT_RSP,                   (ke_msg_func_t)findl_set_alert_rsp_handler},
#endif
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_findme_process_handler(ke_msg_id_t const msgid,
                                                       void const *param,
                                                       ke_task_id_t const dest_id,
                                                       ke_task_id_t const src_id,
                                                       enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, app_findme_process_handlers,
                                         sizeof(app_findme_process_handlers) / sizeof(struct ke_msg_handler));
}
#endif // (BLE_FINDME_TARGET || BLE_FINDME_LOCATOR)

#endif // (BLE_FINDME_TARGET) || (BLE_FINDME_LOCATOR)

#endif // BLE_APP_PRESENT

/// @} APPTASK
