/**
 ****************************************************************************************
 *
 * @file rtc.c
 *
 * @brief Implementation of the Real Time Clock Low Level Driver.
 *
 * Copyright (c) 2018 Dialog Semiconductor. All rights reserved.
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

#include <stdio.h>
#include "rtc.h"
#include "compiler.h"
#include "datasheet.h"

static rtc_interrupt_cb_t   rtc_interrupt_handler   __SECTION_ZERO("retention_mem_area0");

/**
 ****************************************************************************************
 * @brief Converts time from decimal to binary-coded decimal (BCD).
 * @param[in] time Pointer to RTC/alarm time in decimal number format.
 * @return time in BCD format
 ****************************************************************************************
 */
static uint32_t time_to_bcd(const rtc_time_t *time)
{
    uint32_t time_bcd;

    time_bcd = ((time->hsec % 10) << 0);
    time_bcd += ((time->hsec / 10) << 4);

    time_bcd += ((time->sec % 10) << 8);
    time_bcd += ((time->sec / 10) << 12);

    time_bcd += ((time->minute % 10) << 16);
    time_bcd += ((time->minute / 10) << 20);

    time_bcd += ((time->hour % 10) << 24);
    time_bcd += (( time->hour / 10) << 28);

    if (time->hour_mode == RTC_HOUR_MODE_12H)
    {
        time_bcd += ((time->pm_flag) << 30);
    }

    return time_bcd;
}

/**
 ****************************************************************************************
 * @brief Converts calendar date from decimal to binary-coded decimal (BCD).
 * @param[in] clndr Pointer to RTC calendar in decimal number format.
 * @return calendar date in BCD format
 ****************************************************************************************
 */
static uint32_t calendar_to_bcd(const rtc_calendar_t *clndr)
{
    uint32_t clndr_bcd;

    clndr_bcd = clndr->wday & 0x7;

    if (clndr->month > 9)
    {
        clndr_bcd += (0x80 + ((clndr->month-10) << 3));
    }
    else
    {
        clndr_bcd += (clndr->month << 3);
    }

    clndr_bcd += ((clndr->mday % 10) << 8);
    clndr_bcd += ((clndr->mday / 10) << 12);

    clndr_bcd += (((clndr->year % 100) % 10) << 16);
    clndr_bcd += (((clndr->year % 100) / 10) << 20);

    clndr_bcd += (((clndr->year/100) % 10) << 24);
    clndr_bcd += (((clndr->year/100) / 10) << 28);

    return clndr_bcd;
}

/**
 ****************************************************************************************
 * @brief Converts alarm calendar date from decimal to binary-coded decimal (BCD).
 * @param[in] clndr Pointer to RTC alarm calendar in decimal number format.
 * @return alarm calendar date in BCD format
 ****************************************************************************************
 */
static uint32_t alarm_calendar_to_bcd(const rtc_alarm_calendar_t *clndr)
{
    uint32_t clndr_bcd;

    if (clndr->month > 9)
    {
        clndr_bcd = (0x80 + ((clndr->month-10) << 3));
    }
    else
    {
        clndr_bcd = (clndr->month << 3);
    }

    clndr_bcd += ((clndr->mday % 10) << 8);
    clndr_bcd += ((clndr->mday / 10) << 12);

    return clndr_bcd;
}

/**
 ****************************************************************************************
 * @brief Converts RTC time from binary-coded decimal (BCD) to decimal.
 * @param[in] time_bcd Time in BCD.
 * @param[out] time Pointer to RTC time in decimal number format.
 * @return void
 ****************************************************************************************
 */
static void bcd_to_time(uint32_t time_bcd, rtc_time_t *time)
{
    time->pm_flag = (time_bcd & RTC_TIME_PM) >> 30;
    time->hour = (((time_bcd & RTC_TIME_HR_T) >> 28) * 10) + ((time_bcd & RTC_TIME_HR_U) >> 24);
    time->minute = (((time_bcd & RTC_TIME_M_T) >> 20) * 10) + ((time_bcd & RTC_TIME_M_U) >> 16);
    time->sec = (((time_bcd & RTC_TIME_S_T) >> 12) * 10) + ((time_bcd & RTC_TIME_S_U) >> 8);
    time->hsec = (((time_bcd & RTC_TIME_H_T) >> 4) * 10) + (time_bcd & RTC_TIME_H_U);
}

/**
 ****************************************************************************************
 * @brief Converts calendar date from binary-coded decimal (BCD) to decimal.
 * @param[in] date_bcd Calendar date in BCD.
 * @param[out] clndr Pointer to RTC calendar in decimal number format.
 * @return void
 ****************************************************************************************
 */
