/**
 ****************************************************************************************
 *
 * @file adc_531.c
 *
 * @brief DA14531 General Purpose ADC driver source file.
 *
 * Copyright (C) 2019 Dialog Semiconductor. All rights reserved.
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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "adc.h"
#include "compiler.h"
#include "datasheet.h"
#include "otp_cs.h"
#include "arch.h"

#if defined (CFG_ADC_DMA_SUPPORT)
#include "dma.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */

#define ADC_531_MIN(a, b)   (((a) < (b)) ? (a) : (b))

/// Callback function to be called within ADC_Handler.
static adc_interrupt_cb_t intr_cb   __SECTION_ZERO("retention_mem_area0");

/// 25 degrees Celsius calibration value
static uint16_t cal_val;

#if defined (CFG_ADC_DMA_SUPPORT)
/// DMA Channel for ADC RX.
static DMA_ID dma_channel           __SECTION_ZERO("retention_mem_area0");

/// IRQ state of DMA channel for ADC RX.
static DMA_IRQ_CFG dma_irq_state    __SECTION_ZERO("retention_mem_area0");
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if defined (CFG_ADC_DMA_SUPPORT)
/**
 ****************************************************************************************
 * @brief Initializes DMA for ADC according to @p cfg.
 * @param[in] cfg The ADC configuration.
 * @return void
 ****************************************************************************************
 */
static void adc_dma_init(const adc_config_t *cfg)
{
    // Set and retain DMA Channel for ADC RX
    dma_channel = (cfg->dma_channel == ADC_DMA_CHANNEL_01) ? DMA_CHANNEL_0
                                                           : DMA_CHANNEL_2;

    // DMA channel configuration for ADC RX
    dma_cfg_t dma_cfg =
    {
        .bus_width       = DMA_BW_HALFWORD,
        .irq_enable      = DMA_IRQ_STATE_ENABLED,
        .dreq_mode       = DMA_DREQ_TRIGGERED,
        .src_inc         = DMA_INC_FALSE,
        .dst_inc         = DMA_INC_TRUE,
        .circular        = DMA_MODE_NORMAL,
        .dma_prio        = cfg->dma_priority,
        .dma_idle        = DMA_IDLE_BLOCKING_MODE,
        .dma_init        = DMA_INIT_AX_BX_AY_BY,
        .dma_sense       = DMA_SENSE_LEVEL_SENSITIVE,
        .dma_req_mux     = DMA_TRIG_ADC_RX,
        .src_address     = (uint32_t) GP_ADC_RESULT_REG,
        .dst_address     = cfg->dst_addr,
        .irq_nr_of_trans = 0,
        .length          = cfg->len,
        .cb              = cfg->rx_cb,
        .user_data       = cfg->user_data
    };

    // Retain IRQ state of DMA channel for ADC RX
    dma_irq_state = dma_cfg.irq_enable;

    // Enable DMA functionality
    adc_dma_enable();

    // Initialize DMA channel for ADC RX
    dma_initialize(dma_channel, &dma_cfg);
}
#endif // CFG_ADC_DMA_SUPPORT

/**
 ****************************************************************************************
 * @brief Configures ADC according to @p cfg.
 * @param[in] cfg The ADC configuration.
 * @note No configuration is performed if @p cfg is NULL.
 * @return void
 ****************************************************************************************
 */
