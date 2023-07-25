/**
 ****************************************************************************************
 *
 * @file user_ctss.h
 *
 * @brief Header file - User Current Time Service Server.
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
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

#ifndef _USER_CTSS_H_
#define _USER_CTSS_H_

/**
 ****************************************************************************************
 * @addtogroup CTSS Current Time Service Server
 * @ingroup CTS
 * @brief Current Time Service Server
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_CTS_SERVER)

/*
 * DEFINES
 ****************************************************************************************
 */
 /// Time Zones
#define UTC_MINUS1200               -48
#define UTC_PLUS1400                56
#define UTC_UNKNOWN                 -128

/// DST offset
#define STANDARD_TIME               0
#define HALF_HOUR_DAY_TIME          2
#define DAYLIGHT_TIME               4
#define DOUBLE_DAYLIGHT_TIME        8
#define DST_OFFSET_UNK              255

/// Time Source Characteristic
#define TIME_SOURCE_UNKNOWN         0
#define TIME_SOURCE_NET_PROTOCOL    1
#define TIME_SOURCE_GPS             2
#define TIME_SOURCE_RADIO_SIGNAL    3
#define TIME_SOURCE_MANUAL          4
#define TIME_SOURCE_ATOMIC_CLOCK    5
#define TIME_SOURCE_CELL_NET        6

#define ACCURACY_UNKNOWN            255
#define LAST_UPDATE_MAX             255

/*
 * ENUMERATIONS
 ****************************************************************************************
 */
 
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the Current Time Service profile.
   @pararm[out] void
 * @return void
 ****************************************************************************************
*/
void user_ctss_init(void);

/**
 ****************************************************************************************
 * @brief Apply an external change to the Server's current time and date.
 * @param[in]   rtc_time_t time that the RTC should change to
 * @param[in]   rtc_calendar_t date that the RTC should change to
 * @param[in]   struct cts_loc_time_info update the time zone or the destination offset
 * @param[out]  cts_loc_time_info external/manual change of Time zone or destination offset 
 * @return void
 ****************************************************************************************
*/
void ext_cts_adjust_time_date(rtc_time_t *rtc_chg_time, rtc_calendar_t *rtc_chg_clndr, struct cts_loc_time_info time_zone_dst_off);

/**
 ****************************************************************************************
 * @brief Write Destination offset and Time Zone callback fucntion.
   @pararm[in] lt Pointer to received Destination offset and Time Zone value.
 * @return void
 ****************************************************************************************
*/
void user_on_loc_time_info_write_req(const struct cts_loc_time_info *lt);

/**
 ****************************************************************************************
 * @brief Write current time callback fucntion.
   @pararm[in] ct Pionter to new current time value.
 * @return status
 ****************************************************************************************
*/
uint8_t user_on_cur_time_write_req(const struct cts_curr_time *ct);

/**
 ****************************************************************************************
 * @brief Read Destination offset and Time Zone callback fucntion.
   @pararm[in] lt Pointer current Destination offset and Time Zone value.
 * @return void
 ****************************************************************************************
*/
void user_on_loc_time_info_read_req(struct cts_loc_time_info *lt);

/**
 ****************************************************************************************
 * @brief Current time read callback function.
   @pararm[out] ct Pionter to output structure.
 * @return void
 ****************************************************************************************
*/
void user_on_current_time_read(struct cts_curr_time *ct);
 
/**
 ****************************************************************************************
 * @brief Reference time read callback function.
   @pararm[out] rt Pionter to output structure.
 * @return void
 ****************************************************************************************
*/
void user_on_ref_time_info_read(struct cts_ref_time_info *rt);

/**
 ****************************************************************************************
 * @brief Callback triggered to update the reference Time Information fields for last hour
 * and day update.
 ****************************************************************************************
*/
void update_ref_time_info(void);

#endif /* #if (BLE_CTS_SERVER) */

/// @} CTSS

#endif /* _USER_CTSS_H_ */
