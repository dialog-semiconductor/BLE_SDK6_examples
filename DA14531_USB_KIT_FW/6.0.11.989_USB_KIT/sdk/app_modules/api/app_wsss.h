/**
 ****************************************************************************************
 *
 * @file app_wsss.h
 *
 * @brief Weight Scale Profile Server Application header.
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

#ifndef _APP_WSSS_H_
#define _APP_WSSS_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief Weight Scale Service Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_WSS_SERVER)
#include "wss_common.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

struct app_wsss_cb
{
    const struct att_incl_desc *(*on_wsss_bcs_reference_req)(void);
    void (*on_wsss_ind_cfg_ind)(uint8_t conidx, bool ind_en);
    void (*on_wsss_send_measurement_cfm)(uint8_t conidx, uint8_t status);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Weight Scale Service Application
 ****************************************************************************************
 */
void app_wsss_init(void);

/**
 ****************************************************************************************
 * @brief Add a Weight Scale Service instance in the DB
 ****************************************************************************************
 */
void app_wsss_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Weight Scale Profile Server.
 * param[in] conidx     Connection index
 * @return void
 ****************************************************************************************
 */
void app_wsss_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Weight Scale measurement
 * param[in] conidx          Connection index
 * param[in] meas            Weight Scale measurement
 * @return void
 ****************************************************************************************
 */
void app_wsss_send_measurement(uint8_t conidx, const struct wss_wt_meas *meas);

/**
 ****************************************************************************************
 * @brief Set initial Measurement notifications status restored when service is enabled.
 * param[in] enabled     Whether notifications are enabled or not
 * @return void
 ****************************************************************************************
 */
void app_wsss_set_initial_measurement_ind_cfg(bool enabled);

#endif // (BLE_WSS_SERVER)

/// @} APP

#endif // _APP_WSSS_H_

