/**
 ****************************************************************************************
 *
 * @file otp_cs.c
 *
 * @brief OTP Configuration Script source file (only for DA14531).
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if defined (__DA14531__)

#include <stdint.h>
#include <string.h>
#include "arch.h"
#include "otp_cs.h"
#include "rf_531.h"
#include "compiler.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * Default XTAL32M wait time in usec (applicable only to DA14531).
 * The wait time in usec is either provided by the following fixed value or by OTP CS
 * entry.
 ****************************************************************************************
 */
#define XTAL32M_WAIT_TRIM_TIME_USEC      (500)  // 500 usec


/*
 * Structure containing info about debugger configuration and OTP CS registers
 * normally handled by the Booter - resides in the otp_cs_booter section
 * within ER_IROM1 (ER_IROM1 is 192 bytes long and meant for the vector table -
 * the latter is 160 bytes long so we utilize available space within ER_IROM1)
 ****************************************************************************************
 */
otp_cs_booter_val_t booter_val  __SECTION("otp_cs_booter") =
{
    // Debugger configuration
    .dbg_cfg       = 0,

    // BANDGAP_REG value
    .bandgap_reg   = BANDGAP_REG_RESET,

    // CLK_RC32M_REG value
    .clk_rc32m_reg = CLK_RC32M_REG_RESET,

    // CLK_RC32K_REG value
    .clk_rc32k_reg = CLK_RC32K_REG_RESET
};

/*
 * LOCAL VARIABLES
 ****************************************************************************************
 */

static otp_cs_t otp_cs     __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
static uint32_t txdiv_trim;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Calculates the ADC Gain Error based on two points.
 * @param[in] low Measurement at the low end of the full scale.
 * @param[in] high Measurement at the high end of the full scale.
 * @return ADC Gain Error
 ****************************************************************************************
 */
static int16_t calc_adc_ge(uint16_t low, uint16_t high)
{
    return (((high - low) + ((uint16_t) (high - low) >> 2)) - UINT16_MAX);
}

/**
 ****************************************************************************************
 * @brief Calculates the ADC Offset Error based on two points.
 * @param[in] low Measurement at the low end of the full scale.
 * @param[in] high Measurement at the high end of the full scale.
 * @return ADC Offset Error
 ****************************************************************************************
 */
static int16_t calc_adc_offset(uint16_t low, uint16_t high)
{
    return ((int16_t) ((9 * low) - high) >> 3);
}

