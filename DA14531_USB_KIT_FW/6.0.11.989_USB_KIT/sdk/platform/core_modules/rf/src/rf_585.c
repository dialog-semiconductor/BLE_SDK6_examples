/**
 ****************************************************************************************
 *
 * @file rf_585.c
 *
 * @brief DA14585/586 RF driver.
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

#if !defined (__DA14531__)

/*
 * INCLUDES
 ****************************************************************************************
 */

#include "rwip_config.h"        // RW SW configuration
#include <stdint.h>
#include <stdbool.h>
#include "co_utils.h"           // common utility definition
#include "co_math.h"            // common math functions
#include "co_endian.h"          // endian definitions
#include "rf.h"                 // RF interface
#include "rwip.h"               // for RF API structure definition

#if (BLE_EMB_PRESENT)
#include "reg_blecore.h"        // ble core registers
#include "reg_ble_em_cs.h"      // control structure definitions
#endif //BLE_EMB_PRESENT

#include "rf_585.h"
#include "system_library.h"
#include "system_settings.h"
#include "arch.h"
#include "arch_system.h"
#include "reg_access.h"

#if (USE_RANGE_EXT)
#include "range_ext_api.h"
#include "sky66111.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */
#define EM_BLE_FREQ_TABLE_LEN       EM_RF_FREQ_TABLE_LEN

#define RXPWRUP_VAL                 0x54
#define TXPWRUP_VAL                 0x4C

/*
 * GLOBAL VARIABLES DECLARATION
 ****************************************************************************************
 */

extern uint16_t last_temp_count;
extern uint16_t iq_trim_bias_ctrl1_reg;       // stored in retained RAM
extern uint16_t iq_trim_rf_mixer_ctrl1_reg;   // stored in retained RAM

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 *****************************************************************************************
 * @brief Dummy function. Must be defined in RF driver API function.
 * @param[in] address
 * @return void
 *****************************************************************************************
 */
static uint32_t rf_reg_rd(uint16_t address)
{
    return 0;
}

/**
 *****************************************************************************************
 * @brief Dummy function. Must be defined in RF driver API function.
 * @param[in] address
 * @param[in] data
 * @return void
 *****************************************************************************************
 */
static void rf_reg_wr(uint16_t address, uint32_t data)
{

}

/**
 *****************************************************************************************
 * @brief Init RF sequence after reset. Not used.
 * @return void
 *****************************************************************************************
 */
static void rf_reset(void)
{

}

/**
 *****************************************************************************************
 * @brief Enable/disable force AGC mechanism. Not used.
 * @param[in]  en       True: Enable / False: disable.
 * @return void
 *****************************************************************************************
 */
static void rf_force_agc_enable(bool en)
{

}

/**
 *****************************************************************************************
 * @brief Get TX power in dBm from the index in the control structure.
 * @param[in] txpwr_idx     Index of the TX power in the control structure.
 * @param[in] modulation    Modulation: 1 or 2 or 3 MBPS.
 * @return The TX power in dBm.
 *****************************************************************************************
 */
static uint8_t rf_txpwr_dbm_get(uint8_t txpwr_idx, uint8_t modulation)
{
    return 0;
}

/**
 *****************************************************************************************
 * @brief Sets RF module in sleep mode
 * @return void
 *****************************************************************************************
 */
static void rf_sleep(void)
{
    ble_deepslcntl_set(ble_deepslcntl_get() |
                      BLE_DEEP_SLEEP_ON_BIT |    // RW BLE Core sleep
                      BLE_RADIO_SLEEP_EN_BIT |   // Radio sleep
                      BLE_OSC_SLEEP_EN_BIT);     // Oscillator sleep
   // ble_deepslcntl_set(ble_deepslcntl_get() | BLE_DEEP_SLEEP_ON_BIT );
}

/**
 *****************************************************************************************
 * @brief Radio control handler. Not used.
 * @return void
 *****************************************************************************************
 */
static void RADIOCNTL_Handler(void)
{

}

/**
 *****************************************************************************************
 * @brief Radio RSSI convertion routine. Passed to BLE core / RF api structure.
 * @param[in] rssi_reg      RF measured RSSI.
 * @return Converted RSSI.
 *****************************************************************************************
 */
static uint8_t rf_rssi_convert(uint8_t rssi_reg)
{
    // NOTE: Approximate conversion corresponding to the following formula:
    // dBm = (0.474f * rssi) - 112.4f.
    return ((rssi_reg >> 1U) - 112U);
}

/*
 * RADIO FUNCTION INTERFACE
 ****************************************************************************************
 */

