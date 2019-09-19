/**
 ****************************************************************************************
 *
 * @file rtc.h
 *
 * @brief Definition of API for the Real Time Clock Low Level Driver.
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

#ifndef _RTC_H_
#define _RTC_H_

#include <stdbool.h>
#include <stdint.h>
#include "datasheet.h"

/// Hour clock mode
typedef enum
{
    /// 24-hour clock format
    RTC_HOUR_MODE_24H = 0x0,

    /// 12-hour clock format
    RTC_HOUR_MODE_12H = 0x1
} rtc_hour_mode_t;

/// RTC denominator for the fractional division of the source clock
typedef enum
{
    /// Denominator for the fractional division is 1000
    RTC_DIV_DENOM_1000 = 0x0,

    /// Denominator for the fractional division is 1024
    RTC_DIV_DENOM_1024 = 0x1
} rtc_div_denom_t;

/**
 ****************************************************************************************
 * @brief RTC interrupt source
 * @note Can be used as bitmask.
 * @sa rtc_interrupt_enable
 * @sa rtc_interrupt_disable
 * @sa rtc_get_interrupt_mask
 ****************************************************************************************
 */
typedef enum
{
    /// Interrupt when alarm event occurs
    RTC_INTR_ALRM  = RTC_ALRM_INT_EN,

    /// Interrupt when month event occurs
    RTC_INTR_MONTH = RTC_MNTH_INT_EN,

    /// Interrupt when day of the month event occurs
    RTC_INTR_MDAY  = RTC_DATE_INT_EN,

    /// Interrupt when hour event occurs
    RTC_INTR_HOUR  = RTC_HOUR_INT_EN,

    /// Interrupt when minute event occurs
    RTC_INTR_MIN   = RTC_MIN_INT_EN,

    /// Interrupt when seconds event occurs
    RTC_INTR_SEC   = RTC_SEC_INT_EN,

    /// Interrupt when hundredth of a second event occurs
    RTC_INTR_HSEC  = RTC_HOS_INT_EN
} rtc_intr_t;

/**
 ****************************************************************************************
 * @brief RTC Events
 * @note Can be used as bitmask.
 * @sa rtc_get_event_flags
 ****************************************************************************************
 */
typedef enum
{
    /// Alarm event
    RTC_EVENT_ALARM  = RTC_EVENT_ALRM,

    /// Month rolls over event
    RTC_EVENT_MONTH  = RTC_EVENT_MNTH,

    /// Day of the month rolls over event
    RTC_EVENT_MDAY   = RTC_EVENT_DATE,

    /// Hour rolls over event
    RTC_EVENT_HR     = RTC_EVENT_HOUR,

    /// Minute rolls over event
    RTC_EVENT_MINUTE = RTC_EVENT_MIN,

    /// Second rolls over event
    RTC_EVENT_SECOND = RTC_EVENT_SEC,

    /// Hundredths of a second rolls over event
    RTC_EVENT_HSEC   = RTC_EVENT_HOS
} rtc_event_t;

/**
 ****************************************************************************************
 * @brief RTC Alarm enable mask
 * @note Can be used as bitmask.
 * @sa rtc_get_alarm_enable_msk
 * @sa rtc_set_alarm
 * @sa rtc_get_alarm
 ****************************************************************************************
 */
typedef enum
{
    /// Enable to trigger alarm when the month in Calendar Alarm Register has been reached
    RTC_ALARM_EN_MONTH = RTC_ALARM_MNTH_EN,

    /// Enable to trigger alarm when the month day in Calendar Alarm Register has been reached
    RTC_ALARM_EN_MDAY  = RTC_ALARM_DATE_EN,

    /// Enable to trigger alarm when the hour in Time Alarm Register has been reached
    RTC_ALARM_EN_HOUR  = RTC_ALARM_HOUR_EN,

    /// Enable to trigger alarm when the minute in Time Alarm Register has been reached
    RTC_ALARM_EN_MIN   = RTC_ALARM_MIN_EN,

    /// Enable to trigger alarm when the sec in Time Alarm Register has been reached
    RTC_ALARM_EN_SEC   = RTC_ALARM_SEC_EN,

    /// Enable to trigger alarm when the hsec in Time Alarm Register has been reached
    RTC_ALARM_EN_HSEC  = RTC_ALARM_HOS_EN
} rtc_alarm_en_t;

