/**
 ****************************************************************************************
 *
 * @file timer0.h
 *
 * @brief DA14585/DA14586/DA14531 Timer0 driver header file.
 *
 * Copyright (c) 2016-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef _TIMER0_H_
#define _TIMER0_H_

#include <stdint.h>
#include "datasheet.h"

typedef enum
{
    PWM_MODE_ONE              = 0,
    PWM_MODE_CLOCK_DIV_BY_TWO = 1
} PWM_MODE_t;

typedef enum
{
    TIM0_CLK_DIV_BY_10 = 0,
    TIM0_CLK_NO_DIV    = 1
} TIM0_CLK_DIV_t;

typedef enum
{
    TIM0_CLK_32K  = 0,
    TIM0_CLK_FAST = 1
} TIM0_CLK_SEL_t;

typedef enum
{
    TIM0_CTRL_OFF_RESET = 0,
    TIM0_CTRL_RUNNING   = 1
} TIM0_CTRL_t;

typedef void (timer0_handler_function_t)(void);

/**
 ****************************************************************************************
 * @brief  Initializes TIMER0.
 * @param[in] tim0_clk_sel  Timer0 uses 16MHz (fast) or 32kHz (slow) clock frequency
 * @param[in] pwm_mode      (during high time) '1' or (fast) clock divided by 2
 * @param[in] tim0_clk_div  Division factor for TIMER0 "on" time in peripheral divider register
 * @return void
 ****************************************************************************************
 */
void timer0_init(TIM0_CLK_SEL_t tim0_clk_sel, PWM_MODE_t pwm_mode, TIM0_CLK_DIV_t tim0_clk_div);

/**
 ****************************************************************************************
 * @brief Starts TIMER0
 * @return void
 ****************************************************************************************
 */
void timer0_start(void);

/**
 ****************************************************************************************
 * @brief Stops TIMER0.
 * @return void
 ****************************************************************************************
 */
void timer0_stop(void);

/**
 ****************************************************************************************
 * @brief Releases TIMER0. Does not disable the TIM clock, as it is shared with TIMER2.
 * @return void
 ****************************************************************************************
 */
void timer0_release(void);

/**
 ****************************************************************************************
 * @brief Configures TIMER0 ON counter reload value.
 * @param[in] pwm_on ON COUNTER reload value
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer0_set_pwm_on_counter(uint16_t pwm_on)
{
    SetWord16(TIMER0_ON_REG, pwm_on);
}

/**
 ****************************************************************************************
 * @brief Configures TIMER0 M counter reload value (duration of PWM0 high part).
 * @param[in] pwm_high M COUNTER reload value
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer0_set_pwm_high_counter(uint16_t pwm_high)
{
    SetWord16(TIMER0_RELOAD_M_REG, pwm_high);
}

/**
 ****************************************************************************************
 * @brief Configures TIMER0 N counter reload value (duration of PWM0 low part).
 * @param[in] pwm_low N COUNTER reload value
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer0_set_pwm_low_counter(uint16_t pwm_low)
{
    SetWord16(TIMER0_RELOAD_N_REG, pwm_low);
}

/**
 ****************************************************************************************
 * @brief Configures TIMER0 pwm timing parameters.
 * @param[in] pwm_on   ON COUNTER reload value
 * @param[in] pwm_high M COUNTER reload value
 * @param[in] pwm_low  N COUNTER reload value
 * @return void
 ****************************************************************************************
 */
void timer0_set(uint16_t pwm_on, uint16_t pwm_high, uint16_t pwm_low);

/**
 ****************************************************************************************
 * @brief Enables SWTIM_IRQn.
 * @return void
 ****************************************************************************************
 */
void timer0_enable_irq(void);

/**
 ****************************************************************************************
 * @brief Disables SWTIM_IRQn.
 * @return void
 ****************************************************************************************
 */
void timer0_disable_irq(void);

/**
 ****************************************************************************************
 * @brief Registers a callback function to be called within SWTIM_Handler.
 * @param[in] callback Callback function
 * @return void
 ****************************************************************************************
 */
void timer0_register_callback(timer0_handler_function_t *callback);

#endif /* _TIMER0_H_ */
