/**
 ****************************************************************************************
 *
 * @file user_application.c
 *
 * @brief Detect the source of the reset project source code.
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
#include "user_application.h"
#include "co_bt.h"
#include "custs1_task.h"
#include "user_custs1_def.h"
#include "user_custs1_impl.h"
#include "gpio.h"
#include "wkupct_quadec.h"
#include "user_periph_setup.h"

#ifdef CFG_PRINTF
    #include "arch_console.h"
#endif


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint8_t app_connection_idx                      __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
timer_hnd app_adv_data_update_timer_used        __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
timer_hnd app_param_update_request_timer_used   __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY

// Retained variables

// Index of manufacturer data in advertising data or scan response data (when MSB is 1)
uint8_t mnf_data_index                          __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_adv_data_len                     __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data_len                __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_adv_data[ADV_DATA_LEN]           __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data[SCAN_RSP_DATA_LEN] __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY

/*Uninitialized RetRAM data*/

//Unitialized data into Retention RAM
uint8_t uninit_store_data[USER_RETRAM_DATA_LEN] __attribute__((section("retention_mem_area_uninit") , zero_init));  //@RETENTION MEMORY

//Flag into the Retention RAM to detect the source of the reset
uint8_t detect_rst_flag		__attribute__((section("retention_mem_area_uninit"),  zero_init)); 

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/


static void ret_ram_data_init(void)
{
	//Copy the USER_RETRAM_DATA into the RetRAM
	memcpy(uninit_store_data, USER_RETRAM_DATA ,USER_RETRAM_DATA_LEN );
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



/**
 ****************************************************************************************
 * @brief Explicitly cause power on reset upon syestem initialization
 * @return void
 ****************************************************************************************
*/

void user_power_on_reset(void)
{
	if (GPIO_GetPorTime() == 0x18)	// Check for POR_TIMER_REG default value
    {
      GPIO_SetPorTime(0x19);		// Change value to other than default
			
			detect_rst_flag = CUSTS1_POR;

			//Restart advertising after the power on reset
			user_app_adv_start();
    }

}

void user_app_init(void)
{
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    		  
    // Initialize Advertising and Scan Response Data
    memcpy(stored_adv_data, USER_ADVERTISE_DATA, USER_ADVERTISE_DATA_LEN);
    stored_adv_data_len = USER_ADVERTISE_DATA_LEN;
    memcpy(stored_scan_rsp_data, USER_ADVERTISE_SCAN_RESPONSE_DATA, USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN);
    stored_scan_rsp_data_len = USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN;
	
		//Store the user's data into the RetRAM
    ret_ram_data_init();
	
		#ifdef CFG_POWER_ON_RESET
			//User function for power-on-reset
			user_power_on_reset();
		#endif
	
    default_app_on_init();
}

static void append_data(uint8_t *len,
                               const uint8_t name_length,
                               uint8_t *data,
                               const void *name_data)
{
    // Fill Length
    data[0] = name_length + 1;

    // Fill Device Name Flag
    data[1] = GAP_AD_TYPE_MANU_SPECIFIC_DATA;
	
    memcpy(&data[2], name_data, name_length);

    // Update advertising or scan response data length
    *len += name_length + 2;
}

void user_app_update_adv_data(void)
{
	
}

void user_app_adv_start(void)
{
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();
	
		switch(detect_rst_flag)
    {
			
			case CUSTS1_HARDFAULT:
				{
						append_data(&cmd->info.host.adv_data_len,
											APP_ADV_HARDFAULT_LEN,
											&(cmd->info.host.adv_data[cmd->info.host.adv_data_len]),
											APP_ADV_HARDFAULT);
					
						#ifdef CFG_PRINTF
									arch_printf("Source of reset : Hardfault\r\n");
						#endif
					
				}break;
			
        case CUSTS1_NMI:
        {
						append_data(&cmd->info.host.adv_data_len,
									APP_ADV_NMI_LEN,
									&(cmd->info.host.adv_data[cmd->info.host.adv_data_len]),
									APP_ADV_NMI);
					
						#ifdef CFG_PRINTF
									arch_printf("Source of reset : NMI\r\n");
						#endif
					
        } break;
				case CUSTS1_SW_RESET:
				{
						append_data(&cmd->info.host.adv_data_len,
									APP_ADV_NMI_LEN,
									&(cmd->info.host.adv_data[cmd->info.host.adv_data_len]),
									APP_ADV_NMI);
					
						#ifdef CFG_PRINTF
									arch_printf("Source of reset : Software Reset\r\n");
						#endif
					
				}break;
			
        case CUSTS1_POR:
        {
							append_data(&cmd->info.host.adv_data_len,
										APP_ADV_POR_LEN,
										&(cmd->info.host.adv_data[cmd->info.host.adv_data_len]),
										APP_ADV_POR);
							
						#ifdef CFG_PRINTF
									arch_printf("Source of reset : Power-On-Reset\r\n");
						#endif
					
        } break;

        default:
            break;
    }

    app_easy_gap_undirected_advertise_start();
}

void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;

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
        user_app_adv_start();
    }

    default_app_on_connection(connection_idx, param);
}

void user_app_adv_undirect_complete(uint8_t status)
{
    // If advertising was canceled then update advertising data and start advertising again
    if (status == GAP_ERR_CANCELED)
    {
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

    // Restart Advertising
    user_app_adv_start();
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
                case SVC1_IDX_CONTROL_POINT_VAL:
                    user_svc1_ctrl_wr_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                default:
                    break;
            }
				}break;
			
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