/**
 ****************************************************************************************
 * @brief RTC status
 * @note Can be used as bitmask.
 * @sa rtc_get_status
 ****************************************************************************************
 */
typedef enum
{
    /// Valid Calendar Alarm. If cleared then indicates that invalid entry occurred
    /// when writing to Calendar Alarm Register.
    RTC_STATUS_VALID_CAL_ALM  = RTC_VALID_CAL_ALM,

    /// Valid Time Alarm. If cleared then indicates that invalid entry occurred
    /// when writing to Time Alarm Register.
    RTC_STATUS_VALID_TIME_ALM = RTC_VALID_TIME_ALM,

    /// Valid Calendar. If cleared then indicates that invalid entry occurred
    /// when writing to Calendar Register.
    RTC_STATUS_VALID_CAL      = RTC_VALID_CAL,

    /// Valid Time. If cleared then indicates that invalid entry occurred
    /// when writing to Time Register.
    RTC_STATUS_VALID_TIME     = RTC_VALID_TIME
} rtc_status_valid_t;

/**
 ****************************************************************************************
 * @brief RTC status register code
 * @sa rtc_set_time_clndr
 * @sa rtc_set_alarm
 ****************************************************************************************
 */
typedef enum
{
    /// Indicates a valid entry
    RTC_STATUS_CODE_VALID_ENTRY                = 0x1,

    /// Indicates that invalid entry occurred when writing to Time Alarm Register hour clock mode.
    /// For example, when time is in 24h mode, alarm can not be set in 12h mode.
    /// Note that this is not flagged in the status register.
    RTC_STATUS_CODE_INVALID_TIME_HOUR_MODE_ALM = 0x3,

    /// Indicates that invalid entry occurred when writing to Calendar Alarm Register.
    RTC_STATUS_CODE_INVALID_CLNDR_ALM          = 0x7,

    /// Indicates that invalid entry occurred when writing to Time Alarm Register.
    RTC_STATUS_CODE_INVALID_TIME_ALM           = 0xB,

    /// Indicates that invalid entry occurred when writing to Calendar Register
    RTC_STATUS_CODE_INVALID_CLNDR              = 0xD,

    /// Indicates that invalid entry occurred when writing to Time Register 
    RTC_STATUS_CODE_INVALID_TIME               = 0xE,

    /// Indicates that invalid entry occurred to both time and calendar registers
    RTC_STATUS_CODE_INVALID_TIME_CLNDR         = 0xC
} rtc_status_code_t;

/**
 ****************************************************************************************
 * @brief Callback that is fired on RTC events
 * @param[in] event bitmask of rtc_event_t
 * @return void
 ****************************************************************************************
 */
typedef void (*rtc_interrupt_cb_t)(uint8_t event);

/// Calendar configuration for RTC
typedef struct
{
    /// Year from 1900 to 2999
    uint16_t year;

    /// Month from 1 to 12
    uint8_t month;

    /// Day of month from 1 to 28, 29, 30 or 31 (as a function of month and year)
    uint8_t mday;

    /// Day of week from 1 to 7 (mapping is programmable)
    uint8_t wday;
} rtc_calendar_t;

/// Alarm Calendar configuration for RTC
typedef struct
{
    /// Month from 1 to 12
    uint8_t month;

    /// Day of month from 1 to 28, 29, 30 or 31 (as a function of month and year)
    uint8_t mday;
} rtc_alarm_calendar_t;

