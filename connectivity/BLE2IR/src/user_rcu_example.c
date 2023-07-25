/**
 ****************************************************************************************
 *
 * @file user_rcu_example.c
 *
 * @brief Peripheral project source code.
 *
 * Copyright (c) 2015-2021 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration
#include "gap.h"
#include "app_easy_timer.h"
#include "user_rcu_example.h"
#include "user_custs1_impl.h"
#include "user_custs1_def.h"
#include "co_bt.h"
#include "user_modules.h"
#include "user_periph_setup.h"
#include "user_con_timeout.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

// Manufacturer Specific Data ADV structure type
struct mnf_specific_data_ad_structure
{
    uint8_t ad_structure_size;
    uint8_t ad_structure_type;
    uint8_t company_id[APP_AD_MSD_COMPANY_ID_LEN];
    uint8_t proprietary_data[APP_AD_MSD_DATA_LEN];
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint8_t connection_timer_id = 0;
static bool g_notifications_enabled = false;

uint8_t app_connection_idx                      __attribute__((section(".bss.")));
timer_hnd app_adv_data_update_timer_used        __attribute__((section(".bss.")));
timer_hnd app_param_update_request_timer_used   __attribute__((section(".bss.")));

// Retained variables
struct mnf_specific_data_ad_structure mnf_data  __attribute__((section(".bss."))); //@RETENTION MEMORY
// Index of manufacturer data in advertising data or scan response data (when MSB is 1)
uint8_t mnf_data_index                          __attribute__((section(".bss.")));//@RETENTION MEMORY
uint8_t stored_adv_data_len                     __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data_len                __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t stored_adv_data[ADV_DATA_LEN]           __attribute__((section(".bss.")));//@RETENTION MEMORY
uint8_t stored_scan_rsp_data[SCAN_RSP_DATA_LEN]__attribute__((section(".bss."))); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
static void user_sent_keycodes(void)
{
#ifdef CFG_PRINTF
		arch_printf("**  Sending keycode buffer contents...\n\r");
		arch_printf("**  Keycode:");
#endif	
		uint32_t keycode = 0;
		while ( 0 == 0 )
		{
				// retrieve
				keycode = app_kbd_get_key_code();

				if (keycode == KBD_KEY_BUFFER_EMPTY)
						break;
				
				user_svc1_kbd_ntf(keycode);
		#ifdef CFG_PRINTF
				arch_printf("0x%08X, ", keycode);
		#endif					
		}
		#ifdef CFG_PRINTF
				arch_printf("\n\r**  Complete.\n\n\r");
		#endif		
}

static bool user_is_ble_connected(void)
{	
		if (ke_state_get(TASK_APP) == APP_CONNECTED)
				return true;
		else
				return false;
}

#ifdef LED_INDICATION

static void turn_led_on()
{
		GPIO_SetActive(GPIO_LED_PORT, GPIO_LED_PIN);
}

static void turn_led_off()
{
		GPIO_SetInactive(GPIO_LED_PORT, GPIO_LED_PIN);
}

static bool ble_advertising_event()
{
		last_ble_evt event = arch_last_rwble_evt_get(); // Get the last BLE event
		bool ble_advertising_event = false;
	
		switch(event)
		{
				// Advertising Events
				case BLE_EVT_RX:
				case BLE_EVT_TX:
						ble_advertising_event = true;
						break;
				// Other Events		
				default:
						ble_advertising_event = false;
		}
		return ble_advertising_event;
}

static void update_led_status(void)
{		
		if (ble_advertising_event() ||user_is_ble_connected())
		// During advertising events or connection, turn led on		
				turn_led_on();
		else if (!ble_advertising_event() && !user_is_ble_connected())
		// Between advertising events while disconnected, turn led off
				turn_led_off();
}
#endif

static void user_set_notifications_enabled()
{
#ifdef CFG_PRINTF		
		arch_printf("**  Notifications enabled.\n\r");
#endif		
	g_notifications_enabled = true;

}

static void user_set_notifications_disabled()
{
		g_notifications_enabled = false;
}

static bool user_are_notifications_enabled()
{
		if (g_notifications_enabled)
		{
				return true;
		}
		else
		{
		#ifdef CFG_PRINTF
				if (user_is_ble_connected())
						arch_printf("**  Warning: Enable notifications to start keycode transmission!\n\r");
		#endif
				return false;
		}
}

static void mnf_data_init()
{
    mnf_data.ad_structure_size = sizeof(struct mnf_specific_data_ad_structure ) - sizeof(uint8_t); // minus the size of the ad_structure_size field
    mnf_data.ad_structure_type = GAP_AD_TYPE_MANU_SPECIFIC_DATA;
    mnf_data.company_id[0] = APP_AD_MSD_COMPANY_ID & 0xFF; // LSB
    mnf_data.company_id[1] = (APP_AD_MSD_COMPANY_ID >> 8 )& 0xFF; // MSB
    mnf_data.proprietary_data[0] = 0;
    mnf_data.proprietary_data[1] = 0;
}

/**
 ****************************************************************************************
 * @brief Update Manufacturer Specific Data
 * @return void
 ****************************************************************************************
 */
