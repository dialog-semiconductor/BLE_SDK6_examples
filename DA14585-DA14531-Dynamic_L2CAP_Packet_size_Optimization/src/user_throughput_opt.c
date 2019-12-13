/**
 ****************************************************************************************
 *
 * @file user_empty_peripheral_template.c
 * History: search string malam
 * Jon K worked on customer demo to share an optimized project to verify the changes of BLE packet on the fly based on
     DLE and the MTU size is negotiated. 
     BLE PKT really is getting set dynamically.
     
     Smartphone iOS is used 12.0.xx with iPhone 6, it does not support DLE.
     iPhone 8 could be more appropriate to demostrate this work as it supports DLE.
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (c) 2012-2018 Dialog Semiconductor. All rights reserved.
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

#include "app_api.h"
#include "user_throughput_opt.h"
#include "gapc_task.h"
#include "gattc.h"
#include "user_custs1_def.h"
#include "custs1_task.h"
#include "reg_blecore.h"
#include "l2cm.h"

#ifdef CFG_PRINTF
#include "arch_console.h"
#endif 



struct gapc_param_updated_ind user_connection_params 		__attribute__((section("retention_mem_area0"), zero_init));
uint8_t last_connection_idx															__attribute__((section("retention_mem_area0"), zero_init));	
uint16_t user_packet_size																__attribute__((section("retention_mem_area0"), zero_init));	

log_env_t user_log_env																	__attribute__((section("retention_mem_area0"), zero_init));

uint32_t debug_start_time;
uint8_t  custs1_log_ccc_value														__attribute__((section("retention_mem_area0"), zero_init));
bool supports_dle																				__attribute__((section("retention_mem_area0"), zero_init));
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
static void user_handle_peer_features(struct gapc_peer_features_ind const *features_ind)
{
	
		if(features_ind->features[0] & LE_DATA_LENGTH_FEATURE_MASK)
		{
				arch_printf("DLE Supported! \r\n");
				supports_dle = true;
				app_easy_gap_param_update_start(last_connection_idx);
			
		}else
		{
				supports_dle = false;
				arch_printf("DLE NOT Supported :(\r\n");
				app_easy_gap_param_update_start(last_connection_idx);
			
		}
	
}
static void user_get_peer_features(void)
{
	 struct gapc_get_info_cmd *pkt = KE_MSG_ALLOC(GAPC_GET_INFO_CMD,
                                                 KE_BUILD_ID(TASK_GAPC, last_connection_idx),
                                                 TASK_APP, gapc_get_info_cmd);

		pkt->operation = GAPC_GET_PEER_FEATURES;
		ke_msg_send(pkt);
	
}
static void user_update_params()
{
	arch_printf("%s\r\n", __func__);
	app_easy_gap_param_update_start(last_connection_idx);
}
/**
 ****************************************************************************************
 * @brief  Initialize environment variables
 ****************************************************************************************
 */
 
static void user_init_user_vals()
{
		user_packet_size = 20;
		user_log_env.log_transfer_in_progress = false;
		user_log_env.buffer_ptr = 0;
		custs1_log_ccc_value = 0;
}
/**
 ****************************************************************************************
 * @brief  Get the BLE Gross Timer value - in 10ms intervals
 ****************************************************************************************
 */
 
static uint32_t port_get_time(void)
{

    uint32_t ea_time_get_halfslot_rounded(void);   
    return ((ea_time_get_halfslot_rounded() >> 4) & BLE_GROSSTARGET_MASK);
 
}
/**
 ****************************************************************************************
 * @brief  Calculate packet size for throughput
 ****************************************************************************************
 */
 
