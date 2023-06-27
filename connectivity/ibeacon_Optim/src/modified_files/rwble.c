/**
 ****************************************************************************************
 *
 * @file rwble.c
 *
 * @brief Entry points the BLE software
 *
 * Copyright (C) RivieraWaves 2009-2014
 * Copyright (C) 2017-2019 Modified by Dialog Semiconductor
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "arch.h"
#include <string.h>
#include "co_version.h"
#include "rwble.h"
#include "ke_event.h"
#include "ke_timer.h"
#include "co_buf.h"
#include "lld.h"
#include "llc.h"
#include "llm.h"
#include "dbg.h"
#include "ea.h"
#include "lld_evt.h"
#include "reg_blecore.h"
#include "syscntl.h"

#include "nvds.h"         // NVDS definitions

#include "user_app.h"


#include "gtl_eif.h"
#include "uart.h"
#include "ke_mem.h"
#include "gpio.h"
#include "system_library.h"
#include "adc.h"
#include "user_periph_setup.h"
#include "arch_api.h"

#include "arch_system.h"
#include "arch_wdg.h"
#include "llc_util.h"

#include "arch_rom.h"
#include "hci.h"
#include "h4tl.h"

#include "co_math.h"
#if defined (__DA14531__)
#include "otp_cs.h"
#endif

#ifdef DG_DBG_SLEEP
    #include "dbg_sleep.h"
#endif

#if defined (__DA14531__)
#include "rf_531.h"
#endif


#include "lld_util.h"    
uint8_t change_bd_addr_flag = 0; 
uint8_t seed_region = 0x00;
uint8_t store_adv_data_len                     __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t test_scan_rsp_data_len                __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t store_adv_data[ADV_DATA_LEN]           __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t test_scan_rsp_data[SCAN_RSP_DATA_LEN] __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
extern ibeacon_adv_payload_t adv_payload;
extern ibeacon_adv_payload_t_new adv_payload_new;


#if (WLAN_COEX_ENABLED)
#include "wlan_coex.h"
#endif

last_ble_evt        arch_rwble_last_event           __SECTION_ZERO("retention_mem_area0");
boost_overhead_st   set_boost_low_vbat1v_overhead   __SECTION_ZERO("retention_mem_area0");

extern uint16_t clk_freq_trim_reg_value;

#if defined (__DA14531__)
extern uint16_t xtal_wait_trim;
#endif

#if (BLE_METRICS)
arch_ble_metrics_t            metrics                  __SECTION_ZERO("retention_mem_area0");
#endif //(BLE_METRICS)

#if (USE_XTAL16M_ADAPTIVE_SETTLING && USE_POWER_OPTIMIZATIONS)
    uint32_t twirq_set         __SECTION_ZERO("retention_mem_area0");
    uint32_t twirq_set_min     __SECTION_ZERO("retention_mem_area0");
    uint32_t twirq_set_max     __SECTION_ZERO("retention_mem_area0");
    static uint8_t cal_failed;
#endif

/*
 * FORWARD DECLARATION OF GLOBAL FUNCTIONS
 ****************************************************************************************
 */
void patched_ble_regs_pop(void);
extern void rf_reinit(void);

/*
 * GLOBAL VARIABLES DECLARATION
 ****************************************************************************************
 */

extern uint32_t sleep_lp_cycles;
extern uint32_t slp_period_retained;


/*
 * DEFINES
 ****************************************************************************************
 */
#ifndef BLE_WAKEUP_LP_HANDLER_ENTER
    #define BLE_WAKEUP_LP_HANDLER_ENTER()
#endif

#ifndef BLE_WAKEUP_LP_HANDLER_EXIT
    #define BLE_WAKEUP_LP_HANDLER_EXIT()
#endif

#ifndef BLE_SLP_HANDLER_ENTER
    #define BLE_SLP_HANDLER_ENTER()
#endif

#ifndef BLE_SLP_HANDLER_EXIT
    #define BLE_SLP_HANDLER_EXIT()
#endif

#ifndef POWER_UP_ENTER
    #define POWER_UP_ENTER()
#endif

#ifndef POWER_UP_EXIT
    #define POWER_UP_EXIT()
#endif

#ifndef BLE_CSCNT_HANDLER_ENTER
    #define BLE_CSCNT_HANDLER_ENTER()
#endif

#ifndef BLE_CSCNT_HANDLER_EXIT
    #define BLE_CSCNT_HANDLER_EXIT()
#endif

#ifndef BLE_FINETGTIM_HANDLER_ENTER
    #define BLE_FINETGTIM_HANDLER_ENTER()
#endif

#ifndef BLE_FINETGTIM_HANDLER_EXIT
    #define BLE_FINETGTIM_HANDLER_EXIT()
#endif

#ifndef DLG_EVENT_HANDLER_ENTER
    #define DLG_EVENT_HANDLER_ENTER()
#endif

#ifndef DLG_EVENT_HANDLER_EXIT
    #define DLG_EVENT_HANDLER_EXIT()
#endif

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */


