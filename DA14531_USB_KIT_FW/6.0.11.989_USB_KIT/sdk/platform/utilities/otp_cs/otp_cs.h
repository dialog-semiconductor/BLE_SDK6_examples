/**
 ****************************************************************************************
 *
 * @file otp_cs.h
 *
 * @brief OTP Configuration Script header file (only for DA14531).
 *
 * Copyright (c) 2019 Dialog Semiconductor. All rights reserved.
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

#ifndef _OTP_CS_H_
#define _OTP_CS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if defined (__DA14531__)

#include <stdint.h>
#include "arch.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * OTP configuration script field offset from OTP memory starting address
 */
#define OTP_CS_BASE_OFFSET                  (0x7ED0)        // Configuration Script base offset

/*
 * OTP configuration script field base memory address
 */
#define OTP_CS_BASE_ADDR                    (MEMORY_OTP_BASE + OTP_CS_BASE_OFFSET)

/*
 * OTP configuration script constant values
 */
#define OTP_CS_MAX_ENTRIES                  (60)                           // OTP CS can have maximum 60 32-bit entries
#define OTP_CS_MAX_SIZE                     (OTP_CS_MAX_ENTRIES * 4)       // 240 bytes, 60 entries of 4 bytes
#define OTP_CS_END_ADDR                     (OTP_CS_BASE_ADDR + OTP_CS_MAX_SIZE)

#define OTP_CS_CMD_START                    (0xA5A5A5A5)
#define OTP_CS_CMD_DEBUG_VAL                (0x60000000)
#define OTP_CS_CMD_SWD_MODE                 (0x70000000)
#define OTP_CS_CMD_UART_STX                 (0x80000000)
#define OTP_CS_CMD_SDK_VAL                  (0x90000000)
#define OTP_CS_CMD_SPI_CLK                  (0xA0000000)
#define OTP_CS_EMPTY_VAL                    (0xFFFFFFFF)

/*
 * Supported IDs of the command 0x9000xxyy (yy is the ID, xx is the number of items)
 */
#define OTP_CS_PD_SYS                       (1)
#define OTP_CS_PD_SLP                       (2)
#define OTP_CS_PD_TMR                       (4)
#define OTP_CS_PD_RAD                       (6)
#define OTP_CS_PD_ADPLL                     (7)
#define OTP_CS_GP_ADC_SINGLE                (17)
#define OTP_CS_GP_ADC_DIFF                  (18)
#define OTP_CS_GP_ADC_GEN                   (19)
#define OTP_CS_GP_TEMP                      (20)
#define OTP_CS_LP_CLK_SET                   (22)
#define OTP_CS_XTAL_TRIM                    (23)

/*
 * Max number of items per ID (have to be update with the CS specification)
 */
#define OTP_CS_PD_RAD_LEN                   (2) // number of pairs of address/value
#define OTP_CS_PD_ADPLL_LEN                 (6) // number of pairs of address/value
#define OTP_CS_GP_ADC_SINGLE_LEN            (1) // single values
#define OTP_CS_GP_ADC_DIFF_LEN              (1) // single values
#define OTP_CS_GP_ADC_GEN_LEN               (2) // single values
#define OTP_CS_GP_TEMP_LEN                  (1) // single values
#define OTP_CS_LP_CLK_SET_LEN               (1) // single values
#define OTP_CS_XTAL_TRIM_LEN                (1) // single values

/*
 * Error codes
 */
#define OTP_CS_ERROR_OK                     (0)
#define OTP_CS_ERROR_NO_CS_SECTION_FOUND    (-1)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Register address/value pairs
typedef struct
{
    /// Register address
    uint32_t addr;

    /// Register value
    uint32_t val;
} otp_cs_addr_value_t;

