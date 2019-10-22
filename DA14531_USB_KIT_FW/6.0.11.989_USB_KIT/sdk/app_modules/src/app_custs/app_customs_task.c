/**
 ****************************************************************************************
 *
 * @file app_customs_task.c
 *
 * @brief Custom Service application Task Implementation.
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

#include "rwble_config.h"              // SW configuration

#if (BLE_APP_PRESENT)

#if (BLE_CUSTOM_SERVER)
#include "app_customs_task.h"
#include "custs1_task.h"
#include "custs2_task.h"
#include "app_task.h"           // Application Task API
#include "app_entry_point.h"
#include "app.h"

#if (BLE_CUSTOM1_SERVER)
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if 0
/**
 ****************************************************************************************
 * @brief Handles CUSTOM 1 Server profile database creation confirmation.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_custs1_create_db_cfm_handler(ke_msg_id_t const msgid,
                                            struct custs1_create_db_cfm const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == APP_DB_INIT)
    {
        // Inform the Application Manager
        struct app_module_init_cmp_evt *cfm = KE_MSG_ALLOC(APP_MODULE_INIT_CMP_EVT,
                                                           TASK_APP, TASK_APP,
                                                           app_module_init_cmp_evt);

        cfm->status = param->status;

        ke_msg_send(cfm);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles disable indication from the CUSTOM 1 Server profile.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_custs1_disable_ind_handler(ke_msg_id_t const msgid,
                                          struct custs1_disable_ind const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

static const struct ke_msg_handler app_custs1_process_handlers[] =
{
    {CUSTS1_CREATE_DB_CFM,                 (ke_msg_func_t)app_custs1_create_db_cfm_handler},
    {CUSTS1_DISABLE_IND,                   (ke_msg_func_t)app_custs1_disable_ind_handler},
};
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_custs1_process_handler(ke_msg_id_t const msgid,
                                                       void const *param,
                                                       ke_task_id_t const dest_id,
                                                       ke_task_id_t const src_id,
                                                       enum ke_msg_status_tag *msg_ret)
{
#if 0
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, app_custs1_process_handlers,
                                         sizeof(app_custs1_process_handlers) / sizeof(struct ke_msg_handler));
#else
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, NULL, 0);
#endif
}
#endif //BLE_CUSTOM1_SERVER

#if (BLE_CUSTOM2_SERVER)
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if 0
/**
 ****************************************************************************************
 * @brief Handles CUSTOM 2 Server profile database creation confirmation.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_custs2_create_db_cfm_handler(ke_msg_id_t const msgid,
                                            struct custs2_create_db_cfm const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == APP_DB_INIT)
    {
        // Inform the Application Manager
        struct app_module_init_cmp_evt *cfm = KE_MSG_ALLOC(APP_MODULE_INIT_CMP_EVT,
                                                           TASK_APP, TASK_APP,
                                                           app_module_init_cmp_evt);

        cfm->status = param->status;

        ke_msg_send(cfm);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles disable indication from the CUSTOM 2 Server profile.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_custs2_disable_ind_handler(ke_msg_id_t const msgid,
                                          struct custs2_disable_ind const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

static const struct ke_msg_handler app_custs2_process_handlers[] =
{
    {CUSTS2_CREATE_DB_CFM,                 (ke_msg_func_t)app_custs2_create_db_cfm_handler},
    {CUSTS2_DISABLE_IND,                   (ke_msg_func_t)app_custs2_disable_ind_handler},
};
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_custs2_process_handler(ke_msg_id_t const msgid,
                                                       void const *param,
                                                       ke_task_id_t const dest_id,
                                                       ke_task_id_t const src_id,
                                                       enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid, param, src_id, dest_id, msg_ret, NULL, 0);
}
#endif // BLE_CUSTOM2_SERVER

#endif // BLE_CUSTOM_SERVER

#endif // BLE_APP_PRESENT

/// @} APPTASK