static void user_calculate_packet_size(void)
{
	uint16_t ll_buffer_size, mtu_size;
	//3 byte overhead required in MTU - packet size  = mtu-3
	mtu_size = gattc_get_mtu(last_connection_idx);
	ll_buffer_size = l2cm_get_buffer_size(last_connection_idx);
	
	//No DLE is supported /  MTU = 23 and LL buffer = 27.  Largest packet is 20 bytes (MTU - 3 bytes for ATT Header)
	if(ll_buffer_size == L2CAP_STD_PKT_LEN && (mtu_size < ll_buffer_size))
	{
			user_packet_size = STD_BUFFER_LEN;
	}	

    // first one contains 20 then we have 3 * 27 bytes of packets to be sent
	//DLE is not supported but larger MTU size is - 101 byte packet size has been proven empirically the most efficient
	if(ll_buffer_size == L2CAP_STD_PKT_LEN && (mtu_size > ll_buffer_size) )
	{
			if(mtu_size > MTU_MAX_NON_DLE)  // most of the time is done with this part
			{
				//104 - 3 = 101 byte packet size
				user_packet_size = MTU_MAX_NON_DLE - ATT_HEADER_SIZE;
				
			}
			else if(mtu_size < MTU_MAX_NON_DLE)  // this is used to support the corner case
			{
				 user_packet_size = (uint16_t)( (mtu_size - STD_BUFFER_LEN - ATT_HEADER_SIZE)/L2CAP_STD_PKT_LEN )*L2CAP_STD_PKT_LEN + STD_BUFFER_LEN;
			}
		
	}
	
	if(ll_buffer_size == CFG_MAX_TX_PACKET_LENGTH)
	{
		
			user_packet_size = mtu_size - ATT_HEADER_SIZE;
		
	}
	
	arch_printf("new calculation: user_packet_size: %d \tll_buffer_size: %d\tmtu: %d\r\n", user_packet_size, ll_buffer_size, mtu_size);
}
/**
 ****************************************************************************************
 * @brief  Send MTU exchange command
 ****************************************************************************************
 */
static void user_exchange_mtu(uint8_t connection_idx)
{
    struct gattc_exc_mtu_cmd *pkt = KE_MSG_ALLOC(GATTC_EXC_MTU_CMD,
                                                 KE_BUILD_ID(TASK_GATTC, app_env[connection_idx].conidx),
                                                 TASK_APP, gattc_exc_mtu_cmd);
    if (pkt) {

        pkt->operation = GATTC_MTU_EXCH;
        ke_msg_send(pkt);
    }
}

/**
 ****************************************************************************************
 * @brief  Transfer the data log
 ****************************************************************************************
 */
 
 void user_send_log_pkt(void)
 {
	 
	  //If there is enough data in the buffer to fit a whole packet use that, if not send what is left in the buffer
		uint16_t data_size = ( (LOG_SIZE_BYTES - user_log_env.buffer_ptr) > user_packet_size ) ? user_packet_size : (LOG_SIZE_BYTES - user_log_env.buffer_ptr);
	 
	
	 
	  struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_LOG_VAL_CHAR_LEN);


    req->conidx = app_env->conidx;
    req->notification = true;
    req->handle = SVC1_IDX_LOG_VAL_VAL;
    req->length = data_size;
    memcpy(req->value, &user_log_env.data_log[user_log_env.buffer_ptr], data_size);

    ke_msg_send(req);
		
		
		if(data_size != user_packet_size)
		{
			user_log_env.log_transfer_in_progress = false;
			user_log_env.buffer_ptr = 0;
		}
		else{
			user_log_env.log_transfer_in_progress = true;
			user_log_env.buffer_ptr+=data_size;
		}
	 
 }

/**
 ****************************************************************************************
 * @brief Callback from SDK for when DLE is negotiated 
 ****************************************************************************************
 */
 
void user_on_data_length_change(uint8_t connection_idx, struct gapc_le_pkt_size_ind *pkt)
{
	//DLE is supported use this packet length for throughput
	user_calculate_packet_size();
	
}
void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
	
		last_connection_idx = connection_idx;
	
		//store the connection parameters - currently not using but might want to do a connection parameter update
		user_connection_params.con_interval = param->con_interval;
		user_connection_params.con_latency  = param->con_latency;
		user_connection_params.sup_to       = param->sup_to;
	
		arch_printf("%s: Con_interval%d\r\n", __func__, param->con_interval);
	
		if(param->con_interval >  user_connection_param_conf.intv_max){
			app_easy_timer(60 , user_get_peer_features);
		}
		
		//user_get_peer_features(connection_idx);
		//manually sending option of DLE
		// Send an MTU exchange command. This is needed for some hosts which do not
    // initiate the MTU exchange procedure
    //user_exchange_mtu(connection_idx);
	
		user_calculate_packet_size();
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
		//MTU will need to be renegotiated on next connectionso re-initialize
		user_init_user_vals();
		arch_printf("%s\r\n", __func__);
}


void user_on_update_params_rejected(const uint8_t  status)
{
	arch_printf("%s\r\n", __func__);
	
	if(supports_dle)
	{
		
		app_easy_gap_set_data_packet_length(last_connection_idx, user_gapm_conf.max_txoctets, user_gapm_conf.max_txtime);
	}
	
	user_exchange_mtu(last_connection_idx);
}

