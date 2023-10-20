/**
 ****************************************************************************************
 *
 * @file user_callback_config.h
 *
 * @brief Callback functions configuration file.
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _USER_CALLBACK_CONFIG_H_
#define _USER_CALLBACK_CONFIG_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#if defined (REMOTE_MODE) || defined (BATCH_REMOTE_MODE)
#include "app_api.h"
#include "app_callback.h"
#include "app_prf_types.h"
#include "prod_test_rmt.h"
#endif

#include "mainloop_callbacks.h"
#include "dialog_prod.h"

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
#if defined (REMOTE_MODE) || defined (BATCH_REMOTE_MODE)
static const struct app_callbacks user_app_callbacks = {
    .app_on_connection                  = user_on_connection,
    .app_on_disconnect                  = user_on_disconnect,
    .app_on_update_params_rejected      = NULL,
    .app_on_update_params_complete      = NULL,
    .app_on_set_dev_config_complete     = default_app_on_set_dev_config_complete,
    .app_on_adv_nonconn_complete        = NULL,
    .app_on_adv_undirect_complete       = NULL,
    .app_on_adv_direct_complete         = NULL,
    .app_on_db_init_complete            = default_app_on_db_init_complete,
    .app_on_scanning_completed          = NULL,
    .app_on_adv_report_ind              = NULL,
    .app_on_get_dev_name                = default_app_on_get_dev_name,
    .app_on_get_dev_appearance          = default_app_on_get_dev_appearance,
    .app_on_get_dev_slv_pref_params     = default_app_on_get_dev_slv_pref_params,
    .app_on_set_dev_info                = default_app_on_set_dev_info,
    .app_on_data_length_change          = NULL,
    .app_on_update_params_request       = default_app_update_params_request,
    .app_on_generate_static_random_addr = default_app_generate_static_random_addr,
    .app_on_svc_changed_cfg_ind         = NULL,
    .app_on_get_peer_features           = NULL,
};

/*
 * "app_process_catch_rest_cb" symbol handling:
 * - Use #define if "user_catch_rest_hndl" is defined by the user
 * - Use const declaration if "user_catch_rest_hndl" is NULL
 */
#define app_process_catch_rest_cb       user_catch_rest_hndl
//static const catch_rest_event_func_t app_process_catch_rest_cb = NULL;

static const struct default_app_operations user_default_app_operations = {
    .default_operation_adv = default_advertise_operation,
};

#endif // REMOTE_MODE || BATCH_REMOTE_MODE

static const struct arch_main_loop_callbacks user_app_main_loop_callbacks = {
    .app_on_init            = user_app_on_init,

    // By default the watchdog timer is reloaded and resumed when the system wakes up.
    // The user has to take into account the watchdog timer handling (keep it running,
    // freeze it, reload it, resume it, etc), when the app_on_ble_powered() is being
    // called and may potentially affect the main loop.
    .app_on_ble_powered     = app_on_ble_powered,

    // By default the watchdog timer is reloaded and resumed when the system wakes up.
    // The user has to take into account the watchdog timer handling (keep it running,
    // freeze it, reload it, resume it, etc), when the app_on_system_powered() is being
    // called and may potentially affect the main loop.
    .app_on_system_powered  = app_on_full_power,

    .app_before_sleep       = NULL,
    .app_validate_sleep     = NULL,
    .app_going_to_sleep     = app_going_to_sleep,
    .app_resume_from_sleep  = app_resume_from_sleep,
};

#if defined (REMOTE_MODE) || defined (BATCH_REMOTE_MODE)
//place in this structure the app_<profile>_db_create and app_<profile>_enable functions
//for SIG profiles that do not have this function already implemented in the SDK
//or if you want to override the functionality. Check the prf_func array in the SDK
//for your reference of which profiles are supported.
static const struct prf_func_callbacks user_prf_funcs[] =
{
    {TASK_ID_INVALID,    NULL, NULL}   // DO NOT MOVE. Must always be last
};
#endif // REMOTE_MODE || BATCH_REMOTE_MODE

#endif // _USER_CALLBACK_CONFIG_H_