void rf_regs(void)
{
    SetWord32(BLE_RADIOPWRUPDN_REG, PREF_BLE_RADIOPWRUPDN_REG);
    SetWord16(RF_DC_OFFSET_CTRL3_REG, PREF_RF_DC_OFFSET_CTRL3_REG);
    SetWordMasked16(RF_SPARE1_REG, 0x0006, 0x0002);

#if (LUT_PATCH_ENABLED)
    SetBits16(RF_VCOCAL_CTRL_REG, VCO_FREQTRIM_SEL, 0x1);
#else
    SetWord16(RF_VCOCAL_CTRL_REG, PREF_RF_VCOCAL_CTRL_REG);
#endif

    SetWord16(RF_MIXER_CTRL1_REG, iq_trim_rf_mixer_ctrl1_reg);
    SetWord16(BIAS_CTRL1_REG, iq_trim_bias_ctrl1_reg);

    //  Near Field Mode Enable
    if(rf_nfm_is_enabled())
    {
        SetWord16(RF_ENABLE_CONFIG13_REG, 0x0030);
    }
}

void rf_init_func(struct rwip_rf_api *api)
{
    uint8_t idx = 0;
    uint8_t temp_freq_tbl[EM_BLE_FREQ_TABLE_LEN];

   // Initialize the RF driver API structure
    api->reg_rd = rf_reg_rd;
    api->reg_wr = rf_reg_wr;
    api->txpwr_dbm_get = rf_txpwr_dbm_get;

    //api->txpwr_max = RPL_POWER_MAX;
    api->sleep = rf_sleep;
    api->reset = rf_reset;
    #ifdef CFG_BLE
    api->isr = RADIOCNTL_Handler;
    api->force_agc_enable = rf_force_agc_enable;
    #endif //CFG_BLE

    api->rssi_convert = rf_rssi_convert;

    #ifdef CFG_BT
    api->txpwr_inc = rf_txpwr_inc;
    api->txpwr_dec = rf_txpwr_dec;
    api->txpwr_epc_req = rf_txpwr_epc_req;
    api->txpwr_cs_get = rf_txpwr_cs_get;

    api->rssi_high_thr = (uint8_t)RPL_RSSI_20dB_THRHLD;
    api->rssi_low_thr = (uint8_t)RPL_RSSI_60dB_THRHLD;
    api->rssi_interf_thr = (uint8_t)RPL_RSSI_70dB_THRHLD;
    #ifdef CFG_BTCORE_30
    api->wakeup_delay = RPL_WK_UP_DELAY;
    #endif //CFG_BTCORE_30
    api->skew = RPL_RADIO_SKEW;
    #endif //CFG_BT

    SetBits32(BLE_RADIOCNTL1_REG, XRFSEL, 2);
    SetBits32(BLE_CNTL2_REG, SW_RPL_SPI ,0);
    SetBits32(BLE_CNTL2_REG, BB_ONLY,0);

    while(idx < EM_BLE_FREQ_TABLE_LEN)
    {
        temp_freq_tbl[idx] = idx ;
        idx++;
    }

    em_ble_burst_wr(&temp_freq_tbl[0], EM_FT_OFFSET, EM_BLE_FREQ_TABLE_LEN);
    rf_regs();

    last_temp_count = get_rc16m_count();

#if (LUT_PATCH_ENABLED)
    pll_vcocal_LUT_InitUpdate(LUT_INIT);
#endif

#if (LUT_PATCH_ENABLED)
    rf_calibration_580();
#else
    rf_calibration_585();
#endif
}

void rf_reinit_func(void)
{
    SetBits32(BLE_RADIOCNTL1_REG, XRFSEL, 2);
    SetBits32(BLE_RWBLECNTL_REG, SYNCERR, 0); //this must be always '0'

    SetBits16(CLK_RADIO_REG, RFCU_DIV, 1); //RFCU clock must always be 8MHz!
    SetBits16(CLK_RADIO_REG, RFCU_ENABLE, 1);

    if ( arch_clk_is_XTAL32( ) )
    {
        //If LP clock is XTAL32 in Boost mode set XTAL32K_CUR to 1.
        if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
            SetBits16(CLK_32K_REG,  XTAL32K_CUR, 1);
    }

    rf_regs();

#if LUT_PATCH_ENABLED
    const volatile struct LUT_CFG_struct *pLUT_CFG;
    pLUT_CFG = &LUT_CFG;
    if (pLUT_CFG->HW_LUT_MODE)
    {
        SetWord16(RF_VCOCAL_CTRL_REG, vcocal_ctrl_reg_val);
    }
#endif //LUT_PATCH_ENABLED

#if defined (CFG_RF_DIAG_INT)
    enable_rf_diag_irq(RF_DIAG_IRQ_MODE_RXTX);    // This just enables the TX_EN int. RX_EN int enable status remains as it was
#endif

#if (USE_RANGE_EXT)
    range_ext.re_init();
#endif
}

/*
 * NEAR FIELD MODE API
 ****************************************************************************************
 */

// Indicates if Near Field mode is active.
static bool near_field_mode     __SECTION_ZERO("retention_mem_area0");

bool rf_nfm_is_enabled(void)
{
    return near_field_mode;
}

void rf_nfm_enable(void)
{
    near_field_mode = true;
    SetWord16(RF_ENABLE_CONFIG13_REG, 0x0030);
}

void rf_nfm_disable(void)
{
    near_field_mode = false;
    SetWord16(RF_ENABLE_CONFIG13_REG, PREF_RF_ENABLE_CONFIG13_REG);
}

#endif

///@} RF_RPL
