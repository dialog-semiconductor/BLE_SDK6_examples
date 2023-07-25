/**
 ****************************************************************************************
 *
 * \file user_modules.h
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

#ifndef _USER_MODULES_H_
#define _USER_MODULES_H_

/**
 ****************************************************************************************
 * \addtogroup USER
 * \{
 * \addtogroup USER_APP
 * \{
 * \addtogroup USER_MODULES
 *
 * \brief User modules implementation
 * \{
 ****************************************************************************************
 */

#include <user_modules_config.h>

/**
 ****************************************************************************************
 * \brief Reserves all the gpios registered by every user module
 ****************************************************************************************
 */
void user_modules_reserve_gpios(void);

/**
 ****************************************************************************************
 * \brief Initializes all the gpios registered by every user module
 ****************************************************************************************
 */
void user_modules_init_gpios(void);

/**
 ****************************************************************************************
 * \brief Initializes all the user modules
 ****************************************************************************************
 */
void user_modules_init(void);

/**
 ****************************************************************************************
 * \brief Calls all the "on_disconnect" user module registered functions
 ****************************************************************************************
 */
void user_modules_on_disconnect(void);

/**
 ****************************************************************************************
 * \brief Calls all the "on_ble_powered" user module registered functions
 ****************************************************************************************
 */
bool user_modules_on_ble_powered(void);

/**
 ****************************************************************************************
 * \brief Calls all the "on_system_powered" user module registered functions
 ****************************************************************************************
 */
uint8_t user_modules_on_system_powered(void);

/**
 * \}
 * \}
 * \}
 */

#endif // _USER_MODULES_H_
