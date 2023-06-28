/**
 ****************************************************************************************
 *
 * @file user_dyn_custom_svc.c
 *
 * @brief Dynamic custom service project source code.
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

#include "app_api.h"
#include "user_dyn_custom_svc.h"
#include "ke_msg.h"
#include "arch_console.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
void add_char_to_svc_req(struct gattm_add_svc_req* req, att_desc_t *att_descs, uint8_t num_attr);
struct gattm_add_svc_req* create_add_service_req(gattm_add_svc_req_cmd_t *param);
static void initialize_custom_db(void);
static void notify_characteristic2(void);

static uint16_t start_handle_custom_service_1;
static uint16_t service1_char2_ccc;

/**
 ****************************************************************************************
 * @brief Add characteristic to an 'add service' request
 * @param[in] req - pointer to the service request we are adding characteristic to 
 * @param[in] att_descs - array of attributes which comprise the characteristic
 * @param[in] num_attr - number of attributes in the characteristic
 * @param[out] req - pointer to service request with the added characteristic
 * @return void
 ****************************************************************************************
 */
void add_char_to_svc_req(struct gattm_add_svc_req* req, att_desc_t *att_descs, uint8_t num_attr)
{									
		for(int i = 0; i < num_attr; i++)
		{
				struct gattm_att_desc *att_cfg;
				att_cfg = (struct gattm_att_desc*)&req->svc_desc.atts[att_descs[i].attr_idx];
				uint8_t len = att_descs[i].uuid_info.type == ATT_UUID_128 ? ATT_UUID_128_LEN : ATT_UUID_16_LEN;
				memcpy(att_cfg->uuid, &att_descs[i].uuid_info.uuid, len);
				
				att_cfg->perm = att_descs[i].perm; // see attm_perm_mask
				att_cfg->max_len = att_descs[i].max_len | att_descs[i].read_indication; // see attm_value_perm_mask
		}
}

/**
 ****************************************************************************************
 * @brief Create a request to dynamically add a service
 * @param[in] cmd - pointer to the add service command structure 
 * @return pointer the created request
 ****************************************************************************************
 */
struct gattm_add_svc_req* create_add_service_req(gattm_add_svc_req_cmd_t *cmd)
{
    struct gattm_add_svc_req *req = KE_MSG_ALLOC_DYN(GATTM_ADD_SVC_REQ,
																										 TASK_GATTM,
																										 TASK_APP,
																										 gattm_add_svc_req,
																										 (cmd->num_attrs)*sizeof(struct gattm_att_desc)); // 1 attribute accounted for by default in struct gattm_add_svc_req
																									 

    req->svc_desc.start_hdl = 0; // assign automatically
    req->svc_desc.task_id = TASK_APP;
    req->svc_desc.perm |= ((cmd->svc_type << PERM_POS_SVC_PRIMARY) | // primary or secondary service
													(cmd->uuid_info.type == ATT_UUID_128 ? (SVC_PERM_ATT_UUID_LEN_128 << PERM_POS_SVC_UUID_LEN) : SVC_PERM_ATT_UUID_LEN_16) | // uuid length
													(cmd->svc_perm << PERM_POS_SVC_AUTH)); // service permission
													// encryption key size must be 16 = No
													// Multi-instantiated task = No;

    req->svc_desc.nb_att = cmd->num_attrs; // number of attributes
		
		// copy the uuid
		uint8_t len = cmd->uuid_info.type == ATT_UUID_128 ? ATT_UUID_128_LEN : ATT_UUID_16_LEN;
		memcpy(req->svc_desc.uuid, &cmd->uuid_info.uuid, len);

		return req;
}

/**
 ****************************************************************************************
 * @brief Add a custom service to the database
 * @return void
 ****************************************************************************************
 */