#if (BLE_METRICS)

/**
 ****************************************************************************************
 * @brief Updates BLE packet metrics.
 *
 * @param[in] pkts  Number of received packets.
 ****************************************************************************************
*/
static void update_ble_metrics(uint8_t pkts)
{
    uint8_t rx_cnt = pkts;
    uint8_t rx_hdl = co_buf_rx_current_get();

    while(rx_cnt--)
    {
        struct co_buf_rx_desc *rxdesc = co_buf_rx_get(rx_hdl);
        uint8_t status = rxdesc->rxstatus & 0x7F;

        if (status & (BLE_MIC_ERR_BIT | BLE_CRC_ERR_BIT | BLE_LEN_ERR_BIT | BLE_TYPE_ERR_BIT | BLE_SYNC_ERR_BIT))
        {
            metrics.rx_err++;
            if(status & BLE_SYNC_ERR_BIT)   metrics.rx_err_sync ++;
            if(status & BLE_CRC_ERR_BIT)    metrics.rx_err_crc ++;
        }
        else
        {
#if defined (__DA14531__)
            uint16_t reg_val = (GetWord32(RF_RSSI_RESULT_REG) >> 12);
#else
            uint16_t reg_val = (GetWord16(RF_RSSI_RESULT_REG) >> 8);
#endif
            if (!metrics.rx_pkt)
            {
                metrics.rx_rssi = reg_val;
                metrics.rx_pkt++;
            }
            else
            {
                // Reminder: overflow if 32-bit boundary is exceeded
                uint32_t tmp_sum = metrics.rx_rssi * metrics.rx_pkt;
                metrics.rx_pkt++;
                metrics.rx_rssi = (uint16_t)((tmp_sum + reg_val)/(metrics.rx_pkt));
            }
        }
        rx_hdl = co_buf_rx_next(rx_hdl);
    }
}

#endif // BLE_METRICS

uint32_t ble_finetim_corr __SECTION_ZERO("retention_mem_area0");

static void lld_sleep_compensate_core(uint32_t dur_us)
{
        uint32_t fintetime_correction;
        uint32_t slot_cnt;
        uint32_t usec_cnt;

        // Adjust for any shifts done after sleep...
        dur_us += ble_finetim_corr;
        // Reset for future use
        ble_finetim_corr = 0;

        // The correction values are then deduced from the sleep duration in us
        slot_cnt = dur_us / LLD_EVT_SLOT_DURATION;
        ble_basetimecntcorr_set(slot_cnt);

        /* If the sleep duration is a multiple of slot then fine timer correction is set to 0 else
         * it is set to the difference. Note that multiplication is much faster than division, so
         * the modulo operator is not used! */
        usec_cnt = dur_us - (slot_cnt * LLD_EVT_SLOT_DURATION);
        fintetime_correction = ((usec_cnt == 0) ? 0 : (LLD_EVT_SLOT_DURATION - usec_cnt));
        ble_finecntcorr_set((uint16_t)fintetime_correction);

        // Start the correction
        ble_deep_sleep_corr_en_setf(1);
}

void lld_sleep_compensate_func(void)
{
    uint32_t dur_us;
    uint32_t slp_period;

    // Get the number of low power sleep period
    slp_period = ble_deepslstat_get();

    // Convert sleep duration into us
    dur_us = lld_sleep_lpcycles_2_us_sel_func(slp_period);

    lld_sleep_compensate_core(dur_us);

#if (USE_POWER_OPTIMIZATIONS)
    slp_period_retained = slp_period;

#if (USE_XTAL16M_ADAPTIVE_SETTLING)
    if ((sleep_lp_cycles && (sleep_lp_cycles < slp_period)) && (!cal_failed))
    {
        twirq_set = co_min(twirq_set + (slp_period - sleep_lp_cycles + XTAL16_LP_CYCLES_OFFSET), twirq_set_max);
    }
#else
#if (DEVELOPMENT_DEBUG)
    // If this Assertion hits then the LP ISR lasts longer than the time
    // that has been reserved via LP_ISR_TIME_XTAL32_CYCLES and LP_ISR_TIME_USEC.
    if (sleep_lp_cycles && (sleep_lp_cycles < slp_period))
        ASSERT_WARNING(0);
#endif
#endif
#endif
}

/*
 * INTERRUPT HANDLERS
 ****************************************************************************************
 */

#if !defined (__DA14531__)
#if (!USE_POWER_OPTIMIZATIONS)

/*********************************************************************************
 *** BLE_WAKEUP_LP ISR
 ***/