int8_t otp_cs_store(void)
{
    volatile uint32_t value;
    uint32_t offset = 0;
    uint32_t num = 0;
    uint32_t id;

    // Array to temporarily store the GP_ADC_GEN trim values read from CS
    uint32_t gp_adc_gen[OTP_CS_GP_ADC_GEN_LEN];

    // Keep track of the available empty positions in the storage area (per ID)
    otp_cs.pd_rad_pairs = 0;
    otp_cs.pd_adpll_pairs = 0;
    uint32_t gp_adc_gen_idx = 0;

    // Read 1st entry from OTP CS section
    if(GetWord32(OTP_CS_BASE_ADDR) != OTP_CS_CMD_START)
    {
        return OTP_CS_ERROR_NO_CS_SECTION_FOUND; // no CS found;
    }

    // Prepare for CS parsing
    offset = offset + 4;

    // Valid CS was found, continue processing
    while (OTP_CS_BASE_ADDR + offset < OTP_CS_END_ADDR)
    {
        value = GetWord32(OTP_CS_BASE_ADDR + offset);

        if ((value & 0xF0000000) == OTP_CS_CMD_SDK_VAL)
        {
            id = value & 0x000000FF;
            num = (value & 0x0000FF00) >> 8;

            switch (id)
            {
                case OTP_CS_PD_SYS:
                case OTP_CS_PD_SLP:
                case OTP_CS_PD_TMR:
                {
                    break;
                }
                case OTP_CS_PD_RAD:
                {
                    memcpy(&otp_cs.pd_rad[otp_cs.pd_rad_pairs], (void *) (OTP_CS_BASE_ADDR + offset + 4), num * sizeof(uint32_t));
                    otp_cs.pd_rad_pairs = otp_cs.pd_rad_pairs + num;
                    ASSERT_WARNING(otp_cs.pd_rad_pairs <= (OTP_CS_PD_RAD_LEN * 2));
                    break;
                }
                case OTP_CS_PD_ADPLL:
                {
                    memcpy(&otp_cs.pd_adpll[otp_cs.pd_adpll_pairs], (void *) (OTP_CS_BASE_ADDR + offset + 4), num * sizeof(uint32_t));
                    otp_cs.pd_adpll_pairs = otp_cs.pd_adpll_pairs + num;
                    ASSERT_WARNING(otp_cs.pd_adpll_pairs <= (OTP_CS_PD_ADPLL_LEN * 2));
                    break;
                }
                case OTP_CS_GP_ADC_SINGLE:
                {
                    // Extract GPADC_GE_OFFS_SE_10 and GPADC_GE_OFFS_SE_90
                    uint16_t low  = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset + 4);
                    uint16_t high = (uint16_t) (GetWord32(OTP_CS_BASE_ADDR + offset + 4) >> 16);

                    // Calculate ADC Gain Error based on GPADC_GE_OFFS_SE_10 and GPADC_GE_OFFS_SE_90 and retain that
                    otp_cs.trim_values.gp_adc_single_ge = calc_adc_ge(low, high);

                    // Calculate ADC Offset Error based on GPADC_GE_OFFS_SE_10 and GPADC_GE_OFFS_SE_90 and retain that
                    otp_cs.trim_values.gp_adc_single_offset = calc_adc_offset(low, high);

                    break;
                }
                case OTP_CS_GP_ADC_DIFF:
                {
                    // Extract GPADC_GE_OFFS_DIF_m80 and GPADC_GE_OFFS_DIF_p80
                    uint16_t low  = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset + 4);
                    uint16_t high = (uint16_t) (GetWord32(OTP_CS_BASE_ADDR + offset + 4) >> 16);

                    // Calculate ADC Gain Error based on GPADC_GE_OFFS_DIF_m80, GPADC_GE_OFFS_DIF_p80 and retain that
                    otp_cs.trim_values.gp_adc_diff_ge = calc_adc_ge(low, high);

                    // Calculate ADC Offset Error based on GPADC_GE_OFFS_DIF_m80, GPADC_GE_OFFS_DIF_p80 and retain that
                    otp_cs.trim_values.gp_adc_diff_offset = calc_adc_offset(low, high);

                    break;
                }
                case OTP_CS_GP_ADC_GEN:
                {
                    memcpy(&gp_adc_gen[gp_adc_gen_idx], (void *) (OTP_CS_BASE_ADDR + offset + 4), num * sizeof(uint32_t));
                    gp_adc_gen_idx = gp_adc_gen_idx + num;
                    ASSERT_WARNING(gp_adc_gen_idx <= OTP_CS_GP_ADC_GEN_LEN)
                    break;
                }
                case OTP_CS_GP_TEMP:
                {
                    // Extract and retain the 25 degrees Celsius calibration value
                    otp_cs.trim_values.adc_25_cal = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset + 4);
                    break;
                }
                case OTP_CS_LP_CLK_SET:
                {
                    otp_cs.trim_values.lp_clk_set[0] = GetWord32(OTP_CS_BASE_ADDR + offset + 4);
                    break;
                }
                case OTP_CS_XTAL_TRIM:
                {
                    // Low 16-bit hold the XTAL32M wait trim value in usec
                    otp_cs.trim_values.xtal_wait_trim = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset + 4);
                    ASSERT_WARNING(otp_cs.trim_values.xtal_wait_trim) // zero value is not allowed
                    break;
                }
                default:
                {
                    // Place user hook here
                    break;
                }
            }
            offset = offset + 4 + (num * 4);
        }
        else if (value >= 0x40000000 && value <= 0x5000424C)
        {
            // CS reached a register address/value pair (already handled by the booter)
            offset = offset + 4 + 4;

            if (value == BANDGAP_REG)
            {
                /// BANDGAP_REG value exists in OTP CS
                booter_val.bandgap_reg = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset - 4);
            }
            else if (value == CLK_FREQ_TRIM_REG)
            {
                // XTAL32M trim value exists in OTP CS
                otp_cs.trim_values.xtal_trim_value = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset - 4);
            }
            else if (value == CLK_RC32M_REG)
            {
                /// CLK_RC32M_REG value exists in OTP CS
                booter_val.clk_rc32m_reg = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset - 4);
            }
            else if (value == CLK_RC32K_REG)
            {
                /// CLK_RC32K_REG value exists in OTP CS
                booter_val.clk_rc32k_reg = (uint16_t) GetWord32(OTP_CS_BASE_ADDR + offset - 4);
            }
        }
        else if ((value & 0xF0000000) == OTP_CS_CMD_SWD_MODE)
        {
            // CS reached development mode value (already handled by the booter)
            offset = offset + 4;
        }
        else if ((value & 0xF0000000) == OTP_CS_CMD_UART_STX)
        {
            // CS reached UART STX timeout value (already handled by the booter)
            offset = offset + 4;
        }
        else if ((value & 0xF0000000) == OTP_CS_CMD_SPI_CLK)
        {
            // CS reached SPI clock value (already handled by the booter)
            offset = offset + 4;
        }
        else if ((value & 0xF0000000) == OTP_CS_CMD_DEBUG_VAL)
        {
            // CS reached debug value (already handled by the booter)
            offset = offset + 4;
        }
        else if (value == OTP_CS_EMPTY_VAL)
        {
            break; // End of CS
        }
        else
        {
            // should not enter here
            ASSERT_WARNING(0)
        }
    }

    // Store the GP_ADC_TRIM_REG trim value of the GP_ADC_GEN group
    otp_cs.trim_values.gp_adc_trim_reg = (uint16_t) gp_adc_gen[0];

    // Extract GPADC_GE_SHIFTER_10 and GPADC_GE_SHIFTER_90
    uint16_t low  = (uint16_t) gp_adc_gen[1];
    uint16_t high = (uint16_t) (gp_adc_gen[1] >> 16);

    // Calculate ADC Gain Error based on GPADC_GE_SHIFTER_10 and GPADC_GE_SHIFTER_90 and store that
    otp_cs.trim_values.gp_adc_offsh_ge = calc_adc_ge(low, high);

    // Calculate ADC Offset Error based on GPADC_GE_SHIFTER_10 and GPADC_GE_SHIFTER_90 and store that
    otp_cs.trim_values.gp_adc_offsh_offset = calc_adc_offset(low, high);

    // Store the number of <register addr>/<value> pairs that belong to PD_RAD domain
    ASSERT_WARNING(!(otp_cs.pd_rad_pairs & 1U))
    otp_cs.pd_rad_pairs = otp_cs.pd_rad_pairs >> 1U;

    // Store the number of <register addr>/<value> pairs that belong to PD_ADPLL domain
    ASSERT_WARNING(!(otp_cs.pd_adpll_pairs & 1U))
    otp_cs.pd_adpll_pairs = otp_cs.pd_adpll_pairs >> 1U;
    
    // Use default value if respective OTP CS entry is not written
    if (!otp_cs.trim_values.xtal_wait_trim)
    {
        otp_cs.trim_values.xtal_wait_trim = XTAL32M_WAIT_TRIM_TIME_USEC;
    }

    // 4 PD_ADPLL pairs must be included in T2 trimming version
    ASSERT_WARNING(otp_cs.pd_adpll_pairs == 4)

