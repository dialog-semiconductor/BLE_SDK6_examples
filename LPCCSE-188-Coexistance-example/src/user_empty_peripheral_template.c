/**
 ****************************************************************************************
 *
 * @file user_empty_peripheral_template.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "user_empty_peripheral_template.h"

#if (WLAN_COEX_ENABLED)
#include "wlan_coex.h"
#include "lld.h"
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/


void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}

void user_app_on_init(void)
{
    default_app_on_init();
    
#if (WLAN_COEX_ENABLED)
   wlan_coex_init();

   // Adds priority case for a specific connection
   wlan_coex_prio_criteria_add(WLAN_COEX_BLE_PRIO_MISSED, app_env[0].conhdl, 5);
#endif
}
 
sleep_mode_t user_app_on_validate_sleep(sleep_mode_t sleep_mode)
{
#if (WLAN_COEX_ENABLED)
   wlan_coex_going_to_sleep();
#endif
    return sleep_mode;
}
/// @} APP
