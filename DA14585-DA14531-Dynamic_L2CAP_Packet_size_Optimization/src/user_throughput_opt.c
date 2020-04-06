/**
 ****************************************************************************************
 *
 * @file user_throughput_opt.c.
 *
 * @brief User Throughput Optimization project source code.
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

#include "app_api.h"
#include "user_throughput_opt.h"
#include "gapc_task.h"
#include "gattc.h"
#include "user_custs1_def.h"
#include "custs1_task.h"
#include "reg_blecore.h"
#include "l2cm.h"
#include "arch_console.h"


#define START_LOG_TRANSFER (0xAA)
#define LOG_SIZE_BYTES			(1024*10)

#define L2CAP_STD_PKT_LEN		(27)
#define STD_BUFFER_LEN			(20)
#define L2CAP_HEADER_SIZE		(4)
#define ATT_HEADER_SIZE			(3)
#define MTU_MAX_NON_DLE			(104)


#define LE_DATA_LENGTH_FEATURE_MASK (0x20)
#define MAX_COLLISION_RETRIES				( 2 ) 

#define MIN_CON_INTERVAL						( MS_TO_DOUBLESLOTS(30) )
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
typedef struct{
		uint8_t 	data_log[LOG_SIZE_BYTES];
		uint16_t 	buffer_ptr;
		bool 			log_transfer_in_progress;
}log_env_t;

typedef struct
{
	
	uint8_t 												connection_idx;
	bool 														peer_supports_dle;
	uint8_t 												custs1_log_ccc_value;
	uint16_t 												user_packet_size;
	struct gapc_param_updated_ind 	user_connection_params;
	uint32_t 												log_transfer_start_time;
	timer_hnd												feature_timer;
	timer_hnd												param_update_timer;
	uint8_t													collision_retries;
	
	log_env_t												user_log_env;
}tput_envt_t;


tput_envt_t tput_env																		__attribute__((section("retention_mem_area0"), zero_init));


/**
 ****************************************************************************************
 * @brief From the features indication msg it checks if DLE is supported
 * @param[in]   gapc_peer_features_ind pointer to features message indication parameters
 * @return void
 ****************************************************************************************
 */
static void user_handle_peer_features(struct gapc_peer_features_ind const *features_ind)
{
	
		if(features_ind->features[0] & LE_DATA_LENGTH_FEATURE_MASK)
		{
				arch_printf("DLE_ON\r\n");
				tput_env.peer_supports_dle = true;
				app_easy_gap_set_data_packet_length(tput_env.connection_idx, user_gapm_conf.max_txoctets, user_gapm_conf.max_txtime);
			
		}else
		{
				tput_env.peer_supports_dle = false;
				arch_printf("DLE_OFF\r\n");
			
		}
	
}

/**
 ****************************************************************************************
 * @brief  Gets the peer device fatures
 * @return void
  ****************************************************************************************
 */
static void user_get_peer_features(void)
{
	 struct gapc_get_info_cmd *pkt = KE_MSG_ALLOC(GAPC_GET_INFO_CMD,
                                                 KE_BUILD_ID(TASK_GAPC, tput_env.connection_idx),
                                                 TASK_APP, gapc_get_info_cmd);

		pkt->operation = GAPC_GET_PEER_FEATURES;
		ke_msg_send(pkt);
	
}
/**
 ****************************************************************************************
 * @brief  Update connection parameters
 * @return void
 ****************************************************************************************
 */
static void user_update_params()
{
	arch_printf("%s\r\n", __func__);
	app_easy_gap_param_update_start(tput_env.connection_idx);
}
/**
 ****************************************************************************************
 * @brief  Initialize environment variables
 * @return void
 ****************************************************************************************
 */
static void user_init_user_vals()
{
		tput_env.user_packet_size = 20;
		tput_env.user_log_env.log_transfer_in_progress = false;
		tput_env.user_log_env.buffer_ptr = 0;
		tput_env.custs1_log_ccc_value = 0;
}
/**
 ****************************************************************************************
 * @brief  Get the BLE Gross Timer value - in 10ms intervals
 * @return time in 10s of milliseconds
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
 * @return void
 ****************************************************************************************
 */
 
