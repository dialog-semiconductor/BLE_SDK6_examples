/**
 ****************************************************************************************
 *
 * @file arch_wdg.h
 *
 * @brief Watchdog helper functions.
 *
 * Copyright (c) 2012-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef _ARCH_WDG_H_
#define _ARCH_WDG_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "datasheet.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define     WATCHDOG_DEFAULT_PERIOD 0xC8

/// Watchdog enable/disable
#ifndef USE_WDOG
#if defined(CFG_WDOG) 
#define USE_WDOG            1
#else
#define USE_WDOG            0
#endif //CFG_WDOG
#endif

/**
 ****************************************************************************************
 * @brief     Watchdog resume.
 * @return void
 * About: Start the Watchdog
 ****************************************************************************************
 */
__STATIC_INLINE void  wdg_resume(void)
{
#if (USE_WDOG)    
    // Start WDOG
    SetWord16(RESET_FREEZE_REG, FRZ_WDOG);  
#endif
}

/**
****************************************************************************************
* @brief     Freeze Watchdog. Call wdg_resume() to resume operation.
* @return void
* About: Freeze the Watchdog
****************************************************************************************
*/
__STATIC_INLINE void  wdg_freeze(void)
{
    // Freeze WDOG   
    SetWord16(SET_FREEZE_REG, FRZ_WDOG);  
}

/**
 ****************************************************************************************
 * @brief     Watchdog reload.
 * @param[in] period measured in 10.24ms units.
 * @return void
 * About: Load the default value and resume the watchdog
 ****************************************************************************************
 */
__STATIC_INLINE void  wdg_reload(const int period)
{
#if (USE_WDOG)    
    // WATCHDOG_DEFAULT_PERIOD * 10.24ms 
    SetWord16(WATCHDOG_REG, WATCHDOG_DEFAULT_PERIOD);  
#endif
}

/**
 ****************************************************************************************
 * @brief     Initiliaze and start the Watchdog unit.
 * @param[in] If 0 Watchdog is SW freeze capable 
 * @return void
 * About: Enable the Watchdog and configure it to 
 ****************************************************************************************
 */
 __STATIC_INLINE void  wdg_init (const int freeze)
{
#if (USE_WDOG)
    wdg_reload(WATCHDOG_DEFAULT_PERIOD);
    // if freeze equals to zero WDOG can be frozen by SW!
    // it will generate an NMI when counter reaches 0 and a WDOG (SYS) Reset when it reaches -16!
    SetWord16(WATCHDOG_CTRL_REG, (freeze&0x1)); 
    wdg_resume ();
#else
    wdg_freeze();
#endif
}

#endif // _ARCH_WDG_H_
