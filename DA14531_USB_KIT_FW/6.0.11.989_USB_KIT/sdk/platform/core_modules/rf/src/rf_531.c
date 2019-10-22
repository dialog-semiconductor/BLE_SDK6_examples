/**
 ****************************************************************************************
 *
 * @file rf_531.c
 *
 * @brief RF functions DA14531
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

#if defined (__DA14531__)

#include <stdbool.h>
#include <stdint.h>
#include "rf_531.h"
#include "otp_cs.h"

#define FPGA_CTRL_REG (0x500003FC)
volatile struct rf_calibration_result calibration_res;
static uint32_t fsm_ctrl_reg = 0;
volatile uint8_t   pa_en_dcf_tx_setting  = 0xE;

rf_tx_pwr_lvl_t rf_tx_pwr_lvl      __SECTION_ZERO("retention_mem_area0");

#if defined (CFG_ENHANCED_TX_PWR_CTRL)
rf_tx_pwr_lvl_t rf_tx_pwr_conn_lvl[CFG_MAX_CONNECTIONS]  __SECTION_ZERO("retention_mem_area0");
#endif

//void rf_system_init(void)
//{
////    SetBits32(PMU_CTRL_REG, TIM_SLEEP, 0);
////    do {} while ((GetWord32(SYS_STAT_REG) & TIM_IS_UP) != TIM_IS_UP);

////    SetBits32(PMU_CTRL_REG, RADIO_SLEEP, 0);

////    SetBits32(CLK_RADIO_REG, BLE_ENABLE,  1);  // RFCU enabled
////    SetBits32(BLE_CNTL2_REG, BLE_CLK_SEL, 16);
////    SetBits32(CLK_RADIO_REG, RFCU_ENABLE, 1);  // RFCU enabled

//    #ifndef FPGA_SETUP
//      rf_power_up();
//    #endif
//}

/**
 * \brief Preferred settings 531/D2632 radio
 *
 * \param[in]
 *
 * \warning
 */

