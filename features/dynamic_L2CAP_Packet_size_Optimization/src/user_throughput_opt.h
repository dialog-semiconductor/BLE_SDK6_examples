/**
 ****************************************************************************************
 *
 * @file user_throughput_opt.h
 *
 * @brief User Throughput Optimization project header file.
 *
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
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

#ifndef _USER_THROUGHPUT_OPT_H_
#define _USER_THROUGHPUT_OPT_H_

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
 * DEFINES
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles connection event
 * @param[in]   connection_idx Connection index
 * @param[in]   param Parameters of connection
 * @return void
 ****************************************************************************************
 */
void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Handles disconnection event
 * @param[in]   param Parameters of disconnect message
 * @return void
 ****************************************************************************************
 */
void user_on_disconnect( struct gapc_disconnect_ind const *param );

/**
 ****************************************************************************************
 * @brief Handles the messages that are not handled by the SDK internal mechanisms.
 * @param[in] msgid      Id of the message received.
 * @param[in] param      Pointer to the parameters of the message.
 * @param[in] dest_id    ID of the receiving task instance.
 * @param[in] src_id     ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_process_catch_rest(ke_msg_id_t const msgid, void const *param,
                             ke_task_id_t const dest_id, ke_task_id_t const src_id);


/**
 ****************************************************************************************
 * @brief Called after stack initialization
 ****************************************************************************************
 */
void user_app_on_set_dev_config_complete(void);

/**
 ****************************************************************************************
 * @brief Called on the reception of GAPC_LE_PKT_SIZE_IND
 * @return void
 ****************************************************************************************
 */
void user_on_data_length_change(uint8_t connection_idx, struct gapc_le_pkt_size_ind *pkt);

/**
 ****************************************************************************************
 * @brief Callback of the GAPC_UPDATE_PARAMS rejection (parameter update is rejected)
 * @param[in]   status status of the message
 * @return void
 ****************************************************************************************
 */
void user_on_update_params_rejected(const uint8_t  status);

/**
 ****************************************************************************************
 * @brief Callback of the GAPC_UPDATE_PARAMS (parameter update is complete)
 * @return void
 ****************************************************************************************
 */
void user_on_update_params_complete(void);

/**
 ****************************************************************************************
 * @brief Handles GAPC peer features indication
 * @param[in]   connection_idx Connection index
 * @param[in]   gapc_peer_features_ind pointer to features message indication parameters
 * @return void
 ****************************************************************************************
 */
void user_app_on_get_peer_features(const uint8_t conn_id, struct gapc_peer_features_ind const * features);

/// @} APP

#endif // _USER_THROUGHPUT_OPT_H_