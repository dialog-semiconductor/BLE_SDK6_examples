/**
 ****************************************************************************************
 *
 * @file user_wakeup.c
 *
 * @brief user_wakeup source.
 *
 * Copyright (c) 2012-2023 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
 
 /*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include "wkupct_quadec.h" 
#include "user_wakeup.h"
#include "user_periph_setup.h"
#include "gpio.h"

typedef struct
{
	uwakeup_cb cb;
	GPIO_PORT port;
	GPIO_PIN	pin;
	uint8_t polarity;
	
}wakeup_config_t;


wakeup_config_t	wakeup_config								__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

static void _uwakeup_cb(void);

static void uwakeup_reinit(void)
{
	wkupct_enable_irq( 	WKUPCT_PIN_SELECT(wakeup_config.port, wakeup_config.pin), 
										WKUPCT_PIN_POLARITY(wakeup_config.port, wakeup_config.pin, wakeup_config.polarity), 
										EVENTS_BEFORE_INTERRUPT, 
										DEBOUNCE_TIME);
	
	wkupct_register_callback(_uwakeup_cb);
}

static void _uwakeup_cb(void)
{
	uwakeup_reinit();
	wakeup_config.cb();
}



void uwakeup_init(uwakeup_cb cb, GPIO_PORT port, GPIO_PIN pin, uint8_t polarity )
{
	
	wakeup_config.cb = cb;
	wakeup_config.port = port;
	wakeup_config.pin = pin;
	wakeup_config.polarity = polarity;
	
	uwakeup_reinit();

	
}


/// @} APP
