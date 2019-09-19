/**
 ****************************************************************************************
 *
 * @file app_callback.h
 *
 * @brief Application callbacks definitions.
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

#ifndef _APP_CALLBACK_H_
#define _APP_CALLBACK_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "co_bt.h"
#include "gapc_task.h"
#include "gapm_task.h"
#include "gap.h"
#include "app.h"

#if (BLE_APP_SEC)
#include "app_security.h"
#endif // (BLE_APP_SEC)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct app_callbacks
{
    void (*app_on_connection)(const uint8_t, struct gapc_connection_req_ind const *);
    void (*app_on_disconnect)(struct gapc_disconnect_ind const *);
    void (*app_on_update_params_rejected)(const uint8_t);
    void (*app_on_update_params_complete)(void);
    void (*app_on_set_dev_config_complete)(void);
    void (*app_on_adv_nonconn_complete)(const uint8_t);
    void (*app_on_adv_undirect_complete)(const uint8_t);
    void (*app_on_adv_direct_complete)(const uint8_t);
    void (*app_on_db_init_complete)(void);
    void (*app_on_scanning_completed)(const uint8_t);
    void (*app_on_adv_report_ind)(struct gapm_adv_report_ind const *);
    void (*app_on_connect_failed)(void);
    void (*app_on_get_dev_name)(struct app_device_name *);
    void (*app_on_get_dev_appearance)(uint16_t*);
    void (*app_on_get_dev_slv_pref_params)(struct gap_slv_pref*);
    void (*app_on_set_dev_info)(struct gapc_set_dev_info_req_ind const *, uint8_t*);
    void (*app_on_data_length_change)(const uint8_t, struct gapc_le_pkt_size_ind *);
    void (*app_on_update_params_request)(struct gapc_param_update_req_ind const *, struct gapc_param_update_cfm *);
    void (*app_on_generate_static_random_addr)(struct bd_addr *);
    void (*app_on_svc_changed_cfg_ind)(uint8_t, uint16_t);
#if (BLE_APP_SEC)
    void (*app_on_pairing_request)(const uint8_t, struct gapc_bond_req_ind const *);
    void (*app_on_tk_exch)(const uint8_t, struct gapc_bond_req_ind const *);
    void (*app_on_irk_exch)(struct gapc_bond_req_ind const *);
    void (*app_on_csrk_exch)(const uint8_t, struct gapc_bond_req_ind const *);
    void (*app_on_ltk_exch)(const uint8_t, struct gapc_bond_req_ind const *);
    void (*app_on_pairing_succeeded)(const uint8_t);
    void (*app_on_encrypt_ind)(const uint8_t, const uint8_t);
    void (*app_on_encrypt_req_ind)(const uint8_t, struct gapc_encrypt_req_ind const *);
    void (*app_on_security_req_ind)(const uint8_t);
    void (*app_on_addr_solved_ind)(const uint8_t, struct gapm_addr_solved_ind const *);
    void (*app_on_addr_resolve_failed)(const uint8_t);
    void (*app_on_ral_cmp_evt)(struct gapm_cmp_evt const *);
    void (*app_on_ral_size_ind)(const uint8_t);
    void (*app_on_ral_addr_ind)(const uint8_t, const uint8_t *);
#endif // (BLE_APP_SEC)
    void (*app_on_get_peer_features)(const uint8_t, struct gapc_peer_features_ind const *);
};

#if (BLE_APP_SEC)

struct app_bond_db_callbacks
{
    void (*app_bdb_init)(void);
    uint8_t (*app_bdb_get_size)(void);
    void (*app_bdb_add_entry)(struct app_sec_bond_data_env_tag *);
    void (*app_bdb_remove_entry)(enum bdb_search_by_type, enum bdb_remove_type, void *, uint8_t);
    const struct app_sec_bond_data_env_tag * (*app_bdb_search_entry)(enum bdb_search_by_type, void *, uint8_t);
    uint8_t (*app_bdb_get_number_of_stored_irks)(void);
    uint8_t (*app_bdb_get_stored_irks)(struct gap_sec_key *);
    bool (*app_bdb_get_device_info_from_slot)(uint8_t, struct gap_ral_dev_info *);
};

#endif // (BLE_APP_SEC)

/*
 * DEFINES
 ****************************************************************************************
 */

#define CALLBACK_ARGS_0(cb)                         {if (cb != NULL) cb();}
#define CALLBACK_ARGS_1(cb, arg1)                   {if (cb != NULL) cb(arg1);}
#define CALLBACK_ARGS_2(cb, arg1, arg2)             {if (cb != NULL) cb(arg1, arg2);}
#define CALLBACK_ARGS_3(cb, arg1, arg2, arg3)       {if (cb != NULL) cb(arg1, arg2, arg3);}
#define CALLBACK_ARGS_4(cb, arg1, arg2, arg3, arg4) {if (cb != NULL) cb(arg1, arg2, arg3, arg4);}

#endif // _APP_CALLBACK_H_
