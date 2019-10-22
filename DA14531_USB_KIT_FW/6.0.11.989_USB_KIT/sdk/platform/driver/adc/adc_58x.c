/**
 ****************************************************************************************
 *
 * @file adc_58x.c
 *
 * @brief DA14585/DA14586 General Purpose ADC driver source file.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor. All rights reserved.
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

/// Callback function to be called within ADC_Handler.
static adc_interrupt_cb_t intr_cb   __SECTION_ZERO("retention_mem_area0");

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

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
        adc_set_input_mode(cfg->mode);

        // Set input channel as appropriate
        (cfg->mode == ADC_INPUT_MODE_SINGLE_ENDED)       ?
        adc_set_se_input((adc_input_se_t) cfg->input)    :
        adc_set_diff_input((adc_input_diff_t) cfg->input);

        (cfg->sign == false) ? SetBits16(GP_ADC_CTRL_REG, GP_ADC_SIGN, 0)
                             : SetBits16(GP_ADC_CTRL_REG, GP_ADC_SIGN, 1);

        SetWord16(GP_ADC_CTRL2_REG, GP_ADC_DELAY_EN | GP_ADC_I20U);
        (cfg->attn == false) ? SetBits16(GP_ADC_CTRL2_REG, GP_ADC_ATTN3X, 0)
                             : SetBits16(GP_ADC_CTRL2_REG, GP_ADC_ATTN3X, 1);
    }
}

void adc_init(const adc_config_t *cfg)
{
    SetWord16(GP_ADC_CTRL_REG, GP_ADC_LDO_EN | GP_ADC_EN);
    SetWord16(GP_ADC_CTRL2_REG, 0);

    NVIC_DisableIRQ(ADC_IRQn);

    adc_configure(cfg);
}

void adc_reset(void)
{
    // Write the default values without disabling LDO and ADC
    SetWord16(GP_ADC_CTRL_REG, GP_ADC_LDO_EN | GP_ADC_EN);
    SetWord16(GP_ADC_CTRL2_REG, 0);

    NVIC_DisableIRQ(ADC_IRQn);
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

void adc_set_se_input(adc_input_se_t input)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_SEL, input);
}

void adc_set_diff_input(adc_input_diff_t input)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_SEL, input);
}

void adc_enable(void)
{
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_LDO_EN, 1);
    SetBits16(GP_ADC_CTRL_REG, GP_ADC_EN, 1);
}

void adc_disable(void)
{
    SetWord16(GP_ADC_CTRL_REG, 0);
}

uint16_t adc_get_sample(void)
{
    int cnt = ADC_TIMEOUT;

    SetBits16(GP_ADC_CTRL_REG, GP_ADC_START, 1);
    while (cnt-- && (GetWord16(GP_ADC_CTRL_REG) & GP_ADC_START) != 0x0000);
    SetWord16(GP_ADC_CLEAR_INT_REG, 0x0000); // Clear interrupt
    return GetWord16(GP_ADC_RESULT_REG);
}

/**
 ****************************************************************************************
 * @brief Introduces a variable microsend delay for use with ADC peripheral.
 * @param[in] nof_us Number of microseconds to delay
 * @return void
 ****************************************************************************************
 */
static void adc_usDelay(uint32_t nof_us)
{
    while( nof_us-- ){
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }
}

uint32_t adc_get_vbat_sample(bool sample_vbat1v)
{
    uint32_t adc_sample, adc_sample2;
    adc_config_t cfg =
    {
        .mode = ADC_INPUT_MODE_SINGLE_ENDED,
        .sign = true,
        .attn = true
    };

    adc_init(&cfg);
    adc_usDelay(20);

    if (sample_vbat1v)
        adc_set_se_input(ADC_INPUT_SE_VBAT1V);
    else
        adc_set_se_input(ADC_INPUT_SE_VBAT3V);

    adc_sample = adc_get_sample();
    adc_usDelay(1);

    cfg.sign = false;
    adc_init(&cfg);

    if (sample_vbat1v)
        adc_set_se_input(ADC_INPUT_SE_VBAT1V);
    else
        adc_set_se_input(ADC_INPUT_SE_VBAT3V);

    adc_sample2 = adc_get_sample();
    // We have to divide the following result by 2 if the 10-bit accuracy is
    // enough
    adc_sample = (adc_sample2 + adc_sample);
    adc_disable();

    return adc_sample;
}

void adc_offset_calibrate(adc_input_mode_t input_mode)
{
    uint16_t adc_res, adc_off_p, adc_off_n;

    adc_set_input_mode(input_mode);
    adc_enable();
    adc_usDelay(20);

    SetWord16(GP_ADC_OFFP_REG, 0x200);
    SetWord16(GP_ADC_OFFN_REG, 0x200);

    adc_input_mute();

    adc_sign_change_disable();

    adc_res   = adc_get_sample();
    adc_off_p = adc_res - 0x200;

    adc_sign_change_enable();

    adc_res   = adc_get_sample();
    adc_off_n = adc_res - 0x200;

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

    adc_input_unmute();
}

#if !defined (__EXCLUDE_ADC_HANDLER__)
void ADC_Handler(void)
{
    SetWord16(GP_ADC_CLEAR_INT_REG, 1);

    if (intr_cb != NULL)
    {
        intr_cb();
    }
}
#endif