void create_custom_service(void)
{	
		// create the service reqeust
		gattm_add_svc_req_cmd_t cmd =
		{
				.svc_type = GATT_SERVICE_PRIMARY,
				.num_attrs = CUSTOM_SVC_NUM_ATTR,
				.svc_perm = PERM_RIGHT_ENABLE, 
				.uuid_info.type = ATT_UUID_128,
				.uuid_info.uuid = {.uuid128 = CUSTOM_SERVICE_1_UUID_128},
		};
	
		struct gattm_add_svc_req *req = create_add_service_req(&cmd);

		// define the first characteristic
		att_desc_t char_1[CHAR1_NUM_ATTR] =
		{
				// attribute 1
				{
						.attr_idx = CUSTOM_ATTR_HANDLE_OFFSET_DECLARATION_CHAR1 - 1,
						.uuid_info.type = ATT_UUID_16,
						.uuid_info.uuid = {.uuid16 = ATT_DECL_CHARACTERISTIC},
						.perm = (att_perm_t)(ATT_PERM_UUID_LEN_16_BITS |
																 ATT_PERM_ENABLE_READ),
						.max_len = 0, 
						.read_indication = READ_INDICATION_DISABLED,
				},
		
				// attribute 2
				{
						.attr_idx = CUSTOM_ATTR_HANDLE_OFFSET_VALUE_CHAR1 - 1,
						.uuid_info.type = ATT_UUID_128,
						.uuid_info.uuid = {.uuid128 = CUSTOM_SERVICE_CHAR_1_UUID_128},
						.perm = (att_perm_t)(ATT_PERM_UUID_LEN_128_BITS | 
																 ATT_PERM_WRITE_REQUEST | 
																 ATT_PERM_ENABLE_WRITE | 
																 ATT_PERM_ENABLE_READ),
						.max_len = CUSTOM_SERVICE_CHAR_1_LEN,
						.read_indication = READ_INDICATION_ENABLED,
				},
				
				// attribute 3
				{
						.attr_idx = CUSTOM_ATTR_HANDLE_OFFSET_USER_DESC_CHAR1 - 1,
						.uuid_info.type = ATT_UUID_16,
						.uuid_info.uuid = {.uuid16 = ATT_DESC_CHAR_USER_DESCRIPTION},
						.perm = (att_perm_t)(ATT_PERM_UUID_LEN_16_BITS | 
																 ATT_PERM_ENABLE_READ),
						.max_len = sizeof(CUSTOM_SERVICE_CHAR_1_USER_DESC),
						.read_indication = READ_INDICATION_DISABLED,
				},
		};
				
		// add the characteristic to the service request
		add_char_to_svc_req(req, char_1, CHAR1_NUM_ATTR);
		
		// define the second characteristic
		att_desc_t char_2[CHAR2_NUM_ATTR] =
		{
				// attribute 1
				{
						.attr_idx = CUSTOM_ATTR_HANDLE_OFFSET_DECLARATION_CHAR2 - 1,
						.uuid_info.type = ATT_UUID_16,
						.uuid_info.uuid = {.uuid16 = ATT_DECL_CHARACTERISTIC},
						.perm = (att_perm_t)ATT_PERM_ENABLE_READ,
						.max_len = 0,
						.read_indication = READ_INDICATION_DISABLED,
				},
				
				// attribute 2
				{
						.attr_idx =  CUSTOM_ATTR_HANDLE_OFFSET_VALUE_CHAR2 - 1,
						.uuid_info.type = ATT_UUID_128,
						.uuid_info.uuid = {.uuid128 = CUSTOM_SERVICE_CHAR_2_UUID_128},
						.perm = (att_perm_t)(ATT_PERM_UUID_LEN_128_BITS | 
																 ATT_PERM_WRITE_REQUEST | 
																 ATT_PERM_ENABLE_WRITE | 
																 ATT_PERM_ENABLE_NOTIFICATION),
						.max_len = 4,
						.read_indication = READ_INDICATION_DISABLED,
				},
				
				// attribute 3
				{
						.attr_idx = CUSTOM_ATTR_HANDLE_OFFSET_USER_DESC_CHAR2 - 1,
						.uuid_info.type = ATT_UUID_16,
						.uuid_info.uuid = {.uuid16 = ATT_DESC_CHAR_USER_DESCRIPTION},
						.perm = (att_perm_t)(ATT_PERM_UUID_LEN_16_BITS | 
																 ATT_PERM_ENABLE_READ),
						.max_len = sizeof(CUSTOM_SERVICE_CHAR_2_USER_DESC),
						.read_indication = READ_INDICATION_DISABLED,
				},
						
				// attribute 4				
				{
						.attr_idx = CUSTOM_ATTR_HANDLE_OFFSET_CCC_CHAR2 - 1,
						.uuid_info.type = ATT_UUID_16, 
						.uuid_info.uuid = {.uuid16 = ATT_DESC_CLIENT_CHAR_CFG},
						.perm = (att_perm_t)(ATT_PERM_UUID_LEN_16_BITS | 
																 ATT_PERM_WRITE_REQUEST | 
																 ATT_PERM_ENABLE_WRITE | 
																 ATT_PERM_ENABLE_READ),
						.max_len = CUSTOM_SERVICE_CHAR_2_LEN,
						.read_indication = READ_INDICATION_ENABLED,
				},
		};
		
		// add the characteristic to the service request
		add_char_to_svc_req(req, char_2, CHAR2_NUM_ATTR);
		
		// send the request to the kernel
    ke_msg_send((void *) req);
}