void set_recommended_settings()
{
// Preferred Settings File for C-Code field mode
// Device             : D2632AC MFix from: D2632AA
// Package            : All packages, no dependency on package.
// Last change date   : May 01, 2019 - 18:56:04
// Last change item   : Register: RF_AGC_LUT_CDEF_REG, Field: VGA1_GAIN13, Value: 0x2
// File date          : June 12, 2019 - 01:18:37


	// Register: ADPLL_ACC_CTRL_REG. TrimFields: CLIP_MOD_TUNE_0_TX,CLIP_MOD_TUNE_0_RX. PrefValues: NONE. ResValues: EN_CMF_AVG=0x1,-=0x0,CLIP_MOD_TUNE_0_TX=0xBF4,-=0x0,CLIP_MOD_TUNE_0_RX=0xBF4. Location: D2632AC/r_radio_2632_01/r_adpll_2632_00/r_adplldig2632_00
	SetWordMasked32(ADPLL_ANA_CTRL_REG, 0x00007F00, 0x02158B7B); // Register: ADPLL_ANA_CTRL_REG. TrimFields: NONE. PrefValues: DTCOFFSET=0xB. ResValues: -=0x0,DTC_LDO_DMY=0x0,-=0x0,VPASETTLE=0x2,-=0x0,TDC_OFFSET=0x15,DTC_EN=0x1,DTCOFFSET=0x0,TGLDETEN=0x0,EN_CKDCOMOD=0x1,INV_CKDCOMOD=0x1,INV_CKPHV=0x1,INV_CKTDC=0x1,TDC_INV=0x0,TDC_CKVIN_EN=0x1,TDC_DTCIN_EN=0x1. Location: D2632AC/r_radio_2632_01/r_adpll_2632_00/r_adplldig2632_00
	SetWordMasked32(ADPLL_DCOAMP_CAL_CTRL_REG, 0xFFFF0000, 0x88880011); // Register: ADPLL_DCOAMP_CAL_CTRL_REG. TrimFields: NONE. PrefValues: DCOAMPIC_LP_TX=0x8,DCOAMPIC_LP_RX=0x8,DCOAMPIC_HP_TX=0x8,DCOAMPIC_HP_RX=0x8. ResValues: DCOAMPIC_LP_TX=0x7,DCOAMPIC_LP_RX=0x7,DCOAMPIC_HP_TX=0x7,DCOAMPIC_HP_RX=0x7,-=0x0,DCOAMPTM=0x0,AMPCALEN=0x0,KMEDIUM=0x2,KCOARSE=0x1. Location: D2632AC/r_radio_2632_01/r_adpll_2632_00/r_adplldig2632_00
    SetWordMasked32(ADPLL_DIV_CTRL_REG, 0x0001FF00, 0x01982003); // Register: ADPLL_DIV_CTRL_REG. TrimFields: TXDIV_TRIM. PrefValues: RXDIV_TRIM=0x20. ResValues: -=0x0,TXDIV_TRIM=0xCC,RXDIV_TRIM=0xFF,-=0x0,RXDIV_FB_EN_TX=0x0,RXDIV_FB_EN_RX=0x1,FBDIV_EN=0x1. Location: D2632AA/r_radio_2632_00/r_adpll_2632_00/r_adplldig2632_00
	// Register: ADPLL_KDCO_CAL_CTRL1_REG. TrimFields: KDCOHFIC_1M. PrefValues: NONE. ResValues: KDCOLF_IN_1M=0x9B,KDCOHFIC_1M=0x9B. Location: D2632AC/r_radio_2632_01/r_adpll_2632_00/r_adplldig2632_00
	// Register: ADPLL_KDCO_CAL_CTRL2_REG. TrimFields: KMOD_ALPHA_1M. PrefValues: NONE. ResValues: KDCOESTDEV=0x1,KDCOCALTX=0x0,KDCOCALRX=0x0,KDCOLFCALEN=0x1,TKDCOCAL=0x4,-=0x0,KDCOCN_IC=0x14,-=0x0,-=0x0,KMOD_ALPHA_1M=0xA. Location: D2632AC/r_radio_2632_01/r_adpll_2632_00/r_adplldig2632_00
	// Register: ADPLL_KDTCTDC_CAL_CTRL1_REG. TrimFields: KDTCIC,KDTC_ALPHA. PrefValues: NONE. ResValues: KDTCIC=0x10A,KDTCCN_IC=0x14,KDTC_PIPELINE_BYPASS=0x0,KTDC_IN=0x10A,KDTC_ALPHA=0x1C. Location: D2632AC/r_radio_2632_01/r_adpll_2632_00/r_adplldig2632_00
	SetWordMasked32(ADPLL_OVERRULE_CTRL1_REG, 0x00000300, 0x00000300); // Register: ADPLL_OVERRULE_CTRL1_REG. TrimFields: NONE. PrefValues: OVR_VPAEN_WR=0x1,OVR_VPAEN_SEL=0x1. ResValues: OVR_DTC_OH_WR=0x0,OVR_DTC_OH_SEL=0x0,-=0x0,OVR_DCOAMP_WR=0x0,OVR_DCOAMP_SEL=0x0,-=0x0,OVR_DCOAMPHOLD_WR=0x0,OVR_DCOAMPHOLD_SEL=0x0,OVR_RDYFORDIV_WR=0x0,OVR_RDYFORDIV_SEL=0x0,OVR_VPAEN_WR=0x0,OVR_VPAEN_SEL=0x0,OVR_SRESETN_WR=0x0,OVR_SRESETN_SEL=0x0,OVR_ENPAIN_WR=0x0,OVR_ENPAIN_SEL=0x0,OVR_RXBIT_WR=0x0,OVR_RXBIT_SEL=0x0,OVR_ACTIVE_WR=0x0,OVR_ACTIVE_SEL=0x0. Location: D2632AC/r_radio_2632_01/r_adpll_2632_00/r_adplldig2632_00
	SetWordMasked16(ADPLL_RFPT_CTRL_REG, 0x0010, 0x0010); // Register: ADPLL_RFPT_CTRL_REG. TrimFields: NONE. PrefValues: INV_CKRFPT=0x1. ResValues: -=0x0,-=0x0,RFPT_RATE=0x0,INV_CKRFPT=0x0,RFPT_MUX=0x0. Location: D2632AC/r_radio_2632_01/r_adpll_2632_00/r_adplldig2632_00
	SetWord32(BLE_RADIOPWRUPDN_REG, 0x075C0559); // Register: BLE_RADIOPWRUPDN_REG. TrimFields: NONE. PrefValues: RTRIP_DELAY=0x7,RXPWRUP=0x5C,TXPWRDN=0x5,TXPWRUP=0x59. ResValues: RTRIP_DELAY=0x0,RXPWRUP=0xD2,TXPWRDN=0x3,TXPWRUP=0xD2. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/ble2632_00
	// Register: CLK_RC32K_REG. TrimFields: RC32K_TRIM. PrefValues: NONE. ResValues: RC32K_TRIM=0x7,RC32K_DISABLE=0x0. Location: D2632AC/dcore2632_AC/crg2632_00
	// Register: CLK_RC32M_REG. TrimFields: RC32M_BIAS. PrefValues: NONE. ResValues: RC32M_COSC=0xF,RC32M_RANGE=0x0,RC32M_BIAS=0x7,RC32M_DISABLE=0x0. Location: D2632AC/dcore2632_AC/crg2632_00
// "set in system_init()" SetWordMasked16(DCDC_CTRL_REG, 0xF006, 0xF4A0); // Register: DCDC_CTRL_REG. TrimFields: NONE. PrefValues: DCDC_ILIM_MAX=0xF,DCDC_CLK_DIV=0x0. ResValues: DCDC_ILIM_MAX=0x8,DCDC_ILIM_MIN=0x4,DCDC_OK_CLR_CNT=0x2,DCDC_TIMEOUT=0x4,DCDC_CLK_DIV=0x1,DCDC_ENABLE=0x0. Location: D2632AC/dcore2632_AC/crg2632_00/crg2632_dcdc_dig_00
	// Register: GP_ADC_TRIM_REG. TrimFields: GP_ADC_LDO_LEVEL,GP_ADC_OFFS_SH_VREF. PrefValues: NONE. ResValues: GP_ADC_LDO_LEVEL=0x3,GP_ADC_OFFS_SH_VREF=0x8. Location: D2632AC/dcore2632_AC/peripheral_subsystem2632_00/adc2632_00/adc2632_bif_00
// "set in system_init()" SetWordMasked16(POWER_AON_CTRL_REG, 0x4000, 0x4008); // Register: POWER_AON_CTRL_REG. TrimFields: NONE. PrefValues: FORCE_RUNNING_COMP_DIS=0x1. ResValues: FORCE_RUNNING_COMP_DIS=0x0,LDO_RET_TRIM=0x0,CMP_VCONT_SLP_DISABLE=0x0,BOOST_MODE_FORCE=0x0,CHARGE_VBAT_DISABLE=0x0,RC32K_LOW_SPEED_FORCE=0x0,RC32K_HIGH_SPEED_FORCE=0x0,POR_VBAT_HIGH_RST_MASK=0x1,POR_VBAT_LOW_RST_MASK=0x0,VBAT_HL_CONNECT_RES_CTRL=0x0. Location: D2632AC/dcore2632_AC/aon_subsystem2632_00/crg2632_preg_aon_00
// "set in system_init()" SetWordMasked16(POWER_CTRL_REG, 0x4000, 0x0000); // Register: POWER_CTRL_REG. TrimFields: NONE. PrefValues: POR_VBAT_HIGH_HYST_DIS=0x0. ResValues: VBAT_HL_CONNECT_MODE=0x0,POR_VBAT_HIGH_HYST_DIS=0x1,POR_VBAT_HIGH_HYST_SEL=0x0,POR_VBAT_HIGH_DISABLE=0x0,POR_VBAT_LOW_HYST_DIS=0x0,POR_VBAT_LOW_HYST_SEL=0x0,POR_VBAT_LOW_DISABLE=0x0,CP_DISABLE=0x0,LDO_VREF_HOLD_FORCE=0x0,LDO_LOW_CTRL_REG=0x0,LDO_CORE_DISABLE=0x0,LDO_CORE_RET_ENABLE=0x0,VBAT_HL_CONNECT=0x0,CMP_VBAT_HIGH_OK_ENABLE=0x0,CMP_VBAT_HIGH_NOK_ENABLE=0x0. Location: D2632AC/dcore2632_AC/crg2632_00
// "set in system_init()" SetWordMasked16(POWER_LEVEL_REG, 0x000E, 0x1A36); // Register: POWER_LEVEL_REG. TrimFields: NONE. PrefValues: LDO_LOW_TRIM=0x3. ResValues: DCDC_TRIM=0x3,DCDC_LEVEL=0x1,LDO_CORE_RET_CUR_TRIM=0x0,LDO_XTAL_TRIM=0x3,LDO_LOW_TRIM=0x7,LDO_CORE_LEVEL=0x0. Location: D2632AC/dcore2632_AC/crg2632_00
	SetWordMasked16(RF_AFC_CTRL_REG, 0x03F0, 0x02F5); // Register: RF_AFC_CTRL_REG. TrimFields: NONE. PrefValues: AFC_SYNCF_DIRECT=0x1,APD_MODE=0x3,PAD_MODE=0x3. ResValues: -=0x0,AFC_SYNCF_DIRECT=0x0,APD_MODE=0x0,PAD_MODE=0x1,AFC_MODE=0x5. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_dem2632_00
	SetWordMasked32(RF_AGC_CTRL1_REG, 0x001FFFFF, 0x005A5809); // Register: RF_AGC_CTRL1_REG. TrimFields: NONE. PrefValues: AGC_TH2_HIGH=0x69,AGC_TH_HIGH=0x30,AGC_TH_LOW=0x9. ResValues: -=0x0,AGC_MODE=0x2,AGC_TH2_HIGH=0x5B,AGC_TH_HIGH=0x2A,AGC_TH_LOW=0xD. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_dem2632_00
	SetWordMasked32(RF_AGC_CTRL2_REG, 0x00001E01, 0x0040128C); // Register: RF_AGC_CTRL2_REG. TrimFields: NONE. PrefValues: AGC_MAX_SETTING=0x9,FASTAGC_EN=0x0. ResValues: EN_FRZ_GAIN=0x1,AGCSETTING_WR=0x0,AGCSETTING_SEL=0x0,-=0x0,AGC_MAX_SETTING=0xA,AGCDEC_WAIT=0x1,AGCINC_WAIT=0x0,FASTAGC_WAIT=0x1,FASTAGC_STEPS=0x2,FASTAGC_EN=0x1. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_dem2632_00
	SetWordMasked32(RF_AGC_LUT_0123_REG, 0xFFFF1F1F, 0x49290908); // Register: RF_AGC_LUT_0123_REG. TrimFields: NONE. PrefValues: LNA_GAIN3=0x2,VGA1_GAIN3=0x2,VGA2_GAIN3=0x1,LNA_GAIN2=0x1,VGA1_GAIN2=0x2,VGA2_GAIN2=0x1,VGA1_GAIN1=0x2,VGA2_GAIN1=0x1,VGA1_GAIN0=0x2,VGA2_GAIN0=0x0. ResValues: LNA_GAIN3=0x1,VGA1_GAIN3=0x0,VGA2_GAIN3=0x3,LNA_GAIN2=0x0,VGA1_GAIN2=0x0,VGA2_GAIN2=0x3,LNA_GAIN1=0x0,VGA1_GAIN1=0x0,VGA2_GAIN1=0x2,LNA_GAIN0=0x0,VGA1_GAIN0=0x0,VGA2_GAIN0=0x1. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_dem2632_00
	SetWordMasked32(RF_AGC_LUT_4567_REG, 0xFCFCFFFF, 0x6F6B6A69); // Register: RF_AGC_LUT_4567_REG. TrimFields: NONE. PrefValues: LNA_GAIN7=0x3,VGA1_GAIN7=0x3,LNA_GAIN6=0x3,VGA1_GAIN6=0x2,LNA_GAIN5=0x3,VGA1_GAIN5=0x2,VGA2_GAIN5=0x2,LNA_GAIN4=0x3,VGA1_GAIN4=0x2,VGA2_GAIN4=0x1. ResValues: LNA_GAIN7=0x1,VGA1_GAIN7=0x4,VGA2_GAIN7=0x3,LNA_GAIN6=0x1,VGA1_GAIN6=0x3,VGA2_GAIN6=0x3,LNA_GAIN5=0x1,VGA1_GAIN5=0x2,VGA2_GAIN5=0x3,LNA_GAIN4=0x1,VGA1_GAIN4=0x1,VGA2_GAIN4=0x3. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_dem2632_00
	SetWordMasked32(RF_AGC_LUT_89AB_REG, 0x0000E0FC, 0x77777773); // Register: RF_AGC_LUT_89AB_REG. TrimFields: NONE. PrefValues: LNA_GAIN9=0x3,LNA_GAIN8=0x3,VGA1_GAIN8=0x4. ResValues: LNA_GAIN11=0x3,VGA1_GAIN11=0x5,VGA2_GAIN11=0x3,LNA_GAIN10=0x3,VGA1_GAIN10=0x5,VGA2_GAIN10=0x3,LNA_GAIN9=0x2,VGA1_GAIN9=0x5,VGA2_GAIN9=0x3,LNA_GAIN8=0x1,VGA1_GAIN8=0x5,VGA2_GAIN8=0x3. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_dem2632_00
	SetWordMasked32(RF_AGC_LUT_CDEF_REG, 0x1F1F1F1F, 0x080A2A4B); // Register: RF_AGC_LUT_CDEF_REG. TrimFields: NONE. PrefValues: VGA1_GAIN15=0x2,VGA2_GAIN15=0x0,VGA1_GAIN14=0x2,VGA2_GAIN14=0x2,VGA1_GAIN13=0x2,VGA2_GAIN13=0x2,VGA1_GAIN12=0x2,VGA2_GAIN12=0x3. ResValues: LNA_GAIN15=0x0,VGA1_GAIN15=0x0,VGA2_GAIN15=0x1,LNA_GAIN14=0x0,VGA1_GAIN14=0x0,VGA2_GAIN14=0x2,LNA_GAIN13=0x1,VGA1_GAIN13=0x0,VGA2_GAIN13=0x2,LNA_GAIN12=0x2,VGA1_GAIN12=0x0,VGA2_GAIN12=0x3. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_dem2632_00

/* RF_ATTR_REG: PA_POWER_SETTING is set by SDK
    SetWordMasked32(RF_ATTR_REG, 0x00001000, 0x0C001820); // Register: RF_ATTR_REG. TrimFields: NONE. PrefValues: TIA_BIAS=0x1. ResValues: PA_POWER_SETTING=0xC,TIA_BIAS=0x0,RF_BIAS=0x8,-=0x1,IFF_POLARITY=0x0. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00
*/
#if defined (CFG_ENHANCED_TX_PWR_CTRL)
    SetWord32(RF_ATTR_REG, (0x1 << 12U) | (0x8 << 8U) | (0x0 << 3U));
#else
    SetWord32(RF_ATTR_REG, (rf_tx_pwr_lvl << 24U) | (0x1 << 12U) | (0x8 << 8U) | (0x0 << 3U));
#endif
    SetWord16(RF_CNTRL_TIMER_11_REG, 0x1400); // Register: RF_CNTRL_TIMER_11_REG. TrimFields: NONE. PrefValues: RESET_OFFSET=0x14,SET_OFFSET=0x0. ResValues: RESET_OFFSET=0x8,SET_OFFSET=0x1A. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWord16(RF_CNTRL_TIMER_12_REG, 0x1405); // Register: RF_CNTRL_TIMER_12_REG. TrimFields: NONE. PrefValues: RESET_OFFSET=0x14,SET_OFFSET=0x5. ResValues: RESET_OFFSET=0xE,SET_OFFSET=0x1A. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWordMasked16(RF_CNTRL_TIMER_19_REG, 0x00FF, 0x0855); // Register: RF_CNTRL_TIMER_19_REG. TrimFields: NONE. PrefValues: SET_OFFSET=0x55. ResValues: RESET_OFFSET=0x8,SET_OFFSET=0x50. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWord16(RF_CNTRL_TIMER_21_REG, 0x0234); // Register: RF_CNTRL_TIMER_21_REG. TrimFields: NONE. PrefValues: RESET_OFFSET=0x2,SET_OFFSET=0x34. ResValues: RESET_OFFSET=0x0,SET_OFFSET=0x0. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWord16(RF_CNTRL_TIMER_22_REG, 0x0A34); // Register: RF_CNTRL_TIMER_22_REG. TrimFields: NONE. PrefValues: RESET_OFFSET=0xA,SET_OFFSET=0x34. ResValues: RESET_OFFSET=0x0,SET_OFFSET=0x0. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWord16(RF_CNTRL_TIMER_23_REG, 0x0851); // Register: RF_CNTRL_TIMER_23_REG. TrimFields: NONE. PrefValues: RESET_OFFSET=0x8,SET_OFFSET=0x51. ResValues: RESET_OFFSET=0x0,SET_OFFSET=0x0. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWord16(RF_CNTRL_TIMER_24_REG, 0x0A26); // Register: RF_CNTRL_TIMER_24_REG. TrimFields: NONE. PrefValues: RESET_OFFSET=0xA,SET_OFFSET=0x26. ResValues: RESET_OFFSET=0x0,SET_OFFSET=0x0. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWord16(RF_CNTRL_TIMER_25_REG, 0x0858); // Register: RF_CNTRL_TIMER_25_REG. TrimFields: NONE. PrefValues: RESET_OFFSET=0x8,SET_OFFSET=0x58. ResValues: RESET_OFFSET=0x0,SET_OFFSET=0x0. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWord16(RF_DC_OFFSET_CTRL3_REG, 0x120C); // Register: RF_DC_OFFSET_CTRL3_REG. TrimFields: NONE. PrefValues: DCBETA_Q=0x12,DCBETA_I=0xC. ResValues: DCBETA_Q=0xDC,DCBETA_I=0xE4. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_dem2632_00
	SetWordMasked32(RF_DEM_ATTR_REG, 0x00001810, 0x00C4B99F); // Register: RF_DEM_ATTR_REG. TrimFields: NONE. PrefValues: DCOC_SPEED8M_EN=0x1,AGC_SPEED8M_EN=0x1,RXDATA_INV=0x1. ResValues: -=0x0,ADDR_TIMEOUT_X2=0x0,RFADC_CLK_PHASE=0x3,RSSI_TH=0x9,DCPOLE=0x1,DCOC_SPEED8M_EN=0x0,AGC_SPEED8M_EN=0x0,-=0x0,MATCH0101_TH=0x6,DEM_HSI_POL=0x0,RXDATA_INV=0x0,AFC_POLE2=0x3,AFC_POLE1=0x3. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_dem2632_00
	SetWord16(RF_ENABLE_CONFIG0_REG, 0x016B); // Register: RF_ENABLE_CONFIG0_REG. TrimFields: NONE. PrefValues: RFIO_LDO_EN_DCF_TX=0xB,RFIO_LDO_EN_DCF_RX=0xB. ResValues: RFIO_LDO_EN_DCF_TX=0x4,RFIO_LDO_EN_DCF_RX=0x3. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWordMasked16(RF_ENABLE_CONFIG14_REG, 0x03E0, 0x02C0); // Register: RF_ENABLE_CONFIG14_REG. TrimFields: NONE. PrefValues: RFIO_TX_EN_DCF_TX=0x16. ResValues: RFIO_TX_EN_DCF_TX=0xE,RFIO_TX_EN_DCF_RX=0x0. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWordMasked16(RF_ENABLE_CONFIG17_REG, 0x03E0, 0x0250); // Register: RF_ENABLE_CONFIG17_REG. TrimFields: NONE. PrefValues: RFIO_BIAS_SH_OPEN_DCF_TX=0x12. ResValues: RFIO_BIAS_SH_OPEN_DCF_TX=0x10,RFIO_BIAS_SH_OPEN_DCF_RX=0x10. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWordMasked16(RF_ENABLE_CONFIG18_REG, 0x03E0, 0x02A0); // Register: RF_ENABLE_CONFIG18_REG. TrimFields: NONE. PrefValues: PA_RAMP_EN_DCF_TX=0x15. ResValues: PA_RAMP_EN_DCF_TX=0xD,PA_RAMP_EN_DCF_RX=0x0. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWordMasked16(RF_ENABLE_CONFIG19_REG, 0x03E0, 0x02C0); // Register: RF_ENABLE_CONFIG19_REG. TrimFields: NONE. PrefValues: PA_EN_DCF_TX=0x16. ResValues: PA_EN_DCF_TX=0xE,PA_EN_DCF_RX=0x0. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWord16(RF_ENABLE_CONFIG1_REG, 0x018C); // Register: RF_ENABLE_CONFIG1_REG. TrimFields: NONE. PrefValues: PA_LDO_EN_DCF_TX=0xC,PA_LDO_EN_DCF_RX=0xC. ResValues: PA_LDO_EN_DCF_TX=0x4,PA_LDO_EN_DCF_RX=0x3. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWordMasked16(RF_ENABLE_CONFIG34_REG, 0x03E0, 0x0300); // Register: RF_ENABLE_CONFIG34_REG. TrimFields: NONE. PrefValues: ADPLL_LOBUF_PA_EN_DCF_TX=0x18. ResValues: ADPLL_LOBUF_PA_EN_DCF_TX=0xE,ADPLL_LOBUF_PA_EN_DCF_RX=0x0. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWordMasked16(RF_ENABLE_CONFIG39_REG, 0x001F, 0x0019); // Register: RF_ENABLE_CONFIG39_REG. TrimFields: NONE. PrefValues: DEM_SIGDETECT_EN_DCF_RX=0x19. ResValues: SPARE_DEM_SIGDETECT_EN_DCF_TX=0x0,DEM_SIGDETECT_EN_DCF_RX=0x13. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00/rfcu2522_power_00
	SetWordMasked16(RF_IFF_CTRL_REG, 0x6000, 0x2001); // Register: RF_IFF_CTRL_REG. TrimFields: NONE. PrefValues: IFF_DCOC_DAC_REFCUR_CTRL=0x1. ResValues: IFF_DCOC_DAC_REFCUR_CTRL=0x0,IFF_COMPLEX_DIS=0x0,RF_IFF_CTRL_SPARE=0x0,IFF_DCOC_DAC_DIS=0x0,IF_MUTE=0x0,IF_CAL_TRIM=0x1. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00
	// Register: RF_LNA_CTRL1_REG. TrimFields: LNA_TRIM_GAIN4_HP,LNA_TRIM_GAIN3_HP,LNA_TRIM_GAIN2_HP,LNA_TRIM_GAIN1_HP,LNA_TRIM_GAIN0_HP. PrefValues: NONE. ResValues: LNA_TRIM_GAIN4_HP=0x10,LNA_TRIM_GAIN3_HP=0x10,LNA_TRIM_GAIN2_HP=0x10,LNA_TRIM_GAIN1_HP=0x10,LNA_TRIM_GAIN0_HP=0x10. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00
	// Register: RF_LNA_CTRL2_REG. TrimFields: LNA_TRIM_GAIN4_LP,LNA_TRIM_GAIN3_LP,LNA_TRIM_GAIN2_LP,LNA_TRIM_GAIN1_LP,LNA_TRIM_GAIN0_LP. PrefValues: NONE. ResValues: LNA_TRIM_GAIN4_LP=0xD,LNA_TRIM_GAIN3_LP=0xD,LNA_TRIM_GAIN2_LP=0xD,LNA_TRIM_GAIN1_LP=0xD,LNA_TRIM_GAIN0_LP=0xD. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00
	SetWordMasked32(RF_LNA_CTRL3_REG, 0x00000007, 0x00111113); // Register: RF_LNA_CTRL3_REG. TrimFields: LNA_MODE_GAIN4_LP,LNA_MODE_GAIN3_LP,LNA_MODE_GAIN2_LP,LNA_MODE_GAIN1_LP,LNA_MODE_GAIN0_LP. PrefValues: LNA_TRIM_CASC=0x3. ResValues: LNA_SPARE=0x0,LNA_MODE_GAIN4_LP=0x1,LNA_MODE_GAIN3_LP=0x1,LNA_MODE_GAIN2_LP=0x1,LNA_MODE_GAIN1_LP=0x1,LNA_MODE_GAIN0_LP=0x1,-=0x0,LNA_TRIM_CASC=0x4. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00
	SetWord32(RF_MIXER_CTRL1_REG, 0x01500150); // Register: RF_MIXER_CTRL1_REG. TrimFields: NONE. PrefValues: MIXER_IP2_DAC_Q_TRIM=0x150,MIXER_IP2_DAC_I_TRIM=0x150. ResValues: MIXER_IP2_DAC_Q_TRIM=0x10F,MIXER_IP2_DAC_I_TRIM=0x10F. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00
	// Register: RF_PA_CTRL_REG. TrimFields: TRIM_DUTY_NEG,TRIM_DUTY_POS. PrefValues: NONE. ResValues: PA_RAMP_STEP_SPEED=0x3,TRIM_DUTY_NEG=0x0,TRIM_DUTY_POS=0x0. Location: D2632AC/dcore2632_AC/radio_subsystem2632_00/r_rfcu2632_00
// "set in system_init()" SetWordMasked16(XTAL32M_CTRL0_REG, 0x001C, 0x0011); // Register: XTAL32M_CTRL0_REG. TrimFields: NONE. PrefValues: CORE_CUR_SET=0x4. ResValues: XTAL32M_SPARE=0x0,CORE_AMPL_TRIM=0x0,CORE_CUR_SET=0x5,CORE_AMPL_REG_NULLBIAS=0x0,DCBLOCK_ENABLE=0x1. Location: D2632AC/dcore2632_AC/crg2632_00
    SetWordMasked16(ADPLL_TXMOD_CTRL_REG, 0x0001, 0x0001); // Register: ADPLL_TXMOD_CTRL_REG. TrimFields: NONE. PrefValues: BT_SEL=0x1. ResValues: INV_CKMODEXT=0x0,TX_MODE=0x0,-=0x0,EO_PACKET_DIS=0x0,MOD_INDEX=0x0,TX_DATA_INV=0x0,BT_SEL=0x0. Location: D2632AC/r_radio_2632_01/r_adpll_2632_00/r_adplldig2632_00

//  ---------------------- 
//  - POSSIBLE OMISSIONS - 
//  ---------------------- 

// Register bit-fields which have a preferred setting which relates to a different device:
// REG: 'RF_ENABLE_CONFIG15_REG'	BIT-FIELD: 'RFIO_TX_HARM_EN_DCF_TX'	DEVICE: 'D2522AA'	PACKAGE: 'any'
// REG: 'RF_ENABLE_CONFIG15_REG'	BIT-FIELD: 'RFIO_TX_HARM_EN_DCF_TX'	DEVICE: 'D2522AB'	PACKAGE: 'any'
// REG: 'RF_ENABLE_CONFIG15_REG'	BIT-FIELD: 'RFIO_TX_HARM_EN_DCF_TX'	DEVICE: 'D2674AA'	PACKAGE: 'any'
// REG: 'RF_ENABLE_CONFIG15_REG'	BIT-FIELD: 'RFIO_TX_HARM_EN_DCF_TX'	DEVICE: 'D2763AA'	PACKAGE: 'any'
// REG: 'RF_ENABLE_CONFIG15_REG'	BIT-FIELD: 'RFIO_TX_HARM_EN_DCF_TX'	DEVICE: 'D2634AA'	PACKAGE: 'any'
// REG: 'RF_ENABLE_CONFIG15_REG'	BIT-FIELD: 'RFIO_TX_HARM_EN_DCF_TX'	DEVICE: 'D2775AA'	PACKAGE: 'any'
// REG: 'RF_ENABLE_CONFIG38_REG'	BIT-FIELD: 'DEM_AGC_UNFREEZE_EN_DCF_RX'	DEVICE: 'D2632AA'	PACKAGE: 'any'
}


