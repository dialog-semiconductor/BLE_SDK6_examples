/**
 ****************************************************************************************
 *
 * @file user_dyn_custom_svc.h
 *
 * @brief Dynamic custom service project header file.
 *
 * Copyright (C) 2012-2022 Dialog Semiconductor
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

#ifndef _USER_DYN_CUSTOM_SVC_H_
#define _USER_DYN_CUSTOM_SVC_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief 
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwble_config.h"
#include "app_task.h"                  // application task
#include "gapc_task.h"                 // gap functions and messages
#include "gapm_task.h"                 // gap functions and messages
#include "app.h"                       // application definitions
#include "co_error.h"                  // error code definitions
#include "gattc_task.h"
#include "gattm_task.h"

/****************************************************************************
Add here supported profiles' application header files.
i.e.
#if (BLE_DIS_SERVER)
#include "app_dis.h"
#include "app_dis_task.h"
#endif
*****************************************************************************/

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
 typedef enum {
		GATT_SERVICE_SECONDARY,
		GATT_SERVICE_PRIMARY,
} gatt_service_t;

typedef enum {
		GATT_CCC_NONE           = 0x0000,
		GATT_CCC_NOTIFICATIONS  = 0x0001,
		GATT_CCC_INDICATIONS    = 0x0002,
} gatt_ccc_t;

typedef enum {
		ATT_UUID_16,
		ATT_UUID_128
} att_uuid_type_t;

typedef enum {
		SVC_PERM_ATT_UUID_LEN_16 = 0,
		SVC_PERM_ATT_UUID_LEN_32 = 1,
		SVC_PERM_ATT_UUID_LEN_128 = 2,
		SVC_PERM_ATT_UUID_LEN_RFU = 3,
} svc_perm_att_uuid_len_t;

typedef struct {
		att_uuid_type_t type;
		union uuid{
				uint16_t uuid16;
				uint8_t  uuid128[ATT_UUID_128_LEN];
		}uuid;
} att_uuid_t;

typedef struct
{
		att_uuid_t uuid_info;
		gatt_service_t svc_type;
		uint8_t svc_perm;
		uint16_t num_attrs;
} gattm_add_svc_req_cmd_t;

typedef enum
{
		ATT_PERM_ENABLE_READ = (1 << PERM_POS_RD),
		ATT_PERM_UNAUTH_READ = (2 << PERM_POS_RD),
		ATT_PERM_AUTH_READ = (3 << PERM_POS_RD),
		
		ATT_PERM_ENABLE_WRITE = (1 << PERM_POS_WR),
		ATT_PERM_UNAUTH_WRITE = (2 << PERM_POS_WR),
		ATT_PERM_AUTH_WRITE = (3 << PERM_POS_WR),
		
		ATT_PERM_ENABLE_INDICATION = (1 << PERM_POS_IND),  
		ATT_PERM_UNAUTH_INDICATION = (2 << PERM_POS_IND),
		ATT_PERM_AUTH_INDICATION = (3 << PERM_POS_IND),
	
		ATT_PERM_ENABLE_NOTIFICATION = (1 << PERM_POS_NTF),  
		ATT_PERM_UNAUTH_NOTIFICATION = (2 << PERM_POS_NTF),
		ATT_PERM_AUTH_NOTIFICATION = (3 << PERM_POS_NTF),
	
		ATT_PERM_EXTENDED_PROPERTIES = (1 << PERM_POS_EXT),
		ATT_PERM_BROADCAST = (1 << PERM_POS_BROADCAST),
		ATT_PERM_ENC_KEY_16_BYTES = (1 << PERM_POS_EKS),
		ATT_PERM_WRITE_COMMAND = (1 << PERM_POS_WRITE_COMMAND),
		ATT_PERM_WRITE_SIGNED = (1 << PERM_POS_WRITE_SIGNED),
		ATT_PERM_WRITE_REQUEST = (1 << PERM_POS_WRITE_REQ),
		ATT_PERM_UUID_LEN_16_BITS = (0x0 << PERM_POS_UUID_LEN),
		ATT_PERM_UUID_LEN_128_BITS = (0x2 << PERM_POS_UUID_LEN),
} att_perm_t;

