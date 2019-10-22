/**
 ****************************************************************************************
 *
 * @file arch_console.h
 *
 * @brief Serial logging interface header file.
 *
 * Copyright (c) 2013-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef _ARCH_CONSOLE_H_
#define _ARCH_CONSOLE_H_

#if defined (CFG_PRINTF)

#include <stdarg.h>

typedef struct __print_msg {
    struct __print_msg *pNext;
    char pBuf[__ARRAY_EMPTY];
} printf_msg;

#if defined (__DA14531__) && !defined (__EXCLUDE_ROM_ARCH_CONSOLE__)
typedef struct
{
    char *current_msg_buffer;
    uint8_t *current_msg_offset;
    volatile bool *uart_busy;
    printf_msg **printf_msg_list;
    bool *defer_sending;
    void (*uart_write)(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));
    void (*uart_finish)();
    void (*arch_restore_sleep_mode)();
    void (*arch_force_active_mode)();
} rom_console_cfg_t;
#endif

void arch_puts(const char *s);
int arch_vprintf(const char *fmt, va_list args);
int arch_printf(const char *fmt, ...);
void arch_printf_flush(void);
void arch_printf_process(void);

#else // CFG_PRINTF

#define arch_puts(s) {}
#define arch_vprintf(fmt, args) {}
#define arch_printf(fmt, args...) {}
#define arch_printf_process() {}
#define arch_printf_flush() {}

#endif // CFG_PRINTF

#endif // _ARCH_CONSOLE_H_
