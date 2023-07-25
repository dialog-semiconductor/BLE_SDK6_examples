/**
 ****************************************************************************************
 *
 * @file user_barebone.c
 *
 * @brief Barebone project source code.
 *
 * Copyright (c) 2015-2021 Dialog Semiconductor
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.E.
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
#include "user_barebone.h"
#include "co_bt.h"
#include "user_button.h"
#include "uart_utils.h"
#include "user_periph_setup.h"

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
extern advertising_state adv_state;
bool is_led_active                              __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t app_connection_idx                      __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd app_adv_data_update_timer_used        __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd app_param_update_request_timer_used   __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

// Retained variables
struct mnf_specific_data_ad_structure mnf_data  __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
// Index of manufacturer data in advertising data or scan response data (when MSB is 1)
uint8_t mnf_data_index                          __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_adv_data_len                     __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data_len                __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_adv_data[ADV_DATA_LEN]           __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data[SCAN_RSP_DATA_LEN] __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

// Timer example function declaration
#if defined(ADV_EXAMPLE) && !defined(ADV_BUTTON_EXAMPLE)
static void user_resume_from_sleep(void);
#endif 

/**
 ****************************************************************************************
 * @brief Initialize Manufacturer Specific Data
 * @return void
 ****************************************************************************************
 */
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
    data++;
    mnf_data.proprietary_data[0] = data & 0xFF;
    mnf_data.proprietary_data[1] = (data >> 8) & 0xFF;

    if (data == 0xFFFF) {
         mnf_data.proprietary_data[0] = 0;
         mnf_data.proprietary_data[1] = 0;
    }
}

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

void toggle_led(void)
{
    if(is_led_active)
    {
        GPIO_SetInactive(LED_PORT, LED_PIN);
        is_led_active = false;
    }
    else
    {
        GPIO_SetActive(LED_PORT, LED_PIN);
        is_led_active = true;
    }
}

/**
 ****************************************************************************************
 * @brief Advertisement data update timer callback function.
 * @return void
 ****************************************************************************************
*/
static void adv_data_update_timer_cb()
{	
    // If mnd_data_index has MSB set, manufacturer data is stored in scan response
    uint8_t *mnf_data_storage = (mnf_data_index & 0x80) ? stored_scan_rsp_data : stored_adv_data;

    // Update manufacturer data
    mnf_data_update();

    // Update the selected fields of the advertising data (manufacturer data)
    memcpy(mnf_data_storage + (mnf_data_index & 0x7F), &mnf_data, sizeof(struct mnf_specific_data_ad_structure));

#if !defined(ADV_EXAMPLE)
    // Update advertising data on the fly
    app_easy_gap_update_adv_data(stored_adv_data, stored_adv_data_len, stored_scan_rsp_data, stored_scan_rsp_data_len);
    app_adv_data_update_timer_used = app_easy_timer(APP_ADV_DATA_UPDATE_TO, adv_data_update_timer_cb);
#endif
    
#if defined(ADV_EXAMPLE) && !defined(ADV_BUTTON_EXAMPLE)
    app_easy_gap_advertise_stop();
#endif
}

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
    adv_state = UNDIRECT_ADVERTISING;
    
#ifdef ADV_BUTTON_EXAMPLE
    user_button_enable(GPIO_SW_PORT, GPIO_SW_PIN);
#endif
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    
    // Initialize Manufacturer Specific Data
    mnf_data_init();
    
    // Initialize Advertising and Scan Response Data
    memcpy(stored_adv_data, USER_ADVERTISE_DATA, USER_ADVERTISE_DATA_LEN);
    stored_adv_data_len = USER_ADVERTISE_DATA_LEN;
    memcpy(stored_scan_rsp_data, USER_ADVERTISE_SCAN_RESPONSE_DATA, USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN);
    stored_scan_rsp_data_len = USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN;
    
    default_app_on_init();
}

void user_app_adv_start(void)
{
#ifndef ADV_EXAMPLE
    // Schedule the next advertising data update
    app_adv_data_update_timer_used = app_easy_timer(APP_ADV_DATA_UPDATE_TO, adv_data_update_timer_cb);

    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();

    // Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);

    app_easy_gap_undirected_advertise_start();
#else
    user_start_advertising();
#endif
}

void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;

        // Stop the advertising data update timer
        app_easy_timer_cancel(app_adv_data_update_timer_used);

        // Check if the parameters of the established connection are the preferred ones.
        // If not then schedule a connection parameter update request.
        if ((param->con_interval < user_connection_param_conf.intv_min) ||
            (param->con_interval > user_connection_param_conf.intv_max) ||
            (param->con_latency != user_connection_param_conf.latency)  ||
            (param->sup_to != user_connection_param_conf.time_out))
        {
            // Connection params are not these that we expect
            app_param_update_request_timer_used = app_easy_timer(APP_PARAM_UPDATE_REQUEST_TO, param_update_request_timer_cb);
        }
    }
    else
    {
        // No connection has been established, restart advertising
#if defined(ADV_EXAMPLE) && !defined(ADV_BUTTON_EXAMPLE)
        user_change_adv_state(UNDIRECT_ADVERTISING);
#endif
        user_app_adv_start();
    }

    default_app_on_connection(connection_idx, param);
}

