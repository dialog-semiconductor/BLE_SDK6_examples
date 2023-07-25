/**
 ****************************************************************************************
 *
 * \file port_timer_defs.h
 *
 * \brief 
 *
 * Copyright (C) 2017-2021 Dialog Semiconductor
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
# OR OTHER DEALINGS IN THE SOFTWARE.
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

#ifndef PORT_TIMER_DEFS_H_
#define PORT_TIMER_DEFS_H_

#include <stdbool.h>
#include <stdint.h>
#include "rwble_config.h"
#include "ke_msg.h"

typedef ke_msg_id_t timer_id_t;
typedef void (*port_timer_handler_function_t)(void);

struct set_timer_msg {
    ke_msg_id_t id;
    uint32_t time;
};

#endif // PORT_TIMER_DEFS_H_

/**
 * \}
 * \}
 * \}
 */