void BLE_WAKEUP_LP_Handler(void)
{
    volatile long t=0;

    if (!USE_WDOG)
        wdg_freeze(); //Prepare WDOG, i.e. stop

//    // Gives 1dB higher sensitivity - UNTESTED
//    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
//    {
//        // Boost-mode
//        SetBits16(DCDC_CTRL2_REG, DCDC_CUR_LIM, 0x8); // 80mA
//    }
//    else
//    {
//        // Buck-mode
//        SetBits16(DCDC_CTRL2_REG, DCDC_CUR_LIM, 0x4); // 40mA
//    }

    /*
     * Wait and Switch to XTAL 16MHz
     * (default after each wake-up is RC 16MHz, but XTAL initialization sequence has been already initiated by PMU)
     * NOTE 1:
     *       1. If app does not need XTAL16MHz but RC16MHz is enough then skip this section!
     *       2. Wait-loop BEFORE activating PERIPH_PD in order to save some power...
     *
     * NOTE 2:
     *       1. It will save some power if you lower the clock while waiting for XTAL16 to settle.
     *       2. Could also switch to 32KHz, but then processing power is dramatically reduced (e.g. patching() routine may be too slow).
     */
    syscntl_use_lowest_amba_clocks();

#if defined (__DA14531__)
    while ( !GetBits16(SYS_STAT_REG, XTAL32M_SETTLED) )  // this takes some mili seconds
        __NOP(), __NOP(), __NOP();          // reduce some APB activity
#else
    while ( !GetBits16(SYS_STAT_REG, XTAL16_SETTLED) )  // this takes some mili seconds
        __NOP(), __NOP(), __NOP();          // reduce some APB activity
#endif

    SetBits16(CLK_CTRL_REG, SYS_CLK_SEL, 0);// select XTAL MHz clock
#if !defined (__DA14531__)
    SetBits16(CLK_16M_REG, RC16M_ENABLE, 0);// save power from RC 16MHz
#endif

    // and restore clock rates (refer to a couple of lines above)
    syscntl_use_highest_amba_clocks();


    /*
     * Since XTAL 16MHz is activated, power-up the Radio Subsystem (including BLE)
     *
     * Note that BLE core clock is masked in order to handle the case where RADIO_PD does not get into power down state.
     * The BLE clock should be active only as long as system is running at XTAL 16MHz (not at RC16 or 32KHz).
     * Also BLE clock should be enabled before powering up the RADIO Power Domain !
     */
    SetBits16(CLK_RADIO_REG, BLE_ENABLE, 1); // BLE clock enable
    SetBits16(PMU_CTRL_REG, RADIO_SLEEP, 0); // Power up! Note: System must run at 16MHz when powering up RADIO_PD.
    while (!(GetWord16(SYS_STAT_REG) & RAD_IS_UP)) {}; // this may take up to 1/2 of the 32KHz clock period

    /*
     * Init System Power Domain blocks: GPIO, WD Timer, Sys Timer, etc.
     * Power up and init Peripheral Power Domain blocks, and finally release the pad latches.
     */
#if !defined (__DA14531__)
    if (GetBits16(SYS_STAT_REG, PER_IS_DOWN))
    {
        periph_init();
    }
#endif

    /*
     * Wait for at least one Low Power clock edge after the power up of the Radio Power Domain *e.g. with ble_wait_lp_clk_posedge() )
     * or even better check the BLE_CNTL2_REG[WAKEUPLPSTAT] !
     * Thus you assure that BLE_WAKEUP_LP_IRQ is deasserted and BLE_SLP_IRQ is asserted.
     * After this check exit this ISE in order to proceed with BLE_SLP_Handler().
     */
    while ( GetBits32(BLE_CNTL2_REG, WAKEUPLPSTAT) || !GetBits32(BLE_INTSTAT_REG, SLPINTSTAT))
        if (t) break;

    // Now BLE_WAKEUP_LP_IRQ is deasserted and BLE_SLP_IRQ is asserted, so exit in order to proceed with BLE_SLP_Handler().
    // NOTE: If returning from BLE_WAKEUP_LP_Handler() will not cause BLE_SLP_Handler() to start,
    //       but the code after __WFI() is executed, then THERE WAS A SW SETUP PROBLEM !!!
    //       so it is recommended to place a check after __WFI().
}

/*********************************************************************************
 *** SLP_INT ISR
 ***/
__STATIC_INLINE void dlg_slp_isr(void)
{
    patched_ble_regs_pop(); // restore BLE registers from retained memory

#if (WLAN_COEX_ENABLED)
    wlan_coex_BLE_set();
#endif

    SetBits16(GP_CONTROL_REG, BLE_WAKEUP_REQ, 0);   //just to be sure

    if((BLE_APP_PRESENT == 0) || (BLE_INTEGRATED_HOST_GTL == 1 ))
    {
        // UART and pads have already been activated by periph_init() which is called
        // at initialization by main_func() and during wakeup by BLE_WAKEUP_LP_Handler().

        if(use_h4tl & 0x01)
        {
            h4tl_init(rwip_eif_get(RWIP_EIF_HCIC));
        }
        else
        {
            gtl_eif_init();
        }
    }

    // Handle wake-up
    rwip_wakeup();

    /*
    * Radio Subsystem initialization. Execute here after making sure that BLE core is awake.
    */
    rf_reinit();

    if (arch_clk_is_RCX20())
    {
        calibrate_rcx20(RCX_CALIB_TIME);
    }

    arch_rwble_last_event = BLE_EVT_SLP;
}

