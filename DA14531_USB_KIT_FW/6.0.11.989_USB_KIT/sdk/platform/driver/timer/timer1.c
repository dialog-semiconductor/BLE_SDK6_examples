/**
 ****************************************************************************************
 *
 * @file timer1.c
 *
 * @brief DA14531 Timer1 driver source file.
 *
 * Copyright (C) 2018 Dialog Semiconductor. All rights reserved.
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
#include <stdbool.h>
#include "timer1.h"
#include "compiler.h"
#include "datasheet.h"
#include "arch.h"

static timer1_count_handler_t   timer1_count_callback      __SECTION_ZERO("retention_mem_area0");
static timer1_event_handler_t   timer1_event1_callback     __SECTION_ZERO("retention_mem_area0");
static timer1_event_handler_t   timer1_event2_callback     __SECTION_ZERO("retention_mem_area0");

static bool in1_ovrflw_occurred __SECTION_ZERO("retention_mem_area0");
static bool in2_ovrflw_occurred __SECTION_ZERO("retention_mem_area0");

void timer1_count_register_callback(timer1_count_handler_t callback)
{
    timer1_count_callback = callback;
}

void timer1_event1_register_callback(timer1_event_handler_t callback)
{
    timer1_event1_callback = callback;
}

void timer1_event2_register_callback(timer1_event_handler_t callback)
{
    timer1_event2_callback = callback;
}

void timer1_count_config(timer1_count_options_t *count_options, timer1_count_handler_t callback)
{
    ASSERT_WARNING(count_options->reload_val <= TIM1_RELOAD_MAX);

    uint32_t temp = (count_options->input_clk << 15u) + (count_options->free_run << 14u) +
                    (count_options->irq_mask << 13u) + (count_options->count_dir << 12u) +
                    (count_options->reload_val);

    /* Reset the current timer value. */
    // NOTE: The following operation sequence shall not change!!
    SetBits32(TIMER1_CTRL_REG, TIMER1_CLK_EN, TIM1_CLK_ON);
    SetBits32(TIMER1_CTRL_REG, TIMER1_USE_SYS_CLK, TIM1_CLK_SRC_SYS);
    SetBits32(TIMER1_CTRL_REG, TIMER1_COUNT_DOWN_EN, count_options->count_dir);
    SetBits32(TIMER1_CTRL_REG, TIMER1_ENABLE, TIM1_OFF);

    /* Write the provided configuration. */
    SetWord32(TIMER1_CTRL_REG, temp);

    /* Register the provided callback. */
    timer1_count_register_callback(callback);
}

void timer1_event1_config(timer1_event_options_t *event_options, timer1_event_handler_t callback)
{
    ASSERT_WARNING((event_options->period_count - 1 >= 0) && (event_options->period_count <= TIM1_PERIOD_MAX));

    if ( (GetWord32(TIMER1_CTRL_REG) & (TIMER1_ENABLE + TIMER1_CLK_EN)) == (TIMER1_ENABLE + TIMER1_CLK_EN) )
    {
        uint32_t val = (event_options->stamp_type << 13u) + ((event_options->period_count - 1) << 7u) +
                       (event_options->irq_mask << 6u) + (event_options->event_type << 5u) +
                       (event_options->edge_type << 4u);

        /* Write the IN1 event configuration. */
        SetBits32(TIMER1_CAPTURE_REG, TIMER1_IN1_STAMP_TYPE | TIMER1_IN1_PERIOD_MAX |
                                      TIMER1_IN1_IRQ_EN | TIMER1_IN1_COUNT_EN |
                                      TIMER1_IN1_EVENT_FALL_EN | TIMER1_GPIO1_CONF, val);

        timer1_event1_register_callback(callback);

        SetBits32(TIMER1_CAPTURE_REG, TIMER1_GPIO1_CONF, event_options->gpio_pin);

        if (GetBits32(TIMER1_CTRL_REG, TIMER1_USE_SYS_CLK) == TIM1_CLK_SRC_LP)
        {
            /* Wait until edge type is applied. */
            uint16_t temp = GetBits32(TIMER1_STATUS_REG, TIMER1_TIMER_VALUE);
            while (GetBits32(TIMER1_STATUS_REG, TIMER1_TIMER_VALUE) == temp)
                ;
        }
    }
}

