/**
 ****************************************************************************************
 *
 * @file app_default_handlers.h
 *
 * @brief Application default handlers header file.
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef _APP_DEFAULT_HANDLERS_H_
#define _APP_DEFAULT_HANDLERS_H_

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
#include "rwip_config.h"

#if (BLE_APP_PRESENT)

#include <stdio.h>
#include "gapc_task.h"          // GAP Controller Task API
#include "gapm_task.h"          // GAP Manager Task API
#include "co_bt.h"
#include "app_utils.h"
#include "app.h"
#if (BLE_APP_SEC)
#include "app_security.h"
#endif // (BLE_APP_SEC)

/**
 ****************************************************************************************
 * @brief Possible advertise scenarios.
 ****************************************************************************************
 */
enum default_advertise_scenario{
    DEF_ADV_FOREVER,
    DEF_ADV_WITH_TIMEOUT};

/**
 ****************************************************************************************
 * @brief Possible security request scenarios.
 ****************************************************************************************
 */
enum default_security_request_scenario{
    DEF_SEC_REQ_NEVER,
    DEF_SEC_REQ_ON_CONNECT};

/**
 ****************************************************************************************
 * @brief Configuration options for the default_handlers.
 ****************************************************************************************
 */
struct default_handlers_configuration
{
    //Configure the advertise operation used by the default handlers
    enum default_advertise_scenario adv_scenario;

    //Configure the advertise period in case of DEF_ADV_WITH_TIMEOUT.
    //It is measured in timer units (10ms). Use MS_TO_TIMERUNITS macro to convert
    //from milliseconds (ms) to timer units.
    int32_t advertise_period;

