/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.h
 *
 * @brief Peripheral project Custom1 Server implementation header file.
 *
 * Copyright (C) 2015-2023 Renesas Electronics Corporation and/or its affiliates
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

#ifndef _USER_CUSTS1_IMPL_H_
#define _USER_CUSTS1_IMPL_H_


/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gapc_task.h"                 // gap functions and messages
#include "gapm_task.h"                 // gap functions and messages
#include "custs1_task.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

void user_send_temperature_humidity_ind(void);									

void user_message_handler(struct custs1_val_write_ind const *param);		


/**
 ****************************************************************************************
 * @brief Remaining attribute info request handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 ****************************************************************************************
*/
void user_svc1_rest_att_info_req_handler(ke_msg_id_t const msgid,
                                         struct custs1_att_info_req const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id);



																	 

/**
 ****************************************************************************************
 * @brief Remaining attribute info request handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
																				 
																				 
void user_svc1_rest_att_info_req_handler (ke_msg_id_t const msgid,
                                         struct custs1_att_info_req const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id);

/// @} APP

#endif // _USER_CUSTS1_IMPL_H_