static void mnf_data_update()
{
    uint16_t data;

    data = mnf_data.proprietary_data[0] | (mnf_data.proprietary_data[1] << 8);
    data += 1;
    mnf_data.proprietary_data[0] = data & 0xFF;
    mnf_data.proprietary_data[1] = (data >> 8) & 0xFF;

    if (data == 0xFFFF) {
         mnf_data.proprietary_data[0] = 0;
         mnf_data.proprietary_data[1] = 0;
    }
}

#ifndef DIRECTED_ADV

/**
 ****************************************************************************************
 * @brief Add an AD structure in the Advertising or Scan Response Data of the 
  *       GAPM_START_ADVERTISE_CMD parameter struct.
 * @param[in] cmd               GAPM_START_ADVERTISE_CMD parameter struct
 * @param[in] ad_struct_data    AD structure buffer
 * @param[in] ad_struct_len     AD structure length
 * @param[in] adv_connectable   Connectable advertising event or not. It controls whether 
 *                              the advertising data use the full 31 bytes length or only 
 *                              28 bytes (Document CCSv6 - Part 1.3 Flags). 
 * @return void
 */
static void app_add_ad_struct(struct gapm_start_advertise_cmd *cmd, void *ad_struct_data, uint8_t ad_struct_len, uint8_t adv_connectable)
{
    uint8_t adv_data_max_size = (adv_connectable) ? (ADV_DATA_LEN - 3) : (ADV_DATA_LEN);
    
    if ((adv_data_max_size - cmd->info.host.adv_data_len) >= ad_struct_len)
    {
        // Append manufacturer data to advertising data
        memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len], ad_struct_data, ad_struct_len);

        // Update Advertising Data Length
        cmd->info.host.adv_data_len += ad_struct_len;
        
        // Store index of manufacturer data which are included in the advertising data
        mnf_data_index = cmd->info.host.adv_data_len - sizeof(struct mnf_specific_data_ad_structure);
    }
    else if ((SCAN_RSP_DATA_LEN - cmd->info.host.scan_rsp_data_len) >= ad_struct_len)
    {
        // Append manufacturer data to scan response data
        memcpy(&cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len], ad_struct_data, ad_struct_len);

        // Update Scan Response Data Length
        cmd->info.host.scan_rsp_data_len += ad_struct_len;
        
        // Store index of manufacturer data which are included in the scan response data
        mnf_data_index = cmd->info.host.scan_rsp_data_len - sizeof(struct mnf_specific_data_ad_structure);
        // Mark that manufacturer data is in scan response and not advertising data
        mnf_data_index |= 0x80;
    }
    else
    {
        // Manufacturer Specific Data do not fit in either Advertising Data or Scan Response Data
        ASSERT_WARNING(0);
    }
    // Store advertising data length
    stored_adv_data_len = cmd->info.host.adv_data_len;
    // Store advertising data
    memcpy(stored_adv_data, cmd->info.host.adv_data, stored_adv_data_len);
    // Store scan response data length
    stored_scan_rsp_data_len = cmd->info.host.scan_rsp_data_len;
    // Store scan_response data
    memcpy(stored_scan_rsp_data, cmd->info.host.scan_rsp_data, stored_scan_rsp_data_len);
}
#endif

