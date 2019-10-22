/**
 ****************************************************************************************
 *
 * @file syscntl.c
 *
 * @brief System controller driver.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "datasheet.h"
#include "syscntl.h"

#if defined (__DA14531__)
#include <stdbool.h>
#include "hw_otpc.h"
#include "arch.h"
#endif

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

#if defined (__DA14531__)
/// DCDC converter state
syscntl_dcdc_state_t syscntl_dcdc_state;
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void syscntl_use_lowest_amba_clocks(void)
{
    SetBits16(CLK_AMBA_REG, PCLK_DIV, 3);   // lowest is 250KHz (div 8, source is @2MHz)
    SetBits16(CLK_AMBA_REG, HCLK_DIV, 3);   // lowest is 2MHz (div 8, source is @16MHz)
}

void syscntl_use_highest_amba_clocks(void)
{
    SetBits16(CLK_AMBA_REG, PCLK_DIV, 0);
    SetBits16(CLK_AMBA_REG, HCLK_DIV, 0);
}

#if defined (__DA14531__)
void syscntl_cfg_xtal32m_amp_reg(xtal32m_amp_reg_t mode)
{
    SetBits16(XTAL32M_TESTCTRL0_REG, CORE_HOLD_AMP_REG_OVERRIDE, (uint8_t) mode);
}

void syscntl_por_vbat_high_cfg(por_vbat_mask_t mask)
{
    // Enable POR_VBAT_HIGH
    SetBits16(POWER_CTRL_REG, POR_VBAT_HIGH_DISABLE, 0);

    // Set POR_VBAT_HIGH mask mode
    SetBits(POWER_AON_CTRL_REG, POR_VBAT_HIGH_RST_MASK, mask);
}

void syscntl_por_vbat_low_cfg(por_vbat_mask_t mask)
{
    // Enable POR_VBAT_LOW
    SetBits16(POWER_CTRL_REG, POR_VBAT_LOW_DISABLE, 0);

    // Set POR_VBAT_LOW mask mode
    SetBits(POWER_AON_CTRL_REG, POR_VBAT_LOW_RST_MASK, mask);
}

/**
 ****************************************************************************************
 * @brief Configures and enables DCDC converter in the provided voltage level.
 * @param[in] level The voltage level of DCDC converter (nominal level + trim value).
 * @return void
 ****************************************************************************************
 */
#if (!USE_POWER_MODE_BYPASS)
static void dcdc_cfg(syscntl_dcdc_level_t level)
{
    // Set DCDC converter nominal voltage level
    SetBits16(POWER_LEVEL_REG, DCDC_LEVEL, level >> 3U);

    // Set DCDC converter trim value
    SetBits16(POWER_LEVEL_REG, DCDC_TRIM, level & 0x7);

    // Enable the DCDC converter
    SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 1);

    // Wait for the indication that VBAT_LOW (buck mode) or VBAT_HIGH (boost mode) is OK
    while (!GetBits(ANA_STATUS_REG, DCDC_OK));
}
#endif

syscntl_dcdc_level_t syscntl_dcdc_get_level(void)
{
    return ((syscntl_dcdc_level_t) ((GetBits16(POWER_LEVEL_REG, DCDC_LEVEL) << 3U) + GetBits16(POWER_LEVEL_REG, DCDC_TRIM)));
}

void syscntl_dcdc_set_level(syscntl_dcdc_level_t level)
{
    // Set DCDC converter nominal voltage level
    SetBits16(POWER_LEVEL_REG, DCDC_LEVEL, level >> 3U);

    // Set DCDC converter trim value
    SetBits16(POWER_LEVEL_REG, DCDC_TRIM, level & 0x7);
}

int8_t syscntl_dcdc_turn_off(void)
{
#if (USE_POWER_MODE_BYPASS)
    return SYSCNTL_DCDC_ERR_IN_BYPASS;
#else
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
        if (hw_otpc_is_dcdc_reserved())
        {
            return SYSCNTL_DCDC_ERR_RESERVED_BY_OTP;
        }
        if ((GetBits16(POWER_CTRL_REG, POR_VBAT_HIGH_DISABLE) == 0x0) &&
            (GetBits16(POWER_AON_CTRL_REG, POR_VBAT_HIGH_RST_MASK) == 0x0))
        {
            return SYSCNTL_DCDC_ERR_POR_TRIGGER;
        }
    }
    else
    {
        // Enable LDO_LOW
        SetBits16(POWER_CTRL_REG, LDO_LOW_CTRL_REG, 2);
    }

    // Disable the DCDC
    SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 0);

    return SYSCNTL_DCDC_ERR_NO_ERROR;
#endif
}

int8_t syscntl_dcdc_turn_on_in_boost(syscntl_dcdc_level_t level)
{
#if (USE_POWER_MODE_BYPASS)
    return SYSCNTL_DCDC_ERR_IN_BYPASS;
#else
    if (!GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
        return SYSCNTL_DCDC_ERR_NOT_IN_BOOST;
    }
    if (hw_otpc_is_dcdc_reserved() && (level < syscntl_dcdc_get_level()))
    {
        return SYSCNTL_DCDC_ERR_RESERVED_BY_OTP;
    }
    // POR is generated if VBAT_HIGH is below 1.66V
    if ((GetBits16(POWER_CTRL_REG, POR_VBAT_HIGH_DISABLE) == 0x0) &&
        (GetBits16(POWER_AON_CTRL_REG, POR_VBAT_HIGH_RST_MASK) == 0x0) &&
        (level < SYSCNTL_DCDC_LEVEL_1V725))
    {
        return SYSCNTL_DCDC_WARNING_POR_TRIGGER;
    }

    syscntl_dcdc_state.level = level;
    syscntl_dcdc_state.status = true;
    dcdc_cfg(level);

    return SYSCNTL_DCDC_ERR_NO_ERROR;
#endif
}

int8_t syscntl_dcdc_turn_on_in_buck(syscntl_dcdc_level_t level)
{
#if (USE_POWER_MODE_BYPASS)
    return SYSCNTL_DCDC_ERR_IN_BYPASS;
#else
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
        return SYSCNTL_DCDC_ERR_NOT_IN_BUCK;
    }

    dcdc_cfg(level);

    // Disable LDO_LOW
    SetBits16(POWER_CTRL_REG, LDO_LOW_CTRL_REG, 1);

    return SYSCNTL_DCDC_ERR_NO_ERROR;
#endif
}

#else
void syscntl_set_dcdc_vbat3v_level(enum SYSCNTL_DCDC_VBAT3V_LEVEL level)
{
    SetBits16(DCDC_CTRL2_REG, DCDC_VBAT3V_LEV, level);
}
#endif
