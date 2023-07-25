/**
 ****************************************************************************************
 *
 * @file user_coex_demo.h
 *
 * @brief Coexistence demo project header file.
 *
 * Copyright (C) 2012-2021 Dialog Semiconductor
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

#ifndef _USER_COEX_DEMO_H_
#define _USER_COEX_DEMO_H_

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
 * @brief Connection function.
 * @param[in] conidx        Connection Id index
 * @param[in] param         Pointer to GAPC_CONNECTION_REQ_IND message
 ****************************************************************************************
*/
void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Disconnection function.
 * @param[in] param         Pointer to GAPC_DISCONNECT_IND message
 ****************************************************************************************
*/
void user_on_disconnect( struct gapc_disconnect_ind const *param );

/**
 ****************************************************************************************
 * @brief Application initialization function.
 ****************************************************************************************
*/
void user_app_on_init(void);

/**
 ****************************************************************************************
 * @brief User validate going to sleep hook function.
 ****************************************************************************************
*/
sleep_mode_t user_app_on_validate_sleep(sleep_mode_t sleep_mode);
/// @} APP

#endif // _USER_COEX_DEMO_H_
