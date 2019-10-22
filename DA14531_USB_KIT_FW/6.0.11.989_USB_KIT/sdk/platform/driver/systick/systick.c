/**
 ****************************************************************************************
 *
 * @file systick.c
 *
 * @brief SysTick driver.
 *
 * Copyright (c) 2012-2018 Dialog Semiconductor. All rights reserved.
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
#include "compiler.h"
#include "systick.h"
#include "datasheet.h"

systick_callback_function_t callback_function __SECTION_ZERO("retention_mem_area0");

/**
 ****************************************************************************************
 * @brief Register Callback function for SysTick exception.
 *
 * @param[in] callback  Callback function's reference.
 *
 * @return void
 ****************************************************************************************
 */
void systick_register_callback(systick_callback_function_t callback)
{
    callback_function = callback;
}

#if !defined (__EXCLUDE_SYSTICK_HANDLER__)
void SysTick_Handler(void)
{
    if (callback_function != NULL)
        callback_function();
}
#endif

/**
 ****************************************************************************************
 * @brief Function to start the SysTick timer
 *
 * @param[in] usec      the duration of the countdown
 * @param[in] exception set to TRUE to generate an exception when the timer counts down
 *                      to 0, FALSE not to
 *
 * @return void
 ****************************************************************************************
 */
void systick_start(uint32_t usec, uint8_t exception)
{
    SetBits32(&SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, 0);          // disable systick
    SetBits32(&SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, usec-1);     // set systick timeout based on 1MHz clock
    SetBits32(&SysTick->VAL,  SysTick_VAL_CURRENT_Msk, 0);          // clear the Current Value Register and the COUNTFLAG to 0
    SetBits32(&SysTick->CTRL, SysTick_CTRL_TICKINT_Msk, exception); // generate or not the SysTick exception
    SetBits32(&SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk, 0);       // use a reference clock frequency of 1 MHz
    SetBits32(&SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, 1);          // enable systick
}

/**
 ****************************************************************************************
 * @brief Function to stop the SysTick timer
 *
 * @param[in] void
 *
 * @return void
 ****************************************************************************************
 */
void systick_stop(void)
{
    SetBits32(&SysTick->VAL,  SysTick_VAL_CURRENT_Msk, 0);            // clear the Current Value Register and the COUNTFLAG to 0
    SetBits32(&SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, 0);            // disable systick
}

/**
 ****************************************************************************************
 * @brief Function to create a delay
 *
 * @param[in] usec the duration of the delay
 *
 * @return void
 ****************************************************************************************
 */
void systick_wait(uint32_t usec)
{
    systick_start(usec, false);

    // wait for the counter flag to become 1
    // Because the flag is cleared automatically when the register is read, there is no need to clear it
    while (!GetBits32(&SysTick->CTRL, SysTick_CTRL_COUNTFLAG_Msk));

    systick_stop();
}

/**
 ****************************************************************************************
 * @brief Function to read the current value of the timer
 *
 * @param[in] void
 *
 * @return the current value of the timer
 ****************************************************************************************
 */
uint32_t systick_value(void)
{
    return GetBits32(&SysTick->VAL, SysTick_VAL_CURRENT_Msk);
}
