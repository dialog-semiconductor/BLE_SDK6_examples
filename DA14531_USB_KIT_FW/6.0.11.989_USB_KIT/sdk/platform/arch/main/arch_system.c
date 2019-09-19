/**
 ****************************************************************************************
 *
 * @file arch_system.c
 *
 * @brief System setup.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
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
 * INCLUDES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "datasheet.h"
#include "arch.h"
#include "arch_api.h"
#include "arch_wdg.h"
#include "arch_system.h"
#include "gpio.h"
#include "hw_otpc.h"
#include "otp_hdr.h"
#include "user_periph_setup.h"

#if !defined (__NON_BLE_EXAMPLE__)
#include "user_callback_config.h"
#include "rwip.h"     // BLE initialization
#include "llc.h"
#include "gtl_env.h"
#include "gtl_task.h"
#include "gap_cfg.h"
#include "co_math.h"
#include "system_library.h"
#include "nvds.h"
#include "app.h"
#include "rwble.h"
#include "da1458x_scatter_config.h"
#include "arch_rom.h"
#if defined (__DA14531__)
#include "rf_531.h"
#else
#include "rf_585.h"
#endif
#endif // (__NON_BLE_EXAMPLE__)

#if (USE_TRNG)
#include "trng.h"       // True random number generator API
#endif

#if defined (__DA14531__)
#include "adc.h"
#include "otp_cs.h"
#include "syscntl.h"
#endif

#if (USE_RANGE_EXT)
#include "range_ext_api.h"
#include "sky66111.h"
#endif

#if ((EXTERNAL_WAKEUP) && (!BLE_APP_PRESENT)) // external wake up, only in full embedded designs
#include "wkupct_quadec.h"
#endif

#if defined (__DA14586__)
#include "spi_flash.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */

#if defined (__DA14531__)
#define DEFAULT_XTAL32M_TRIM_VALUE_QFN      (0x80)
#define DEFAULT_XTAL32M_TRIM_VALUE_WLCSP    (0x6E)
#else
// DA14585/DA14586 case
#if (HW_CONFIG_PRO_DK)
    #define DEFAULT_XTAL16M_TRIM_VALUE (1111)
#else
    #define DEFAULT_XTAL16M_TRIM_VALUE (1114)
#endif // HW_CONFIG_PRO_DK
#endif // __DA14531__

#if (LOG_HEAP_USAGE)
    #if (!DEVELOPMENT_DEBUG)
        #error "LOG_HEAP_USAGE must not be set when building for production (DEVELOPMENT_DEBUG is 0)"
    #else
        #warning "LOG_HEAP_USAGE is set!"
    #endif
#endif

/// Watchdog behavior at value 0 in production mode
#if (DEVELOPMENT_DEBUG == 0)
#if defined (CFG_WDG_TRIGGER_HW_RESET_IN_PRODUCTION_MODE)
#define USER_WDG_CFG                         (1)
#else
#define USER_WDG_CFG                         (0)
#endif
#else
#define USER_WDG_CFG                         (0)
#endif

#if !defined (__NON_BLE_EXAMPLE__)

/**
 * @addtogroup DRIVERS
 * @{
 */

bool sys_startup_flag __SECTION_ZERO("retention_mem_area0");

// Delay sleep entrance at system startup
uint32_t startup_sleep_delay = STARTUP_SLEEP_DELAY_DEFAULT;

#if defined (__DA14531__)
/// Last temperature read from the die temperature sensor through GPADC
static int8_t last_temp    __SECTION_ZERO("retention_mem_area0");
/// Radio calibration counter increasing by one just before calibration runs
static uint32_t rfcal_count __SECTION_ZERO("retention_mem_area0");
#else
uint16_t last_temp_count    __SECTION_ZERO("retention_mem_area0"); // temperature counter
#endif

uint32_t last_temp_time     __SECTION_ZERO("retention_mem_area0"); // time of last temperature count measurement

extern uint32_t lld_sleep_us_2_lpcycles_func(uint32_t us);
extern uint32_t lld_sleep_lpcycles_2_us_func(uint32_t lpcycles);

extern void ble_init(uint32_t base);

uint32_t lp_clk_sel         __SECTION_ZERO("retention_mem_area0");   //low power clock selection
uint32_t rcx_freq           __SECTION_ZERO("retention_mem_area0");
uint8_t rcx_cal_in_progress __SECTION_ZERO("retention_mem_area0");
uint32_t rcx_period         __SECTION_ZERO("retention_mem_area0");
uint64_t rcx_slot_duration  __SECTION_ZERO("retention_mem_area0");

uint8_t force_rf_cal        __SECTION_ZERO("retention_mem_area0");

#if (RCX_MEASURE_ENABLED)
uint32_t rcx_freq_min       __SECTION_ZERO("retention_mem_area0");
uint32_t rcx_freq_max       __SECTION_ZERO("retention_mem_area0");
uint32_t rcx_period_last    __SECTION_ZERO("retention_mem_area0");
uint32_t rcx_period_diff    __SECTION_ZERO("retention_mem_area0");
#endif

#if defined (__DA14531__)
    #define RCX_PERIOD_CORRECTION_FACTOR_IN_PPM     0

    // Definitions for RCX correction
    #define RCX_COR_MULTIPLIER                  ((uint32_t) (1024*1024))
    #define RCX_COR_SCALED_PPM                  ((uint16_t) (RCX_PERIOD_CORRECTION_FACTOR_IN_PPM * RCX_COR_MULTIPLIER / 1000000))
    #define RCX_COR_DIVIDER                     ((uint64_t) (RCX_COR_MULTIPLIER + RCX_COR_SCALED_PPM))

    #if (RCX_PERIOD_CORRECTION_FACTOR_IN_PPM > 0)
        #define RCX_SLOT_DURATION_FACTOR        (((uint64_t) (625 * 16 * RCX_CALIB_TIME) * RCX_COR_MULTIPLIER << 20) / RCX_COR_DIVIDER)
        #define RCX_PERIOD_FACTOR               ((uint32_t) (((uint64_t) (RCX_COR_DIVIDER) * 64 << 15) / (RCX_CALIB_TIME * RCX_COR_MULTIPLIER)))
    #else
        #define RCX_SLOT_DURATION_FACTOR        ((uint64_t) (625 * 16 * RCX_CALIB_TIME ) << 20)
        #define RCX_PERIOD_FACTOR               (((uint32_t) (64 << 15) / (RCX_CALIB_TIME)))
    #endif
#endif

#ifndef USE_ARCH_WKUPCT_DEB_TIME
#define USE_ARCH_WKUPCT_DEB_TIME
uint16_t arch_wkupct_deb_time           __SECTION_ZERO("retention_mem_area0"); // Wakeup timer debouncing time
#endif

