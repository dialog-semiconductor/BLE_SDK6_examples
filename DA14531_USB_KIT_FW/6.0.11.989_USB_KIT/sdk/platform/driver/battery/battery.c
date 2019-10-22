/**
 ****************************************************************************************
 *
 * @file battery.c
 *
 * @brief Battery driver. Provides Battery level. Uses ADC module to get current voltage.
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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "battery.h"
#include "datasheet.h"
#include "adc.h"
#include "compiler.h"
#include "arch.h"

#if !defined (__DA14531__)
//internal configuration option to select the filtering of the adc_values
//0<= No filter, 1<=Drop the last 4-bits, 2<= Moving average filter, 3<=Moving average, reactive at big changes
static const uint8_t battery_filter_option=0;
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Calculates voltage of CR2032 lithium cell batteries.
 * @return battery voltage in mV
 ****************************************************************************************
 */
static uint16_t batt_cal_volt_cr2032(void)
{
    uint16_t adc_sample;

    adc_config_t adc_cfg =
    {
        .input_mode       = ADC_INPUT_MODE_SINGLE_ENDED,
        .input            = ADC_INPUT_SE_VBAT_HIGH,
        .smpl_time_mult   = 2,
        .continuous       = false,
        .interval_mult    = 0,
        .input_attenuator = ADC_INPUT_ATTN_4X,
        .chopping         = false,
        .oversampling     = 1
    };

    adc_offset_calibrate(ADC_INPUT_MODE_SINGLE_ENDED);

    adc_init(&adc_cfg);

    adc_sample = adc_get_sample();
    adc_sample = adc_correct_sample(adc_sample);

    adc_disable();

    // Divider depends on oversampling setting
    return ((3600 * adc_sample) / 2047);
}

/**
 ****************************************************************************************
 * @brief Calculates voltage of alkaline cell batteries.
 * @warning DA14531 shall operate in boost mode.
 * @return battery voltage in mV
 ****************************************************************************************
 */
static uint16_t batt_cal_volt_alkaline(void)
{
    uint16_t adc_sample;

    adc_config_t adc_cfg =
    {
        .input_mode       = ADC_INPUT_MODE_SINGLE_ENDED,
        .input            = ADC_INPUT_SE_VBAT_LOW,
        .smpl_time_mult   = 2,
        .continuous       = false,
        .interval_mult    = 0,
        .input_attenuator = ADC_INPUT_ATTN_NO,
        .chopping         = false,
        .oversampling     = 1
    };

    adc_init(&adc_cfg);

    // Configure and enable the input shifter of ADC
    adc_input_shift_config(ADC_INPUT_SH_GAIN_2X, ADC_INPUT_SH_CM_1V30);

    adc_sample = adc_get_sample();
    adc_sample = adc_correct_sample(adc_sample);

    adc_input_shift_disable();
    adc_disable();

    // Divider depends on oversampling setting
    return (850 + ((900 * adc_sample) / 2047));
}

/// Function pointer array encapsulating battery-specific functions for voltage
/// calculations.
static uint16_t (*const batt_cal_volt[])(void) =
{
    batt_cal_volt_cr2032,
    batt_cal_volt_alkaline
};

/**
 ****************************************************************************************
 * @brief Returns voltage of the @p batt_type battery.
 * @param[in] batt_type Battery type.
 * @return battery voltage in mV
 ****************************************************************************************
 */
uint16_t battery_get_voltage(const batt_t batt_type)
{
    uint16_t batt_volt = batt_cal_volt[batt_type]();

    return batt_volt;
}

/**
 ****************************************************************************************
 * @brief Calculates battery level for CR2032 lithium cell batteries.
 * @param[in] batt_volt Battery voltage in mV.
 * @return battery level (0-100%)
 ****************************************************************************************
 */
static uint8_t batt_cal_cr2032(uint16_t batt_volt)
{
    if (batt_volt < 2000)
    {
        return 0;
    }

    if (batt_volt > 3000)
    {
        return 100;
    }
    else
    {
        return (((batt_volt - 2000) * 100) / (3000 - 2000));
    }
}

