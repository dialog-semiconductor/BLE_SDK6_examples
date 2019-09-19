/**
 ****************************************************************************************
 *
 * @file cts_common.h
 *
 * @brief Header file - Current Time Service common types.
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
 ****************************************************************************************
 */

#ifndef _CTS_COMMON_H_
#define _CTS_COMMON_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_CTS_CLIENT || BLE_CTS_SERVER)

#include "prf_types.h"

/**
 ****************************************************************************************
 * @addtogroup CTS Current Time Service
 * @ingroup CTS
 * @brief Current Time Service
 * @{
 ****************************************************************************************
 */

/// Day of Week Characteristic - UUID:0x2A09
typedef uint8_t cts_day_of_week;

/// Day Date Time Characteristic Structure - UUID:0x2A0A
struct cts_day_date_time
{
    struct prf_date_time date_time;
    cts_day_of_week day_of_week;
};

/// Exact Time 256 Characteristic Structure - UUID:0x2A0C
struct cts_exact_time_256
{
    struct cts_day_date_time day_date_time;
    /// 1/256th of a second
    uint8_t fraction_256;
};

/// Adjust Reason Flags field bit values
enum
{
    CTSS_REASON_FLAG_MAN_TIME_UPDATE = 0x01,
    CTSS_REASON_FLAG_EXT_TIME_UPDATE = 0x02,
    CTSS_REASON_FLAG_CHG_TIME_ZONE = 0x04,
    CTSS_REASON_FLAG_DST_CHANGE = 0x08,
};

/// Current Time Characteristic Structure - UUID:0x2A2B
struct cts_curr_time
{
    struct cts_exact_time_256 exact_time_256;
    uint8_t adjust_reason;
};

/**
 *
 * Time Zone Characteristic - UUID:0x2A0E
 * Min value : -48 (UTC-12:00), Max value : 56 (UTC+14:00)
 * -128 : Time zone offset is not known
 */
typedef int8_t cts_time_zone;

/**
 * DST Offset Characteristic - UUID:0x2A2D
 * Min value : 0, Max value : 8
 * 255 = DST is not known
 */
typedef uint8_t cts_dst_offset;

/// Local Time Info Characteristic Structure - UUID:0x2A0F
struct cts_loc_time_info
{
    cts_time_zone time_zone;
    cts_dst_offset dst_offset;
};

/**
 * Time Source Characteristic - UUID:0x2A13
 * Min value : 0, Max value : 6
 * 0 = Unknown
 * 1 = Network Time Protocol
 * 2 = GPS
 * 3 = Radio Time Signal
 * 4 = Manual
 * 5 = Atomic Clock
 * 6 = Cellular Network
 */
typedef uint8_t cts_time_source;

/**
 * Time Accuracy Characteristic - UUID:0x2A12
 * Accuracy (drift) of time information in steps of 1/8 of a second (125ms) compared
 * to a reference time source. Valid range from 0 to 253 (0s to 31.5s). A value of
 * 254 means Accuracy is out of range (> 31.5s). A value of 255 means Accuracy is
 * unknown.
 */
typedef uint8_t cts_time_accuracy;

/// Reference Time Info Characteristic Structure - UUID:0x2A14
struct cts_ref_time_info
{
    cts_time_source time_source;
    cts_time_accuracy time_accuracy;
    /**
     * Days since last update about Reference Source
     * Min value : 0, Max value : 254
     * 255 = 255 or more days
     */
    uint8_t days_update;
    /**
     * Hours since update about Reference Source
     * Min value : 0, Mac value : 23
     * 255 = 255 or more days (If Days Since Update = 255, then Hours Since Update shall
     * also be set to 255)
     */
    uint8_t hours_update;
};

#endif /* #if (BLE_CTS_CLIENT || BLE_CTS_SERVER) */

/// @} cts_common

#endif /* _CTS_COMMON_H_ */
