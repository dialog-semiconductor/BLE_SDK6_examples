/**
 ****************************************************************************************
 *
 * @file user_led_alert.h
 *
 * @brief Social distancing software example header file for LED blink.
 *
 * Copyright (c) 2015-2021 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
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
#define INVALID_ZONE                        (0)
#define DANGER_ZONE                         2   // 20ms
#define WARNING_ZONE                        10  // 100ms
#define COARSE_ZONE                         15  // 150ms
#define ALERT_TIME                          100 // 1sec

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
typedef void (*timer_led_alert_cb_t)(void);
typedef void (*alert_cmp_t)(uint8_t);


/**
 ****************************************************************************************
 * @brief A structure to represent an alert object
 ****************************************************************************************
 */
typedef struct {
    bool                    state;              // pin state
    bool                    alert_active;       // alert mechanish is running
    timer_hnd               alert_timer_interval_hnd;    // timer handler of led alert
    timer_hnd               alert_timer_end_hnd;
    timer_led_alert_cb_t    cb;                 // timer callback handles blink
    alert_cmp_t             cmp_cb;             // callback when alert is done
    uint16_t                alert_type;         // alert type
    uint8_t                 con_idx;            // connection index
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
 * @brief Stops the proximity alert
 * @return void
 ****************************************************************************************
 */
void alert_user_stop(void);

/**
 ****************************************************************************************
 * @brief Starts the proximity alert
 * @param[in] danger_zone, The proximity zone defines the amount of on/off alerts
 * @param[in] cb, callback, for the action after the alert is done
 * @param[in] con_idx, connection index of the current connection
 * @return void
 ****************************************************************************************
 */
void alert_user_start(uint16_t danger_zone, alert_cmp_t cb, uint8_t con_idx);

/// @} APP

#endif //_USER_LED_ALERT_H_
