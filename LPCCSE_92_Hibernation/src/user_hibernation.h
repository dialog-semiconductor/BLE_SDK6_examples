/**
 ****************************************************************************************
 *
 * @file user_hibernation.h
 *
 * @brief Hibernation demonstration application header file.
 *
 * Copyright (c) 2015-2020 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#ifndef _USER_HIBERNATION_H_
#define _USER_HIBERNATION_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief User Application entry point.
 *
 * @{
 ****************************************************************************************
 */

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
* Selects the sleep mode that the device will enter after advetising completion         *
*  - CFG_APP_GOTO_DEEP_SLEEP        Deep Sleep mode                                     *
*  - CFG_APP_GOTO_HIBERNATION       Hibernation mode (DA14531 only)                     *
* Note: If none is defined, then system will enter the selected Extended sleep mode     *
****************************************************************************************/
#undef CFG_APP_GOTO_DEEP_SLEEP

#if defined (__DA14531__)
#define CFG_APP_GOTO_HIBERNATION

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



#endif


#if defined(CFG_APP_GOTO_DEEP_SLEEP) && defined(CFG_APP_GOTO_HIBERNATION)
    #error "Config error: Can not define both CFG_APP_GOTO_DEEP_SLEEP and CFG_APP_GOTO_HIBERNATION."
#endif

//#if defined(CFG_SPI_FLASH_ENABLE) && defined(CFG_APP_GOTO_HIBERNATION)
//    #error "Config error: Can not define both CFG_SPI_FLASH_ENABLE and CFG_APP_GOTO_HIBERNATION."
//#endif

#if defined(CFG_APP_GOTO_DEEP_SLEEP)
/****************************************************************************************
* Deep Sleep mode and Wake-up from Deep Sleep Configuration                             *
* Selects the trigger mechanism to wake up from deep sleep.                             *
*                                                                                       *
* - CFG_DEEP_SLEEP_WAKEUP_POR       - Wake up from POR pin                              *
*                                                                                       *
* - CFG_DEEP_SLEEP_WAKEUP_GPIO      - Wake up from WakeUp Controller                    *
*                                                                                       *
* - CFG_DEEP_SLEEP_WAKEUP_RTC       - Wake up from RTC Timer (only in DA14531)          *
*                                                                                       *
* - CFG_DEEP_SLEEP_WAKEUP_TIMER1    - Wake up from Timer1 (only in DA14531)             *
*                                                                                       *
* NOTE:                                                                                 *
* A hardware reset or power cycle will always wake up the system from deep sleep.       *
****************************************************************************************/
#undef CFG_DEEP_SLEEP_WAKEUP_POR
#define CFG_DEEP_SLEEP_WAKEUP_GPIO

/****************************************************************************************
 * Deep sleep mode configuration                                                        *
 ****************************************************************************************/
#define CFG_DEEP_SLEEP_RAM1            PD_SYS_DOWN_RAM_OFF
#define CFG_DEEP_SLEEP_RAM2            PD_SYS_DOWN_RAM_OFF
#define CFG_DEEP_SLEEP_RAM3            PD_SYS_DOWN_RAM_OFF
#define CFG_DEEP_SLEEP_PAD_LATCH_EN    false

#if defined (__DA14531__)
#undef CFG_DEEP_SLEEP_WAKEUP_RTC
#undef CFG_DEEP_SLEEP_WAKEUP_TIMER1
#endif

#if defined(CFG_DEEP_SLEEP_WAKEUP_POR) && defined(CFG_DEEP_SLEEP_WAKEUP_GPIO)
    #error "Config error: Can not define both CFG_DEEP_SLEEP_WAKEUP_POR and CFG_DEEP_SLEEP_WAKEUP_GPIO."
#endif

#if !defined(CFG_DEEP_SLEEP_WAKEUP_POR) && !defined(CFG_DEEP_SLEEP_WAKEUP_GPIO) && \
    !defined(CFG_DEEP_SLEEP_WAKEUP_RTC) && !defined(CFG_DEEP_SLEEP_WAKEUP_TIMER1)
    #error "Config error: At least one wake-up source has to be selected."
#endif

#endif

/****************************************************************************************
* Wake-up from Extended Sleep Configuration                                             *
* Selects the trigger mechanism to wake up from extended sleep.                         *
*                                                                                       *
* - CFG_EXT_SLEEP_WAKEUP_RTC       - Wake up from RTC Timer (only in DA14531)           *
*                                                                                       *
* - CFG_EXT_SLEEP_WAKEUP_TIMER1    - Wake up from Timer1 (only in DA14531)              *
*                                                                                       *
* NOTE:                                                                                 *
* If none is defined, then system will wake up from extended sleep by button press.     *
****************************************************************************************/
#undef CFG_EXT_SLEEP_WAKEUP_RTC
#undef CFG_EXT_SLEEP_WAKEUP_TIMER1

#if defined(CFG_EXT_SLEEP_WAKEUP_RTC) && defined(CFG_EXT_SLEEP_WAKEUP_TIMER1)
    #error "Config error: Can not define both CFG_EXT_SLEEP_WAKEUP_RTC and CFG_EXT_SLEEP_WAKEUP_TIMER1."
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Enable push button. Register callback function for button press event.
 *        Must be called in periph_init().
 * @return void
 ****************************************************************************************
 */
void app_button_enable(void);

/**
 ****************************************************************************************
 * @brief Function to be called on the advertising completion event.
 * @param[in] status GAP Error code
 * @return void
 ****************************************************************************************
 */
void app_advertise_complete(const uint8_t status);

#if (BLE_SUOTA_RECEIVER)
/**
 ****************************************************************************************
 * @brief Function called when the SUOTAR status changes.
 * @param[in] suotar_event SUOTAR_START or SUOTAR_STOP
 * @return void
 ****************************************************************************************
 */
void on_suotar_status_change(const uint8_t suotar_event);
#endif // BLE_SUOTA_RECEIVER

/**
 ****************************************************************************************
 * @brief Disconnection function.
 * @param[in] param         Pointer to GAPC_DISCONNECT_IND message
 * @return void
 ****************************************************************************************
*/
void user_app_on_disconnect(struct gapc_disconnect_ind const *param);

/// @} APP

#endif // _USER_PROXR_H_