#if defined (CFG_ENHANCED_TX_PWR_CTRL)
    for (int i = 0; i < otp_cs.pd_rad_pairs; i++)
    {
        if ((otp_cs.pd_rad[i].addr == RF_PA_CTRL_REG) &&
            // Distinguish T2 from T1 - assumed that T2 samples have at least one nonzero bit
            // spread from bit 24 to bit 10, at RF_PA_CTRL_REG index
            (((otp_cs.pd_rad[i].val >> 10U) & 0x7FFF) != 0))
        {
            // Store the RF_PA_CTRL_REG content for 3dBm
            otp_cs.trim_values.rf_pa_ctrl_3dBm = ((otp_cs.pd_rad[i].val & PA_RAMP_STEP_SPEED) | ((otp_cs.pd_rad[i].val >> 10) & (TRIM_DUTY_NEG | TRIM_DUTY_POS)));

            // Store the 9-bit value for 3dBm (ADPLL_DIV_CTRL_REG.TXDIV_TRIM setting)
            txdiv_trim = (otp_cs.pd_rad[i].val >> 16U) & 0x01FF;

            // Store the RF_PA_CTRL_REG content for 0dBm
            otp_cs.trim_values.rf_pa_ctrl_0dBm = ((otp_cs.pd_rad[i].val & PA_RAMP_STEP_SPEED) | (otp_cs.pd_rad[i].val & (TRIM_DUTY_NEG | TRIM_DUTY_POS)));

            break;
        }
    }

    /* Special handling for ADPLL_DIV_CTRL_REG:
     *      PrefValues: TXDIV_TRIM = OTP CS value, RXDIV_TRIM = will be applied by `set_recommended_settings()`.
     *      The rest fields (RXDIV_FB_EN_TX, RXDIV_FB_EN_RX, FBDIV_EN) keep their reset values.
     * Reset values for ADPLL_DIV_CTRL_REG are: TXDIV_TRIM = 0xCC, RXDIV_TRIM = 0xFF, RXDIV_FB_EN_TX = 0x0, RXDIV_FB_EN_RX = 0x1, FBDIV_EN = 0x1.
     */
    for (int i = 0; i < otp_cs.pd_adpll_pairs; i++)
    {
        if (otp_cs.pd_adpll[i].addr == ADPLL_DIV_CTRL_REG)
        {
            // Store the ADPLL_DIV_CTRL_REG content for 3dBm
            otp_cs.trim_values.adpll_div_ctrl_3dBm = ((otp_cs.pd_adpll[i].val & (~TXDIV_TRIM)) | (txdiv_trim << 17U));

            // Store the ADPLL_DIV_CTRL_REG content for 0dBm
            otp_cs.trim_values.adpll_div_ctrl_0dBm = otp_cs.pd_adpll[i].val;

            break;
        }
    }