/*********************************************************************************
 *** CSCNT_INT ISR
 ***/
__STATIC_INLINE void dlg_cscnt_isr(void)
{
    // Handle end of wake-up
    rwip_wakeup_end();
    // Try to schedule immediately
    ea_finetimer_isr();

#if (WLAN_COEX_ENABLED)
    wlan_coex_finetimer_isr();
#endif

    arch_rwble_last_event = BLE_EVT_CSCNT;
}

/*********************************************************************************
 *** FINE_INT_ISR
 ***/
__STATIC_INLINE void dlg_finetgtim_isr(void)
{
    //CHECK_AND_CALL(custom_pti_set);
    ea_finetimer_isr();

#if (WLAN_COEX_ENABLED)
    wlan_coex_finetimer_isr();
#endif
}

#endif // !USE_POWER_OPTIMIZATIONS
#endif // !defined (__DA14531__)


#if (USE_POWER_OPTIMIZATIONS)


// forward declaration
void power_up(void);

#if (USE_XTAL16M_ADAPTIVE_SETTLING)
/**
 *****************************************************************************************
 * @brief Simple adjustment algorithm to determine a value of TWIRQ_SET which
 *        will guarantee that X% of the time, the crystal will be settled by
 *        now. In the case where the crystal is not settled, delay the switch
 *        to XTAL16M until it is settled.
 * @param[in]       the calibration time in clock cycles
 * @return          the frequency value
 *****************************************************************************************
 */
static uint32_t calibrate_XTAL16(uint8_t ticks_for_cal)
{
    uint32_t cnt = 0;
    /* Check if XTAL16 has settled into the correct frequency */
    SetBits16(CLK_REF_SEL_REG, REF_CLK_SEL, 0x02);
    SetWord16(CLK_REF_CNT_REG, ticks_for_cal);
    SetBits16(CLK_REF_SEL_REG, REF_CAL_START, 0x1);
    syscntl_use_lowest_amba_clocks();
    while(GetBits16(CLK_REF_SEL_REG, REF_CAL_START) != 0)
    {        __NOP(); };
    syscntl_use_highest_amba_clocks();

    uint32_t clk_ref_val_h_reg = GetWord16(CLK_REF_VAL_H_REG) << 16;
    cnt = clk_ref_val_h_reg + GetWord16(CLK_REF_VAL_L_REG);
    return cnt;
}
#endif

/*********************************************************************************
 *** BLE_WAKEUP_LP ISR
 ***/
void BLE_WAKEUP_LP_Handler(void)
{
    BLE_WAKEUP_LP_HANDLER_ENTER();

    if (!USE_WDOG)
        wdg_freeze(); // Prepare WDOG, i.e. stop

#if !defined (__DA14531__)
    /*
     * Check if XTAL16M (DA14585/586)
     */
    if (GetBits16(CLK_CTRL_REG, SYS_CLK_SEL) != 0)
    {

        // 1. Power up peripherals' power domain
        if (GetBits16(SYS_STAT_REG, PER_IS_DOWN))
        {
            periph_init();
        }

        // 2. Enable RC32K
        SetBits16(CLK_32K_REG, RC32K_ENABLE, 1);

        // 3. XTAL16M Trim Procedure
        // 3a. Start counting
        SetWord16(CLK_REF_CNT_REG, 255); // 255/32kHz = 11.1ms
        SetWord16(CLK_REF_SEL_REG, REF_CAL_START); // select RC32kHz oscillator, start!

        // 3b. Lower clocks
        syscntl_use_lowest_amba_clocks();

        // 3c. Wait until num_clk_cycles have been measured
        while(1)
        {
            uint32_t clk_ref_val_h_reg = GetWord16(CLK_REF_VAL_H_REG) << 16;
            uint32_t cnt = clk_ref_val_h_reg + GetWord16(CLK_REF_VAL_L_REG);
            if (cnt > CLK_TRIM_WAIT_CYCLES)
                break;
        }

        SetWord16(CLK_REF_SEL_REG, (REF_CAL_START | 1)); // stop!

        // 3d. Apply Trim value
        SetWord16(CLK_FREQ_TRIM_REG, clk_freq_trim_reg_value);

        // 4. Disable RC32K
        SetBits16(CLK_32K_REG, RC32K_ENABLE, 0);
    }
#endif

#if defined (__DA14531__)
    // Lower clocks
    syscntl_use_lowest_amba_clocks();
#endif

    // Wait until BLE core de-asserts the LP IRQ
    while( GetWord16(GP_CONTROL_REG) & BLE_WAKEUP_LP_IRQ)
        ;

#if defined (__DA14531__)
    // Speed up clocks for fast command execution
    syscntl_use_highest_amba_clocks();

    // 1. Load trim value to CLK_FREQ_TRIM_REG
    SetWord16(CLK_FREQ_TRIM_REG, clk_freq_trim_reg_value);

    // 2. Wait for additional s/w defined time. Use either of the following:
    //   - fixed time provided by SDK value (by default 1msec)
    //   - fixed time provided per design and read from OTP CS entry (XTAL_WAIT_TRIM in usec)
    SetWord16(CLK_REF_CNT_REG, xtal_wait_trim - 2); // Two extra cycles are required by the calibration circuit

    if (arch_clk_is_RCX20())
    {
        SetWord16(CLK_REF_SEL_REG, (REF_CAL_START) | 3); // select RCX oscillator, start!
    }
    else if (arch_clk_is_XTAL32())
    {
        SetWord16(CLK_REF_SEL_REG, (REF_CAL_START) | 2); // select XTAL32K oscillator, start!
    }

    // Lower clocks
    syscntl_use_lowest_amba_clocks();

    // 3. Wait until wait time is elapsed
    while (GetWord16(CLK_REF_SEL_REG) & REF_CAL_START) {};
#endif

    // Power up BLE
    power_up();

    BLE_WAKEUP_LP_HANDLER_EXIT();
}

