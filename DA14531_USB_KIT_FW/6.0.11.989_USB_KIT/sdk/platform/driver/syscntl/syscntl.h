/**
 ****************************************************************************************
 *
 * @file syscntl.h
 *
 * @brief System controller driver header file.
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

#ifndef SYSCNTL_H_
#define SYSCNTL_H_

#include "datasheet.h"

#if defined (__DA14531__)
#include <stdbool.h>
#include "arch.h"
#endif

/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */

#if defined (__DA14531__)

/* Error Codes */
#define SYSCNTL_DCDC_ERR_NO_ERROR               (0)
#define SYSCNTL_DCDC_ERR_RESERVED_BY_OTP        (-1)
#define SYSCNTL_DCDC_ERR_NOT_IN_BOOST           (-2)
#define SYSCNTL_DCDC_ERR_NOT_IN_BUCK            (-3)
#define SYSCNTL_DCDC_ERR_POR_TRIGGER            (-4)
#define SYSCNTL_DCDC_WARNING_POR_TRIGGER        (-5)
#define SYSCNTL_DCDC_ERR_IN_BYPASS              (-6)

/// DCDC converter nominal voltage levels
enum {
    /// DCDC nominal voltage level 1.1V
    SYSCNTL_DCDC_NOMINAL_LEVEL_1V1      = 0U,

    /// DCDC nominal voltage level 1.8V
    SYSCNTL_DCDC_NOMINAL_LEVEL_1V8      = 1U,

    /// DCDC nominal voltage level 2.5V
    SYSCNTL_DCDC_NOMINAL_LEVEL_2V5      = 2U,

    /// DCDC nominal voltage level 3.0V
    SYSCNTL_DCDC_NOMINAL_LEVEL_3V0      = 3U,
};

/// DCDC converter trim values
enum {
    /// DCDC trim -75mV
    SYSCNTL_DCDC_TRIM_N_75mV      = 0U,

    /// DCDC trim -50mV
    SYSCNTL_DCDC_TRIM_N_50mV      = 1U,

    /// DCDC trim -25mV
    SYSCNTL_DCDC_TRIM_N_25mV      = 2U,

    /// DCDC trim 0mV
    SYSCNTL_DCDC_TRIM_0mV         = 3U,

    /// DCDC trim 25mV
    SYSCNTL_DCDC_TRIM_P_25mV      = 4U,

    /// DCDC trim 50mV
    SYSCNTL_DCDC_TRIM_P_50mV      = 5U,

    /// DCDC trim 75mV
    SYSCNTL_DCDC_TRIM_P_75mV      = 6U,

    /// DCDC trim 100mV
    SYSCNTL_DCDC_TRIM_P_100mV     = 7U,
};