#endif

    return OTP_CS_ERROR_OK;
}

void otp_cs_load_pd_rad(void)
{
    for (int i = 0; i < otp_cs.pd_rad_pairs; i++)
    {
#if defined (CFG_ENHANCED_TX_PWR_CTRL)
        if (otp_cs.pd_rad[i].addr == RF_PA_CTRL_REG)
        {
            continue;
        }
#else
        if ((otp_cs.pd_rad[i].addr == RF_PA_CTRL_REG) &&
            (((otp_cs.pd_rad[i].val >> 10U) & 0x7FFF) != 0) &&  // Distinguish T2 from T1 - assumed that T2 samples have at least one non zero bit spread from bit 24 to bit 10, at RF_PA_CTRL_REG index
            (rf_pa_pwr_get() > RF_TX_PWR_LVL_0d0))         // 0dBm mode for all power settings equal to 9 and below - 3dBm for 10 and above
            {
                SetWord32(otp_cs.pd_rad[i].addr, (otp_cs.pd_rad[i].val & PA_RAMP_STEP_SPEED) | ((otp_cs.pd_rad[i].val >> 10) & 0x03F));
                txdiv_trim = (otp_cs.pd_rad[i].val >> 16U) & 0x01FF; // store the 9-bit value for 3dBm (power setting 10 and above) ADPLL_DIV_CTRL_REG.TXDIV_TRIM setting 
                continue;
            }
#endif
        SetWord32(otp_cs.pd_rad[i].addr, otp_cs.pd_rad[i].val);
    }
}