static void bcd_to_clndr(uint32_t date_bcd, rtc_calendar_t *clndr)
{
    clndr->year = (((date_bcd & RTC_CAL_C_T) >> 28) * 1000) +
                  (((date_bcd & RTC_CAL_C_U) >> 24) * 100) +
                  (((date_bcd & RTC_CAL_Y_T) >> 20) * 10) +
                  ((date_bcd & RTC_CAL_Y_U) >> 16);
    clndr->mday = (((date_bcd & RTC_CAL_D_T) >> 12) * 10) + ((date_bcd & RTC_CAL_D_U) >> 8);
    clndr->month = (((date_bcd & RTC_CAL_M_T) >> 7) * 10) + ((date_bcd & RTC_CAL_M_U) >> 3);
    clndr->wday = date_bcd & RTC_DAY;
}

void rtc_init(const rtc_config_t *cfg)
{
    rtc_set_hour_clk_mode(cfg->hour_clk_mode);
    rtc_set_keep_reg_on_reset(cfg->keep_rtc);
}

void rtc_register_intr(rtc_interrupt_cb_t handler, uint8_t mask)
{
    rtc_interrupt_handler = handler;
    rtc_interrupt_enable(mask);

    // Set the priority of RTC interrupt to level 2
    NVIC_SetPriority(RTC_IRQn, 2);
    // Enable RTC interrupt
    NVIC_EnableIRQ(RTC_IRQn);
}

void rtc_unregister_intr(void)
{
    NVIC_DisableIRQ(RTC_IRQn);
    rtc_interrupt_disable(0xFF);
    NVIC_ClearPendingIRQ(RTC_IRQn);
    rtc_interrupt_handler = NULL;
}

rtc_status_code_t rtc_set_time_clndr(const rtc_time_t *time, const rtc_calendar_t *clndr)
{
    uint8_t status;
    /* Stores the current RTC time.
       If the new time value causes an entry error, this time value will be
       re-written. */
    uint32_t time_cur_val;
    /* Stores the current RTC calendar.
       If the new calendar value causes an entry error, this value will be
       re-written. */
    uint32_t clndr_cur_val;

    if ((time != NULL) && (clndr != NULL))
    {
        /* Set both time and calendar. Stop and start counters at the same time. */
        rtc_stop();
        time_cur_val = rtc_get_time_bcd();
        clndr_cur_val = rtc_get_clndr_bcd();
        rtc_set_hour_clk_mode(time->hour_mode);
        rtc_set_time_bcd(time_to_bcd(time));
        rtc_set_clndr_bcd(calendar_to_bcd(clndr));
        status = rtc_get_status();
        if ((status & (RTC_STATUS_VALID_TIME | RTC_STATUS_VALID_CAL)) == 0x0)
        {
            rtc_set_clndr_bcd(clndr_cur_val);
            rtc_set_time_bcd(time_cur_val);
            rtc_start();
            return RTC_STATUS_CODE_INVALID_TIME_CLNDR;
        }
        else if ((status & RTC_STATUS_VALID_TIME) != RTC_STATUS_VALID_TIME)
        {
            rtc_set_time_bcd(time_cur_val);
            rtc_start();
            return RTC_STATUS_CODE_INVALID_TIME;
        }
        else if ((status & RTC_STATUS_VALID_CAL) != RTC_STATUS_VALID_CAL)
        {
            rtc_set_clndr_bcd(clndr_cur_val);
            rtc_start();
            return RTC_STATUS_CODE_INVALID_CLNDR;
        }
        rtc_start();
    }
    else if (time != NULL)
    {
        rtc_time_stop();
        time_cur_val = rtc_get_time_bcd();
        rtc_set_hour_clk_mode(time->hour_mode);
        rtc_set_time_bcd(time_to_bcd(time));
        status = rtc_get_status();
        if ((status & RTC_STATUS_VALID_TIME) != RTC_STATUS_VALID_TIME)
        {
            rtc_set_time_bcd(time_cur_val);
            rtc_time_start();
            return RTC_STATUS_CODE_INVALID_TIME;
        }
        rtc_time_start();
    }
    else if (clndr != NULL)
    {
        rtc_clndr_stop();
        clndr_cur_val = rtc_get_clndr_bcd();
        rtc_set_clndr_bcd(calendar_to_bcd(clndr));
        status = rtc_get_status();
        if((status & RTC_STATUS_VALID_CAL) != RTC_STATUS_VALID_CAL)
        {
            rtc_set_clndr_bcd(clndr_cur_val);
            rtc_clndr_start();
            return RTC_STATUS_CODE_INVALID_CLNDR;
        }
        rtc_clndr_start();
    }

    return RTC_STATUS_CODE_VALID_ENTRY;
}

