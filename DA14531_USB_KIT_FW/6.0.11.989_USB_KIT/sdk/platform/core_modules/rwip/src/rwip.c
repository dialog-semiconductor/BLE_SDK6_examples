/**
****************************************************************************************
*
* @file rwip.c
*
* @brief RW IP SW main module
*
* Copyright (C) RivieraWaves 2009-2014
* Copyright (C) 2017-2018 Modified by Dialog Semiconductor
*
****************************************************************************************
*/

/**
 ****************************************************************************************
 * @addtogroup RW IP SW main module
 * @ingroup ROOT
 * @brief The RW IP SW main module.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"    // RW SW configuration

#include <string.h>         // for mem* functions
#include "co_version.h"
#include "rwip.h"           // RW definitions
#include "arch.h"           // Platform architecture definition
#include "arch_api.h"


#if (BLE_EMB_PRESENT)
    #include "rwble.h"          // rwble definitions
#endif //BLE_EMB_PRESENT

#if (BLE_HOST_PRESENT)
    #include "rwble_hl.h"       // BLE HL definitions
    #include "gapc.h"
    #include "smpc.h"
    #include "gattc.h"
    #include "attc.h"
    #include "atts.h"
    #include "l2cc.h"
#endif //BLE_HOST_PRESENT

#if (BLE_APP_PRESENT)
    #include "app.h"            // Application definitions
#endif //BLE_APP_PRESENT

#include "syscntl.h"
#include "lld_sleep.h"      // definitions for sleep mode
#include "llc.h"

#include "plf.h"            // platform definition
#include "rf.h"             // RF definitions

#if (GTL_ITF)
    #include "gtl.h"
    #include "gtl_eif.h"
#endif //GTL_ITF

#include "h4tl.h"

#if (KE_SUPPORT)
    #include "ke.h"             // kernel definition
    #include "ke_event.h"       // kernel event
    #include "ke_timer.h"       // definitions for timer
    #include "ke_mem.h"         // kernel memory manager
    #include "ke_env.h"
#endif //KE_SUPPORT

#include "dbg.h"            // debug definition

#if ((BLE_APP_PRESENT) || ((BLE_HOST_PRESENT && (!GTL_ITF))))
    #include "app.h"
#endif //BLE_APP_PRESENT

#include "em_map_ble.h"
#include "arch_system.h"

#if defined (__DA14531__)
#include "otp_cs.h"
#endif

uint32_t sleep_lp_cycles __SECTION_ZERO("retention_mem_area0");
uint32_t slp_period_retained __SECTION_ZERO("retention_mem_area0"); // sleep counter value at wakeup

/*
 * FORWARD DECLARATION OF GLOBAL FUNCTIONS
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLES DECLARATION
 ****************************************************************************************
 */
extern boost_overhead_st set_boost_low_vbat1v_overhead;
extern uint64_t rcx_slot_duration;
extern struct arch_sleep_env_tag sleep_env;

uint32_t retained_BLE_EM_BASE_REG __SECTION_ZERO("retention_mem_area0");

#if (USE_XTAL16M_ADAPTIVE_SETTLING && USE_POWER_OPTIMIZATIONS)
    extern uint32_t twirq_set;
    extern uint32_t twirq_set_min;
    extern uint32_t twirq_set_max;
#endif

/*
 * PROFILING CODE
 ****************************************************************************************
 */
#if defined(DEBUG_RWIP_SLEEP)

    uint32_t dbg_rwip_sleep_step __SECTION_ZERO("retention_mem_area0");

    #define DEBUG_RWIP_SLEEP_STEP(step) \
        do {                            \
            dbg_rwip_sleep_step = step; \
        } while(0)
#else

    #define DEBUG_RWIP_SLEEP_STEP(step)

#endif // defined(DEBUG_RWIP_SLEEP)

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */
static uint32_t rwip_slot_2_lpcycles(uint32_t slot_cnt)
{
    uint32_t lpcycles;

    // Sanity check: The number of slots should not be too high to avoid overflow
    ASSERT_ERROR(slot_cnt < 1000000);

#ifdef XTAL_32000HZ
    // Compute the low power clock cycles - case of a 32kHz clock
    lpcycles = slot_cnt * 20;
#else
    // Compute the low power clock cycles - case of a 32.768kHz clock
    lpcycles = (slot_cnt << 11)/100;
#endif

    if(lpcycles)
    {
       lpcycles--;
    }

    
    return(lpcycles);
}