/**
 ****************************************************************************************
 * @brief Handler for the @ref GATTC_READ_REQ_IND message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return void
 ****************************************************************************************
 */
void handle_gattc_read_req_ind(ke_msg_id_t msgid,
															 struct gattc_read_req_ind const *param,
															 ke_task_id_t dest_id,
															 ke_task_id_t src_id)	
{
		uint8_t error = ATT_ERR_NO_ERROR;
		uint16_t len = 0;
		uint8_t *data = NULL;

		uint32_t random_value = co_rand_word();
		switch(param->handle - start_handle_custom_service_1)
		{
				case CUSTOM_ATTR_HANDLE_OFFSET_VALUE_CHAR1:
						// for the first characteristic, return random data
						data = (uint8_t *) &random_value;
						len = sizeof(random_value);
						break;

				case CUSTOM_ATTR_HANDLE_OFFSET_CCC_CHAR2:
						// return the current value of the ccc
						data = (uint8_t*)&service1_char2_ccc;
						len = sizeof(service1_char2_ccc);
						break;

				default:
						error = ATT_ERR_REQUEST_NOT_SUPPORTED;
						break;
		}
		
		// send read confirmation
		struct gattc_read_cfm *cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM,
																									src_id,
																									dest_id,
																									gattc_read_cfm,
																									len);

		cfm->handle = param->handle;
		cfm->status = error;
		cfm->length = len;
		memcpy(cfm->value, data, len);
		
		arch_printf("Received read request for param->handle: %d. Responding with: ", param->handle);
		for(int i = 0; i<len; i++)
			arch_printf("0x%02X ", cfm->value[i]);
		arch_printf("\r\n");

		ke_msg_send((void *) cfm);
}

/**
 ****************************************************************************************
 * @brief Handler for the @ref GATTC_WRITE_REQ_IND message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return void
 ****************************************************************************************
 */
void handle_gattc_write_req_ind(ke_msg_id_t msgid,
																struct gattc_write_req_ind const *param,
																ke_task_id_t dest_id,
																ke_task_id_t src_id)
{

			arch_printf("Received data written to param->handle: %d\r\n", param->handle);
			arch_printf("\tConn idx: %d\r\n", (src_id >> 8) & 0xFF);
			arch_printf("\tLength: %d\r\n", param->length);
			arch_printf("\tData: ");
			for(int i = 0; i<param->length; i++)
					arch_printf("0x%02X ", param->value[i]);
			arch_printf("\r\n");
	
			switch(param->handle - start_handle_custom_service_1)
			{
					case CUSTOM_ATTR_HANDLE_OFFSET_VALUE_CHAR1:
					{
							// if notifications are enabled on char2 when there is a write on char1
							// send a notification to char2 with random data
							if(service1_char2_ccc == GATT_CCC_NOTIFICATIONS)
								notify_characteristic2();
							break;
					}
					case CUSTOM_ATTR_HANDLE_OFFSET_CCC_CHAR2:
					{
							uint16_t new_val = param->value[0] | (param->value[1] << 8);
							update_custom_service_ccc(new_val);
							break;
					}
					default:
							break;
			}

			// send write confirmation
			struct gattc_write_cfm *cfm = KE_MSG_ALLOC_DYN(GATTC_WRITE_CFM,
																										 src_id,
																										 dest_id,
																										 gattc_write_cfm,
																										 0);
			cfm->handle = param->handle;
			cfm->status = ATT_ERR_NO_ERROR;

			ke_msg_send((void *) cfm);
}

/**
 ****************************************************************************************
 * @brief Handler for the @ref GATTM_ADD_SVC_RSP message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return void
 ****************************************************************************************
 */
