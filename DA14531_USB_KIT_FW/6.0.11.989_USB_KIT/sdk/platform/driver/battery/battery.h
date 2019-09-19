/**
 ****************************************************************************************
 *
 * @file battery.h
 *
 * @brief Battery driver header file.
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
 
#ifndef _BATTERY_H_
#define _BATTERY_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>

/*
 * DEFINES
 ****************************************************************************************
 */

#if defined (__DA14531__)
typedef enum
{
    /// CR2032 lithium cell battery
    BATT_CR2032   = 0,

    /// Alkaline cell battery
    BATT_ALKALINE = 1
} batt_t;
#else
typedef enum
{
    /// CR2032 lithium cell battery
    BATT_CR2032 = 0,

    /// CR1225 lithium cell battery
    BATT_CR1225 = 1,

    /// AAA alkaline battery (1 cell in boost, 2 cells in buck mode)
    BATT_AAA    = 2
} batt_t;

#define BATTERY_MEASUREMENT_BOOST_AT_1V5    (0x340)
#define BATTERY_MEASUREMENT_BOOST_AT_1V0    (0x230)
#define BATTERY_MEASUREMENT_BOOST_AT_0V9    (0x1F0)
#define BATTERY_MEASUREMENT_BOOST_AT_0V8    (0x1B0)

#define BATTERY_MEASUREMENT_BUCK_AT_3V0     (0x6B0)
#define BATTERY_MEASUREMENT_BUCK_AT_2V8     (0x640)
#define BATTERY_MEASUREMENT_BUCK_AT_2V6     (0x5D0)
#define BATTERY_MEASUREMENT_BUCK_AT_2V4     (0x550)
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Returns voltage of the @p batt_type battery.
 * @param[in] batt_type Battery type.
 * @return battery voltage in mV
 ****************************************************************************************
 */
uint16_t battery_get_voltage(const batt_t batt_type);
#endif

/**
 ****************************************************************************************
 * @brief Returns battery level for @p batt_type.
 * @param[in] batt_type Battery type.
 * @return battery level (0-100%)
 ****************************************************************************************
 */
uint8_t battery_get_lvl(const batt_t batt_type);

#endif // _BATTERY_H_
