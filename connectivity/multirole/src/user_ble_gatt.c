/**
 ****************************************************************************************
 *
 * @file user_ble_gatt.c
 *
 * @brief User BLE GATT source code.
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
#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_ble_gatt.h"
#include "user_ble_common.h"


/**
 ****************************************************************************************
 * @brief Perform a service discovery
 * @param[in] con_idx - connection identifier
 * @param[in] con_idx - start handle to start discovery 
 * @return void
 ****************************************************************************************
 */
void user_gatt_discover_all_services(uint8_t con_idx, uint16_t start_handle)
{
	
		if(start_handle > 0)
		{
			struct gattc_sdp_svc_disc_cmd *cmd  = KE_MSG_ALLOC_DYN(GATTC_SDP_SVC_DISC_CMD,
																											 KE_BUILD_ID(TASK_GATTC, con_idx),
																											 TASK_APP,
																											 gattc_sdp_svc_disc_cmd,
																											 sizeof(struct gattc_sdp_svc_disc_cmd) );
			
			cmd->operation = GATTC_SDP_DISC_SVC_ALL;
			cmd->seq_num = 0;
			cmd->start_hdl = start_handle;
			cmd->end_hdl = 0x00FF;
			
			ke_msg_send(cmd);
	}
	
}

/**
 ****************************************************************************************
 * @brief Perform a gatt write
 * @param[in] con_idx - connection identifier
 * @param[in] handle - attribute handle to write 
 * @param[in] data - data to write
 * @param[in] data_len - data len
 * @return void
 ****************************************************************************************
 */
void user_ble_gatt_write(uint8_t op, uint8_t con_idx, uint16_t handle, uint8_t *data, uint8_t data_len)
{
	struct gattc_write_cmd *cmd  = KE_MSG_ALLOC_DYN(GATTC_WRITE_CMD,
																									 KE_BUILD_ID(TASK_GATTC, con_idx),
																									 TASK_APP,
																									 gattc_write_cmd,
																									 sizeof(struct gattc_write_cmd) );

	cmd->operation = op;
	cmd->auto_execute = 1;
	cmd->seq_num = 0;
	cmd->offset = 0;
	cmd->length = data_len;
	cmd->cursor = 0;
	cmd->handle = handle;
	memcpy(&cmd->value[0], data, data_len);

	ke_msg_send(cmd);
	
}

/**
 ****************************************************************************************
 * @brief Return number of characteristics in a service
 * @param[in] svc - pointer to the service discovered
 * @return number of characteristics
 ****************************************************************************************
 */
uint32_t user_ble_get_svc_size(struct gattc_sdp_svc_ind const *svc)
{
	
	uint16_t i;
	uint16_t num_items = 0;
	
	for (i = 0; i < svc->end_hdl - svc->start_hdl; i++) {
					uint8_t att_type = svc->info[i].att_type;

					/* Value is not a separate item - it's part of the characteristic item */
					if (att_type == GATTC_SDP_NONE || att_type == GATTC_SDP_ATT_VAL) {
									continue;
					}

					if(att_type == GATTC_SDP_ATT_CHAR)
						num_items++;
	}
	
	return ( sizeof(service_info_t) + num_items*sizeof(gattc_chars_t) );
}


/**
 ****************************************************************************************
 * @brief Perform a simple gatt read - read is returned by GATTC_READ_IND
 * @param[in] con_idx - connection identifier
 * @param[in] handle - attribute handle to read
 * @return void
 ****************************************************************************************
 */
void user_gatt_read_simple(uint8_t con_idx, uint16_t handle)
{
    struct gattc_read_cmd * req  = KE_MSG_ALLOC(GATTC_READ_CMD, KE_BUILD_ID(TASK_GATTC, con_idx),
            TASK_APP, gattc_read_cmd);
    //request type
    req->operation                       = GATTC_READ;
    req->nb                             = 1;
    req->req.simple.offset              = 0;
    req->req.simple.length              = 0;
    req->req.simple.handle              = handle;

    //send request to GATT
    ke_msg_send(req);
}

