/**
 ****************************************************************************************
 *
 * @file app_easy_timer.c
 *
 * @brief The easy timer api implementation.
 *
 * Copyright (c) 2016-2019 Dialog Semiconductor. All rights reserved.
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

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT)
#include "ke_timer.h"
#include "ke_msg.h"
#include "arch_api.h"
#include "app.h"
#include "app_msg_utils.h"
#include "app_entry_point.h"
#include "app_easy_timer.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define APP_TIMER_MAX_NUM                         (APP_TIMER_API_LAST_MES - APP_TIMER_API_MES0 + 1)
/*
    HND: Timer handler values = 1...APP_TIMER_MAX_NUM
    IDX: The index to the table = 0...APP_TIMER_MAX_NUM-1
    The reason of HND and IDX are different has to do with the fact that the retention
    should be populated with zero init data. This makes difficult to have the invalid hnd in a
    value other than zero.
 */
#define APP_EASY_TIMER_HND_TO_MSG_ID(timer_id)    (timer_id - 1 + APP_TIMER_API_MES0)
#define APP_EASY_TIMER_MSG_ID_TO_HND(timer_msg)   (timer_msg - APP_TIMER_API_MES0 + 1)
#define APP_EASY_TIMER_HND_TO_IDX(timer_id)       (timer_id - 1)
#define APP_EASY_TIMER_IDX_TO_HND(timer_id)       (timer_id + 1)
#define APP_EASY_TIMER_HND_IS_VALID(timer_id)     ((timer_id > 0) && (timer_id <= APP_TIMER_MAX_NUM))

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

struct create_timer_struct
{
    timer_hnd timer_id;
    uint32_t delay;
};

struct cancel_timer_struct
{
    timer_hnd timer_id;
    uint32_t delay;
};

struct modify_timer_struct
{
    timer_hnd timer_id;
    uint32_t delay;
};

// Array that holds the callback function of the active timers
static timer_callback timer_callbacks[APP_TIMER_MAX_NUM]          __SECTION_ZERO("retention_mem_area0");

// Array that holds the callback function of the active timers, whose delay period is to be modified
static timer_callback modified_timer_callbacks[APP_TIMER_MAX_NUM] __SECTION_ZERO("retention_mem_area0");

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_EASY_TIMER__)

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Place a callback in the first available position in the timer callback array.
 * @param[in] fn The callback to be added
 * @return The handler of the timer for future reference. EASY_TIMER_INVALID_TIMER if
 * there is no timer available
 ****************************************************************************************
 */
static timer_hnd set_callback(timer_callback fn)
{
    for (int i = 0; i < APP_TIMER_MAX_NUM; i++)
    {
        if (timer_callbacks[i] == NULL)
        {
            timer_callbacks[i] = fn;
            return APP_EASY_TIMER_IDX_TO_HND(i);
        }
    }
    return EASY_TIMER_INVALID_TIMER;
}

/**
 ****************************************************************************************
 * @brief Dummy callback used when canceling a timer.
 * @details This callback is registered in case the application tries to cancel a timer
 * just before it expires.
 * @return void
 ****************************************************************************************
 */
static void timer_canceled_handler(void)
{
/*
   Do nothing
 */
}

/**
 ****************************************************************************************
 * @brief Dummy callback used when modifying a timer.
 * @details This callback is registered in case the application tries to modify a timer
 * just before it expires.
 * @return void
 ****************************************************************************************
 */
static void timer_modified_handler(void)
{
/*
   Do nothing
 */
}

/**
 ****************************************************************************************
 * @brief Call the callback of a specific timer handler if it exists.
 * @param[in] timer_id The handler to call
 * @return void
 ****************************************************************************************
 */
static void call_callback(const timer_hnd timer_id)
{
    void (*return_timer_cb)(timer_hnd timer_id);

    if APP_EASY_TIMER_HND_IS_VALID(timer_id)
    {
        if (timer_callbacks[APP_EASY_TIMER_HND_TO_IDX(timer_id)] != NULL)
        {
            timer_callback fn = timer_callbacks[APP_EASY_TIMER_HND_TO_IDX(timer_id)];
            // Check if the a timer to be modified or canceled has expired
            if ((fn != timer_modified_handler) && (fn != timer_canceled_handler))
            {
                timer_callbacks[APP_EASY_TIMER_HND_TO_IDX(timer_id)] = NULL;
                modified_timer_callbacks[APP_EASY_TIMER_HND_TO_IDX(timer_id)] = NULL;
                return_timer_cb = (void (*)(timer_hnd))fn;
                return_timer_cb(timer_id);
            }
        }
    }
    else
    {
        ASSERT_WARNING(0);
    }
}

/**
 ****************************************************************************************
 * @brief Helper function that creates a timer.
 * @details If BLE is active it will immediately create the timer. If BLE is in sleep mode,
 * it will force a BLE wakeup and will send a message to wait for BLE to be actually woken
 * up. The timer will be created when the APP_CREATE_TIMER message is handled in
 * create_timer_handler().
 * @param[in] timer_id The timer handler
 * @param[in] delay    The delay of the timer
 * @return void
 ****************************************************************************************
 */