static void user_calculate_packet_size(void)
{
	uint16_t ll_buffer_size, mtu_size;
	//3 byte overhead required in MTU - packet size  = mtu-3
	mtu_size = gattc_get_mtu(tput_env.connection_idx);
	ll_buffer_size = l2cm_get_buffer_size(tput_env.connection_idx);
	
	//No DLE is supported /  MTU = 23 and LL buffer = 27.  Largest packet is 20 bytes (MTU - 3 bytes for ATT Header)
	if(ll_buffer_size == L2CAP_STD_PKT_LEN && (mtu_size < ll_buffer_size))
	{
			tput_env.user_packet_size = STD_BUFFER_LEN;
	}	

    // first one contains 20 then we have 3 * 27 bytes of packets to be sent
	//DLE is not supported but larger MTU size is - 101 byte packet size has been proven empirically the most efficient
	if(ll_buffer_size == L2CAP_STD_PKT_LEN && (mtu_size > ll_buffer_size) )
	{
			if(mtu_size > MTU_MAX_NON_DLE)  // most of the time is done with this part
			{
				//104 - 3 = 101 byte packet size
				tput_env.user_packet_size = MTU_MAX_NON_DLE - ATT_HEADER_SIZE;
				
			}
			else if(mtu_size < MTU_MAX_NON_DLE)  // this is used to support the corner case
			{
				 tput_env.user_packet_size = (uint16_t)( (mtu_size - STD_BUFFER_LEN - ATT_HEADER_SIZE)/L2CAP_STD_PKT_LEN )*L2CAP_STD_PKT_LEN + STD_BUFFER_LEN;
			}
		
	}
	
	if(ll_buffer_size == CFG_MAX_TX_PACKET_LENGTH)
	{
		
			tput_env.user_packet_size = mtu_size - ATT_HEADER_SIZE;
		
	}
	
	arch_printf("new calculation: tput_env.user_packet_size: %d \tll_buffer_size: %d\tmtu: %d\r\n", tput_env.user_packet_size, ll_buffer_size, mtu_size);
}
/**
 ****************************************************************************************
 * @brief Sends a exchange MTU command
 * @param[in]   conidx Connection index
 * @return void
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
 * @brief  Callback from SDK for features request to peer
 ****************************************************************************************
 */
void update_connection_params_cb(void)
{
	
	tput_env.param_update_timer = EASY_TIMER_INVALID_TIMER;
	if(tput_env.user_connection_params.con_interval > MIN_CON_INTERVAL){
		app_easy_gap_param_update_start(tput_env.connection_idx);
	}
}


/**
 ****************************************************************************************
 * @brief  Callback from SDK for features request to peer
 ****************************************************************************************
 */
void user_app_on_get_peer_features(const uint8_t conn_id, struct gapc_peer_features_ind const * features)
{
		tput_env.feature_timer = EASY_TIMER_INVALID_TIMER;
		user_handle_peer_features(features);
}

/**
 ****************************************************************************************
 * @brief  Transfer the data log
 * @return void
 ****************************************************************************************
 */
 
 void user_send_log_pkt(void)
 {
	 
	  //If there is enough data in the buffer to fit a whole packet use that, if not send what is left in the buffer
		uint16_t data_size = ( (LOG_SIZE_BYTES - tput_env.user_log_env.buffer_ptr) > tput_env.user_packet_size ) ? tput_env.user_packet_size : (LOG_SIZE_BYTES - tput_env.user_log_env.buffer_ptr);
	 
	
	 
	  struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_LOG_VAL_CHAR_LEN);


    req->conidx = app_env->conidx;
    req->notification = true;
    req->handle = SVC1_IDX_LOG_VAL_VAL;
    req->length = data_size;
    memcpy(req->value, &tput_env.user_log_env.data_log[tput_env.user_log_env.buffer_ptr], data_size);

    ke_msg_send(req);
		
		
		if((LOG_SIZE_BYTES - tput_env.user_log_env.buffer_ptr) <= tput_env.user_packet_size )
		{
			tput_env.user_log_env.log_transfer_in_progress = false;
			tput_env.user_log_env.buffer_ptr = 0;
		}
		else{
			tput_env.user_log_env.log_transfer_in_progress = true;
			tput_env.user_log_env.buffer_ptr+=data_size;
		}
	 
 }

