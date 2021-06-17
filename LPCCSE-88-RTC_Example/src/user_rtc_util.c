/**
 ****************************************************************************************
 *
 * @file user_rtc_util.c
 *
 * @brief User code for RTC utilities.
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
 
 /*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "arch_system.h"
 
extern uint32_t rcx_freq;
 
void rtc_clock_set(void)
{
    // Set the frequency that the RTC will operate
    if (arch_clk_is_RCX20()) {
        // Calculate RCX frequency. (1000000 / 625 >> 20) is equal to (100 >> 16), which is equal to 25 >> 14
        rtc_clk_config(RTC_DIV_DENOM_1000, rcx_freq);
    } else {
        rtc_clk_config(RTC_DIV_DENOM_1000, 32768);
    }
}

rtc_status_code_t rtc_configure(rtc_time_t *time, rtc_calendar_t *cal, rtc_config_t *config)
{
    // Power up the PD_TIM domain
    SetBits16(PMU_CTRL_REG,TIM_SLEEP, 0); // wakup timer PD
    while (GetBits16(SYS_STAT_REG,TIM_IS_UP) == 0);

    // Initialize the RTC configuration
    rtc_init(config);
    // Set the clock of the RTC depending on the selected LP clock
    rtc_clock_set();
    // Enable the RTC clk
    rtc_clock_enable();
    return rtc_set_time_clndr(time, cal);
}

uint32_t rtc_convert_time_to_msec(rtc_time_t *time)
{
    return ((((time->hour * 60 + time->minute) * 60) + time->sec) * 1000 + time->hsec *10);
}
 
 