/**
 ****************************************************************************************
 * @brief Parameter update request timer callback function.
 * @return void
 ****************************************************************************************
*/
static void param_update_request_timer_cb()
{
    app_easy_gap_param_update_start(app_connection_idx);
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
}

void user_app_init(void)
{
	  app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
	
		// Initialize user modules. Set module initialization functions in user_module_config structure 
    user_modules_init();
    
    // Initialize Manufacturer Specific Data
    mnf_data_init();
    
    // Initialize Advertising and Scan Response Data
    memcpy(stored_adv_data, USER_ADVERTISE_DATA, USER_ADVERTISE_DATA_LEN);
    stored_adv_data_len = USER_ADVERTISE_DATA_LEN;
    memcpy(stored_scan_rsp_data, USER_ADVERTISE_SCAN_RESPONSE_DATA, USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN);
    stored_scan_rsp_data_len = USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN;
    
    default_app_on_init();
	
		// register keys in buffer until connection is established
		app_kbd_start_reporting(); 
	
#ifdef CFG_PRINTF
		arch_printf("\n\r*****************************************************************************************"				);
		arch_printf("\n\r**  Initialisation complete.                                                             "				);
		arch_printf("\n\r*****************************************************************************************\n\r\n\r");
#endif
}

void user_app_adv_start(void)
{
#ifdef DIRECTED_ADV
		app_easy_gap_directed_advertise_get_active(0);
		app_easy_gap_directed_advertise_start(0);
#else
		struct gapm_start_advertise_cmd* cmd;
    
    cmd = app_easy_gap_undirected_advertise_get_active();
    
    // Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);

		// Starts advertising for 10 seconds. Button press will start the advertising cycle again.
    app_easy_gap_undirected_advertise_with_timeout_start(1000, NULL); 
#endif

#ifdef CFG_PRINTF
		arch_printf("**  Initiated undirect advertising\n\n\r");
#endif	
}

void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;
			
				// Stop the advertising timeout
				app_easy_gap_advertise_with_timeout_stop();

        // Check if the parameters of the established connection are the preferred ones.
        // If not then schedule a connection parameter update request.
        if ((param->con_interval < user_connection_param_conf.intv_min) ||
            (param->con_interval > user_connection_param_conf.intv_max) ||
            (param->con_latency != user_connection_param_conf.latency) ||
            (param->sup_to != user_connection_param_conf.time_out))
        {
            // Connection params are not these that we expect
            app_param_update_request_timer_used = app_easy_timer(APP_PARAM_UPDATE_REQUEST_TO, param_update_request_timer_cb);
        }
		#ifdef CFG_PRINTF
				arch_printf("**  Connection established.\n\n\r");;
		#endif
    }
    else
    {
        // No connection has been established, restart advertising
        user_app_adv_start();
    }
		default_app_on_connection(connection_idx, param); 
		
#ifdef CONNECTION_TIMEOUT
		user_con_timeout_init(CONNECTION_TIMEOUT_DELAY, connection_idx);
#endif
}

void user_app_adv_undirect_complete(uint8_t status)
{
    // If advertising was canceled then update advertising data and start advertising again
    if (status == GAP_ERR_CANCELED)
    {
        //user_app_adv_start();
    }
}

#ifdef DIRECTED_ADV

void user_app_adv_direct_complete(uint8_t status)
{
}

#endif //DIRECTED_ADV

