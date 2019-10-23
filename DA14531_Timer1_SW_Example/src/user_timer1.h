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
#include "user_periph_setup.h"
#include "timer1.h"
#include "timer0_2.h"
#include "timer0.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/****************************************************************************************/
/* Select Counting or Capturing Mode                                                    */
/*	- COUNTING_MODE : Counting Mode of Timer1. Toggling LED GPIO (Default Configuration)*/
/*	- CAPTURING_MODE : Capturing Mode of Timer1. Supply PWM input source to CAPTURE GPIO*/
/****************************************************************************************/
#define				COUNTING_MODE
#undef				CAPTURING_MODE

/****************************************************************************************/
/* In case of Capturing Mode, define the TIMER0_SOURCE macro is:                                                   */
/*	- defined 	: PWM from SW TImer0 should be applied on CAPTURE GPIO.					*/
/*	- undefined : PWM from an external source should be applied on CAPTURE GPIO.        */
/****************************************************************************************/
#if defined (CAPTURING_MODE)
#undef	 		TIMER0_SOURCE
#endif


/****************************************************************************************/
/* SW Timer0 - PWM settings                                                  			*/
/*	- TIMER_ON 	: Congigure TIMER0_ON_REG register										*/
/*	- PWM_HIGH 	: Congigure TIMER0_RELOAD_M_REG register       							*/
/*	- PWM_LOW 	: Congigure TIMER0_RELOAD_N_REG register       							*/
/****************************************************************************************/
#define 		TIMER_ON        	1000
#define 		PWM_HIGH        	500
#define 		PWM_LOW         	500


/****************************************************************************************/
/* Define the Timer0 reload value                                                 		*/
/****************************************************************************************/
#define COUNTER_RELOAD_VALUE     	200

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

#endif
 
 
#if defined (COUNTING_MODE)
 
 /**
 ****************************************************************************************
 * @brief Main routine of the Timer1 Software Example
 *
 ****************************************************************************************
 */
void timer1_general_test(void);
 
#endif
 
 /// @} APP

#endif // _USER_TIMER1_H_
