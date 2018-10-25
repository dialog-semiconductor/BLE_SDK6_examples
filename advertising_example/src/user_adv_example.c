/**
 ****************************************************************************************
 *
 * @file user_adv_example.c
 *
 * @brief Barebone project source code.
 *
 * Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs 
 * included in this package ("Software") include confidential, proprietary information 
 * of Dialog Semiconductor. All Rights Reserved.
 * 
 * THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR (‘DIALOG’) AND MAY
 * ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND. THIS 
 * SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS. RECIPIENT 
 * SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT DIALOG’S PRIOR 
 * WRITTEN PERMISSION.
 * 
 * UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT 
 * TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”, WITHOUT 
 * WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT 
 * LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, 
 * MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM 
 * COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS
 * AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY 
 * REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED 
 * SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, 
 * AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY 
 * OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED 
 * BY THE FCC OR OTHER LIKE AGENCIES.
 * 
 * IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
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
#include "user_adv_example.h"
#include "co_bt.h"
#include "user_periph_setup.h"
#include "wkupct_quadec.h"
#include "gpio.h"
#include "user_button.h"
#include "rwble.h"
#include "arch_console.h"


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

advertising_state adv_state = UNDIRECT_ADVERTISING;
extern last_ble_evt arch_rwble_last_event;

uint8_t app_connection_idx                      __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
timer_hnd app_adv_data_update_timer_used        __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
timer_hnd app_param_update_request_timer_used   __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY

// Retained variables
struct mnf_specific_data_ad_structure mnf_data  __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
// Index of manufacturer data in advertising data or scan response data (when MSB is 1)
uint8_t mnf_data_index                          __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_adv_data_len                     __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data_len                __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_adv_data[ADV_DATA_LEN]           __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data[SCAN_RSP_DATA_LEN] __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY

// Timer example function declaration
#ifdef ADV_TIMER_EXAMPLE
		static void user_resume_from_sleep(void);
#endif //ADV_TIMER_EXAMPLE 

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief Initialize Manufacturer Specific Data
 * @return void
 ****************************************************************************************
 */
void mnf_data_init()
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
void app_add_ad_struct(struct gapm_start_advertise_cmd *cmd, void *ad_struct_data, uint8_t ad_struct_len, uint8_t adv_connectable)
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

/**
 ****************************************************************************************
 * @brief Advertisement data update timer callback function.
 * @return void
 ****************************************************************************************
*/
void adv_data_update_timer_cb()
{
    // If mnd_data_index has MSB set, manufacturer data is stored in scan response
    uint8_t *mnf_data_storage = (mnf_data_index & 0x80) ? stored_scan_rsp_data : stored_adv_data;

    // Update manufacturer data
    mnf_data_update();

    // Update the selected fields of the advertising data (manufacturer data)
    memcpy(mnf_data_storage + (mnf_data_index & 0x7F), &mnf_data, sizeof(struct mnf_specific_data_ad_structure));

    // Update advertising data on the fly
    app_easy_gap_update_adv_data(stored_adv_data, stored_adv_data_len, stored_scan_rsp_data, stored_scan_rsp_data_len);
    
    // Stop advertising air operation - a timer will wake-up the system
    app_easy_gap_advertise_stop();
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
		#ifdef ADV_BUTTON_EXAMPLE
				user_init_button_interrupt(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN);
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
		// check state to start the correct advertising mode or go to / remain in sleep
				switch (adv_state)
				{
						case UNDIRECT_ADVERTISING:
								user_undirect_adv_start();
								break;
						case NONCON_ADVERTISING:
								user_noncon_adv_start();
								break;
						case SLEEP:
								user_enable_extended_sleep();
								break;
						default :
								user_undirect_adv_start();
				}
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
            (param->con_latency != user_connection_param_conf.latency) ||
            (param->sup_to != user_connection_param_conf.time_out))
        {
            // Connection params are not these that we expect
            app_param_update_request_timer_used = app_easy_timer(APP_PARAM_UPDATE_REQUEST_TO, param_update_request_timer_cb);
        }
    }
    else
    {
        // No connection has been established, restart advertising
				#ifdef ADV_TIMER_EXAMPLE
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
		#ifdef ADV_TIMER_EXAMPLE
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
		#ifdef ADV_TIMER_EXAMPLE
				user_change_adv_state(SLEEP);
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
		// Schedule the next advertising data update
    app_adv_data_update_timer_used = app_easy_timer(UNDIRECT_ADV_DURATION, adv_data_update_timer_cb);
    
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
		// Schedule the next advertising data update
    app_adv_data_update_timer_used = app_easy_timer(NONCONN_ADV_DURATION, adv_data_update_timer_cb);
	
		struct gapm_start_advertise_cmd* cmd;
		cmd = app_easy_gap_non_connectable_advertise_get_active();
		
		cmd->intv_min = MS_TO_BLESLOTS(NONCONN_ADV_INTERVAL);
		cmd->intv_max = MS_TO_BLESLOTS(NONCONN_ADV_INTERVAL);
	
		// Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);
	
		app_easy_gap_non_connectable_advertise_start();
}