bool dcoff_calibration(void)
{
       union _rf_dem_attr_reg_u {
                uint32_t raw;
                struct __RF_DEM_ATTR_REG bf;
        };
        bool retval = false;

        // Save the values of registers that will be changed in the function
        uint32_t rf_enable_config2_reg_value = GetWord32(RF_ENABLE_CONFIG2_REG);
        uint32_t rf_enable_config20_reg_value = GetWord32(RF_ENABLE_CONFIG20_REG);
        uint32_t rf_enable_config21_reg_value = GetWord32(RF_ENABLE_CONFIG21_REG);
        uint32_t rf_dc_offset_ctrl2_reg_value = GetWord32(RF_DC_OFFSET_CTRL2_REG);
        uint32_t rf_cal_ctrl_reg_value = GetWord32(RF_CAL_CTRL_REG);
        uint32_t rf_enable_config37_reg_value = GetWord32(RF_ENABLE_CONFIG37_REG);
        uint32_t rf_rfcu_ctrl_reg_value = GetWord32(RF_RFCU_CTRL_REG);
        uint32_t rf_attr_reg_value = GetWord32(RF_ATTR_REG);
        uint32_t adpll_attr_ctrl_reg_value = GetWord32(ADPLL_ATTR_CTRL_REG);

        // Switch RFCU to 8MHz
        SetBits(RF_RFCU_CTRL_REG, RF_RFCU_CLK_DIV, 0x1);

        // Set RADIO_REGS_RDY to prevent CM_PHY_ERR after starting the DC-offset calibration
        SetByte(RF_RADIO_INIT_REG + 0x2, RADIO_REGS_RDY >> 16);

        // Required setting for the DC-offset calibration
        SetWord32(RF_ENABLE_CONFIG20_REG, 0x0000);  // Disable LNA_CORE
        SetWord32(RF_ENABLE_CONFIG21_REG, 0x0000);  // Disable LNA_CGM
        SetWord32(RF_DC_OFFSET_CTRL2_REG, 0x00F8A494);
        SetBits32(RF_DC_OFFSET_CTRL2_REG, DCPARCAL_EN, 0x1);
        SetBits32(RF_DC_OFFSET_CTRL2_REG, DCPAR_DCF_EN, 0x0);
        SetBits32(RF_DC_OFFSET_CTRL2_REG, DCPAR_MULT_STEPS_EN, 0x1);
        SetBits32(RF_DC_OFFSET_CTRL2_REG, DCPARCAL_INIT, 0x2);
        SetWord32(RF_ENABLE_CONFIG37_REG, 0x0000);  // Disable the DCF triggered partial DCOC

        // Clear eo-cal interrupt, such that new calibration can be stared
        SetWord32(RF_IRQ_CTRL_REG, 0x0000);

        do {
                // Wait until the DC-offset calibration can be started
                WAIT10();//delay_us(5);
                if ((GetWord32(RF_CAL_CTRL_REG) & EO_CAL) != 0) {
                        break;
                };

                // Start the DC-offset calibration
                SetBits(RF_CAL_CTRL_REG, SO_CAL, 1);

                // Wait until the DC-offset calibration has started (roughly 3 usec)
                WAIT100();//delay_us(15);
                if ((GetWord32(RF_CAL_CTRL_REG) & SO_CAL) == 0) {
                        break;
                };

                // Wait until the DC-offset calibration is completed (roughly 220 usec)
                WAIT1000();//delay_us(300);
                if ((GetWord32(RF_CAL_CTRL_REG) & SO_CAL) != 0) {
                        break;
                };

                retval = true;
        } while (0);

        // Clear eo-cal interrupt, such that new calibration can be started
        SetWord32(RF_IRQ_CTRL_REG, 0x0000);

        /* Be aware that an RF calibration raises the exception: CM_EXC_STAT_REG.EXC_PHY after the
         * calibration is ready. This can be disabled but it is better to avoid it. Due to this
         * exception, the HW_GEN_IRQ is triggered. This exception must be acknowledged in the
         * HW_GEN_IRQ ISR. If this is not done, the processor will keep entering the HW_GEN_IRQ
         * continuously till the CMAC WDOG expires.
         */

        // Restore the values of registers that were changed in the function
        SetWord32(RF_ENABLE_CONFIG2_REG, rf_enable_config2_reg_value);
        SetWord32(RF_ENABLE_CONFIG20_REG, rf_enable_config20_reg_value);
        SetWord32(RF_ENABLE_CONFIG21_REG, rf_enable_config21_reg_value);
        SetWord32(RF_CAL_CTRL_REG, rf_cal_ctrl_reg_value);
        SetWord32(RF_ENABLE_CONFIG37_REG, rf_enable_config37_reg_value);
        SetWord32(RF_RFCU_CTRL_REG, rf_rfcu_ctrl_reg_value);
        SetWord32(RF_DC_OFFSET_CTRL2_REG, rf_dc_offset_ctrl2_reg_value);
        SetWord32(RF_ATTR_REG, rf_attr_reg_value);
        SetWord32(ADPLL_ATTR_CTRL_REG, adpll_attr_ctrl_reg_value);

        return retval;
}