/// Defines the DCDC converter voltage levels (nominal voltage + trim)
typedef enum {
    /// DCDC voltage level 1.025V
    SYSCNTL_DCDC_LEVEL_1V025     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V1 << 3U) + SYSCNTL_DCDC_TRIM_N_75mV),

    /// DCDC voltage level 1.05V
    SYSCNTL_DCDC_LEVEL_1V05      = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V1 << 3U) + SYSCNTL_DCDC_TRIM_N_50mV),

    /// DCDC voltage level 1.075V
    SYSCNTL_DCDC_LEVEL_1V075     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V1 << 3U) + SYSCNTL_DCDC_TRIM_N_25mV),

    /// DCDC voltage level 1.1V
    SYSCNTL_DCDC_LEVEL_1V1       = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V1 << 3U) + SYSCNTL_DCDC_TRIM_0mV),

    /// DCDC voltage level 1.125V
    SYSCNTL_DCDC_LEVEL_1V125     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V1 << 3U) + SYSCNTL_DCDC_TRIM_P_25mV),

    /// DCDC voltage level 1.150V
    SYSCNTL_DCDC_LEVEL_1V150     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V1 << 3U) + SYSCNTL_DCDC_TRIM_P_50mV),

    /// DCDC voltage level 1.175V
    SYSCNTL_DCDC_LEVEL_1V175     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V1 << 3U) + SYSCNTL_DCDC_TRIM_P_75mV),

    /// DCDC voltage level 1.2V
    SYSCNTL_DCDC_LEVEL_1V2       = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V1 << 3U) + SYSCNTL_DCDC_TRIM_P_100mV),


    /// DCDC voltage level 1.725V
    SYSCNTL_DCDC_LEVEL_1V725     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V8 << 3U) + SYSCNTL_DCDC_TRIM_N_75mV),

    /// DCDC voltage level 1.75V
    SYSCNTL_DCDC_LEVEL_1V75      = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V8 << 3U) + SYSCNTL_DCDC_TRIM_N_50mV),

    /// DCDC voltage level 1.775V
    SYSCNTL_DCDC_LEVEL_1V775     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V8 << 3U) + SYSCNTL_DCDC_TRIM_N_25mV),

    /// DCDC voltage level 1.8V
    SYSCNTL_DCDC_LEVEL_1V8       = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V8 << 3U) + SYSCNTL_DCDC_TRIM_0mV),

    /// DCDC voltage level 1.825V
    SYSCNTL_DCDC_LEVEL_1V825     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V8 << 3U) + SYSCNTL_DCDC_TRIM_P_25mV),

    /// DCDC voltage level 1.850V
    SYSCNTL_DCDC_LEVEL_1V850     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V8 << 3U) + SYSCNTL_DCDC_TRIM_P_50mV),

    /// DCDC voltage level 1.875V
    SYSCNTL_DCDC_LEVEL_1V875     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V8 << 3U) + SYSCNTL_DCDC_TRIM_P_75mV),

    /// DCDC voltage level 1.9V
    SYSCNTL_DCDC_LEVEL_1V9       = ((SYSCNTL_DCDC_NOMINAL_LEVEL_1V8 << 3U) + SYSCNTL_DCDC_TRIM_P_100mV),


    /// DCDC voltage level 2.4025V
    SYSCNTL_DCDC_LEVEL_2V425     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_2V5 << 3U) + SYSCNTL_DCDC_TRIM_N_75mV),

    /// DCDC voltage level 2.45V
    SYSCNTL_DCDC_LEVEL_2V45      = ((SYSCNTL_DCDC_NOMINAL_LEVEL_2V5 << 3U) + SYSCNTL_DCDC_TRIM_N_50mV),

    /// DCDC voltage level 2.475V
    SYSCNTL_DCDC_LEVEL_2V475     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_2V5 << 3U) + SYSCNTL_DCDC_TRIM_N_25mV),

    /// DCDC voltage level 2.5V
    SYSCNTL_DCDC_LEVEL_2V5       = ((SYSCNTL_DCDC_NOMINAL_LEVEL_2V5 << 3U) + SYSCNTL_DCDC_TRIM_0mV),

    /// DCDC voltage level 2.525V
    SYSCNTL_DCDC_LEVEL_2V525     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_2V5 << 3U) + SYSCNTL_DCDC_TRIM_P_25mV),

    /// DCDC voltage level 2.550V
    SYSCNTL_DCDC_LEVEL_2V550     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_2V5 << 3U) + SYSCNTL_DCDC_TRIM_P_50mV),

    /// DCDC voltage level 2.575V
    SYSCNTL_DCDC_LEVEL_2V575     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_2V5 << 3U) + SYSCNTL_DCDC_TRIM_P_75mV),

    /// DCDC voltage level 2.6V
    SYSCNTL_DCDC_LEVEL_2V6       = ((SYSCNTL_DCDC_NOMINAL_LEVEL_2V5 << 3U) + SYSCNTL_DCDC_TRIM_P_100mV),


    /// DCDC voltage level 2.925V
    SYSCNTL_DCDC_LEVEL_2V925     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_3V0 << 3U) + SYSCNTL_DCDC_TRIM_N_75mV),

    /// DCDC voltage level 2.95V
    SYSCNTL_DCDC_LEVEL_2V95      = ((SYSCNTL_DCDC_NOMINAL_LEVEL_3V0 << 3U) + SYSCNTL_DCDC_TRIM_N_50mV),

    /// DCDC voltage level 2.975V
    SYSCNTL_DCDC_LEVEL_2V975     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_3V0 << 3U) + SYSCNTL_DCDC_TRIM_N_25mV),

    /// DCDC voltage level 3.0V
    SYSCNTL_DCDC_LEVEL_3V0       = ((SYSCNTL_DCDC_NOMINAL_LEVEL_3V0 << 3U) + SYSCNTL_DCDC_TRIM_0mV),

    /// DCDC voltage level 3.025V
    SYSCNTL_DCDC_LEVEL_3V025     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_3V0 << 3U) + SYSCNTL_DCDC_TRIM_P_25mV),

    /// DCDC voltage level 3.050V
    SYSCNTL_DCDC_LEVEL_3V050     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_3V0 << 3U) + SYSCNTL_DCDC_TRIM_P_50mV),

    /// DCDC voltage level 3.075V
    SYSCNTL_DCDC_LEVEL_3V075     = ((SYSCNTL_DCDC_NOMINAL_LEVEL_3V0 << 3U) + SYSCNTL_DCDC_TRIM_P_75mV),

    /// DCDC voltage level 3.1V
    SYSCNTL_DCDC_LEVEL_3V1       = ((SYSCNTL_DCDC_NOMINAL_LEVEL_3V0 << 3U) + SYSCNTL_DCDC_TRIM_P_100mV),
} syscntl_dcdc_level_t;

