/**
 ****************************************************************************************
 *
 * @file user_peripheral.c
 *
 * @brief Peripheral project source code.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration
#include "gap.h"
#include "app_easy_timer.h"
#include "user_peripheral.h"
#include "user_custs1_def.h"
#include "custs1_task.h"
#include "co_bt.h"

#if BLE_HID_DEVICE

#include "l2cm.h"
#include "ke_event.h"
#include "user_gamepad.h"
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
timer_hnd app_param_update_request_timer_used   __SECTION_ZERO("retention_mem_area0");


uint8_t app_connection_idx                      __SECTION_ZERO("retention_mem_area0");


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/


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
    default_app_on_init();
		user_gamepad_init();
}

void user_app_on_db_init_complete(void){
#if (CFG_USE_JOYSTICKS)
	user_gamepad_toggle_axis_polling(true);
#endif
	default_app_on_db_init_complete();
}

void user_app_adv_start(void)
{
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

void user_custs1_server_rx_ind_handler(ke_msg_id_t const msgid,
                                     struct custs1_val_write_ind const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
		if(param->length == sizeof(struct gamepad_digitizer_config_pack_t)){ //apply inconming 16-byte config, no validation
			struct gamepad_digitizer_config_pack_t* config = (struct gamepad_digitizer_config_pack_t*)param->value;
			user_gamepad_set_button_location(BUTTON_STA, config->button_map[0]);
			user_gamepad_set_button_location(BUTTON_SEL, config->button_map[1]);
			user_gamepad_set_button_location(BUTTON_A, config->button_map[2]);
			user_gamepad_set_button_location(BUTTON_B, config->button_map[3]);
	
			user_gamepad_config_joystick(&LS_config, config->joystick_map[0]);
			user_gamepad_config_joystick(&RS_config, config->joystick_map[1]);
		}
		else{
			user_gamepad_config_digitizer(); //resets config
		}
}

void user_custs1_server_tx_cfg_ind_handler(ke_msg_id_t const msgid,
                                            struct custs1_val_write_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
	//CCC value already handled in cust1 task 
}

void user_custs1_server_tx_ntf_cfm_handler(ke_msg_id_t const msgid,
                                            struct custs1_val_ntf_cfm const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{

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
                case CUST1_IDX_SERVER_RX_VAL:
                    user_custs1_server_rx_ind_handler(msgid, msg_param, dest_id, src_id);
									 break;
                case CUST1_IDX_SERVER_TX_NTF_CFG:
                    user_custs1_server_tx_cfg_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

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
					#if BLE_HID_DEVICE
            app_hid_gamepad_event_handler(msgid, param, dest_id, src_id);
					#endif // BLE_HID_DEVICE
            break;
    }
}

/// @} APP