/// Device BD address in OTP header
struct bd_addr dev_bdaddr               __SECTION_ZERO("retention_mem_area0");

uint16_t iq_trim_bias_ctrl1_reg         __SECTION_ZERO("retention_mem_area0");
uint16_t iq_trim_rf_mixer_ctrl1_reg     __SECTION_ZERO("retention_mem_area0");

// Heap logging structs
struct mem_usage_log heap_usage_env     __SECTION_ZERO("retention_mem_area0");
struct mem_usage_log heap_usage_db      __SECTION_ZERO("retention_mem_area0");
struct mem_usage_log heap_usage_msg     __SECTION_ZERO("retention_mem_area0");
struct mem_usage_log heap_usage_nonRet  __SECTION_ZERO("retention_mem_area0");

#if (USE_XTAL16M_ADAPTIVE_SETTLING && USE_POWER_OPTIMIZATIONS)
    extern uint32_t twirq_set_min;
    extern uint32_t twirq_set;
    extern uint32_t twirq_set_max;
#endif

#if defined (__DA14531__)
uint16_t xtal_wait_trim      __SECTION_ZERO("retention_mem_area0");
uint32_t twirq_set_value     __SECTION_ZERO("retention_mem_area0");
uint32_t twirq_reset_value   __SECTION_ZERO("retention_mem_area0");
uint32_t twext_value         __SECTION_ZERO("retention_mem_area0");
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Select low power clock.
 * @return void
 * @note function store_low_power_clock() handles the case where CFG_LP_CLK = LP_CLK_FROM_OTP.
 ****************************************************************************************
 */
static void select_lp_clk()
{
    if (CFG_LP_CLK != LP_CLK_FROM_OTP)
    {
        lp_clk_sel = CFG_LP_CLK;
    }
}

/**
 ****************************************************************************************
 * @brief Store low power clock selection from OTP.
 * @return void
 ****************************************************************************************
 */
#if defined (CFG_LP_CLK) && (CFG_LP_CLK == LP_CLK_FROM_OTP)
static void store_low_power_clock(void)
{
#if defined (__DA14531__)
    lp_clk_sel = otp_cs_get_low_power_clock();
#else
    lp_clk_sel = GetWord32(OTP_HDR_SLEEP_CLK_SOURCE_ADDR);
#endif
}
#endif

/**
 ****************************************************************************************
 * @brief Initialisation of ble core, pwr and clk
 *
 * The Hclk and Pclk are set
 ****************************************************************************************
 */
__STATIC_INLINE void init_pwr_and_clk_ble(void)
{
    SetBits16(CLK_RADIO_REG, BLE_DIV, 0);
    SetBits16(CLK_RADIO_REG, BLE_ENABLE, 1);
#if !defined (__DA14531__)
    SetBits16(CLK_RADIO_REG, RFCU_DIV, 1);
#endif
    SetBits16(CLK_RADIO_REG, RFCU_ENABLE, 1);

    /*
     * Power up BLE core & reset BLE Timers
     */
#if defined (__DA14531__)
    SetBits16(CLK_RC32K_REG, RC32K_DISABLE, 0);
    SetBits16(CLK_CTRL_REG, LP_CLK_SEL, 0);
#else
    SetBits16(CLK_32K_REG,  RC32K_ENABLE, 1);
    SetBits16(SYS_CTRL_REG, CLK32_SOURCE, 0);
#endif

    SetBits16(CLK_RADIO_REG, BLE_LP_RESET, 1);
    SetBits16(PMU_CTRL_REG, RADIO_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & RAD_IS_UP)); // Just wait for radio to truely wake up

    select_lp_clk();

    if ( arch_clk_is_XTAL32( ) )
    {
#if defined (__DA14531__)
    SetBits16(GP_DATA_REG, 0x80, 1);                // Disable RC input filter for P0_3 and P0_4
    SetBits16(CLK_XTAL32K_REG, XTAL32K_ENABLE, 1);  // Enable XTAL32KHz
#else
    SetBits16(CLK_32K_REG, XTAL32K_ENABLE, 1);      // Enable XTAL32KHz
#endif

#if !defined (__DA14531__)
    // Disable XTAL32K amplitude regulation in BOOST mode
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
    {
        SetBits16(CLK_32K_REG,  XTAL32K_DISABLE_AMPREG, 1);
    }
    else
    {
        SetBits16(CLK_32K_REG,  XTAL32K_DISABLE_AMPREG, 0);
    }
#endif

#if defined (__DA14531__)
        SetBits16(CLK_XTAL32K_REG, XTAL32K_CUR, 5);
        SetBits16(CLK_XTAL32K_REG, XTAL32K_RBIAS, 3);
        SetBits16(CLK_CTRL_REG, LP_CLK_SEL, 2);    // Select XTAL32K as LP clock
#else
        SetBits16(CLK_32K_REG, XTAL32K_CUR, 5);
        SetBits16(CLK_32K_REG, XTAL32K_RBIAS, 3);
        SetBits16(SYS_CTRL_REG, CLK32_SOURCE, 1);  // Select XTAL32K as LP clock
#endif

    }
    else if ( arch_clk_is_RCX20( ) )
    {
#if defined (__DA14531__)
        SetBits16(CLK_RCX_REG, RCX_BIAS, 0xA);
        SetBits16(CLK_RCX_REG, RCX_ENABLE, 1);
        SetBits16(CLK_CTRL_REG, LP_CLK_SEL, 1);        // Select RCX as LP clock
        SetBits16(CLK_XTAL32K_REG, XTAL32K_ENABLE, 0); // Disable XTAL32K
#else
        SetBits16(CLK_RCX20K_REG, RCX20K_NTC, 0xB);
        SetBits16(CLK_RCX20K_REG, RCX20K_BIAS, 0);
        SetBits16(CLK_RCX20K_REG, RCX20K_TRIM, 0);
        SetBits16(CLK_RCX20K_REG, RCX20K_LOWF, 1);
        SetBits16(CLK_RCX20K_REG, RCX20K_ENABLE, 1);
        SetBits16(CLK_RCX20K_REG, RCX20K_SELECT, 1);
        SetBits16(SYS_CTRL_REG, CLK32_SOURCE, 0);
        SetBits16(CLK_32K_REG, XTAL32K_ENABLE, 0);     // Disable XTAL32K
#endif
    }
    else
    {
        ASSERT_WARNING(0);
    }

#if !defined (__DA14531__)
    SetBits16(CLK_32K_REG,  RC32K_ENABLE, 0);      // Disable RC32K
#endif

    SetBits16(CLK_RADIO_REG, BLE_LP_RESET, 0);

#if !defined (__DA14531__)
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
    {
        SetWord16(DCDC_CTRL3_REG, 0x5);
    }
