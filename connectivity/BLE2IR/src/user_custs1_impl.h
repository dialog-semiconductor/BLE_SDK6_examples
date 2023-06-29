/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.h
 *
 * @brief Peripheral project Custom1 Server implementation header file.
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

#ifndef _USER_CUSTS1_IMPL_H_
#define _USER_CUSTS1_IMPL_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief
 *
 * @{
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

enum
{
    CUSTS1_CP_ADC_VAL1_DISABLE = 0,
    CUSTS1_CP_ADC_VAL1_ENABLE,
};

enum
{
    CUSTS1_LED_OFF = 0,
    CUSTS1_LED_ON,
};

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gapc_task.h"                 // gap functions and messages
#include "gapm_task.h"                 // gap functions and messages
#include "custs1_task.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */													 

/**
 ****************************************************************************************
 * @brief 		send notification containing kbd_value
 * @param[in] uint32_t kbd_value: Keyboard value
 * @return 		void
 ****************************************************************************************
 */	void user_svc1_kbd_ntf(uint32_t kbd_value);																				 

/// @} APP

#endif // _USER_CUSTS1_IMPL_H_