/**
 ****************************************************************************************
 * @brief Calculates battery level for alkaline cell batteries.
 * @param[in] batt_volt Battery voltage in mV.
 * @return battery level (0-100%)
 ****************************************************************************************
 */
static uint8_t batt_cal_alkaline(uint16_t batt_volt)
{
    if (batt_volt < 1100)
    {
        return 0;
    }

    if (batt_volt > 1500)
    {
        return 100;
    }
    else
    {
        return (((batt_volt - 1100) * 100) / (1500 - 1100));
    }
}

/// Function pointer array encapsulating battery-specific functions for battery
/// level calculations.
static uint8_t (*const batt_cal_lvl[])(uint16_t batt_volt) =
{
    batt_cal_cr2032,
    batt_cal_alkaline
};
#else
/**
 ****************************************************************************************
 * @brief Calculates battery level for CR2032 batteries.
 * @param[in] adc_sample ADC sample.
 * @return battery level (0-100%)
 ****************************************************************************************
 */
static uint8_t batt_cal_cr2032(uint16_t adc_sample)
{
    uint8_t batt_lvl;

    if (adc_sample > 1705)
        batt_lvl = 100;
    else if (adc_sample <= 1705 && adc_sample > 1584)
        batt_lvl = 28 + (uint8_t)(( ( ((adc_sample - 1584) << 16) / (1705 - 1584) ) * 72 ) >> 16) ;
    else if (adc_sample <= 1584 && adc_sample > 1360)
        batt_lvl = 4 + (uint8_t)(( ( ((adc_sample - 1360) << 16) / (1584 - 1360) ) * 24 ) >> 16) ;
    else if (adc_sample <= 1360 && adc_sample > 1136)
        batt_lvl = (uint8_t)(( ( ((adc_sample - 1136) << 16) / (1360 - 1136) ) * 4 ) >> 16) ;
    else
        batt_lvl = 0;

    return batt_lvl;
}

/**
 ****************************************************************************************
 * @brief Calculates battery level for CR1225 batteries.
 * @param[in] adc_sample ADC sample.
 * @return battery level (0-100%)
 ****************************************************************************************
 */
static uint8_t batt_cal_cr1225(uint16_t adc_sample)
{
    uint8_t batt_lvl;

    //1705=3.0V, 1137=2V
    if(adc_sample >= 1137)
        batt_lvl = (adc_sample - 1137)*100/568;
    else
        batt_lvl = 0;

    return batt_lvl;
}


/**
 ****************************************************************************************
 * @brief Calculates battery level for a single AAA battery.
 * @param[in] adc_sample ADC sample.
 * @return battery level (0-100%)
 ****************************************************************************************
 */
static uint8_t batt_cal_aaa(uint16_t adc_sample)
{
    uint8_t batt_lvl;

    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
    {
        //Boost mode
        if(adc_sample >= BATTERY_MEASUREMENT_BOOST_AT_0V8)
            batt_lvl = (adc_sample - BATTERY_MEASUREMENT_BOOST_AT_0V8)*100/
                        (BATTERY_MEASUREMENT_BOOST_AT_1V5-BATTERY_MEASUREMENT_BOOST_AT_0V8);
        else
            batt_lvl = 0;
    }
    else
    {
        //Buck mode
        if(adc_sample >= BATTERY_MEASUREMENT_BUCK_AT_2V4)
            batt_lvl = (adc_sample - BATTERY_MEASUREMENT_BUCK_AT_2V4)*100/
                        (BATTERY_MEASUREMENT_BUCK_AT_3V0-BATTERY_MEASUREMENT_BUCK_AT_2V4);
        else
            batt_lvl = 0;
    }
    return batt_lvl;
}

/**
 ****************************************************************************************
 * @brief Filters the adc_sample_value.
 * @param[in] new_adc_value The adc_sample value.
 * @return The filtered output
 ****************************************************************************************
 */
