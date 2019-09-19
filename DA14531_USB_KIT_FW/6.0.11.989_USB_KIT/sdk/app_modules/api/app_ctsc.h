/**
 ****************************************************************************************
 *
 * @file app_ctsc.h
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

#ifndef _APP_CTSC_H_
#define _APP_CTSC_H_

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

#if (BLE_CTS_CLIENT)

#include "cts_common.h"
#include "ctsc.h"

/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */

struct app_ctsc_cb
{
    void (*on_connect)(uint8_t conidx, const struct ctsc_cts_content *cts);
    void (*on_ct_read)(uint8_t conidx, uint8_t status, const struct cts_curr_time *ct, bool notification);
    void (*on_rti_read)(uint8_t conidx, uint8_t status, const struct cts_ref_time_info *ct);
    void (*on_lti_read)(uint8_t conidx, uint8_t status, const struct cts_loc_time_info *lt);
    void (*on_desc_read)(uint8_t conidx, uint8_t status, uint16_t value);
    void (*on_ct_write)(uint8_t conidx, uint8_t status);
    void (*on_lti_write)(uint8_t conidx, uint8_t status);
    void (*on_desc_write)(uint8_t conidx, uint8_t status);
};

/**
 ****************************************************************************************
 * @brief Initialize Current Time Service Application
 ****************************************************************************************
 */
void app_ctsc_init(void);

/**
 ****************************************************************************************
 * @brief Add a Current Time Service instance in the DB
 ****************************************************************************************
 */
void app_ctsc_create_task(void);

/**
 ****************************************************************************************
 * @brief Enable Current Time Service client profile.
 * @param[in] conidx            The connection id number
 * @return void
 ****************************************************************************************
 */
void app_ctsc_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Read current time from peer
 ****************************************************************************************
 */
void app_ctsc_read_ct(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Read Local Time information from peer
 ****************************************************************************************
 */
void app_ctsc_read_lti(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Read Reference Time information from peer
 ****************************************************************************************
 */
void app_ctsc_read_rti(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Read Current Time conjuration descriptor from peer
 ****************************************************************************************
 */
void app_ctsc_read_desc(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Write current time to peer
 ****************************************************************************************
 */
void app_ctsc_write_ct(uint8_t conidx, const struct cts_curr_time *ct);

/**
 ****************************************************************************************
 * @brief Write local time information to peer
 ****************************************************************************************
 */
void app_ctsc_write_lti(uint8_t conidx, const struct cts_loc_time_info *lti);

/**
 ****************************************************************************************
 * @brief Write Current Time configuration descriptor to peer
 ****************************************************************************************
 */
void app_ctsc_write_desc(uint8_t conidx, uint16_t value);

#endif // (BLE_CTS_CLIENT)

/// @} APP

#endif // _APP_CTSC_H_
