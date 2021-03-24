/**
 ****************************************************************************************
 *
 * @file user_ble_gap.h
 *
 * @brief User BLE GAP header file.
 *
 * Copyright (c) 2012-2020 Dialog Semiconductor. All rights reserved.
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

#ifndef _USER_BLE_GAP_H_
#define _USER_BLE_GAP_H_

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
 * @brief Start a central scan
 * @param[in] scan_active - perform active or passive scan
 * @param[in] interval_ms - scan interval in milliseconds
 * @param[in] window_ms		- scan window in milliseconds
 * @param[in] filter_dupl - filter duplicate advertisements (buffer only size 10)
 * @return ble_gap_eror_t error code
 ****************************************************************************************
 */
ble_gap_error_t user_ble_gap_start_scan(bool scan_active, uint16_t interval_ms, uint16_t window_ms, bool filter_dupl);


/**
 ****************************************************************************************
 * @brief Get number of advertisement elements
 * @param[in] data - pointer to advertisment data
 * @param[in] len - data length
 * @return number of elements
 ****************************************************************************************
 */
uint8_t user_ble_gap_get_adv_num_elements(const uint8_t *data, uint8_t len);


/**
 ****************************************************************************************
 * @brief parse advertisment data into ad_elements structure
 * @param[in] in_len - advertisement data length to parse
 * @param[in] in_data - data to parse
 * @param[out] ad_elements - parsed data
 * @return void
 ****************************************************************************************
 */
void user_ble_gap_parse_adv_data(uint8_t in_len, const uint8_t *in_data, ble_gap_adv_struct_t *ad_elements);

 /**
 ****************************************************************************************
 * @brief Cancel scan operation
 * @return void
 ****************************************************************************************
 */
void user_ble_gap_stop_scan(void);


/**
 ****************************************************************************************
 * @brief Start a connection
 * @param[in] bd_addr - bluetooth device address to connect to
 * @param[in] addr_type - PUBLIC/PRIVATE
 * @return void 
 ****************************************************************************************
 */
void user_ble_gap_connect(const uint8_t *bd_addr, uint8_t addr_type);


/**
 ****************************************************************************************
 * @brief Cancel connect operation
 * @return void
 ****************************************************************************************
 */
void user_ble_cancel_connect(void);










/// @} APP

#endif // _USER_BLE_GAP_H_

