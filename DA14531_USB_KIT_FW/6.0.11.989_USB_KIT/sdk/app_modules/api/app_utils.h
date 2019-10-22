/**
 ****************************************************************************************
 *
 * @file app_utils.h
 *
 * @brief Application generic utilities helper functions header file.
 *
 * Copyright (c) 2017-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef _APP_UTILS_H_
#define _APP_UTILS_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "rwip_config.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Type of address
enum app_addr_types
{
    /// Public address
    APP_PUBLIC_ADDR_TYPE,
    /// Random Static address
    APP_RANDOM_STATIC_ADDR_TYPE,
    /// Resolvable Private Address
    APP_RANDOM_PRIVATE_RESOLV_ADDR_TYPE,
    /// non-Resolvable Private Address
    APP_RANDOM_PRIVATE_NONRESOLV_ADDR_TYPE,
    /// Address has been resolved by the Controller
    APP_ID_ADDR_TYPE,
    /// Unknown address type
    APP_ERROR_ADDR_TYPE,
};

enum app_ral_operations
{
    /// Get resolving address list size
    APP_GET_RAL_SIZE = GAPM_GET_RAL_SIZE,
    /// Get resolving local address
    APP_GET_RAL_LOC_ADDR = GAPM_GET_RAL_LOC_ADDR,
    /// Get resolving peer address
    APP_GET_RAL_PEER_ADDR = GAPM_GET_RAL_PEER_ADDR,
    /// Add device in resolving address list
    APP_ADD_DEV_IN_RAL = GAPM_ADD_DEV_IN_RAL,
    /// Remove device from resolving address list
    APP_RMV_DEV_FRM_RAL = GAPM_RMV_DEV_FRM_RAL,
    /// Clear resolving address list
    APP_CLEAR_RAL = GAPM_CLEAR_RAL,
    /// Set Network Privacy Mode for peer in resolving list
    APP_NETWORK_MODE_RAL = GAPM_NETWORK_MODE_RAL,
    /// Set Network Privacy Mode for peer in resolving list
    APP_DEVICE_MODE_RAL = GAPM_DEVICE_MODE_RAL,
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Find the type of Bluetooth Device Address
 * @param[in] addr_type PUBLIC or RANDOM type of BDA
 * @param[in] addr      Bluetooth Device Address
 * @return              Type of Bluetooth Device Address
 ****************************************************************************************
 */
enum app_addr_types app_get_address_type(uint8_t addr_type, struct bd_addr addr);

/**
 ****************************************************************************************
 * @brief Fills an array with random bytes (starting from the end of the array)
 *          Assuming an array of array_size and a dynamic key size, so that key_size = M*4+N:
 *          Calls co_rand_word() M times and appends the 4 bytes of each 32 bit return value
 *          to the output array
 *          Calls co_rand_word() once and appends the N most significant bytes of the 32 bit
 *          return value to the output array
 *          Fills the rest bytes of the array with zeroes
 *
 * @param[in] *dst              The address of the array
 * @param[in] key_size          Number of bytes that shall be randomized
 * @param[in] array_size        Total size of the array
 *
 * @return void
 ****************************************************************************************
 */
void app_fill_random_byte_array(uint8_t *dst, uint8_t key_size, uint8_t array_size);

/// @} APP

#endif // _APP_EASY_SECURITY_H_