void power_up(void)
{
    POWER_UP_ENTER();

    // Speed up clocks for fast command execution
    syscntl_use_highest_amba_clocks();

#if (USE_XTAL16M_ADAPTIVE_SETTLING)
    uint32_t cnt;
    static uint16_t success_cnt    __SECTION_ZERO("retention_mem_area0");
    uint8_t tries = XTAL16M_CAL_MAX_TRIES;
    cal_failed = 0;

    while(tries--)
    {
        cnt = calibrate_XTAL16(CAL_LP_TICKS);
        if(cnt < (EXPECTED_XTAL32K_CYCLES + 3) && cnt > (EXPECTED_XTAL32K_CYCLES - 3))
        {
            success_cnt++;
            if(success_cnt >= XTAL16_CALIBRATION_SUCCESS_COUNT)
            {
                success_cnt = 0;
                /*
                 * Minimum is 64 so we have ~2ms to get through LP_Handler
                 *
                 */
                twirq_set = co_max(twirq_set - 1, twirq_set_min);
            }
            break;
        }
        else
        {
            success_cnt = 0;
            twirq_set = co_min(twirq_set + 2, twirq_set_max);
            cal_failed = 1 ;
        }
    }
#endif

    /*
     * Check if XTAL16M (DA14585/586) or XTAL32M (DA14531) is already being used.
     */
    if (GetBits16(CLK_CTRL_REG, SYS_CLK_SEL) != 0)
    {
        /*
         * Switch to XTAL16M (DA14585/586) or XTAL32M (DA14531)
         */
        SetBits16(CLK_CTRL_REG, SYS_CLK_SEL, 0);    // select XTAL16M (DA14585/586) or XTAL32M (DA14531)
#if !defined (__DA14531__)
        SetBits16(CLK_16M_REG, RC16M_ENABLE, 0);    // save power from RC16M
#else
        // RC32M must always be enabled. It will be auto-switched off in sleep
#endif
    }

    /*
     * Since XTAL 16MHz is activated, power-up the Radio Subsystem (including BLE)
     *
     * Note that BLE core clock is masked in order to handle the case where RADIO_PD does not
     * get into power down state. The BLE clock should be active only as long as system is
     * running at XTAL 16MHz (not at RC16 or 32KHz). Also the BLE clock should be enabled
     * before powering up the RADIO Power Domain!
     */
    SetBits16(CLK_RADIO_REG, BLE_ENABLE, 1); // BLE clock enable
    SetBits16(PMU_CTRL_REG, RADIO_SLEEP, 0); // Power up! Note: System must run at 16MHz when powering up RADIO_PD.
    while (!(GetWord16(SYS_STAT_REG) & RAD_IS_UP)) {}; // this may take up to 1/2 of the 32KHz clock period

    /*
     * BLE is up. The register status can be restored.
     */
    patched_ble_regs_pop();

#ifdef DG_DBG_DIAG_PORT
    dbg_init_diag_port();
#endif

#if (WLAN_COEX_ENABLED)
    wlan_coex_BLE_set();
#endif

    /*
     * Check if BLE_SLP_IRQ has already asserted. In this case, we delayed in periph_init().
     * Increase LP_ISR_TIME_XTAL32_CYCLES and LP_ISR_TIME_USEC values to give more execution time
     * to periph_init().
     */
    if ( GetBits32(BLE_INTSTAT_REG, SLPINTSTAT) )
        ASSERT_WARNING(0);

    /*
     * Wait for BLE_SLP_IRQ to be asserted. Use low clock until LP is deasserted to preserve power.
     * After this check, exit this Exception Handler in order to proceed with the BLE_SLP_Handler().
     */
    while ( !GetBits32(BLE_INTSTAT_REG, SLPINTSTAT) ) {};

    POWER_UP_EXIT();
}

/*********************************************************************************
 *** SLP_INT ISR
 ***/