#endif

    /*
     * Just make sure that BLE core is stopped (if already running)
     */
    SetBits32(BLE_RWBLECNTL_REG, RWBLE_EN, 0);

    /*
     * Since BLE is stopped (and powered), set CLK_SEL
     */
    SetBits32(BLE_CNTL2_REG, BLE_CLK_SEL, 16);
    SetBits32(BLE_CNTL2_REG, BLE_RSSI_SEL, 1);
}

void calibrate_rcx20(uint16_t cal_time)
{
    if ((CFG_LP_CLK == LP_CLK_FROM_OTP) || (CFG_LP_CLK == LP_CLK_RCX20))
    {
        SetWord16(CLK_REF_CNT_REG, cal_time);
        SetBits16(CLK_REF_SEL_REG, REF_CLK_SEL, 0x3); //RCX select
        SetBits16(CLK_REF_SEL_REG, REF_CAL_START, 0x1); //Start Calibration
        rcx_cal_in_progress = 1;
    }
}

#if defined (__DA14531__)
void read_rcx_freq(bool wait_for_calibration)
{
    if (rcx_cal_in_progress && ((CFG_LP_CLK == LP_CLK_FROM_OTP) || (CFG_LP_CLK == LP_CLK_RCX20)))
    {
        if (wait_for_calibration)
        {
            while (GetBits16(CLK_REF_SEL_REG, REF_CAL_START) == 1);
        }
        else
        {
            if (GetBits16(CLK_REF_SEL_REG, REF_CAL_START) == 1)
            {
                return;
            }
        }

        uint32_t high = GetWord16(CLK_REF_VAL_H_REG);
        uint32_t low = GetWord16(CLK_REF_VAL_L_REG);
        uint32_t value = (high << 16) + low;

        rcx_cal_in_progress = 0;

        // Calculate RCX period
        rcx_period = (((uint32_t) ((value * RCX_PERIOD_FACTOR) >> 14) + 1) >> 1);
        // Calculate RCX slot duration
        rcx_slot_duration = (RCX_SLOT_DURATION_FACTOR / value);
        // Calculate RCX frequency. (1000000 / 625 >> 20) is equal to (100 >> 16)
        rcx_freq = (((uint32_t) rcx_slot_duration) * 100) >> 16;

#if (RCX_MEASURE_ENABLED)
        if (rcx_period_last)
        {
            int diff = rcx_period_last - rcx_period;
            if (abs(diff) > rcx_period_diff)
            {
                rcx_period_diff = abs(diff);
            }
        }
        rcx_period_last = rcx_period;

        if (rcx_freq_min == 0)
        {
            rcx_freq_min = rcx_freq;
            rcx_freq_max = rcx_freq;
        }

        if (rcx_freq < rcx_freq_min)
        {
            rcx_freq_min = rcx_freq;
        }
        else if (rcx_freq > rcx_freq_max)
        {
            rcx_freq_max = rcx_freq;
        }
#endif
    }
}

#else
void read_rcx_freq(uint16_t cal_time)
{
    if ( (rcx_cal_in_progress) && ((CFG_LP_CLK == LP_CLK_FROM_OTP) || (CFG_LP_CLK == LP_CLK_RCX20)) )
    {
        while(GetBits16(CLK_REF_SEL_REG, REF_CAL_START) == 1);
        uint32_t high = GetWord16(CLK_REF_VAL_H_REG);
        uint32_t low = GetWord16(CLK_REF_VAL_L_REG);
        uint32_t value = ( high << 16 ) + low;

#ifdef RCX_PERIOD_CORRECTION_FACTOR_IN_PPM
        uint32_t f = ((uint64_t)16000000 * cal_time) * 1000000 / (1000000 + RCX_PERIOD_CORRECTION_FACTOR_IN_PPM) / value  ;
#else
        uint32_t f = (16000000 * cal_time) / value;
#endif

        rcx_cal_in_progress = 0;

        rcx_freq = f;

#ifdef RCX_PERIOD_CORRECTION_FACTOR_IN_PPM
        rcx_period = ((uint64_t)64 * value) * (1000000 + RCX_PERIOD_CORRECTION_FACTOR_IN_PPM) / 1000000 / cal_time;
#else
        rcx_period = (64 * value) / cal_time;
#endif

        rcx_slot_duration = 625 * cal_time * 16;

#ifdef RCX_PERIOD_CORRECTION_FACTOR_IN_PPM
        rcx_slot_duration = (rcx_slot_duration << 20) * 1000000 / (1000000 + RCX_PERIOD_CORRECTION_FACTOR_IN_PPM);
#else
        rcx_slot_duration = rcx_slot_duration << 20;
#endif

        rcx_slot_duration = rcx_slot_duration / value;

#if (RCX_MEASURE_ENABLED)
        if (rcx_period_last)
        {
            int diff = rcx_period_last - rcx_period;
            if (abs(diff) > rcx_period_diff)
                rcx_period_diff = abs(diff);
        }
        rcx_period_last = rcx_period;

        if (rcx_freq_min == 0)
        {
            rcx_freq_min = rcx_freq;
            rcx_freq_max = rcx_freq;
        }
        if (rcx_freq < rcx_freq_min)
            rcx_freq_min = rcx_freq;
        else if (rcx_freq > rcx_freq_max)
            rcx_freq_max = rcx_freq;
#endif
    }
}
#endif

void set_sleep_delay(void)
{
    int16_t delay;

    if (!USE_POWER_OPTIMIZATIONS)
    {
        uint32_t sleep_delay;

        delay = 0;

        if ( arch_clk_is_RCX20() )
        {
            if (rcx_period > (RCX_PERIOD_MAX << 10) )
                ASSERT_ERROR(0);

            sleep_delay = SLP_PROC_TIME + SLEEP_PROC_TIME + (4 * RCX_PERIOD_MAX); // 400
        }
        else
        {
            sleep_delay = /*SLP_PROC_TIME + SLEEP_PROC_TIME + */(4 * XTAL32_PERIOD_MAX); // ~200
        }

        // Actual "delay" is application specific and is the execution time of the BLE_WAKEUP_LP_Handler(), which depends on XTAL trimming delay.
        // In case of OTP copy, this is done while the XTAL is settling. Time unit of delay is usec.
        delay += XTAL_TRIMMING_TIME_USEC;
        // Increase time taking into account the time from the setting of DEEP_SLEEP_ON until the assertion of DEEP_SLEEP_STAT.
        delay += sleep_delay;
        // Add any application specific delay
        delay += APP_SLEEP_DELAY_OFFSET;
    }
    else // USE_POWER_OPTIMIZATIONS
    {
#if (USE_XTAL16M_ADAPTIVE_SETTLING)
        delay = 0;
        if (twirq_set > 87)
            delay = (twirq_set - 87);

        delay = (delay/2) * 61;
        if (delay%2 != 0)
            delay += 31;
        delay = co_max(delay, MINIMUM_SLEEP_DURATION);
#else
#if defined (__DA14531__)
        delay = lld_sleep_lpcycles_2_us_sel_func(twirq_set_value) + MINIMUM_SLEEP_DURATION;
#else
        delay = MINIMUM_SLEEP_DURATION;
#endif

        // if XTAL_TRIMMING_TIME_USEC changes (i.e. gets larger) then this
        // will make sure that the user gets notified to increase "delay" by 1 or more
        // slots so there's enough time for XTAL to settle
        # if ( (3125 + (MINIMUM_SLEEP_DURATION + 625)) < (LP_ISR_TIME_USEC + 1100)) //1.1ms max power-up time
        # error "Minimum sleep duration is too small for the 16MHz crystal used..."
        # endif
#endif
    }

    rwip_wakeup_delay_set(delay);
}