/// Time configuration for RTC
typedef struct
{
    /// Hour mode: "0" for 24-hour clock or "1" for 12-hour clock
    rtc_hour_mode_t     hour_mode;

    /// In 12 hour clock mode, indicates PM when set to "1"
    bool                pm_flag;

    /// Hour from 0 to 23 in 24-hour mode, or from 1 to 12 in 12-hour mode
    uint8_t             hour;

    /// Minutes after the hour, in the range 0 to 59
    uint8_t             minute;

    /// Seconds after the minute, in the range 0 to 59
    uint8_t             sec;

    /// Hundredth of the second, in the range 0 to 99
    uint8_t             hsec;
} rtc_time_t;

/// RTC configuration
typedef struct
{
    /// Hour clock mode
    rtc_hour_mode_t     hour_clk_mode;

    /// When true, the time and calendar values are NOT reset when software reset is applied
    bool                keep_rtc;
} rtc_config_t;

/**
 ****************************************************************************************
 * @brief RTC initialization
 * @note After initialization RTC is not running. Start RTC by calling rtc_start().
 * @param[in] cfg configuration
 * @return void
 ****************************************************************************************
 */
void rtc_init(const rtc_config_t *cfg);

/**
 ****************************************************************************************
 * @brief Registers an interrupt handler.
 * @param[in] handler callback function to be called when RTC event occurs
 * @param[in] mask initial bitmask of requested interrupt events
 * @return void
 * @sa rtc_interrupt_enable
 * @sa rtc_interrupt_disable
 ****************************************************************************************
 */
void rtc_register_intr(rtc_interrupt_cb_t handler, uint8_t mask);

/**
 ****************************************************************************************
 * @brief Unregisters the event handler and disables RTC interrupt (NVIC).
 * @details This function disables all RTC interrupts by masking them. Moreover,
 *          it clears any pending ones on the ARM core.
 *          The status of RAW_INTR_STAT_REG remains unchanged.
 * @return void
 * @sa rtc_interrupt_disable
 * @sa rtc_interrupt_enable
 ****************************************************************************************
 */
void rtc_unregister_intr(void);

/**
 ****************************************************************************************
 * @brief Sets RTC time and/or calendar date.
 * @details This function will stop the respective counter before setting
 *          the time and will resume the counter afterwards.
 * @param[in] time Value to set the RTC time to.
 * @param[in] clndr Value to set the RTC calendar date to.
 * @note @p time can be NULL if calendar is set only; @p clndr can be NULL
 *       if time is set only.
 * @return RTC_STATUS_CODE_VALID_ENTRY on success, otherwise the failure cause
 ****************************************************************************************
 */
rtc_status_code_t rtc_set_time_clndr(const rtc_time_t *time, const rtc_calendar_t *clndr);

/**
 ****************************************************************************************
 * @brief Sets the RTC time register.
 * @param[in] time_bcd time in binary-coded decimal (BCD) format
 * @return void
 * @note Use wrapper function rtc_set_time() to set time in easy-to-use
 *       decimal number format. If this function is used, then rtc_get_status()
 *       should be called to check the status.
 * @sa rtc_get_status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_set_time_bcd(uint32_t time_bcd)
{
    SetWord32(RTC_TIME_REG, time_bcd);
}

/**
 ****************************************************************************************
 * @brief Sets the RTC calendar register.
 * @param[in] clndr_bcd Calendar date in binary-coded decimal (BCD) format
 * @return void
 * @note Use wrapper function rtc_set_clndr() to set calendar in easy-to-use
 *       decimal number format. If this function is used, then rtc_get_status()
 *       should be called to check the status.
 * @sa rtc_get_status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_set_clndr_bcd(uint32_t clndr_bcd)
{
    SetWord32(RTC_CALENDAR_REG, clndr_bcd);
}

/**
 ****************************************************************************************
 * @brief Gets RTC time and/or calendar date.
 * @param[out] time Returns the RTC time. Set NULL if there is no need to read time.
 * @param[out] clndr returns the RTC calendar. Set NULL if there is no need to read calendar date.
 * @return void
 * @warning When reading simultaneously time and date, in order to obtain a
 *          coherent view of time and date, the counters shall be stopped while
 *          reading them. This avoids the situation where the date counter
 *          increments between reading the time register and reading the date
 *          register. To avoid stopping the counters temporarily,
 *          call this function with @p clndr to NULL, to read time first
 *          and then call this function with @p time to NULL, to read the date.
 ****************************************************************************************
 */
