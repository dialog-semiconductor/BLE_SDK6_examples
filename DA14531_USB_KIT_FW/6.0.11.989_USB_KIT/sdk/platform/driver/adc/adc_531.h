/**
 ****************************************************************************************
 *
 * @file adc_531.h
 *
 * @brief DA14531 General Purpose ADC driver header file.
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

#ifndef _ADC_531_H_
#define _ADC_531_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>

#include "datasheet.h"
#include "arch.h"

#if defined (CFG_ADC_DMA_SUPPORT)
#include "dma.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */

/// ADC input channel mode
typedef enum
{
    /// Differential mode
    ADC_INPUT_MODE_DIFFERENTIAL = 0,

    /// Single-ended mode
    ADC_INPUT_MODE_SINGLE_ENDED = 1
} adc_input_mode_t;

/// ADC positive input channels
typedef enum
{
    /// Single-ended P0_1
    ADC_INPUT_SE_P0_1      = 0,

    /// Single-ended P0_2
    ADC_INPUT_SE_P0_2      = 1,

    /// Single-ended P0_6
    ADC_INPUT_SE_P0_6      = 2,

    /// Single-ended P0_7
    ADC_INPUT_SE_P0_7      = 3,

    /// Temperature Sensor
    ADC_INPUT_SE_TEMP_SENS = 4,

    /// VBAT_HIGH
    ADC_INPUT_SE_VBAT_HIGH = 5,

    /// VBAT_LOW
    ADC_INPUT_SE_VBAT_LOW  = 6,

    /// VDD
    ADC_INPUT_SE_VDD       = 7
} adc_input_se_t;

/// ADC negative input channels
typedef enum
{
    /// Differential P0_1
    ADC_INPUT_DIFF_P0_1 = 0,

    /// Differential P0_2
    ADC_INPUT_DIFF_P0_2 = 1,

    /// Differential P0_6
    ADC_INPUT_DIFF_P0_6 = 2,

    /// Differential P0_7
    ADC_INPUT_DIFF_P0_7 = 3
} adc_input_diff_t;

/// ADC attenuator options
typedef enum
{
    /// No attenuator; input voltages up to 0.9V allowed
    ADC_INPUT_ATTN_NO = 0,

    /// 2x attenuator; input voltages up to 1.8V allowed
    ADC_INPUT_ATTN_2X = 1,

    /// 3x attenuator; input voltages up to 2.7V allowed
    ADC_INPUT_ATTN_3X = 2,

    /// 4x attenuator; input voltages up to 3.6V allowed
    ADC_INPUT_ATTN_4X = 3
} adc_input_attn_t;

/// Gain options for the input shifter
typedef enum
{
    /// Gain 2x
    ADC_INPUT_SH_GAIN_2X = 0,

    /// Gain 2.25x
    ADC_INPUT_SH_GAIN_2X25 = 1
} adc_input_sh_gain_t;

/// Common mode adjustment options for the input shifter
typedef enum
{
    /// CM = 1.25V (input range: 0.80 - 1.70)
    ADC_INPUT_SH_CM_1V25 = 0,

    /// CM = 1.30V (input range: 0.85 - 1.75)
    ADC_INPUT_SH_CM_1V30 = 1,

    /// CM = 1.35V (input range: 0.90 - 1.80)
    ADC_INPUT_SH_CM_1V35 = 2,

    /// CM = 1.40V (input range: 0.95 - 1.85)
    ADC_INPUT_SH_CM_1V40 = 3
} adc_input_sh_cm_t;

#if defined (CFG_ADC_DMA_SUPPORT)
/// ADC DMA channel configuration
typedef enum
{
    /// ADC RX in channel 0
    ADC_DMA_CHANNEL_01 = 0,

    /// ADC RX in channel 2
    ADC_DMA_CHANNEL_23 = 1
} adc_dma_channel_t;
#endif // CFG_ADC_DMA_SUPPORT

