/**
 ****************************************************************************************
 *
 * @file nmi_handler.c
 *
 * @brief NMI handler.
 *
 * Copyright (c) 2012-2019 Dialog Semiconductor. All rights reserved.
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

#include "datasheet.h"
#include "arch_wdg.h"
#include "arch.h"
#include "syscntl.h"

/*
 * This is the base address in Retention RAM where the stacked information will be copied.
 */
#if defined (__DA14531__)
    #define STATUS_BASE (0x07FC9050)
#else
    #define STATUS_BASE (0x07FD0050)
#endif

extern void dbg_prod_output(int mode, unsigned long *hardfault_args);

void NMI_HandlerC(unsigned long *hardfault_args)
{
    // Reached this point due to a WDOG timeout

    SetBits16(PMU_CTRL_REG, RADIO_SLEEP, 1);        // turn off radio PD
#if !defined (__DA14531__)
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 1);       // turn off peripheral power domain
#endif
    SetBits16(CLK_RADIO_REG, BLE_LP_RESET, 1);      // reset the BLE LP timer
    NVIC_ClearPendingIRQ(BLE_WAKEUP_LP_IRQn);       // clear any pending LP IRQs

    if (DEVELOPMENT_DEBUG)
    {
        wdg_freeze();            // Stop WDOG
#if defined (__DA14531__) && (!USE_POWER_MODE_BYPASS)

        // In Boost mode enable the DCDC converter to supply VBAT_HIGH for debugger pins
        if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
        {
            syscntl_dcdc_set_level(SYSCNTL_DCDC_LEVEL_3V0);
            SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 1);
        }
#endif
        syscntl_load_debugger_cfg();        // load debugger configuration to be able to re-attach the debugger

        *(volatile unsigned long *)(STATUS_BASE       ) = hardfault_args[0];    // R0
        *(volatile unsigned long *)(STATUS_BASE + 0x04) = hardfault_args[1];    // R1
        *(volatile unsigned long *)(STATUS_BASE + 0x08) = hardfault_args[2];    // R2
        *(volatile unsigned long *)(STATUS_BASE + 0x0C) = hardfault_args[3];    // R3
        *(volatile unsigned long *)(STATUS_BASE + 0x10) = hardfault_args[4];    // R12
        *(volatile unsigned long *)(STATUS_BASE + 0x14) = hardfault_args[5];    // LR
        *(volatile unsigned long *)(STATUS_BASE + 0x18) = hardfault_args[6];    // PC
        *(volatile unsigned long *)(STATUS_BASE + 0x1C) = hardfault_args[7];    // PSR
        *(volatile unsigned long *)(STATUS_BASE + 0x20) = (unsigned long)hardfault_args;    // Stack Pointer

        *(volatile unsigned long *)(STATUS_BASE + 0x24) = (*((volatile unsigned long *)(0xE000ED28)));    // CFSR
        *(volatile unsigned long *)(STATUS_BASE + 0x28) = (*((volatile unsigned long *)(0xE000ED2C)));    // HFSR
        *(volatile unsigned long *)(STATUS_BASE + 0x2C) = (*((volatile unsigned long *)(0xE000ED30)));    // DFSR
        *(volatile unsigned long *)(STATUS_BASE + 0x30) = (*((volatile unsigned long *)(0xE000ED3C)));    // AFSR
        *(volatile unsigned long *)(STATUS_BASE + 0x34) = (*((volatile unsigned long *)(0xE000ED34)));    // MMAR
        *(volatile unsigned long *)(STATUS_BASE + 0x38) = (*((volatile unsigned long *)(0xE000ED38)));    // BFAR

        if ((GetWord16(SYS_STAT_REG) & DBG_IS_UP) == DBG_IS_UP)
            __asm("BKPT #0\n");
        else
        {
            while(1);
        }
    }
    else // DEVELOPMENT_DEBUG
    {
        if (PRODUCTION_DEBUG_OUTPUT)
        {
#if !defined (__DA14531__)
            // Power up peripherals' power domain
            SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
            while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
#endif

            dbg_prod_output(0, hardfault_args);
        }
    }

    // Remap addres 0x00 to ROM and force execution
    SetWord16(SYS_CTRL_REG, (GetWord16(SYS_CTRL_REG) & ~REMAP_ADR0) | SW_RESET );

}