static void create_timer(timer_hnd timer_id, uint32_t delay)
{
    if (app_check_BLE_active())
    {
        ke_timer_set(APP_EASY_TIMER_HND_TO_MSG_ID(timer_id), TASK_APP, delay);
    }
    else
    {
        arch_ble_force_wakeup(); //wake_up BLE
        //send a message to wait for BLE to be woken up before executing the
        struct create_timer_struct *req = KE_MSG_ALLOC(APP_CREATE_TIMER, TASK_APP, TASK_APP,
                                                       create_timer_struct);

        req->delay = delay;
        req->timer_id = timer_id;
        ke_msg_send(req);
    }
}

/**
 ****************************************************************************************
 * @brief Handler function that is called when the TASK_APP receives the APP_CREATE_TIMER
 *        message. Called after the ble wakes up in the case where the ble is sleeping
 *        when trying to set a new timer.
 * @param[in] msgid Id of the message received
 * @param[in] param The timer details to be set
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id ID of the sending task instance
 * @return KE_MSG_CONSUMED
 ****************************************************************************************
 */
static int create_timer_handler(ke_msg_id_t const msgid,
                                struct create_timer_struct const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
    // Sanity checks
    ASSERT_ERROR(param->delay > 0);                  // Delay should not be zero
    ASSERT_ERROR(param->delay < KE_TIMER_DELAY_MAX); // Delay should not be more than maximum allowed

    ke_timer_set(APP_EASY_TIMER_HND_TO_MSG_ID(param->timer_id), TASK_APP, param->delay);
    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handler function that is called when the TASK_APP receives the APP_CANCEL_TIMER
 *        message.
 * @param[in] msgid Id of the message received
 * @param[in] param The timer details to be canceled
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id ID of the sending task instance
 * @return KE_MSG_CONSUMED
 ****************************************************************************************
 */
static int cancel_timer_handler(ke_msg_id_t const msgid,
                                struct cancel_timer_struct const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
    if APP_EASY_TIMER_HND_IS_VALID(param->timer_id)
    {
        int i = APP_EASY_TIMER_HND_TO_IDX(param->timer_id);
        if (timer_callbacks[i] == timer_canceled_handler)
        {
            timer_callbacks[i] = NULL;
            modified_timer_callbacks[i] = NULL;
        }
        else if (timer_callbacks[i] == timer_modified_handler)
        {
            struct modify_timer_struct *req = KE_MSG_ALLOC(APP_MODIFY_TIMER,
                                                           TASK_APP,
                                                           TASK_APP,
                                                           modify_timer_struct);

            req->timer_id = param->timer_id;
            req->delay = param->delay;
            ke_msg_send(req);
        }
    }
    else
    {
        ASSERT_ERROR(0);
    }
    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handler function that is called when the TASK_APP receives the APP_MODIFY_TIMER
 *        message.
 * @param[in] msgid Id of the message received
 * @param[in] param The timer details to be modified
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id ID of the sending task instance
 * @return KE_MSG_CONSUMED
 ****************************************************************************************
 */
static int modify_timer_handler(ke_msg_id_t const msgid,
                                struct modify_timer_struct const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
    if APP_EASY_TIMER_HND_IS_VALID(param->timer_id)
    {
        int i = APP_EASY_TIMER_HND_TO_IDX(param->timer_id);
        if (timer_callbacks[i] == timer_modified_handler)
        {
            // Restore timer callback function
            timer_callbacks[i] = modified_timer_callbacks[i];

            // Re-create the timer with new delay
            create_timer(param->timer_id, param->delay);
        }
    }
    else
    {
        ASSERT_ERROR(0);
    }
    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief The actual timer handler that calls the user callback.
 * @param[in] msgid Id of the message received
 * @param[in] param No parameters are required
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id ID of the sending task instance
 * @return KE_MSG_CONSUMED
 ****************************************************************************************
 */
static int call_callback_handler(ke_msg_id_t const msgid,
                                 void const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
    call_callback(APP_EASY_TIMER_MSG_ID_TO_HND(msgid));
    return KE_MSG_CONSUMED;
}

enum process_event_response app_timer_api_process_handler(ke_msg_id_t const msgid,
                                                          void const *param,
                                                          ke_task_id_t const dest_id,
                                                          ke_task_id_t const src_id,
                                                          enum ke_msg_status_tag *msg_ret)
{
    switch (msgid)
    {
        case APP_CANCEL_TIMER:
            *msg_ret = (enum ke_msg_status_tag)cancel_timer_handler(msgid, param, dest_id, src_id);
            return PR_EVENT_HANDLED;

        case APP_MODIFY_TIMER:
            *msg_ret = (enum ke_msg_status_tag)modify_timer_handler(msgid, param, dest_id, src_id);
            return PR_EVENT_HANDLED;

        case APP_CREATE_TIMER:
            *msg_ret = (enum ke_msg_status_tag)create_timer_handler(msgid, param, dest_id, src_id);
            return PR_EVENT_HANDLED;

        default:
            if ((msgid < APP_TIMER_API_MES0) || (msgid > APP_TIMER_API_LAST_MES))
            {
                return PR_EVENT_UNHANDLED;
            }
            else
            {
                *msg_ret = (enum ke_msg_status_tag)call_callback_handler(msgid, param, dest_id, src_id);
            }
            return PR_EVENT_HANDLED;
    }
}

timer_hnd app_easy_timer(const uint32_t delay, timer_callback fn)
{
    // Sanity checks
    ASSERT_ERROR(delay > 0);                  // Delay should not be zero
    ASSERT_ERROR(delay < KE_TIMER_DELAY_MAX); // Delay should not be more than maximum allowed

    timer_hnd timer_id = set_callback(fn);
    if (timer_id == EASY_TIMER_INVALID_TIMER)
    {
        return EASY_TIMER_INVALID_TIMER; //No timers available
    }

    // Create timer
    create_timer(timer_id, delay);

    return timer_id;
}

void app_easy_timer_cancel(const timer_hnd timer_id)
{
    if APP_EASY_TIMER_HND_IS_VALID(timer_id)
    {
        if ((timer_callbacks[APP_EASY_TIMER_HND_TO_IDX(timer_id)] != NULL) &&
            (timer_callbacks[APP_EASY_TIMER_HND_TO_IDX(timer_id)] != timer_canceled_handler))
        {
            // Remove the timer from the timer queue
            ke_timer_clear(APP_EASY_TIMER_HND_TO_MSG_ID(timer_id), TASK_APP);

            timer_callbacks[APP_EASY_TIMER_HND_TO_IDX(timer_id)] = timer_canceled_handler;
            /*
                Send a message to the kernel in order to clear the timer callback function and
                free the respective position in the timers callback array.
                The app_easy_timer_cancel() function cannot guarantee if a timer has entered
                the message queue or not. Therefore a message must be sent to the kernel and
                inform it about the requested cancel operation.
            */
            struct cancel_timer_struct *req = KE_MSG_ALLOC(APP_CANCEL_TIMER, TASK_APP, TASK_APP,
                                                           cancel_timer_struct);

            req->timer_id = timer_id;
            ke_msg_send(req);
        }
        else
        {
            ASSERT_WARNING(0);
        }
   }
   else
   {
       ASSERT_WARNING(0);
   }
}

timer_hnd app_easy_timer_modify(const timer_hnd timer_id, uint32_t delay)
{
    // Sanity checks
    ASSERT_ERROR(delay > 0);                  // Delay should not be zero
    ASSERT_ERROR(delay < KE_TIMER_DELAY_MAX); // Delay should not be more than maximum allowed

    if APP_EASY_TIMER_HND_IS_VALID(timer_id)
    {
        if ((timer_callbacks[APP_EASY_TIMER_HND_TO_IDX(timer_id)] != NULL) &&
            (timer_callbacks[APP_EASY_TIMER_HND_TO_IDX(timer_id)] != timer_modified_handler))
        {
            // Remove the timer from the timer queue
            ke_timer_clear(APP_EASY_TIMER_HND_TO_MSG_ID(timer_id), TASK_APP);

            // Store the timer function callback
            modified_timer_callbacks[APP_EASY_TIMER_HND_TO_IDX(timer_id)] = timer_callbacks[APP_EASY_TIMER_HND_TO_IDX(timer_id)];

            timer_callbacks[APP_EASY_TIMER_HND_TO_IDX(timer_id)] = timer_modified_handler;
            /*
                Send a message to the kernel in order to clear the timer callback function and
                free the respective position in the timers callback array.
                The app_easy_timer_cancel() function cannot guarantee if a timer has entered
                the message queue or not. Therefore a message must be sent to the kernel and
                inform it about the requested cancel operation.
            */
            struct cancel_timer_struct *req = KE_MSG_ALLOC(APP_CANCEL_TIMER, TASK_APP, TASK_APP,
                                                           cancel_timer_struct);

            req->timer_id = timer_id;
            req->delay = delay;
            ke_msg_send(req);

            return timer_id;
        }
        else
        {
            ASSERT_WARNING(0);
        }
    }
    else
    {
        ASSERT_WARNING(0);
    }
    return EASY_TIMER_INVALID_TIMER;
}

void app_easy_timer_cancel_all(void)
{
    for (int i = 0; i < APP_TIMER_MAX_NUM; i++)
    {
        if ((timer_callbacks[i] != NULL) &&
            (timer_callbacks[i] != timer_canceled_handler))
            {
                app_easy_timer_cancel(APP_EASY_TIMER_IDX_TO_HND(i));
            }
    }
}

#else

const rom_app_easy_timer_cfg_t rom_app_easy_timer_cfg =
{
    .timer_callbacks          = timer_callbacks,
    .modified_timer_callbacks = modified_timer_callbacks,
    .app_check_BLE_active     = app_check_BLE_active,
    .arch_ble_force_wakeup    = arch_ble_force_wakeup,
};

#endif

#endif // (BLE_APP_PRESENT)

/// @} APP
