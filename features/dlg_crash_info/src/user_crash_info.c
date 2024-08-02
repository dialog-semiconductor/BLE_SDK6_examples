/**
 ****************************************************************************************
 *
 * @file user_crash_info.c
 *
 * @brief user crash info application c source.
 *
 * Copyright (c) 2012-2023 Renesas Electronics Corporation and/or its affiliates
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
 

#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "user_crash_info.h"
#include "dci.h"
#include "user_periph_setup.h"
#include "wkupct_quadec.h"
#include "user_wakeup.h"
#include "arch_console.h"
#include "dci_svc.h"
#include "custs1_task.h"
#include "user_custs1_def.h"


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/



#define ERROR_HARDFAULT				0
#define ERROR_NMI							1
#define ERROR_PLATFORM_RESET	2

#define ERROR_TYPE						ERROR_HARDFAULT		
#define EPOCH_MN							(0xBADDCAFE)
#define EPOCH_TICK_10MS				(100)

typedef struct
{
	// Payload length, BLE length byte will get calculated in helper function
	uint8_t 	len;
	// GAP Flag
	uint8_t 	type;
	// Payload data
	uint8_t 	*data;
	
}gap_adv_struct_t;

#ifdef ENABLE_DCI_SERVICE
static void fill_adv_data(struct gapm_start_advertise_cmd* cmd);
static void fill_scan_rsp_data(struct gapm_start_advertise_cmd* cmd);
#endif // ENABLE_DCI_SERVICE

uint32_t s_app_epoch				__SECTION_ZERO("retention_mem_area_uninit");


static void button_pressed_cb(void)
{
	
#if ERROR_TYPE == ERROR_HARDFAULT
		//create unaligned access by CPU
		void (*func)(void) = (void (*)(void ))(0x0);
		func();
#elif ERROR_TYPE == ERROR_NMI
		volatile int x = 1;
		do{
		}while(x);
	
#elif ERROR_TYPE == ERROR_PLATFORM_RESET
#endif 
	
}

static void epoch_tick_cb(void)
{
	s_app_epoch++;
	app_easy_timer(EPOCH_TICK_10MS, epoch_tick_cb); 
}

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
		dci_svc_cleanup_on_disconnect();
    default_app_on_disconnect(param);
}

void user_app_on_set_dev_config_complete(void)
{
		dci_info_t info;
		default_app_on_set_dev_config_complete();
		uwakeup_init(button_pressed_cb, GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, WKUPCT_PIN_POLARITY_LOW);
		dci_get_data(&info);
		dci_print_verbose();
	
		#ifdef ENABLE_DCI_SERVICE
		dci_svc_init();
		#endif // ENABLE_DCI_SERVICE
	
		app_easy_timer(100, epoch_tick_cb);
}

__USED void reset_indication(uint16_t reset_stat)
{
	if(dci_init(reset_stat))
	{
		s_app_epoch = 0;
	}
}

uint32_t  dci_get_epoch(void)
{
	return s_app_epoch;
}
 
void user_advertise_operation(void)
{
		#ifdef ENABLE_DCI_SERVICE
		struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();

    fill_adv_data(cmd);
		fill_scan_rsp_data(cmd);
	
    // Send the message
    app_easy_gap_undirected_advertise_start();
		#else
		default_advertise_operation();
		#endif // ENABLE_DCI_SERVICE
}

#ifdef ENABLE_DCI_SERVICE
/**
 ****************************************************************************************
 * @brief Add Debug Crash Info Service data to the advertising data
 * @param[in] cmd			Pointer to the start advertising command structure
 * @return none
 ****************************************************************************************
*/
static void fill_adv_data(struct gapm_start_advertise_cmd* cmd)
{
    // zero the data
    cmd->info.host.adv_data_len = 0;
    memset(cmd->info.host.adv_data, 0, ADV_DATA_LEN);
	
		// Advertising data:
    // Flags (3 Bytes, added automatically by stack)
		// 128 Bit UUID (16 bytes, 18 bytes for entire AD Structure)
    
		att_svc_desc128_t dlg_crash_info_svc_uuid = DCI_SVC_UUID_128;
		uint8_t svc_data[ATT_UUID_128_LEN + 1];
		
		memcpy(svc_data, dlg_crash_info_svc_uuid, ATT_UUID_128_LEN);
		dci_info_t crash_info;
		dci_get_data(&crash_info);
		svc_data[ATT_UUID_128_LEN] = crash_info.data->num_resets;

		gap_adv_struct_t adv_data[] = 
		{
				{
					.len = ATT_UUID_128_LEN + 2, // UUID len + 1 byte for data (num of resets) + 1 byte for AD Type
					.type = GAP_AD_TYPE_SERVICE_128_BIT_DATA,
					.data = (uint8_t *)svc_data
				}
		};
		
		// AD 1, 128 Bit UUID
	  cmd->info.host.adv_data[0] = adv_data[0].len;
    // AD Type
    cmd->info.host.adv_data[1] = adv_data[0].type;
    // AD Data
    memcpy(&cmd->info.host.adv_data[2], adv_data[0].data, adv_data[0].len);
    // increment data length
    cmd->info.host.adv_data_len = (1 + adv_data[0].len); // +1 for AD Length
}

