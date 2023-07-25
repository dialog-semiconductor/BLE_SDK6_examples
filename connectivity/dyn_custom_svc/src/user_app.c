/**
 ****************************************************************************************
 *
 * @file user_app.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (C) 2012-2022 Renesas Electronics Corporation and/or its affiliates
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
 * OR OTHER DEALINGS IN THE SOFTWARE.E.
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
#include "user_app.h"
#include "ke_msg.h"
#include "arch_console.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

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
	
		update_custom_service_ccc(GATT_CCC_NONE);
	
		arch_printf("Connection established\r\n");
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
	
		arch_printf("Disconnected\r\n");
}

/**
 ****************************************************************************************
 * @brief Called after stack configuration
 * @return void
 ****************************************************************************************
 */
void user_app_on_set_dev_config_complete(void)
{
		arch_printf("Dynamic Custom Service Example\r\n\r\n");
	
		default_app_on_set_dev_config_complete();
		
		create_custom_service();		
}

void user_catch_rest_hndl(ke_msg_id_t const msgid,
													void const *param,
													ke_task_id_t const dest_id,
													ke_task_id_t const src_id)
{
		switch(msgid)
		{				
				// Response to add service in database request
        case GATTM_ADD_SVC_RSP:
            handle_gattm_add_svc_rsp(msgid, (struct gattm_add_svc_rsp *)param, dest_id, src_id);
            break;
				
				 // Central has read attribute with Read Indication
        case GATTC_READ_REQ_IND:
            handle_gattc_read_req_ind(msgid, (struct gattc_read_req_ind *)param, dest_id, src_id);
            break;

        // Central has written attribute 
        case GATTC_WRITE_REQ_IND:
						handle_gattc_write_req_ind(msgid, (struct gattc_write_req_ind *)param, dest_id, src_id);
            break;
				
				// Response to setting attribute
				case GATTM_ATT_SET_VALUE_RSP:
						handle_gattm_att_set_value_rsp(msgid, (struct gattm_att_set_value_rsp *)param, dest_id, src_id);
						break;
				
				default:
						break;
		}
}
 

/// @} APP
