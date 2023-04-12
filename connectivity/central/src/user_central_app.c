/**
 ****************************************************************************************
 *
 * @file user_central_app.c
 *
 * @brief Cental application project source code.
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
#include "user_central_app.h"
#include "user_ble_gap.h"
#include "user_ble_common.h"
#include "user_ble_gatt.h"
#include "user_config.h"
#include "user_central_config.h"
#include "gap.h"
#include "ke_task.h"
#include "wkupct_quadec.h"
#include "user_periph_setup.h"
#include "spi_flash.h"


#define SCAN_INTVL_MS		(50)
#define SCAN_WINDOW_MS	(50)



typedef struct
{
	struct bd_addr addr;
}connecting_device_t;



typedef struct
{
	periph_device_t 	periph_devices[CFG_MAX_CONNECTIONS];
	
	bool 							connect_to_periph;
	struct bd_addr 		connect_to_addr;
	uint8_t 					connect_to_addr_type;
	uint8_t						num_connections;
	timer_hnd					connection_timer;
	
}central_app_env_t;

central_app_env_t central_app_env;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

#ifdef ENABLE_IAS
static void configure_alert_button(void);
#endif

#if (SCAN_FILTER == (SCAN_FILTER_NAME) || SCAN_FILTER == SCAN_FILTER_NONE) 
static inline void format_adv_string(uint8_t * data, uint8_t len, char *out_string)
{
	memcpy(out_string, data, len);
	out_string[len] = '\0';
	
}
#endif
/* return true if UUIDs match */
static bool match_uuid(uint8_t * uuid1, uint8_t *uuid2, uint8_t len)
{
	return !memcmp(uuid1, uuid2, len);
}

#if (SCAN_FILTER == SCAN_FILTER_NONE || SCAN_FILTER == SCAN_FILTER_16_BIT_SVC_DATA || SCAN_FILTER == SCAN_FILTER_MFG_DATA)

/* return static buffer with formatted hex string */
static char *format_hex_string(uint8_t *data, uint8_t len)	
{
	
	static char buf[61];
	uint8_t i;
	for(i = 0; i < len; i++)
	{
		sprintf(&buf[i*2], "%02x", data[i]);
	}
	
	buf[(i+1)*2+1] = '\0';
	
	return buf;
	
}

#endif
	/* return static buffer with formatted address */
static const char *format_bd_address(const struct bd_addr *addr)
{
        static char buf[19];
        int i;

        for (i = 0; i < sizeof(addr->addr); i++) {
                int idx;

                // for printout, address should be reversed
                idx = sizeof(addr->addr) - i - 1;
                sprintf(&buf[i * 3], "%02x:", addr->addr[idx]);
        }

        buf[sizeof(buf) - 2] = '\0';

        return buf;
}
/* return static buffer with formatted UUID */
static const char *format_uuid(att_uuid_t *uuid)
{
        static char buf[37];

        if (uuid->type == ATT_UUID_16) {
                sprintf(buf, "0x%04x", uuid->uuid.uuid16);
        } else {
                int i;
                int idx = 0;

                for (i = ATT_UUID_LENGTH; i > 0; i--) {
                        if (i == 12 || i == 10 || i == 8 || i == 6) {
                                buf[idx++] = '-';
                        }

                        idx += sprintf(&buf[idx], "%02x", uuid->uuid.uuid128[i - 1]);
                }
        }

        return buf;
}

/* return static buffer with characteristics properties mask */
static const char *format_properties(uint8_t properties)
{
        static const char props_str[] = "BRXWNISE"; // each letter corresponds to single property
        static char buf[9];
        int i;

        // copy full properties mask
        memcpy(buf, props_str, sizeof(props_str));

        for (i = 0; i < 8; i++) {
                // clear letter from mask if property not present
                if ((properties & (1 << i)) == 0) {
                        buf[i] = '-';
                }
        }

        return buf;
}