typedef struct
{
    syscntl_dcdc_level_t level;
    bool status;
}syscntl_dcdc_state_t;

/// Defines the XTAL32M amplitude regulator mode
typedef enum {
    /// Normal mode (following radio enable)
    XTAL32M_AMP_REG_NORMAL      = 0,

    /// Continues tracking mode
    XTAL32M_AMP_REG_TRACKING    = 1,

    /// Hold mode
    XTAL32M_AMP_REG_HOLD        = 2,
} xtal32m_amp_reg_t;

/// Defines the POR on VBAT HIGH/LOW Mask mode
typedef enum {
    /// Mask OFF
    POR_VBAT_MASK_OFF           = 0,

    /// Mask ON
    POR_VBAT_MASK_ON            = 1,
} por_vbat_mask_t;
#else
/// Nominal VBAT3V output voltage of the boost converter
enum SYSCNTL_DCDC_VBAT3V_LEVEL
{
    SYSCNTL_DCDC_VBAT3V_LEVEL_2V4    = 4, // 2.4V
    SYSCNTL_DCDC_VBAT3V_LEVEL_2V5    = 5, // 2.5V
    SYSCNTL_DCDC_VBAT3V_LEVEL_2V62   = 6, // 2.62V
    SYSCNTL_DCDC_VBAT3V_LEVEL_2V76   = 7, // 2.76V
};
#endif

/*
 * DEFINITIONS
 *****************************************************************************************
 */

#if defined (__DA14531__)
#define CLK_AMBA_REG_RSV3_RSV2       (0x000C)
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Sets AMBA clocks to minimum frequency
 * @return void
 ****************************************************************************************
 */
void syscntl_use_lowest_amba_clocks(void);

/**
 ****************************************************************************************
 * @brief Sets AMBA clocks to maximum frequency.
 * @return void
 ****************************************************************************************
 */
