/**
 ****************************************************************************************
 *
 * @file app_gattc.h
 *
 * @brief Generic Attribute Profile Service Client Application entry point.
 *
 * Copyright (c) 2018 Dialog Semiconductor. All rights reserved.
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
 *
 ****************************************************************************************
 */

#ifndef APP_GATTC_H_
#define APP_GATTC_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief Generic Attribute Service Client Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_GATT_CLIENT)

#include "gatt_client.h"

/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */

struct app_gattc_cb
{
    void (*on_gattc_enable)(uint8_t conidx, uint8_t status, const struct gatt_client_content *gatt);
    void (*on_gattc_rd_cfg_ind_rsp)(uint8_t conidx, uint8_t status, bool enabled);
    void (*on_gattc_wr_cfg_ind_rsp)(uint8_t conidx, uint8_t status);
    void (*on_gattc_svc_changed_ind)(uint8_t conidx, struct prf_svc *val);
};

/*
 ****************************************************************************************
 * @brief Initialize GATT Service Application
 * @return void
 ****************************************************************************************
 */
void app_gattc_init(void);

/**
 ****************************************************************************************
 * @brief Add a GATT Service instance
 * @return void
 ****************************************************************************************
 */
void app_gattc_create_task(void);

/**
 ****************************************************************************************
 * @brief Enable GATT Service client
 * @param[in] conidx            The connection id number
 * @return void
 ****************************************************************************************
 */
void app_gattc_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Write GATT service indication configuration
 * @param[in] conidx            The connection id number
 * @param[in] enable            Indication enable/disable
 * @return void
 ****************************************************************************************
 */
void app_gattc_write_ind_cfg(uint8_t conidx, bool enable);

/**
 ****************************************************************************************
 * @brief Read GATT service indicaion configuration value
 * @param[in] conidx            The connection id number
 * @return void
 ****************************************************************************************
 */
void app_gattc_read_ind_cfg(uint8_t conidx);

#endif // (BLE_GATT_CLIENT)

/// @} APP

#endif // APP_GATTC_H_