#ifdef ENABLE_IAS
/**
 ****************************************************************************************
 * @brief callback from alert button
 * @return void
 ****************************************************************************************
 */
static void app_button_press_cb()
{
	uint8_t i;
	for(i = 0; i < CFG_MAX_CONNECTIONS ; i++)
	{
		
		if(central_app_env.periph_devices[i].con_valid){
			if(central_app_env.periph_devices[i].serv_disc.ias_alert_counter > 1)
			{
				central_app_env.periph_devices[i].serv_disc.ias_alert_counter = 0;
			}else
			{
				central_app_env.periph_devices[i].serv_disc.ias_alert_counter++;
			}
			

			user_ble_gatt_write(central_app_env.periph_devices[i].serv_disc.ias_write_op,
														central_app_env.periph_devices[i].con_idx, 
														central_app_env.periph_devices[i].serv_disc.ias_char.c.value_handle, 
														&central_app_env.periph_devices[i].serv_disc.ias_alert_counter, 
														sizeof(uint8_t));
		
			dbg_printf("Write Alert Level: %d \r\n", central_app_env.periph_devices[i].serv_disc.ias_alert_counter);
		}
		
	}
	
	configure_alert_button();
}

/**
 ****************************************************************************************
 * @brief configure alert button
 * @return void
 ****************************************************************************************
 */
static void configure_alert_button()
{
	wkupct_register_callback(app_button_press_cb);


	wkupct_enable_irq(WKUPCT_PIN_SELECT(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN), // select pin (GPIO_BUTTON_PORT, GPIO_BUTTON_PIN)
										WKUPCT_PIN_POLARITY(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, WKUPCT_PIN_POLARITY_LOW), // polarity low
										1, // 1 event
										0x3F); // debouncing time = 63

	
}
#endif 

/**
 ****************************************************************************************
 * @brief After full discovery finished
 * @param[in] con_idx - connection identifier
 * @return void
 ****************************************************************************************
 */
static void handle_service_disc_finished(uint8_t con_idx)
{
#ifdef ENABLE_BAS
	if(central_app_env.periph_devices[con_idx].serv_disc.bas_handle_valid){
		user_gatt_read_simple(con_idx, 
						central_app_env.periph_devices[con_idx].serv_disc.bas_char.c.value_handle);
	}
#endif 

#ifdef ENABLE_IAS
	if(central_app_env.periph_devices[con_idx].serv_disc.ias_handle_valid){
			configure_alert_button();
	}
#endif 
	
}

/**
 ****************************************************************************************
 * @brief Scan for devices
 * @return void
 ****************************************************************************************
 */
static void ble_scan_for_devices()
{
	
	ke_state_set(TASK_APP, APP_CONNECTABLE);
	ble_gap_error_t err = user_ble_gap_start_scan(true, SCAN_INTVL_MS, SCAN_WINDOW_MS, false);
	ASSERT_ERROR(err == BLE_GAP_ERROR_NO_ERROR);
}

/**
 ****************************************************************************************
 * @brief Callback from SDK when disconnect occurs.
 * @param[in] report  advertising report information
 * @return void
 ****************************************************************************************
 */