/// Trim values
typedef struct
{
#if defined (CFG_ENHANCED_TX_PWR_CTRL)
    /// RF_PA_CTRL_REG content for 3dBm
    uint32_t rf_pa_ctrl_3dBm;

    /// ADPLL_DIV_CTRL_REG content for 3dBm
    // NOTE: Applicable to T2 version samples only
    uint32_t adpll_div_ctrl_3dBm;

    /// RF_PA_CTRL_REG content for 0dBm
    uint32_t rf_pa_ctrl_0dBm;

    /// ADPLL_DIV_CTRL_REG content for 0dBm
    // NOTE: Applicable to T2 version samples only
    uint32_t adpll_div_ctrl_0dBm;
#endif // CFG_ENHANCED_TX_PWR_CTRL

    /// Trim values for LP_CLK_SET
    uint32_t lp_clk_set[OTP_CS_LP_CLK_SET_LEN];
    
    /// Calculated ADC Gain Error based on GPADC_GE_OFFS_SE_10 and GPADC_GE_OFFS_SE_90
    /// (GP_ADC_SINGLE group in CS)
    // NOTE: To be taken into account when in single-ended mode
    // with the input shifter being disabled
    int16_t gp_adc_single_ge;

    /// Calculated ADC Offset Error based on GPADC_GE_OFFS_SE_10 and GPADC_GE_OFFS_SE_90
    /// (GP_ADC_SINGLE group in CS)
    // NOTE: To be taken into account when in single-ended mode
    // with the input shifter being disabled
    int16_t gp_adc_single_offset;

    /// Calculated ADC Gain Error based on GPADC_GE_OFFS_DIF_m80 and GPADC_GE_OFFS_DIF_p80
    /// (GP_ADC_DIFF group in CS)
    // NOTE: To be taken into account when in differential mode
    // with the input shifter being disabled
    int16_t gp_adc_diff_ge;

    /// Calculated ADC Offset Error based on GPADC_GE_OFFS_DIF_m80 and GPADC_GE_OFFS_DIF_p80
    /// (GP_ADC_DIFF group in CS)
    // NOTE: To be taken into account when in differential mode
    // with the input shifter being disabled
    int16_t gp_adc_diff_offset;

    /// GP_ADC_TRIM_REG trim value of the GP_ADC_GEN group
    uint16_t gp_adc_trim_reg;

    /// Calculated ADC Gain Error based on GPADC_GE_SHIFTER_10 and GPADC_GE_SHIFTER_90
    /// (GP_ADC_GEN group in CS)
    // NOTE: To be taken into account when the input shifter is enabled
    int16_t gp_adc_offsh_ge;

    /// Calculated ADC Offset Error based on GPADC_GE_SHIFTER_10 and GPADC_GE_SHIFTER_90
    /// (GP_ADC_GEN group in CS)
    // NOTE: To be taken into account when the input shifter is enabled
    int16_t gp_adc_offsh_offset;

    /// Extracted temp sensor calibration data (GP_TEMP group in CS)
    uint16_t adc_25_cal;
    
    /// XTAL32M wait trim value
    uint16_t xtal_wait_trim;
} otp_cs_trim_value_t;

/// Configuration script storage structure
typedef struct
{
    /// PD_RAD settings stored in address/value pairs
    otp_cs_addr_value_t pd_rad[OTP_CS_PD_RAD_LEN];

    /// PD_ADPLL settings stored in address/value pairs
    otp_cs_addr_value_t pd_adpll[OTP_CS_PD_ADPLL_LEN];

    /// Trim values
    otp_cs_trim_value_t trim_values;

    /// PD_RAD pairs found
    uint8_t pd_rad_pairs;

    /// PD_ADPLL pairs found
    uint8_t pd_adpll_pairs;
} otp_cs_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Stores OTP configuration script values to retention memory.
 * @note OTP memory has to be enabled in read mode, prior to calling this function.
 * @return error code
 ****************************************************************************************
 */
int8_t otp_cs_store(void);

/**
 ****************************************************************************************
 * @brief Loads values from retention memory to respective PD_RAD registers.
 * @note Call @c otp_cs_load_pd_rad() before @c otp_cs_load_pd_adpll().
 * return void
 ****************************************************************************************
 */
void otp_cs_load_pd_rad(void);

/**
 ****************************************************************************************
 * @brief Loads values from retention memory to respective PD_ADPLL registers.
 * @return void
 ****************************************************************************************
 */
void otp_cs_load_pd_adpll(void);