static void adc_configure(const adc_config_t *cfg)
{
    if (cfg != NULL)
    {
        adc_set_input_mode(cfg->input_mode);

        // Set input channel as appropriate
        (cfg->input_mode == ADC_INPUT_MODE_SINGLE_ENDED) ?
        adc_set_se_input((adc_input_se_t) cfg->input)    :
        adc_set_diff_input((adc_input_diff_t) cfg->input);

        // Check whether Continuous mode shall be enabled
        (cfg->continuous == true) ? adc_continuous_enable() : adc_continuous_disable();

        adc_set_interval(cfg->interval_mult);

        // Check whether the die temperature sensor shall be enabled - if so,
        // configure ADC accordingly
        if (adc_get_se_input() == ADC_INPUT_SE_TEMP_SENS)
        {
            // ADC_OTP_CAL_25C
            cal_val = otp_cs_get_adc_25_cal();
            if (cal_val == 0)
            {
                // OTP calibration value is not present - use typical value
                // instead (= 473*64)
                cal_val = 30272;
            }
            adc_temp_sensor_enable();
            adc_chopper_enable();
            adc_set_input_mode(ADC_INPUT_MODE_SINGLE_ENDED);
            adc_ldo_const_current_enable();
            SetBits16(GP_ADC_CTRL2_REG, GP_ADC_STORE_DEL, 0);
            adc_set_sample_time(15);
            adc_set_oversampling(6);
            adc_attn_config(ADC_INPUT_ATTN_NO);
        }
        else
        {
            adc_temp_sensor_disable();
            adc_set_sample_time(cfg->smpl_time_mult);
            adc_attn_config(cfg->input_attenuator);

            // Check whether Chopper mode shall be enabled
            (cfg->chopping == true) ? adc_chopper_enable() : adc_chopper_disable();

            adc_set_oversampling(cfg->oversampling);
        }

#if defined (CFG_ADC_DMA_SUPPORT)
        adc_dma_init(cfg);
#endif
    }
}

/**
 ****************************************************************************************
 * @details Writes the default values to the CTRL registers as well as the trim
 * value from OTP CS to the GP_ADC_TRIM_REG register and configures ADC
 * according to @p cfg.
 * @note No configuration is performed if @p cfg is NULL.
 ****************************************************************************************
 */
void adc_init(const adc_config_t *cfg)
{
    // Write the default values
    SetWord16(GP_ADC_CTRL_REG, 0);
    SetWord16(GP_ADC_CTRL2_REG, 0x210);
    SetWord16(GP_ADC_CTRL3_REG, 0x40);

    // Write the trim value from OTP CS to the GP_ADC_TRIM_REG register
    SetWord16(GP_ADC_TRIM_REG, otp_cs_get_adc_trim_val());
    // Set GP_ADC_LDO_LEVEL to the preferred level of 925mV
    SetBits16(GP_ADC_TRIM_REG, GP_ADC_LDO_LEVEL, 4);

    NVIC_DisableIRQ(ADC_IRQn);

    adc_configure(cfg);

    adc_enable();
}

void adc_reset(void)
{
    // Write the default values without disabling LDO and ADC
    SetWord16(GP_ADC_CTRL_REG, GP_ADC_EN);
    SetWord16(GP_ADC_CTRL2_REG, 0x210);
    SetWord16(GP_ADC_CTRL3_REG, 0x40);

    NVIC_DisableIRQ(ADC_IRQn);
}

/**
 ****************************************************************************************
 * @brief Adds a delay in microseconds.
 * @note Code shall be built using -O3 optimization level.
 * @param[in] nof_us The desired number of microseconds to wait for.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void usDelay(uint32_t nof_us)
{
    for(uint32_t i = 4*nof_us; i != 0; i--)
    {
        __NOP();
    }
}

void adc_input_shift_enable(void)
{
    SetBits16(GP_ADC_CTRL2_REG, GP_ADC_OFFS_SH_EN, 1);

    // Guideline from the Analog IC Team: Wait for 5us to let the input shifter
    // settle just after enabling it
    usDelay(5);
}

void adc_input_shift_disable(void)
{
    SetBits16(GP_ADC_CTRL2_REG, GP_ADC_OFFS_SH_EN, 0);
}

void adc_input_shift_config(adc_input_sh_gain_t gain, adc_input_sh_cm_t cm)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_OFFS_SH_GAIN_SEL, gain);
    SetBits16(GP_ADC_CTRL2_REG, GP_ADC_OFFS_SH_CM, cm);

    adc_input_shift_enable();
}

void adc_set_se_input(adc_input_se_t input)
{
    SetBits16(GP_ADC_SEL_REG, GP_ADC_SEL_P, input);
}

void adc_set_diff_input(adc_input_diff_t input)
{
    SetBits16(GP_ADC_SEL_REG, GP_ADC_SEL_N, input);
}

void adc_register_interrupt(adc_interrupt_cb_t callback)
{
    intr_cb = callback;
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_MINT, 1);

    // Set the priority of ADC interrupt to level 2
    NVIC_SetPriority(ADC_IRQn, 2);
    // Enable ADC interrupt
    NVIC_EnableIRQ(ADC_IRQn);
}

void adc_unregister_interrupt(void)
{
    NVIC_DisableIRQ(ADC_IRQn);
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_MINT, 0);
    NVIC_ClearPendingIRQ(ADC_IRQn);
    intr_cb = NULL;
}

/**
 ****************************************************************************************
 * @note Sampling gets started; conversion gets started as soon as
 * adc_start() is called.
 * @sa adc_delay_set
 * @sa adc_start
 ****************************************************************************************
 */