void user_on_adv_report_ind(struct gapm_adv_report_ind const * adv_ind)
{
	bool dev_found = false;
	
#if CONNECT_TO_PERIPHERAL == 1
	bool conn_to_device = false;
	struct bd_addr dev_addr = adv_ind->report.adv_addr;
#endif 

	uint8_t num_ad_elements = user_ble_gap_get_adv_num_elements(adv_ind->report.data, 
																																		adv_ind->report.data_len);
	ble_gap_adv_struct_t adv_data_structs[num_ad_elements]; 
	
	user_ble_gap_parse_adv_data(adv_ind->report.data_len, adv_ind->report.data, adv_data_structs);
	
	uint8_t i;
	for(i = 0 ; i < num_ad_elements; i++)
	{
		
		switch(adv_data_structs[i].type)
		{
#if SCAN_FILTER == SCAN_FILTER_NONE
			case GAP_AD_TYPE_FLAGS:
			{
				dev_found = true;
				dbg_block_printf("GAP FLAGS: %s\r\n", 
													format_hex_string(adv_data_structs[i].data, adv_data_structs[i].len) );
				break;
			}
#endif 
#if (SCAN_FILTER == SCAN_FILTER_NONE) 
			case GAP_AD_TYPE_MORE_16_BIT_UUID:
			{
				dev_found = true;
				dbg_block_printf("INCOMP LIST 16-BIT SVC: %s\r\n", 
													format_hex_string(adv_data_structs[i].data, adv_data_structs[i].len) );
				break;
			}
#endif
#if (SCAN_FILTER == (SCAN_FILTER_NAME) || SCAN_FILTER == SCAN_FILTER_NONE) 

			case GAP_AD_TYPE_COMPLETE_NAME:
			{
				dev_found = true;
				
				char local_name[adv_data_structs[i].len + 1];
				format_adv_string(adv_data_structs[i].data,adv_data_structs[i].len, local_name);
				dbg_block_printf("Device Local Name: %s\r\n", local_name);
	#if CONNECT_TO_PERIPHERAL == 1
				conn_to_device = memcmp(PERIPH_MATCH_DATA, adv_data_structs[i].data, PERIPH_MATCH_DATA_LEN) ? false : true;
	#endif 
				break;
			}
#endif 
#if SCAN_FILTER == SCAN_FILTER_NONE
			
			case GAP_AD_TYPE_TRANSMIT_POWER:
			{
				dev_found = true;
				
				int8_t power;
				memcpy(&power, adv_data_structs[i].data, 1); 
				dbg_block_printf("TX_POWER: %i dBm\r\n", power);
													
				break;
			}
#endif
#if (SCAN_FILTER == SCAN_FILTER_16_BIT_SVC_DATA || SCAN_FILTER == SCAN_FILTER_NONE) 
			case GAP_AD_TYPE_SERVICE_16_BIT_DATA:
			{
				dev_found = true;
				
				uint16_t UUID;
				memcpy(&UUID, adv_data_structs[i].data, sizeof(uint16_t));
				dbg_block_printf("GAP_TYPE: SVC_DATA_16_BIT: UUID:%04X DATA:%s\r\n", 
														UUID, format_hex_string(adv_data_structs[i].data+2, adv_data_structs[i].len-2) );
				break;
			}
#endif 
#if (SCAN_FILTER == SCAN_FILTER_MFG_DATA || SCAN_FILTER == SCAN_FILTER_NONE) 
			case GAP_AD_TYPE_MANU_SPECIFIC_DATA:
			{
				dev_found = true;
				dbg_block_printf("MFG_SPECIFIC_DATA: %s\r\n", 
													format_hex_string(adv_data_structs[i].data, adv_data_structs[i].len) );
				break;
			}
#endif 
			default:
			{
#if SCAN_FILTER == SCAN_FILTER_NONE
				dev_found = true;
				
				dbg_block_printf("GAP Type: %02x, Data: %s\r\n", adv_data_structs[i].type, 
												format_hex_string(adv_data_structs[i].data, adv_data_structs[i].len) );
#endif
				break;
			}
			
		}
		
	}
	
	if(dev_found){
		uint8_t rssi_abs = 256 - adv_ind->report.rssi;
		dbg_block_printf("RSSI: -%d\r\n", rssi_abs);
		dbg_block_printf("BD_ADDR:%s \r\n---------------END_ADV-----------\r\n", 
																						format_bd_address(&adv_ind->report.adv_addr) );
	}

	#if CONNECT_TO_PERIPHERAL
	if(conn_to_device)
	{
		dbg_block_printf("Connecting to Device...\r\n", NULL);
		central_app_env.connect_to_periph = true;
		memcpy(&central_app_env.connect_to_addr, &dev_addr, sizeof(struct bd_addr));
		central_app_env.connect_to_addr_type = adv_ind->report.adv_addr_type;
		
		user_ble_gap_stop_scan();
		
	}
	#endif

	
	

}