/// Callback type to be associated with ADC_IRQn.
typedef void (*adc_interrupt_cb_t)(void);

/// ADC configuration
typedef struct
{
    /// Input channel mode
    adc_input_mode_t    input_mode;

    /// Input channel; possible values: @c adc_input_se_t, @c adc_input_diff_t
    uint8_t             input;

    /// Sample time
    uint8_t             smpl_time_mult;

    /// Continuous mode off/on
    bool                continuous;

    /// Interval between conversions when in continuous mode
    uint8_t             interval_mult;

    /// Attenuator
    adc_input_attn_t    input_attenuator;

    /// Chopper mode off/on
    bool                chopping;

    /// Oversampling mode
    uint8_t             oversampling;

#if defined (CFG_ADC_DMA_SUPPORT)
    /// Destination address
    uint32_t            dst_addr;

    /// DMA transfer length, i.e. number of 16-bit raw conversion values
    uint16_t            len;

    /// DMA callback for ADC RX
    dma_cb_t            rx_cb;

    /// Data to pass to DMA callback
    void                *user_data;

    /// DMA channel for ADC RX
    adc_dma_channel_t   dma_channel;

    /// DMA channel priority for ADC RX
    DMA_PRIO_CFG        dma_priority;
#endif // CFG_ADC_DMA_SUPPORT
} adc_config_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Enables the input shifter of ADC.
 * @return void
 ****************************************************************************************
 */
void adc_input_shift_enable(void);

/**
 ****************************************************************************************
 * @brief Disables the input shifter of ADC.
 * @return void
 ****************************************************************************************
 */
void adc_input_shift_disable(void);

/**
 ****************************************************************************************
 * @brief Configures the input shifter of ADC according to @p gain and @p cm.
 * @details Enables the input shifter of ADC as well.
 * @param[in] gain The gain option for the input shifter.
 * @param[in] cm The common mode adjustment option for the input shifter.
 * @return void
 ****************************************************************************************
 */
void adc_input_shift_config(adc_input_sh_gain_t gain, adc_input_sh_cm_t cm);

#if defined (CFG_ADC_DMA_SUPPORT)
/**
 ****************************************************************************************
 * @brief Enables DMA functionality for ADC RX.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_dma_enable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_DMA_EN, 1);
}

/**
 ****************************************************************************************
 * @brief Disables DMA functionality for ADC RX.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_dma_disable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_DMA_EN, 0);
}
#endif // CFG_ADC_DMA_SUPPORT

/**
 ****************************************************************************************
 * @brief Clears the interrupt.
 * @return void
 * @sa adc_register_interrupt
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_clear_interrupt(void)
{
    SetWord16(GP_ADC_CLEAR_INT_REG, 1);
}

/**
 ****************************************************************************************
 * @brief Sets the delay taking place after LDO has been enabled and before ADC
 * gets enabled.
 * @param[in] delay_factor The delay multiplication factor needed to calculate
 * the delay.
 * @details Delay is equal to (@p delay_factor) * 4 * (ADC_CLK period).
 * @return void
 * @sa adc_enable
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_delay_set(uint8_t delay_factor)
{
    ASSERT_WARNING(delay_factor != 0);
    SetBits16(GP_ADC_CTRL3_REG, GP_ADC_EN_DEL, delay_factor);
}

/**
 ****************************************************************************************
 * @brief Starts a conversion.
 * @details Starts DMA channel for ADC RX, only if @c CFG_ADC_DMA_SUPPORT is defined.
 * @note It is not allowed to call this function while a conversion is in progress.
 * @return void
 * @sa adc_in_progress
 ****************************************************************************************
 */
void adc_start(void);

/**
 ****************************************************************************************
 * @brief Checks whether a conversion is in progress.
 * @return true if conversion is in progress; false if conversion is ready
 ****************************************************************************************
 */