/**
 ****************************************************************************************
 * @brief Parse a service from a service discovered indication
 * @param[in] svc - service to parse
 * @param[in] con_idx - connection id
 * @param[out] svc_ret - parsed service
 * @return void
 ****************************************************************************************
 */
void user_gatt_parse_service(struct gattc_sdp_svc_ind const *svc, service_info_t *svc_ret, uint8_t con_idx)
{
	
	uint8_t i;
	svc_ret->num_chars = 0;
	svc_ret->svc_uuid.type = (svc->uuid_len == sizeof(uint16_t) ? ATT_UUID_16 : ATT_UUID_128);
	uint16_t current_handle;
	
	static uint8_t char_index = 0;
	
	if(svc_ret->svc_uuid.type == ATT_UUID_16){
		memcpy(&svc_ret->svc_uuid.uuid.uuid16, svc->uuid, svc->uuid_len);
		svc_ret->svc_uuid.type = ATT_UUID_16;
	}else
	{
		memcpy(svc_ret->svc_uuid.uuid.uuid128, svc->uuid, svc->uuid_len);
		svc_ret->svc_uuid.type = ATT_UUID_128;
	}
	
	for(i = 0; i < svc->end_hdl - svc->start_hdl; i++)
	{

		const union gattc_sdp_att_info *info = &svc->info[i];
		
		if (info->att_type == GATTC_SDP_NONE) {
					continue;
		}
		
		current_handle = svc->start_hdl + i + 1;
		switch(info->att_type)
		{
			case GATTC_SDP_ATT_VAL:
			{
				
				//struct gattc_sdp_att att;
				memcpy(&svc_ret->items[char_index].uuid.uuid, info->att.uuid, info->att.uuid_len);
				svc_ret->items[char_index].c.value_handle = current_handle;
				svc_ret->items[char_index].uuid.type = info->att.uuid_len == sizeof(uint16_t) ? ATT_UUID_16 : ATT_UUID_128;
				
				
			}break;
			/*Descriptor*/
			case GATTC_SDP_ATT_DESC:
			{
				
				    //struct gattc_sdp_att att;
				uint16_t att_ccc = ATT_DESC_CLIENT_CHAR_CFG;
				if( !memcmp(&att_ccc, info->att.uuid, sizeof(uint16_t))  && info->att.uuid_len == sizeof(uint16_t)  )
				{
					if(svc_ret->items[char_index].c.properties & GATT_PROP_NOTIFY)
					{
						uint16_t ccc = GATT_CCC_NOTIFICATIONS;
						
						user_ble_gatt_write(GATTC_WRITE, con_idx, current_handle, (uint8_t *)&ccc, sizeof(ccc));			
					}
					if(svc_ret->items[char_index].c.properties & GATT_PROP_INDICATE)
					{
						uint16_t ccc = GATT_CCC_INDICATIONS;
						
						user_ble_gatt_write(GATTC_WRITE, con_idx, current_handle, (uint8_t *)&ccc, sizeof(ccc));

					}
				
				}

				
			}break;
			/*Characteristic Declaration*/
			case GATTC_SDP_ATT_CHAR:
			{
				char_index  = svc_ret->num_chars;
				svc_ret->num_chars++;
				
				//struct gattc_sdp_att_char att_char
				memcpy(&svc_ret->items[char_index].uuid.uuid, info->att.uuid, info->att.uuid_len);
				svc_ret->items[char_index].c.properties = info->att_char.prop;
				svc_ret->items[char_index].handle = info->att_char.handle;
				
				
			
				
				break;
			}
			/*Included Service*/
			case GATTC_SDP_INC_SVC:
			{
				break;
			}
			default:
				break;
		}
			
		
	}
	
}
/// @} APP

