/**
 ****************************************************************************************
 *
 * @file hardfault_handler.c
 *
 * @brief Hardfault handler source code file.
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
#include "arch.h"
#include "uart.h"
#include "gpio.h"
#include "arch_wdg.h"
#include "syscntl.h"
#include "user_periph_setup.h"

/*
 * This is the base address in Retention RAM where the stacked information will be copied.
 */
#if defined (__DA14531__)
    #define STATUS_BASE (0x07FC9000)
#else
    #define STATUS_BASE (0x07FD0000)
#endif

#if PRODUCTION_DEBUG_OUTPUT

#if (DEVELOPMENT_DEBUG)
#error "DEVELOPMENT_DEBUG cannot be combined with PRODUCTION_DEBUG_OUTPUT"
#endif

static const char dbg_names[8][4] = { "r0 ", "r1 ", "r2 ", "r3 ", "r12", "lr ", "pc ", "psr" };

void dbg_prod_prepare(unsigned char *buf, int value, int index)
{
    int i;
    int nibble;

    buf[0] = dbg_names[index][0];
    buf[1] = dbg_names[index][1];
    buf[2] = dbg_names[index][2];
    buf[3] = '=';
    buf[4] = '0';
    buf[5] = 'x';

    for (i = 13; i >= 6; i--)
    {
        nibble = value & 0xF;
        if (nibble < 10)
            buf[i] = '0' + nibble;
        else
            buf[i] = 'a' + nibble - 10;
        value >>= 4;
    }
    buf[14] = '\r';
    buf[15] = '\n';
}

void dbg_prod_output(int mode, unsigned long *hardfault_args)
{
    unsigned char string[16];
    int i;

    // Switch to XTAL16 clock, if necessary
    if (GetBits16(CLK_CTRL_REG, RUNNING_AT_XTAL16M) == 0)
    {
        while ( !GetBits16(SYS_STAT_REG, XTAL16_SETTLED) ); // this takes some mili seconds

        SetBits16(CLK_CTRL_REG, SYS_CLK_SEL, 0);    // select XTAL 16MHz
    }

    // Configure UART
    GPIO_ConfigurePin(PRODUCTION_DEBUG_PORT, PRODUCTION_DEBUG_PIN, OUTPUT, PID_UART1_TX, false);
    SetBits16(CLK_PER_REG, UART1_ENABLE, 1);        // enable clock - always @16MHz
    uart_init(UART_BAUDRATE_115K2, UART_FRAC_BAUDRATE_115K2, UART_CHARFORMAT_8);

    // Debug status - short messages only!

    if (mode)
    {
        uart_write((uint8_t *)("\n--- HF STATUS\r\n"), 16, NULL);
    }
    else
    {
        uart_write((uint8_t *)("\n--- NMI STATUS\r\n"), 17, NULL);
    }
    for (volatile int j = 0; j < 5000; j++);

    for (i = 0; i < 8; i++)
    {
        dbg_prod_prepare(string, hardfault_args[i], i);
        uart_write(string, 16, NULL);
        for (volatile int j = 0; j < 5000; j++);
    }
}
#endif


/*
 * Usage
 * -----
 *
 * Upon exception, all valuable (and available) information will be copied to the area
 * with base address equal to STATUS_BASE. The user can press the Reset button and
 * restart the debugger. Since the Retention RAM is not reset, the information will be
 * accessible via any Memory window.
 *
 * If SmartSnippets is used and P1[3] is available then the user may use the call
 * to set_pxact_gpio() to have a visual indication of the occurrence of a Hard Fault
 * exception.
 */

void HardFault_HandlerC(unsigned long *hardfault_args)
{

    if (DEVELOPMENT_DEBUG)
    {
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
        if (USE_WDOG)
            wdg_freeze();            // Stop WDOG

        if ((GetWord16(SYS_STAT_REG) & DBG_IS_UP) == DBG_IS_UP)
            __asm("BKPT #0\n");
        else
            while(1);
    }
    else // DEVELOPMENT_DEBUG
    {
        if (PRODUCTION_DEBUG_OUTPUT)
        {
            if (USE_WDOG)
                wdg_reload(WATCHDOG_DEFAULT_PERIOD);                  // Reset WDOG! 200 * 10.24ms active time for UART to finish printing!

            dbg_prod_output(1, hardfault_args);
        }


        // Force execution of NMI Handler
        wdg_reload(1);
        wdg_resume();
        while(1);
    }

}
