/**
 ****************************************************************************************
 *
 * @file app_easy_gap.h
 *
 * @brief Easy GAP API.
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

#ifndef _APP_EASY_GAP_H_
#define _APP_EASY_GAP_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_APP_PRESENT)

#include <stdint.h>          // Standard Integer Definition
#include "co_bt.h"           // Common BT Definitions
#include "arch.h"            // Platform Definitions
#include "gapc.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Connection handle to connection Id transformation.
 * @param[in] conhdl    Connection handle number.
 * @return Connection Id number
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t conhdl_to_conidx(uint16_t conhdl)
{
    return (gapc_get_conidx(conhdl));
}

/**
 ****************************************************************************************
 * @brief Connection Id to connection handle transformation.
 * @param[in] conidx    Connection Id number.
 * @return Connection handle number
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t conidx_to_conhdl(uint8_t conidx)
{
    return (gapc_get_conhdl(conidx));
}

/**
 ****************************************************************************************
 * @brief Connection handle to connection Id transformation.
 * @return Connection Id number
 ****************************************************************************************
 */
int8_t active_conhdl_to_conidx(uint16_t conhdl);

/**
 ****************************************************************************************
 * @brief Connection Id to connection handle transformation
 * @return Connection handle number
 ****************************************************************************************
 */
