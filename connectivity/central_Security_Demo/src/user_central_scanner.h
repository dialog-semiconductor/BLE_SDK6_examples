/**
 ****************************************************************************************
 *
 * @file user_central_scanner.h
 *
 * @brief Security central project header file.
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

#ifndef _USER_CENTRAL_SCANNER_H_
#define _USER_CENTRAL_SCANNER_H_

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

struct adv_device
{

    uint8_t        index;
    ///index of multiple advertising devices
    struct bd_addr adv_addr;
	
		//uint8_t evt_type;
    ///Data length in advertising packet
		///Data of advertising packet
    uint8_t       device_name_len;
    uint8_t       device_name[20];
    ///RSSI value for advertising packet
    uint8_t       evt_type;
		uint8_t				addr_type;       
};

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param);

void user_on_disconnect( struct gapc_disconnect_ind const *param );

void user_scan_start(void);

void user_scan_complete_cb(uint8_t status);

void user_scan_report_ind_cb(struct gapm_adv_report_ind const *param);

void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id);

void user_choose_device(void);
/// @} APP

#endif // _USER_CENTRAL_SCANNER_H_