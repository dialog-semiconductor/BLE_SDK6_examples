/**
 ****************************************************************************************
 *
 * @file user_hibernation.h
 *
 * @brief Hibernation example application header file.
 *
 * Copyright (C) 2015-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _USER_PROXR_H_
#define _USER_PROXR_H_

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
	#define HIBERNATION_SPI
	#undef HIBERNATION_OTP
	#undef HIBERNATION_SYSRAM

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

#if defined (CFG_APP_GOTO_DEEP_SLEEP) && (defined (CFG_APP_GOTO_HIBERNATION) || defined (CFG_APP_GOTO_STATEFUL_HIBERNATION))
    #error "Config error: Can not define both CFG_APP_GOTO_DEEP_SLEEP and CFG_APP_GOTO_HIBERNATION/CFG_APP_GOTO_STATEFUL_HIBERNATION."
#endif

//#if defined (CFG_SPI_FLASH_ENABLE) && (defined (CFG_APP_GOTO_HIBERNATION) || defined (CFG_APP_GOTO_STATEFUL_HIBERNATION))
//    #error "Config error: Can not define both CFG_SPI_FLASH_ENABLE and CFG_APP_GOTO_HIBERNATION/CFG_APP_GOTO_STATEFUL_HIBERNATION."
//#endif


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

#if defined (CFG_EXT_SLEEP_WAKEUP_RTC) && defined (CFG_EXT_SLEEP_WAKEUP_TIMER1)
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

#if (BLE_SUOTA_RECEIVER)
/**
 ****************************************************************************************
 * @brief Function called when the SUOTAR status changes.
 * @param[in] suotar_event SUOTAR_START or SUOTAR_STOP
 ****************************************************************************************
 */
void on_suotar_status_change(const uint8_t suotar_event);
#endif // BLE_SUOTA_RECEIVER

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
