/**
 ****************************************************************************************
 *
 * @file user_active_scanner.c
 *
 * @brief Active scanner project source code.
 *
 * Copyright (c) 2012-2020 Dialog Semiconductor. All rights reserved.
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
#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_active_scanner.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

struct scan_configuration {
    /// Operation code.
    uint8_t     code;
    /// Own BD address source of the device
    uint8_t     addr_src;
    /// Scan interval
    uint16_t    interval;
    /// Scan window size
    uint16_t    window;
    /// Scanning mode
    uint8_t     mode;
    /// Scan filter policy
    uint8_t     filt_policy;
    /// Scan duplicate filtering policy
    uint8_t     filter_duplic;
};

static const struct scan_configuration user_scan_conf ={
    /// Operation code.
    .code = GAPM_SCAN_ACTIVE,
    /// Own BD address source of the device
    .addr_src = GAPM_CFG_ADDR_PUBLIC,
    /// Scan interval
    .interval = 10,
    /// Scan window size
    .window = 5,
    /// Scanning mode
    .mode = GAP_GEN_DISCOVERY,
    /// Scan filter policy
    .filt_policy = SCAN_ALLOW_ADV_ALL,
    /// Scan duplicate filtering policy
    .filter_duplic = SCAN_FILT_DUPLIC_EN
};

void user_scan_start( void );



void user_app_on_init(void)
{

    // Set sleep mode
    arch_set_sleep_mode(app_default_sleep_mode);
  
}

/**
 ****************************************************************************************
 * @brief Called upon device configuration complete message
 * @return void
 ****************************************************************************************
 */
void user_on_set_dev_config_complete( void )
{
		// start the scanning
		arch_printf( "Dev config complete\r\n");
		user_scan_start();
}



/**
 ****************************************************************************************
 * @brief Scan function
 * @return void
 ****************************************************************************************
 */
void user_scan_start(void)
{
    struct gapm_start_scan_cmd* cmd = KE_MSG_ALLOC(GAPM_START_SCAN_CMD,
                            TASK_GAPM, TASK_APP,
                            gapm_start_scan_cmd);

    cmd->op.code = user_scan_conf.code;
    cmd->op.addr_src = user_scan_conf.addr_src;
    cmd->interval = user_scan_conf.interval;
    cmd->window = user_scan_conf.window;
    cmd->mode = user_scan_conf.mode;
    cmd->filt_policy = user_scan_conf.filt_policy;
    cmd->filter_duplic = user_scan_conf.filter_duplic;

    // Send the message
    ke_msg_send(cmd);

    // We are now connectable
    ke_state_set(TASK_APP, APP_CONNECTABLE);
    
		arch_printf( "SCAN START\r\n");
    
}

/**
 ****************************************************************************************
 * @brief Scan complete function. Restart the scanning
 * @return void
 ****************************************************************************************
 */
void user_on_scan_complete(const uint8_t param){
    arch_printf( "SCAN COMPLETE\r\n");
		user_scan_start();
}

/**
 ****************************************************************************************
 * @brief Advertising report function. Decode and display most popular advertising field
 * @return void
 ****************************************************************************************
 */
void user_adv_report_ind (struct gapm_adv_report_ind const * param ) {
	uint8_t ad_len,index=0;
	
	// report the bluetooth device address
	arch_printf( "[%02x:%02x:%02x:",  (int)param->report.adv_addr.addr[5], (int)param->report.adv_addr.addr[4], (int)param->report.adv_addr.addr[3] );
  arch_printf( "%02x:%02x:%02x] ", (int)param->report.adv_addr.addr[2], (int)param->report.adv_addr.addr[1], (int)param->report.adv_addr.addr[0] );
		
	// report the advertising payload length
	arch_printf("LEN %d\r\n",param->report.data_len);
		
	// loop through the advertising data information
	while( index < param->report.data_len) {
		
		// extract field length
		ad_len = param->report.data[index];
		arch_printf( "  - ");
		
		// decode field type
		if(param->report.data[index + 1 ] == 0x9) {                // Device name
			arch_printf( "Name: ", param->report.data[index + 1 ]);
			for(int i= index + 2; i < index + ad_len + 1; i++) {
				arch_printf("%c", param->report.data[i]);
			}
		} else if(param->report.data[index + 1 ] == 0x1) {        // Device flags
			arch_printf( "Flags: ", param->report.data[index + 1 ]);
			for(int i= index + 2; i < index + ad_len + 1; i++) {
				arch_printf("%X ", param->report.data[i]);
			}
		} else if(param->report.data[index + 1 ] == 0x3 || param->report.data[index + 1 ] == 0x2) { // Services 16 bits UUID
			arch_printf( "Services ID: ", param->report.data[index + 1 ]);
			for(int i= index + 2; i < index + ad_len + 1; i+=2) {
				arch_printf("0x%X%X, ", param->report.data[i+1],param->report.data[i]);
			}
		} else if(param->report.data[index + 1 ] == 0x7) {       // Services 128 bits UUID
			arch_printf( "128 Bits UUID: ", param->report.data[index + 1 ]);
			for(int i= index + 2; i < index + ad_len + 1; i++) {
				arch_printf("%X ", param->report.data[i]);
			}
		} else if(param->report.data[index + 1 ] == 0x15) {     // Services solicitation 128 bits UUID
			arch_printf( "128 Bits solicitation UUID: ", param->report.data[index + 1 ]);
			for(int i= index + 2; i < index + ad_len + 1; i++) {
				arch_printf("%X ", param->report.data[i]);
			}
		} else if(param->report.data[index + 1 ] == 0xFF) {    // Manufacturer specific data
			arch_printf( "Manufacture Specific: ", param->report.data[index + 1 ]);
			for(int i= index + 2; i < index + ad_len + 1; i++) {
				arch_printf("%X ", param->report.data[i]);
			}
		} else {                                              // not decoded raw data
			arch_printf( "%X:", param->report.data[index + 1 ]);
			for(int i= index + 2; i < index + ad_len + 1; i++) {
				arch_printf("%X ", param->report.data[i]);
			}
		}
		
		arch_printf( "\r\n");
		index += ad_len + 1;
	} 
    
}