#if !defined (__DA14531__)
uint16_t get_rc16m_count(void)
{
    uint16_t count;

    uint16_t trim = GetBits16(CLK_16M_REG, RC16M_TRIM); // save trim value

    SetBits16(CLK_16M_REG, RC16M_ENABLE, 1);
    SetBits16(CLK_16M_REG, RC16M_TRIM, 0x0);

    SetWord16(CLK_REF_CNT_REG, RC16_CAL_REF_CYCLES);    // Preset reference cycles to 1024
    SetWord16(CLK_REF_SEL_REG, 0x0005);                 // Select RC16M and start counting

    while (GetWord16(CLK_REF_SEL_REG) & 0x0004);

    count = GetWord16(CLK_REF_VAL_L_REG);               // Read back number of XTAL16M cycles

    SetBits16(CLK_16M_REG, RC16M_TRIM, 0xA);
    SetBits16(CLK_16M_REG, RC16M_ENABLE, 0);

    SetBits16(CLK_16M_REG, RC16M_TRIM, trim);           // restore trim value

    return count;
}
#endif

void conditionally_run_radio_cals(void)
{
#if defined (__DA14531__)
    // 531 case
    uint32_t current_time = lld_evt_time_get();

    if (current_time < last_temp_time)
    {
        last_temp_time = 0;
    }

    if ((current_time - last_temp_time) >= 3200) //2 sec
    {
        last_temp_time = current_time;

        // Store the current content of the following registers:
        // - GP_ADC_CTRL_REG,
        // - GP_ADC_CTRL2_REG,
        // - GP_ADC_CTRL3_REG,
        // - GP_ADC_SEL_REG.
        uint16_t tmp_adc_ctrl_reg  = GetWord16(GP_ADC_CTRL_REG);
        uint16_t tmp_adc_ctrl2_reg = GetWord16(GP_ADC_CTRL2_REG);
        uint16_t tmp_adc_ctrl3_reg = GetWord16(GP_ADC_CTRL3_REG);
        uint16_t tmp_adc_sel_reg   = GetWord16(GP_ADC_SEL_REG);

        adc_config_t cfg =
        {
            .input_mode = ADC_INPUT_MODE_SINGLE_ENDED,
            .input      = ADC_INPUT_SE_TEMP_SENS,
            .continuous = false
        };

        // Initialize and enable ADC
        adc_init(&cfg);

        int8_t current_temp = adc_get_temp();

        int8_t temp_diff = current_temp - last_temp;

        if (temp_diff < 0)
        {
            temp_diff = -temp_diff;
        }

        if (temp_diff >= 8)
        {
            // Calibrate the RF if the temperature difference is greater than
            // (or equal to) 8 degrees Celsius
            rfcal_count++;
            rf_calibration();

            last_temp = current_temp;
        }

        // Restore the content of the ADC CTRL and SEL registers
        SetWord16(GP_ADC_CTRL_REG, tmp_adc_ctrl_reg);
        SetWord16(GP_ADC_CTRL2_REG, tmp_adc_ctrl2_reg);
        SetWord16(GP_ADC_CTRL3_REG, tmp_adc_ctrl3_reg);
        SetWord16(GP_ADC_SEL_REG, tmp_adc_sel_reg);
    }
#else
    // 585/586 case
    uint16_t count, count_diff;

    bool rf_cal_stat;

    uint32_t current_time = lld_evt_time_get();

    if (current_time < last_temp_time)
    {
        last_temp_time = 0;
    }

    if (force_rf_cal)
    {
        force_rf_cal = 0;

        last_temp_time = current_time;
        last_temp_count = get_rc16m_count();
        #if (LUT_PATCH_ENABLED)
        pll_vcocal_LUT_InitUpdate(LUT_UPDATE);    //Update pll look up table
        #endif

        #if (LUT_PATCH_ENABLED)
        rf_cal_stat = rf_calibration_580();
        #else
        rf_cal_stat = rf_calibration_585();
        #endif

        if ( rf_cal_stat == false)
            force_rf_cal = 1;

        return;
    }

    if ( (current_time - last_temp_time) >= 3200) //2 sec
    {
        last_temp_time = current_time;
        count = get_rc16m_count();                  // Estimate the RC16M frequency

        if (count > last_temp_count)
            count_diff = count - last_temp_count;
        else
            count_diff = last_temp_count - count ;

        if (count_diff >= 24)// If corresponds to 5 C degrees difference
        {

            // Update the value of last_count
            last_temp_count = count;
            #if (LUT_PATCH_ENABLED)
            pll_vcocal_LUT_InitUpdate(LUT_UPDATE);    //Update pll look up table
            #endif

            #if (LUT_PATCH_ENABLED)
            rf_cal_stat = rf_calibration_580();
            #else
            rf_cal_stat = rf_calibration_585();
            #endif

            if ( rf_cal_stat == false)
                force_rf_cal = 1;         // Perform the radio calibrations
        }
    }
#endif
}

/**
 ****************************************************************************************
 * @brief       Converts us to low power cycles for RCX20 clock.
 *
 * @param[in]   us. microseconds
 *
 * @return      uint32_t. Low power cycles
 ****************************************************************************************
 */
uint32_t lld_sleep_us_2_lpcycles_rcx_func(uint32_t us)
{
    uint32_t lpcycles = 0;

    lpcycles = (us * rcx_freq + 500000) / 1000000;

    return(lpcycles);
}

/**
 ****************************************************************************************
 * @brief       Converts low power cycles to us for RCX20 clock.
 *
 * @param[in]   lpcycles. Low power cycles
 *
 * @return      uint32_t. microseconds
 ****************************************************************************************
 */
uint32_t lld_sleep_lpcycles_2_us_rcx_func(uint32_t lpcycles)
{
    uint32_t us = 0;

    // Sanity check: The number of lp cycles should not be too high to avoid overflow
    ASSERT_ERR(lpcycles < 1000000);

    if (lpcycles >= 0x8000) {
        // lpcycles MUST be very large!
        uint64_t res = (uint64_t)lpcycles * rcx_period;
        us = res >> 10;
    }
    else {
        us = (lpcycles * rcx_period) >> 10;
    }

    return(us);
}