void handle_gattm_add_svc_rsp(ke_msg_id_t msgid,
                                  struct gattm_add_svc_rsp *param,
                                  ke_task_id_t dest_id,
                                  ke_task_id_t src_id)
{
		switch(param->status)
		{
				case ATT_ERR_NO_ERROR:
						start_handle_custom_service_1 = param->start_hdl; // track the start handle of the service
						initialize_custom_db();
						break;

				case ATT_ERR_INVALID_HANDLE:
						printf("Error creating custom DB, invalid handle\n");
						break;

				case GAP_ERR_INVALID_PARAM:
						printf("Error creating custom DB, invalid param\n");
						break;

				case GATT_ERR_INVALID_PERM:
						printf("Error creating custom DB, invalid permission\n");
						break;

				default:
						printf("Error creating custom DB: %04X\n", param->status);
						break;
		}
}

/**
 ****************************************************************************************
 * @brief Handler for the @ref GATTM_ATT_SET_VALUE_RSP message
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return void
 ****************************************************************************************
 */
void handle_gattm_att_set_value_rsp(ke_msg_id_t msgid,
                                    struct gattm_att_set_value_rsp *param,
                                    ke_task_id_t dest_id,
                                    ke_task_id_t src_id)																		
{
	
}

/**
 ****************************************************************************************
 * @brief Initialize custom service database with data
 * @return void
 ****************************************************************************************
 */
void initialize_custom_db(void)
{

		// Set User Description values in database. 
		// The User Description permissions were set with READ_INDICATION_DISABLED
		// These values will be readable in the database, but the application will not receive
		// a GATTC_READ_REQ_IND message when they are read.
	
		struct gattm_att_set_value_req *req = KE_MSG_ALLOC_DYN(GATTM_ATT_SET_VALUE_REQ,
																														TASK_GATTM,
																														TASK_APP,
																														gattm_att_set_value_req,
																														sizeof(CUSTOM_SERVICE_CHAR_1_USER_DESC) - 1);

		req->handle = start_handle_custom_service_1 + CUSTOM_ATTR_HANDLE_OFFSET_USER_DESC_CHAR1;
    req->length = sizeof(CUSTOM_SERVICE_CHAR_1_USER_DESC) - 1;
    memcpy(req->value, CUSTOM_SERVICE_CHAR_1_USER_DESC, sizeof(CUSTOM_SERVICE_CHAR_1_USER_DESC) - 1);

    ke_msg_send((void *) req);
	
    req = KE_MSG_ALLOC_DYN(GATTM_ATT_SET_VALUE_REQ,
												   TASK_GATTM,
													 TASK_APP,
													 gattm_att_set_value_req,
												 	 sizeof(CUSTOM_SERVICE_CHAR_2_USER_DESC) - 1);

		req->handle = start_handle_custom_service_1 + CUSTOM_ATTR_HANDLE_OFFSET_USER_DESC_CHAR2;
    req->length = sizeof(CUSTOM_SERVICE_CHAR_2_USER_DESC) - 1;
    memcpy(req->value, CUSTOM_SERVICE_CHAR_2_USER_DESC, sizeof(CUSTOM_SERVICE_CHAR_2_USER_DESC) - 1);

    ke_msg_send((void *) req);
}

/**
 ****************************************************************************************
 * @brief Send a notifiation on custom service characteristic with a random value
 * @return void
 ****************************************************************************************
 */
void notify_characteristic2()
{
    uint32_t random_value = rand();
		
    struct gattc_send_evt_cmd *req = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD ,
                                                    TASK_GATTC,
                                                    TASK_APP,
                                                    gattc_send_evt_cmd,
                                                    sizeof(random_value));

    req->operation = GATTC_NOTIFY;
    req->seq_num = 0;
    req->handle = start_handle_custom_service_1 + CUSTOM_ATTR_HANDLE_OFFSET_VALUE_CHAR2;
    req->length = sizeof(random_value);
    memcpy(req->value, &random_value, sizeof(random_value));

		arch_printf("Sending notification for param->handle: %d. Notification data: ", req->handle);
		for(int i = 0; i<req->length; i++)
			arch_printf("0x%02X ", req->value[i]);
		arch_printf("\r\n");
		
    ke_msg_send((void *) req);
}

/**
 ****************************************************************************************
 * @brief Update the Client Characteristic Configuration
 * @return void
 ****************************************************************************************
 */
void update_custom_service_ccc(uint16_t new_val)
{
    if(new_val <= GATT_CCC_INDICATIONS)
		{
        service1_char2_ccc = new_val;
		}
}


/// @} APP