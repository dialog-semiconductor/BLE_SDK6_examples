/**
 ****************************************************************************************
 *
 * @file user_crash_info.h
 *
 * @brief user_crash_info project header file.
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

#ifndef _USER_CRASH_INFO_H_
#define _USER_CRASH_INFO_H_

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

void user_app_on_set_dev_config_complete(void);

/******************************************************************************************
 * @brief Write request validation handler for services registered with CUSTS1 TASK. 
					This function is an opportunuty to reject the request before it is written to the 
					BLE database. If the write is accepted, the request will result in a 
					CUSTS1_VAL_WRITE_IND. 
					
 * @param[in] att_idx   handle of the attribute written
 * @param[in] offset    offset at which data has to be written
 * @param[in] length 		length of data to write
 * @param[in] value  	pointer to data to write
 * @return error code indicating write is accepted or reason for rejection (see enum hl_err in rwble_hl_error.h)
 *****************************************************************************************
 */
uint8_t user_cust_svc_validate_write_handler(uint16_t att_idx, uint16_t offset, uint16_t length, uint8_t* value);

/**
 ****************************************************************************************
 * @brief Handles the messages that are not handled by the SDK internal mechanisms.
 * @param[in] msgid      Id of the message received.
 * @param[in] param      Pointer to the parameters of the message.
 * @param[in] dest_id    ID of the receiving task instance.
 * @param[in] src_id     ID of the sending task instance.
 * @return none
 ****************************************************************************************
 */
void user_message_handler(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Start advertising
 * @return none
 ****************************************************************************************
*/													
void user_advertise_operation(void);
													
/// @} APP

#endif // _USER_CRASH_INFO_H_