/**
 ****************************************************************************************
 * @brief Callback from SDK when  connection occurs.
 * @param[in] connection_idx  connection identifier
 * @param[in] param  peer connection info
 * @return void
 ****************************************************************************************
 */
void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
	
		if(central_app_env.connection_timer != EASY_TIMER_INVALID_TIMER){
			app_easy_timer_cancel(central_app_env.connection_timer);
			central_app_env.connection_timer = EASY_TIMER_INVALID_TIMER;
		}
	
    default_app_on_connection(connection_idx, param);
		dbg_printf("%s\r\n", __func__);
		
		central_app_env.periph_devices[connection_idx].addr = param->peer_addr;
		central_app_env.periph_devices[connection_idx].con_idx = connection_idx;
		central_app_env.periph_devices[connection_idx].con_valid = true;
		central_app_env.num_connections++;
	
		if(central_app_env.num_connections < CFG_MAX_CONNECTIONS)
		{
			ble_scan_for_devices();
		}

#ifdef ENABLE_IAS		
		central_app_env.periph_devices[connection_idx].serv_disc.ias_handle_valid = false;
#endif
#ifdef ENABLE_BAS
		central_app_env.periph_devices[connection_idx].serv_disc.bas_handle_valid = false;
#endif
		
		user_gatt_discover_all_services(connection_idx, 1);
}

/**
 ****************************************************************************************
 * @brief Callback from SDK when disconnect occurs.
 * @param[in] param  peer connection info
 * @return void
 ****************************************************************************************
 */
void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
		central_app_env.periph_devices[param->conhdl].con_valid = false;
	
		if(central_app_env.connection_timer != EASY_TIMER_INVALID_TIMER){
			app_easy_timer_cancel(central_app_env.connection_timer);
			central_app_env.connection_timer = EASY_TIMER_INVALID_TIMER;
		}
		if( central_app_env.num_connections == CFG_MAX_CONNECTIONS)
		{
			ble_scan_for_devices();
		}
		central_app_env.num_connections--;
		
		dbg_printf("%s: reason:%02x\r\n", __func__, param->reason);
#ifdef ENABLE_IAS		
		if(central_app_env.num_connections < 1)
		{
			wkupct_disable_irq();
		}
#endif
		
		
}

/**
 ****************************************************************************************
 * @brief Callback from SDK when disconnect occurs.
 * @return void
 ****************************************************************************************
 */
static void connection_timeout_cb(){
	
	central_app_env.connection_timer = EASY_TIMER_INVALID_TIMER;
	user_ble_cancel_connect();
}

/**
 ****************************************************************************************
 * @brief Callback from SDK when disconnect occurs.
 * @param[in] reason reason for disconnect BT SIG reason defined in co_error.h
 * @return void
 ****************************************************************************************
 */
void user_on_scanning_completed(uint8_t reason)
{
	
	if(reason == GAP_ERR_CANCELED)
	{
		user_ble_gap_connect(central_app_env.connect_to_addr.addr, central_app_env.connect_to_addr_type);
		central_app_env.connection_timer = app_easy_timer(CONNECTION_TIMEOUT_10MS, connection_timeout_cb);
	}
	else{
			
		dbg_printf("%s: ERR: reason: %d\r\n", __func__, reason);
	}
	
	
	
}

/**
 ****************************************************************************************
 * @brief Callback from SDK once stack is configured
 * @return void
 ****************************************************************************************
 */