__STATIC_FORCEINLINE bool adc_in_progress(void)
{
    return (GetBits16(GP_ADC_CTRL_REG, GP_ADC_START));
}

/**
 ****************************************************************************************
 * @brief Enables continuous mode.
 * @note When in continuous mode, ADC constantly performs conversions one after
 * the other. @c adc_start shall be called to initiate the very first conversion.
 * Interval between subsequent conversions may be adjusted calling @c adc_set_interval.
 * @return void
 * @sa adc_start
 * @sa adc_set_interval
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_continuous_enable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_CONT, 1);
}

/**
 ****************************************************************************************
 * @brief Disables continuous mode.
 * @return void
 * @sa adc_continuous_enable
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_continuous_disable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_CONT, 0);
}

/**
 ****************************************************************************************
 * @brief Returns the current single-ended input channel.
 * @return one of the adc_input_se_t input channels
 ****************************************************************************************
 */
__STATIC_FORCEINLINE adc_input_se_t adc_get_se_input(void)
{
    return ((adc_input_se_t) GetBits16(GP_ADC_SEL_REG, GP_ADC_SEL_P));
}

/**
 ****************************************************************************************
 * @brief Returns the current differential input channel.
 * @return one of the adc_input_diff_t input channels
 ****************************************************************************************
 */
__STATIC_FORCEINLINE adc_input_diff_t adc_get_diff_input(void)
{
    return ((adc_input_diff_t) GetBits16(GP_ADC_SEL_REG, GP_ADC_SEL_N));
}

/**
 ****************************************************************************************
 * @brief Sets oversampling mode.
 * @details When in oversampling mode, multiple successive conversions are
 * executed and the results are averaged to increase the effective number of
 * bits.
 * The number of successively executed conversions is 2^(@p oversampling_mode).
 * @param[in] oversampling_mode Oversampling mode option (0-7).
 * @return void
 * @sa adc_correction_apply
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_set_oversampling(uint8_t oversampling_mode)
{
    ASSERT_WARNING(oversampling_mode <= 7);
    SetBits16(GP_ADC_CTRL2_REG, GP_ADC_CONV_NRS, oversampling_mode);
}

/**
 ****************************************************************************************
 * @brief Returns the current oversampling mode option.
 * @return oversampling mode option (0-7)
 * @sa adc_set_oversampling
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t adc_get_oversampling(void)
{
    return (GetBits16(GP_ADC_CTRL2_REG, GP_ADC_CONV_NRS));
}

/**
 ****************************************************************************************
 * @brief Sets the sampling time.
 * @param[in] mult The multiplication factor needed to calculate the sampling
 * time.
 * @details Sample time is @p mult * 8 ADC_CLK cycles when @p mult is 1-15 or
 * 1 ADC_CLK cycle when @p mult is 0.
 * @return void
 * @sa adc_attn_config
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_set_sample_time(uint8_t mult)
{
    ASSERT_WARNING(mult <= 15);
    SetBits16(GP_ADC_CTRL2_REG, GP_ADC_SMPL_TIME, mult);
}

/**
 ****************************************************************************************
 * @brief Configures the attenuator.
 * @note When the attenuator is enabled, sampling time shall increase calling
 * @c adc_set_sample_time.
 * @param[in] attn The desired attenuator option.
 * @return void
 * @sa adc_set_sample_time
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_attn_config(adc_input_attn_t attn)
{
    SetBits16(GP_ADC_CTRL2_REG, GP_ADC_ATTN, attn);
}

/**
 ****************************************************************************************
 * @brief Enables chopper mode.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_chopper_enable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_CHOP, 1);
}

/**
 ****************************************************************************************
 * @brief Disables chopper mode.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_chopper_disable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_CHOP, 0);
}

/**
 ****************************************************************************************
 * @brief Enables a 20-uA constant load current at the ADC LDO output.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_ldo_const_current_enable(void)
{
    SetBits16(GP_ADC_CTRL2_REG, GP_ADC_I20U, 1);
}

/**
 ****************************************************************************************
 * @brief Disables the 20-uA constant load current at the ADC LDO output.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_ldo_const_current_disable(void)
{
    SetBits16(GP_ADC_CTRL2_REG, GP_ADC_I20U, 0);
}

/**
 ****************************************************************************************
 * @brief Sets the interval between two successive conversions when in
 * continuous mode.
 * @param[in] mult The multiplication factor needed to calculate the interval.
 * @note Interval time is @p mult x 1.024 ms. Valid values are 0-255.
 * @return void
 * @sa adc_continuous_enable
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_set_interval(uint8_t mult)
{
    SetBits16(GP_ADC_CTRL3_REG, GP_ADC_INTERVAL, mult);
}

/**
 ****************************************************************************************
 * @brief Returns the corrected conversion result.
 * @details The raw conversion result is corrected:
 * - Gain Error & Offset compensation, if Gain Error and Offset parameters are available,
 * - Oversampling is taken into account.
 * @param[in] adc_val Raw ADC value, read from the selected input channel,
 * calling @c adc_get_sample. In the case of interrupt-driven measurements, the
 * raw ADC value shall be obtained by reading the GP_ADC_RESULT_REG register
 * directly, i.e. GetWord16(GP_ADC_RESULT_REG).
 * @return conversion result
 * @note In the case of single-ended mode,
 * actual input voltage = Vref * (conversion result) / (2^N - 1).
 * In the case of differential mode,
 * actual input voltage = (2 * Vref * (conversion result) / (2^N - 1)) - Vref.
 * In both cases:
 * - Vref may be 0.9V/1.8V/2.7V/3.6V,
 * - N may be 10-16 bits, depending on the oversampling mode.
 * @sa adc_correction_apply
 * @sa adc_set_oversampling
 ****************************************************************************************
 */