void user_app_disconnect(struct gapc_disconnect_ind const *param)
{
		// change notifications to disabled
		user_set_notifications_disabled();
	
    // Cancel the parameter update request timer
    if (app_param_update_request_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_param_update_request_timer_used);
        app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    }
    // Update manufacturer data for the next advertsing event
    mnf_data_update();

		ke_state_set(TASK_APP, APP_CONNECTABLE);
		
		#ifdef LED_INDICATION
				update_led_status();
		#endif		
		
		#ifdef CONNECTION_TIMEOUT
				user_con_timeout_cancel();
		#endif
		
		#ifdef CFG_PRINTF
				arch_printf("**  Disconnected.\n\n\r");
		#endif
}

#ifdef HAS_KBD
void user_kbd_notification_cb(enum kbd_notification notification)
{
#ifdef CFG_PRINTF
		arch_printf("**  Keyboard event.\n\r");
#endif		
		// If notifications are enabled send keycode buffer contents
		if (user_are_notifications_enabled())
				user_sent_keycodes();
		
		// If a connection is active, restart the value of the connection timeout
		if (user_is_ble_connected())
				user_con_timeout_reset();
		
		// If no connection is active start advertising
		else
				user_app_adv_start();

}
#endif // HAS_KBD

arch_main_loop_callback_ret_t user_on_system_powered(void)		
{
		arch_main_loop_callback_ret_t ret;
    uint8_t power_status;

    // Perform actions needed by modules while BLE is powered.
    // Set module functions in user_module_config structure
    // Modules can block sleep by returning APP_GOTO_SLEEP or 
    // force BLE to wake up by returning APP_BLE_WAKEUP
    power_status = user_modules_on_system_powered();
    
    ret = (power_status == APP_KEEP_POWERED) ? KEEP_POWERED : GOTO_SLEEP;
    
    if ((power_status & APP_BLE_WAKEUP) != 0 && user_is_ble_connected()) {
        if(GetBits16(CLK_RADIO_REG, BLE_ENABLE) == 0) {
            // If BLE is sleeping, wake it up!
            ret = arch_ble_force_wakeup() ? KEEP_POWERED : GOTO_SLEEP;    
        }
        else {
            ret = KEEP_POWERED;
        }
    }
       
    if(ret == KEEP_POWERED) {
        wdg_reload(WATCHDOG_DEFAULT_PERIOD);
    }
       
    return ret;
}	

arch_main_loop_callback_ret_t user_on_ble_powered(void)
{
#ifdef LED_INDICATION
		// Update the status of the advertising indication led
		update_led_status();
#endif
	
		arch_main_loop_callback_ret_t ret;
    
    // Perform actions needed by modules while BLE is powered.
    // Set module functions in user_module_config structure.
    // Modules can block sleep by returning true
    ret = user_modules_on_ble_powered() ? KEEP_POWERED : GOTO_SLEEP;
                   
    if(ret == KEEP_POWERED)
        wdg_reload(WATCHDOG_DEFAULT_PERIOD);
    
		return ret;
}

void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msgid)
    {
        case CUSTS1_VAL_WRITE_IND:
        {
            struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

            switch (msg_param->handle)
            {
								case SVC1_IDX_KBD_DATA_DATA_NTF_CFG:
									
										if (user_are_notifications_enabled())
										{
												user_set_notifications_disabled();
												break;
										}
										else
										{
												user_set_notifications_enabled();
												user_sent_keycodes();
												break;
										}
                default:
                    break;
            }
        } break;		

        case GAPC_PARAM_UPDATED_IND:
        {
            // Cast the "param" pointer to the appropriate message structure
            struct gapc_param_updated_ind const *msg_param = (struct gapc_param_updated_ind const *)(param);

            // Check if updated Conn Params filled to preferred ones
            if ((msg_param->con_interval >= user_connection_param_conf.intv_min) &&
                (msg_param->con_interval <= user_connection_param_conf.intv_max) &&
                (msg_param->con_latency == user_connection_param_conf.latency) &&
                (msg_param->sup_to == user_connection_param_conf.time_out))
            {
								
            }
        } break;

        default:
            break;
    }
}
/// @} APP