uint32_t lld_sleep_us_2_lpcycles_sel_func(uint32_t us)
{
    uint32_t lpcycles = 0;

    if ( arch_clk_is_XTAL32( ) )
        lpcycles = lld_sleep_us_2_lpcycles_func(us);
    else if ( arch_clk_is_RCX20( ) )
        lpcycles = lld_sleep_us_2_lpcycles_rcx_func(us);

    return(lpcycles);
}

uint32_t lld_sleep_lpcycles_2_us_sel_func(uint32_t lpcycles)
{
    uint32_t us = 0;

    if ( arch_clk_is_XTAL32( ) )
        us = lld_sleep_lpcycles_2_us_func(lpcycles);
    else  if ( arch_clk_is_RCX20() )
        us = lld_sleep_lpcycles_2_us_rcx_func(lpcycles);

    return(us);
}


#if (!BLE_APP_PRESENT)

bool check_gtl_state(void)
{
    bool ret = true;

    if (use_h4tl & 0x01)
    {
        if ((h4tl_env.tx_state != H4TL_STATE_TX_IDLE)||
            ((h4tl_env.rx_state != H4TL_STATE_RX_START) &&
            (h4tl_env.rx_state != H4TL_STATE_RX_OUT_OF_SYNC)) )
            ret = false;
    }
    else
    {
        if ((ke_state_get(TASK_GTL) != GTL_TX_IDLE) ||
            ((gtl_env.rx_state != GTL_STATE_RX_START) &&
            (gtl_env.rx_state != GTL_STATE_RX_OUT_OF_SYNC)) )
            ret = false;
    }

    return ret;
}
#endif

#if ((EXTERNAL_WAKEUP) && (!BLE_APP_PRESENT)) // only in full embedded designs
void ext_wakeup_enable(uint32_t port, uint32_t pin, uint8_t polarity)
{
    rwip_env.ext_wakeup_enable = 2;
    if (DEVELOPMENT_DEBUG)
        RESERVE_GPIO(EXT_WAKEUP_GPIO, (GPIO_PORT) port, (GPIO_PIN) pin, PID_GPIO);

    if ( polarity == 0 ) // active low
        GPIO_ConfigurePin((GPIO_PORT) port, (GPIO_PIN) pin, INPUT_PULLUP, PID_GPIO, false ); // active low. Set default to input high.
    else // active high
        GPIO_ConfigurePin((GPIO_PORT) port, (GPIO_PIN) pin, INPUT_PULLDOWN, PID_GPIO, false); // active high. Set default to input low.

    wkupct_register_callback(ext_wakeup_cb);

    wkupct_enable_irq(WKUPCT_PIN_SELECT((GPIO_PORT) port, (GPIO_PIN) pin), // Select pin
                      WKUPCT_PIN_POLARITY((GPIO_PORT) port, (GPIO_PIN) pin, (polarity == 0) ? WKUPCT_PIN_POLARITY_LOW : WKUPCT_PIN_POLARITY_HIGH), // Polarity
                      1, // 1 event
                      0); // debouncing time = 0
}

void ext_wakeup_resume_from_sleep(void)
{
}

void ext_wakeup_cb(void)
{
#if !defined (__DA14531__)
    if (GetBits16(SYS_STAT_REG, PER_IS_DOWN))
    {
        // Return GPIO functionality from external wakeup GPIO
        if (DEVELOPMENT_DEBUG)
            GPIO_reservations();
        set_pad_functions();
    }
#endif
    SetBits32(GP_CONTROL_REG, BLE_WAKEUP_REQ, 1);
}
#endif // ((EXTERNAL_WAKEUP) && (!BLE_APP_PRESENT))

uint8_t check_sys_startup_period(void)
{
    uint8_t ret_value = 0;

    if (sys_startup_flag)
    {
        uint32_t current_time;

        current_time = lld_evt_time_get();

        // startup_sleep_delay after system startup to allow system to sleep
        if (current_time < startup_sleep_delay)
        {
            ret_value = 1;
        }
        else // After 2 seconds system can sleep
        {
            sys_startup_flag = false;
        }
    }
    return ret_value;
}

__WEAK bool app_use_lower_clocks_check(void)
{
    return false;
}

void lld_sleep_init_func(void)
{
    /// ROM struct definition
    struct lld_sleep_env_tag
    {
        uint32_t irq_mask;
    };
    extern struct lld_sleep_env_tag lld_sleep_env;


    // Clear the environment
    memset(&lld_sleep_env, 0, sizeof(lld_sleep_env));

    // Set wakeup_delay
    set_sleep_delay();

    // Enable external wake-up by default
    ble_extwkupdsb_setf(0);
}

void arch_wkupct_tweak_deb_time(bool tweak)
{
    if (arch_clk_is_RCX20())
    {
        if (tweak)
        {
            // Recalculate debouncing time to correct silicon bug
            uint32_t temp = (rcx_freq * arch_wkupct_deb_time) / 32768;
            SetBits16(WKUP_CTRL_REG, WKUP_DEB_VALUE, temp & 0x3F);
        }
        else
        {
            SetBits16(WKUP_CTRL_REG, WKUP_DEB_VALUE, arch_wkupct_deb_time & 0x3F);
        }
    }
}
#endif // (__NON_BLE_EXAMPLE__)

/*
 * Non BLE specific code
 */

#if defined (__DA14531__)
void arch_set_pxact_gpio(GPIO_PORT port, GPIO_PIN pin)
{
#if (DEVELOPMENT_DEBUG)
    uint32_t i;
    uint32_t data_reg = GPIO_BASE + (port << 5);
    uint32_t mode_reg = data_reg + 0x6 + (pin << 1);
    uint32_t set_data_reg = data_reg + 2;
    uint32_t reset_data_reg = data_reg + 4;

    SetWord16(mode_reg, PID_GPIO | OUTPUT);
    SetWord16(set_data_reg, 1 << pin);
    for (i = 0; i < 190; i++); //190 is almost 47usec of time.
    SetWord16(reset_data_reg, 1 << pin);
#endif
}
#else
void arch_set_pxact_gpio(void)
{
#if (DEVELOPMENT_DEBUG)
    uint32_t i;

    SetWord16(P13_MODE_REG, PID_GPIO|OUTPUT);
    SetWord16(P1_SET_DATA_REG, 0x8);
    for (i = 0; i < 150; i++); //150 is almost 47usec of time.
    SetWord16(P1_RESET_DATA_REG, 0x8);
#endif
}
#endif