static uint32_t rwip_slot_2_lpcycles_rcx(uint32_t slot_cnt)
{
    uint32_t lpcycles;

    // Sanity check: The number of slots should not be too high to avoid overflow
    ASSERT_ERROR(slot_cnt < 1000000);

    lpcycles = (uint32_t) ((slot_cnt * rcx_slot_duration) >> 20);

    return lpcycles;
}

static uint32_t get_max_sleep_duration()
{
    uint32_t max_sleep_duration;

    if(BLE_APP_PRESENT)
    {
        if(BLE_INTEGRATED_HOST_GTL)
        {
            // Single processor application with external interface (GTL or HCI)
            max_sleep_duration = rom_cfg_table[max_sleep_duration_periodic_wakeup_pos];
        }
        else
        {
            // Single processor application
            max_sleep_duration = rom_cfg_table[max_sleep_duration_external_wakeup_pos];
        }
    }
    else
    {
        if (EXTERNAL_WAKEUP)
        {
            // External processor configuration with EXTERNAL_WAKEUP support
            max_sleep_duration = rom_cfg_table[max_sleep_duration_external_wakeup_pos];
        }
        else
        {
            // External processor configuration
            max_sleep_duration = rom_cfg_table[max_sleep_duration_periodic_wakeup_pos];
        }
    }

    return max_sleep_duration;
}

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void patched_ble_regs_push(void)
{
    // ROM function declaration
    void ble_regs_push(void);

    ble_regs_push();
    retained_BLE_EM_BASE_REG = GetWord32(BLE_EM_BASE_REG);
}

void patched_ble_regs_pop(void)
{
    // ROM function declaration
    void ble_regs_pop(void);

    ble_regs_pop();
    SetWord32(BLE_EM_BASE_REG, retained_BLE_EM_BASE_REG);
}

