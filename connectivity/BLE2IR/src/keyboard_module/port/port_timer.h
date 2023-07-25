/**
 ****************************************************************************************
 *
 * \file port_timer.h
 *
 * \brief timer functions
 *
 * Copyright (C) 2017-2021 Dialog Semiconductor
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */
 
/**
 ****************************************************************************************
 * \addtogroup APP_UTILS
 * \{
 * \addtogroup TIMER
 * \{
 * \addtogroup PORT_TIMER
 * \{
 ****************************************************************************************	 
 */ 
 
#ifndef PORT_TIMER_H_
#define PORT_TIMER_H_

#include <port_timer_config.h>
#include "port_platform.h"

/**
 ****************************************************************************************
 * \brief      Sets a timer to start counting
 *
 * \param[in]  timer_id     The timer's ID  
 * \param[in]  task_handler The timer's handling task
 * \param[in]  delay        The timer's delay in ms
 ****************************************************************************************
 */
void port_timer_set(timer_id_t const timer_id, task_id_t task_handler, uint32_t delay);

/**
 ****************************************************************************************
 * \brief      Clears a counting timer
 *
 * \param[in]  timer_id     The timer's ID  
 * \param[in]  task_handler The timer's handling task
 ****************************************************************************************
 */
void port_timer_clear(timer_id_t const timer_id, task_id_t task_handler);

 /**
  ****************************************************************************************
  * \brief      Clears an existing timer and returns its current counting value
  *
  * \param[in]  timer_id        The timer's ID  
  * \param[in]  task_handler    The timer's handling task
  *
  * \return     int             The timer's counting value
  ****************************************************************************************
  */
int port_timer_remove(timer_id_t const timer_id, task_id_t task_handler);

#endif // PORT_TIMER_H_

/**
 * \}
 * \}
 * \}
 */