#if defined (__DA14531__)
void set_xtal32m_trim_value(uint16_t trim_value)
{
    SetBits16(CLK_RC32M_REG, RC32M_DISABLE, 0);                     // enable RC32M
    for (volatile int i = 0; i < 20; i++);

    SetBits16(CLK_CTRL_REG, SYS_CLK_SEL, 1);                        // switch to RC32M
    while( (GetWord16(CLK_CTRL_REG) & RUNNING_AT_RC32M) == 0 );     // wait for actual switch

    SetWord16(CLK_FREQ_TRIM_REG, trim_value);                       // set default trim value
    for (volatile int i = 0; i < 1800; i++);                        // wait for 1 ms

    SetBits16(CLK_CTRL_REG , SYS_CLK_SEL ,0);                       // switch to XTAL32M
    while( (GetWord16(CLK_CTRL_REG) & RUNNING_AT_XTAL32M) == 0 );   // wait for actual switch
}
#else
void set_xtal16m_trim_value(uint16_t trim_value)
{
    SetBits16(CLK_16M_REG, RC16M_ENABLE, 1);                        // enable RC16M
    for (volatile int i = 0; i < 20; i++);

    SetBits16(CLK_CTRL_REG, SYS_CLK_SEL, 1);                        // switch to RC16M
    while( (GetWord16(CLK_CTRL_REG) & RUNNING_AT_RC16M) == 0 );     // wait for actual switch

    SetWord16(CLK_FREQ_TRIM_REG, trim_value);                       // set default trim value
    for (volatile int i = 0; i < 1600; i++);                        // wait for 1 ms

    SetBits16(CLK_CTRL_REG , SYS_CLK_SEL ,0);                       // switch to XTAL16M
    while( (GetWord16(CLK_CTRL_REG) & RUNNING_AT_XTAL16M) == 0 );   // wait for actual switch
}
#endif

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Check if XTAL32M clock is calibrated. If not set the trimming value to
 *        the default one.
 * @warning OTP memory should have been enabled in read mode, prior to calling
 *          this function.
 * @return void
 ****************************************************************************************
 */
static __inline void xtal32m_calibration_check(void)
{
    // Apply the default XTAL32M trim value if a trim value has not been programmed in OTP
    if (!otp_cs_is_xtal32m_trim_present())
    {
        if ((GetWord32(OTP_HDR_PACKAGE_ADDR) & 0xFF) == PACKAGE_QFN_24)
        {
            set_xtal32m_trim_value(DEFAULT_XTAL32M_TRIM_VALUE_QFN);
        }
        else
        {
            set_xtal32m_trim_value(DEFAULT_XTAL32M_TRIM_VALUE_WLCSP);
        }
    }
    else
    {
        // Booter has already written the trim value to h/w
        set_xtal32m_trim_value(GetWord16(CLK_FREQ_TRIM_REG));
    }
}
#else
/**
 ****************************************************************************************
 * @brief Check if XTAL16M clock is calibrated. If not set the trimming value to
 *        the default one.
 * @return void
 ****************************************************************************************
 */
static __inline void xtal16m_calibration_check(void)
{
#if (DEFAULT_XTAL16M_TRIM_VALUE_USED)
    // Apply the default XTAL16M trim value if a trim value has not been programmed in OTP
    if (GetWord16(CLK_FREQ_TRIM_REG) == 0)
    {
        set_xtal16m_trim_value(DEFAULT_XTAL16M_TRIM_VALUE);
    }
#endif
}
#endif

void set_system_clocks(void);

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Notifies application that system has been reset.
 * @param[in] reset_stat The value of @c RESET_STAT_REG; if reset_stat is zero,
 *                       system has woken up from hibernation.
 * @note Application shall define this function or get notified when a reset occurs.
 ****************************************************************************************
 */
__WEAK void reset_indication(uint16_t reset_stat)
{

}
#else
/**
 ****************************************************************************************
 * @brief Notifies application that system has been reset.
 * @param[in] por_time The value of @c POR_TIMER_REG. @c POR_TIMER_REG is cleared
 *                     only after a POR reset. Application can write to @c POR_TIMER_REG
 *                     a value (other than the reset value) to determine whether
 *                     the system woke up from deep sleep.
 * @note Application shall define this function or get notified when a reset occurs.
 ****************************************************************************************
 */
__WEAK void reset_indication(uint16_t por_time)
{

}
#endif

/*
 * System initialization
 */

void system_init(void)
{
    // Initialize the Watchdog unit
    wdg_init(USER_WDG_CFG);

    // Initialize the GPIOs
    GPIO_init();

#if defined (__DA14531__)
    // Get the reason of reset
    volatile uint16_t reset_stat = GetWord16(RESET_STAT_REG);

    // Notify application
    reset_indication(reset_stat);

    // Check if the system woke up from hibernation
    // Booter performs a SW reset to execute the software, so in this case
    // SWRESET_STAT is set.
    if ((reset_stat & (~SWRESET_STAT)) == 0)
    {
        // This is a wakeup from hibernation
        // Disable Hibernation
        SetBits16(HIBERN_CTRL_REG, HIBERNATION_ENABLE, 0);
    }
    else
    {
        // This is a wakeup from reset (POR, HW or watchdog)
        // Clear RESET_STAT_REG to be ready for next time
        SetWord16(RESET_STAT_REG, 0);
    }
#else
    // Provide the value of POR_TIMER_REG to the function to determine
    // whether the system woke up from deep sleep (hard reset will be issued after
    // wake up) or did a power cycle:
    // Deep sleep wake up: The user must initialize the POR_TIMER_REG
    // to a value different than the reset one. The POR_TIMER_REG is retained
    // during deep sleep.
    // Power cycle (either through power supply on/off or power on reset):
    // The POR_TIMER_REG will be set to its reset value.
    reset_indication(GetWord16(POR_TIMER_REG));
#endif

#if defined (__DA14531__)
    // Set clamp output below LDO_CORE level
    SetBits16(POWER_AON_CTRL_REG, LDO_RET_TRIM, 0xF);

    // Enable LDO_CORE_RET
    SetBits16(POWER_CTRL_REG, LDO_CORE_RET_ENABLE, 1);

    // Recommended settings copied from rf_531.c
    SetBits16(DCDC_CTRL_REG, DCDC_CLK_DIV, 0x0);
    SetBits16(POWER_AON_CTRL_REG, FORCE_RUNNING_COMP_DIS, 0x1);
    SetBits16(POWER_CTRL_REG, POR_VBAT_HIGH_HYST_DIS, 0);    // Sets the POR VBAT HIGH threshold at 1.66V
    SetBits16(POWER_LEVEL_REG, LDO_LOW_TRIM, 3);

    // Perform the following steps when in Boost (or Bypass) mode
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
        SetBits16(DCDC_CTRL_REG, DCDC_ILIM_MIN, 0x4);
        // Set ILIM_MAX to half its maximum value to limit inrush current
        // during wake-up or cold boot for high-ohmic batteries.
        SetBits16(DCDC_CTRL_REG, DCDC_ILIM_MAX, 0x8);

        // Disable the POR triggered from VBAT_HIGH voltage level sensing
        SetBits16(POWER_AON_CTRL_REG, POR_VBAT_HIGH_RST_MASK, 1);

        // Enable the POR triggered from VBAT_LOW voltage level sensing
        SetBits16(POWER_AON_CTRL_REG, POR_VBAT_LOW_RST_MASK, 0);

        // Disable the DC/DC. It has been left enabled by the booter.
        // - It has to be enabled for OTP access and GPIOs.
        SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 0);
    }
    // Perform the following steps, when in Buck mode
    else
    {
        SetBits16(DCDC_CTRL_REG, DCDC_ILIM_MIN, 0x6);
        SetBits16(DCDC_CTRL_REG, DCDC_ILIM_MAX, 0xF);

        // Enable the POR triggered from VBAT_HIGH voltage level sensing
        SetBits16(POWER_AON_CTRL_REG, POR_VBAT_HIGH_RST_MASK, 0);

        // Enable the POR triggered from VBAT_LOW voltage level sensing
        SetBits16(POWER_AON_CTRL_REG, POR_VBAT_LOW_RST_MASK, 0);

        // Set DC/DC level to 1.1V
        SetBits16(POWER_LEVEL_REG, DCDC_LEVEL, 0);

        // Enable the DC/DC
        SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 1);

        while(GetBits16(ANA_STATUS_REG, DCDC_OK) != 1);
    }

    // Connection between VBAT_HIGH and VBAT_LOW: Automatic connection control
    SetBits16(POWER_AON_CTRL_REG, VBAT_HL_CONNECT_RES_CTRL, 3);

    // Turn off LDO_LOW.
    // Boost (or Bypass) mode: it must be turned off always.
    // Buck mode: it must be turned off since DC/DC is used for VBAT_LOW. It has to be turned on, as late as possible, before sleep.
    SetBits16(POWER_CTRL_REG, LDO_LOW_CTRL_REG, 1);

    // Set for proper RCX operation
    SetBits16(GP_DATA_REG, 0x60, 1);