void adc_enable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_EN, 1);
}

/**
 ****************************************************************************************
 * @note Conversion shall have completed before disabling ADC.
 * When in continuous mode, continuous mode shall be disabled and the pending
 * conversion shall complete in order for ADC to be in a defined state.
 * @sa adc_in_progress
 * @sa adc_set_continuous
 ****************************************************************************************
 */
void adc_disable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_EN, 0);
}

void adc_start(void)
{
#if defined (CFG_ADC_DMA_SUPPORT)
    // Start DMA channel for ADC RX
    dma_channel_start(dma_channel, dma_irq_state);
#endif
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_START, 1);
}

/**
 ****************************************************************************************
 * @note The corrected conversion result may be read calling @c adc_correct_sample.
 ****************************************************************************************
 */
uint16_t adc_get_sample(void)
{
    adc_start();
    while (adc_in_progress())
        ;
    adc_clear_interrupt();

    return (GetWord16(GP_ADC_RESULT_REG));
}

void adc_offset_calibrate(adc_input_mode_t input_mode)
{
    // Input shifter shall be disabled
    ASSERT_WARNING(GetBits16(GP_ADC_CTRL2_REG, GP_ADC_OFFS_SH_EN) == 0);

    uint16_t adc_val, adc_off_p, adc_off_n;
    uint32_t diff;

    // Store the current content of the following registers:
    // - GP_ADC_CTRL_REG,
    // - GP_ADC_CTRL2_REG.
    uint16_t tmp_adc_ctrl_reg  = GetWord16(GP_ADC_CTRL_REG);
    uint16_t tmp_adc_ctrl2_reg = GetWord16(GP_ADC_CTRL2_REG);

    // Write the default values to the CTRL registers and enable ADC
    adc_init(NULL);

    // Before starting the offset calibration itself, set the following:
    // - GP_ADC_I20U      = 1,
    // - GP_ADC_SMPL_TIME = 1,
    // - GP_ADC_CHOP      = 0,
    // - GP_ADC_CONV_NRS  > 3.
    adc_ldo_const_current_enable();
    adc_set_sample_time(1);
    adc_chopper_disable();
    adc_set_oversampling(7);

    // Set the provided input mode just to be on the safe side
    adc_set_input_mode(input_mode);

    // Try once to calibrate correctly

    adc_set_offset_positive(0x200);
    adc_set_offset_negative(0x200);
    adc_input_mute();

    adc_sign_change_disable();

    adc_val   = adc_get_sample();
    adc_off_p = (adc_val >> 6) - 0x200;

    adc_sign_change_enable();

    adc_val   = adc_get_sample();
    adc_off_n = (adc_val >> 6) - 0x200;

    if (input_mode == ADC_INPUT_MODE_SINGLE_ENDED)
    {
        adc_set_offset_positive(0x200 - 2 * adc_off_p);
        adc_set_offset_negative(0x200 - 2 * adc_off_n);
    }
    else
    {
        adc_set_offset_positive(0x200 - adc_off_p);
        adc_set_offset_negative(0x200 - adc_off_n);
    }

    adc_sign_change_disable();

    // Verify the calibration result

    adc_val = adc_get_sample() >> 6;

    if (adc_val > 0x200)
    {
        diff = adc_val - 0x200;
    }
    else
    {
        diff = 0x200 - adc_val;
    }

    if (diff >= 0x8)
    {
        // Calibration failed
        ASSERT_WARNING(0);
    }

    // Restore the content of the ADC CTRL and CTRL2 registers
    SetWord16(GP_ADC_CTRL_REG, tmp_adc_ctrl_reg);
    SetWord16(GP_ADC_CTRL2_REG, tmp_adc_ctrl2_reg);
}