typedef enum
{
		READ_INDICATION_DISABLED,
		READ_INDICATION_ENABLED = (1 << PERM_POS_RI),
		READ_INDICATION_MAX =  0xFFFF,
} read_indication_t;

typedef struct 
{
		uint16_t  attr_idx;
		att_uuid_t	uuid_info;
		att_perm_t	perm;
		uint16_t 	max_len;
		read_indication_t read_indication;
} att_desc_t;

// Custom service attribute handle enum
typedef enum
{
    CUSTOM_ATTR_HANDLE_OFFSET_SERVICE1,

		CUSTOM_ATTR_HANDLE_OFFSET_DECLARATION_CHAR1,
    CUSTOM_ATTR_HANDLE_OFFSET_VALUE_CHAR1,
    CUSTOM_ATTR_HANDLE_OFFSET_USER_DESC_CHAR1,
		CHAR1_NUM_ATTR = CUSTOM_ATTR_HANDLE_OFFSET_USER_DESC_CHAR1,
	
    CUSTOM_ATTR_HANDLE_OFFSET_DECLARATION_CHAR2,
    CUSTOM_ATTR_HANDLE_OFFSET_VALUE_CHAR2,
    CUSTOM_ATTR_HANDLE_OFFSET_USER_DESC_CHAR2,
    CUSTOM_ATTR_HANDLE_OFFSET_CCC_CHAR2,
		CHAR2_NUM_ATTR = (CUSTOM_ATTR_HANDLE_OFFSET_CCC_CHAR2 - CHAR1_NUM_ATTR),
	
		CUSTOM_SVC_NUM_ATTR = CUSTOM_ATTR_HANDLE_OFFSET_CCC_CHAR2,
} custom_attr_handle_offset_t;

/*
 * DEFINES
 ****************************************************************************************
 */

#define CUSTOM_SERVICE_1_UUID_128        {0x2F, 0x2A, 0x93, 0xA6, 0xBD, 0xD8, 0x41, 0x52, 0xAC, 0x0B, 0x10, 0x99, 0x2E, 0xC6, 0xFE, 0xED}

#define CUSTOM_SERVICE_CHAR_1_UUID_128   {0x20, 0xEE, 0x8D, 0x0C, 0xE1, 0xF0, 0x4A, 0x0C, 0xB3, 0x25, 0xDC, 0x53, 0x6A, 0x68, 0x86, 0x2D}
#define CUSTOM_SERVICE_CHAR_1_USER_DESC  "Custom Char 1"
#define CUSTOM_SERVICE_CHAR_1_LEN    		 4

#define CUSTOM_SERVICE_CHAR_2_UUID_128   {0x4F, 0x43, 0x31, 0x3C, 0x93, 0x92, 0x42, 0xE6, 0xA8, 0x76, 0xFA, 0x3B, 0xEF, 0xB4, 0x87, 0x5A}
#define CUSTOM_SERVICE_CHAR_2_USER_DESC  "Custom Char 2"
#define CUSTOM_SERVICE_CHAR_2_LEN    		 4

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void create_custom_service(void);

void handle_gapc_connection_req_ind(ke_msg_id_t msgid,
                                    struct gapc_connection_req_ind *param,
                                    ke_task_id_t dest_id,
                                    ke_task_id_t src_id);
																		
void handle_gattc_read_req_ind(ke_msg_id_t msgid,
															 struct gattc_read_req_ind const *param,
															 ke_task_id_t dest_id,
															 ke_task_id_t src_id);

void handle_gattc_write_req_ind(ke_msg_id_t msgid,
																struct gattc_write_req_ind const *param,
																ke_task_id_t dest_id,
																ke_task_id_t src_id);
																
void handle_gattm_add_svc_rsp(ke_msg_id_t msgid,
                              struct gattm_add_svc_rsp *param,
                              ke_task_id_t dest_id,
                              ke_task_id_t src_id);
															
void handle_gattm_att_set_value_rsp(ke_msg_id_t msgid,
                                    struct gattm_att_set_value_rsp *param,
                                    ke_task_id_t dest_id,
                                    ke_task_id_t src_id);
																
void update_custom_service_ccc(uint16_t new_val);

/// @} APP

#endif // _USER_DYN_CUSTOM_SVC_H_
