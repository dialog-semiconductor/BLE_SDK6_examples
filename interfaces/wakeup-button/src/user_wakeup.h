/**
 ****************************************************************************************
 *
 * @file user_wakeup.h
 *
 * @brief Barebone application header file.
 *
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
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
#define EVENTS_BEFORE_INTERRUPT         1  // Must be a 8 bits value

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

void user_wakeup_example_init(void);

void user_app_wakeup_press_cb(void);

/// @} APP

#endif //_USER_WAKEUP_H_