/**
 ****************************************************************************************
 * @brief Callback checking BLE event status to toggle LED
 * @return void
 ****************************************************************************************
*/
arch_main_loop_callback_ret_t user_ble_powered_on(void)
{
		#ifdef LED_INDICATION
				// If BLE event has ended turn of the LED
				if(arch_rwble_last_event == BLE_EVT_END)
				{
						GPIO_SetInactive(GPIO_LED_PORT, GPIO_LED_PIN);
				}
				// If BLE is transmitting turn on the LED
				if(arch_rwble_last_event == BLE_EVT_TX)
				{
						GPIO_SetActive(GPIO_LED_PORT, GPIO_LED_PIN);				
				}
				
		#endif
		return GOTO_SLEEP;
}

/**
 ****************************************************************************************
 * @brief Callback when waking op the system to start advertising
 * @return void
 ****************************************************************************************
*/
void user_resume_from_sleep(void)
{
    arch_disable_sleep();
    if (GetBits16(SYS_STAT_REG, PER_IS_DOWN))
    {
         periph_init();
    }
		user_change_adv_state(UNDIRECT_ADVERTISING);
		user_app_adv_start();
}

/**
 ****************************************************************************************
 * @brief Send the device to sleep.
 * @return void
 ****************************************************************************************
*/                   
void user_enable_extended_sleep(void)
{
		arch_set_sleep_mode(ARCH_EXT_SLEEP_ON);
    arch_set_extended_sleep(false);
		#ifdef ADV_TIMER_EXAMPLE
				app_easy_timer(SLEEP_DURATION, user_resume_from_sleep);
		#endif
}

/**
 ****************************************************************************************
 * @brief Function to set the advertising state
					[Optional] If CFG_PRINTF is defined the advertising state will be send to
					a UART serial terminal (i.e. Tera Term).
 * @return void
 ****************************************************************************************
*/
void user_change_adv_state(advertising_state state)
{
	switch (state)
	{
			case UNDIRECT_ADVERTISING:
					adv_state = UNDIRECT_ADVERTISING;
					#ifdef CFG_PRINTF
							arch_printf("\n\n\rAdvertising state changed to undirected advertising...");
					#endif
					break;
			case NONCON_ADVERTISING:
					adv_state = NONCON_ADVERTISING;
					#ifdef CFG_PRINTF
							arch_printf("\n\n\rAdvertising state changed to nonconnectable advertising...");
					#endif
					break;
			case SLEEP:
					adv_state = SLEEP;
					#ifdef CFG_PRINTF
							arch_printf("\n\n\rAdvertising turned off, system going to sleep...");
					#endif
					break;
			default :
					adv_state = NONCON_ADVERTISING;
					#ifdef CFG_PRINTF
							arch_printf("\n\n\rAdvertising state changed to undirected advertising...");
					#endif
	}	
}
#endif //ADV_EXAMPLE

/// @} APP
