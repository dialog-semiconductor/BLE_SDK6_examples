/**
 ****************************************************************************************
 *
 * @file app_entry_point.h
 *
 * @brief Application entry point header file.
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

#ifndef _APP_ENTRY_POINT_H_
#define _APP_ENTRY_POINT_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "ke_msg.h"
#include "ke_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define KE_FIND_RELATED_TASK(task) ((ke_msg_id_t)((task) >> 10))

#if defined (__DA14531__) && !defined (__EXCLUDE_ROM_APP_TASK__) && (APP_EASY_MAX_ACTIVE_CONNECTION > 1)
    #error "Cannot use ROM app task symbols if number of connections is greater than 1. \
Workaround: Define the __EXCLUDE_ROM_APP_TASK__ flag and comment out the app task related symbols in ROM symbols file (da14531_symbols.txt)"
#endif

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response
{
    PR_EVENT_HANDLED =0,
    PR_EVENT_UNHANDLED
};

/// Format of a task message handler function
typedef enum process_event_response(*process_event_func_t)(ke_msg_id_t const msgid,
                                                           void const *param,
                                                           ke_task_id_t const dest_id,
                                                           ke_task_id_t const src_id,
                                                           enum ke_msg_status_tag *msg_ret);

/// Format of a catch rest event handler function
typedef void(*catch_rest_event_func_t)(ke_msg_id_t const msgid,
                                       void const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id);

#if defined (__DA14531__) && !defined (__EXCLUDE_ROM_APP_TASK__)

#include "app_prf_perm_types.h"

#if (BLE_APP_SEC)
#include "app_security.h"
#endif

typedef struct
{
    void (*app_easy_gap_dev_configure)(void);
    bool (*app_db_init_next)(void);
    const struct app_callbacks *user_app_callbacks;
    const catch_rest_event_func_t catch_rest_func;
    const process_event_func_t *app_process_handlers;
    struct app_env_tag *app_env;
    struct app_sec_bond_data_env_tag *app_sec_env;
    app_prf_srv_sec_t *app_prf_srv_perm;
} rom_app_task_cfg_t;

#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Application entry point handler.
 * @param[in] msgid       Message Id
 * @param[in] param       Pointer to message
 * @param[in] dest_id     Destination task Id
 * @param[in] src_id      Source task Id
 * @return Message status
 ****************************************************************************************
 */
int app_entry_point_handler(ke_msg_id_t const msgid,
                            void const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id);
                                       
/**
 ****************************************************************************************
 * @brief Application standard process event handler.
 * @param[in] msgid       Message Id
 * @param[in] param       Pointer to message
 * @param[in] src_id      Source task Id
 * @param[in] dest_id     Destination task Id
 * @param[in] msg_ret     Message status returned
 * @param[in] handlers    Pointer to message handlers
   @param[in] handler_num Handler number
 * @return process_event_response PR_EVENT_HANDLED or PR_EVENT_UNHANDLED
 ****************************************************************************************
 */
enum process_event_response app_std_process_event(ke_msg_id_t const msgid,
                                                  void const *param,
                                                  ke_task_id_t const src_id,
                                                  ke_task_id_t const dest_id,
                                                  enum ke_msg_status_tag *msg_ret,
                                                  const struct ke_msg_handler *handlers,
                                                  const int handler_num);

/// @} APP

#endif // _APP_ENTRY_POINT_H_
