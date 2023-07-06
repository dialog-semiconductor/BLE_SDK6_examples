#include "dci_svc.h"
#include "dci.h"
#include "user_custs1_def.h"
#include "custs1.h"
#include "gatt.h"
#include "gattc.h"

#ifdef ENABLE_DCI_SERVICE

#define ATT_OVERHEAD								3

typedef enum
{
		DCI_SERVICE_COMMAND_NONE,
		DCI_SERVICE_COMMAND_GET_ALL_RESET_DATA,
		DCI_SERVICE_COMMAND_GET_NUM_RESETS,
		DCI_SERVICE_COMMAND_MAX = 0xFF,
		
} dci_svc_command_t;

typedef __PACKED_STRUCT
{
		uint16_t current_command; 
		uint16_t length;
} dci_svc_rsp_hdr_t;

typedef __PACKED_STRUCT
{
		dci_svc_rsp_hdr_t hdr;
		uint8_t data[];
} dci_svc_rsp_t;

typedef __PACKED_STRUCT
{
		dci_svc_rsp_hdr_t hdr;
		dci_data_t dci_data;
} dci_svc_get_all_reset_rsp_t;

typedef __PACKED_STRUCT
{
		dci_svc_rsp_hdr_t hdr;
		uint8_t num_resets;
} dci_svc_get_num_reset_rsp_t;

typedef enum 
{
    CCC_STOP = 0x0000,
    CCC_NOTIFICATION,
    CCC_INDICATION
} ccc_t;

typedef struct
{
		uint16_t ccc;
		uint8_t mtu_size;
		uint8_t rsp_index;
		uint8_t current_command; 
		dci_svc_rsp_t *response;
} dci_svc_env_t;

static void continue_response_send(uint16_t length, uint8_t* value);
static void free_response(void);
static void handle_ccc_write(uint16_t ccc);
static void handle_dci_svc_write(dci_svc_command_t command, uint8_t conn_idx);
static void handle_get_all_data_command(uint8_t conn_idx);
static void handle_get_all_data_notification_cfm(void);
static void handle_get_number_of_resets_command(uint8_t conn_idx);
static void handle_get_number_of_resets_notification_cfm(void);
static void start_response_send(uint16_t length, dci_svc_rsp_t* rsp);
static uint8_t validate_ccc_write(uint16_t length, uint8_t *value);
static uint8_t validate_rx_char_write(uint16_t length, uint8_t *value);

// DCI Service Enviornment variable
dci_svc_env_t dci_svc_env;

/****************************************************************************************
 * @brief Continue responding to a DCI Service command by sending a notification
					on the TX characteristic.
 * @param[in] length   		Length of data to send
 * @param[in] value				Pointer to data to send
 * @return none
 ****************************************************************************************
*/
static void continue_response_send(uint16_t length, uint8_t* value)
{
    struct custs1_val_ntf_ind_req* req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																													prf_get_task_from_id(TASK_ID_CUSTS1),
																													TASK_APP,
																													custs1_val_ntf_ind_req,
																													length);

    req->conidx = app_env->conidx;
		req->notification = true;
    req->handle = DEBUG_CRASH_INFO_TX_VAL;
    req->length = length;
    memcpy(req->value, value, length);

    ke_msg_send(req);
}

void dci_svc_cleanup_on_disconnect(void)
{
		if(dci_svc_env.current_command != DCI_SERVICE_COMMAND_NONE)
		{
				// If a response was in progress, free the response buffer
				free_response();
		}
}

void dci_svc_init(void)
{
		memset(&dci_svc_env, 0, sizeof(dci_svc_env_t));
}

void dci_svc_notification_cfm_handler(struct custs1_val_ntf_cfm const *param)
{
		if(param->handle == DEBUG_CRASH_INFO_TX_VAL && 
			 param->status == ATT_ERR_NO_ERROR && 
			 dci_svc_env.current_command != DCI_SERVICE_COMMAND_NONE)
		{
				switch(dci_svc_env.current_command)
				{
						case DCI_SERVICE_COMMAND_GET_ALL_RESET_DATA:
						{
								handle_get_all_data_notification_cfm();
								break;
						}
						case DCI_SERVICE_COMMAND_GET_NUM_RESETS:
						{
								handle_get_number_of_resets_notification_cfm();
								break;
						}
						default:
								break;
				}	
		}
}

