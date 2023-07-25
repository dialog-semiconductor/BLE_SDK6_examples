/**
 ****************************************************************************************
 *
 * @file user_con_timeout.c
 *
 * @brief Source file containing the functionality to terminate a connection after a
 *				specific amount of time for a specific connection id. 
 *
 * Copyright (C) 2018-2021 Renesas Electronics Corporation and/or its affiliates
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
 
 #include "user_con_timeout.h"
 #include "app.h"							// dependencency on disconnection functionality
 #include "app_easy_timer.h"	// dependencency on timer functionality
 #include "systick.h"
 
 #ifdef CFG_PRINTF
		#include "arch_console.h"
 #endif
 
static struct con_timeout_t
{
	uint8_t connection_id;
	uint8_t timer_id;
	uint32_t delay;
	bool active;
}con_timeout = {0,0,0, false};
 
static void user_con_timeout_cb(void);
static bool user_con_timeout_is_active(void);

void user_con_timeout_init(uint32_t delay, uint8_t connection_id)
{
		//Stop timer if one is running
	  if (user_con_timeout_is_active())
		{
				app_easy_timer_cancel(con_timeout.timer_id);
		}
		
		con_timeout.timer_id = app_easy_timer(delay, user_con_timeout_cb);
		con_timeout.active = true;
		con_timeout.connection_id = connection_id;
		con_timeout.delay = delay;
}

void user_con_timeout_cancel(void)
{
		if(user_con_timeout_is_active())
		{
				app_easy_timer_cancel(con_timeout.timer_id);
				con_timeout.active = false;
		}
}

void user_con_timeout_reset(void)
{
		if (user_con_timeout_is_active())
		{
				user_con_timeout_cancel();
				user_con_timeout_init(con_timeout.delay, con_timeout.connection_id);
		}		
}

static void user_con_timeout_cb(void)
{
#ifdef CFG_PRINTF
		arch_printf("Timer with id 0x%x expired.\n\r Disconnecting...", con_timeout.timer_id);
#endif	
		app_easy_gap_disconnect(con_timeout.connection_id);
		con_timeout.active = false;
}

static bool user_con_timeout_is_active(void)
{
		return (con_timeout.active && con_timeout.timer_id);
}

