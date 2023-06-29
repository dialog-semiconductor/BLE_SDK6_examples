/**
 ****************************************************************************************
 *
 * @file user_button.h
 *
 * @brief Barebone application header file.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
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
 #define GOTO_SLEEP_TIME        300


#include "gpio.h" 

#ifdef ADV_BUTTON_EXAMPLE
/**
 ****************************************************************************************
 * @brief Enable the wakeup controller peripheral and initialize the wakeup callbacks
 * @param[in] Set the port of the pin that wakes up the device
 * @param[in] Set the pin that will wake up the device
 * @return void
 ****************************************************************************************
*/
void user_button_enable(GPIO_PORT gpio_port, GPIO_PIN gpio_pin);
/// @} APP

#endif //ADV_BUTTON_EXAMPLE

#endif //_USER_BAREBONE_H_