uint8_t dci_svc_validate_write_handler(uint16_t att_idx, uint16_t offset, uint16_t length, uint8_t* value)
{
	 uint8_t error = ATT_ERR_WRITE_NOT_PERMITTED;
    
		// determine which characteristic request is for
		switch(att_idx)
		{
				case DEBUG_CRASH_INFO_RX_VAL:
				{
						error = validate_rx_char_write(length, value);
						break;
				}
				case DEBUG_CRASH_INFO_TX_CCC:
				{	
						error = validate_ccc_write(length, value);
						break;
				}
				default:
				{
						error = ATT_ERR_WRITE_NOT_PERMITTED;
						break;
				}
		}
    
    return error;
}

void dci_svc_write_handler(ke_msg_id_t const msgid,
                           struct custs1_val_write_ind const *param,
                           ke_task_id_t const dest_id,
                           ke_task_id_t const src_id)
{
    // determine which characteristic request is for
    switch(param->handle)
    {
        case DEBUG_CRASH_INFO_RX_VAL:
				{
						handle_dci_svc_write((dci_svc_command_t)param->value[0], KE_IDX_GET(src_id));
						break;
				}
				case DEBUG_CRASH_INFO_TX_CCC:
				{
						uint16_t ccc = param->value[0] | (param->value[1] << 8);
						handle_ccc_write(ccc);
						break;
				}
        default:
            break;
    }
}

/****************************************************************************************
 * @brief Free the DCI Service response buffer
 * @return none
 ****************************************************************************************
*/
static void free_response()
{
		ke_free(dci_svc_env.response);
		dci_svc_env.rsp_index = 0;
		dci_svc_env.current_command = DCI_SERVICE_COMMAND_NONE;
}

/****************************************************************************************
 * @brief Handle writes to the TX characteristic CCC
 * @param[in] ccc   	new ccc value
 * @return none
 ****************************************************************************************
*/
static void handle_ccc_write(uint16_t ccc)
{
		dci_svc_env.ccc = ccc;
}

/****************************************************************************************
 * @brief Handle writes to the RX characteristic
 * @param[in] command   	Incoming DCI service command
 * @param[in] conn_idx   	Connection index of the client
 * @return none
 ****************************************************************************************
*/
static void handle_dci_svc_write(dci_svc_command_t command, uint8_t conn_idx)
{	
		// determine which characteristic request is for
    switch(command)
    {
        case DCI_SERVICE_COMMAND_GET_ALL_RESET_DATA:
				{
						handle_get_all_data_command(conn_idx);
						break;
				}
				case DCI_SERVICE_COMMAND_GET_NUM_RESETS:
				{
						handle_get_number_of_resets_command(conn_idx);
						break;
				}
        default:
            break;
    }
}

