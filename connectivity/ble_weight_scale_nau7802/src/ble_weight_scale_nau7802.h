/**
 ****************************************************************************************
 *
 * @file user_empty_peripheral_template.h
 *
 * @brief Empty peripheral template project header file.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation and/or its affiliates
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
 ***************************************************************************************
 */

#ifndef _USER_EMPTY_PERIPHERAL_TEMPLATE_H_
#define _USER_EMPTY_PERIPHERAL_TEMPLATE_H_

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
 
 #define WEIGHT_SCALE_UPDATE_INTERVAL 100

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
 
 /**
 ****************************************************************************************
 * @brief gets weight value from nau7802 and add new callback for next update
 ****************************************************************************************
*/
void update_weight(void);

/**
 ****************************************************************************************
 * @brief Converts a byte array containing wieght data in grams to 10s of milligrams.
 * @param[in] data   Byte array of grams. Least significat byte first
 * @return uint32_r  Weight value in milligrams
 ****************************************************************************************
*/
uint32_t grams_byte_array_to_10milligrams(const uint8_t data[4]);

 /**
 ****************************************************************************************
 * @brief starts updating the nau7802 and calls default connection
 * @param[in] connection_idx  Index number that identifies a connected device
 * @param[in] param   				Pointer to the parameters of the message.
 ****************************************************************************************
*/
void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param);

 /**
 ****************************************************************************************
 * @brief stops the weight scale updating and calls default disconnect
 ****************************************************************************************
*/
void user_on_disconnect( struct gapc_disconnect_ind const *param );

 /**
 ****************************************************************************************
 * @brief initializes wsss app and calls default initializer
 ****************************************************************************************
*/
void user_app_on_init(void);

/**
 ****************************************************************************************
 * @brief Handles the messages that are not handled by the SDK internal mechanisms.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 ****************************************************************************************
*/
void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id);

/// @} APP

#endif // _USER_EMPTY_PERIPHERAL_TEMPLATE_H_