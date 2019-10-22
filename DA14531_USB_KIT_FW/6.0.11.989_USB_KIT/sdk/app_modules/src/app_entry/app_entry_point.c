/**
 ****************************************************************************************
 *
 * @file app_entry_point.c
 *
 * @brief Application entry point source code.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "app_api.h"
#include "user_callback_config.h"
#include "user_modules_config.h"

#if (!EXCLUDE_DLG_GAP)
#include "app_task.h"
#endif

#if (!EXCLUDE_DLG_TIMER)
#include "app_easy_timer.h"
#endif

#if (!EXCLUDE_DLG_MSG)
#include "app_easy_msg_utils.h"
#endif

#if ((BLE_APP_SEC) && (!EXCLUDE_DLG_SEC))
#include "app_security_task.h"
#endif

#if ((BLE_DIS_SERVER) && (!EXCLUDE_DLG_DISS))
#include "app_diss_task.h"
#endif

#if ((BLE_PROX_REPORTER) && (!EXCLUDE_DLG_PROXR))
#include "app_proxr_task.h"
#endif

#if ((BLE_BATT_SERVER) && (!EXCLUDE_DLG_BASS))
#include "app_bass_task.h"
#endif

#if ((BLE_CTS_SERVER) && (!EXCLUDE_DLG_CTSS))
#include "app_ctss_task.h"
#endif

#if ((BLE_CTS_CLIENT) && (!EXCLUDE_DLG_CTSC))
#include "app_ctsc_task.h"
#endif

#if (((BLE_FINDME_TARGET) && (!EXCLUDE_DLG_FINDT)) || ((BLE_FINDME_LOCATOR) && (!EXCLUDE_DLG_FINDL)))
#include "app_findme_task.h"
#endif

#if ((BLE_BMS_SERVER) && (!EXCLUDE_DLG_BMSS))
#include "app_bmss_task.h"
#endif

#if ((BLE_BCS_SERVER) && (!EXCLUDE_DLG_BCSS))
#include "app_bcss_task.h"
#endif

#if ((BLE_UDS_SERVER) && (!EXCLUDE_DLG_UDSS))
#include "app_udss_task.h"
#endif

#if ((BLE_GATT_CLIENT) && (!EXCLUDE_DLG_GATTC))
#include "app_gattc_task.h"
#endif

#if ((BLE_SUOTA_RECEIVER) && (!EXCLUDE_DLG_SUOTAR))
#include "app_suotar_task.h"
#endif

#if (BLE_CUSTOM_SERVER)
#include "app_customs_task.h"
#endif

#if ((BLE_ANC_CLIENT) && (!EXCLUDE_DLG_ANCC))
#include "app_ancc_task.h"
#endif

#if ((BLE_WSS_SERVER) && (!EXCLUDE_DLG_WSSS))
#include "app_wsss_task.h"
#endif

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

const process_event_func_t app_process_handlers[] = {

#if (!EXCLUDE_DLG_GAP)
    (process_event_func_t) app_gap_process_handler,
#endif

#if (!EXCLUDE_DLG_TIMER)
    (process_event_func_t) app_timer_api_process_handler,
#endif

#if (!EXCLUDE_DLG_MSG)
    (process_event_func_t) app_msg_utils_api_process_handler,
#endif

#if ((BLE_APP_SEC) && (!EXCLUDE_DLG_SEC))
    (process_event_func_t) app_sec_process_handler,
#endif

#if ((BLE_DIS_SERVER) && (!EXCLUDE_DLG_DISS))
    (process_event_func_t) app_diss_process_handler,
#endif

#if ((BLE_PROX_REPORTER) && (!EXCLUDE_DLG_PROXR))
    (process_event_func_t) app_proxr_process_handler,
#endif

#if ((BLE_BATT_SERVER) && (!EXCLUDE_DLG_BASS))
    (process_event_func_t) app_bass_process_handler,
#endif

#if ((BLE_CTS_SERVER) && (!EXCLUDE_DLG_CTSS))
    (process_event_func_t) app_ctss_process_handler,
#endif

#if ((BLE_CTS_CLIENT) && (!EXCLUDE_DLG_CTSC))
    (process_event_func_t) app_ctsc_process_handler,
#endif

#if (((BLE_FINDME_TARGET) && (!EXCLUDE_DLG_FINDT)) || ((BLE_FINDME_LOCATOR) && (!EXCLUDE_DLG_FINDL)))
    (process_event_func_t) app_findme_process_handler,
#endif

#if ((BLE_BMS_SERVER) && (!EXCLUDE_DLG_BMSS))
    (process_event_func_t) app_bmss_process_handler,
#endif

#if ((BLE_BCS_SERVER) && (!EXCLUDE_DLG_BCSS))
    (process_event_func_t) app_bcss_process_handler,
#endif

#if ((BLE_ANC_CLIENT) && (!EXCLUDE_DLG_ANCC))
    (process_event_func_t) app_ancc_process_handler,
#endif

#if ((BLE_UDS_SERVER) && (!EXCLUDE_DLG_UDSS))
    (process_event_func_t) app_udss_process_handler,
#endif

#if ((BLE_SUOTA_RECEIVER) && (!EXCLUDE_DLG_SUOTAR))
    (process_event_func_t) app_suotar_process_handler,
#endif

#if ((BLE_CUSTOM1_SERVER) && (!EXCLUDE_DLG_CUSTS1))
    (process_event_func_t) app_custs1_process_handler,
#endif

#if ((BLE_CUSTOM2_SERVER) && (!EXCLUDE_DLG_CUSTS2))
    (process_event_func_t) app_custs2_process_handler,
#endif
#if ((BLE_WSS_SERVER) && (!EXCLUDE_DLG_WSSS))
    (process_event_func_t) app_wsss_process_handler,
#endif
#if ((BLE_GATT_CLIENT) && (!EXCLUDE_DLG_GATTC))
    (process_event_func_t) app_gattc_process_handler,
#endif

#if defined (__DA14531__) && !defined (__EXCLUDE_ROM_APP_TASK__)
    #define END_PROCESS_HANDLER_TABLE       (0xF8)
    (process_event_func_t) END_PROCESS_HANDLER_TABLE,
#endif
};

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_TASK__)

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

int app_entry_point_handler(ke_msg_id_t const msgid,
                            void const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id)
{
    int i = 0;
    enum ke_msg_status_tag process_msg_handling_result;

    while (i < (sizeof(app_process_handlers) / sizeof(process_event_func_t)))
    {
        ASSERT_ERROR(app_process_handlers[i]);
         if (app_process_handlers[i](msgid, param, dest_id, src_id, &process_msg_handling_result) == PR_EVENT_HANDLED)
         {
             return (process_msg_handling_result);
         }
         i++;
    }

    //user cannot do anything else than consume the message
    if (app_process_catch_rest_cb != NULL)
    {
        app_process_catch_rest_cb(msgid,param, dest_id, src_id);
    }

    return (KE_MSG_CONSUMED);
};

/**
 ****************************************************************************************
 * @brief Search for a message handler function.
 * @param[in] msg_id        Id of the message whose handler is under request
 * @param[in] handlers      Pointer to the message handler table
 * @param[in] handler_num   Range of the message handler table
 * @return The message handler function or null
 ****************************************************************************************
 */