void user_app_adv_undirect_complete(uint8_t status)
{
	
    // If advertising was canceled then update advertising data and start advertising again
    if (status == GAP_ERR_CANCELED)
    {
#if defined(ADV_EXAMPLE) && !defined(ADV_BUTTON_EXAMPLE)
        user_change_adv_state(NONCON_ADVERTISING);
#endif
        user_app_adv_start();
    }
}

void user_app_adv_nonconn_complete(uint8_t status)
{	
	// If advertising was canceled then update advertising data and start advertising again
    if (status == GAP_ERR_CANCELED)
    {
#if defined(ADV_EXAMPLE) && !defined(ADV_BUTTON_EXAMPLE)
        user_change_adv_state(PERMANENT_SLEEP);
#endif
		user_app_adv_start();
    }
}

void user_app_disconnect(struct gapc_disconnect_ind const *param)
{
    // Cancel the parameter update request timer
    if (app_param_update_request_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_param_update_request_timer_used);
        app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    }
    // Update manufacturer data for the next advertsing event
    mnf_data_update();
    
    // Restart Advertising
#ifdef ADV_EXAMPLE
    user_change_adv_state(UNDIRECT_ADVERTISING);
#endif
		
    user_app_adv_start();
}

void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msgid)
    {
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

#ifdef ADV_EXAMPLE

/**
 ****************************************************************************************
 * @brief Start undirected advertising                
 * @return void
 ****************************************************************************************
*/ 
void user_undirect_adv_start(void)
{
#if !defined(ADV_BUTTON_EXAMPLE)
    // Schedule the next advertising data update
    app_adv_data_update_timer_used = app_easy_timer(UNDIRECT_ADV_DURATION, adv_data_update_timer_cb);
#else
    adv_data_update_timer_cb();
#endif
    
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();
    
    cmd->intv_min = MS_TO_BLESLOTS(UNDIRECT_ADV_INTERVAL);
    cmd->intv_max = MS_TO_BLESLOTS(UNDIRECT_ADV_INTERVAL);
		
    // Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);

    app_easy_gap_undirected_advertise_start();
}

/**
 ****************************************************************************************
 * @brief start non-connectible advertising
 * @return void
 ****************************************************************************************
*/ 
void user_noncon_adv_start(void)
{
#if !defined(ADV_BUTTON_EXAMPLE)
	// Schedule the next advertising data update
    app_adv_data_update_timer_used = app_easy_timer(NONCONN_ADV_DURATION, adv_data_update_timer_cb);
#else
    adv_data_update_timer_cb();
#endif
	
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_non_connectable_advertise_get_active();
    
    cmd->intv_min = MS_TO_BLESLOTS(NONCONN_ADV_INTERVAL);
    cmd->intv_max = MS_TO_BLESLOTS(NONCONN_ADV_INTERVAL);
	
	// Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);
	
	app_easy_gap_non_connectable_advertise_start();
}

#if defined(ADV_EXAMPLE) && !defined(ADV_BUTTON_EXAMPLE)
/**
 ****************************************************************************************
 * @brief Callback when waking op the system to start advertising
 * @return void
 ****************************************************************************************
*/
void user_resume_from_sleep(void)
{		
#if !defined(__DA14531__)
    if (GetBits16(SYS_STAT_REG, PER_IS_DOWN))
    {
             periph_init();
    }
#endif
		
    user_change_adv_state(UNDIRECT_ADVERTISING);
    user_app_adv_start();
}
#endif

/**
 ****************************************************************************************
 * @brief Send the device to sleep.
 * @return void
 ****************************************************************************************
*/                   
void user_enable_permanent_extended_sleep(void)
{
	arch_set_sleep_mode(ARCH_EXT_SLEEP_ON);
    GPIO_SetInactive(LED_PORT, LED_PIN);
    is_led_active = false;
#if defined(ADV_EXAMPLE) && !defined(ADV_BUTTON_EXAMPLE) 
    app_easy_timer(SLEEP_DURATION, user_resume_from_sleep);
#else
    arch_ble_ext_wakeup_on();
#endif
}

void user_change_adv_state(advertising_state state)
{
    adv_state = state;
    
#ifdef CFG_PRINTF
    if(adv_state == UNDIRECT_ADVERTISING)
        printf_string(UART2,"\n\n\rAdvertising state changed to undirected advertising...");
    else if (adv_state == NONCON_ADVERTISING)
        printf_string(UART2,"\n\n\rAdvertising state changed to nonconnectable advertising...");
    else if (adv_state == PERMANENT_SLEEP)
        printf_string(UART2,"\n\n\rAdvertising turned off, system going to sleep...");
#endif //CFG_PRINTF
    
    app_easy_gap_advertise_stop();
}

void user_start_advertising(void)
{
    switch(adv_state)
    {
        case UNDIRECT_ADVERTISING:
            toggle_led();
            user_undirect_adv_start();
            break;
        case NONCON_ADVERTISING:
            toggle_led();
            user_noncon_adv_start();
            break;
        case PERMANENT_SLEEP:
            user_enable_permanent_extended_sleep();
            break;
        default:
            user_noncon_adv_start();
            break;
    }
}

#endif //ADV_EXAMPLE



/// @} APP
