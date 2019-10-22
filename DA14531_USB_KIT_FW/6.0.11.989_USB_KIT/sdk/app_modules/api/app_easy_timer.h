/**
 ****************************************************************************************
 *
 * @file app_easy_timer.h
 *
 * @brief The easy timer api definitions.
 *
 * Copyright (c) 2015-2019 Dialog Semiconductor. All rights reserved.
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

#ifndef _APP_EASY_TIMER_H_
#define _APP_EASY_TIMER_H_

/**
 ****************************************************************************************
 * @addtogroup APP_TIMER
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

#include "ke_msg.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Timer handler type
typedef uint8_t timer_hnd;

/// Timer callback function type definition
typedef void (* timer_callback)(void);

#if defined (__DA14531__) && !defined (__EXCLUDE_ROM_APP_EASY_TIMER__)
typedef struct
{
    timer_callback *timer_callbacks;
    timer_callback *modified_timer_callbacks;
    bool (*app_check_BLE_active)(void);
    bool (*arch_ble_force_wakeup)(void);
} rom_app_easy_timer_cfg_t;
#endif

/*
 * DEFINES
 ****************************************************************************************
 */

// Max timer delay 41943sec (4194300 * 10ms)
#define KE_TIMER_DELAY_MAX          (4194300)

// Value indicating an invalide timer operation
#define EASY_TIMER_INVALID_TIMER    (0x0)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Process handler for the Easy Timer messages.
 * @param[in] msgid   Id of the message received
 * @param[in] param   Pointer to the parameters of the message
 * @param[in] dest_id ID of the receiving task instance (probably unused)
 * @param[in] src_id  ID of the sending task instance
 * @param[in] msg_ret Result of the message handler
 * @return Returns if the message is handled by the process handler
 ****************************************************************************************
 */
enum process_event_response app_timer_api_process_handler(ke_msg_id_t const msgid,
                                                          void const *param,
                                                          ke_task_id_t const dest_id,
                                                          ke_task_id_t const src_id,
                                                          enum ke_msg_status_tag *msg_ret);

/**
 ****************************************************************************************
 * @brief Create a new timer. Activate the ble if required.
 * @param[in] delay The amount of timer slots (10 ms) to wait (time resolution is 10ms)
 * @param[in] fn    The callback to be called when the timer expires
 * @return The handler of the timer for future reference. If there are not timers available
 *         EASY_TIMER_INVALID_TIMER will be returned
 ****************************************************************************************
 */
timer_hnd app_easy_timer(const uint32_t delay, timer_callback fn);

/**
 ****************************************************************************************
 * @brief Cancel an active timer.
 * @param[in] timer_id The timer handler to cancel
 * @return void
 ****************************************************************************************
 */
void app_easy_timer_cancel(const timer_hnd timer_id);

/**
 ****************************************************************************************
 * @brief Modify the delay of an existing timer.
 * @param[in] timer_id The timer handler to modify
 * @param[in] delay    The new delay value (time resolution is 10ms)
 * @return The timer handler if everything is ok
 ****************************************************************************************
 */
timer_hnd app_easy_timer_modify(const timer_hnd timer_id, const uint32_t delay);

/**
 ****************************************************************************************
 * @brief Cancel all the active timers.
 * @return void
 ****************************************************************************************
 */
void app_easy_timer_cancel_all(void);

/// @} APP_TIMER

#endif // _APP_EASY_TIMER_H_