__STATIC_INLINE void dlg_slp_isr(void)
{
    /* >>> Start of sleep exit delay measurement <<< */
    BLE_SLP_HANDLER_ENTER();

    SetBits16(GP_CONTROL_REG, BLE_WAKEUP_REQ, 0);   //just to be sure

    if((BLE_APP_PRESENT == 0) || (BLE_INTEGRATED_HOST_GTL == 1 ))
    {
        // UART and pads have already been activated by periph_init() which is called
        // at initialization by main_func() and during wakeup by BLE_WAKEUP_LP_Handler().

        if(use_h4tl & 0x01)
        {
            h4tl_init(rwip_eif_get(RWIP_EIF_HCIC));
        }
        else
        {
            gtl_eif_init();
        }
    }

    // Handle wake-up
    rwip_wakeup();

    /* >>> End of sleep exit delay measurement <<< */

    /*
     * Radio Subsystem initialization.
     */
    rf_reinit();

    /*
     * Start RCX calibration (if applicable).
     */
    if (arch_clk_is_RCX20())
    {
        calibrate_rcx20(RCX_CALIB_TIME);
    }

    /*
     * Mark event type.
     */
    arch_rwble_last_event = BLE_EVT_SLP;

#if !defined (__DA14531__)
    /*
     * When in BOOST mode, sample VBAT1V to handle longer startup delays at low voltages.
     */
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 1)
    {
        if (set_boost_low_vbat1v_overhead == NOT_MEASURED)
        {
            // default is to apply the overhead
            set_boost_low_vbat1v_overhead = APPLY_OVERHEAD;

            // Use ADC to measure VBAT1V (approx. 40usec delay)
            if (GetBits16(GP_ADC_CTRL_REG, GP_ADC_EN) == 0)
            {
                uint32_t adc_sample = adc_get_vbat_sample(true);

                if (adc_sample > 0x22C) // higher than 1V
                    set_boost_low_vbat1v_overhead = NO_OVERHEAD;
            }
        }
    }
#endif

    BLE_SLP_HANDLER_EXIT();
}

#if defined (__DA14531__) && defined (CFG_ENHANCED_TX_PWR_CTRL)
extern void ble_arp_update_tx_pwr(void);
#endif

/*********************************************************************************
 *** CSCNT ISR
 ***/
__STATIC_INLINE void dlg_cscnt_isr(void)
{
    BLE_CSCNT_HANDLER_ENTER();

    // Handle end of wake-up
    rwip_wakeup_end();
    // Try to schedule immediately
    ea_finetimer_isr();

#if defined (__DA14531__) && defined (CFG_ENHANCED_TX_PWR_CTRL)
    // Check upcoming BLE event and update output power accordingly
    ble_arp_update_tx_pwr();
#endif

#if (WLAN_COEX_ENABLED)
    wlan_coex_finetimer_isr();
#endif

    arch_rwble_last_event = BLE_EVT_CSCNT;

    BLE_CSCNT_HANDLER_EXIT();
}

/*********************************************************************************
 *** FINE_INT_ISR
 ***/
__STATIC_INLINE void dlg_finetgtim_isr(void)
{
    BLE_FINETGTIM_HANDLER_ENTER();

    ea_finetimer_isr();

#if defined (__DA14531__) && defined (CFG_ENHANCED_TX_PWR_CTRL)
    // Check upcoming BLE event and update output power accordingly
    ble_arp_update_tx_pwr();
#endif

#if (WLAN_COEX_ENABLED)
    wlan_coex_finetimer_isr();
#endif

    if (arch_clk_is_RCX20())
    {
        calibrate_rcx20(RCX_CALIB_TIME);
    }

    BLE_FINETGTIM_HANDLER_EXIT();
}

#endif // USE_POWER_OPTIMIZATIONS


/*********************************************************************************
 *** RX_INT ISR
 ***/
__STATIC_INLINE void dlg_rx_isr(void)
{
#if (BLE_METRICS)
    update_ble_metrics(LLD_RX_IRQ_THRES);
#endif // (BLE_METRICS)

    lld_evt_rx_isr();
}

/*********************************************************************************
 *** EVENT_INT ISR
 ***/
__STATIC_INLINE void dlg_event_isr(void)
{
    DLG_EVENT_HANDLER_ENTER();

#if (BLE_METRICS)
    {
        // Get the current event programmed
        struct ea_elt_tag *elt = (struct ea_elt_tag *)co_list_pick(&lld_evt_env.elt_prog);
        // Get the associated BLE event
        struct lld_evt_tag *evt = LLD_EVT_ENV_ADDR_GET(elt);

        // Read total number of RX descriptor burnt during the event
        uint8_t rx_cnt = ble_rxdesccnt_getf(evt->conhdl);

        update_ble_metrics(rx_cnt - evt->rx_cnt);
    }
#endif // (BLE_METRICS)

    lld_evt_end_isr(false);

#if (WLAN_COEX_ENABLED)
    // Give priority to WLAN over BLE
    wlan_coex_prio_level(0);
#endif

    arch_rwble_last_event = BLE_EVT_END;

#if defined (__DA14531__)
    rf_adplldig_deactivate();
#endif

    DLG_EVENT_HANDLER_EXIT();
}