void rtc_get_time_clndr(rtc_time_t *time, rtc_calendar_t *clndr)
{
    if ((time != NULL) && (clndr != NULL))
    {
        uint32_t val_bcd;

        /* In order to obtain a coherent view of time and date, the counters
           shall be stopped before reading them (as the date counter might
           increase between reading the time register and reading the calendar
           register). */
        rtc_stop();
        val_bcd = rtc_get_time_bcd();
        bcd_to_time(val_bcd, time);
        time->hour_mode = rtc_get_hour_clk_mode();
        val_bcd = rtc_get_clndr_bcd();
        bcd_to_clndr(val_bcd, clndr);
        rtc_start();
    }
    else if (time != NULL)
    {
        uint32_t time_bcd;

        time_bcd = rtc_get_time_bcd();
        bcd_to_time(time_bcd, time);
        time->hour_mode = rtc_get_hour_clk_mode();
    }
    else if (clndr != NULL)
    {
        uint32_t date_bcd;

        date_bcd = rtc_get_clndr_bcd();
        bcd_to_clndr(date_bcd, clndr);
    }
}

rtc_status_code_t rtc_set_alarm(const rtc_time_t *time, const rtc_alarm_calendar_t *clndr, const uint8_t mask)
{
    uint8_t status;
    uint32_t current_val;
    uint8_t tmp_msk;

    /* Store the current value of the alarm enable register to restore it
       in case an invalid entry occurs. */
    tmp_msk = rtc_get_alarm_enable_msk();

    /* Disable alarm events. */
    rtc_interrupt_disable(RTC_INTR_ALRM);
    rtc_alarm_enable(0x0);

    if (time != NULL)
    {
        if (time->hour_mode != rtc_get_hour_clk_mode())
        {
            rtc_alarm_enable(tmp_msk);
            rtc_interrupt_enable(RTC_INTR_ALRM);
            /* Do not allow alarm with different hour clock mode from time. */
            return RTC_STATUS_CODE_INVALID_TIME_HOUR_MODE_ALM;
        }
        current_val = rtc_get_alarm_time_bcd();
        rtc_set_alarm_time_bcd(time_to_bcd(time));
        status = rtc_get_status();
        if ((status & RTC_STATUS_VALID_TIME_ALM) != RTC_STATUS_VALID_TIME_ALM)
        {
            rtc_set_alarm_time_bcd(current_val);
            rtc_alarm_enable(tmp_msk);
            rtc_interrupt_enable(RTC_INTR_ALRM);
            return RTC_STATUS_CODE_INVALID_TIME_ALM;
        }
    }

    if (clndr != NULL)
    {
        current_val = rtc_get_alarm_clndr_bcd();
        rtc_set_alarm_clndr_bcd(alarm_calendar_to_bcd(clndr));
        status = rtc_get_status();
        if ((status & RTC_STATUS_VALID_CAL_ALM) != RTC_STATUS_VALID_CAL_ALM)
        {
            rtc_set_alarm_clndr_bcd(current_val);
            rtc_alarm_enable(tmp_msk);
            rtc_interrupt_enable(RTC_INTR_ALRM);
            return RTC_STATUS_CODE_INVALID_CLNDR_ALM;
        }
    }

    rtc_alarm_enable(mask);
    rtc_interrupt_enable(RTC_INTR_ALRM);

    return RTC_STATUS_CODE_VALID_ENTRY;
}

void rtc_get_alarm(rtc_time_t *time, rtc_alarm_calendar_t *clndr, uint8_t *mask)
{
    if (time != NULL)
    {
        uint32_t time_bcd;

        time_bcd = rtc_get_alarm_time_bcd();

        time->pm_flag = (time_bcd & RTC_TIME_PM) >> 30;
        time->hour = (((time_bcd & RTC_TIME_HR_T) >> 28) * 10) + ((time_bcd & RTC_TIME_HR_U) >> 24);
        time->minute = (((time_bcd & RTC_TIME_M_T) >> 20) * 10) + ((time_bcd & RTC_TIME_M_U) >> 16);
        time->sec = (((time_bcd & RTC_TIME_S_T) >> 12) * 10) + ((time_bcd & RTC_TIME_S_U) >> 8);
        time->hsec = (((time_bcd & RTC_TIME_H_T) >> 4) * 10) + (time_bcd & RTC_TIME_H_U);
        time->hour_mode = rtc_get_hour_clk_mode();
    }

    if (clndr != NULL)
    {
        uint32_t date_bcd;

        date_bcd = rtc_get_alarm_clndr_bcd();

        clndr->mday = (((date_bcd & RTC_CAL_D_T) >> 12) * 10) + ((date_bcd & RTC_CAL_D_U) >> 8);
        clndr->month = (((date_bcd & RTC_CAL_M_T) >> 7) * 10) + ((date_bcd & RTC_CAL_M_U) >> 3);
    }

    if (mask != NULL)
    {
        *mask = rtc_get_alarm_enable_msk();
    }

}

#if !defined (__EXCLUDE_RTC_HANDLER__)
void RTC_Handler(void)
{
    if (rtc_interrupt_handler != NULL)
    {
        uint8_t event;
        event = rtc_get_event_flags();
        rtc_interrupt_handler(event);
    }
}
#endif
