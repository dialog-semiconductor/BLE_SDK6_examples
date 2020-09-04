/**
 ****************************************************************************************
 *
 * @file user_led_alert.h
 *
 * @brief Social distancing software example header file for LED blink.
 *
 * Copyright (c) 2015-2020 Dialog Semiconductor. All rights reserved.
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

#ifndef _USER_LED_ALERT_H_
#define _USER_LED_ALERT_H_

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

#include "rwble_config.h"
#include "app_task.h"                  // application task
#include "gapc_task.h"                 // gap functions and messages
#include "gapm_task.h"                 // gap functions and messages
#include "app.h"                       // application definitions
#include "co_error.h"                  // error code definitions
#include "arch_wdg.h"

#include "app_callback.h"
#include "app_default_handlers.h"
#include "app_easy_timer.h"


/*
 * DEFINES
 ****************************************************************************************
 */
#define ALERT_LED_ON                        (10)        // 100ms stay on

#define INVALID_ZONE                        (0)

#define BLINK(x)                            (x*2)

#define DANGER_ZONE                         BLINK(5)
#define WARNING_ZONE                        BLINK(2)
#define COARSE_ZONE                         BLINK(1)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
typedef void (*timer_led_alert_cb_t)(void);
typedef void (*alert_cmp_t)(void);


/**
 ****************************************************************************************
 * @brief A structure to represent an alert object
 ****************************************************************************************
 */
typedef struct {
    bool                    state;              // pin state
    bool                    alert_active;       // alert mechanish is running
    timer_hnd               alert_timer_hnd;    // timer handler of led alert 
    timer_led_alert_cb_t    cb;                 // timer callback handles blink
    alert_cmp_t             cmp_cb;             // callback when alert is done
    uint16_t                alert_type;         // alert type
} alert_led_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Timer callback function for proximity alert
 * @return void
 ****************************************************************************************
 */
void timer_alert_cb(void);

/**
 ****************************************************************************************
 * @brief Initialization function for proximity alert
 * @return void
 ****************************************************************************************
 */
void alert_user_init(void);

/**
 ****************************************************************************************
 * @brief Stops the proximity proximity alert
 * @return void
 ****************************************************************************************
 */
void alert_user_stop(void);

/**
 ****************************************************************************************
 * @brief Starts the proximity alert
 * @param[in] danger_zone The proximity zone as determined by the RSSI
 * @return void
 ****************************************************************************************
 */
void alert_user_start(uint16_t danger_zone, alert_cmp_t cb);

/// @} APP

#endif //_USER_LED_ALERT_H_
