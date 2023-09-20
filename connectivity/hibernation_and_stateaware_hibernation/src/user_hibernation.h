/**
 ****************************************************************************************
 *
 * @file user_hibernation.h
 *
 * @brief Hibernation example application header file.
 *
 * Copyright (C) 2015-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef _USER_PROXR_H_
#define _USER_PROXR_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "gapc_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/****************************************************************************************
* Selects the sleep mode that the device will enter after advertising completion        *
*  																																                      *
*  - CFG_APP_GOTO_HIBERNATION       Hibernation mode (DA14531 only)                     *
*  - CFG_APP_GOTO_STATEFUL_HIBERNATION   Stateful Hibernation mode (DA14531 only)       *
* Note: If none is defined, then system will enter the selected Extended sleep mode     *
*                                                                                       *
* IMPORTANT: If CFG_APP_GOTO_STATEFUL_HIBERNATION is defined, CFG_STATEFUL_HIBERNATION  *
* shall be defined as well (in Options for Target 'DA14531' --> Asm -->                 *
* Conditional Assembly Control Symbols --> Define:)                                     *
****************************************************************************************/

#if defined (__DA14531__)
#define CFG_APP_GOTO_HIBERNATION
#undef CFG_APP_GOTO_STATEFUL_HIBERNATION

#if defined (CFG_APP_GOTO_HIBERNATION)
	#undef HIBERNATION_SPI
	#undef HIBERNATION_OTP
	#define HIBERNATION_SYSRAM

#if defined (HIBERNATION_SPI)
/****************************************************************************************
 * Hibernation mode configuration                                                       *
 ****************************************************************************************/
#define CFG_HIBERNATION_RAM1            PD_SYS_DOWN_RAM_OFF
#define CFG_HIBERNATION_RAM2            PD_SYS_DOWN_RAM_OFF
#define CFG_HIBERNATION_RAM3            PD_SYS_DOWN_RAM_OFF
#define CFG_HIBERNATION_REMAP           REMAP_ADDR0_TO_ROM
#define CFG_HIBERNATION_PAD_LATCH_EN    false
#endif

#if defined (HIBERNATION_OTP)
/****************************************************************************************
 * Hibernation mode configuration                                                       *
 ****************************************************************************************/
#define CFG_HIBERNATION_RAM1            PD_SYS_DOWN_RAM_OFF
#define CFG_HIBERNATION_RAM2            PD_SYS_DOWN_RAM_OFF
#define CFG_HIBERNATION_RAM3            PD_SYS_DOWN_RAM_OFF
#define CFG_HIBERNATION_REMAP           REMAP_ADDR0_TO_OTP
#define CFG_HIBERNATION_PAD_LATCH_EN    false
#endif

#if defined (HIBERNATION_SYSRAM)
/****************************************************************************************
 * Hibernation mode configuration                                                       *
 ****************************************************************************************/
#define CFG_HIBERNATION_RAM1            PD_SYS_DOWN_RAM_ON
#define CFG_HIBERNATION_RAM2            PD_SYS_DOWN_RAM_ON
#define CFG_HIBERNATION_RAM3            PD_SYS_DOWN_RAM_ON
#define CFG_HIBERNATION_REMAP           REMAP_ADDR0_TO_RAM1
#define CFG_HIBERNATION_PAD_LATCH_EN    false
#endif

#endif // CFG_APP_GOTO_HIBERNATION


#if defined (CFG_APP_GOTO_STATEFUL_HIBERNATION)
/****************************************************************************************
 * Stateful Hibernation mode configuration                                              *
 ****************************************************************************************/
#define CFG_STATEFUL_HIBERNATION_RAM1           PD_SYS_DOWN_RAM_ON
#define CFG_STATEFUL_HIBERNATION_RAM2           PD_SYS_DOWN_RAM_ON
#define CFG_STATEFUL_HIBERNATION_RAM3           PD_SYS_DOWN_RAM_ON
#define CFG_STATEFUL_HIBERNATION_REMAP          STATEFUL_HIBERN_REMAP_ADDR0_TO_RAM1
#define CFG_STATEFUL_HIBERNATION_PAD_LATCH_EN   false
#endif //CFG_APP_GOTO_STATEFUL_HIBERNATION

#endif // __DA14531__

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Enable push button. Register callback function for button press event.
 *        Must be called in periph_init().
 ****************************************************************************************
 */
void app_button_enable(void);

/**
 ****************************************************************************************
 * @brief Function to be called on the advertising completion event.
 * @param[in] status GAP Error code
 ****************************************************************************************
 */
void app_advertise_complete(const uint8_t status);

/**
 ****************************************************************************************
 * @brief Disconnection function.
 * @param[in] param         Pointer to GAPC_DISCONNECT_IND message
 ****************************************************************************************
*/
void user_app_on_disconnect(struct gapc_disconnect_ind const *param);

void user_app_on_init(void);

/// @} APP

#endif // _USER_PROXR_H_
