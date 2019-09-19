/**
 ****************************************************************************************
 *
 * @file app_bass.h
 *
 * @brief Battery server application header file.
 *
 * Copyright (c) 2012-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef _APP_BASS_H_
#define _APP_BASS_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Battery Service Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_BATT_SERVER)
#include <stdint.h>
#include "gpio.h"

/*
 * EXTERNAL VARIABLES DECLARATION
 ****************************************************************************************
 */

extern uint8_t cur_batt_level;
extern uint8_t batt_alert_en;
extern uint8_t bat_led_state;

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

struct app_bass_cb
{
    void (*on_batt_level_upd_rsp)(uint8_t status);
    void (*on_batt_level_ntf_cfg_ind)(uint8_t conidx, uint8_t ntf_cfg);
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Battery Service Application.
 * @return void
 ****************************************************************************************
 */
void app_batt_init(void);

/**
 ****************************************************************************************
 * @brief Enable the battery service profile.
 * @param[in] conidx Connection index
 * @return void
 ****************************************************************************************
 */
 void app_bass_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Configure the battery service profile.
 * @param[in] batt_lvl          Battery level
 * @param[in] lvl_alert_used    Battery level to generate alert
 * @param[in] led_port          Port of used LED
 * @param[in] led_pin           Pin of used LED
 * @return void
 ****************************************************************************************
 */
void app_batt_config(uint8_t batt_lvl, uint8_t lvl_alert_used, GPIO_PORT led_port, 
                     GPIO_PIN led_pin);

/**
 ****************************************************************************************
 * @brief Create the battery service profile.
 * @return void
 ****************************************************************************************
 */
void app_bass_create_db(void);

/**
 ****************************************************************************************
 * @brief Reads devices current battery level and sends battery level update request to
 *        Battery Service.
 * @return void
 ****************************************************************************************
 */
void app_batt_lvl(void);

/**
 ****************************************************************************************
 * @brief Sends battery level update request to Battery Service.
 * @param[in] batt_lvl          Battery level to set
 * @return void
 ****************************************************************************************
 */
void app_batt_set_level(uint8_t batt_level);

/**
 ****************************************************************************************
 * @brief Starts battery level polling.
 * @param[in] poll_timeout      Battery Polling frequency
 * @return void
 ****************************************************************************************
 */
void app_batt_poll_start(uint16_t poll_timeout);

/**
 ****************************************************************************************
 * @brief Stops battery level polling.
 * @return void
 ****************************************************************************************
 */
void app_batt_poll_stop(void);

/**
 ****************************************************************************************
 * @brief Starts battery alert. Battery Low.
 * @return void
 ****************************************************************************************
*/
void app_batt_alert_start(void);

/**
 ****************************************************************************************
 * @brief Stops battery alert. Battery Low.
 * @return void
 ****************************************************************************************
 */
void app_batt_alert_stop(void);

/**
 ****************************************************************************************
 * @brief Reinit battery alert LED port. Must be called in periph_init().
 * @return void
 ****************************************************************************************
 */
void app_batt_port_reinit(void);

#endif // (BLE_BATT_SERVER)

/// @} APP

#endif // _APP_BASS_H_