bool kdtc_calibration(void)
{
        uint32_t kdtc_res;

        /* ======================================================================================
         * Start KDTC MOD1
         * ====================================================================================== */
        en_hclk();
        SetBits(ADPLL_CN_CTRL_REG, CN, 20);     // do set the initial calibration at channel 20
        kdtc_cal_init();        // initial settings for KDTC calibration
        en_kdtc_cal_mod1();     // first enable KDTC calibration mode 1
        dis_hclk();             // disable HCLK to reduce interference.
        en_adpll_tx();
        WAIT400();//delay_us(100);
        en_hclk();
        kdtc_res = GetBits(ADPLL_KDTC_RD_REG, KDTC_OUT);
        //calibration_res.kdtc_result[1] = kdtc_res;
        dis_adpll();
        WAIT10();//delay_us(10);

        /* ======================================================================================
         * Start KDTC MOD0
         * ====================================================================================== */
        // first update the calibrated value to ADPLL before calibration
        SetBits(ADPLL_KDTCTDC_CAL_CTRL1_REG, KDTCIC, kdtc_res);
        SetBits(ADPLL_KDTCTDC_CAL_CTRL1_REG, KTDC_IN, kdtc_res);
        en_kdtc_cal_mod0();     // first enable KDTC calibration mode 1
        dis_hclk();             // disable HCLK to reduce interference.
        en_adpll_tx();
        WAIT400();//delay_us(100);
        en_hclk();
        kdtc_res = GetBits(ADPLL_KDTC_RD_REG, KDTC_OUT);
        //calibration_res.kdtc_result[2] = kdtc_res;
        dis_adpll();
        WAIT10();//delay_us(10);


        /* ======================================================================================
         * Start KDTC MOD0 again (for better accuracy)
         * ====================================================================================== */
        // first update the calibrated value to ADPLL before calibration
        SetBits(ADPLL_KDTCTDC_CAL_CTRL1_REG, KDTCIC, kdtc_res);
        SetBits(ADPLL_KDTCTDC_CAL_CTRL1_REG, KTDC_IN, kdtc_res);
        en_kdtc_cal_mod0();     // first enable KDTC calibration mode 1
        dis_hclk();             // disable HCLK to reduce interference.
        en_adpll_tx();
        WAIT400();//delay_us(100);
        en_hclk();
        kdtc_res = GetBits(ADPLL_KDTC_RD_REG, KDTC_OUT);
        SetBits(ADPLL_KDTCTDC_CAL_CTRL1_REG, KDTCIC, kdtc_res);
        SetBits(ADPLL_KDTCTDC_CAL_CTRL1_REG, KTDC_IN, kdtc_res);
        dis_kdtc_cal();
        kdtc_cal_end();
        dis_adpll();
        WAIT10();//delay_us(10);

        calibration_res.kdtc_result[0] = kdtc_res;

        return true;
}

