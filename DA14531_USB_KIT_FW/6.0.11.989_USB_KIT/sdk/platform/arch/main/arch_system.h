/**
 ****************************************************************************************
 *
 * @file arch_system.h
 *
 * @brief Architecture System calls
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

#ifndef _ARCH_SYSTEM_H_
#define _ARCH_SYSTEM_H_

#include <stdbool.h>   // boolean definition
#include "datasheet.h"
#include "arch.h"
#include "arch_api.h"

#if !defined (__NON_BLE_EXAMPLE__)
/*
 * DEFINES
 ****************************************************************************************
 */
// RCX calibration time in RCX cycles
#if defined (__DA14531__)
    #define RCX_CALIB_TIME      30
#else
    #define RCX_CALIB_TIME      20
#endif

extern uint32_t lp_clk_sel;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Check if RCX is selected as the low power clock.
 * @return true if RCX is selected, otherwise false
 ****************************************************************************************
 */
__STATIC_FORCEINLINE bool arch_clk_is_RCX20(void)
{
    return (((lp_clk_sel == LP_CLK_RCX20) && (CFG_LP_CLK == LP_CLK_FROM_OTP)) || (CFG_LP_CLK == LP_CLK_RCX20));
}

/**
 ****************************************************************************************
 * @brief Check if XTAL32 is selected as the low power clock.
 * @return true if XTAL32 is selected, otherwise false
 ****************************************************************************************
 */
__STATIC_FORCEINLINE bool arch_clk_is_XTAL32(void)
{
    return (((lp_clk_sel == LP_CLK_XTAL32) && (CFG_LP_CLK == LP_CLK_FROM_OTP)) || (CFG_LP_CLK == LP_CLK_XTAL32));
}

/**
 ****************************************************************************************
 * @brief Calibrate the RCX20 clock if selected from the user.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rcx20_calibrate(void)
{
    if (arch_clk_is_RCX20())
    {
        calibrate_rcx20(RCX_CALIB_TIME);
#if defined (__DA14531__)
        read_rcx_freq(true);
#else
        read_rcx_freq(RCX_CALIB_TIME);
#endif
    }
}

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Read RCX frequency value.
 * @param[in] wait_for_calibration  If true, function waits for calibration to complete,
 *                                  else it returns immediately if calibration is in
 *                                  progress.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rcx20_read_freq(bool wait_for_calibration)
{
    if (arch_clk_is_RCX20())
    {
        // RCX is running at 14.5KHz so more cycles are required for the same accuracy
        read_rcx_freq(wait_for_calibration);
    }
}
#else
/**
 ****************************************************************************************
 * @brief Read RCX frequency value.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void rcx20_read_freq(void)
{
    if (arch_clk_is_RCX20())
    {
        read_rcx_freq(RCX_CALIB_TIME);
    }
}
#endif

/**
 ****************************************************************************************
 * @brief Tweak wakeup timer debouncing time. It corrects the debouncing time when RCX is
 * selected as the clock in sleep mode. Due to a silicon bug the wakeup timer debouncing
 * time is multiplied by ~3 when the system is clocked by RCX in sleep mode.
 * @note This function SHOULD BE called when the system is ready to enter sleep
 * (tweak = true) to load the WKUP_CTRL_REG[WKUP_DEB_VALUE] with the recalculated
 * debouncing time value or when it comes out of sleep mode (tweak = false) to reload the
 * WKUP_CTRL_REG[WKUP_DEB_VALUE] with the initial programmed debouncing time value (stored
 * in the retention memory). The above software fix does not provide full guarantee that
 * it will remedy the silicon bug under all circumstances.
 * @param[in] tweak If true the debouncing time will be corrected, otherwise the initial
 *                  programmed debouncing time, stored in the retention memory, will be
 *                  set
 * @return void
 ****************************************************************************************
 */
void arch_wkupct_tweak_deb_time(bool tweak);

#if !defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Estimate the RC16M frequency.
 * @return The number of XTAL16M clock cycles during the calibration time.
 *****************************************************************************************
 */
uint16_t get_rc16m_count(void);
#endif

/**
 ****************************************************************************************
 * @brief Conditionally run radio calibration.
 * @return void
 *****************************************************************************************
 */
void conditionally_run_radio_cals(void);
#endif // (__NON_BLE_EXAMPLE__)

#if !defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Sets TRIM_TIME and SETTLE_TIME trim values and bias current for the XTAL16M.
 *        - Trim values are set to 0 if power optimizations are used, since they are handled
 *          differently. Else, they are set to the value defined in XTAL16_TRIM_DELAY_SETTING.
 *        - Bias current is set to the Reset Value: 0x05.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void xtal16m_trim_init(void)
{
#if (USE_POWER_OPTIMIZATIONS)
    SetWord16(TRIM_CTRL_REG, 0x00); // ((0x0 + 1) + (0x0 + 1)) * 250usec but settling time is controlled differently
#else
    SetWord16(TRIM_CTRL_REG, XTAL16M_TRIM_DELAY_SETTING); // ((0xA + 1) + (0x2 + 1)) * 250usec settling time
#endif
    SetBits16(CLK_16M_REG, XTAL16_CUR_SET, 0x5);
}
#endif

/**
 ****************************************************************************************
 * @brief  Global System Init
 * @return void
 ****************************************************************************************
 */
void system_init(void);

#endif

/// @}
