/**
 ****************************************************************************************
 *
 * @file timer2.c
 *
 * @brief DA14585/DA14586/DA14531 Timer2 driver source file.
 *
 * Copyright (c) 2019 Dialog Semiconductor. All rights reserved.
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

#include <stdint.h>
#include "timer2.h"
#include "datasheet.h"
#include "arch.h"

void timer2_config(tim2_config_t *config)
{
#if defined (__DA14531__)
    SetBits16(TRIPLE_PWM_CTRL_REG, TRIPLE_PWM_CLK_SEL, config->clk_source);
#endif
    SetBits16(TRIPLE_PWM_CTRL_REG, HW_PAUSE_EN, config->hw_pause);
}

void timer2_pwm_freq_set(uint32_t pwm_freq, uint32_t input_freq)
{
    SetWord16(TRIPLE_PWM_FREQUENCY, (input_freq / pwm_freq) - 1u);
}

void timer2_pwm_signal_config(tim2_pwm_config_t *pwm_config)
{
    ASSERT_WARNING(pwm_config->pwm_dc <= 100u);

    uint32_t counter, dc;

    counter = GetWord16(TRIPLE_PWM_FREQUENCY) + 1u;
    dc      = (counter * (pwm_config->pwm_dc)) / 100u;
#if defined (__DA14531__)
    ASSERT_WARNING(pwm_config->pwm_offset <= 100u);
    uint32_t offset  = (counter * (pwm_config->pwm_offset)) / 100u;
#endif

    switch (pwm_config->pwm_signal)
    {
        case TIM2_PWM_2:
        {
#if defined (__DA14531__)
            SetWord16(PWM2_START_CYCLE, offset);
            SetWord16(PWM2_END_CYCLE, dc + offset);
#else
            SetWord16(PWM2_DUTY_CYCLE, dc);
#endif
            break;
        }

        case TIM2_PWM_3:
        {
#if defined (__DA14531__)
            SetWord16(PWM3_START_CYCLE, offset);
            SetWord16(PWM3_END_CYCLE, dc + offset);
#else
            SetWord16(PWM3_DUTY_CYCLE, dc);
#endif
            break;
        }

        case TIM2_PWM_4:
        {
#if defined (__DA14531__)
            SetWord16(PWM4_START_CYCLE, offset);
            SetWord16(PWM4_END_CYCLE, dc + offset);
#else
            SetWord16(PWM4_DUTY_CYCLE, dc);
#endif
            break;
        }

#if defined (__DA14531__)
        case TIM2_PWM_5:
        {
            SetWord16(PWM5_START_CYCLE, offset);
            SetWord16(PWM5_END_CYCLE, dc + offset);
            break;
        }

        case TIM2_PWM_6:
        {
            SetWord16(PWM6_START_CYCLE, offset);
            SetWord16(PWM6_END_CYCLE, dc + offset);
            break;
        }

        case TIM2_PWM_7:
        {
            SetWord16(PWM7_START_CYCLE, offset);
            SetWord16(PWM7_END_CYCLE, dc + offset);
            break;
        }
#endif

        default:
        {
            /* Default case added for the sake of completeness; it will
               never be reached. */
            break;
        }
    }
}
