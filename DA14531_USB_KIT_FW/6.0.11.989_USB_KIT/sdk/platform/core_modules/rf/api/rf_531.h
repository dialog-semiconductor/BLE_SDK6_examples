/**
 ****************************************************************************************
 *
 * @file rf_531.h
 *
 * @brief RF functions DA14531 header file.
 *
 * Copyright (c) 2018-2019 Dialog Semiconductor. All rights reserved.
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

#ifndef _RF_531_H_
#define _RF_531_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdbool.h>
#include <stdint.h>
#include "datasheet.h"
#include "compiler.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define KDCO_RESULT_SZ          (8)
#define KDTC_RESULT_SZ          (8)
#define TX_DIV2_RESULT_SZ       (8)
#define IQ_DIV2_RESULT_SZ       (8)

/// Defines the transmit output power level from -18dBm up to 3dBm
typedef enum
{
    /// -18 dBm
    RF_TX_PWR_LVL_MINUS_18d0     = 1U,

    /// -12 dBm
    RF_TX_PWR_LVL_MINUS_12d0     = 2U,

    /// -8.6 dBm
    RF_TX_PWR_LVL_MINUS_8d6      = 3U,

    /// -6.2 dBm
    RF_TX_PWR_LVL_MINUS_6d2      = 4U,

    /// -4.3 dBm
    RF_TX_PWR_LVL_MINUS_4d3      = 5U,

    /// -2.8 dBm
    RF_TX_PWR_LVL_MINUS_2d8      = 6U,

    /// -1.5 dBm
    RF_TX_PWR_LVL_MINUS_1d5      = 7U,

    /// -0.3 dBm
    RF_TX_PWR_LVL_MINUS_0d3      = 8U,

    /// +0.6 dBm
    RF_TX_PWR_LVL_PLUS_0d6       = 9U,

    /// +1.5 dBm
    RF_TX_PWR_LVL_PLUS_1d5       = 10U,

    /// +2.3 dBm
    RF_TX_PWR_LVL_PLUS_2d3       = 11U,

    /// +3.1 dBm
    RF_TX_PWR_LVL_PLUS_3d1       = 12U,
} rf_tx_pwr_lvl_t;

#define WAIT()     for (volatile int wait=0; wait < 0xB;  wait++) {}
#define WAIT10()   for (volatile int wait=0; wait < 0x10;  wait++) {}
#define WAIT100()   for (volatile int wait=0; wait < 0x100;  wait++) {} // wait  30 us
#define WAIT400()   for (volatile int wait=0; wait < 0x400;  wait++) {} // wait  120 us
#define WAIT1000()   for (volatile int wait=0; wait < 0x1000;  wait++) {} // wait  500 us

#define DA14531
void rf_system_init(void);
void set_recommended_settings(void);
bool dcoff_calibration(void);
bool kdtc_calibration(void);
bool kdco_calibration(void);
void rf_calibration(void);
void rf_power_up(void);
void rf_ldo_cont_mode_en(void);
void rf_adplldig_ldo_on(void);
void rf_adplldig_attr(uint32_t);
void rf_adplldig_regf_rst(void);
void rf_adplldig_txmod(void);
void rf_prepare_tx(uint8_t, uint8_t);
void rf_prepare_rx(uint8_t, uint8_t, uint8_t, uint8_t);
void switch_xtal_clk(void);
void dis_hclk(void);
void en_hclk(void);
void kdtc_cal_init(void);
void kdtc_cal_end(void);
void en_kdtc_cal_mod1(void);
void en_kdtc_cal_mod0(void);
void dis_kdtc_cal(void);
void kdco_cal_init(void);
void kdco_cal_end(void);
void dis_kdco_cal(void);
void en_adpll_tx(void);
void dis_adpll(void);

/**
 *****************************************************************************************
 * @brief Activate ADPLL.
 * @return void
 *****************************************************************************************
 */
void rf_adplldig_activate(void);

/**
 *****************************************************************************************
 * @brief Deactivate ADPLL.
 * @return void
 *****************************************************************************************
 */
void rf_adplldig_deactivate(void);

#if defined (CFG_ENHANCED_TX_PWR_CTRL)
/**
 ****************************************************************************************
 * @brief Sets the transmit output power level of advertising events through the ARP Table.
 * @details Initializes the transmit output power level of connection events as well.
 * @param[in] level The transmit output power level.
 * @return void
 ****************************************************************************************
 */
void rf_pa_pwr_adv_set(rf_tx_pwr_lvl_t level);

/**
 ****************************************************************************************
 * @brief Sets the transmit output power level of connection events through the ARP Table.
 * @param[in] level The transmit output power level.
 * @param[in] conidx The connection index.
 * @return void
 ****************************************************************************************
 */
void rf_pa_pwr_conn_set(rf_tx_pwr_lvl_t level, uint8_t conidx);
#else
/**
 *****************************************************************************************
 * @brief Selects the Tx output power level. The selected value will be applied to 
 *        hardware when @p set_recommended_settings() is called.
 * @param[in] level    The Tx output power level from -18dBm up to 3dBm
 * @return void
 *****************************************************************************************
 */
void rf_pa_pwr_set(rf_tx_pwr_lvl_t level);

/**
 *****************************************************************************************
 * @brief Get the Tx output power level value saved in retained memory block.
 * @return the Tx output power level value.
 * @note It may be different than the value read from RF_ATTR_REG.PA_POWER_SETTING field.
 *****************************************************************************************
 */
__WEAK rf_tx_pwr_lvl_t rf_pa_pwr_get(void);
#endif

/// Structure holding the results of the calibration
struct rf_calibration_result {
        ///DC-offset result
        uint8_t dcoff_result;
        ///KDCO result
        uint8_t kdco_result[KDCO_RESULT_SZ];
        ///KDTC result
        uint16_t kdtc_result[KDTC_RESULT_SZ];
        ///Tx-Div2 result
        uint8_t tx_div2_result[TX_DIV2_RESULT_SZ];
        ///IQ-Div2 result
        uint8_t iq_div2_result[IQ_DIV2_RESULT_SZ];
};
extern volatile struct rf_calibration_result calibration_res;

/*
 * NEAR FIELD MODE API
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Checks if Near Field mode is enabled.
 * @return true if Near Field mode is enabled, otherwise false.
 ****************************************************************************************
 */
bool rf_nfm_is_enabled(void);

/**
 ****************************************************************************************
 * @brief Enables Near Field mode for all connections. Tx output power is set to -18dBm.
 * @return void
 ****************************************************************************************
 */
void rf_nfm_enable(void);

/**
 ****************************************************************************************
 * @brief Disables Near Field mode for all connections. The previous Tx output power is
 * restored.
 * @return void
 ****************************************************************************************
 */
void rf_nfm_disable(void);

#endif //_RF_531_H_
