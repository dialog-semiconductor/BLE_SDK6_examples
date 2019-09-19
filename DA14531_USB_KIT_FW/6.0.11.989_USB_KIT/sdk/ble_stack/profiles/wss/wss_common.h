/**
 ****************************************************************************************
 *
 * @file wss_common.h
 *
 * @brief Header file - Weight Scale Service common types.
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

#ifndef WSS_COMMON_H_
#define WSS_COMMON_H_
/**
 ************************************************************************************
 * @addtogroup WSSS Weight Scale Service Collector
 * @ingroup WSS
 * @brief Weight Scale Service Collector
 * @{
 ************************************************************************************
 */

/*
 * INCLUDE FILES
 ************************************************************************************
 */

#if (BLE_WSS_SERVER || BLE_WSS_COLLECTOR)

#include "prf_types.h"


/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Weight Measurement Flags field bit values
enum
{
    /// Imperial Measurement Units (weight in lb and height in inches)
    WSS_MEAS_FLAG_UNIT_IMPERIAL      = 0x01,
    /// Time Stamp present
    WSS_MEAS_FLAG_TIME_STAMP         = 0x02,
    /// User ID present
    WSS_MEAS_FLAG_USERID_PRESENT     = 0x04,
    /// BMI & Height present
    WSS_MEAS_FLAG_BMI_HT_PRESENT     = 0x08,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Weight measurement structure - shall be dynamically allocated
struct wss_wt_meas
{
    /// Flags
    uint8_t flags;
    /// Weight
    float weight;
    /// Time stamp
    struct prf_date_time datetime;
    /// User ID
    uint8_t userid;
    /// BMI
    float bmi;
    /// Height
    float height;
};

#endif
/// @} wss_common

#endif /* WSS_COMMON_H_ */
