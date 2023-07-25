/**
 ****************************************************************************************
 *
 * \file port_timer.c
 *
 * \brief Timer controller functions
 *
 * Define symbol HAS_TIMERS to include this module in the application.
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

/**
 ****************************************************************************************
 * \addtogroup APP_UTILS
 * \{
 * \addtogroup TIMER
 * \{
 * \addtogroup PORT_TIMER
 * \{
 ****************************************************************************************	 
 */ 

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#ifdef HAS_TIMERS

#include "port_timer.h"
#include "port_platform.h"
#include "ke_env.h"

#ifdef EXTENDED_TIMERS_ON 
    uint16_t extended_timer_cnt[NUM_OF_TIMERS] __PORT_RETAINED;
#endif

timer_hnd port_timer_hnds[NUM_OF_TIMERS] __PORT_RETAINED;

static void port_timer_callback(timer_hnd handler)
{
    for (int i = 0; i < NUM_OF_TIMERS; i++) {
        if (port_timer_hnds[i] == handler) {
            port_timer_hnds[i] = EASY_TIMER_INVALID_TIMER;
#ifdef EXTENDED_TIMERS_ON
            if (extended_timer_cnt[timer_id]) {
                extended_timer_cnt[timer_id]--;
                port_timer_set(timer_id, task_id, KE_TIMER_DELAY_MAX);    
                return;
            }
#endif                
            port_timer_cbs[i]();
            return;
        }
    }
}


/**
 ****************************************************************************************
 * \brief       Creates a timer
 *
 * \param[in]   delay    The time delay of the timer in ms
 * \param[in]   timer_id The created timer's ID
 *
 * \return      bool    True if the timer was successfully created, false otherwise
 ****************************************************************************************
 */
static bool port_timer_create(const uint16_t delay, timer_id_t const timer_id)
{
    if (port_timer_hnds[timer_id] == EASY_TIMER_INVALID_TIMER) {
        port_timer_hnds[timer_id] = app_easy_timer(delay, (timer_callback)port_timer_callback);
    } else {
        ASSERT_ERROR(app_easy_timer_modify(port_timer_hnds[timer_id], delay) != EASY_TIMER_INVALID_TIMER);
    }
    return port_timer_hnds[timer_id] != EASY_TIMER_INVALID_TIMER;
}

void port_timer_set(timer_id_t const timer_id, task_id_t task_handler, uint32_t delay)
{
    // Delay should not be zero
    delay /=10;
    if(delay == 0) {
        delay = 1;
    }
                
#ifdef EXTENDED_TIMERS_ON
    extended_timer_cnt[timer_id] = delay / KE_TIMER_DELAY_MAX;
    delay %= KE_TIMER_DELAY_MAX;
    
    if (delay == 0) {
        delay = KE_TIMER_DELAY_MAX;
        extended_timer_cnt[timer_id]--;
    }
#else
    ASSERT_WARNING(delay <= KE_TIMER_DELAY_MAX); 
#endif    
                
    // Delay shall not be more than maximum allowed
    if(delay > KE_TIMER_DELAY_MAX) {
        delay = KE_TIMER_DELAY_MAX;
    }

    ASSERT_ERROR (port_timer_create(delay, timer_id));
}

void port_timer_clear(timer_id_t const timer_id, task_id_t task_handler)
{
    if(port_timer_hnds[timer_id] != EASY_TIMER_INVALID_TIMER) {
        app_easy_timer_cancel(port_timer_hnds[timer_id]);
        port_timer_hnds[timer_id] = EASY_TIMER_INVALID_TIMER;
    }
    
#ifdef EXTENDED_TIMERS_ON
    extended_timer_cnt[timer_id] = 0;
#endif    
}

/**
 ****************************************************************************************
 * \brief       Gets the current value of a timer 
 *
 * \param[in]   timer_id    The timer's ID
 * \param[in]   task_id     The timer's task ID
 *
 * \return      int The timer's value
 ****************************************************************************************
 */
static int port_timer_get_value(ke_msg_id_t const timer_id, ke_task_id_t const task_id)
{
    int ret = 0;
    
    struct ke_timer *timer = (struct ke_timer *) ke_env.queue_timer.first;

    /* scan the timer queue to look for a message element with the same id and destination*/
    while (timer != NULL) {
        if ((timer->id == timer_id) &&
            (timer->task == task_id) ) {
            /* Element has been found */
            break;
        }

        /* Check  next timer */
        timer = timer->next;
    }

    /* If the element has been found */
    if (timer != NULL) {
        ret = timer->time - port_get_time();
        if (ret < 0) {
            ret += (BLE_GROSSTARGET_MASK+1);        
    }
    }
    
    return ret*10;
}

int port_timer_remove(timer_id_t const timer_id, task_id_t task_handler)
{
    int ret = port_timer_get_value(port_timer_hnds[timer_id] - 1 + APP_TIMER_API_MES0, task_handler);
    
    if (ret != 0) {
        port_timer_clear(timer_id, task_handler);
    }
#ifdef EXTENDED_TIMERS_ON
    ret += extended_timer_cnt[timer_id] * KE_TIMER_DELAY_MAX;
#endif    
    return ret;
}

#endif

/**
 * \}
 * \}
 * \}
 */