__STATIC_INLINE void dlg_eventapfa_isr(void)
{
    lld_evt_end_isr(true);

#if (WLAN_COEX_ENABLED)
    // Give priority to WLAN over BLE
    wlan_coex_prio_level(0);
#endif
}

__STATIC_INLINE void dlg_sw_isr(void)
{
    ea_sw_isr();
}

__STATIC_INLINE void dlg_grosstgtim_isr(void)
{
    lld_evt_timer_isr();
}

__STATIC_INLINE void dlg_crypt_isr(void)
{
    lld_crypt_isr();
}

__STATIC_INLINE void dlg_error_isr(void)
{
    ASSERT_WARNING(0);
}


__BLEIRQ void rwble_isr(void)
{
#if defined (__DA14531__)
    if (GetWord32(BLE_CNTL2_REG) & EMACCERRSTAT)
    {
        SetBits32(BLE_CNTL2_REG, EMACCERRACK, 1);
    }

    if (GetWord32(BLE_CNTL2_REG) & BLE_ARP_PHY_ERR_STAT)
    {
        SetBits32(BLE_CNTL2_REG, BLE_ARP_PHY_ERR_STAT, 1);
    }
#endif

    // Loop until no more interrupts have to be handled
    while (1)
    {
        // Check BLE interrupt status and call the appropriate handlers
        uint32_t irq_stat = ble_intstat_get();
        if (irq_stat == 0)
            break;

        // Event target interrupt
        if (irq_stat & BLE_FINETGTIMINTSTAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, FINETGTIMINT, 1);

            // Clear the interrupt
            ble_intack_clear(BLE_FINETGTIMINTACK_BIT);

            dlg_finetgtim_isr();

            DBG_SWDIAG(BLE_ISR, FINETGTIMINT, 0);
        }

        // Rx interrupt
        if (irq_stat & BLE_RXINTSTAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, RXINT, 1);

            ble_intack_clear(BLE_RXINTSTAT_BIT);

            dlg_rx_isr();

            DBG_SWDIAG(BLE_ISR, RXINT, 0);
        }
        // End of event interrupt
        if (irq_stat & BLE_EVENTINTSTAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, EVENTINT, 1);

            // Clear the interrupt
            ble_intack_clear(BLE_EVENTINTSTAT_BIT);

            dlg_event_isr();

            DBG_SWDIAG(BLE_ISR, EVENTINT, 0);

								if(change_bd_addr_flag % 2)
				
									{
										user_app_adv_start();
				
										/* Alternate User advertising data*/
										memcpy(store_adv_data, &adv_payload, sizeof(ibeacon_adv_payload_t));
										store_adv_data_len = sizeof(ibeacon_adv_payload_t);
				
									  /* Alternate Tx output power, Set output power to maximum value (0dBm) */
										rf_pa_pwr_set(RF_TX_PWR_LVL_0d0);
	
										/* Alternate Static random address */
										llm_le_env.rand_add.addr[0] = 0x11;
										llm_le_env.rand_add.addr[1] = 0x12;
										llm_le_env.rand_add.addr[2] = 0x13;
										llm_le_env.rand_add.addr[3] = 0x14;
										llm_le_env.rand_add.addr[4] = 0x15;
										llm_le_env.rand_add.addr[5] = 0x16;
				
									}
									else 
										
									{
										user_app_adv_start_new();
										
										/* Alternate User advertising data*/
										memcpy(store_adv_data, &adv_payload_new, sizeof(ibeacon_adv_payload_t_new));
										store_adv_data_len = sizeof(ibeacon_adv_payload_t_new);
							
										/* Alternate Tx output power, Set output power to maximum value (+2.5dBm) */
										rf_pa_pwr_set(RF_TX_PWR_LVL_PLUS_2d5);

										/* Alternate Static random address */
										llm_le_env.rand_add.addr[0] = 0x01;
										llm_le_env.rand_add.addr[1] = 0x02;
										llm_le_env.rand_add.addr[2] = 0x03;
										llm_le_env.rand_add.addr[3] = 0x04;
										llm_le_env.rand_add.addr[4] = 0x05;
										llm_le_env.rand_add.addr[5] = 0x06;
									}	         

										 app_easy_gap_update_adv_data(store_adv_data, store_adv_data_len, test_scan_rsp_data, test_scan_rsp_data_len);
																		
										//first byte of random static MAC address is incremented upon each beacon in a sequence of ten (inc_v) beacons
										llm_le_env.rand_add.addr[0] = seed_region + change_bd_addr_flag; 
									
										lld_util_set_bd_address(&llm_le_env.rand_add, ADDR_RAND);

										change_bd_addr_flag++;
										if(change_bd_addr_flag >= 10) change_bd_addr_flag = 0;
	
        } // End of event interrupt (BLE_EVENTINTSTAT_BIT)
	
        // AFPM interrupt
        if (irq_stat & BLE_EVENTAPFAINTSTAT_BIT)
        {
            // Clear the interrupt
            ble_intack_clear(BLE_EVENTAPFAINTSTAT_BIT);

            dlg_eventapfa_isr();
        }


        // SW interrupt
        if (irq_stat & BLE_SWINTSTAT_BIT)
        {
            // Clear the interrupt
            ble_intack_clear(BLE_SWINTSTAT_BIT);

            dlg_sw_isr();
        }

        #if (RW_BLE_SUPPORT)
        // General purpose timer interrupt
        if (irq_stat & BLE_GROSSTGTIMINTSTAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, GROSSTGTIMINT, 1);

            // Clear the interrupt
            ble_intack_clear(BLE_GROSSTGTIMINTACK_BIT);

            dlg_grosstgtim_isr();

            DBG_SWDIAG(BLE_ISR, GROSSTGTIMINT, 0);
        }
        #endif //(RW_BLE_SUPPORT)

        // End of encryption interrupt
        if (irq_stat & BLE_CRYPTINTSTAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, CRYPTINT, 1);

            ble_intack_clear(BLE_CRYPTINTSTAT_BIT);

            dlg_crypt_isr();

            DBG_SWDIAG(BLE_ISR, CRYPTINT, 0);
        }

        #if DEEP_SLEEP
        #if RW_BLE_SUPPORT
        // End of sleep interrupt
        if (irq_stat & BLE_SLPINTSTAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, SLPINT, 1);

            // Clear the interrupt
            ble_intack_clear(BLE_SLPINTACK_BIT);

            dlg_slp_isr();

            DBG_SWDIAG(BLE_ISR, SLPINT, 0);
        }

        // Slot interrupt
        if (irq_stat & BLE_CSCNTINTSTAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, CSCNTINT, 1);

            // Clear the interrupt
            ble_intack_clear(BLE_CSCNTINTACK_BIT);

            dlg_cscnt_isr();

            DBG_SWDIAG(BLE_ISR, CSCNTINT, 0);
        }
        #endif //RW_BLE_SUPPORT
        #endif //DEEP_SLEEP

        // Error interrupt
        if (irq_stat & BLE_ERRORINTSTAT_BIT)
        {
            DBG_SWDIAG(BLE_ISR, ERRORINT, 1);

            ///ASSERT_INFO(0, ble_errortypestat_get(), 0);
            // Clear the interrupt
            ble_intack_clear(BLE_ERRORINTSTAT_BIT);

            dlg_error_isr();

            DBG_SWDIAG(BLE_ISR, ERRORINT, 0);
        }
    }
		   /* Optimize the time between Tx pulses */
				ble_advtim_set (500);
}

