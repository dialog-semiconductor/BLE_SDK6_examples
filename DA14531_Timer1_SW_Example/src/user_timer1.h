/**
 ****************************************************************************************
 *
 * @file user_timer1.h
 *
 * @brief User SW Timer1 header file.
 *
 * Copyright (c) 2015-2019 Dialog Semiconductor. All rights reserved.
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

#ifndef _USER_TIMER1_H_
#define _USER_TIMER1_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include <stdio.h>
#include <stdint.h>
#include "arch_system.h"
#include "uart.h"
#include "uart_utils.h"
#include "user_periph_setup.h"
#include "timer1.h"
#include "timer0_2.h"
#include "timer0.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#undef				COUNTING_MODE
#define				CAPTURING_MODE
#define	 			TIMER0_SOURCE

// SW Timer0 - PWM settings
#define 		TIMER_ON        1000
#define 		PWM_HIGH        500
#define 		PWM_LOW         500

#define COUNTER_RELOAD_VALUE     200

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if defined (CAPTURING_MODE)
/**
 ****************************************************************************************
 * @brief Timer0 PWM setup function
 * @return void
 ****************************************************************************************
 */
void timer0_pwm_setup(void);

/**
 ****************************************************************************************
 * @brief Timer1 setup capturing finction
 * @param[in] void
 * @return void
 ****************************************************************************************
 */
void timer1_capturing_setup(void);

/**
 ****************************************************************************************
 * @brief Main routine of the Timer1 Software Example
 *
 ****************************************************************************************
 */
 #endif
 
 #if defined (COUNTING_MODE)
 void timer1_general_test(void);
 #endif
 
 /// @} APP

#endif // _USER_TIMER1_H_