void rtc_get_time_clndr(rtc_time_t *time, rtc_calendar_t *clndr);

/**
 ****************************************************************************************
 * @brief Reads the RTC time register.
 * @return Time in binary-coded decimal (BCD) format.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint32_t rtc_get_time_bcd(void)
{
    return GetWord32(RTC_TIME_REG);
}

/**
 ****************************************************************************************
 * @brief Reads the RTC calendar register.
 * @return Calendar date in binary-coded decimal (BCD) format.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint32_t rtc_get_clndr_bcd(void)
{
    return GetWord32(RTC_CALENDAR_REG);
}

/**
 ****************************************************************************************
 * @brief Starts RTC time.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_time_start(void)
{
    SetBits32(RTC_CONTROL_REG, RTC_TIME_DISABLE, 0);
}

/**
 ****************************************************************************************
 * @brief Stops RTC time.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_time_stop(void)
{
    SetBits32(RTC_CONTROL_REG, RTC_TIME_DISABLE, 1);
}

/**
 ****************************************************************************************
 * @brief Starts RTC Calendar.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_clndr_start(void)
{
    SetBits32(RTC_CONTROL_REG, RTC_CAL_DISABLE, 0);
}

/**
 ****************************************************************************************
 * @brief Stops RTC Calendar.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_clndr_stop(void)
{
    SetBits32(RTC_CONTROL_REG, RTC_CAL_DISABLE, 1);
}

/**
 ****************************************************************************************
 * @brief Starts RTC, both the time and the calendar.
 * @return void
 * @warning RTC is using the low power clock (lp_clk) as clock source, therefore
 *          first lp_clk must be configured and enabled and second the 100-Hz
 *          RTC clock must be configured and enabled
 *          before calling this function.
 * @sa rtc_clk_config
 * @sa rtc_clock_enable
 ****************************************************************************************
 */

__STATIC_FORCEINLINE void rtc_start(void)
{
    SetWord32(RTC_CONTROL_REG, 0x0);
}

/**
 ****************************************************************************************
 * @brief Stop RTC, both the time and the calendar.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_stop(void)
{
    SetWord32(RTC_CONTROL_REG, 0x3);
}

/**
 ****************************************************************************************
 * @brief Sets RTC alarm.
 * @details Sets time and/or calendar alarms. Enable the @p mask bitmask to
 *          select the alarms needed. The alarm interrupt is enabled
 *          automatically; an interrupt will be generated when an alarm event
 *          occurs.
 * @param[in] time Time alarm. Can be NULL if calendar alarm is set only.
 * @param[in] clndr Calendar alarm. Can be NULL if time alarm is set only.
 * @param[in] mask Bitmask of rtc_alarm_en_t. Set "1" to enable:
 * @parblock
 *        Bit:          |    5    |     4   |    3    |    2   |    1   |     0    |
 *                      +---------+---------+---------+--------+--------+----------+
 *        Alarm enable: | on month| on mday | on hour | on min | on sec | on hsec  |
 *                      +---------+---------+---------+--------+--------+----------+
 * @endparblock
 * @return RTC_STATUS_CODE_VALID_ENTRY on success, otherwise the failure cause
 * @sa rtc_interrupt_enable
 * @sa rtc_alarm_enable
 ****************************************************************************************
 */
rtc_status_code_t rtc_set_alarm(const rtc_time_t *time, const rtc_alarm_calendar_t *clndr, const uint8_t mask);