void timer1_event2_config(timer1_event_options_t *event_options, timer1_event_handler_t callback)
{
    ASSERT_WARNING((event_options->period_count - 1 >= 0) && (event_options->period_count <= TIM1_PERIOD_MAX));

    if ( (GetWord32(TIMER1_CTRL_REG) & (TIMER1_ENABLE + TIMER1_CLK_EN)) == (TIMER1_ENABLE + TIMER1_CLK_EN) )
    {
        uint32_t val = (event_options->stamp_type << 13u) + ((event_options->period_count - 1) << 7u) +
                       (event_options->irq_mask << 6u) + (event_options->event_type << 5u) +
                       (event_options->edge_type << 4u);

        /* Write the IN2 event configuration. */
        SetBits32(TIMER1_CAPTURE_REG, TIMER1_IN2_STAMP_TYPE | TIMER1_IN2_PERIOD_MAX |
                                      TIMER1_IN2_IRQ_EN | TIMER1_IN2_COUNT_EN |
                                      TIMER1_IN2_EVENT_FALL_EN | TIMER1_GPIO2_CONF, val);

        timer1_event2_register_callback(callback);

        SetBits32(TIMER1_CAPTURE_REG, TIMER1_GPIO2_CONF, event_options->gpio_pin);

        if (GetBits32(TIMER1_CTRL_REG, TIMER1_USE_SYS_CLK) == TIM1_CLK_SRC_LP)
        {
            /* Wait until edge type is applied. */
            uint16_t temp = GetBits32(TIMER1_STATUS_REG, TIMER1_TIMER_VALUE);
            while (GetBits32(TIMER1_STATUS_REG, TIMER1_TIMER_VALUE) == temp)
                ;
        }
    }
}

void timer1_enable_irq(void)
{
    // Set the priority of SWTIM1 interrupt to level 2
    NVIC_SetPriority(SWTIM1_IRQn, 2);
    // Enable SWTIM1 interrupt
    NVIC_EnableIRQ(SWTIM1_IRQn);
}

void timer1_disable_irq(void)
{
    NVIC_DisableIRQ(SWTIM1_IRQn);
    SetBits32(TIMER1_CTRL_REG, TIMER1_IRQ_EN, TIM1_IRQ_MASK_ON);
    SetBits32(TIMER1_CAPTURE_REG, TIMER1_IN1_IRQ_EN | TIMER1_IN2_IRQ_EN, TIM1_EVENT_IRQ_MASK_ON);
    NVIC_ClearPendingIRQ(SWTIM1_IRQn);
}

bool timer1_is_in1_ovrflw_occurred(void)
{
    return in1_ovrflw_occurred;
}


bool timer1_is_in2_ovrflw_occurred(void)
{
    return in2_ovrflw_occurred;
}

#if !defined(__EXCLUDE_SWTIM1_HANDLER__)
/**
 ****************************************************************************************
 * @brief Defines the SWTIM1_IRQn interrupt handler.
 * @return void.
 ****************************************************************************************
 */
void SWTIM1_Handler(void)
{
    uint32_t temp = GetWord32(TIMER1_STATUS_REG);

    in1_ovrflw_occurred = false;
    in2_ovrflw_occurred = false;

    if ((temp & TIMER1_TIMER_EVENT) == TIMER1_TIMER_EVENT)
    {
        /* Clear pending count interrupt. */
        SetBits32(TIMER1_CLR_EVENT_REG, TIMER1_CLR_TIMER_EVENT, 1);

        if (timer1_count_callback != NULL)
        {
            timer1_count_callback();
        }
    }

    if ((temp & TIMER1_IN1_EVENT) == TIMER1_IN1_EVENT)
    {
        /* Check whether new IN1 event occurred while interrupt was pending. */
        if ((temp & TIMER1_IN1_OVRFLW) == TIMER1_IN1_OVRFLW)
        {
            in1_ovrflw_occurred = true;
        }

        /* Clear pending IN1 event interrupt. */
        SetBits32(TIMER1_CLR_EVENT_REG, TIMER1_CLR_IN1_EVENT, 1);

        if (timer1_event1_callback != NULL)
        {
            timer1_event1_callback();
        }
    }

    if ((temp & TIMER1_IN2_EVENT) == TIMER1_IN2_EVENT)
    {
        /* Check whether new IN2 event occurred while interrupt was pending. */
        if ((temp & TIMER1_IN2_OVRFLW) == TIMER1_IN2_OVRFLW)
        {
            in2_ovrflw_occurred = true;
        }

        /* Clear pending IN2 event interrupt. */
        SetBits32(TIMER1_CLR_EVENT_REG, TIMER1_CLR_IN2_EVENT, 1);

        if (timer1_event2_callback != NULL)
        {
            timer1_event2_callback();
        }
    }
}
#endif
