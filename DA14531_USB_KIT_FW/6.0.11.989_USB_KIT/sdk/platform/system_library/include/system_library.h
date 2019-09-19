/**
 ****************************************************************************************
 *
 * @file system_library.h
 *
 * @brief System library header file.
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

#ifndef _SYSTEM_LIBRARY_H_
#define _SYSTEM_LIBRARY_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>

/**
 ****************************************************************************************
 * @addtogroup SYSTEM_LIBRARY
 * @brief System Library
 * @{
 ****************************************************************************************
 */

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// ROM Patch utilities
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief ROM patch function.
 * @return void
 ****************************************************************************************
 */
void patch_func(void);

/**
 ****************************************************************************************
 * @brief Initializes global variables used by the ROM patched functions.
 * @return void
 ****************************************************************************************
 */
void patch_global_vars_init(void);

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// RF Calibration utilities
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/*
 * DEFINES
 ****************************************************************************************
 */

#define RC16_CAL_REF_CYCLES           (0x400)
#define RF_DIAG_IRQ_MODE_RXTX         (1)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void enable_rf_diag_irq(uint8_t mode);

void set_gauss_modgain(uint16_t cn);

bool IffCalibration_580(void);
bool DCoffsetCalibration_580(void);
bool rf_calibration_580(void);

bool rf_calibration_585(void);
extern void PllLF_IFF_MIX_CapCalibration(void); // defined in ROM
extern void DCoffsetCalibration(void);          // defined in ROM

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// PLL LUT utilities (used by the old 580 calibration method)
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/*
 * DEFINES
 ****************************************************************************************
 */

// Visible to the user
#define LUT_INIT        (0)
#define LUT_UPDATE      (1)

struct LUT_CFG_struct
{
    int8_t      HW_LUT_MODE;              // 1: HW LUT mode, 0: SW LUT mode using ISR
    int8_t      RX_HSI_ENABLED;           // 1: HSI mode, 0:LSI mode
    int8_t      PA_PULLING_OFFSET;        // in channel numbers
    int8_t      NR_CCUPD_1ST;             // max number of real time iterations during the first min_cn calcap update routine
    int8_t      NR_CCUPD_REST;            // max number of real time iterations during the rest min_cn / max_cn updates
    int8_t      NR_CCUPD_OL;              // Extra number of iterations during the LUT INIT (non real time mode). Should be less than 126 in total
    int8_t      BLE_BAND_MARGIN;          // Extention of BLE band in MHz, to make sure that we keep updating enough number of calcaps
    int8_t      EST_HALF_OVERLAP;         // Estimated half overlap between successive calcaps, in MHz
    int8_t      REQUIRED_CHAN_OVERLAP;    // Min required overlap in channels
    uint8_t     PLL_LOCK_VTUNE_NUMAVGPOW;
    int16_t     PLL_LOCK_VTUNE_LIMIT_LO;
    int16_t     PLL_LOCK_VTUNE_LIMIT_HI;
    int16_t     PLL_LOCK_VTUNE_P2PVAR;
    int16_t     PLL_LOCK_TIMING;
    uint16_t    VCO_CALCNT_STARTVAL;
    uint16_t    VCO_CALCNT_TIMEOUT;
};

extern const struct LUT_CFG_struct LUT_CFG;

extern volatile uint16_t vcocal_ctrl_reg_val;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

uint8_t pll_vcocal_LUT_InitUpdate(uint8_t mode);

void set_rf_cal_cap(uint16_t cn); // This is used for overrule based testing

/**
 * @brief Verify CRC of OTP calibration values and apply them to the device.
 *
 * @return true on success else false.
 *
 * @note This function assumes that the system clock frequency is 16MHz.
 */
bool otp_verify_crc(void);

/// @} SYSTEM_LIBRARY

#endif // _SYSTEM_LIBRARY_H_