#if defined (CFG_RF_DIAG_INT)

#if defined (__DA14531__)

__WEAK void ble_tx_evt_handler() {};
__WEAK void ble_rx_evt_handler() {};

// DA14531
void BLE_RF_DIAG_Handler(void)
{
    uint16_t rf_diagirq_stat;

    rf_diagirq_stat = GetWord16(RF_DIAGIRQ_STAT_REG);

//TXEN RISING EDGE
    if ( rf_diagirq_stat & (1<<3) ) //TXEN rising edge
    {
        ble_tx_evt_handler();
    }

//TXEN FALLING EDGE
    else if ( rf_diagirq_stat & (1<<1))  //TXEN falling edge
    {
    }

//RXEN RISING EDGE
    else if ( rf_diagirq_stat & (1<<0) ) //RXEN rising edge
    {
        ble_rx_evt_handler();
    }

//RXEN FALLING EDGE
    else if ( rf_diagirq_stat & (1<<2) ) //RXEN falling edge
    {
    }
}
#else
// DA14585, DA14586
void BLE_RF_DIAG_Handler(void)
{
    uint16_t diag_irq_stat;
    uint16_t rf_diag_irq_01;

#if (LUT_PATCH_ENABLED)
    uint16_t cn;
    cn = GetWord16(RF_BMCW_REG) & 0x003F;
#endif

    diag_irq_stat = GetWord16(RF_DIAGIRQ_STAT_REG);
    rf_diag_irq_01 = GetWord16(RF_DIAGIRQ01_REG);

#if (LUT_PATCH_ENABLED)
    const volatile struct LUT_CFG_struct *pLUT_CFG = &LUT_CFG;
    if(!pLUT_CFG->HW_LUT_MODE)
    {
        set_rf_cal_cap(cn);
    }
#endif

    if(( diag_irq_stat & DIAGIRQ_STAT_0) && ( rf_diag_irq_01 & DIAGIRQ_MASK_0))//check TXEN posedge
    {
        arch_rwble_last_event = BLE_EVT_TX;

        #if (PRODUCTION_TEST)
            test_tx_packet_nr++;
        #endif
    }

    if(( diag_irq_stat & DIAGIRQ_STAT_1) && ( rf_diag_irq_01 & DIAGIRQ_MASK_1))//check RXEN posedge
    {
        arch_rwble_last_event = BLE_EVT_RX;

        #if (PRODUCTION_TEST)
            test_rx_irq_cnt++;
        #endif
    }
}
#endif // DA14531
#endif // CFG_RF_DIAG_INT

/// @} RWBLE
