/**
 ****************************************************************************************
 *
 * @file user_central_app_template.h
 *
 * @brief Central Application project header file.
 *
 * Copyright (c) 2012-2021 Dialog Semiconductor
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

#ifndef _USER_CENTRAL_APP_H_
#define _USER_CENTRAL_APP_H_

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


/**
 ****************************************************************************************
 * @brief Callback from SDK when  connection occurs.
 * @param[in] connection_idx  connection identifier
 * @param[in] param  peer connection info
 * @return void
 ****************************************************************************************
 */
void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Callback from SDK when disconnect occurs.
 * @param[in] param  peer connection info
 * @return void
 ****************************************************************************************
 */
void user_on_disconnect( struct gapc_disconnect_ind const *param );

/**
 ****************************************************************************************
 * @brief Callback from SDK once stack is configured
 * @return void
 ****************************************************************************************
 */
void user_app_on_set_dev_config_complete(void);

/**
 ****************************************************************************************
 * @brief Callback from SDK when disconnect occurs.
 * @param[in] report  advertising report information
 * @return void
 ****************************************************************************************
 */
void user_on_adv_report_ind(struct gapm_adv_report_ind const * report);

/**
 ****************************************************************************************
 * @brief Callback from SDK when disconnect occurs.
 * @param[in] reason reason for disconnect BT SIG reason defined in co_error.h
 * @return void
 ****************************************************************************************
 */
void user_on_scanning_completed(uint8_t reason);

/**
 ****************************************************************************************
 * @brief Callback from SDK when an unhandled stack event occurs
 * @param[in] msgid ID of message from stack
 * @param[in] param - message data
 * @param[in] dest_id - destination task of message, e.g. TASK_APP
 * @param[in] src_id - task originator of message
 * @return void
 ****************************************************************************************
 */
void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id);


/// @} APP

#endif // _USER_CENTRAL_APP_H_