int16_t active_conidx_to_conhdl(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send BLE disconnect command.
 * @param[in] conidx    Connection Id
 * @return void
 ****************************************************************************************
 */
void app_easy_gap_disconnect(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send a connection confirmation message.
 * @param[in] conidx                    Connection Id number
 * @param[in] auth                      Authentication requirements (@see gap_auth)
 * @param[in] svc_changed_ind_enable    Service changed indication enable/disable
 * @return void
 ****************************************************************************************
 */
void app_easy_gap_confirm(uint8_t conidx, enum gap_auth auth, uint8_t svc_changed_ind_enable);

/**
 ****************************************************************************************
 * @brief Start advertising for undirected connection.
 * @return void
 ****************************************************************************************
 */
void app_easy_gap_undirected_advertise_start(void);

/**
 ****************************************************************************************
 * @brief Start advertising for directed connection.
 * @param[in] ldc_enable       Enable/disable low duty cycle mode. 
 *                                 - 0 = disabled
 *                                 - 1 = enabled
 * @return void
 ****************************************************************************************
 */
void app_easy_gap_directed_advertise_start(uint8_t ldc_enable);

/**
 ****************************************************************************************
 * @brief Start advertising for non-connectable peripheral.
 * @return void
 ****************************************************************************************
 */
void app_easy_gap_non_connectable_advertise_start(void);

/**
 ****************************************************************************************
 * @brief Stop advertising.
 * @return void
 ****************************************************************************************
 */
void app_easy_gap_advertise_stop(void);

/**
 ****************************************************************************************
 * @brief Start advertising for undirected connection with timeout start.
 * @param[in] delay            Delay of the timer
 * @param[in] timeout_callback Timeout callback function
 * @return void
 ****************************************************************************************
 */
void app_easy_gap_undirected_advertise_with_timeout_start(uint32_t delay, void (*timeout_callback)(void));

/**
 ****************************************************************************************
 * @brief Stop advertising with timeout stop.
 * @return void
 ****************************************************************************************
 */
void app_easy_gap_advertise_with_timeout_stop(void);

/**
 ****************************************************************************************
 * @brief Get non connectable advertising message with filled parameters.
 * @return gapm_start_advertise_cmd Pointer to advertising message
 ****************************************************************************************
*/
struct gapm_start_advertise_cmd* app_easy_gap_non_connectable_advertise_get_active(void);

/**
 ****************************************************************************************
 * @brief Get undirected connectable advertising message with filled parameters.
 * @return gapm_start_advertise_cmd Pointer to advertising message
 ****************************************************************************************
*/
struct gapm_start_advertise_cmd* app_easy_gap_undirected_advertise_get_active(void);

/**
 ****************************************************************************************
 * @brief Get directed connectable advertising message with filled parameters. It
 *        supports the low duty cycle directed advertising mode.
 * @param[in] ldc_enable       Enable/disable low duty cycle mode.
 *                                 - 0 = disabled
 *                                 - 1 = enabled
 * @return gapm_start_advertise_cmd Pointer to advertising message
 ****************************************************************************************
*/
struct gapm_start_advertise_cmd* app_easy_gap_directed_advertise_get_active(uint8_t ldc_enable);

/**
 ****************************************************************************************
 * @brief Send the command which updates the advertising or scan response data.
 * @param[in] update_adv_data               Buffer that holds the new advertising data.
 * @param[in] update_adv_data_len           Length of the advertising data buffer.
 * @param[in] update_scan_rsp_data          Buffer that holds the new scan response data.
 * @param[in] update_scan_rsp_data_len      Length of the scan response data buffer.
 * @return void
 ****************************************************************************************
 */
void app_easy_gap_update_adv_data(const uint8_t *update_adv_data,
                                  uint8_t update_adv_data_len,
                                  const uint8_t *update_scan_rsp_data,
                                  uint8_t update_scan_rsp_data_len);

/**
 ****************************************************************************************
 * @brief Param update start request.
 * @param[in] conidx    Connection Id number
 * @return void
 ****************************************************************************************
*/
void app_easy_gap_param_update_start(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Get param update request message with filled parameters.
 * @param[in] conidx    Connection Id number
 * @return gapc_param_update_cmd Pointer to param update request message
 ****************************************************************************************
*/
struct gapc_param_update_cmd* app_easy_gap_param_update_get_active(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Start connection.
 * @return void
 ****************************************************************************************
*/
void app_easy_gap_start_connection_to(void);

/**
 ****************************************************************************************
 * @brief Start direct connection to known peer.
 * @param[in] peer_addr_type    Peer to connect address type
 * @param[in] peer_addr         Peer to connect address
 * @param[in] intv              Connection interval
 * @return void
 ****************************************************************************************
*/
void app_easy_gap_start_connection_to_set(uint8_t peer_addr_type,
                                          const uint8_t *peer_addr, uint16_t intv);

/**
 ****************************************************************************************
 * @brief Get connection message with filled parameters
 * @return gapm_start_connection_cmd Pointer to connection message
 ****************************************************************************************
*/
struct gapm_start_connection_cmd* app_easy_gap_start_connection_to_get_active(void);

/**
 ****************************************************************************************
 * @brief Get device configuration message with filled parameters
 * @return gapm_set_dev_config_cmd Pointer to device configuration message
 ****************************************************************************************
*/
struct gapm_set_dev_config_cmd* app_easy_gap_dev_config_get_active(void);

/**
 ****************************************************************************************
 * @brief Configure device
 * @return void
 ****************************************************************************************
*/
void app_easy_gap_dev_configure(void);

/**
 ****************************************************************************************
 * @brief Requests peer device features info.
 * @param[in] conidx Index of connection to peer device.
 * @return void
 ****************************************************************************************
 */
void app_easy_gap_get_peer_features(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Set maximum transmission packet size and maximum packet transmission time to be 
 *        used for the connection. This command may be sent either from peripheral or 
 *        central.
 * @param[in] conidx            Connection Id number.
 * @param[in] tx_octets         Maximum transmission packet size.
 * @param[in] tx_time           Maximum packet transmission time.
 * @return void
 * @warning @c app_easy_gap_get_peer_features() function shall be called prior
 *          to calling this function, to determine whether peer device supports
 *          LE Data Packet Length Extension.
 ****************************************************************************************
*/
void app_easy_gap_set_data_packet_length(uint8_t conidx, uint16_t tx_octets, uint16_t tx_time);

/// @} APP

#endif //(BLE_APP_PRESENT)

#endif // _APP_EASY_GAP_H_
