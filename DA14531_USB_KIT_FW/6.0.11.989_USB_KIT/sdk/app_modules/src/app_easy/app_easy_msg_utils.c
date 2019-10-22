/**
 ****************************************************************************************
 *
 * @file app_easy_msg_utils.c
 *
 * @brief Message related helper function.
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

#include "rwip_config.h"

#if (BLE_APP_PRESENT)
#include "ke_msg.h"
#include "app.h"
#include "app_msg_utils.h"
#include "app_entry_point.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define APP_MSG_UTIL_MAX_NUM    (APP_MSG_UTIL_API_LAST_MES - APP_MSG_UTIL_API_MES0 + 1)

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

typedef void (*void_msg_void)(void);

// Array holding the callbacks for the pool of messages
static void_msg_void msg_util_callbacks[APP_MSG_UTIL_MAX_NUM] __SECTION_ZERO("retention_mem_area0");

// The message id used by the easy wakeup functions
static ke_msg_id_t wakeup_id                                  __SECTION_ZERO("retention_mem_area0");

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Find the array index for a given message id.
 * @param[in] msg_util_id The message id
 * @return Returns the index in the callback array
 ****************************************************************************************
 */
static int8_t app_easy_msg_find_idx(ke_msg_id_t msg_util_id)
{
    int i;
    i = msg_util_id - APP_MSG_UTIL_API_MES0;
    if ((i >= 0) && (i < APP_MSG_UTIL_MAX_NUM))
    {
        return i;
    }
    return -1;
}

/**
 ****************************************************************************************
 * @brief Find the first free entry in the array of callbacks and populate it
 *        with a callback.
 * @param[in] fn The pointer to the callback function
 * @return Returns the message id hooked with the callback and zero if no free entry exists
 ****************************************************************************************
 */
static ke_msg_id_t app_easy_msg_set_callback(void (*fn)(void))
{
    int i;
    for (i = 0; i < APP_MSG_UTIL_MAX_NUM; i++)
    {
        if (msg_util_callbacks[i] == NULL)
        {
            msg_util_callbacks[i] = fn;
            return (APP_MSG_UTIL_API_MES0 + i);
        }
    }
    return 0;
}

 /**
 ****************************************************************************************
 * @brief Call the callback hooked to a given message.
 * @param[in] msg_util_id The message id
 * @return void
 ****************************************************************************************
 */
static void app_easy_msg_call_callback(ke_msg_id_t msg_util_id)
{
    int i;
    i = app_easy_msg_find_idx(msg_util_id);
    if (i >= 0)
    {
        if (msg_util_callbacks[i] != NULL)
        {
            void (*callback)(void) = msg_util_callbacks[i];
            msg_util_callbacks[i] = NULL;
            callback();
        }
    }
    else
    {
        ASSERT_WARNING(0);
    }
}

ke_msg_id_t app_easy_msg_free_callback(ke_msg_id_t msg_util_id)
{
    int i;
    i = app_easy_msg_find_idx(msg_util_id);
    if (i >= 0)
    {
        msg_util_callbacks[i] = NULL;
        return msg_util_id;
    }
    else
    {
        ASSERT_WARNING(0);
    }
    return 0;
}

ke_msg_id_t app_easy_msg_set(void (*fn)(void))
{
    ke_msg_id_t msg_id = app_easy_msg_set_callback(fn);
    if (msg_id == 0)
    {
        return 0xFFFF; // No available message
    }
    return msg_id;
}

ke_msg_id_t app_easy_msg_modify(ke_msg_id_t msg_util_id, void (*fn)(void))
{
    int i;
    i = app_easy_msg_find_idx(msg_util_id);
    if (i >= 0)
    {
        msg_util_callbacks[i] = fn;
        return msg_util_id;
    }
    else
    {
        ASSERT_WARNING(0);
    }
    return 0xFFFF;
}

void app_easy_wakeup_set(void (*fn)(void))
{
    if (wakeup_id == 0)
    {
        wakeup_id = app_easy_msg_set(fn);
    }
    else
    {
        app_easy_msg_modify(wakeup_id, fn);
    }
}

void app_easy_wakeup_free(void)
{
    app_easy_msg_free_callback(wakeup_id);
    wakeup_id = 0;
}

void app_easy_wakeup(void)
{
    if (wakeup_id != 0)
    {
        ke_msg_send_basic(wakeup_id, TASK_APP, 0);
    }
}

enum process_event_response app_msg_utils_api_process_handler(ke_msg_id_t const msgid,
                                                              void const *param,
                                                              ke_task_id_t const dest_id,
                                                              ke_task_id_t const src_id,
                                                              enum ke_msg_status_tag *msg_ret)
{
    if ((msgid < APP_MSG_UTIL_API_MES0) || (msgid > APP_MSG_UTIL_API_LAST_MES))
    {
        return PR_EVENT_UNHANDLED;
    }
    else
    {
        app_easy_msg_call_callback(msgid);
        *msg_ret = KE_MSG_CONSUMED;
        return PR_EVENT_HANDLED;
    }
}

#endif // (BLE_APP_PRESENT)

/// @} APP