/**
 ****************************************************************************************
 * @brief Add device name to scan response data
 * @param[in] cmd			Pointer to the start advertising command structure
 * @return none
 ****************************************************************************************
*/
static void fill_scan_rsp_data(struct gapm_start_advertise_cmd* cmd)
{
    // zero the data
    cmd->info.host.scan_rsp_data_len = 0;
    memset(cmd->info.host.scan_rsp_data, 0, SCAN_RSP_DATA_LEN);
	
		// Scan response data:
    // BLE Device Name 29 byte max, 31 bytes for entire AD Structure)
    
		uint8_t dev_name[USER_DEVICE_NAME_LEN];
		memcpy(dev_name, USER_DEVICE_NAME, USER_DEVICE_NAME_LEN);

		gap_adv_struct_t scan_data[] = 
		{	
				{
					.len = USER_DEVICE_NAME_LEN + 1, // AD Type + AD Data
					.type = GAP_AD_TYPE_COMPLETE_NAME,
					.data = (uint8_t *)dev_name
					
				}
		};
				
		// AD 1, Device Name
    // Length
    cmd->info.host.scan_rsp_data[0] = scan_data[0].len;
    // AD Type
    cmd->info.host.scan_rsp_data[1] = scan_data[0].type;
    // AD Data
    memcpy(&cmd->info.host.scan_rsp_data[2], scan_data[0].data, scan_data[0].len);
    // increment data length
    cmd->info.host.scan_rsp_data_len += (1 +  scan_data[0].len);  // +1 for AD Length
}
#endif // ENABLE_DCI_SERVICE

void user_message_handler(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
	
		switch(msgid)
		{
				
				case CUSTS1_VAL_WRITE_IND:
				{
						struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);
						
						#ifdef ENABLE_DCI_SERVICE
						if(msg_param->handle > DEBUG_CRASH_INFO_SVC_START &&
						   msg_param->handle <= DEBUG_CRASH_INFO_SVC_END)
						{
								// write request to the DCI service
								dci_svc_write_handler(msgid, msg_param, dest_id, src_id);
						}
						#endif // ENABLE_DCI_SERVICE
						
						break;
						
				} 
				case CUSTS1_VAL_NTF_CFM:
				{
						struct custs1_val_ntf_cfm const *msg_param = (struct custs1_val_ntf_cfm const *)(param);
								
						#ifdef ENABLE_DCI_SERVICE
						if(msg_param->handle > DEBUG_CRASH_INFO_SVC_START &&
							 msg_param->handle <= DEBUG_CRASH_INFO_SVC_END)
						{
								// notification confirmation for the DCI service
								dci_svc_notification_cfm_handler(msg_param);
						}
						#endif // ENABLE_DCI_SERVICE
						
						break;
				} 
				
				default:
					break;  
		}
}

uint8_t user_cust_svc_validate_write_handler(uint16_t att_idx, uint16_t offset, uint16_t length, uint8_t* value)
{
		uint8_t error = ATT_ERR_WRITE_NOT_PERMITTED;
		
		#ifdef ENABLE_DCI_SERVICE
		if (att_idx > DEBUG_CRASH_INFO_SVC_START &&
			  att_idx <= DEBUG_CRASH_INFO_SVC_END)
		{
				error = dci_svc_validate_write_handler(att_idx, offset, length, value);
		}
		#endif // ENABLE_DCI_SERVICE
    
    return error;
}

/// @} APP