//void user_catch_rest_hndl(ke_msg_id_t const msgid,
//                          void const *param,
//                          ke_task_id_t const dest_id,
//                          ke_task_id_t const src_id)
//{
//    switch(msgid)
//    {
//        case CUSTS1_VAL_WRITE_IND:
//        {
//            struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

//            switch (msg_param->handle)
//            {
//                case SVC1_IDX_CONTROL_POINT_VAL:
//                    user_svc1_ctrl_wr_ind_handler(msgid, msg_param, dest_id, src_id);
//                    break;

//                case SVC1_IDX_LED_STATE_VAL:
//                    user_svc1_led_wr_ind_handler(msgid, msg_param, dest_id, src_id);
//                    break;

//                case SVC1_IDX_ADC_VAL_1_NTF_CFG:
//                    user_svc1_adc_val_1_cfg_ind_handler(msgid, msg_param, dest_id, src_id);
//                    break;

//                case SVC1_IDX_INDICATEABLE_IND_CFG:
//                    user_svc1_long_val_cfg_ind_handler(msgid, msg_param, dest_id, src_id);
//                    break;

//                case SVC1_IDX_LONG_VALUE_NTF_CFG:
//                    user_svc1_long_val_cfg_ind_handler(msgid, msg_param, dest_id, src_id);
//                    break;

//                case SVC1_IDX_LONG_VALUE_VAL:
//                    user_svc1_long_val_wr_ind_handler(msgid, msg_param, dest_id, src_id);
//                    break;
//                
//                case SVC2_WRITE_1_VAL:
//                    user_svc2_write_1_val_wr_ind_handler(msgid, msg_param, dest_id, src_id);
//                    break;

//                default:
//                    break;
//            }
//        } break;

//        case CUSTS1_VAL_NTF_CFM:
//        {
//            struct custs1_val_ntf_cfm const *msg_param = (struct custs1_val_ntf_cfm const *)(param);

//            switch (msg_param->handle)
//            {
//                case SVC1_IDX_ADC_VAL_1_VAL:
//                    break;

//                case SVC1_IDX_LONG_VALUE_VAL:
//                    break;

//                default:
//                    break;
//            }
//        } break;

//        case CUSTS1_VAL_IND_CFM:
//        {
//            struct custs1_val_ind_cfm const *msg_param = (struct custs1_val_ind_cfm const *)(param);

//            switch (msg_param->handle)
//            {
//                case SVC1_IDX_INDICATEABLE_VAL:
//                    break;

//                default:
//                    break;
//             }
//        } break;

//        case CUSTS1_ATT_INFO_REQ:
//        {
//            struct custs1_att_info_req const *msg_param = (struct custs1_att_info_req const *)param;

//            switch (msg_param->att_idx)
//            {
//                case SVC1_IDX_LONG_VALUE_VAL:
//                    user_svc1_long_val_att_info_req_handler(msgid, msg_param, dest_id, src_id);
//                    break;

//                default:
//                    user_svc1_rest_att_info_req_handler(msgid, msg_param, dest_id, src_id);
//                    break;
//             }
//        } break;

//        case GAPC_PARAM_UPDATED_IND:
//        {
//            // Cast the "param" pointer to the appropriate message structure
//            struct gapc_param_updated_ind const *msg_param = (struct gapc_param_updated_ind const *)(param);

//            // Check if updated Conn Params filled to preferred ones
//            if ((msg_param->con_interval >= user_connection_param_conf.intv_min) &&
//                (msg_param->con_interval <= user_connection_param_conf.intv_max) &&
//                (msg_param->con_latency == user_connection_param_conf.latency) &&
//                (msg_param->sup_to == user_connection_param_conf.time_out))
//            {
//            }
//        } break;

//        case CUSTS1_VALUE_REQ_IND:
//        {
//            struct custs1_value_req_ind const *msg_param = (struct custs1_value_req_ind const *) param;

//            switch (msg_param->att_idx)
//            {
//                case SVC3_IDX_READ_1_VAL:
//                {
//                    user_svc3_read_non_db_val_handler(msgid, msg_param, dest_id, src_id);
//                } break;
//                
//                case SVC2_WRITE_1_VAL:
//                {
//                    user_svc2_write_non_db_val_handler(msgid, msg_param, dest_id, src_id);
//                }break;

//                default:
//                {
//                    // Send Error message
//                    struct custs1_value_req_rsp *rsp = KE_MSG_ALLOC(CUSTS1_VALUE_REQ_RSP,
//                                                                    src_id,
//                                                                    dest_id,
//                                                                    custs1_value_req_rsp);

//                    // Provide the connection index.
//                    rsp->conidx  = app_env[msg_param->conidx].conidx;
//                    // Provide the attribute index.
//                    rsp->att_idx = msg_param->att_idx;
//                    // Force current length to zero.
//                    rsp->length = 0;
//                    // Set Error status
//                    rsp->status  = ATT_ERR_APP_ERROR;
//                    // Send message
//                    ke_msg_send(rsp);
//                } break;
//             }
//        } break;

//        default:
//            break;
//    }
//}

 

/// @} APP
