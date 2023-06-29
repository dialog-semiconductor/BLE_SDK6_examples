/**
 ****************************************************************************************
 *
 * @file user_ble_gatt.h
 *
 * @brief User BLE GATT header file.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
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

#ifndef _USER_BLE_GATT_H_
#define _USER_BLE_GATT_H_

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

#include <stdint.h>
#include <stdbool.h>
#include "user_central_config.h"
#include "compiler.h"
#include "user_ble_common.h"
#include "gattc_task.h"



/**
 ****************************************************************************************
 * @brief Perform a service discovery
 * @param[in] con_idx - connection identifier
 * @param[in] con_idx - start handle to start discovery 
 * @return void
 ****************************************************************************************
 */
void user_gatt_discover_all_services(uint8_t con_idx, uint16_t start_handle);

/**
 ****************************************************************************************
 * @brief Perform a gatt write
 * @param[in] con_idx - connection identifier
 * @param[in] handle - attribute handle to write 
 * @param[in] data - data to write
 * @param[in] data_len - data len
 * @return void
 ****************************************************************************************
 */
void user_gatt_parse_service(struct gattc_sdp_svc_ind const *svc, service_info_t *svc_ret, uint8_t con_idx);


/**
 ****************************************************************************************
 * @brief Perform a simple gatt read - read is returned by GATTC_READ_IND
 * @param[in] con_idx - connection identifier
 * @param[in] handle - attribute handle to read
 * @return void
 ****************************************************************************************
 */
void user_gatt_read_simple(uint8_t con_idx, uint16_t handle);

/**
 ****************************************************************************************
 * @brief Parse a service from a service discovered indication
 * @param[in] svc - service to parse
 * @param[in] con_idx - connection id
 * @param[out] svc_ret - parsed service
 * @return void
 ****************************************************************************************
 */
void user_ble_gatt_write(uint8_t op, uint8_t con_idx, uint16_t handle, uint8_t *data, uint8_t data_len);


/**
 ****************************************************************************************
 * @brief Return number of characteristics in a service
 * @param[in] svc - pointer to the service discovered
 * @return number of characteristics
 ****************************************************************************************
 */
uint32_t user_ble_get_svc_size(struct gattc_sdp_svc_ind const *svc);

/// @} APP

#endif // _USER_BLE_GATT_H_