/**
 ****************************************************************************************
 * @brief Called on the reception of GAPC_LE_PKT_SIZE_IND
 * @return void
 ****************************************************************************************
 */

void user_on_data_length_change(uint8_t connection_idx, struct gapc_le_pkt_size_ind *pkt)
{
	//DLE is supported use this packet length for throughput
	user_calculate_packet_size();
	/*exchange mtu*/
	user_exchange_mtu(tput_env.connection_idx);
	
}

/**
 ****************************************************************************************
 * @brief Handles connection event
 * @param[in]   connection_idx Connection index
 * @param[in]   param Parameters of connection
 * @return void
 ****************************************************************************************
 */
void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
	
		tput_env.connection_idx = connection_idx;
	
		//store the connection parameters - currently not using but might want to do a connection parameter update
		tput_env.user_connection_params.con_interval = param->con_interval;
		tput_env.user_connection_params.con_latency  = param->con_latency;
		tput_env.user_connection_params.sup_to       = param->sup_to;
	
		arch_printf("%s\r\n", __func__);
	
		tput_env.collision_retries = 0;
		tput_env.feature_timer = app_easy_timer(60 , user_get_peer_features);
		tput_env.param_update_timer = app_easy_timer(500, update_connection_params_cb);
	
		user_calculate_packet_size();
}

/**
 ****************************************************************************************
 * @brief Handles disconnection event
 * @param[in]   param Parameters of disconnect message
 * @return void
 ****************************************************************************************
 */
void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
	
		if(tput_env.param_update_timer != EASY_TIMER_INVALID_TIMER)
		{
			app_easy_timer_cancel(tput_env.param_update_timer);
			tput_env.param_update_timer = EASY_TIMER_INVALID_TIMER;
		}
		if(tput_env.feature_timer != EASY_TIMER_INVALID_TIMER)
		{
			app_easy_timer_cancel(tput_env.feature_timer);
			tput_env.feature_timer = EASY_TIMER_INVALID_TIMER;
		}
    default_app_on_disconnect(param);
		//MTU will need to be renegotiated on next connection so re-initialize
		user_init_user_vals();
		arch_printf("%s\r\n", __func__);
}

/**
 ****************************************************************************************
 * @brief Callback of the GAPC_UPDATE_PARAMS rejection (parameter update is rejected)
 * @param[in]   status status of the message
 * @return void
 ****************************************************************************************
 */
void user_on_update_params_rejected(const uint8_t  status)
{
	arch_printf("%s: %d\r\n", __func__, status);
	
	/*This only gets called if the scheduler is busy and can't schedule the request (can happen in high throughput applications)*/
	if( status == LL_ERR_LMP_COLLISION ) {
		if(tput_env.collision_retries++ < MAX_COLLISION_RETRIES){
        app_easy_gap_param_update_start(tput_env.connection_idx);
		}
	}
		
	user_exchange_mtu(tput_env.connection_idx);
}

/**
 ****************************************************************************************
 * @brief Callback of the GAPC_UPDATE_PARAMS (parameter update is complete)
 * @param[in]   src_id the source id of the task
 * @return void
 ****************************************************************************************
 */
void user_on_update_params_complete(void)
{
	arch_printf("%s\r\n", __func__);
	user_exchange_mtu(tput_env.connection_idx);
}

/**
 ****************************************************************************************
 * @brief Handles the messages that are not handled by the SDK internal mechanisms.
 * @param[in] msgid      Id of the message received.
 * @param[in] param      Pointer to the parameters of the message.
 * @param[in] dest_id    ID of the receiving task instance.
 * @param[in] src_id     ID of the sending task instance.
 * @return void
 ****************************************************************************************
 */

