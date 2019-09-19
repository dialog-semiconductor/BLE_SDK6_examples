/**
 ****************************************************************************************
 *
 * @file bcss.h
 *
 * @brief Header file - Body Composition Service Server Application Layer.
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

#ifndef APP_BCSS_H_
#define APP_BCSS_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief Body Composition Service Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_BCS_SERVER)
#include "bcs_common.h"
#include "attm.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

struct app_bcss_cb
{
    void (*on_bcss_enable_rsp)(uint8_t conidx, uint8_t status, uint16_t init_cfg_ind);
    void (*on_bcss_meas_val_ind_rsp)(uint8_t conidx, uint8_t status);
    void (*on_bcss_ind_cfg_ind)(uint8_t conidx, bool status);
};


/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Body Composition Service Application
 ****************************************************************************************
 */
void app_bcss_init(void);

/**
 ****************************************************************************************
 * @brief Add a Body Composition Service instance in the DB
 ****************************************************************************************
 */
void app_bcss_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable Body Composition Service Server.
 * param[in] conidx     Connection handle
 * @return void
 ****************************************************************************************
 */
void app_bcss_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send Body Composition measurement
 * param[in] connhdl          Connection handle
 * param[in] scan_refresh_val Scan Refresh value
 * @return void
 ****************************************************************************************
 */
void app_bcss_send_measurement(uint8_t conidx, const bcs_meas_t *meas);

/**
 ****************************************************************************************
 * @brief Set initial Measurement notifications status restored when service is enabled.
 * param[in] enabled     Whether notifications are enabled or not
 * @return void
 ****************************************************************************************
 */
void app_bcss_set_initial_measurement_ind_cfg(bool enabled);

#endif // (BLE_BCS_SERVER)

/// @} APP

#endif