    //Configure the security start operation of the default handlers
    //if the security is enabled (CFG_APP_SECURITY)
    enum default_security_request_scenario security_request_scenario;
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Default function called on initialization event.
 * @return void
 ****************************************************************************************
 */
void default_app_on_init(void);

/**
 ****************************************************************************************
 * @brief Default function called on connection event.
 * @param[in] conidx Connection Id number
 * @param[in] param          Pointer to GAPC_CONNECTION_REQ_IND message
 * @return void
 ****************************************************************************************
 */
void default_app_on_connection(uint8_t conidx, struct gapc_connection_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on disconnection event.
 * @param[in] param          Pointer to GAPC_DISCONNECT_IND message
 * @return void
 ****************************************************************************************
 */
void default_app_on_disconnect(struct gapc_disconnect_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on device configuration completion event.
 * @return void
 ****************************************************************************************
 */
void default_app_on_set_dev_config_complete(void);

/**
 ****************************************************************************************
 * @brief Default function called on database initialization completion event.
 * @return void
 ****************************************************************************************
 */
void default_app_on_db_init_complete(void);

/**
 ****************************************************************************************
 * @brief Default function called on advertising operation.
 * @return void
 ****************************************************************************************
 */
void default_advertise_operation(void);

/**
 ****************************************************************************************
 * @brief Structure containing the operations used by the default handlers.
 * @return void
 ****************************************************************************************
 */
struct default_app_operations {
    void (*default_operation_adv)(void);
};

/**
 ****************************************************************************************
 * @brief Default function called on device name read request from peer.
 * @param[in,out] device_name       The device_name value returned.
 * @return void
 ****************************************************************************************
 */
void default_app_on_get_dev_name(struct app_device_name* device_name);

/**
 ****************************************************************************************
 * @brief Default function called on device appearance read request from peer.
 * @param[in,out] appearance         The appearance value returned.
 * @return void
 ****************************************************************************************
 */
void default_app_on_get_dev_appearance(uint16_t* appearance);

/**
 ****************************************************************************************
 * @brief Default function called on slave preferred connection parameters read request
 *        from peer.
 * @param[in|out] slv_params         The slave preferred connection parameters.
 * @return void
 ****************************************************************************************
 */
void default_app_on_get_dev_slv_pref_params(struct gap_slv_pref* slv_params);

/**
 ****************************************************************************************
 * @brief Default function called on device info write request from peer.
 * @param[in] param          Pointer to GAPC_SET_DEV_INFO_REQ_IND message
 * @param[in,out] status     Status code used to know if requested has been accepted
 *                           or not.
 * @return void
 ****************************************************************************************
 */
void default_app_on_set_dev_info(struct gapc_set_dev_info_req_ind const * param, uint8_t* status);

/**
 ****************************************************************************************
 * @brief Default function called on parameter update request indication.
 * @param[in] param             Pointer to a @gapc_param_update_req_ind message struct
 * @param[in|out] cfm           Pointer to a @gapc_param_update_cfm message struct
 * @return void
 * @note The application may accept or reject the received parameter update request,
 *       depending on the received param values.
 ****************************************************************************************
 */
void default_app_update_params_request(struct gapc_param_update_req_ind const *param,
                                       struct gapc_param_update_cfm *cfm);

/**
****************************************************************************************
* @brief Generate a 48-bit static random address. The static random address is generated
*        only once in a device power cycle and it is stored in the retention RAM.
*        The two MSB shall be equal to '1'.
* @param[in|out] addr      The generated static random address
* @return void
****************************************************************************************
*/
void default_app_generate_static_random_addr(struct bd_addr *addr);

/**
 ****************************************************************************************
 * @brief Generate a 48-bit unique static random address. The static random address is
 *        generated only once in a device power cycle and it is stored in the retention
 *        RAM. It is based on the OTP header data (device time stamp, site ID, test site,
 *        bandgap and LNA trim value) thus is guaranteed to be constant over every power
 *        cycle. The two MSB shall be equal to '1'.
 * @param[in|out] addr      The generated static random address
 * @return void
 ****************************************************************************************
 */
void default_app_generate_unique_static_random_addr(struct bd_addr *addr);

#if (BLE_APP_SEC)

/**
 ****************************************************************************************
 * @brief Default function called on pairing request event.
 * @param[in] conidx         Connection Id number
 * @param[in] param          Pointer to GAPC_BOND_REQ_IND message
 * @return void
 ****************************************************************************************
 */
void default_app_on_pairing_request(uint8_t conidx, struct gapc_bond_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on no-man-in-the-middle TK exchange event.
 * @param[in] conidx         Connection Id number
 * @param[in] param          Pointer to GAPC_BOND_REQ_IND message
 * @return void
 ****************************************************************************************
 */
void default_app_on_tk_exch(uint8_t conidx, struct gapc_bond_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on CSRK exchange event.
 * @param[in] conidx         Connection Id number
 * @param[in] param          Pointer to GAPC_BOND_REQ_IND message
 * @return void
 ****************************************************************************************
 */
void default_app_on_csrk_exch(uint8_t conidx, struct gapc_bond_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on long term key exchange event.
 * @param[in] conidx         Connection Id number
 * @param[in] param          Pointer to GAPC_BOND_REQ_IND message
 * @return void
 ****************************************************************************************
 */
void default_app_on_ltk_exch(uint8_t conidx, struct gapc_bond_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on encryption request event.
 * @param[in] conidx         Connection Id number
 * @param[in] param          Pointer to GAPC_ENCRYPT_REQ_IND message
 * @return void
 ****************************************************************************************
 */
void default_app_on_encrypt_req_ind(uint8_t conidx, struct gapc_encrypt_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on pairing succeeded.
 * @param[in] conidx         Connection Id number
 * @return void
 ****************************************************************************************
 */
void default_app_on_pairing_succeeded(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Default function called on address resolved indication.
 * @param[in] conidx         Connection Id number
 * @param[in] param          Pointer to GAPM_ADDR_SOLVED_IND message
 * @return void
 ****************************************************************************************
 */
void default_app_on_addr_solved_ind(uint8_t conidx, struct gapm_addr_solved_ind const *param);

/**
 ****************************************************************************************
 * @brief Default function called on GAPM_CMP_EVT (GAPM_RESOLV_ADDR) event with
 *        status = GAP_ERR_NOT_FOUND.
 * @param[in] conidx         Connection Id number
 * @return void
 ****************************************************************************************
 */
void default_app_on_addr_resolve_failed(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Default function called on GAPM_CMP_EVT for any RAL operation
 * @param[in] param         Pointer to GAPM_CMP_EVT message
 * @return void
 ****************************************************************************************
 */
void default_app_on_ral_cmp_evt(const struct gapm_cmp_evt *param);

#endif // (BLE_APP_SEC)

#endif // (BLE_APP_PRESENT)

/// @} APP

#endif // _APP_DEFAULT_HANDLERS_H_
