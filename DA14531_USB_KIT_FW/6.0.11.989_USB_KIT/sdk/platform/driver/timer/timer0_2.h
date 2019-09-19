/**
 ****************************************************************************************
 *
 * @file timer0_2.h
 *
 * @brief DA14585/DA14586/DA14531 Timer0/Timer2 driver header file - common part.
 *
 * Copyright (c) 2016-2019 Dialog Semiconductor. All rights reserved.
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

#ifndef _TIMER0_2_H_
#define _TIMER0_2_H_

#include "datasheet.h"

/// Timer0/Timer2 input clock division factor (provided that system clock is the input clock)
typedef enum
{
    /// System clock divided by 1
    TIM0_2_CLK_DIV_1 = 0,

    /// System clock divided by 2
    TIM0_2_CLK_DIV_2 = 1,

    /// System clock divided by 4
    TIM0_2_CLK_DIV_4 = 2,

    /// System clock divided by 8
    TIM0_2_CLK_DIV_8 = 3
} tim0_2_clk_div_t;

/// Timer0/Timer2 input clock disable/enable
typedef enum
{
    /// Timer0/Timer2 input clock disabled
    TIM0_2_CLK_OFF,

    /// Timer0/Timer2 input clock enabled
    TIM0_2_CLK_ON,
} tim0_2_clk_t;

/// Configuration bearing the Timer0/Timer2 input clock division factor
typedef struct
{
    /// Timer0/Timer2 input clock division factor
    tim0_2_clk_div_t clk_div;
} tim0_2_clk_div_config_t;

/**
 ****************************************************************************************
 * @brief Enables the Timer0/Timer2 input clock.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer0_2_clk_enable(void)
{
    SetBits16(CLK_PER_REG, TMR_ENABLE, TIM0_2_CLK_ON);
}

/**
 ****************************************************************************************
 * @brief Disables the Timer0/Timer2 input clock.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer0_2_clk_disable(void)
{
    SetBits16(CLK_PER_REG, TMR_ENABLE, TIM0_2_CLK_OFF);
}

/**
 ****************************************************************************************
 * @brief Sets the Timer0/Timer2 input clock division factor.
 * @param[in] clk_div_config The configuration bearing the division factor.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer0_2_clk_div_set(tim0_2_clk_div_config_t *clk_div_config)
{
    SetBits16(CLK_PER_REG, TMR_DIV, clk_div_config->clk_div);
}

#endif /* _TIMER0_2_H_ */