void user_on_update_params_complete(void)
{
	arch_printf("%s\r\n", __func__);
	
	if(supports_dle)
	{
		
		app_easy_gap_set_data_packet_length(last_connection_idx, user_gapm_conf.max_txoctets, user_gapm_conf.max_txtime);
	}
	
	user_exchange_mtu(last_connection_idx);
}
void user_process_catch_rest(ke_msg_id_t const msgid, void const *param,
                             ke_task_id_t const dest_id, ke_task_id_t const src_id) 
{
	
	uint8_t conidx = KE_IDX_GET(src_id);
	switch(msgid)
	{
		case GAPC_PARAM_UPDATED_IND:
		{

			struct gapc_param_updated_ind const *msg_param = (struct gapc_param_updated_ind const *)(param);
			arch_printf("PARAM_UPDATE: %d %d %d \r\n", msg_param->con_interval, msg_param->con_latency, msg_param->sup_to);
			//We want faster but iOS device might only allow 30 so we don't want to continously call param update
			if(msg_param->con_interval > (MS_TO_DOUBLESLOTS(30) )){
					user_update_params();
			}
			//connection parameters updated, calculate new parameters
			user_calculate_packet_size();
			break;
		}
		case GATTC_MTU_CHANGED_IND:
		{
			//MTU exchanged , get new MTU and calculate packet length
			arch_printf("MTU Changed! \r\n");
			user_calculate_packet_size();	
			break;
		}
		
		case CUSTS1_VAL_WRITE_IND:
		{
				struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);
			
				if(msg_param->handle == SVC1_IDX_CONTROL_POINT_VAL)
				{
						if( (msg_param->value[0] == START_LOG_TRANSFER) && (msg_param->length < 2))
						{
							if(!user_log_env.log_transfer_in_progress & custs1_log_ccc_value)
							{
									debug_start_time = port_get_time();
									user_send_log_pkt();
							}
						}
					
				}
				if(msg_param->handle == SVC1_IDX_LOG_VAL_NTF_CFG)
				{
					 if(msg_param->length == 2){
								custs1_log_ccc_value = msg_param->value[0];
						}	
				}
				break;
		}
		
		case CUSTS1_VAL_NTF_CFM:
		{
			struct custs1_val_ntf_cfm const *msg_param = (struct custs1_val_ntf_cfm const *)(param);
			
			if(msg_param->handle == SVC1_IDX_LOG_VAL_VAL){
				if(user_log_env.log_transfer_in_progress && custs1_log_ccc_value)
				{
						user_send_log_pkt();
				}
				else
				{
						uint32_t debug_end_time = port_get_time();
						uint32_t total_time = (debug_end_time > debug_start_time) ? (debug_end_time - debug_start_time) : ((BLE_GROSSTARGET_MASK - debug_start_time) + debug_end_time);
					
						arch_printf("TIME (ms):  %d       DATA_SENT:  %dBytes \r\n", total_time*10, LOG_SIZE_BYTES);
						arch_printf("s: %d  e: %d \r\n", debug_start_time, debug_end_time);
				}
			}
			break;
		}
		
		case GAPC_PEER_FEATURES_IND:
		{
			//Getting the peer features back
			struct gapc_peer_features_ind const *msg_param = (struct gapc_peer_features_ind const *)(param);
			
			user_handle_peer_features(msg_param);
			
		
		}
		
		case GAPC_CMP_EVT:
		{
			struct gapc_cmp_evt const *cmp_param = param;
			switch(cmp_param->operation)
			{ 
				
				case GAPC_GET_PEER_FEATURES:
					
				if( cmp_param->status == GAP_ERR_NO_ERROR )
				{   
						arch_printf("GAPC_PEER_FEATURES_IND OK\r\n");
				}   
				else if ( cmp_param->status == LL_ERR_UNSUPPORTED_REMOTE_FEATURE )
				{   
						supports_dle = false;
						arch_printf("GAPC_PEER_FEATURES_IND UNSUPPORTED, DLE OFF\r\n");
						arch_printf("PARAM UPDATE START\r\n");
						app_easy_gap_param_update_start(conidx);
				}   
				else        
				{   
						supports_dle = false;
						arch_printf("GAPC_PEER_FEATURES_IND UKN STATUS, DLE OFF\r\n");
						arch_printf("PARAM UPDATE START\r\n");
						app_easy_gap_param_update_start(conidx);
				}   
				
		break;
		default:
				arch_printf("xxxx %i\r\n",(int)cmp_param->operation);
		break;
				
			}
						
		}
		default:
			break;
			
			
	}
	
}

void user_app_on_set_dev_config_complete(void)
{
		default_app_on_set_dev_config_complete();
		user_init_user_vals();
		memset(&user_log_env.data_log[0], 0xAA, LOG_SIZE_BYTES);
		
#ifdef CFG_PRINTF
		arch_printf("%s\r\n", __func__);
#endif
		
}
 

/// @} APP
