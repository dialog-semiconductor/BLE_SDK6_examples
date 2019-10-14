/**
 ****************************************************************************************
 *
 * @file user_wakeup.h
 *
 * @brief Barebone application header file.
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

#ifndef _USER_WAKEUP_H_
#define _USER_WAKEUP_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup
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
 

/*
 * DEFINES
 ****************************************************************************************
 */

#define DEBOUNCE_TIME 					30 // Time in ms, can be a value from 0 to 63 
#define EVENTS_BEFORE_INTERRUPT 1  // Must be a 8 bits value

enum { 
				INTERRUPT_DISABLE = 0,
				INTERRUPT_ENABLE
		 };

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if defined(__DA14531__)
void user_reset_event_counter(void);
#endif

void user_wakeup_example_init(void);

void user_app_wakeup_press_cb(void);

/// @} APP

#endif //_USER_BAREBONE_H_
