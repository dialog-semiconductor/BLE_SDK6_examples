/**
 ****************************************************************************************
 *
 * @file user_social_distancing.h
 *
 * @brief Social Distancing application header file.
 *
 * Copyright (c) 2015-2021 Dialog Semiconductor
# The MIT License (MIT)
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
# OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */

#ifndef _USER_SOCIAL_DISTANCING_H_
#define _USER_SOCIAL_DISTANCING_H_

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
#define USER_SCANNING_TIMEOUT               (1)     // Setting this to 0 defaults to the scanning timeout 10 seconds
#define ADVERTISING_TIMEOUT_msec            (5000)  // Advertising timeout
#define SCANNING_PERCENTAGE                 (60)    // Scanning timeout based on advertising timeout
#define CONNECT_INIT_PERCENTAGE             (30)    // Connection attempt timeout based on advertising timeout

#define USER_SWITCH_ADV_SCAN_TO             (ADVERTISING_TIMEOUT_msec/10)

#if USER_SCANNING_TIMEOUT
#define USER_SCANNING_TO                    ((ADVERTISING_TIMEOUT_msec*SCANNING_PERCENTAGE)/1000)
#endif

#define USER_INITIATE_CONN_TO               ((ADVERTISING_TIMEOUT_msec*CONNECT_INIT_PERCENTAGE)/1000)

#define USER_UPD_CONN_RSSI_TO               (20)    // 20*10ms = 200msec


#ifdef CFG_PRINTF
#define BOLD                                "1"
#define PLAIN                               "0"

#define RED(x)                           "\033["x";31m"
#define GREEN(x)                         "\033["x";32m"
#define YELLOW(x)                        "\033["x";33m"
#define BLUE(x)                          "\033["x";34m"
#define MAGENTA(x)                       "\033["x";35m"
#define CYAN(x)                          "\033["x";36m"
#define RESET_COLOUR                     "\033[0m"
#endif

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
enum user_prox_zones
{
    USER_PROX_ZONE_DANGER,
    USER_PROX_ZONE_WARNING,
    USER_PROX_ZONE_COARSE,
    USER_PROX_ZONE_BLE_LIMIT,
    USER_PROX_ZONE_COUNT
};


struct user_adv_rssi_node
{
    ///Advertising address type: public/random
    uint8_t                     adv_addr_type;
    ///Advertising address value
    struct bd_addr              adv_addr;
    ///Mean RSSI value for advertising packets
    uint8_t                     mean_rssi;
    ///Number of times the advertising report has been received
    uint8_t                     count;
    ///Accessed flag
    bool                        accessed;
    ///Next linked node
    struct user_adv_rssi_node*  next;
};

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
 * @brief Function called when scanning completes.
 * @return void
 ****************************************************************************************
*/
void user_app_on_scanning_completed(const uint8_t);

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

 
/**
 ****************************************************************************************
 * @brief Advertising report callback.
 * @param[in] param GAP advertising report
 * @return void
 ****************************************************************************************
*/                          
void user_app_on_adv_report_ind(struct gapm_adv_report_ind const * param);
      
/**
 ****************************************************************************************
 * @brief Connection attempt callback.
 * @param[in] void
 * @return void
 ****************************************************************************************
*/                                
void user_app_on_connect_failed(void);
    
/**
 ****************************************************************************************
 * @brief Start a connection attempt to the next available node.
 * @param[in] void
 * @return true if node found and a connection attempt starts false otherwise 
 ****************************************************************************************
*/     
bool initiate_connection_attempt(void);

/**
 ****************************************************************************************
 * @brief User function for generating and storing a static random bd address
 * @param[out] pointer that will hold the generated bd address 
 ****************************************************************************************
*/   
void user_on_app_generate_unique_static_random_addr(struct bd_addr *addr);
/// @} APP

#endif //_USER_SOCIAL_DISTANCING_H_
