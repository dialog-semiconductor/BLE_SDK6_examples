/**
 ****************************************************************************************
 *
 * @file bcs_common.h
 *
 * @brief Header file - Body Composition Service common types.
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef BCS_COMMON_H_
#define BCS_COMMON_H_

/**
 ****************************************************************************************
 * @addtogroup BCS Body Composition Service Server
 * @ingroup BCS
 * @brief Body Composition Service Server
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_BCS_CLIENT || BLE_BCS_SERVER)

#include "prf_types.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Body composition measurement unit
enum {
        /// Weight and mass in kilograms and height in meters
        BCS_UNIT_SI,
        /// Weight and mass in pounds and height in inches
        BCS_UNIT_IMPERIAL,
};

/// Body composition service features
enum {
        /// Time stamp supported
        BCS_FEAT_TIME_STAMP             = 0x000001,
        /// Multiple users supported
        BCS_FEAT_MULTIPLE_USERS         = 0x000002,
        /// Basal metabolism supported
        BCS_FEAT_BASAL_METABOLISM       = 0x000004,
        /// Muscle percentage supported
        BCS_FEAT_MUSCLE_PERCENTAGE      = 0x000008,
        /// Muscle mass supported
        BCS_FEAT_MUSCLE_MASS            = 0x000010,
        /// Fat free mass supported
        BCS_FEAT_FAT_FREE_MASS          = 0x000020,
        /// Soft lean mass supported
        BCS_FEAT_SOFT_LEAN_MASS         = 0x000040,
        /// Body water mass supported
        BCS_FEAT_BODY_WATER_MASS        = 0x000080,
        /// Impedance supported
        BCS_FEAT_IMPEDANCE              = 0x000100,
        /// Weight supported
        BCS_FEAT_WEIGHT                 = 0x000200,
        /// Height supported
        BCS_FEAT_HEIGHT                 = 0x000400,
        /// Resolution of 0.5 kg
        BCS_FEAT_MASS_RES_05_KG         = 0x000800,
        /// Resolution of 0.2 kg
        BCS_FEAT_MASS_RES_02_KG         = 0x001000,
        /// Resolution of 0.1 kg
        BCS_FEAT_MASS_RES_01_KG         = 0x001800,
        /// Resolution of 0.05 kg
        BCS_FEAT_MASS_RES_005_KG        = 0x002000,
        /// Resolution of 0.02 kg
        BCS_FEAT_MASS_RES_002_KG        = 0x002800,
        /// Resolution of 0.01 kg
        BCS_FEAT_MASS_RES_001_KG        = 0x003000,
        /// Resolution of 0.005 kg
        BCS_FEAT_MASS_RES_0005_KG       = 0x003800,
        /// Resolution of 1 lb
        BCS_FEAT_MASS_RES_1_LB          = 0x000800,
        /// Resolution of 0.5 lb
        BCS_FEAT_MASS_RES_05_LB         = 0x001000,
        /// Resolution of 0.2 lb
        BCS_FEAT_MASS_RES_02_LB         = 0x001800,
        /// Resolution of 0.1 lb
        BCS_FEAT_MASS_RES_01_LB         = 0x002000,
        /// Resolution of 0.05 lb
        BCS_FEAT_MASS_RES_005_LB        = 0x002800,
        /// Resolution of 0.02 lb
        BCS_FEAT_MASS_RES_002_LB        = 0x003000,
        /// Resolution of 0.01 lb
        BCS_FEAT_MASS_RES_001_LB        = 0x003800,
        /// Resolution of 0.01 meter
        BCS_FEAT_HEIGHT_RES_001_M       = 0x008000,
        /// Resolution of 0.005 meter
        BCS_FEAT_HEIGHT_RES_005_M       = 0x010000,
        /// Resolution of 0.001 meter
        BCS_FEAT_HEIGHT_RES_0001_M      = 0x018000,
        /// Resolution of 1 inch
        BCS_FEAT_HEIGHT_RES_1_INCH      = 0x008000,
        /// Resolution of 0.5 inch
        BCS_FEAT_HEIGHT_RES_05_INCH     = 0x010000,
        /// Resolution of 0.1 inch
        BCS_FEAT_HEIGHT_RES_01_INCH     = 0x018000,
};

/// Body composition measurement flags
enum {
        BCM_FLAG_UNIT_IMPERIAL          = 0x0001,
        BCM_FLAG_TIME_STAMP             = 0x0002,
        BCM_FLAG_USER_ID                = 0x0004,
        BCM_FLAG_BASAL_METABOLISM       = 0x0008,
        BCM_FLAG_MUSCLE_PERCENTAGE      = 0x0010,
        BCM_FLAG_MUSCLE_MASS            = 0x0020,
        BCM_FLAG_FAT_FREE_MASS          = 0x0040,
        BCM_FLAG_SOFT_LEAN_MASS         = 0x0080,
        BCM_FLAG_BODY_WATER_MASS        = 0x0100,
        BCM_FLAG_IMPEDANCE              = 0x0200,
        BCM_FLAG_WEIGHT                 = 0x0400,
        BCM_FLAG_HEIGHT                 = 0x0800,
        BCM_FLAG_MULTIPLE_PACKET        = 0x1000,
};

/// Body composition timestamp type
typedef struct prf_date_time bcs_date_time_t;

/// Body composition measurement information
typedef struct {
        /// Measurement Unit (applies to all mass, weight and height values)
        uint8_t             measurement_unit;
        /// Time Stamp
        bcs_date_time_t     time_stamp;
        /// User ID from 0 to 255 (special value - unknown user)
        uint8_t             user_id;
        /// Body Fat Percentage (fixed point value)
        uint16_t            body_fat_percentage;
        /// Basal Metabolism in kilo Joules (fixed point value)
        uint16_t            basal_metabolism;
        /// Muscle Percentage (fixed point value)
        uint16_t            muscle_percentage;
        /// Muscle Mass (fixed point value)
        uint16_t            muscle_mass;
        /// Fat Free Mass (fixed point value)
        uint16_t            fat_free_mass;
        /// Soft Lean Mass (fixed point value)
        uint16_t            soft_lean_mass;
        /// Body Water Mass (fixed point value)
        uint16_t            body_water_mass;
        /// Impedance in Ohms (fixed point value)
        uint16_t            impedance;
        /// Weight (fixed point value)
        uint16_t            weight;
        /// Height (fixed point value)
        uint16_t            height;
} bcs_meas_t;

#endif // (BLE_BCS_CLIENT || BLE_BCS_SERVER)

/// @} BCS

#endif // BCS_COMMON_H_
