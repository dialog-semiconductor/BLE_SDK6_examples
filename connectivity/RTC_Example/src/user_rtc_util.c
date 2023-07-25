/**
 ****************************************************************************************
 *
 * @file user_rtc_util.c
 *
 * @brief User code for RTC utilities.
 *
 * Copyright (C) 2012-2021 Renesas Electronics Corporation and/or its affiliates
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
 
static uint8_t rtc_interrupt_enable_reg_r               __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
extern rcx_time_data_t rcx_time_data;
 
void rtc_clock_set(void)
{
    // Set the frequency that the RTC will operate
    if (arch_clk_is_RCX20()) {
        // Calculate RCX frequency. (1000000 / 625 >> 20) is equal to (100 >> 16), which is equal to 25 >> 14
        rtc_clk_config(RTC_DIV_DENOM_1000, rcx_time_data.rcx_freq);
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

uint8_t user_get_rtc_interrupt_enabled(void)
{
    return rtc_interrupt_enable_reg_r;
}

void user_rtc_register_intr(rtc_interrupt_cb_t handler, uint8_t mask)
{
    rtc_interrupt_enable_reg_r |= mask;
    rtc_register_intr(handler, rtc_interrupt_enable_reg_r);
}

uint8_t user_rtc_unregister_intr(uint8_t mask)
{
    rtc_interrupt_enable_reg_r ^= mask;
    rtc_interrupt_disable(~rtc_interrupt_enable_reg_r);
    
    if ( !rtc_interrupt_enable_reg_r )
        rtc_unregister_intr();
    
    return rtc_interrupt_enable_reg_r;
}
