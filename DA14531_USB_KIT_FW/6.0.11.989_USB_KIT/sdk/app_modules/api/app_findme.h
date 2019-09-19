/**
 ****************************************************************************************
 *
 * @file app_findme.h
 *
 * @brief Findme Target/Locator application.
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

#ifndef _APP_FINDME_H_
#define _APP_FINDME_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Findme Target/Locator Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdbool.h>
#include "user_profiles_config.h"

#if (BLE_FINDME_TARGET)
#include "gpio.h"
#include "findt_task.h"
#endif

#if (BLE_FINDME_LOCATOR)
#include "findl_task.h"
#endif

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

#if (BLE_FINDME_TARGET) || (BLE_FINDME_LOCATOR)
struct app_findt_cb
{
#if (BLE_FINDME_TARGET)
    void (*on_findt_alert_ind)(uint8_t conidx, uint8_t alert_lvl);
#endif

#if (BLE_FINDME_LOCATOR)
    void (*on_findl_enable_rsp)(struct findl_enable_rsp const *param);
    void (*on_findl_set_alert_rsp)(uint8_t status);
#endif
};
#endif //(BLE_FINDME_TARGET) || (BLE_FINDME_LOCATOR)

#if (BLE_FINDME_TARGET)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef struct
{
    uint32_t blink_timeout;
    uint8_t blink_toggle;
    uint8_t lvl;
    GPIO_PORT port;
    GPIO_PIN pin;
} app_alert_state;

extern app_alert_state alert_state;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Findme Target profile.
 * @return void
 ****************************************************************************************
 */
void app_findt_init(void);

/**
 ****************************************************************************************
 * @brief Create the Findme Target task.
 * @return void
 ****************************************************************************************
 */
void app_findt_create_db(void);

/**
 ****************************************************************************************
 * @brief Start the FindMe Target alert logic.
 * @param[in] lvl       The alert level. The valid values are:
 *                          FINDL_ALERT_NONE
 *                          FINDL_ALERT_MILD
 *                          FINDL_ALERT_HIGH
 * @return void
 ****************************************************************************************
 */
void app_findt_alert_start(uint8_t lvl);

/**
 ****************************************************************************************
 * @brief Stop the FindMe Target alert logic.
 * @return void
 ****************************************************************************************
 */
void app_findt_alert_stop(void);

/**
 ****************************************************************************************
 * @brief Default Findme Target alert indication handler.
 * @param[in] conidx            The connection id number
 * @param[in] lvl               The alert level
 * @return void
 ****************************************************************************************
 */
void default_findt_alert_ind_handler(uint8_t conidx, uint8_t lvl);
#endif // BLE_FINDME_TARGET

#if (BLE_FINDME_LOCATOR)
/**
 ****************************************************************************************
 * @brief Initialize Findme Locator profile.
 * @return void
 ****************************************************************************************
 */
void app_findl_init(void);

/**
 ****************************************************************************************
 * @brief Create the Findme Locator task.
 * @return void
 ****************************************************************************************
 */
void app_findl_create_task(void);

/**
 ****************************************************************************************
 * @brief Enable Findme Locator profile.
 * @param[in] conidx            The connection id number
 * @return void
 ****************************************************************************************
 */
void app_findl_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Set alert level.
 * @param[in] lvl       The alert level. The valid values are:
 *                          FINDL_ALERT_NONE
 *                          FINDL_ALERT_MILD
 *                          FINDL_ALERT_HIGH
 * @return void
 ****************************************************************************************
 */
void app_findl_set_alert(uint8_t lvl);
#endif // BLE_FINDME_LOCATOR

/// @} APP

#endif // _APP_FINDME_H_