bool kdco_calibration(void)
{
       // open loop kdco calibration
        uint32_t kdco_res;

        uint32_t cc, cm, cf;
        // uint32_t delay_time;
        uint32_t f1, f2;
        // lock at ch20
        en_hclk();
        SetBits(ADPLL_CN_CTRL_REG, CN, 20);
        en_adpll_tx();
        WAIT400();//delay_us(120);

        // readout cmf
        cc = GetBits(ADPLL_DCO_RD_REG, DCOCOARSE);
        cm = GetBits(ADPLL_DCO_RD_REG, DCOMEDIUM);
        cf = GetBits(ADPLL_DCO_RD_REG, DCOFINE);

        // override cmf
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOCOARSE_WR, cc);
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOMEDIUM_WR, cm);
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOFINE_WR, cf);
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOMOD_WR, 96);

        SetBits(ADPLL_OVERRULE_CTRL1_REG, OVR_ACTIVE_WR, 0);
        SetBits(ADPLL_OVERRULE_CTRL1_REG, OVR_ACTIVE_SEL, 1);
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOCOARSE_SEL, 1);
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOMEDIUM_SEL, 1);
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOFINE_SEL, 1);
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOMOD_SEL, 1);

        // en rfmeas
        SetBits(ADPLL_MON_CTRL_REG, ENRFMEAS, 1);
        SetBits(ADPLL_MON_CTRL_REG, TFREQMEAS, 31);

        // start first slot freq meas
        SetBits(ADPLL_OVERRULE_CTRL1_REG, OVR_ACTIVE_WR, 1);

        // while(GetBits(ADPLL_FREQMEAS_RD_REG, MEASDONE_OUT)==0)
        // {
            // delay_usec_long_cmac(50);
        // }
        WAIT1000();//delay_us(500);
        f1  = GetBits(ADPLL_FREQMEAS_RD_REG, FREQDIFF);
        calibration_res.kdtc_result[5] = f1;

        // Second slot meas
        SetBits(ADPLL_OVERRULE_CTRL1_REG, OVR_ACTIVE_WR, 0);
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOMOD_WR, 160);
        SetBits(ADPLL_OVERRULE_CTRL1_REG, OVR_ACTIVE_WR, 1);
        // while(GetBits(ADPLL_FREQMEAS_RD_REG, MEASDONE_OUT)==0)
        // {
            // delay_usec_long_cmac(50);
        // }
        WAIT1000();//delay_us(500);
        f2  = GetBits(ADPLL_FREQMEAS_RD_REG, FREQDIFF);
        calibration_res.kdtc_result[6] = f2;

        // uint32_t lco = (f1-f2)/448.5*2000/32
        // ldco = 4e3/ldco --> ldco = 4e3*448.5*32/2000/(f1-f2) = 2*448.5*64/f1-f2
        uint32_t ldco = 57408/(f1-f2);
        calibration_res.kdtc_result[7] = ldco;
        uint32_t exp_kdco = (ldco>>8)&1;
        uint32_t lin_kdco = (ldco>>exp_kdco)&127;
        kdco_res = (exp_kdco<<7)+lin_kdco+3;
        SetBits(ADPLL_OVERRULE_CTRL1_REG, OVR_ACTIVE_SEL, 0);
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOCOARSE_SEL, 0);
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOMEDIUM_SEL, 0);
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOFINE_SEL, 0);
        SetBits(ADPLL_OVERRULE_CTRL2_REG, OVR_DCOMOD_SEL, 0);
        SetBits(ADPLL_MON_CTRL_REG, ENRFMEAS, 0);
        dis_adpll();
        WAIT10();//delay_us(10);

        calibration_res.kdco_result[0] = kdco_res;

        SetBits(ADPLL_KDCO_CAL_CTRL1_REG, KDCOHFIC_1M, kdco_res);
        SetBits(ADPLL_KDCO_CAL_CTRL1_REG, KDCOLF_IN_1M, kdco_res);

        return true;
}

