/**
 ****************************************************************************************
 *
 * @file    arch_sleep.c
 *
 * @brief   Sleep control functions.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdbool.h>
#include "arch.h"
#include "arch_api.h"
#include "app.h"
#include "rwip.h"
#include "gpio.h"

/// Application Environment Structure
extern struct arch_sleep_env_tag    sleep_env;
static uint8_t sleep_md             __SECTION_ZERO("retention_mem_area0");
static uint8_t sleep_pend           __SECTION_ZERO("retention_mem_area0");
static uint8_t sleep_cnt            __SECTION_ZERO("retention_mem_area0");
static bool sleep_ext_force         __SECTION_ZERO("retention_mem_area0");

extern last_ble_evt arch_rwble_last_event;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void arch_disable_sleep(void)
{
    if (sleep_md == 0)
    {
        sleep_env.slp_state = ARCH_SLEEP_OFF;

        rwip_env.sleep_enable = false;

    }
    else
        sleep_pend = 0x80 | ARCH_SLEEP_OFF;
}

void arch_set_extended_sleep(bool otp_copy)
{
    if (sleep_md == 0)
    {
        sleep_env.slp_state = otp_copy ? ARCH_EXT_SLEEP_OTP_COPY_ON : ARCH_EXT_SLEEP_ON;

        rwip_env.sleep_enable = true;
    }
    else
    {
        sleep_pend = 0x80 | (otp_copy ? ARCH_EXT_SLEEP_OTP_COPY_ON : ARCH_EXT_SLEEP_ON);
    }
}

#if defined (__DA14531__)
void arch_set_deep_sleep(pd_sys_down_ram_t ram1,
                         pd_sys_down_ram_t ram2,
                         pd_sys_down_ram_t ram3,
                         bool pad_latch_en)
{
    // Stop watchdog timer
    wdg_freeze();

    // Disable Interrupts (CPSR)
    GLOBAL_INT_STOP()

    // Disable radio
    SetBits16(PMU_CTRL_REG, RADIO_SLEEP, 1);

    // Disable the BLE core clocks
    SetBits16(CLK_RADIO_REG, BLE_ENABLE, 0);

    // Apply HW reset to BLE_Timers
    SetBits16(CLK_RADIO_REG, BLE_LP_RESET, 1);

    // Clear quadec pending interrupt - mask quadec interrupt
    SetWord16(QDEC_CTRL_REG, QDEC_IRQ_STATUS);

    // Close peripheral clock
    SetBits16(CLK_PER_REG, QUAD_ENABLE, 0);

    // Clear all pending interrupts
    NVIC->ICPR[0] = 0xFFFFFFFFUL;

    // Debugger must be disabled before entering a sleep mode. Wait until debugger has been disabled.
    while (GetBits16(SYS_STAT_REG, DBG_IS_UP) != 0);
    SetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE, 0);

    // Set deep sleep mode
    SetBits16(HIBERN_CTRL_REG, HIBERNATION_ENABLE, 0);
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);

    // Configure the state of RAM blocks during the deep sleep mode
    SetBits16(RAM_PWR_CTRL_REG, RAM1_PWR_CTRL, ram1);
    SetBits16(RAM_PWR_CTRL_REG, RAM2_PWR_CTRL, ram2);
    SetBits16(RAM_PWR_CTRL_REG, RAM3_PWR_CTRL, ram3);

    // Perform HW reset on wake-up
    SetBits16(PMU_CTRL_REG, RESET_ON_WAKEUP, 1);

    // Enable/Disable latching of pads state during sleep
    SetBits16(PAD_LATCH_REG, PAD_LATCH_EN, pad_latch_en);

    // Enable the TLS (transparent light sleep) mode by setting the RAM mux to zero,
    // to optimize RAM consumption.
    // Use of this mux should only be allowed if VDD is 10% less not more.
    SetBits16(RAM_LPMX_REG, RAMx_LPMX, 7);

    // Set clamp output (0xF = 603mV)
    SetBits(POWER_AON_CTRL_REG, LDO_RET_TRIM, 0x0F);

    // Disable the testmode comparator
    SetBits(POWER_AON_CTRL_REG, FORCE_RUNNING_COMP_DIS, 1);

    // Perform the following steps when in boost (or bypass) mode
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
    {
        // Always LDO_LOW Off
        SetBits16(POWER_CTRL_REG, LDO_LOW_CTRL_REG, 1);

        // Force connection between VBAT_HIGH and VBAT_LOW
        SetBits(POWER_AON_CTRL_REG, VBAT_HL_CONNECT_RES_CTRL, 2);
    }
    // Perform the following steps, when in buck mode
    else
    {
        // High-current mode in active, Low-current mode in sleep
        SetBits16(POWER_CTRL_REG, LDO_LOW_CTRL_REG, 3);

        // Set automatic connection control between VBAT_HIGH and VBAT_LOW
        SetBits(POWER_AON_CTRL_REG, VBAT_HL_CONNECT_RES_CTRL, 3);
    }

    // Disable the POR triggered from VBAT_HIGH voltage level sensing
    SetBits(POWER_AON_CTRL_REG, POR_VBAT_HIGH_RST_MASK, 1);

    // Enable the POR triggered from VBAT_LOW voltage level sensing
    SetBits(POWER_AON_CTRL_REG, POR_VBAT_LOW_RST_MASK, 0);

    // Set for proper RCX operation
    SetBits16(GP_DATA_REG, 0x60, 2);

    __NOP();
    __NOP();
    __NOP();

    // Enter Deep Sleep Mode
    __WFI();
}
#else
void arch_set_deep_sleep(bool ext_wakeup)
{
    wdg_freeze();                                               // Stop watchdog timer

    GLOBAL_INT_STOP()                                           // disable interrupts

    SetBits16(PMU_CTRL_REG, RADIO_SLEEP, 1);                    // disable radio

    SetBits16(CLK_RADIO_REG, BLE_ENABLE, 0);                    // disable the BLE core clocks

    SetBits16(CLK_RADIO_REG, BLE_LP_RESET, 1);                  // apply HW reset to BLE_Timers

    SCB->SCR |= 1<<2;                                           // enable deep sleep

    SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 0);                   // activate PAD latches

    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 1);                   // disable peripherals

    SetBits16(PMU_CTRL_REG, RETENTION_MODE, 0x00);              // turn off all RAM blocks

    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 0);           // set BIAS to '0' if sleep has been decided

    SetBits16(CLK_32K_REG, XTAL32K_DISABLE_AMPREG, 0);          // enable the amplitude regulation of the XTAL32K osc

    if (ext_wakeup)
    {
        GPIO_DisablePorPin();                                   // Disable if any configured POR pins
        SetBits16(PMU_CTRL_REG, RESET_ON_WAKEUP, 1);            // enable boot after wake-up
    }
    else
    {
        SetBits16(PMU_CTRL_REG, RESET_ON_WAKEUP, 0);
        SetWord16(WKUP_CTRL_REG, 0x00);                         // disable wake-up interrupt
    }

    *((volatile unsigned long *)(0xE000E280)) = 0xFFFFFFFF;     // clear all interrupts pending status

    __NOP();
    __NOP();
    __NOP();

    __WFI();                                                      // enter deep sleep mode
}
#endif


#if defined (__DA14531__)
void arch_set_hibernation(uint8_t wkup_mask,
                          pd_sys_down_ram_t ram1,
                          pd_sys_down_ram_t ram2,
                          pd_sys_down_ram_t ram3,
                          remap_addr0_t remap_addr0,
                          bool pad_latch_en)
{

    // Check GPIOs wake up mask
    wkup_mask >>= 1;
    ASSERT_WARNING((wkup_mask & ~0x1F) == 0);

    // Assert warning if RAM 1 or RAM 3 is remapped to address 0 and the respective block is not retained.
    ASSERT_WARNING(remap_addr0 == REMAP_ADDR0_TO_RAM1 ? (ram1 == PD_SYS_DOWN_RAM_ON) : \
                   remap_addr0 == REMAP_ADDR0_TO_RAM3 ? (ram3 == PD_SYS_DOWN_RAM_ON) : true);

    // Stop watchdog timer
    wdg_freeze();

    // Disable Interrupts
    GLOBAL_INT_STOP()

    // Clear all pending interrupts
    NVIC->ICPR[0] = 0xFFFFFFFFUL;

    // Debugger must be disabled before entering a sleep mode. Wait until debugger has been disabled.
    while (GetBits16(SYS_STAT_REG, DBG_IS_UP) != 0);
    SetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE, 0);

    // Set the wake up pins
    SetBits16(HIBERN_CTRL_REG, HIBERN_WKUP_MASK, wkup_mask);

    // Check the output of the clockless wakeup XOR tree to determine the wake up polarity
    if (GetBits16(ANA_STATUS_REG, CLKLESS_WAKEUP_STAT) == 0)
    {
        SetBits16(HIBERN_CTRL_REG, HIBERN_WKUP_POLARITY, GetBits16(HIBERN_CTRL_REG, HIBERN_WKUP_POLARITY) == 0 ? 1 : 0);
    }

    while (GetBits16(ANA_STATUS_REG, CLKLESS_WAKEUP_STAT) == 0);

    // Set hibernation sleep mode
    SetBits16(HIBERN_CTRL_REG, HIBERNATION_ENABLE, 1);
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);

    // Configure the state of RAM blocks during hibernation mode
    SetBits16(RAM_PWR_CTRL_REG, RAM1_PWR_CTRL, ram1);
    SetBits16(RAM_PWR_CTRL_REG, RAM2_PWR_CTRL, ram2);
    SetBits16(RAM_PWR_CTRL_REG, RAM3_PWR_CTRL, ram3);

    // Remap address 0
    SetBits16(SYS_CTRL_REG, REMAP_ADR0, remap_addr0);

    // Enable/Disable latching of pads state during sleep
    SetBits16(PAD_LATCH_REG, PAD_LATCH_EN, pad_latch_en);

    // Enable the TLS (transparent light sleep) mode by setting the RAM mux to zero,
    // to optimize RAM consumption.
    // Use of this mux should only be allowed if VDD is 10% less not more.
    SetBits16(RAM_LPMX_REG, RAMx_LPMX, (ram3 << 2) | (ram2 << 1) | ram1);

    // Set clamp output (0xB = 872mV)
    SetBits(POWER_AON_CTRL_REG, LDO_RET_TRIM, 0x0B);

    // Disable the testmode comparator
    SetBits(POWER_AON_CTRL_REG, FORCE_RUNNING_COMP_DIS, 1);

    // Perform the following steps when in boost (or bypass) mode
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
    {
        // Force connection between VBAT_HIGH and VBAT_LOW
        SetBits(POWER_AON_CTRL_REG, VBAT_HL_CONNECT_RES_CTRL, 2);
    }
    // Perform the following steps, when in buck mode
    else
    {
        // Set automatic connection control between VBAT_HIGH and VBAT_LOW
        SetBits(POWER_AON_CTRL_REG, VBAT_HL_CONNECT_RES_CTRL, 3);
    }

    // Disable the POR triggered from VBAT_HIGH voltage level sensing
    SetBits(POWER_AON_CTRL_REG, POR_VBAT_HIGH_RST_MASK, 1);

    // Enable the POR triggered from VBAT_LOW voltage level sensing
    SetBits(POWER_AON_CTRL_REG, POR_VBAT_LOW_RST_MASK, 0);

    // Set for proper RCX operation
    SetBits16(GP_DATA_REG, 0x60, 2);

    __NOP();
    __NOP();
    __NOP();

    // Enter Hibernation Mode
    __WFI();
}
#endif

void arch_set_sleep_mode(sleep_state_t sleep_state)
{
    switch (sleep_state)
    {
        case ARCH_SLEEP_OFF:
            arch_disable_sleep();
            break;
        case ARCH_EXT_SLEEP_ON:
            arch_set_extended_sleep(false);
            break;
        case ARCH_EXT_SLEEP_OTP_COPY_ON:
            arch_set_extended_sleep(true);
            break;
        default:
            break;
    }
}

uint8_t arch_get_sleep_mode(void)
{
    uint8_t ret = 0;

    switch(sleep_env.slp_state)
    {
        case ARCH_SLEEP_OFF:
            ret = 0; break;
        case ARCH_EXT_SLEEP_ON:
            ret = 1; break;
        case ARCH_EXT_SLEEP_OTP_COPY_ON:
            ret = 2; break;
    }

    return ret;
}

void arch_restore_sleep_mode(void)
{
    uint8_t cur_mode;

    if (sleep_cnt > 0)
        sleep_cnt--;

    if (sleep_cnt > 0)
        return;     // cannot restore it yet. someone else has requested active mode and we'll wait him to release it.

    if (sleep_pend != 0)
    {
        sleep_md = sleep_pend & 0x3;
        sleep_pend = 0;
    }
    else if (sleep_md)
        sleep_md--;

    cur_mode = sleep_md;
    sleep_md = 0;

    switch(cur_mode)
    {
       case 0:  break;
       case 1:  arch_set_extended_sleep(false); break;
       case 2:  arch_set_extended_sleep(true); break;
       default: break;
    }
}

void arch_force_active_mode(void)
{
    uint8_t cur_mode;

    sleep_cnt++;

    if (sleep_md == 0)  // add this check for safety! If it's called again before restore happens then sleep_md won't get overwritten
    {
        cur_mode = arch_get_sleep_mode();
        cur_mode++;     // 1: disabled, 2: extended sleep without OTP copy, 3: extended sleep with OTP copy (!=0 ==> sleep is in forced state)
        arch_disable_sleep();
        sleep_md = cur_mode;
    }
}

void arch_ble_ext_wakeup_on(void)
{
    sleep_ext_force = true;
}

void arch_ble_ext_wakeup_off(void)
{
    sleep_ext_force = false;
}

bool arch_ble_ext_wakeup_get(void)
{
    return sleep_ext_force;
}

bool arch_ble_force_wakeup(void)
{
    bool retval = false;

    // If BLE is sleeping, wake it up!
    GLOBAL_INT_DISABLE();
    if (GetBits16(CLK_RADIO_REG, BLE_ENABLE) == 0) { // BLE clock is off
        SetBits16(GP_CONTROL_REG, BLE_WAKEUP_REQ, 1);
        retval = true;
    }
    GLOBAL_INT_RESTORE();

    return retval;
}

last_ble_evt arch_last_rwble_evt_get(void)
{
    return arch_rwble_last_event;
}


