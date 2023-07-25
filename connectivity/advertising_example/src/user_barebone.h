/**
 ****************************************************************************************
 *
 * @file user_barebone.h
 *
 * @brief Barebone application header file.
 *
 * Copyright (c) 2015-2021 Dialog Semiconductor
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

#ifndef _USER_BAREBONE_H_
#define _USER_BAREBONE_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup
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
 
#include "gapc_task.h"                 // gap functions and messages
#include "app_task.h"                  // application task
#include "app.h"                       // application definitions
#include "app_callback.h"
#include "arch_api.h"
/*
 * DEFINES
 ****************************************************************************************
 */

// Configurable advertising intervals
#define UNDIRECT_ADV_INTERVAL	            150		//(150 	* 1msec 	=  150msec)
#define NONCONN_ADV_INTERVAL	            1050	//(1050 * 1msec		= 1050msec)

// Configurable state length (for timer based example)
#define	UNDIRECT_ADV_DURATION	            200		//(200 	* 10msec 	= 	2sec)
#define	NONCONN_ADV_DURATION	            300		//(300 	* 10msec 	= 	3sec)
#define SLEEP_DURATION		                250		//(250 	* 10msec 	= 	2.5sec) 

/* Duration of timer for connection parameter update request */
#define APP_PARAM_UPDATE_REQUEST_TO         (1000)   // 1000*10ms = 10sec, The maximum allowed value is 41943sec (4194300 * 10ms)

/* Advertising data update timer */
#define APP_ADV_DATA_UPDATE_TO              (500)   // 500*10ms = 5sec, The maximum allowed value is 41943sec (4194300 * 10ms)

/* Manufacturer specific data constants */
#define APP_AD_MSD_COMPANY_ID               (0xABCD)
#define APP_AD_MSD_COMPANY_ID_LEN           (2)
#define APP_AD_MSD_DATA_LEN                 (sizeof(uint16_t))

typedef enum 
{
    UNDIRECT_ADVERTISING,
    NONCON_ADVERTISING,
    PERMANENT_SLEEP 						
}advertising_state;
						 
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Application initialization function.
 * @return void
 ****************************************************************************************
*/
void user_app_init(void);

/**
 ****************************************************************************************
 * @brief Advertising function.
 * @return void
 ****************************************************************************************
*/
void user_app_adv_start(void);

/**
 ****************************************************************************************
 * @brief Connection function.
 * @param[in] conidx        Connection Id index
 * @param[in] param         Pointer to GAPC_CONNECTION_REQ_IND message
 * @return void
 ****************************************************************************************
*/
void user_app_connection(const uint8_t conidx, struct gapc_connection_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Undirect advertising completion function.
 * @param[in] status Command complete event message status
 * @return void
 ****************************************************************************************
*/
void user_app_adv_undirect_complete(uint8_t status);

/**
 ****************************************************************************************
 * @brief nonconn advertising completion function.
 * @param[in] status Command complete event message status
 * @return void
 ****************************************************************************************
*/
void user_app_adv_nonconn_complete(uint8_t status);
/**
 ****************************************************************************************
 * @brief Disconnection function.
 * @param[in] param         Pointer to GAPC_DISCONNECT_IND message
 * @return void
 ****************************************************************************************
*/
void user_app_disconnect(struct gapc_disconnect_ind const *param);

/**
 ****************************************************************************************
 * @brief Handles the messages that are not handled by the SDK internal mechanisms.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id);
/**
 ****************************************************************************************
 * @brief Function to start non-connectable advertising
 * @return void
 ****************************************************************************************
*/
void user_noncon_adv_start(void);

/**
 ****************************************************************************************
 * @brief Function to start undirected advertising
 * @return void
 ****************************************************************************************
*/
void user_undirect_adv_start(void);


/**
 ****************************************************************************************
 * @brief Set the device in permanent sleep mode.
 * @return void
 ****************************************************************************************
*/                          
void user_enable_permanent_extended_sleep(void);                          

/**
 ****************************************************************************************
 * @brief Change the advertising type according to the state variable
 * @param[in] state of the advertising to change to
 * @return void
 ****************************************************************************************
*/
void user_change_adv_state(advertising_state state);

/**
 ****************************************************************************************
 * @brief Depending on the state parameter the device will initiate the correposponding
 *          action, either connectable or non-connectable advertising or sleep.
 * @param[in] state of the advertising that should be initiated
 * @return void
 ****************************************************************************************
*/
void user_start_advertising(void);

/// @} APP

#endif //_USER_BAREBONE_H_