#endif

#if !defined (__NON_BLE_EXAMPLE__)
    sys_startup_flag = true;
#endif

#if !defined (__DA14531__)
    // Disable DC-DC converter automatic calibration (buck or boost mode)
    SetBits16(DCDC_CTRL2_REG, DCDC_AUTO_CAL, 0);

    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
    {
        // Register settings applied when in boost mode
        SetBits16(DCDC_CTRL2_REG, DCDC_VOLT_LEV, 0xA); // 1.45V
        SetBits16(DCDC_CTRL2_REG, DCDC_CUR_LIM, 0x8);  // 80mA
    }
#endif

#if defined (__DA14531__)
    // Configure XTAL32M
    SetBits16(TRIM_CTRL_REG, XTAL_TRIM_SELECT, 2);       // only CLK_FREQ_TRIM_REG will be used for XTAL32M trimming
    SetBits16(XTAL32M_START_REG, XTAL32M_START, 0);
    SetBits16(XTAL32M_CTRL0_REG, CORE_CUR_SET, 2);
#else
    // Confirm XTAL16M calibration
    xtal16m_calibration_check();
#endif

    // Set the system clocks (rom function call)
#if !defined (__DA14531__)
    SetBits16(CLK_16M_REG, RC16M_ENABLE, 0);
#endif

#if defined (__DA14531__)
    #if (DISABLE_QUADEC_ON_START_UP)
        // Clear quadec pending interrupt - mask quadec interrupt
        SetWord16(QDEC_CTRL_REG, QDEC_IRQ_STATUS);

        // Close peripheral clock
        SetBits16(CLK_PER_REG, QUAD_ENABLE, 0);

        // Clear respective NVIC entry
        NVIC_ClearPendingIRQ(WKUP_QUADEC_IRQn);
    #endif
#endif

    // Initialize OTP controller
    hw_otpc_init();

#if defined (__DA14531__)
    hw_otpc_enter_mode(HW_OTPC_MODE_READ);
#else
    hw_otpc_manual_read_on(false);
#endif

#if defined (__DA14531__)
#if !defined (__FPGA__)
    uint8_t dbg_cfg = (GetWord32(OTP_HDR_PACKAGE_ADDR) & 0xFF) == PACKAGE_QFN_24 ? SWD_DATA_AT_P0_10 : SWD_DATA_AT_P0_5;
#else
    uint8_t dbg_cfg = SWD_DATA_AT_P0_10;
#endif

    // Store default debugger configuration
    syscntl_store_debugger_cfg(dbg_cfg);
#endif

#if !defined (__NON_BLE_EXAMPLE__)
    // Read BD device address
    otp_hdr_get_bd_address((uint8_t *) &dev_bdaddr);

#if !defined (__DA14531__)
    // Read IQ trim values
    iq_trim_bias_ctrl1_reg = GetWord16(OTP_HDR_IQ_TRIM_VALUE_ADDR);
    iq_trim_rf_mixer_ctrl1_reg = GetWord16(OTP_HDR_IQ_TRIM_VALUE_ADDR + 2);
#endif
#endif // (__NON_BLE_EXAMPLE__)

#if defined (__DA14531__)
    // Store OTP Configuration Script values to retention memory
    otp_cs_store();

    // Confirm XTAL32M calibration
    xtal32m_calibration_check();
#endif

    // Read low power clock source from OTP
#if defined (CFG_LP_CLK) && (CFG_LP_CLK == LP_CLK_FROM_OTP)
    store_low_power_clock();
#endif

    hw_otpc_disable();

#if !defined (__NON_BLE_EXAMPLE__)
    // Initialize ROM variables
    arch_rom_init();

#if (USE_RANGE_EXT)
    // Register range extender driver API
    range_ext_init(&range_ext);
#endif
#endif // (__NON_BLE_EXAMPLE__)

#if defined (__DA14586__)
    power_down_da14586_flash();
#endif

    // Peripheral initialization
    periph_init();

#if !defined (__NON_BLE_EXAMPLE__)
    // Initialize random process
    co_random_init(1);

    /*
     ************************************************************************************
     * BLE initialization
     ************************************************************************************
     */
    init_pwr_and_clk_ble();

    ble_init(_ble_base);

#if (USE_RANGE_EXT)
    // Enable range extender
    range_ext.enable(MAX_POWER, NULL);
#endif

    // Disable sleep and external wake up. We have to do this because rwip_init() sets them both to true.
    rwip_env.sleep_enable = false;
    rwip_env.ext_wakeup_enable = false;

    //Enable the BLE core
    SetBits32(BLE_RWBLECNTL_REG, RWBLE_EN, 1);

    // Initialize random number generator seed using random bits acquired from TRNG
