/**
 ****************************************************************************************
 *
 * @file user_ctss.c
 *
 * @brief User implementation of the CTS Server profile.
 *
 * Copyright (C) 2012-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
 
 #if BLE_CTS_SERVER
 
 /*
 * INCLUDE FILES
 ****************************************************************************************
 */
 #include "user_ctss.h"
 
 
struct cts_curr_time cts_current_time                   __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
struct cts_loc_time_info cts_current_dst_tz             __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
struct cts_ref_time_info ref_time                       __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
 
 /*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief Maps the rtc's time and calendar values to the cts structure.
 * @param[in] rtc_time_t time aquired from the RTC.
 * @param[in] rtc_calendar_t calendar aquired from the RTC
 * @param[out] cts_curr_time cts structure holding time and date
 * @return void
 ****************************************************************************************
*/
static void rtc_to_cts_date_time_struct(rtc_time_t *rtc_time, rtc_calendar_t *rtc_clndr, struct cts_curr_time *cts_time)
{
    cts_time->exact_time_256.fraction_256 = (rtc_time->hsec << 8) / 100;
    cts_time->exact_time_256.day_date_time.day_of_week = rtc_clndr->wday;
    cts_time->exact_time_256.day_date_time.date_time.year = rtc_clndr->year;
    cts_time->exact_time_256.day_date_time.date_time.month = rtc_clndr->month;
    cts_time->exact_time_256.day_date_time.date_time.day = rtc_clndr->mday;
    cts_time->exact_time_256.day_date_time.date_time.hour = rtc_time->hour;
    cts_time->exact_time_256.day_date_time.date_time.min = rtc_time->minute;
    cts_time->exact_time_256.day_date_time.date_time.sec = rtc_time->sec;
}

void update_ref_time_info(void)
{
    ref_time.hours_update++;
    
    if(ref_time.hours_update == 23)
    {
        ref_time.days_update++;
        ref_time.hours_update = 0;
    }
    
    if((ref_time.days_update == LAST_UPDATE_MAX - 1) && (ref_time.hours_update == 23))
    {
        ref_time.days_update = LAST_UPDATE_MAX;
        ref_time.hours_update = LAST_UPDATE_MAX;
    }
}

void user_ctss_init(void)
{
    struct prf_date_time date_time = {
        user_calendar_conf.year, 
        user_calendar_conf.month, 
        user_calendar_conf.mday, 
        user_time_conf.hour, 
        user_time_conf.minute, 
        user_time_conf.sec
    };
    
    cts_current_time.adjust_reason = CTSS_REASON_FLAG_MAN_TIME_UPDATE;
    cts_current_time.exact_time_256.day_date_time.date_time = date_time;
    cts_current_time.exact_time_256.day_date_time.day_of_week = user_calendar_conf.wday;
    cts_current_time.exact_time_256.fraction_256 = 0;

    cts_current_dst_tz.time_zone = CTS_DEFAULT_TIME_ZONE;
    cts_current_dst_tz.dst_offset = CTS_DEFAULT_DST_OFFSET;
    
    ref_time.time_source = TIME_SOURCE_MANUAL;
    ref_time.time_accuracy = ACCURACY_UNKNOWN;
    ref_time.days_update = 0;
    ref_time.hours_update = 0;
}

void ext_cts_adjust_time_date(rtc_time_t *rtc_chg_time, rtc_calendar_t *rtc_chg_clndr, struct cts_loc_time_info time_zone_dst_off)
{
    rtc_status_code_t status = rtc_set_time_clndr(rtc_chg_time, rtc_chg_clndr);
    
    if (status != RTC_STATUS_CODE_VALID_ENTRY)
    {
        cts_current_time.adjust_reason = CTSS_REASON_FLAG_EXT_TIME_UPDATE;
        // Convert from RTC date and time format to CTS date and time format
        rtc_to_cts_date_time_struct(rtc_chg_time, rtc_chg_clndr, &cts_current_time);
        
        ref_time.days_update = 0;
        ref_time.hours_update = 0;
        
        if(time_zone_dst_off.dst_offset != cts_current_dst_tz.dst_offset)
        {
            cts_current_time.adjust_reason |= CTSS_REASON_FLAG_DST_CHANGE;
            cts_current_dst_tz.dst_offset = time_zone_dst_off.dst_offset;
        }
        
        if(time_zone_dst_off.time_zone != cts_current_dst_tz.time_zone)
        {
            cts_current_time.adjust_reason |= CTSS_REASON_FLAG_CHG_TIME_ZONE;
            cts_current_dst_tz.time_zone = time_zone_dst_off.time_zone;
        }
        
        app_ctss_notify_current_time(&cts_current_time);
    }
}

/* User application Handler */

void user_on_loc_time_info_write_req(const struct cts_loc_time_info *lt)
{       
    cts_current_dst_tz.dst_offset = (lt->dst_offset == STANDARD_TIME || 
                                    lt->dst_offset == HALF_HOUR_DAY_TIME || 
                                    lt->dst_offset == DAYLIGHT_TIME || 
                                    lt->dst_offset == DOUBLE_DAYLIGHT_TIME) ? lt->dst_offset : DST_OFFSET_UNK;
    cts_current_dst_tz.time_zone = (lt->time_zone >= UTC_MINUS1200 && lt->time_zone <= UTC_PLUS1400) ? lt->time_zone : UTC_UNKNOWN;
}

void user_on_loc_time_info_read_req(struct cts_loc_time_info *lt)
{
    *lt = cts_current_dst_tz;
}

void user_on_current_time_read(struct cts_curr_time *ct)
{
    rtc_time_t rtc_current_time;
    rtc_calendar_t rtc_current_date;    
    rtc_get_time_clndr(&rtc_current_time, &rtc_current_date);
    
    // Update the struct for the CTS profile with the data from the RTC
    rtc_to_cts_date_time_struct(&rtc_current_time, &rtc_current_date, &cts_current_time);
    
    *ct = cts_current_time;
}

uint8_t user_on_cur_time_write_req(const struct cts_curr_time *ct)
{
    // Check the alarms when the time is updated by the accumulation value
     
    rtc_time_t rtc_current_time;
    rtc_calendar_t rtc_current_date;
    
    rtc_current_date.wday   = ct->exact_time_256.day_date_time.day_of_week;
    rtc_current_date.year   = ct->exact_time_256.day_date_time.date_time.year;
    rtc_current_date.month  = ct->exact_time_256.day_date_time.date_time.month;
    rtc_current_date.mday   = ct->exact_time_256.day_date_time.date_time.day;
    rtc_current_time.hour   = ct->exact_time_256.day_date_time.date_time.hour;
    rtc_current_time.minute = ct->exact_time_256.day_date_time.date_time.min;
    rtc_current_time.sec    = ct->exact_time_256.day_date_time.date_time.sec;
    // First have the RTC driver to check if the date/time entry is valid
    rtc_status_code_t status = rtc_set_time_clndr(&rtc_current_time, &rtc_current_date);
    
    if(status != RTC_STATUS_CODE_VALID_ENTRY)
    {
        return ATT_ERR_APP_ERROR;
    }
    else
    {
        cts_current_time = *ct;     // Update the current time of the profile
        return ATT_ERR_NO_ERROR;
    }
}

void user_on_ref_time_info_read(struct cts_ref_time_info *rt)
{
    *rt = ref_time;
}

#endif // BLE_CTS_SERVER