/**
 ****************************************************************************************
 * @brief Sets the RTC time alarm register.
 * @param[in] time_bcd Time in binary-coded decimal (BCD) format.
 * @return void
 * @note Use wrapper function rtc_set_alarm_time() to set the alarm in
 *       easy-to-use decimal number format. If this function is used,
 *       rtc_get_status() shall be called to check the status.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_set_alarm_time_bcd(uint32_t time_bcd)
{
    SetWord32(RTC_TIME_ALARM_REG, time_bcd);
}

/**
 ****************************************************************************************
 * @brief Sets the RTC calendar alarm register.
 * @param[in] clndr_bcd Calendar date in binary-coded decimal (BCD) format.
 * @return void
 * @note Use wrapper function rtc_set_alarm_clndr() to set the alarm in
 *       easy-to-use decimal number format. If this function is used,
 *       rtc_get_status() shall be called to check the status.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_set_alarm_clndr_bcd(uint32_t clndr_bcd)
{
    SetWord32(RTC_CALENDAR_ALARM_REG, clndr_bcd);
}

/**
 ****************************************************************************************
 * @brief Gets RTC time and/or calendar alarms.
 * @param[out] time Returns time alarm. Set NULL if there is no need to read time alarm.
 * @param[out] clndr Returns calendar alarm. Set NULL if there is no need to read calendar alarm.
 * @param[out] mask Returns bitmask of rtc_alarm_en_t. Set NULL if there is no need to read bitmask.
 *             "1" means the alarm is enabled:
 * @parblock
 *        Bit:          |    5    |     4   |    3    |    2   |    1   |     0    |
 *                      +---------+---------+---------+--------+--------+----------+
 *        Alarm enable: | on month| on mday | on hour | on min | on sec | on hsec  |
 *                      +---------+---------+---------+--------+--------+----------+
 * @endparblock
 * @return void
 ****************************************************************************************
 */
void rtc_get_alarm(rtc_time_t *time, rtc_alarm_calendar_t *clndr, uint8_t *mask);