static ke_msg_func_t handler_search(ke_msg_id_t const msg_id,
                                    const struct ke_msg_handler *handlers,
                                    const int handler_num)
{
    //table is empty
    if (handler_num == 0)
    {
        return NULL;
    }

    // Get the message handler function by parsing the message table
    for (int i = (handler_num-1); 0 <= i; i--)
    {
        if ((handlers[i].id == msg_id)
                || (handlers[i].id == KE_MSG_DEFAULT_HANDLER))
        {
            // If handler is NULL, message should not have been received in this state
            ASSERT_ERROR(handlers[i].func);

            return handlers[i].func;
        }
    }

    // If we execute this line of code, it means that we did not find the handler
    return NULL;
}

enum process_event_response app_std_process_event(ke_msg_id_t const msgid,
                                                  void const *param,
                                                  ke_task_id_t const src_id,
                                                  ke_task_id_t const dest_id,
                                                  enum ke_msg_status_tag *msg_ret,
                                                  const struct ke_msg_handler *handlers,
                                                  const int handler_num)
{
    ke_msg_func_t func = NULL;
    func = handler_search(msgid, handlers, handler_num);

    if (func != NULL)
    {
        *msg_ret = (enum ke_msg_status_tag) func(msgid, param, dest_id, src_id);
        return PR_EVENT_HANDLED;
    }
    else
    {
        return PR_EVENT_UNHANDLED;
    }
}

#else

const rom_app_task_cfg_t rom_app_task_cfg =
{
    .app_easy_gap_dev_configure = app_easy_gap_dev_configure,
    .app_db_init_next           = app_db_init_next,
    .user_app_callbacks         = &user_app_callbacks,
    .catch_rest_func            = app_process_catch_rest_cb,
    .app_process_handlers       = app_process_handlers,
    .app_env                    = app_env,
#if (BLE_APP_SEC)
    .app_sec_env                = app_sec_env,
#else
    .app_sec_env                = NULL,
#endif
    .app_prf_srv_perm           = app_prf_srv_perm,
};

#endif