/****************************************************************************************
 * @brief Handle requests for all DCI service reset data
 * @param[in] conn_idx   	Connection index of the client
 * @return none
 ****************************************************************************************
*/
static void handle_get_all_data_command(uint8_t conn_idx)
{	
		// Ensure client is subscribed to notifications
		if(dci_svc_env.ccc)
		{	
				dci_svc_env.current_command = DCI_SERVICE_COMMAND_GET_ALL_RESET_DATA;
			
				// Get the crash info
				dci_info_t crash_info;
				dci_get_data(&crash_info);
						
				uint16_t len_to_tx;
				if (crash_info.data->num_resets > 0)
				{
						// Reset data available
					
						// calculate length of valid reset data
						uint16_t invalid_fault_data_len = ((DCI_NUM_RESETS_STORED - crash_info.data->num_resets) * sizeof(dci_fault_info_t));
						uint16_t dci_valid_data_len = sizeof(dci_data_t) - invalid_fault_data_len;
					
						// allocate the response buffer
						uint16_t buffer_size = sizeof(dci_svc_rsp_hdr_t) + dci_valid_data_len;
						dci_svc_get_all_reset_rsp_t *response = (dci_svc_get_all_reset_rsp_t*)ke_malloc(buffer_size, KE_MEM_ENV);
						
						// set the response data
						response->hdr.current_command = dci_svc_env.current_command;
						response->hdr.length = dci_valid_data_len;
						memcpy(&response->dci_data, crash_info.data, response->hdr.length);
		 
						// keep track of the response buffer
						dci_svc_env.response = (dci_svc_rsp_t*)response;
						
						// Calculate how many bytes can be transmitted in one packet
						dci_svc_env.mtu_size = gattc_get_mtu(conn_idx); 
						uint16_t remaining_bytes = sizeof(dci_svc_rsp_hdr_t) + response->hdr.length;
						uint16_t max_bytes_in_packet = dci_svc_env.mtu_size - ATT_OVERHEAD;
						len_to_tx = remaining_bytes > max_bytes_in_packet ? max_bytes_in_packet : remaining_bytes;
				}
				else
				{
						// Respond with length zero to indicate no reset data available
					
						// allocate the response buffer
						uint16_t buffer_size = sizeof(dci_svc_rsp_hdr_t);
						dci_svc_env.response = (dci_svc_rsp_t*)ke_malloc(buffer_size, KE_MEM_ENV);
						
						// set the response data
						dci_svc_env.response->hdr.current_command = dci_svc_env.current_command;
						dci_svc_env.response->hdr.length = 0;
						len_to_tx = sizeof(dci_svc_rsp_hdr_t);
				}
				
				// start sending the response
				start_response_send(len_to_tx, dci_svc_env.response);
				dci_svc_env.rsp_index = len_to_tx;
		}
}

/****************************************************************************************
 * @brief Notification confirmation handler for DCI_SERVICE_COMMAND_GET_ALL_RESET_DATA command. 
					Continue sending data if data left to transmit
 * @return none
 ****************************************************************************************
*/
static void handle_get_all_data_notification_cfm()
{
		// Data is sent in the form: <current_commmand, length, data>
		// Here we check if all the data has been transmtted yet
		if(dci_svc_env.rsp_index != (sizeof(dci_svc_rsp_hdr_t) + dci_svc_env.response->hdr.length))
		{
				// Ensure notifications still enabled
				if(dci_svc_env.ccc)
				{	
						// Calculate how many bytes can be transmitted in one packet
						uint16_t remaining_bytes = (sizeof(dci_svc_rsp_hdr_t) + dci_svc_env.response->hdr.length) - dci_svc_env.rsp_index;
						uint16_t max_bytes_in_packet = dci_svc_env.mtu_size - ATT_OVERHEAD;
						uint16_t len_to_tx = remaining_bytes > max_bytes_in_packet ? max_bytes_in_packet : remaining_bytes;
						
						continue_response_send(len_to_tx,((uint8_t*)dci_svc_env.response)+dci_svc_env.rsp_index);
						dci_svc_env.rsp_index += len_to_tx;
				}
		}
		else
		{
				// free response buffer
				free_response();
				
				// clear existing reset data as it has been transmitted to a client
				dci_clear_data();
		}
}

/****************************************************************************************
 * @brief Handle requests for number of resets
 * @param[in] conn_idx   	Connection index of the client
 * @return none
 ****************************************************************************************
*/
static void handle_get_number_of_resets_command(uint8_t conn_idx)
{
		// Ensure client is subscribed to notifications
		if(dci_svc_env.ccc)
		{	
				dci_svc_env.current_command = DCI_SERVICE_COMMAND_GET_NUM_RESETS;
			
				// Get the crash info
				dci_info_t crash_info;
				dci_get_data(&crash_info);
			
				// allocate the response buffer
				dci_svc_get_num_reset_rsp_t* response = (dci_svc_get_num_reset_rsp_t*)ke_malloc(sizeof(dci_svc_get_num_reset_rsp_t), KE_MEM_ENV);
	
				// set the response data
				response->hdr.current_command = dci_svc_env.current_command;
				response->hdr.length = sizeof(crash_info.data->num_resets);
				response->num_resets = crash_info.data->num_resets;

				// keep track of the response buffer
				dci_svc_env.response = (dci_svc_rsp_t*)response;
			
				// send the response
				start_response_send(sizeof(dci_svc_get_num_reset_rsp_t), dci_svc_env.response);
		}
}

