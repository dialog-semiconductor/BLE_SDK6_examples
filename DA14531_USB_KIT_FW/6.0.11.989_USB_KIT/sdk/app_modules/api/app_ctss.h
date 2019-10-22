/**
 ****************************************************************************************
 *
 * @file app_ctss.h
 *
 * @brief Current Time Service Application entry point.
 *
 * Copyright (c) 2017-2018 Dialog Semiconductor. All rights reserved.
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
 *
 ****************************************************************************************
 */

#ifndef _APP_CTSS_H_
#define _APP_CTSS_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief Current Time Service Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_CTS_SERVER)

#include "cts_common.h"

/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */

struct app_ctss_cb
{
    void (*on_cur_time_read_req)(struct cts_curr_time *ct);
    uint8_t (*on_cur_time_write_req)(const struct cts_curr_time *ct);
    void (*on_cur_time_notified)(uint8_t status);
    void (*on_loc_time_info_write_req)(const struct cts_loc_time_info *lt);
    void (*on_ref_time_info_read_req)(struct cts_ref_time_info *rt);
};

/**
 ****************************************************************************************
 * @brief Initialize Current Time Service Application
 ****************************************************************************************
 */
void app_ctss_init(void);

/**
 ****************************************************************************************
 * @brief Add a Current Time Service instance in the DB
 ****************************************************************************************
 */
void app_ctss_create_db(void);

/**
 ****************************************************************************************
 * @brief Notify Current Time Service clients
 ****************************************************************************************
 */
void app_ctss_notify_current_time(const struct cts_curr_time *curr_time);

#endif // (BLE_CTS_SERVER)

/// @} APP

#endif // _APP_CTSS_H_
