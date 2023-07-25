/**
 ****************************************************************************************
 *
 * @file user_con_timeout.h
 *
 * @brief API containing the functionality for connection timeout after (user)inactivity
 * 				to save power. This includes, initializing, starting, stopping and resetting
 *				the timeout. 
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
**/

#ifndef _USR_CON_TIMEOUT_H_
#define _USR_CON_TIMEOUT_H_

#include "stdint.h"

/**
 ****************************************************************************************
 * @brief 		Initialization procudure for the connection timeout. This must be called
 *						first if the other routines are to have any effect. If there is another 
 *						connection timeout present it will be overridden. 
 * @param[in]	uint32_t delay: The time (in 10ms) before a connection timeout will occur.
 *						uint_8t connection_id: id of the connection to be terminated. 
 * @return 		void
 ****************************************************************************************
*/	void user_con_timeout_init(uint32_t delay, uint8_t connection_id);


/**
 ****************************************************************************************
 * @brief 		Procedure to cancel the timer.
 ****************************************************************************************
*/	void user_con_timeout_cancel(void);

/**
 ****************************************************************************************
 * @brief 		Procedure to reset the timer. This can be used to postpone the time-out when 
 *						called after (user)activity. 
 * @return 		void
 ****************************************************************************************
*/	void user_con_timeout_reset(void);

#endif