/****************************************************************************************
 * @brief Notification confirmation handler for DCI_SERVICE_COMMAND_GET_NUM_RESETS command. 
					All data has been sent. Reset the response structure
 * @return none
 ****************************************************************************************
*/
static void handle_get_number_of_resets_notification_cfm()
{
		// free response buffer
		free_response();
}

/****************************************************************************************
 * @brief Start responding to a DCI Service command by sending a notification
					on the TX characteristic.
 * @param[in] length   		Length of data to send
 * @param[in] rsp					Pointer to response data
 * @return none
 ****************************************************************************************
*/
static void start_response_send(uint16_t length, dci_svc_rsp_t* rsp)
{
    struct custs1_val_ntf_ind_req* req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																													prf_get_task_from_id(TASK_ID_CUSTS1),
																													TASK_APP,
																													custs1_val_ntf_ind_req,
																													length);

    req->conidx = app_env->conidx;
		req->notification = true;
    req->handle = DEBUG_CRASH_INFO_TX_VAL;
    req->length = rsp->hdr.length+sizeof(dci_svc_rsp_hdr_t);
		memcpy(&req->value[0], (void*)&rsp->hdr.current_command, sizeof(rsp->hdr.current_command));
		memcpy(&req->value[2], (void*)&rsp->hdr.length, sizeof(rsp->hdr.length));
    memcpy(&req->value[4], rsp->data, length-sizeof(dci_svc_rsp_hdr_t));

    ke_msg_send(req);
}

/****************************************************************************************
 * @brief Validate write requests to the TX characteristic CCC
 * @param[in] length   		Length of data to send
 * @param[in] value				Pointer to data to send
* @return error code indicating write is accepted or reason for rejection (see enum hl_err in rwble_hl_error.h)
 ****************************************************************************************
*/
static uint8_t validate_ccc_write(uint16_t length, uint8_t *value)
{
		uint8_t error = ATT_ERR_WRITE_NOT_PERMITTED;
	
		if(length == sizeof(uint16_t))
		{
				uint16_t ccc = value[0] | (value[1] << 8);
				if(ccc == CCC_STOP || ccc == CCC_NOTIFICATION)
				{
						error = ATT_ERR_NO_ERROR;
				}
		}
		return error;
}

/****************************************************************************************
 * @brief Validate write requests to the RX characteristic
 * @param[in] length   		Length of data to send
 * @param[in] value				Pointer to data to send
* @return error code indicating write is accepted or reason for rejection (see enum hl_err in rwble_hl_error.h)
 ****************************************************************************************
*/
static uint8_t validate_rx_char_write(uint16_t length, uint8_t *value)
{
		uint8_t error = ATT_ERR_WRITE_NOT_PERMITTED;
	
		// Client must be subscribed to notifications on the TX characteristic to receive responses
		if(dci_svc_env.ccc != CCC_NOTIFICATION)
				return PRF_CCCD_IMPR_CONFIGURED;
		
		// response not pointing anywhere
		if(dci_svc_env.current_command != DCI_SERVICE_COMMAND_NONE)
			return PRF_PROC_IN_PROGRESS;
	
		// Verify request is valid
		if(length == DCI_SVC_RX_CHAR_LEN)
		{
				if(value[0] == DCI_SERVICE_COMMAND_GET_ALL_RESET_DATA ||
					 value[0] == DCI_SERVICE_COMMAND_GET_NUM_RESETS)
				{
						error = ATT_ERR_NO_ERROR;
				}
		}
		return error;
}

#endif // ENABLE_DCI_SERVICE
