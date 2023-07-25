/**
 ****************************************************************************************
 *
 * @file wkup_keys_config.h
 *
 * @brief Wakeup keys configuration file.
 *
 * Copyright (C) 2017-2021 Renesas Electronics Corporation and/or its affiliates
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
 * OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */
 
#ifndef WKUP_KEYS_CONFIG_H
#define WKUP_KEYS_CONFIG_H
#include "user_periph_setup.h"

#define WKUP_KEYS_NUM   (1)

#if WKUP_KEYS_NUM
// Push button key configuration
#define WKUP_KEY_0_PORT         INERTIAL_INT_PORT
#define WKUP_KEY_0_PIN          INERTIAL_INT_PIN
#define WKUP_KEY_0_POLARITY     WKUP_KEY_ACTIVE_LOW
#define WKUP_KEY_0_MODE         INPUT_PULLUP
#define WKUP_KEY_0_CHANNEL      WKUP_KEYS_CHANNEL_1
#endif

#if (WKUP_KEYS_NUM > 1)
// Push button key configuration
#define WKUP_KEY_1_PORT         PUSH_BUTTON_1_PORT
#define WKUP_KEY_1_PIN          PUSH_BUTTON_1_PIN
#define WKUP_KEY_1_POLARITY     WKUP_KEY_ACTIVE_LOW
#define WKUP_KEY_1_MODE         INPUT_PULLUP
#define WKUP_KEY_1_CHANNEL      WKUP_KEYS_CHANNEL_2
#endif

#if (WKUP_KEYS_NUM > 2)
// Push button key configuration
#define WKUP_KEY_2_PORT         GPIO_PORT_x
#define WKUP_KEY_2_PIN          GPIO_PIN_x
#define WKUP_KEY_2_POLARITY     WKUP_KEY_ACTIVE_HIGH
#define WKUP_KEY_2_MODE         INPUT
#define WKUP_KEY_2_CHANNEL      WKUP_KEYS_CHANNEL_1
#endif

#define WKUP_KEYS_CFG_DEBOUNCE_TIME_MS  (10)

#define WKUP_KEYS_USE_BOTH_CHANNELS     (1)

enum wkup_keys
{
    INERTIAL_SENSOR_INTERRUPT_KEY,
    PUSH_BUTTON_1_KEY,
};

void user_wkup_keys_notification_cb(enum wkup_keys key, bool pressed);

/**
 ****************************************************************************************
 * \brief This callback will be called to notify the application that a key has been
 *        pressed or released.
 ****************************************************************************************
 */

#define WKUP_KEYS_NOTIFICATION_CB user_wkup_keys_notification_cb
#endif // WKUP_KEYS_CONFIG_H