void rf_calibration(void)
{
    //dcoff_calibration(); //Jan 01 2019 : This is actually adding more DC offset than reducing it. 
    kdtc_calibration();
}



/*********************************************************************************
 *** rf_power_up()
 *** About: Radio & ADPLL LDO activation.
 ***        Also preload any costant ADPLL/Modulator/etc. registers
 ***/
void rf_power_up(void)
{
#if 0
    #ifdef DTM_SPECIFIC_FUNCTIONS
        rf_ldo_cont_mode_en();
        WAIT10();
        WAIT100();
        rf_adplldig_ldo_on();
        WAIT100();
        en_hclk();
    #endif
#endif
}

void rf_power_down(void)
{
//    SetWord32(RF_RADIO_INIT_REG, 0x0);
}

/*********************************************************************************
 *** rf_adplldig_ldo_on()
 *** About: Activate the ADPLL Dig related LDOs.
 ***        After calling this function you have to wait for some time to stabilize before accessing the register file.
 ***/
void rf_adplldig_ldo_on(void)
{
   // Activate LDO
    SetByte(RF_RADIO_INIT_REG, (ADPLLDIG_PWR_SW1_EN | RADIO_LDO_EN));
}  // rf_adplldig_ldo_on()


/*********************************************************************************
 *** rf_adplldig_txmod()
 *** About: Set pulse shaper settings for ADPLLDIG
 ***/