void syscntl_use_highest_amba_clocks(void);

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Store debugger configuration to CLK_AMBA_REG reserved bits 3 and 2.
 * @note DA1585/586: Dedicated pins are used for the SWD interface.
 *       DA14531:    The SWD interface pins are configurable.
 * @param[in] cfg    The debugger pins configurarion
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void syscntl_store_debugger_cfg(uint8_t cfg)
{
    SetBits16(CLK_AMBA_REG, CLK_AMBA_REG_RSV3_RSV2, cfg);
}
#endif

/**
 ****************************************************************************************
 * @brief Load debugger configuration stored in CLK_AMBA_REG reserved bits 3 and 2.
 * @note DA1585/586: Dedicated pins are used for the SWD interface.
 *       DA14531:    The SWD interface pins are configurable.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void syscntl_load_debugger_cfg(void)
{
#if defined (__DA14531__)
    SetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE, GetBits16(CLK_AMBA_REG, CLK_AMBA_REG_RSV3_RSV2));
#else
    SetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE, 1);
#endif
}

#if !defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Sets the Nominal VBAT3V output voltage of the boost converter.
 * @param[in] level DCDC VBAT3V output voltage level (see enum SYSCNTL_DCDC_VBAT3V_LEVEL).
 * @return void
 ****************************************************************************************
 */
void syscntl_set_dcdc_vbat3v_level(enum SYSCNTL_DCDC_VBAT3V_LEVEL level);
#endif

#if defined (__DA14531__)

/**
 ****************************************************************************************
 * @brief Gets the voltage level of DCDC converter.
 * @return the voltage level of DCDC converter (nominal level + trim value)
 ****************************************************************************************
 */
syscntl_dcdc_level_t syscntl_dcdc_get_level(void);

/**
 ****************************************************************************************
 * @brief Sets the DCDC converter voltage level on hardware (nominal + trim value).
 * @param[in] level     The voltage level to be set.
 * @return void
 ****************************************************************************************
 */
void syscntl_dcdc_set_level(syscntl_dcdc_level_t level);

/**
 ****************************************************************************************
 * @brief Disables DCDC converter. In Buck mode enables the LDO_LOW.
 * @return Error code
 ****************************************************************************************
 */
int8_t syscntl_dcdc_turn_off(void);

/**
 ****************************************************************************************
 * @brief Enables DCDC converter in the provided voltage level in order to supply
 *        VBAT_HIGH in Boost mode.
 * @param[in] level The voltage level of DCDC converter (nominal level + trim value).
 * @return Error code
 ****************************************************************************************
 */
int8_t syscntl_dcdc_turn_on_in_boost(syscntl_dcdc_level_t level);

/**
 ****************************************************************************************
 * @brief Enables DCDC converter in the provided voltage level in order to supply
 *        VBAT_LOW and disables LDO_LOW.
 * @param[in] level The voltage level of DCDC converter (nominal level + trim value).
 * @return Error code
 ****************************************************************************************
 */
int8_t syscntl_dcdc_turn_on_in_buck(syscntl_dcdc_level_t level);

/**
 ****************************************************************************************
 * @brief Sets the XTAL32M amplitude regulator mode.
 * @param[in] mode      Normal, Tracking or Hold mode.
 * @return void
 ****************************************************************************************
 */
void syscntl_cfg_xtal32m_amp_reg(xtal32m_amp_reg_t mode);

/**
 ****************************************************************************************
 * @brief Configures the POR_VBAT_HIGH circuit.
 * @param[in] mask       POR_VBAT_HIGH Mask ON/OFF
 * @return void
 ****************************************************************************************
 */
void syscntl_por_vbat_high_cfg(por_vbat_mask_t mask);

/**
 ****************************************************************************************
 * @brief Configures the POR_VBAT_LOW circuit.
 * @param[in] mask       POR_VBAT_LOW Mask ON/OFF
 * @return void
 ****************************************************************************************
 */
void syscntl_por_vbat_low_cfg(por_vbat_mask_t mask);
#endif

#endif // SYSCNTL_H_
