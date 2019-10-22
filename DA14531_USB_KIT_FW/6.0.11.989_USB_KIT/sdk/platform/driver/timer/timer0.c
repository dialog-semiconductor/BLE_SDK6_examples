/**
 ****************************************************************************************
 *
 * @file timer0.c
 *
 * @brief DA14585/DA14586/DA14531 Timer0 driver source file.
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

#include <stdint.h>
#include <stddef.h>
#include "compiler.h"
#include "datasheet.h"
#include "timer0.h"

timer0_handler_function_t *TIMER0_callback      __SECTION_ZERO("retention_mem_area0");

void timer0_init(TIM0_CLK_SEL_t tim0_clk_sel, PWM_MODE_t pwm_mode, TIM0_CLK_DIV_t tim0_clk_div)
{
    uint16_t temp = (((uint16_t) pwm_mode) << 3) +
                    (((uint16_t) tim0_clk_div) << 2) +
                    (((uint16_t) tim0_clk_sel) << 1);

    SetWord16(TIMER0_CTRL_REG, temp);

    /* Set Priority for TIM0 Interrupt to level 2. */
    NVIC_SetPriority (SWTIM_IRQn, 2);
}

void timer0_start(void)
{
    /* Timer0 is running. */
    SetBits16(TIMER0_CTRL_REG, TIM0_CTRL, TIM0_CTRL_RUNNING);
}

void timer0_stop(void)
{
    /* Timer0 is off and in reset state. */
    SetBits16(TIMER0_CTRL_REG, TIM0_CTRL, TIM0_CTRL_OFF_RESET);
}

void timer0_release(void)
{
    timer0_stop();
}

void timer0_set(uint16_t pwm_on, uint16_t pwm_high, uint16_t pwm_low)
{
    SetWord16(TIMER0_ON_REG, pwm_on);
    SetWord16(TIMER0_RELOAD_M_REG, pwm_high);
    SetWord16(TIMER0_RELOAD_N_REG, pwm_low);
}

void timer0_enable_irq(void)
{
    NVIC_EnableIRQ(SWTIM_IRQn);
}

void timer0_disable_irq(void)
{
    NVIC_DisableIRQ(SWTIM_IRQn);
}

void timer0_register_callback(timer0_handler_function_t *callback)
{
    TIMER0_callback = callback;
}

#if !defined(__EXCLUDE_SWTIM_HANDLER__)
/**
 ****************************************************************************************
 * @brief SWTIM_Handler. The SWTIM_IRQn interrupt handler
 * @return void
 ****************************************************************************************
 */
void SWTIM_Handler(void)
{
    if (TIMER0_callback != NULL)
        TIMER0_callback();
}
#endif