void rf_adplldig_txmod(void)
{
// Modulation settings (for 1 both 2Mbps)
    SetBits(ADPLL_TXMOD_CTRL_REG, TX_DATA_INV, 1);

}  // rf_adplldig_txmod()



// disable hclk for the ADPLL AHB
void dis_hclk(void)
{
   SetByte(RF_RADIO_INIT_REG + 2, ADPLLDIG_HCLK_DIS >> 16);
   WAIT();//delay_us(2);
}


// Enable hclk for the ADPLL AHB
void en_hclk(void)
{
        SetByte(RF_RADIO_INIT_REG + 0x1, (ADPLLDIG_HRESET_N | ADPLLDIG_HCLK_EN) >> 8);
}

// Disable ADPLL manual activation
void dis_adpll(void)
{
        SetWord32(RF_OVERRULE_REG, 0);
        WAIT100();//delay_usec_long_cmac(20);

        // restore setting
        SetBits(RF_ENABLE_CONFIG19_REG, PA_EN_DCF_TX, pa_en_dcf_tx_setting);
}

// Manually activate ADPLL in TX mode
void en_adpll_tx(void)
{
        // disable pa output during calibration
        pa_en_dcf_tx_setting = GetBits(RF_ENABLE_CONFIG19_REG, PA_EN_DCF_TX);
        SetBits(RF_ENABLE_CONFIG19_REG, PA_EN_DCF_TX, 0);

        SetByte(RF_RADIO_INIT_REG + 0x2, RADIO_REGS_RDY >> 16);
        SetBits(RF_OVERRULE_REG, TX_EN_OVR, 3);
        WAIT100(); // wait  30 us

}

void kdtc_cal_init(void)
{
        /* Set the kdtc calibration initial conditions. Mainly set ADPLL timing to max to ensure
         * stable operation. Back up registers in the memory, will write them back when calibration
         * is not used.
         */
        fsm_ctrl_reg = GetWord32(ADPLL_FSM_CTRL_REG);                   // back up fsm_ctrl_reg register
        SetWord32(ADPLL_FSM_CTRL_REG, 0xFFFFFFFF);                      // set all fsm timing to max
        SetBits(ADPLL_KDTCTDC_CAL_CTRL1_REG, KDTC_ALPHA, 0);            // disable kdtc alpha compensation
        SetBits(ADPLL_KDTCTDC_CAL_CTRL1_REG, KDTC_PIPELINE_BYPASS, 0);  // disable KDTC pipeline (may not be necessary)
        SetBits(ADPLL_KDTCTDC_CAL_CTRL2_REG, PHRDLY, 2);
        SetBits(ADPLL_KDTCTDC_CAL_CTRL2_REG, KDTCCALLG, 4);
        SetBits(ADPLL_LF_CTRL2_REG, MODKZ, 22);
        SetBits(ADPLL_LF_CTRL2_REG, MODK_TUNE, 24);
        SetBits(ADPLL_LF_CTRL2_REG, MODTAU_TUNE, 24);
}