#if !defined (__EXCLUDE_ADC_HANDLER__)
void ADC_Handler(void)
{
    adc_clear_interrupt();

    if (intr_cb != NULL)
    {
        intr_cb();
    }
}
#endif

/**
 ****************************************************************************************
 * @brief Returns the corrected conversion result.
 * @param[in] input Raw ADC value.
 * @return corrected conversion result
 ****************************************************************************************
 */
static uint16_t adc_correction_apply(uint16_t input)
{
    int16_t gain_error, offset;
    uint32_t res;

    if (GetBits16(GP_ADC_CTRL2_REG, GP_ADC_OFFS_SH_EN) == 1)
    {
        // Input shifter is enabled
        // Set Gain Error and Offset accordingly
        gain_error = otp_cs_get_adc_offsh_ge();
        offset     = otp_cs_get_adc_offsh_offset();
    }
    else if (adc_get_input_mode() == ADC_INPUT_MODE_SINGLE_ENDED)
    {
        // Single-ended mode; input shifter is disabled
        // Set Gain Error and Offset accordingly
        gain_error = otp_cs_get_adc_single_ge();
        offset     = otp_cs_get_adc_single_offset();
    }
    else
    {
        // Differential mode; input shifter is disabled
        // Set Gain Error and Offset accordingly
        gain_error = otp_cs_get_adc_diff_ge();
        offset     = otp_cs_get_adc_diff_offset();
    }

    // Gain error and offset correction in two steps - not that efficient -
    // to avoid possible overflows
    res = (UINT16_MAX * (uint32_t) input) / (UINT16_MAX + gain_error);
    res = res - (UINT16_MAX * offset) / (UINT16_MAX + gain_error);

    // Boundary check for lower limit
    if (res > 2 * UINT16_MAX)
    {
        return 0;
    }

    // Boundary check for upper limit
    if (res > UINT16_MAX)
    {
        return UINT16_MAX;
    }

    return ((uint16_t) res);
}

uint16_t adc_correct_sample(const uint16_t adc_val)
{
    return (adc_correction_apply(adc_val) >> (6 - ADC_531_MIN(6, adc_get_oversampling())));
}

void adc_temp_sensor_enable(void)
{
    SetBits16(GP_ADC_CTRL_REG, DIE_TEMP_EN, 1);

    // Guideline from the Analog IC Team: Wait for 25us to let the temperature
    // sensor settle just after enabling it
    usDelay(25);
}

void adc_temp_sensor_disable(void)
{
    SetBits16(GP_ADC_CTRL_REG, DIE_TEMP_EN, 0);
}

/**
 ****************************************************************************************
 * @brief Performs calculations related to the @c ADC_INPUT_SE_TEMP_SENS input channel.
 * @param[in] adc_sample Raw ADC sample being read from the @c ADC_INPUT_SE_TEMP_SENS
 *                       input channel.
 * @return temperature value in degrees Celsius
 ****************************************************************************************
 */
__STATIC_FORCEINLINE int8_t temp_calc(uint16_t adc_sample)
{
    // Tx (Celsius) = 25 + (ADCx - ADC_OTP_CAL_25C) / (1.606 * 64) (+/- 0.5, to improve accuracy)
    // cal_val is either ADC_OTP_CAL_25C or 30272 (the latter when OTP calibration value is not present)
    int32_t temp = (int32_t) ((adc_sample - cal_val) * 1000);
    if (temp > 0)
    {
        temp = temp + 803;
    }
    else if (temp < 0)
    {
        temp = temp - 803;
    }
    return ((int8_t) (25 + ((temp / 1606) >> 6)));
}

int8_t adc_get_temp(void)
{
    adc_start();
    while (adc_in_progress())
        ;
    adc_clear_interrupt();

    uint16_t adc_sample = GetWord16(GP_ADC_RESULT_REG);

    int8_t temp = temp_calc(adc_sample);

    return temp;
}

void adc_get_temp_async(uint16_t *adc_samples, uint16_t len, int8_t *temp_values)
{
    // Clear the ADC interrupt just to be on the safe side
    adc_clear_interrupt();

    for (uint16_t i = 0; i < len; i++)
    {
        temp_values[i] = temp_calc(adc_samples[i]);
    }
}