uint16_t adc_correct_sample(const uint16_t adc_val);

/**
 ****************************************************************************************
 * @brief Enables the die temperature sensor.
 * @return void
 ****************************************************************************************
 */
void adc_temp_sensor_enable(void);

/**
 ****************************************************************************************
 * @brief Disables the die temperature sensor.
 * @return void
 ****************************************************************************************
 */
void adc_temp_sensor_disable(void);

/**
 ****************************************************************************************
 * @brief Returns temperature based on raw ADC sample being read from the @c ADC_INPUT_SE_TEMP_SENS
 *        input channel.
 * @details Starts a polling-based conversion, waits for the result, clears the
 *          ADC interrupt, and returns temperature, either calibrated or uncalibrated
 *          (the latter when OTP calibration value is not present).
 * @return temperature value in degrees Celsius
 ****************************************************************************************
 */
int8_t adc_get_temp(void);

/**
 ****************************************************************************************
 * @brief Returns temperature values based on raw ADC samples obtained by monitoring
 *        the @c ADC_INPUT_SE_TEMP_SENS input channel and then reading the GP_ADC_RESULT_REG
 *        register directly, i.e. @c GetWord16(GP_ADC_RESULT_REG).
 * @note Shall be called in the case of interrupt-driven measurements:
 *       - single ADC interrupt,
 *       - continuous mode; DMA functionality disabled,
 *       - continuous mode; DMA functionality enabled.
 * @details Clears the ADC interrupt and calculates temperature values in degrees
 *          Celsius, either calibrated or uncalibrated (the latter when OTP calibration
 *          value is not present).
 * @param[in] adc_samples Pointer to raw ADC samples.
 * @param[in] len Number of raw ADC samples.
 * @param[out] temp_values Pointer to temperature values in degrees Celsius.
 * @warning @c adc_start shall be called first.
 * @sa adc_start
 ****************************************************************************************
 */
void adc_get_temp_async(uint16_t *adc_samples, uint16_t len, int8_t *temp_values);

#endif /* _ADC_531_H_ */
