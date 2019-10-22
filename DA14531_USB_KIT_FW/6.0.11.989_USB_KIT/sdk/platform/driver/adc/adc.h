/**
 ****************************************************************************************
 *
 * @file adc.h
 *
 * @brief General Purpose ADC driver generic header file.
 *
 * @note This is the header file to be included by any source file that is to
 * call (DA14585/DA14586 OR DA14531) ADC functions.
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

#ifndef _ADC_H_
#define _ADC_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "datasheet.h"
#include "arch.h"
#if defined (__DA14531__)
    #include "adc_531.h"
#else
    #include "adc_58x.h"
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes and enables ADC.
 * @note In the case of DA14531, @c adc_init applies the mandatory ADC settings,
 *       when the @c ADC_INPUT_SE_TEMP_SENS input channel is to be used.
 * @param[in] cfg The ADC configuration.
 * @note Initialization does not take place if @p cfg is NULL.
 * @return void
 ****************************************************************************************
 */
void adc_init(const adc_config_t *cfg);

/**
 ****************************************************************************************
 * @brief Resets ADC.
 * @details Writes the default values to the CTRL registers without disabling
 * LDO and ADC.
 * @return void
 ****************************************************************************************
 */
void adc_reset(void);

/**
 ****************************************************************************************
 * @brief Registers a callback function to be called within ADC_Handler.
 * @note Enables the ADC_IRQn interrupt.
 * @param[in] callback The callback function.
 * @return void
 ****************************************************************************************
 */
void adc_register_interrupt(adc_interrupt_cb_t callback);

/**
 ****************************************************************************************
 * @brief Unregisters the callback function that was previously registered.
 * @note Disables the ADC_IRQn interrupt.
 * @return void
 ****************************************************************************************
 */
void adc_unregister_interrupt(void);

/**
 ****************************************************************************************
 * @brief Sets the provided input mode.
 * @param[in] input_mode The desired input mode.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_set_input_mode(adc_input_mode_t input_mode)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_SE, input_mode);
}

/**
 ****************************************************************************************
 * @brief Returns the current input mode.
 * @return input mode
 ****************************************************************************************
 */
__STATIC_FORCEINLINE adc_input_mode_t adc_get_input_mode(void)
{
    return ((adc_input_mode_t) GetBits16(GP_ADC_CTRL_REG, GP_ADC_SE));
}

/**
 ****************************************************************************************
 * @brief Sets the provided single-ended input channel.
 * @param[in] input The desired single-ended input channel.
 * @return void
 ****************************************************************************************
 */
void adc_set_se_input(adc_input_se_t input);

/**
 ****************************************************************************************
 * @brief Sets the provided differential input channel.
 * @param[in] input The desired differential input channel.
 * @return void
 ****************************************************************************************
 */
void adc_set_diff_input(adc_input_diff_t input);

/**
 ****************************************************************************************
 * @brief Returns raw ADC value.
 * @details Starts a polling-based conversion, waits for the result, clears the
 *          ADC interrupt, and returns the raw ADC value.
 * @return raw ADC value
 ****************************************************************************************
 */
uint16_t adc_get_sample(void);

/**
 ****************************************************************************************
 * @brief Enables ADC.
 * @return void
 ****************************************************************************************
 */
void adc_enable(void);

/**
 ****************************************************************************************
 * @brief Disables ADC.
 * @return void
 ****************************************************************************************
 */
void adc_disable(void);

/**
 ****************************************************************************************
 * @brief Mutes the ADC inputs.
 * @note Samples are taken at mid-scale to determine internal offset and/or
 * noise of ADC with respect to VDD_REF.
 * @return void
 * @sa adc_offset_calibrate
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_input_mute(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_MUTE, 1);
}

/**
 ****************************************************************************************
 * @brief Unmutes the ADC inputs.
 * @return void
 * @sa adc_offset_calibrate
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_input_unmute(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_MUTE, 0);
}

/**
 ****************************************************************************************
 * @brief Enables ADC input/output sign change.
 * @return void
 * @sa adc_offset_calibrate
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_sign_change_enable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_SIGN, 1);
}

/**
 ****************************************************************************************
 * @brief Disables ADC input/output sign change.
 * @return void
 * @sa adc_offset_calibrate
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_sign_change_disable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_SIGN, 0);
}

/**
 ****************************************************************************************
 * @brief Adjusts offset of positive ADC array.
 * @param[in] offset The offset adjustment.
 * @return void
 * @sa adc_offset_calibrate
 * @sa adc_set_offset_negative
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_set_offset_positive(uint16_t offset)
{
    ASSERT_WARNING(offset < 1024);
    SetBits16(GP_ADC_OFFP_REG, GP_ADC_OFFP, offset);
}

/**
 ****************************************************************************************
 * @brief Returns the current offset adjustment of positive ADC array.
 * @return offset adjustment
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t adc_get_offset_positive(void)
{
    return (GetBits16(GP_ADC_OFFP_REG, GP_ADC_OFFP));
}

/**
 ****************************************************************************************
 * @brief Adjusts offset of negative ADC array.
 * @param[in] offset The offset adjustment.
 * @return void
 * @sa adc_offset_calibrate
 * @sa adc_set_offset_positive
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void adc_set_offset_negative(uint16_t offset)
{
    ASSERT_WARNING(offset < 1024);
    SetBits16(GP_ADC_OFFN_REG, GP_ADC_OFFN, offset);
}

/**
 ****************************************************************************************
 * @brief Returns the current offset adjustment of negative ADC array.
 * @return offset adjustment
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t adc_get_offset_negative(void)
{
    return (GetBits16(GP_ADC_OFFN_REG, GP_ADC_OFFN));
}

/**
 ****************************************************************************************
 * @brief Performs ADC offset calibration.
 * @note In the case of DA14531, offset calibration cannot be performed when the
 * input shifter is enabled.
 * @param[in] input_mode The mode (single-ended/differential) offset calibration
 *                       shall be performed in.
 * @return void
 ****************************************************************************************
 */
void adc_offset_calibrate(adc_input_mode_t input_mode);

#endif /* _ADC_H_ */
