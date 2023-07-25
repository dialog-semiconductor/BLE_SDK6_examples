/**
 ****************************************************************************************
 *
 * @file user_rtc_util.h
 *
 * @brief User code for RTC utilities header file.
 *
 * Copyright (c) 2012-2021 Dialog Semiconductor
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */

#ifndef _USER_RTC_UTIL_H_
#define _USER_RTC_UTIL_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
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
 
 /*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
 
 /*
 * DEFINES
 ****************************************************************************************
 */
 
 /*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
 
/**
 ****************************************************************************************
 * @brief Configures the clock that the RTC will operate with
 * @details This function will check if the current lp clock (RCX or XTAL) and
 *          will configure the divider accordingly.
 ****************************************************************************************
 */
void rtc_clock_set(void);
     
/**
 ****************************************************************************************
 * @brief Configures the time and the clock that RTC will operate with
 * @details Sets the starting time and calendar for the RTC and sets the clock,
 *          user should invoke rtc_time_start(); for the RTC to start counting
 * @param[in] time the starting time of the RTC
 * @param[in] cal the starting date of the RTC
 * @param[in] config am/pm configuration and retain RTC registers on a sw reset
 * @return rtc_status_code_t invalid or valid settings
 ****************************************************************************************
 */
rtc_status_code_t rtc_configure(rtc_time_t *time, rtc_calendar_t *cal, rtc_config_t *config);
 
/**
 ****************************************************************************************
 * @brief Converts time from rtc_time_t format to milliseconds starting from 00:00
 * @param[in] rtc_time_t time to be converted
 * @return uint32_t time in milliseconds from 00:00
 ****************************************************************************************
 */
uint32_t rtc_convert_time_to_msec(rtc_time_t *time);

/**
 ****************************************************************************************
 * @brief Get the enabled interrupts of the RTC
 * @return uint8_t enabled interrupts
 ****************************************************************************************
 */
uint8_t user_get_rtc_interrupt_enabled(void);

/**
 ****************************************************************************************
 * @brief Configures the interrupt for an RTC event and registers an interrupt handler.
 * @param[in] rtc_interrupt_cb_t handler to triggered upon interrupt
 * @param[in] uint8_t event that will trigger the interrupt
 * @return void
 ****************************************************************************************
 */
void user_rtc_register_intr(rtc_interrupt_cb_t handler, uint8_t mask);

/**
 ****************************************************************************************
 * @brief Unregisters a specific event from triggering an interrupt if no events are to 
 * trigger and interrupt the RTC interrupt is disabled the the interrupt handler is set
 * to NULL 
 * @param[in] uint8_t interrupt to be disabled
 * @return interrupts enabled
 ****************************************************************************************
 */
uint8_t user_rtc_unregister_intr(uint8_t mask);

#endif // _USER_RTC_UTIL_H_