/**
 ****************************************************************************************
 * @brief Reads the RTC time alarm register.
 * @return Time in binary-coded decimal (BCD) format.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint32_t rtc_get_alarm_time_bcd(void)
{
    return GetWord32(RTC_TIME_ALARM_REG);
}

/**
 ****************************************************************************************
 * @brief Reads the RTC calendar alarm calendar.
 * @return Calendar date in binary-coded decimal (BCD) format.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint32_t rtc_get_alarm_clndr_bcd(void)
{
    return GetWord32(RTC_CALENDAR_ALARM_REG);
}

/**
 ****************************************************************************************
 * @brief Enables RTC alarm.
 * @param[in] mask bitmask of rtc_alarm_en_t. Set "1" to enable:
 * @parblock
 *        Bit:          |    5    |     4   |    3    |    2   |    1   |     0    |
 *                      +---------+---------+---------+--------+--------+----------+
 *        Alarm enable: | on month| on mday | on hour | on min | on sec | on hsec  |
 *                      +---------+---------+---------+--------+--------+----------+
 * @endparblock
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_alarm_enable(const uint8_t mask)
{
    SetWord32(RTC_ALARM_ENABLE_REG, mask);
}

/**
 ****************************************************************************************
 * @brief Returns RTC alarm enable bitmask.
 * @return bitmask of rtc_alarm_en_t. "1" indicate the alarm is enabled:
 * @parblock
 *        Bit:          |    5    |     4   |    3    |    2   |    1   |     0    |
 *                      +---------+---------+---------+--------+--------+----------+
 *        Alarm enable: | on month| on mday | on hour | on min | on sec | on hsec  |
 *                      +---------+---------+---------+--------+--------+----------+
 * @endparblock
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t rtc_get_alarm_enable_msk(void)
{
    return GetWord32(RTC_ALARM_ENABLE_REG);
}

/**
 ****************************************************************************************
 * @brief Enables RTC interrupt(s).
 * @param[in] mask bitmask of available interrupts (rtc_intr_t). Set "1" to enable:
 * @parblock
 *        Bit:        |  6     |    5   |   4   |   3   |  2   |  1   |   0   |
 *                    +--------+--------+-------+-------+------+------+-------+
 *        Enable irq: |on alarm|on month|on mday|on hour|on min|on sec|on hsec|
 *                    +--------+--------+-------+-------+------+------+-------+
 * @endparblock
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_interrupt_enable(const uint8_t mask)
{
    SetWord32(RTC_INTERRUPT_ENABLE_REG, mask);
}

/**
 ****************************************************************************************
 * @brief Disables RTC interrupt(s).
 * @param[in] mask bitmask of available interrupts (rtc_intr_t). Set "1" to disable:
 * @parblock
 *        Bit:         |  6     |    5   |   4   |   3   |  2   |  1   |   0   |
 *                     +--------+--------+-------+-------+------+------+-------+
 *        Disable irq: |on alarm|on month|on mday|on hour|on min|on sec|on hsec|
 *                     +--------+--------+-------+-------+------+------+-------+
 * @endparblock
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_interrupt_disable(const uint8_t mask)
{
    SetWord32(RTC_INTERRUPT_DISABLE_REG, mask);
}

/**
 ****************************************************************************************
 * @brief Returns RTC event flags.
 * @return bitmask of event flags (rtc_event_t). "1" indicates that the event occurred since the last reset:
 * @parblock
 *        Bit:   |  6     |    5   |   4   |   3   |  2   |  1   |   0   |
 *               +--------+--------+-------+-------+------+------+-------+
 *        Event: |on alarm|on month|on mday|on hour|on min|on sec|on hsec|
 *               +--------+--------+-------+-------+------+------+-------+
 * @endparblock
 * @note Upon reading the event flag register, the register gets cleared.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t rtc_get_event_flags(void)
{
    return GetWord32(RTC_EVENT_FLAGS_REG);
}

/**
 ****************************************************************************************
 * @brief Returns RTC interrupt mask.
 * @return interrupt bitmask (rtc_intr_t)
 * @parblock
 *        Bit:      |  6     |    5   |   4   |   3   |  2   |  1   |   0   |
 *                  +--------+--------+-------+-------+------+------+-------+
 *        Intr msk: | alarm  | month  | mday  | hour  | min  | sec  | hsec  |
 *                  +--------+--------+-------+-------+------+------+-------+
 * @endparblock
 * @note Bitmask can be cleared by enabling the corresponding interrupt and
 *       can be set by disabling the corresponding interrupt.
 * @sa rtc_interrupt_enable
 * @sa rtc_interrupt_disable
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t rtc_get_interrupt_mask(void)
{
    return GetWord32(RTC_INTERRUPT_MASK_REG);
}

/**
 ****************************************************************************************
 * @brief Sets RTC hour clock mode.
 * @param[in] mode 12-hour or 24-hour clock format
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_set_hour_clk_mode(rtc_hour_mode_t mode)
{
    SetWord32(RTC_HOUR_MODE_REG, mode);
}

/**
 ****************************************************************************************
 * @brief Returns RTC hour clock mode.
 * @return 12-hour or 24-hour clock format
 ****************************************************************************************
 */
__STATIC_FORCEINLINE rtc_hour_mode_t rtc_get_hour_clk_mode(void)
{
    return ( (GetBits32(RTC_HOUR_MODE_REG, RTC_HMS) == 1) ? RTC_HOUR_MODE_12H : RTC_HOUR_MODE_24H );
}

