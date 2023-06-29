 /**
 ****************************************************************************************
 *
 * @file user_ble_gap.h
 *
 * @brief User BLE GAP header file.
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

#ifndef _USER_BLE_COMMON_H_
#define _USER_BLE_COMMON_H_

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
 
#include <stdint.h>
#include <stdbool.h>
#include "user_central_config.h"
#include "compiler.h"
 
 #define ATT_UUID_LENGTH (16)
 
 /** GATT characteristic properties */
typedef enum {
        GATT_PROP_NONE                          = 0,
        GATT_PROP_BROADCAST                     = 0x0001,
        GATT_PROP_READ                          = 0x0002,
        GATT_PROP_WRITE_NO_RESP                 = 0x0004,
        GATT_PROP_WRITE                         = 0x0008,
        GATT_PROP_NOTIFY                        = 0x0010,
        GATT_PROP_INDICATE                      = 0x0020,
        GATT_PROP_WRITE_SIGNED                  = 0x0040,
        GATT_PROP_EXTENDED                      = 0x0080,
        GATT_PROP_EXTENDED_RELIABLE_WRITE       = 0x0100,
        GATT_PROP_EXTENDED_WRITABLE_AUXILIARIES = 0x0200,
} gatt_prop_t;
 
 typedef enum {
        ATT_UUID_16,
        ATT_UUID_128
} att_uuid_type_t;
 
typedef enum{
	GATT_CCC_NONE           = 0x0000,
	GATT_CCC_NOTIFICATIONS  = 0x0001,
	GATT_CCC_INDICATIONS    = 0x0002,
}gatt_ccc_t;

union uuid{
				uint16_t uuid16;
				uint8_t  uuid128[ATT_UUID_LENGTH];
};
typedef struct {
        att_uuid_type_t type;
				union uuid			uuid;
        
} att_uuid_t;

typedef struct {
        att_uuid_t 	uuid;                ///< item UUID
        uint16_t 		handle;                ///< item handle 	
				/** characteristic data (if type == ::GATTC_ITEM_TYPE_CHARACTERISTIC) */
				struct {
								uint16_t value_handle;  ///< characteristic value handle
								uint8_t properties;     ///< characteristic properties
				} c;

                
} gattc_chars_t;

typedef struct{
	uint16_t 			last_handle;
#ifdef ENABLE_BAS
	gattc_chars_t	bas_char;
	bool 		 			bas_handle_valid;
#endif
#ifdef ENABLE_IAS
	gattc_chars_t ias_char;
	bool 		 			ias_handle_valid;
	uint8_t 			ias_write_op;
	uint8_t	 			ias_alert_counter;
#endif
	 
}service_disc_t;
 
typedef struct{
	 
	uint8_t 					con_idx;
	bool 	 						con_valid;
	struct 						bd_addr addr;
	service_disc_t		serv_disc;
	 
}periph_device_t;

typedef struct{
	
	 att_uuid_t			svc_uuid;
	 uint16_t				num_chars;
	 gattc_chars_t 	items[__ARRAY_EMPTY];
	
}service_info_t;
 
typedef enum{
	
	 BLE_GAP_ERROR_NO_ERROR,
	 BLE_GAP_ERROR_INVALID_PARAMS,
	
}ble_gap_error_t;
 
typedef struct
{
	///Payload length, BLE length byte will get calculated in helper function
	uint8_t 	len;
	///GAP Flag
	uint8_t 	type;
	///Payload data
	uint8_t 	*data;
	
}ble_gap_adv_struct_t;


#endif //_USER_BLE_COMMON_H_


/// @} APP