void otp_cs_load_pd_adpll(void)
{
    for (int i = 0; i < otp_cs.pd_adpll_pairs; i++)
    {
#if defined (CFG_ENHANCED_TX_PWR_CTRL)
        if (otp_cs.pd_adpll[i].addr == ADPLL_DIV_CTRL_REG)
        {
            continue;
        }
#else
        /* Special handling for ADPLL_DIV_CTRL_REG:
         *      PrefValues: TXDIV_TRIM = OTP CS value, RXDIV_TRIM = will be applied by `set_recommended_settings()`.
         *      The rest fields (RXDIV_FB_EN_TX, RXDIV_FB_EN_RX, FBDIV_EN) keep their reset values.
         * Reset values for ADPLL_DIV_CTRL_REG are: TXDIV_TRIM = 0xCC, RXDIV_TRIM = 0xFF, RXDIV_FB_EN_TX = 0x0, RXDIV_FB_EN_RX = 0x1, FBDIV_EN = 0x1.
        */
        if ((otp_cs.pd_adpll[i].addr == ADPLL_DIV_CTRL_REG) &&
            (rf_pa_pwr_get() > RF_TX_PWR_LVL_0d0))    // 0dBm mode for all power settings equal to 9 and below - 3dBm for 10 and above
            {
                SetWord32(otp_cs.pd_adpll[i].addr, ((otp_cs.pd_adpll[i].val & (~TXDIV_TRIM)) | (txdiv_trim << 17U)));
                continue;
            }
#endif
        SetWord32(otp_cs.pd_adpll[i].addr, otp_cs.pd_adpll[i].val);
    }
}

uint32_t otp_cs_get_low_power_clock(void)
{
    return otp_cs.trim_values.lp_clk_set[0];
}

int16_t otp_cs_get_adc_single_ge(void)
{
    return otp_cs.trim_values.gp_adc_single_ge;
}

int16_t otp_cs_get_adc_single_offset(void)
{
    return otp_cs.trim_values.gp_adc_single_offset;
}

int16_t otp_cs_get_adc_diff_ge(void)
{
    return otp_cs.trim_values.gp_adc_diff_ge;
}

int16_t otp_cs_get_adc_diff_offset(void)
{
    return otp_cs.trim_values.gp_adc_diff_offset;
}

uint16_t otp_cs_get_adc_trim_val(void)
{
    return otp_cs.trim_values.gp_adc_trim_reg;
}

int16_t otp_cs_get_adc_offsh_ge(void)
{
    return otp_cs.trim_values.gp_adc_offsh_ge;
}

int16_t otp_cs_get_adc_offsh_offset(void)
{
    return otp_cs.trim_values.gp_adc_offsh_offset;
}

uint16_t otp_cs_get_adc_25_cal(void)
{
    return otp_cs.trim_values.adc_25_cal;
}

uint16_t otp_cs_get_xtal32m_trim_value(void)
{
    return otp_cs.trim_values.xtal_trim_value;
}

uint16_t otp_cs_get_xtal_wait_trim(void)
{
    return otp_cs.trim_values.xtal_wait_trim;
}

#if defined (CFG_ENHANCED_TX_PWR_CTRL)
uint32_t otp_cs_get_rf_pa_ctrl_3dBm(void)
{
    return otp_cs.trim_values.rf_pa_ctrl_3dBm;
}

uint32_t otp_cs_get_adpll_div_ctrl_3dBm(void)
{
    return otp_cs.trim_values.adpll_div_ctrl_3dBm;
}

uint32_t otp_cs_get_rf_pa_ctrl_0dBm(void)
{
    return otp_cs.trim_values.rf_pa_ctrl_0dBm;
}

uint32_t otp_cs_get_adpll_div_ctrl_0dBm(void)
{
    return otp_cs.trim_values.adpll_div_ctrl_0dBm;
}
#endif // CFG_ENHANCED_TX_PWR_CTRL

#endif // __DA14531__