/**
 ****************************************************************************************
 * @brief Returns RTC status.
 * @details Upon setting time/calendar or alarm, the entry can be invalid.
 *          The status bitmask indicates whether the entry was valid or not.
 * @return status bitmask: "1" means valid entry and "0" invalid entry
 * @parblock
 *        Bit:    |   3            |  2         |  1       |   0  |
 *                +----------------+------------+----------+------+
 *        Status: | calendar alarm | time alarm | calendar | time |
 *                +----------------+------------+----------+------+
 * @endparblock
 * @sa rtc_set_time_clndr
 * @sa rtc_set_alarm
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t rtc_get_status(void)
{
    return GetWord32(RTC_STATUS_REG);
}

/**
 ****************************************************************************************
 * @brief Configures RTC to keep or reset its registers after software reset.
 * @param[in] keep When true, the time and calendar registers and any other
 *            registers which directly affect or are affected by the time and
 *            calendar registers are NOT reset when software reset is applied.
 *            When false, the software reset will reset every register except
 *            the keep RTC and control registers.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_set_keep_reg_on_reset(bool keep)
{
    SetWord32(RTC_KEEP_RTC_REG, keep);
}

/**
 ****************************************************************************************
 * @brief Get RTC keep register status
 * @return A boolean flag:
 *         - true meaning that the time and calendar registers and any other
 *           registers which directly affect or are affected by the time and
 *           calendar registers are NOT reset when software reset is applied,
 *         - false, the software reset will reset every register except the
 *           keep RTC and control registers.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE bool rtc_get_keep_reg_on_reset(void)
{
    return GetWord32(RTC_KEEP_RTC_REG);
}

/**
 ****************************************************************************************
 * @brief Resets the RTC block.
 * @return void
 * @note Reset pulse width is not important since the reset is asynchronous.
 * @sa rtc_set_keep_reg_on_reset
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_reset(void)
{
    uint8_t temp_reg;
    temp_reg = GetWord32(RTC_INTERRUPT_MASK_REG);
    /* Mask all interrupts to prevent any from being triggered due to block reset. */
    SetWord32(RTC_INTERRUPT_DISABLE_REG, 0xFF);
    SetBits32(CLK_RTCDIV_REG, RTC_RESET_REQ, 1);
    SetBits32(CLK_RTCDIV_REG, RTC_RESET_REQ, 0);
    /* Restore the interrupt mask. */
    SetWord32(RTC_INTERRUPT_ENABLE_REG, ~temp_reg & 0x7F);
}

/**
 ****************************************************************************************
 * @brief Issues a reset request for the RTC block.
 * @return void
 * @note RTC reset shall be cleared before starting the RTC block.
 * @sa rtc_reset_disable
 * @sa rtc_set_keep_reg_on_reset
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_reset_enable(void)
{
    SetBits32(CLK_RTCDIV_REG, RTC_RESET_REQ, 1);
}

/**
 ****************************************************************************************
 * @brief Clears the reset request for the RTC block.
 * @return void
 * @sa rtc_reset_enable
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_reset_disable(void)
{
    SetBits32(CLK_RTCDIV_REG, RTC_RESET_REQ, 0);
}

/**
 ****************************************************************************************
 * @brief Configures RTC clock by calculating RTC_DIV_INT and RTC_DIV_FRAC
 * @param[in] div_denom denominator for the fractional division
 * @param[in] lp_clk_freq frequency of the RTC clock source in Hz
 * @return void
 * @note RTC uses the low power clock (lp_clk) as clock source, therefore
 *       lp_clk must be configured and enabled before calling this function.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_clk_config(rtc_div_denom_t div_denom, uint16_t lp_clk_freq)
{
    uint16_t temp;

    SetBits32(CLK_RTCDIV_REG, RTC_DIV_DENOM, div_denom);

    temp = lp_clk_freq / 100u;
    SetBits32(CLK_RTCDIV_REG, RTC_DIV_INT, temp);

    temp = 10u * (lp_clk_freq - (temp * 100u));
    SetBits32(CLK_RTCDIV_REG, RTC_DIV_FRAC, temp);
}

/**
 ****************************************************************************************
 * @brief Enables the generation of 100 Hz for the RTC block.
 * @return void
 * @sa rtc_clk_config
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rtc_clock_enable(void)
{
    SetBits32(CLK_RTCDIV_REG, RTC_DIV_ENABLE, 1);
}

#endif /* _RTC_H_ */
