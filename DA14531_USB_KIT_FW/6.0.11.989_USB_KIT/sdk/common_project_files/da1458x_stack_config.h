/**
 ****************************************************************************************
 *
 * @file da1458x_stack_config.h
 *
 * @brief RW stack configuration file.
 *
 * Copyright (c) 2014-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef DA1458X_STACK_CONFIG_H_
#define DA1458X_STACK_CONFIG_H_

/////////////////////////////////////////////////////////////
/* Do not alter */

#define CFG_EMB
#define CFG_HOST
#define CFG_ALLROLES        1
#define CFG_GTL
#define CFG_BLE
#define CFG_EXT_DB
#if defined (__DA14531__)
    #define CFG_CON             3
#else
    #define CFG_CON             8
#endif
#define CFG_SECURITY_ON     1
#define CFG_ATTC
#define CFG_ATTS
#define CFG_PRF
#define CFG_NB_PRF          32
#define CFG_H4TL

#define CFG_CHNL_ASSESS

/* Enable power optimizations */
#define CFG_POWER_OPTIMIZATIONS

/*Misc*/
#define __NO_EMBEDDED_ASM

/****************************************************************************************************************/
/* Use the 580 PLL LUT RF calibration or the 585 VCO coarse RF calibration.                                     */
/* - DEFINE   : 580 PLL LUT RF calibration.                                                                     */
/* - UNDEFINE : 585 VCO coarse RF calibration (must be kept undefined by default).                              */
/****************************************************************************************************************/
#undef CFG_LUT_PATCH

/*
 * Automatically enable RF DIAG interrupt:
 * - DA14585/586: To be enabled for PLL LUT and production test.
 * - DA14531: To be enabled for production test.
 */
#if !defined (__DA14531__)              // DA14585/586
    #if defined (CFG_PRODUCTION_TEST) || defined (CFG_LUT_PATCH)
        #define CFG_RF_DIAG_INT
    #else
        #undef CFG_RF_DIAG_INT
    #endif
#else                                   // DA14531
    #if defined (CFG_PRODUCTION_TEST)
        #define CFG_RF_DIAG_INT
    #else
        #undef CFG_RF_DIAG_INT
    #endif
#endif

#if defined (__FPGA__)
    #define XTAL_32000HZ
#endif

/*Radio interface*/
#if defined (__FPGA__)
    #if defined (__DA14531__)
        #define RADIO_ANALOG        0
        #define RADIO_EXPOSED       1
    #else
        #define RADIO_ANALOG        0
        #define RADIO_EXPOSED       0
    #endif

    #define RIPPLE_ID           66
    #define RADIO_585           0
#else
    #define RADIO_ANALOG        0
    #define RADIO_RIPPLE        0
#endif

#if !defined (__DA14531__) && defined (CFG_LUT_PATCH) && !defined (CFG_RF_DIAG_INT)
#error "DA14585/586: If PLL LUT is used the CFG_RF_DIAG_INT flag must be defined"
#endif

#endif // DA1458X_STACK_CONFIG_H_
