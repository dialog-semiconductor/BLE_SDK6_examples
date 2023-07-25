/**
 ****************************************************************************************
 *
 * @file user_hibernation_timer.h
 *
 * @brief BLE Eddystone beacon with hibernation or deep sleep project header file. 
 *
 * Copyright (c) 2015-2021 Dialog Semiconductor
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

#ifndef _USER_HIBERNATION_TIMER_H_
#define _USER_HIBERNATION_TIMER_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup
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
 
#include "gapc_task.h"                 // gap functions and messages
#include "app_task.h"                  // application task
#include "app.h"                       // application definitions
#include "app_callback.h"
#include "arch_api.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/* Define hibernation mode */
#undef CFG_HIBERNATION_MODE
#define  CFG_DEEP_SLEEP_MODE

#if defined(CFG_HIBERNATION_MODE) && defined(CFG_DEEP_SLEEP_MODE)
#error "Config error: CFG_HIBERNATION_MODE and CFG_DEEP_SLEEP_MODE cannot be defined at the same time"
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Application initialization function.
 * @return void
 ****************************************************************************************
*/
void user_app_on_init(void);

/**
 ****************************************************************************************
 * @brief Advertising function.
 * @return void
 ****************************************************************************************
*/
void user_app_adv_start(void);

/**
 ****************************************************************************************
 * @brief Undirect non-connectable advertising completion function.
 * @param[in] status Command complete event message status
 * @return void
 ****************************************************************************************
*/
void user_app_on_adv_nonconn_complete(uint8_t status);

/**
 ****************************************************************************************
 * @brief Function called when BLE is powered. 
 * @return GOTO_SLEEP
 * @note Return type allows the device to enter sleep.
 ****************************************************************************************
*/
arch_main_loop_callback_ret_t user_on_ble_powered(void);

/// @} APP

#endif //_USER_HIBERNATION_TIMER_H_