void user_app_on_set_dev_config_complete(void)
{
		/*Advertising starts within this default handler, but we have made the callback NULL to avoid advertising. 
		* alternate method, would be to copy the default handler code into here, removing the advertising call
		*/
		default_app_on_set_dev_config_complete();
	
		ble_scan_for_devices();
	
		spi_flash_power_down(); //power down flash before entering sleep
}


static void handle_svc_ind(uint8_t con_idx, struct gattc_sdp_svc_ind const *disc_svc)
{
	service_info_t *svc = ke_malloc(user_ble_get_svc_size(disc_svc), KE_MEM_NON_RETENTION);
	dbg_block_printf("%s: conn_idx=%04x start_h=%04x end_h=%04x\r\n", __func__, con_idx,
                                                                        disc_svc->start_hdl, disc_svc->end_hdl);

	user_gatt_parse_service(disc_svc, svc, con_idx);
	
	uint16_t i;
	
#ifdef ENABLE_IAS
	bool ias_uuid_match;
	uint16_t ias_uuid = ATT_SVC_IMMEDIATE_ALERT;
	uint16_t ias_alert_uuid = ATT_CHAR_ALERT_LEVEL;
	if(svc->svc_uuid.type == ATT_UUID_16){
		ias_uuid_match = match_uuid((uint8_t *)&ias_uuid, (uint8_t *)&svc->svc_uuid.uuid.uuid16, sizeof(uint16_t));
	}
	
	if(ias_uuid_match){
		dbg_block_printf("Immediate Alert Service: ", NULL);
	}
#endif 
	
#ifdef ENABLE_BAS
	bool bas_uuid_match;
	uint16_t bas_uuid = ATT_SVC_BATTERY_SERVICE;
	uint16_t bas_level_uuid= ATT_CHAR_BATTERY_LEVEL;
	
	if(svc->svc_uuid.type == ATT_UUID_16){
		bas_uuid_match = match_uuid((uint8_t *)&bas_uuid, (uint8_t *)&svc->svc_uuid.uuid.uuid16, sizeof(uint16_t));
	}
	if(bas_uuid_match){
		dbg_block_printf("Battery Service: ", NULL);
	}
#endif
	dbg_block_printf("%s: \r\n", format_uuid(&svc->svc_uuid) );
	for(i = 0 ; i<svc->num_chars; i++)
	{
		gattc_chars_t *gatt_char = &svc->items[i];
#ifdef ENABLE_IAS
		if(ias_uuid_match)
		{
			if( match_uuid( (uint8_t *)&ias_alert_uuid , (uint8_t *)&gatt_char->uuid.uuid, 2) )
			{
				memcpy(&central_app_env.periph_devices[con_idx].serv_disc.ias_char, 
																									gatt_char, sizeof(gattc_chars_t) );
				
				central_app_env.periph_devices[con_idx].serv_disc.ias_handle_valid = true;
				dbg_block_printf("\tAlert Level Char\r\n", NULL);
				
				if(gatt_char->c.properties & GATT_PROP_WRITE)
				{
					central_app_env.periph_devices[con_idx].serv_disc.ias_write_op = GATTC_WRITE;
				}
				else if(gatt_char->c.properties & GATT_PROP_WRITE_NO_RESP)
				{
					central_app_env.periph_devices[con_idx].serv_disc.ias_write_op = GATTC_WRITE_NO_RESPONSE;	
				}
				else if(gatt_char->c.properties & GATT_PROP_WRITE_SIGNED)
				{
					central_app_env.periph_devices[con_idx].serv_disc.ias_write_op = GATTC_WRITE_SIGNED;	
				}else
				{		
					central_app_env.periph_devices[con_idx].serv_disc.ias_handle_valid = false;
				}
			}
		}
#endif 
#ifdef ENABLE_BAS
		if(bas_uuid_match)
		{
			if( match_uuid( (uint8_t *)&bas_level_uuid , (uint8_t *)&gatt_char->uuid.uuid, 2) )
			{
				memcpy(&central_app_env.periph_devices[con_idx].serv_disc.bas_char, 
																										gatt_char, sizeof(gattc_chars_t));
				
				central_app_env.periph_devices[con_idx].serv_disc.bas_handle_valid = true;
				dbg_block_printf("\tBattery Level Char\r\n", NULL);
			}
		}
#endif 
		dbg_block_printf("\t%04x char %s prop=%02x (%s)\r\n", gatt_char->handle,
                                                format_uuid(&gatt_char->uuid), gatt_char->c.properties,
                                                format_properties(gatt_char->c.properties));	
	}
	
	ke_free(svc);
	
	central_app_env.periph_devices[con_idx].serv_disc.last_handle = disc_svc->end_hdl;
	
	
}

