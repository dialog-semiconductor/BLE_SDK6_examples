/**
 ****************************************************************************************
 *
 * @file ADV_TIMER_EXAMPLE_timer.h
 *
 * @brief Headerfile of advertising example
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
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
#ifndef _ADVERTISING_EXAMPLE_H_
#define _ADVERTISING_EXAMPLE_H_

#include "gapc_task.h"                 // gap functions and messages
#include "app_task.h"                  // application task
#include "app.h"                       // application definitions
#include "app_callback.h"
 
#ifdef ADV_EXAMPLE

// Configurable advertising intervals
#define UNDIRECT_ADV_INTERVAL	150		//(150 	* 1msec 	=  150msec)
#define NONCONN_ADV_INTERVAL	1050	//(1050 * 1msec		= 1050msec)

// Configurable state lenght (for timer based example)
#define	UNDIRECT_ADV_DURATION	200		//(200 	* 10msec 	= 		2sec)
#define	NONCONN_ADV_DURATION	300		//(300 	* 10msec 	= 		3sec)
#define SLEEP_DURATION				250		//(250 	* 10msec 	= 	2.5sec)

// declaring three enumerated advertising_state types
typedef enum 
{
		UNDIRECT_ADVERTISING,
		NONCON_ADVERTISING,
		SLEEP 						
}advertising_state;


/**
 ****************************************************************************************
 * @brief Send the device to sleep .
 * @param[in] none
 * @return void
 ****************************************************************************************
*/                          
void user_activate_sleep(void);
                          

/**
 ****************************************************************************************
 * @brief Function controlling the advertising state
 * @return void
 ****************************************************************************************
*/

void user_change_adv_state(advertising_state state);

/**
 ****************************************************************************************
 * @brief Function to start undirected advertising
 * @return void
 ****************************************************************************************
*/

#ifdef ADV_BUTTON_EXAMPLE
/**
 ****************************************************************************************
 * @brief Setting the interrupt button to change advertising state
 * @return void
 ****************************************************************************************
*/
void user_init_button_interrupt(void);

#endif	// ADV_BUTTON_EXAMPLE
#endif 	// ADV_EXAMPLE
#endif 	//_ADVERTISING_EXAMPLE_H_