void user_process_catch_rest(ke_msg_id_t const msgid, void const *param,
                             ke_task_id_t const dest_id, ke_task_id_t const src_id) 
{
	
	switch(msgid)
	{
		case GAPC_PARAM_UPDATED_IND:
		{

			struct gapc_param_updated_ind const *msg_param = (struct gapc_param_updated_ind const *)(param);
			arch_printf("PARAM_UPDATE: %d %d %d \r\n", msg_param->con_interval, msg_param->con_latency, msg_param->sup_to);
			tput_env.user_connection_params.con_interval = msg_param->con_interval;
			//We want faster but iOS device might only allow 30 so we don't want to continously call param update
			if(msg_param->con_interval > MIN_CON_INTERVAL ){
					user_update_params();
			}
			//connection parameters updated, calculate new parameters
			user_calculate_packet_size();
			break;
		}
		case GATTC_MTU_CHANGED_IND:
		{
			//MTU exchanged , get new MTU and calculate packet length
			arch_printf("GATTC_MTU_CHANGED_IND\r\n");
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
							if(!tput_env.user_log_env.log_transfer_in_progress & tput_env.custs1_log_ccc_value)
							{
									tput_env.log_transfer_start_time = port_get_time();
									user_send_log_pkt();
							}
						}
					
				}
				if(msg_param->handle == SVC1_IDX_LOG_VAL_NTF_CFG)
				{
					 if(msg_param->length == 2){
								tput_env.custs1_log_ccc_value = msg_param->value[0];
						}	
				}
				break;
		}
		
		case CUSTS1_VAL_NTF_CFM:
		{
			struct custs1_val_ntf_cfm const *msg_param = (struct custs1_val_ntf_cfm const *)(param);
			
			if(msg_param->handle == SVC1_IDX_LOG_VAL_VAL){
				if(tput_env.user_log_env.log_transfer_in_progress && tput_env.custs1_log_ccc_value)
				{
						user_send_log_pkt();
				}
				else
				{
					  uint32_t tput_calc_bytes_per_sec;
						uint32_t tput_calc_bits_per_sec;
					
						uint32_t debug_end_time = port_get_time();
						uint32_t total_time = (debug_end_time > tput_env.log_transfer_start_time) ? 
																		(debug_end_time - tput_env.log_transfer_start_time) : ((BLE_GROSSTARGET_MASK - tput_env.log_transfer_start_time) + debug_end_time);				
						arch_printf("TIME (ms):  %d       DATA_SENT:  %dBytes \r\n", total_time*10, LOG_SIZE_BYTES);
					
						tput_calc_bytes_per_sec = (LOG_SIZE_BYTES*100)/total_time;
						tput_calc_bits_per_sec = tput_calc_bytes_per_sec*8;
					
						arch_printf("Throughput Calculation: %d Bytes/sec, %d bits/sec\r\n", tput_calc_bytes_per_sec, tput_calc_bits_per_sec);
				}
			}
			break;
		}
		
		case GAPC_PEER_FEATURES_IND:
		{		
			break;		
		
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
						tput_env.peer_supports_dle = false;
						arch_printf("GAPC_PEER_FEATURES_IND UNSUPPORTED, DLE OFF\r\n");
						arch_printf("PARAM UPDATE START\r\n");
				}   
				else        
				{   
						tput_env.peer_supports_dle = false;
						arch_printf("GAPC_PEER_FEATURES_IND UKN STATUS, DLE OFF\r\n");
						arch_printf("PARAM UPDATE START\r\n");
				}   
				
		break;
		default:
			break;
				
			}
						
		}
		default:
			break;
			
			
	}
	
}


/**
 ****************************************************************************************
 * @brief Called on stack initialization
 * @return void
 ****************************************************************************************
 */
void user_app_on_set_dev_config_complete(void)
{
		default_app_on_set_dev_config_complete();
		user_init_user_vals();
		memset(&tput_env.user_log_env.data_log[0], 0xAA, LOG_SIZE_BYTES);
		
#ifdef CFG_PRINTF
		arch_printf("%s\r\n", __func__);
#endif
		
}
 

/// @} APP
