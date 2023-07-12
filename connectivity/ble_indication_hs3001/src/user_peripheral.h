/**
 ****************************************************************************************
 *
 * @file user_peripheral_proj.h
 *
 * @brief Peripheral project header file.
 *
 * Copyright (c) 2015-2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#ifndef _USER_PERIPHERAL_H_
#define _USER_PERIPHERAL_H_

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
#include "arch_wdg.h"

#include "app_callback.h"
#include "app_default_handlers.h"


/*
 * DEFINES
 ****************************************************************************************
 */
#define DECIMAL "%d"
#define FLOAT   "%.4f" 
 
#if defined (CFG_USE_INTERNAL_TEMP_SENSOR) && (__DA14531__)  
#define TEMPERATURE_DATA    (4)
#define HUMIDITY_DATA    (9)
#define SNPRINT_FORMAT      DECIMAL
#else
#define TEMPERATURE_DATA    (9)
#define HUMIDITY_DATA       (9)
#define SNPRINT_FORMAT      FLOAT
#endif

/* Duration of timer for connection parameter update request */
#define APP_PARAM_UPDATE_REQUEST_TO         (1000)  	 	// 1000*10ms = 10sec, The maximum allowed value is 41943sec (4194300 * 10ms)

/* Advertising data update timer */
#define APP_ADV_DATA_UPDATE_TO              (3000)   		// 3000*10ms = 30sec, The maximum allowed value is 41943sec (4194300 * 10ms)

/* Manufacturer specific data constants */
#define APP_AD_MSD_COMPANY_ID              (0xABCD)
#define APP_AD_MSD_COMPANY_ID_LEN          (2)
#define APP_AD_MSD_DATA_LEN_temp         TEMPERATURE_DATA
#define APP_AD_MSD_DATA_LEN_hum          HUMIDITY_DATA

#define APP_PERIPHERAL_CTRL_TIMER_DELAY    100
#define INDICATION_DELAY 					         300  								//Time between notifications in ms
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

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
 * @param[in] connection_idx Connection Id index
 * @param[in] param Pointer to GAPC_CONNECTION_REQ_IND message
 * @return void
 ****************************************************************************************
*/
void user_app_connection(uint8_t connection_idx, 
                         struct gapc_connection_req_ind const *param);

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
 * @brief Disconnection function.
 * @param[in] param Pointer to GAPC_DISCONNECT_IND message
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

/// @} APP

#endif // _USER_PERIPHERAL_H_
