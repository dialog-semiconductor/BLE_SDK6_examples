/**
 ****************************************************************************************
 *
 * @file adc_58x.h
 *
 * @brief DA14585/DA14586 General Purpose ADC driver header file.
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

#ifndef _ADC_58X_H_
#define _ADC_58X_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * DEFINES
 ****************************************************************************************
 */

#define ADC_TIMEOUT             (100000)

/// ADC input channel mode
typedef enum
{
    /// Differential mode
    ADC_INPUT_MODE_DIFFERENTIAL = 0,

    /// Single-ended mode
    ADC_INPUT_MODE_SINGLE_ENDED = 1
} adc_input_mode_t;

/// ADC single-ended input channels
typedef enum
{
    /// P0_0
    ADC_INPUT_SE_P0_0    = 0,

    /// P0_1
    ADC_INPUT_SE_P0_1    = 1,

    /// P0_2
    ADC_INPUT_SE_P0_2    = 2,

    /// P0_3
    ADC_INPUT_SE_P0_3    = 3,

    /// AVS
    ADC_INPUT_SE_AVS     = 4,

    /// VDD_REF
    ADC_INPUT_SE_VDD_REF = 5,

    /// VDD_RTT
    ADC_INPUT_SE_VDD_RTT = 6,

    /// VBAT3V
    ADC_INPUT_SE_VBAT3V  = 7,

    /// VDCDC
    ADC_INPUT_SE_VDCDC   = 8,

    /// VBAT1V
    ADC_INPUT_SE_VBAT1V  = 9
} adc_input_se_t;

/// ADC differential input channels
typedef enum
{
    /// P0_0 vs P0_1
    ADC_INPUT_DIFF_P0_0_P0_1 = 0,

    /// P0_2 vs P0_3
    ADC_INPUT_DIFF_P0_2_P0_3 = 1
} adc_input_diff_t;

/// Callback type to be associated with ADC_IRQn.
typedef void (*adc_interrupt_cb_t)(void);

/// ADC configuration
typedef struct
{
    /// Input channel mode
    adc_input_mode_t    mode;

    /// Input channel; possible values: @c adc_input_se_t, @c adc_input_diff_t
    uint8_t             input;

    /// Input/output sign change off/on
    bool                sign;

    /// Input attenuator off/on
    bool                attn;
} adc_config_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Gets ADC sample from VBAT1V or VBAT3V power supplies using the 20 usec delay.
 * @param[in] sample_vbat1v :true = sample VBAT1V, false = sample VBAT3V
 * @return ADC VBAT1V or VBAT3V sample
 ****************************************************************************************
 */
uint32_t adc_get_vbat_sample(bool sample_vbat1v);

#endif // _ADC_58X_H_