sleep_mode_t rwip_sleep(void)
{
    sleep_mode_t proc_sleep = mode_active;
#if !defined (__DA14531__)
    uint32_t twirq_set_value;
    uint32_t twirq_reset_value;
    uint32_t twext_value;
#endif

    uint32_t sleep_duration = get_max_sleep_duration();

#if (USE_POWER_OPTIMIZATIONS)
    // We may lower the clock now while we are waiting the BLE to go to sleep...
    bool slow_system_clk = false;
#endif

#if (!USE_XTAL16M_ADAPTIVE_SETTLING)
    #if (!DEVELOPMENT_DEBUG)
        uint32_t sleep_lp_cycles;
    #endif
#endif

    if (BLE_APP_PRESENT)
    {
        if ( arch_ble_ext_wakeup_get() || (rwip_env.ext_wakeup_enable == 2) )  // sleep forever!
            sleep_duration = 0;
    }
    else if (!EXTERNAL_WAKEUP) // sleep_duration will remain as it was set above....
    {
        if (rwip_env.ext_wakeup_enable == 2)
            sleep_duration = 0;
    }
        
    do
    {
        /************************************************************************
         **************            CHECK BLE WAKEUP REQ            **************
         ************************************************************************/
        if (GetBits16(GP_CONTROL_REG, BLE_WAKEUP_REQ))
        {
            break;
        }

        /************************************************************************
         **************            CHECK STARTUP FLAG             **************
         ************************************************************************/

        DEBUG_RWIP_SLEEP_STEP(0);

        // Do not allow sleep if system is in startup period 
        // (only applicable if XTAL32K is the LP clock)
        if (arch_clk_is_XTAL32())
        {
            if (check_sys_startup_period())
            {
                break;
            }
        }

        DEBUG_RWIP_SLEEP_STEP(1);

        /************************************************************************
         **************            CHECK KERNEL EVENTS             **************
         ************************************************************************/
        // Check if some kernel processing is ongoing
        if (!ke_sleep_check())
            break;
        
        DEBUG_RWIP_SLEEP_STEP(2);

        // Processor sleep can be enabled
        proc_sleep = mode_idle;
    
        /************************************************************************
         **************             CHECK ENABLE FLAG              **************
         ************************************************************************/
        // Check sleep enable flag
        if(!rwip_env.sleep_enable)
            break;

        DEBUG_RWIP_SLEEP_STEP(3);
        
        /************************************************************************
         **************           CHECK RADIO POWER DOWN           **************
         ************************************************************************/
        // Check if BLE + Radio are still sleeping
        if(GetBits16(SYS_STAT_REG, RAD_IS_DOWN)) {
            // If BLE + Radio are in sleep return the appropriate mode for ARM
            proc_sleep = mode_sleeping;
            break;
        }

        DEBUG_RWIP_SLEEP_STEP(4);

        /************************************************************************
         **************              CHECK RW FLAGS                **************
         ************************************************************************/
        // First check if no pending procedure prevents us from going to sleep
        if (rwip_prevent_sleep_get() != 0)
            break;

        DEBUG_RWIP_SLEEP_STEP(5);

        /************************************************************************
         *                                                                      *
         *                   CHECK DURATION UNTIL NEXT EVENT                    *
         *                                                                      *
         ************************************************************************/
        // If there's any timer pending, compute the time to wake-up to serve it
        if ((!arch_ble_ext_wakeup_get()) && (ke_env.queue_timer.first != NULL))
        {
            sleep_duration = get_max_sleep_duration();
        }

#if (USE_XTAL16M_ADAPTIVE_SETTLING && USE_POWER_OPTIMIZATIONS)
        set_sleep_delay();
#endif

#if defined (__DA14531__)
        set_sleep_delay();
#endif

        /************************************************************************
         **************            CHECK KERNEL TIMERS             **************
         ************************************************************************/
        // Compute the duration up to the next software timer expires
        if (!ke_timer_sleep_check(&sleep_duration, rwip_env.wakeup_delay))
            break;

        DEBUG_RWIP_SLEEP_STEP(6);

        // Check if a kernel timer has expired before the above ke_timer_sleep_check() 
        // function call. If that happens the ke_timer_sleep_check() will incorrectly 
        // allow the system to enter sleep, because the timeout of the first timer is 
        // already in the past, and consequently the timer event will not be serviced 
        // until the next system wake up (the rwip_sleep() function is being called 
        // while the interrupts are disabled).
        if ((ble_intstat_get() & BLE_GROSSTGTIMINTSTAT_BIT))
        {
            break;
        }

        DEBUG_RWIP_SLEEP_STEP(7);

        /************************************************************************
         **************                 CHECK EA                   **************
         ************************************************************************/
        if (!ea_sleep_check(&sleep_duration, rwip_env.wakeup_delay))
                break;

        DEBUG_RWIP_SLEEP_STEP(8);

        #if (TL_ITF)
        /************************************************************************
         **************                 CHECK TL                   **************
         ************************************************************************/
        if((BLE_APP_PRESENT == 0) || (BLE_INTEGRATED_HOST_GTL == 1 ))
        {       
            // Try to switch off TL
            if (!h4tl_stop())
            {
                proc_sleep = mode_active;
                break;
            }
        }
        #endif //TL_ITF
        
        DEBUG_RWIP_SLEEP_STEP(9);

        #if (GTL_ITF)
        /************************************************************************
         **************                 CHECK TL                   **************
         ************************************************************************/
        if((BLE_APP_PRESENT == 0) || (BLE_INTEGRATED_HOST_GTL == 1 ))
        {
            // Try to switch off Transport Layer
            if (!gtl_enter_sleep())
            {
                proc_sleep = mode_active;
                break;
            }
        }
        #endif //GTL_ITF

        DEBUG_RWIP_SLEEP_STEP(10);


        if(USE_POWER_OPTIMIZATIONS && ((BLE_APP_PRESENT == 0) || (BLE_INTEGRATED_HOST_GTL == 1 )))
        {
            bool sleep_check = false;

            do
            {
                /************************************************************************
                 **************            CHECK KERNEL TIMERS             **************
                 ************************************************************************/
                // Compute the duration up to the next software timer expires
                if (!ke_timer_sleep_check(&sleep_duration, rwip_env.wakeup_delay))
                    break;
                if ((ble_intstat_get() & BLE_GROSSTGTIMINTSTAT_BIT))
                {
                    break;
                }

                /************************************************************************
                 **************                 CHECK EA                   **************
                 ************************************************************************/
                if (!ea_sleep_check(&sleep_duration, rwip_env.wakeup_delay))
                        break;

                sleep_check = true;

            } while(0);

            if (!sleep_check)
            {
                extern char use_h4tl; // ROM variable
                if(use_h4tl & 0x01)
                {
                    h4tl_init(rwip_eif_get(RWIP_EIF_HCIC));
                }
                else
                {
                    gtl_eif_init();
                }

                // sleep is aborted and serial i/f communication is restored
                break;
            }
        }

#if defined (__DA14531__)
        syscntl_cfg_xtal32m_amp_reg(XTAL32M_AMP_REG_TRACKING);      // switch to TRACKING mode when going to sleep
#endif
        
        if (USE_POWER_OPTIMIZATIONS)
        {
            if ( arch_clk_is_RCX20() )
            {
                // The XTAL16M adaptive settling cannot be used with RCX
                ASSERT_WARNING(!USE_XTAL16M_ADAPTIVE_SETTLING)
                
                if (sleep_duration)
                {
                    uint32_t finecnt = ble_finetimecnt_get();

                    // If we are close before the half of this slot then the actual sleep entry will
                    // occur during the next one. But the sleep_duration will have been calculated
                    // based on the current slot...
                    if (finecnt <= (530) && finecnt >= (624/2))
                        sleep_duration--;
                }
            }
        }
#if defined (__DA14531__)
        extern uint32_t twirq_set_value, twirq_reset_value, twext_value;
        extern uint16_t xtal_wait_trim;

        /************************************************************************
         **************          PROGRAM CORE DEEP SLEEP           **************
         ************************************************************************/

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

        // TWEXT setting
        twext_value = xtal_wait_trim + twirq_reset_value;
#else
        /************************************************************************
         **************          PROGRAM CORE DEEP SLEEP           **************
         ************************************************************************/
        if ( arch_clk_is_RCX20() )
        {
            // Boost mode + RCX is not supported in DA14585/586
            if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 1)
                 ASSERT_WARNING(0);

            #if (!USE_POWER_OPTIMIZATIONS)
                twirq_set_value = lld_sleep_us_2_lpcycles_sel_func(XTAL_TRIMMING_TIME_USEC);
                twirq_reset_value = TWIRQ_RESET_VALUE;
                
                // TWEXT setting
                twext_value = TWEXT_VALUE_RCX;
            #else
                // Calculate the time we need to wake-up before "time 0" to do XTAL16 settling,
                // call periph_init() and power-up the BLE core.
                uint32_t lpcycles = lld_sleep_us_2_lpcycles_sel_func(LP_ISR_TIME_USEC);
                
                // Set TWIRQ_SET taking into account that some LP cycles are needed for the power up FSM.
                twirq_set_value = RCX_POWER_UP_TIME + lpcycles;
                if (sleep_env.slp_state == ARCH_EXT_SLEEP_OTP_COPY_ON)
                    twirq_set_value += RCX_OTP_COPY_OVERHEAD;

                // Program LP deassertion to occur when the XTAL16M has settled
                uint32_t xtal_settling_cycles = lld_sleep_us_2_lpcycles_sel_func(XTAL16M_SETTLING_IN_USEC);
                twirq_reset_value = lpcycles - xtal_settling_cycles;
                
                // TWEXT setting
                twext_value = lpcycles;
            #endif
        }
        else if ( arch_clk_is_XTAL32( ) )
        {
            #if (!USE_POWER_OPTIMIZATIONS)
                twirq_set_value = XTAL_TRIMMING_TIME;
                twirq_reset_value = TWIRQ_RESET_VALUE;
                twext_value = TWEXT_VALUE_XTAL32;
            #else
                // The time we need to wake-up before "time 0" to do XTAL16 settling,
                // call periph_init() and power-up the BLE core is LP_ISR_TIME_XTAL32_CYCLES in this case.
                
                #if (USE_XTAL16M_ADAPTIVE_SETTLING)
                    twirq_set_value = twirq_set;
                #else
                    // Set TWIRQ_SET taking into account that some LP cycles are needed for the power up FSM.
                    twirq_set_value = XTAL32_POWER_UP_TIME + LP_ISR_TIME_XTAL32_CYCLES;
                #endif

                if (sleep_env.slp_state == ARCH_EXT_SLEEP_OTP_COPY_ON)
                    twirq_set_value += XTAL32_OTP_COPY_OVERHEAD;
                
                // Adjust TWIRQ_SET in case of BOOST mode, if needed
                if (set_boost_low_vbat1v_overhead == APPLY_OVERHEAD)
                    twirq_set_value += BOOST_POWER_UP_OVERHEAD;
                set_boost_low_vbat1v_overhead = NOT_MEASURED;

                #if (USE_XTAL16M_ADAPTIVE_SETTLING)
                    twirq_reset_value = TWIRQRESET_XTAL32_TICKS;
                #else
                    // Program LP deassertion to occur when the XTAL16M has settled
                    twirq_reset_value = LP_ISR_TIME_XTAL32_CYCLES - XTAL16M_SETTLING_IN_XTAL32_CYCLES;
                #endif

                // TWEXT setting
                twext_value = LP_ISR_TIME_XTAL32_CYCLES;
            #endif
        }
#endif // __DA14531__

        //Prepare BLE_ENBPRESET_REG for next sleep cycle
        SetBits32(BLE_ENBPRESET_REG, TWIRQ_RESET, twirq_reset_value);   // TWIRQ_RESET
        SetBits32(BLE_ENBPRESET_REG, TWIRQ_SET, twirq_set_value);       // TWIRQ_SET
        SetBits32(BLE_ENBPRESET_REG, TWEXT, twext_value);               // TWEXT

        //Everything ready for sleep!
        proc_sleep = mode_sleeping;

        // Put BT core into deep sleep
        if ( arch_clk_is_XTAL32( ) )
            sleep_lp_cycles = rwip_slot_2_lpcycles(sleep_duration);
        else if ( arch_clk_is_RCX20() )
            sleep_lp_cycles = rwip_slot_2_lpcycles_rcx(sleep_duration);

        // Apply correction to sleep_lp_cycles in order to handle the additional
        // microseconds that may be added to the total sleep duration for clock
        // compensation due to ble_finetim_corr. This prevents getting more whole
        // slots for compensation than the ones requested in sleep_duration.
        if ( arch_clk_is_XTAL32() )
            sleep_lp_cycles -= 3;
        else if ( arch_clk_is_RCX20() )
            sleep_lp_cycles -= 1;

        // Apply correction for boundary condition
        if (sleep_lp_cycles == twirq_set_value + 1)
        {
           sleep_lp_cycles =  twirq_set_value;
        }
        
        lld_sleep_enter(sleep_lp_cycles, rwip_env.ext_wakeup_enable);

        /************************************************************************
         **************               SWITCH OFF RF                **************
         ************************************************************************/        

        /* >>> Start of BLE deep sleep entry delay measurement <<< */

        rwip_rf.sleep();
        
        #if (USE_POWER_OPTIMIZATIONS)
            if (BLE_APP_PRESENT)            
            {
                if ( app_use_lower_clocks_check() )
                {
                    // It will save some power if you lower the clock while waiting for STAT...
                    syscntl_use_lowest_amba_clocks();                    
                    slow_system_clk = true;
                }
            }
        #endif
                
        while(!ble_deep_sleep_stat_getf());

        /* >>> End of BLE deep sleep entry delay measurement <<< */

        //check and wait till you may disable the radio. 32.768KHz XTAL must be running!
        //(debug note: use BLE_CNTL2_REG:MON_LP_CLK bit to check (write 0, should be set to 1 by the BLE))
        while ( !(GetWord32(BLE_CNTL2_REG) & RADIO_PWRDN_ALLOW) ) {}
            
        SetBits32(BLE_CNTL2_REG, MON_LP_CLK, 0);              // Clear LP edge flag
        
        #if (USE_POWER_OPTIMIZATIONS)
            if (slow_system_clk)
            {
                // and restore clock rates (refer to a couple of lines above)
                syscntl_use_highest_amba_clocks();
            }
        #endif

        /* The time needed from SLP assertion until the completion of the clock
         * compensation is 54usec for single processor applications and 66usec
         * for external processor applications. The threshold is set to 70usec.
         */
        {
            const uint32_t ISR_DELAY = 70;
            extern uint32_t ble_finetim_corr;
            uint16_t rem_time;
#if defined (__DA14531__)
            rem_time = GetWord16(BLE_TIMER_REG);
#else
            rem_time = GetWord16(BLE_FINECNT_SAMP_REG);
#endif
            if (rem_time < ISR_DELAY)
            {
                ble_finetim_corr = ISR_DELAY - rem_time;
#if defined (__DA14531__)
                SetWord16(BLE_TIMER_REG, ISR_DELAY);
#else
                SetWord16(BLE_FINECNT_SAMP_REG, ISR_DELAY);
#endif
            }
        }

        patched_ble_regs_push(); // save BLE registers to retained memory
        
        while ( !(GetWord32(BLE_CNTL2_REG) & MON_LP_CLK) ) {}    // Wait for LP rising edge
           
        //BLE CLK must be turned off when DEEP_SLEEP_STAT is set
        SetBits16(CLK_RADIO_REG, BLE_ENABLE, 0);

    } while(0);

    return proc_sleep;
}

