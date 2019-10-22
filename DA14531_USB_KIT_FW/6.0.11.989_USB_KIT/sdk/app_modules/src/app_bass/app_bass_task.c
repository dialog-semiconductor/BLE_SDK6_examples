/**
 ****************************************************************************************
 *
 * @file app_bass_task.c
 *
 * @brief Battery server application task.
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

#include "rwip_config.h"

#if (BLE_APP_PRESENT)

#if (BLE_BATT_SERVER)

#include "bass_task.h"
#include "app_bass.h"
#include "app_bass_task.h"
#include "gpio.h"
#include "app_entry_point.h"
#include "app.h"
#include "user_callback_config.h"

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static int bass_enable_rsp_handler(ke_msg_id_t const msgid,
                                   struct bass_enable_rsp const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    return (KE_MSG_CONSUMED);
}

static int bass_batt_level_upd_rsp_handler(ke_msg_id_t const msgid,
                                           struct bass_batt_level_upd_rsp const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    CALLBACK_ARGS_1(user_app_bass_cb.on_batt_level_upd_rsp, param->status)
    
    return (KE_MSG_CONSUMED);
}

static int bass_batt_level_ntf_cfg_ind_handler(ke_msg_id_t const msgid,
                                               struct bass_batt_level_ntf_cfg_ind const *ind,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_bass_cb.on_batt_level_ntf_cfg_ind, ind->conidx, ind->ntf_cfg)
    
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles Battery Level polling timer
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_bass_timer_handler(ke_msg_id_t const msgid,
                                  void const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    app_batt_lvl();

    app_timer_set(APP_BASS_TIMER, dest_id, bat_poll_timeout);

    return (KE_MSG_CONSUMED);
}

#ifndef CUSTOM_BATTERY_LEVEL_ALERT_LED_HANDLING
/**
 ****************************************************************************************
 * @brief Handles Battery Alert time.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_bass_alert_timer_handler(ke_msg_id_t const msgid,
                                        void const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    // Read LED GPIO state
    if (bat_lvl_alert_used)
    {
        if (bat_led_state)
        {
            GPIO_SetInactive(bat_led_port, bat_led_pin);
            bat_led_state = 0;
            app_timer_set(APP_BASS_ALERT_TIMER, dest_id, 20);
        }
        else
        {
            GPIO_SetActive(bat_led_port, bat_led_pin);
            bat_led_state = 1;
            app_timer_set(APP_BASS_ALERT_TIMER, dest_id, 5);
        }
    }
    return (KE_MSG_CONSUMED);
}
#endif // CUSTOM_BATTERY_LEVEL_ALERT_LED_HANDLING

/*
 * LOCAL VARIABLES DEFINITION
 ****************************************************************************************
 */

static const struct ke_msg_handler app_bass_process_handlers[] =
{
    {BASS_ENABLE_RSP,                       (ke_msg_func_t)bass_enable_rsp_handler},
    {BASS_BATT_LEVEL_UPD_RSP,               (ke_msg_func_t)bass_batt_level_upd_rsp_handler},
    {BASS_BATT_LEVEL_NTF_CFG_IND,           (ke_msg_func_t)bass_batt_level_ntf_cfg_ind_handler},
    {APP_BASS_TIMER,                        (ke_msg_func_t)app_bass_timer_handler},
    {APP_BASS_ALERT_TIMER,                  (ke_msg_func_t)app_bass_alert_timer_handler},
};

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_bass_process_handler(ke_msg_id_t const msgid,
                                                     void const *param,
                                                     ke_task_id_t const dest_id,
                                                     ke_task_id_t const src_id,
                                                     enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param,src_id, dest_id, msg_ret, app_bass_process_handlers,
                                         sizeof(app_bass_process_handlers) / sizeof(struct ke_msg_handler));
}

#endif //(BLE_BATT_SERVER)

#endif //(BLE_APP_PRESENT)

/// @} APPTASK