#if (USE_TRNG)
    init_rand_seed_from_trng();
#endif

#ifdef DG_DBG_DIAG_PORT
    dbg_init_diag_port();
#endif

    rcx20_calibrate ();

    arch_disable_sleep();

    /*
     ************************************************************************************
     * Application initializations
     ************************************************************************************
     */
    // Initialise APP

#if (BLE_APP_PRESENT)
    app_init();         // Initialize APP
#endif

     if (user_app_main_loop_callbacks.app_on_init !=NULL)
          user_app_main_loop_callbacks.app_on_init();

    //Initialise lld_sleep
    lld_sleep_init_func();

#if (USE_XTAL16M_ADAPTIVE_SETTLING && USE_POWER_OPTIMIZATIONS)
    twirq_set_min = TWIRQSET_MIN_XTAL32_TICKS;
    twirq_set_max = TWIRQSET_MAX_XTAL32_TICKS;
    twirq_set     = TWIRQSET_INIT_XTAL32_TICKS;
#endif
     
#if (__DA14531__)
    extern struct arch_sleep_env_tag    sleep_env;
#define HW_STARTUP_TIME_LP_CYCLES            (7)  // 7 LP clocks for startup state machine handling
#define BLE_PWR_UP_TO_SLP_IRQ_USEC           (60)
    uint16_t xtal_wait_trim_usec = otp_cs_get_xtal_wait_trim();
    xtal_wait_trim = lld_sleep_us_2_lpcycles_sel_func(xtal_wait_trim_usec);
    
    // TWIRQ_RESET in LP clock cycles (time to reset BLE_WAKEUP_LP IRQ before SLP IRQ assertion)
    // Extra LP clock cycles: 1 due to calibration circuit, 2 to read SLP IRQ status, 1 for margin
    twirq_reset_value = 4 + lld_sleep_us_2_lpcycles_sel_func(BLE_PWR_UP_TO_SLP_IRQ_USEC + xtal_wait_trim_usec);
    
    // TWIRQ_SET in LP clock cycles (time to set BLE_WAKEUP_LP IRQ before SLP IRQ assertion)
    twirq_set_value = HW_STARTUP_TIME_LP_CYCLES + xtal_wait_trim + twirq_reset_value;
    
    if (sleep_env.slp_state == ARCH_EXT_SLEEP_OTP_COPY_ON)
    {
        if (arch_clk_is_RCX20())
        {
            twirq_set_value += RCX_OTP_COPY_OVERHEAD;
        }
        else if (arch_clk_is_XTAL32())
        {
            twirq_set_value += XTAL32_OTP_COPY_OVERHEAD;
        }
    }
#endif

#endif // (__NON_BLE_EXAMPLE__)

#if !defined (__DA14531__)
    xtal16m_trim_init();
#endif

#if defined (CFG_RF_DIAG_INT)
    // Enable RF DIAG interrupts
    enable_rf_diag_irq(RF_DIAG_IRQ_MODE_RXTX); // argument is unused for DA14531
#endif

#if !defined (__NON_BLE_EXAMPLE__)
    /*
     ************************************************************************************
     * Watchdog start
     ************************************************************************************
     */
#if (USE_WDOG)
    wdg_reload(WATCHDOG_DEFAULT_PERIOD);
    wdg_resume ();
#endif
#endif
}

#if !defined (__NON_BLE_EXAMPLE__)
// Use ROM 'gap_cfg_user_var_struct symbol' only for DA14531 and ROM SDK combination
#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_GAP_CFG_DATA__)
const struct gap_cfg_user_struct gap_cfg_user_var_struct =
{
    .GAP_TMR_LECB_DISCONN_TIMEOUT_VAR=0x0BB8,
    .GAP_APPEARANCE_VAR=0x0000,
    .GAP_PPCP_CONN_INTV_MAX_VAR=0x0064,
    .GAP_PPCP_CONN_INTV_MIN_VAR=0x00c8,
    .GAP_PPCP_SLAVE_LATENCY_VAR=0x0000,
    .GAP_PPCP_STO_MULT_VAR=0x07d0,
    .GAP_TMR_LIM_ADV_TIMEOUT_VAR=0x4650,
    .GAP_TMR_GEN_DISC_SCAN_VAR=0x0300,
    .GAP_TMR_LIM_DISC_SCAN_VAR=0x0300,
    .GAP_TMR_PRIV_ADDR_INT_VAR=0x3A98,
    .GAP_TMR_CONN_PAUSE_CT_VAR=0x0064,
    .GAP_TMR_CONN_PAUSE_PH_VAR=0x01F4,
    .GAP_TMR_CONN_PARAM_TIMEOUT_VAR=0x0BB8,
    .GAP_TMR_LECB_CONN_TIMEOUT_VAR=0x0BB8,
    .GAP_TMR_SCAN_FAST_PERIOD_VAR=0x0C00,
    .GAP_TMR_ADV_FAST_PERIOD_VAR=0x0BB8,
    .GAP_LIM_DISC_SCAN_INT_VAR=0x0012,
    .GAP_SCAN_FAST_INTV_VAR=0x0030,
    .GAP_SCAN_FAST_WIND_VAR=0x0030,
    .GAP_SCAN_SLOW_INTV1_VAR=0x00CD,
    .GAP_SCAN_SLOW_INTV2_VAR=0x019A,
    .GAP_SCAN_SLOW_WIND1_VAR=0x0012,
    .GAP_SCAN_SLOW_WIND2_VAR=0x0024,
    .GAP_ADV_FAST_INTV1_VAR=0x0030,
    .GAP_ADV_FAST_INTV2_VAR=0x0064,
    .GAP_ADV_SLOW_INTV_VAR=0x00B0,
    .GAP_INIT_CONN_MIN_INTV_VAR=0x0018,
    .GAP_INIT_CONN_MAX_INTV_VAR=0x0028,
    .GAP_INQ_SCAN_INTV_VAR=0x0012,
    .GAP_INQ_SCAN_WIND_VAR=0x0012,
    .GAP_CONN_SUPERV_TIMEOUT_VAR=0x07D0,
    .GAP_CONN_MIN_CE_VAR=0x0000,
    .GAP_CONN_MAX_CE_VAR=0xFFFF,
    .GAP_CONN_LATENCY_VAR=0x0000,
    //.GAP_LE_MASK_VAR=0x1F,
    .GAP_MAX_LE_MTU_VAR=512,
#if defined (__DA14586__)
    .GAP_DEV_NAME_VAR="DA14586",
#elif defined (__DA14531__)
    .GAP_DEV_NAME_VAR="DA14531",
#else
    .GAP_DEV_NAME_VAR="DA14585",
#endif
};
#endif
#endif // (__NON_BLE_EXAMPLE__)

/// @}