/**
 ****************************************************************************************
 * @brief Callback from SDK when an unhandled stack event occurs
 * @param[in] msgid ID of message from stack
 * @param[in] param - message data
 * @param[in] dest_id - destination task of message, e.g. TASK_APP
 * @param[in] src_id - task originator of message
 * @return void
 ****************************************************************************************
 */
void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
	
	
	uint8_t conn_idx = KE_IDX_GET(src_id);
	switch(msgid)
	{
		case GATTC_CMP_EVT:
		{
			
			struct gattc_cmp_evt const* evt = (struct gattc_cmp_evt const *)(param);
			switch(evt->operation)
			{
				case GATTC_SDP_DISC_SVC_ALL:
				{
					
					dbg_printf("Completion Event: GATTC_DISC_ALL_SVC: status: %04X\r\n", evt->status);
					if(evt->status != CO_ERROR_NO_ERROR){

							user_gatt_discover_all_services(conn_idx, central_app_env.periph_devices[conn_idx].serv_disc.last_handle);
					}else
					{
							handle_service_disc_finished(conn_idx);
					}
					
					
				}break;
				case GATTC_WRITE:
				{
					dbg_printf("Completion Event: GATTC_WRITE\r\n", evt->operation);
					break;
				}
				case GATTC_READ:
					dbg_printf("GATTC_READ: status: %04x\r\n", evt->status);
					break;
				
				default:
					break;
			}
			
		}break;
		
		case GATTC_EVENT_IND:
		{
			struct gattc_event_ind const *ind = (struct gattc_event_ind const *)param;
#ifdef ENABLE_BAS
			if(ind->handle == central_app_env.periph_devices[conn_idx].serv_disc.bas_char.c.value_handle)
			{
				dbg_printf("Battery Level Changed: %d \r\n", ind->value[0]);			
			}
#endif		
		}break;
		
		case GAPM_CMP_EVT:
		{
			struct gapm_cmp_evt const* evt = (struct gapm_cmp_evt const *)(param);
			
			if(evt->operation == GAPM_CANCEL)
			{
				dbg_printf("Connection Timeout\r\n", NULL);
				ble_scan_for_devices();
			}
			break;
			
		}
		case GATTC_SDP_SVC_IND:
		{
			struct gattc_sdp_svc_ind const *disc_svc = (struct gattc_sdp_svc_ind const *)(param);
			
			uint8_t con_idx = KE_IDX_GET(src_id);
			handle_svc_ind(con_idx, disc_svc);
		}break;
		
		case GATTC_READ_IND:
		{
			struct gattc_read_ind const *ind = (struct gattc_read_ind const*)param;
#ifdef ENABLE_BAS
			if(ind->handle == central_app_env.periph_devices[conn_idx].serv_disc.bas_char.c.value_handle)
			{
				dbg_printf("Battery Level Read: %d \r\n", ind->value[0]);
			}else
#endif 
			{
				dbg_printf("GATTC_READ_IND: handle: %04x\r\n", ind->handle);
				
			}
		}break;
		
		
		default:
			break;
		
	}
	
	
}
 

/// @} APP
