/**
 ****************************************************************************************
 *
 * @file user_empty_peripheral_template.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (c) 2012-2021 Renesas Electronics Corporation and/or its affiliates
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

#include "user_svc_data_beacon.h"

typedef struct
{
	///Payload length, BLE length byte will get calculated in helper function
	uint8_t 	len;
	///GAP Flag
	uint8_t 	type;
	///Payload data
	uint8_t 	*data;
	
}gap_adv_struct_t;
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
/* 3 bytes are added by the ROM, which are required from the the BT SIG*/
#define BLE_ADV_MAX_LEN				(	ADV_DATA_LEN -3)
/*Scan response does not require the additional 3 bytes*/
#define BLE_SCAN_RSP_MAX_LEN		(ADV_DATA_LEN)
/*Include 2 extra bytes required in the payload for the 16-bit UUID*/
#define SERIAL_NUM_PAYLOAD_LEN	(APP_DIS_SERIAL_NB_STR_LEN + 2)

//Total advertising elements - Service data, local name
#define TOTAL_ADV_STRUCTS			(2)

/**
 ****************************************************************************************
 * @brief Serializes advertising data.
 * @param[in] dst_len - destination len
 * @param[in] src_len - source length 
 * @param[in] src - pointer to original advertising structure
 * @param[out] Total advertising data length
 * @return true if successful and false otherwise
 ****************************************************************************************
 */
static bool ad_format_serialize(uint8_t dst_len, uint8_t *dst, uint8_t src_len, gap_adv_struct_t *src, uint8_t *written)
{
	uint8_t i, wr_index;
	
	for(i = 0, wr_index = 0; i < src_len; i++)
	{
		if(wr_index >=dst_len)
		{
				return false;
		}
		
		//Add 1 to adhere to BLE spec
		dst[wr_index++] = src[i].len + 1;
		dst[wr_index++] = src[i].type;
		memcpy(&dst[wr_index], src[i].data, src[i].len);
		wr_index += src[i].len;
	}
	
	*written = wr_index;
	return true;
}
/**
 ****************************************************************************************
 * @brief Set adverstising data and start advertising
 * @param[in] adv_struct_len - advertising structure len
 * @param[in] ad - pointer to array of gap_adv_struct_t 
 * @param[in] src - pointer to original advertising structure
 * @param[in] scan_res_struct_len - scan response structure len
 * @param[in] adv_mode - advertising mode 
 * @return void
 ****************************************************************************************
 */
static void ble_gap_adv_ad_struct_set(uint8_t adv_struct_len, gap_adv_struct_t *ad, 
																				uint8_t scan_res_struct_len, gap_adv_struct_t *sd, uint8_t adv_mode)
{
	
	uint8_t ad_data[BLE_ADV_MAX_LEN];
	uint8_t ad_data_len;
	
	uint8_t sd_data[ADV_DATA_LEN];
	uint8_t sd_data_len;
	
	bool serial_success = ad_format_serialize(BLE_ADV_MAX_LEN, ad_data, adv_struct_len, ad, &ad_data_len); 
	
	if(scan_res_struct_len > 0)
	{
		serial_success |= ad_format_serialize(ADV_DATA_LEN, sd_data, scan_res_struct_len, sd, &sd_data_len);
	}
	else
	{
		sd_data_len = 0;
	}
	
	 

	struct gapm_start_advertise_cmd *cmd = KE_MSG_ALLOC(GAPM_START_ADVERTISE_CMD,
                                                        TASK_GAPM,
                                                        TASK_APP,
                                                        gapm_start_advertise_cmd);
	cmd->op.code = adv_mode;
	cmd->op.addr_src = user_adv_conf.addr_src;
	cmd->intv_min = user_adv_conf.intv_min;
	cmd->intv_max = user_adv_conf.intv_max;
	cmd->channel_map = user_adv_conf.channel_map;
	cmd->info.host.mode = user_adv_conf.mode;
	cmd->info.host.adv_filt_policy = user_adv_conf.adv_filt_policy;
	cmd->info.host.adv_data_len = ad_data_len;
	memcpy(&(cmd->info.host.adv_data[0]), ad_data, ad_data_len);
	cmd->info.host.scan_rsp_data_len = sd_data_len;
	memcpy(&(cmd->info.host.scan_rsp_data[0]), sd_data, sd_data_len);
	
	ke_msg_send(cmd);

	
}

/**
 ****************************************************************************************
 * @brief user advertise operation
 * @note - SDK pointer to the default advertise operation
 * @return void
 ****************************************************************************************
 */
void user_advertise_operation(void)
{
	
	uint8_t dev_name[USER_DEVICE_NAME_LEN];
	uint8_t serial_num_struct[SERIAL_NUM_PAYLOAD_LEN];
	
	/*
	* Manufacturer SN UUID 0x2A25 (ATT_CHAR_SERIAL_NB)
	*/
	serial_num_struct[0] = 0x25;
	serial_num_struct[1] = 0x2A;
	
	memcpy(dev_name, USER_DEVICE_NAME, USER_DEVICE_NAME_LEN);
	memcpy(&serial_num_struct[2], APP_DIS_SERIAL_NB_STR, APP_DIS_SERIAL_NB_STR_LEN);
	
	gap_adv_struct_t adv_data[TOTAL_ADV_STRUCTS] = 
	{
		{
			.len = SERIAL_NUM_PAYLOAD_LEN,
			.type = GAP_AD_TYPE_SERVICE_16_BIT_DATA,
			.data = (uint8_t *)serial_num_struct
		} ,
		
		{
			.len = USER_DEVICE_NAME_LEN,
			.type = GAP_AD_TYPE_COMPLETE_NAME,
			.data = (uint8_t *)dev_name
			
		}
		
	};
	
	uint8_t total_adv_data_len = 2 + SERIAL_NUM_PAYLOAD_LEN + 2 + USER_DEVICE_NAME_LEN;
	ASSERT_ERROR(total_adv_data_len <= BLE_ADV_MAX_LEN);
	
	ble_gap_adv_ad_struct_set(TOTAL_ADV_STRUCTS,adv_data, 0, NULL, GAPM_ADV_NON_CONN);
}

/**
 ****************************************************************************************
 * @brief on connection callback
 * @param[in] - connection_idx - connection index
 * @param[in] - param 
 * @return void
 ****************************************************************************************
 */
void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}
/**
 ****************************************************************************************
 * @brief on disconnect callback
 * @param[in] - param - disconnect structure
 * @return void
 ****************************************************************************************
 */
void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}
/**
 ****************************************************************************************
 * @brief Called after stack configuration
 * @return void
 ****************************************************************************************
 */
void user_app_on_set_dev_config_complete(void)
{
		default_app_on_set_dev_config_complete();
}

/// @} APP
