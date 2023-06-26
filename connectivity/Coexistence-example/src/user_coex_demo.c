/**
 ****************************************************************************************
 *
 * @file user_coex_demo.c
 *
 * @brief Coexistence demo project source code.
 *
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
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
#include "user_coex_demo.h"

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
    
    // Adds priority for the advertising events only
    wlan_coex_prio_criteria_add(WLAN_COEX_BLE_PRIO_ADV, LLD_ADV_HDL, 0);
    
    // Uncomment for assigning priority on every data packets 
    //wlan_coex_prio_criteria_add(WLAN_COEX_BLE_PRIO_DATA, app_env[0].conhdl, 0);
    
    // Uncomment for assigning priority on every 6th missed event
    //wlan_coex_prio_criteria_add(WLAN_COEX_BLE_PRIO_MISSED, app_env[0].conhdl, 5);
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