static inline uint16_t battery_moving_average_filter(uint16_t new_adc_value)
{
    static uint32_t old_value __SECTION_ZERO("retention_mem_area0");
    const int filter_coefficient = 4;

    //this part of the filter is used to remove the delay on start up
    if ( old_value == 0)
        old_value = new_adc_value << filter_coefficient;
    else
    //this part filters the values
        old_value = old_value - (old_value >> filter_coefficient) + new_adc_value;
    return ( old_value >> filter_coefficient);
}

/**
 ****************************************************************************************
 * @brief Filters the adc_sample_value.
 * @param[in] new_adc_value The adc_sample value.
 * @return The filtered output
 ****************************************************************************************
 */
static inline uint16_t battery_moving_average_with_reaction_filter(uint16_t new_adc_value)
{
    static uint32_t old_value __SECTION_ZERO("retention_mem_area0");
    //the filter implement a moving average on 2^filter_coefficient values
    const int filter_coefficient = 3;

    //the adc_sample value is 11-bit we consider a change larger than 1.5% of
    //the reference voltage to be a big change and the filter will
    //go immediately to the new value
    const int considered_big_change = 32;

    //this part of the filter is used to quickly jump to the new value when the
    //sampling rate is to slow and the battery changes with a quick rate
    if (abs(old_value-new_adc_value)>considered_big_change)
        old_value = new_adc_value << filter_coefficient;
    else
    //this part filters fairly constant values to avoid
    //the random noise in the measurement
        old_value = old_value - (old_value >> filter_coefficient) + new_adc_value;
    return ( old_value >> filter_coefficient);
}


/**
 ****************************************************************************************
 * @brief Filters the adc_sample_value according to the constant battery_filter_option.
 * @param[in] new_adc_value The adc_sample value.
 * @return The filtered output
 ****************************************************************************************
 */
static inline uint16_t battery_filter_value(uint16_t new_adc_value)
{
     switch(battery_filter_option)
     {
         case 0:
                 return(new_adc_value);
         case 1:
                 return(new_adc_value&0xFFF0);
         case 2:
                 return(battery_moving_average_filter(new_adc_value));
         case 3:
                 return(battery_moving_average_with_reaction_filter(new_adc_value));
     }
     return (new_adc_value);
}

/// Function pointer array encapsulating battery-specific functions for battery
/// level calculations.
static uint8_t (*const batt_cal_lvl[])(uint16_t adc_sample) =
{
    batt_cal_cr2032,
    batt_cal_cr1225,
    batt_cal_aaa
};
#endif // __DA14531__

uint8_t battery_get_lvl(const batt_t batt_type)
{
    uint8_t batt_lvl = 0;
#if defined (__DA14531__)
    uint16_t batt_volt = 0;
#else
    uint16_t adc_sample = 0;
#endif

#if !defined (__DA14531__)
    // NOTE1: In the DA14585/DA14586 case, ADC offset calibration is performed
    // for each and every battery type.
    // NOTE2: In the DA14531 case, ADC offset calibration is performed as
    // appropriate (in the context of battery-specific functions, see also
    // batt_cal_volt).
    adc_offset_calibrate(ADC_INPUT_MODE_SINGLE_ENDED);
#endif

#if defined (__DA14531__)
    batt_volt = battery_get_voltage(batt_type);
#else
    if ((batt_type == BATT_AAA) && (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1))
    {
        // BOOST mode (single AAA battery)
        adc_sample = adc_get_vbat_sample(true);
    }
    else
    {
        adc_sample = adc_get_vbat_sample(false);
    }

    adc_sample = battery_filter_value(adc_sample);
#endif // __DA14531__

#if defined (__DA14531__)
    batt_lvl = batt_cal_lvl[batt_type](batt_volt);
#else
    batt_lvl = batt_cal_lvl[batt_type](adc_sample);
#endif

    return batt_lvl;
}
