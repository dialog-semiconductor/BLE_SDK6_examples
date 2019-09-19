/**
 ****************************************************************************************
 *
 * @file systick.h
 *
 * @brief SysTick driver header file.
 *
 * Copyright (c) 2014-2018 Dialog Semiconductor. All rights reserved.
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
#ifndef _SYSTICK_H_
#define _SYSTICK_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdbool.h>
#include <stdint.h>

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef void (*systick_callback_function_t)(void);

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Register Callback function for SysTick exception.
 * @param[in] callback Callback function's reference
 * @return void
 ****************************************************************************************
 */
void systick_register_callback(systick_callback_function_t callback);

/**
 ****************************************************************************************
 * @brief Function to start the SysTick timer.
 * @param[in] usec      The duration of the countdown
 * @param[in] exception Set to TRUE to generate an exception when the timer counts down
 *                      to 0, FALSE not to
 * @return void
 ****************************************************************************************
 */
void systick_start(uint32_t usec, uint8_t exception);

/**
 ****************************************************************************************
 * @brief Function to create a delay.
 * @param[in] usec the duration of the delay
 * @return void
 ****************************************************************************************
 */
void systick_wait(uint32_t usec);

/**
 ****************************************************************************************
 * @brief Function to stop the SysTick timer.
 * @return void
 ****************************************************************************************
 */
void systick_stop(void);

/**
 ****************************************************************************************
 * @brief Function to read the current value of the timer.
 * @return the current value of the timer
 ****************************************************************************************
 */
uint32_t systick_value(void);

#endif // _SYSTICK_H_