void kdtc_cal_end(void)
{
        // restore values
        SetWord32(ADPLL_FSM_CTRL_REG, fsm_ctrl_reg );                   // restore fsm_ctrl_reg register
        SetBits(ADPLL_KDTCTDC_CAL_CTRL1_REG, KDTC_ALPHA, 0x1C);         // restore the reset value
        SetBits(ADPLL_KDTCTDC_CAL_CTRL1_REG, KDTC_PIPELINE_BYPASS, 0);  // disable KDTC pipeline (may not be necessary)
        SetBits(ADPLL_LF_CTRL2_REG, MODKZ, 28);
        SetBits(ADPLL_LF_CTRL2_REG, MODK_TUNE, 30);
        SetBits(ADPLL_LF_CTRL2_REG, MODTAU_TUNE, 30);
}


// Set parameters for KDTC calibration mode 1
// (KDTCCALEN = 1, KDTCCALMOD = 1, KDTCCALMOD1P = 1, KDTCCALLG = 4)
void en_kdtc_cal_mod1(void)
{
        SetByte(ADPLL_KDTCTDC_CAL_CTRL2_REG, 0x43);
}


// Set parameters for KDTC calibration mode 0 (KDTCCALEN = 1, KDTCCALMOD = 0, KDTCCALLG = 5)
void en_kdtc_cal_mod0(void)
{
        SetByte(ADPLL_KDTCTDC_CAL_CTRL2_REG, 0x41);
        SetBits(ADPLL_KDTCTDC_CAL_CTRL2_REG, PHRDLY, 2);
}


// Disable KDTC calibration (KDTCCALEN = 0)
void dis_kdtc_cal(void)
{
        SetBits(ADPLL_KDTCTDC_CAL_CTRL2_REG, KDTCCALEN, 0);
}


void kdco_cal_init(void)
{
        // Disable kdco alpha compensation
        SetByte(ADPLL_KDCO_CAL_CTRL2_REG, 0);
        fsm_ctrl_reg = GetWord32(ADPLL_FSM_CTRL_REG);   // back up fsm_ctrl_reg register
        SetWord32(ADPLL_FSM_CTRL_REG, 0xFFFFFFFF);      // set all fsm timing to max
}


void kdco_cal_end(void)
{
        // Restore values
        SetWord32(ADPLL_FSM_CTRL_REG, fsm_ctrl_reg);

        SetByte(ADPLL_KDCO_CAL_CTRL2_REG, 0xA);         // reset value, be aware in case of 1M
}


//static void en_kdco_cal(bool txmod)
//{
//        // Initial settings for KDCO calibration depend on TX or RX
//        if (txmod) {
//                SetBits(ADPLL_KDCO_CAL_CTRL2_REG, KDCOCALTX, 1);
//                SetBits(ADPLL_KDCO_CAL_CTRL2_REG, KDCOCALRX, 0);
//        } else {
//                SetBits(ADPLL_KDCO_CAL_CTRL2_REG, KDCOCALTX, 0);
//                SetBits(ADPLL_KDCO_CAL_CTRL2_REG, KDCOCALRX, 1);
//        }
//}


void dis_kdco_cal(void)
{
        SetBits(ADPLL_KDCO_CAL_CTRL2_REG, KDCOCALTX, 0);
        SetBits(ADPLL_KDCO_CAL_CTRL2_REG, KDCOCALRX, 0);
}

/*********************************************************************************
 *** rf_ldo_cont_mode_en()
 *** About: Activate radio LDOs.
 ***/
void rf_ldo_cont_mode_en(void)
{
    SetBits32(RF_LDO_CTRL_REG, LDO_RADIO_CONT_ENABLE, 1);
    SetBits32(RF_LDO_CTRL_REG, LDO_DTC_CONT_ENABLE, 1);
    SetBits32(RF_LDO_CTRL_REG, LDO_DCO_CONT_ENABLE, 1);

    SetBits32(RF_LDO_CTRL_REG, LDO_DCO_HOLD_OVR_EN,1);
    SetBits32(RF_LDO_CTRL_REG, LDO_DCO_HOLD_OVR_VAL,0);
    SetBits32(RF_LDO_CTRL_REG, LDO_DTC_HOLD_OVR_EN,1);
    SetBits32(RF_LDO_CTRL_REG, LDO_DTC_HOLD_OVR_VAL,0);
    SetBits32(RF_LDO_CTRL_REG, LDO_RADIO_HOLD_OVR_EN,1);
    SetBits32(RF_LDO_CTRL_REG, LDO_RADIO_HOLD_OVR_VAL,0);
}  // rf_ldo_cont_mode_en()

void rf_adplldig_activate(void)
{
    // Enable ADPLL related LDOs
    SetByte(RF_RADIO_INIT_REG, (ADPLLDIG_PWR_SW1_EN | RADIO_LDO_EN));

    // wait for some time (5us) to stabilize before accessing the ADPLL register file
    WAIT()

    // Enable ADPLL HCLK and reset ADPLL register file
    SetByte(RF_RADIO_INIT_REG + 0x1, (ADPLLDIG_HRESET_N | ADPLLDIG_HCLK_EN) >> (8 * 0x1));
}

void rf_adplldig_deactivate(void)
{
    // Disable hclk and set RADIO_REGS_RDY to 1 at the same time
    SetByte(RF_RADIO_INIT_REG + 0x2, ((ADPLLDIG_HCLK_DIS | RADIO_REGS_RDY) >> (8 * 0x2)));

    // Disable ADPLL related LDOs
    SetByte(RF_RADIO_INIT_REG, 0);
}

#if defined (CFG_ENHANCED_TX_PWR_CTRL)
void rf_pa_pwr_adv_set(rf_tx_pwr_lvl_t level)
{
    rf_tx_pwr_lvl = level;

    for (uint32_t i = 0; i < CFG_MAX_CONNECTIONS; i++)
    {
        rf_tx_pwr_conn_lvl[i] = level;
    }
}

void rf_pa_pwr_conn_set(rf_tx_pwr_lvl_t level, uint8_t conidx)
{
    rf_tx_pwr_conn_lvl[conidx] = level;
}
#else
void rf_pa_pwr_set(rf_tx_pwr_lvl_t level)
{
    rf_tx_pwr_lvl = level;
}

rf_tx_pwr_lvl_t rf_pa_pwr_get(void)
{
    return rf_tx_pwr_lvl;
}
#endif

/*
 * NEAR FIELD MODE API
 ****************************************************************************************
 */

static bool near_field_mode        __SECTION_ZERO("retention_mem_area0");
#if !defined (CFG_ENHANCED_TX_PWR_CTRL)
static uint32_t saved_tx_power     __SECTION_ZERO("retention_mem_area0");
#endif

bool rf_nfm_is_enabled(void)
{
    return near_field_mode;
}

void rf_nfm_enable(void)
{
    if (!near_field_mode)
    {
        near_field_mode = true;
#if !defined (CFG_ENHANCED_TX_PWR_CTRL)
        saved_tx_power = GetBits32(RF_ATTR_REG, PA_POWER_SETTING);
#endif
    }
}

void rf_nfm_disable(void)
{
    if (near_field_mode)
    {
        near_field_mode = false;
#if !defined (CFG_ENHANCED_TX_PWR_CTRL)
        rf_pa_pwr_set((rf_tx_pwr_lvl_t) saved_tx_power);
#endif
    }
}

#endif  // __DA14531__