/**
 ****************************************************************************************
 * @brief Get the low power clock configuration.
 * @return low power clock configuration
 ****************************************************************************************
 */
 uint32_t otp_cs_get_low_power_clock(void);

/**
 ****************************************************************************************
 * @brief Returns the ADC Gain Error in single-ended mode.
 * @return ADC Gain Error in single-ended mode
 ****************************************************************************************
 */
int16_t otp_cs_get_adc_single_ge(void);

/**
 ****************************************************************************************
 * @brief Returns the ADC Offset Error in single-ended mode.
 * @return ADC Offset Error in single-ended mode
 ****************************************************************************************
 */
int16_t otp_cs_get_adc_single_offset(void);

/**
 ****************************************************************************************
 * @brief Returns the ADC Gain Error in differential mode.
 * @return ADC Gain Error in differential mode
 ****************************************************************************************
 */
int16_t otp_cs_get_adc_diff_ge(void);

/**
 ****************************************************************************************
 * @brief Returns the ADC Offset Error in differential mode.
 * @return ADC Offset Error in differential mode
 ****************************************************************************************
 */
int16_t otp_cs_get_adc_diff_offset(void);

/**
 ****************************************************************************************
 * @brief Returns the GP_ADC_TRIM_REG trim value.
 * @return GP_ADC_TRIM_REG trim value
 ****************************************************************************************
 */
uint16_t otp_cs_get_adc_trim_val(void);

/**
 ****************************************************************************************
 * @brief Returns the ADC Gain Error (input shifter is enabled).
 * @return ADC Gain Error when the input shifter is enabled
 ****************************************************************************************
 */
int16_t otp_cs_get_adc_offsh_ge(void);

/**
 ****************************************************************************************
 * @brief Returns the ADC Offset Error (input shifter is enabled).
 * @return ADC Offset Error when the input shifter is enabled
 ****************************************************************************************
 */
int16_t otp_cs_get_adc_offsh_offset(void);

/**
 ****************************************************************************************
 * @brief Returns the 25 degrees Celsius calibration value, needed to calculate
 * calibrated temperature based on raw ADC value.
 * @return 25 degrees Celsius calibration value
 ****************************************************************************************
 */
uint16_t otp_cs_get_adc_25_cal(void);

/**
 ****************************************************************************************
 * @brief Checks whether XTAL32M trim value is written in OTP CS section.
 * @return true if present; false otherwise
 ****************************************************************************************
 */
bool otp_cs_is_xtal32m_trim_present(void);

#if defined (CFG_ENHANCED_TX_PWR_CTRL)
/**
 ****************************************************************************************
 * @brief Returns the content of @c RF_PA_CTRL_REG for 3dBm.
 * @return @c RF_PA_CTRL_REG content for 3dBm
 ****************************************************************************************
 */
uint32_t otp_cs_get_rf_pa_ctrl_3dBm(void);

/**
 ****************************************************************************************
 * @brief Returns the content of @c ADPLL_DIV_CTRL_REG for 3dBm.
 * @return @c ADPLL_DIV_CTRL_REG content for 3dBm
 ****************************************************************************************
 */
uint32_t otp_cs_get_adpll_div_ctrl_3dBm(void);

/**
 ****************************************************************************************
 * @brief Returns the content of @c RF_PA_CTRL_REG for 0dBm.
 * @return @c RF_PA_CTRL_REG content for 0dBm
 ****************************************************************************************
 */
uint32_t otp_cs_get_rf_pa_ctrl_0dBm(void);

/**
 ****************************************************************************************
 * @brief Returns the content of @c ADPLL_DIV_CTRL_REG for 0dBm.
 * @return @c ADPLL_DIV_CTRL_REG content for 0dBm
 ****************************************************************************************
 */
uint32_t otp_cs_get_adpll_div_ctrl_0dBm(void);
#endif // CFG_ENHANCED_TX_PWR_CTRL

/**
 ****************************************************************************************
 * @brief Get the XTAL32M wait trim value.
 * @return The value represents time measured in usec.
 ****************************************************************************************
 */
uint16_t otp_cs_get_xtal_wait_trim(void);

#endif // __DA14531__

#endif //_OTP_CS_H_
