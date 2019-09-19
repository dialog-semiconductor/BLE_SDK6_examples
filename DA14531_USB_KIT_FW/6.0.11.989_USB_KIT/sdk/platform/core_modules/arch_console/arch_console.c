/**
 ****************************************************************************************
 *
 * @file arch_console.c
 *
 * @brief Serial logging interface.
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

#if defined (CFG_PRINTF)

#include <stdlib.h>
#include <string.h>
#include "arch_console.h"
#include "arch_api.h"
#include "ke_mem.h"
#include "uart.h"

#if defined (CFG_PRINTF_UART2) || defined (CFG_UART1_SDK)
    #define USE_UART_SDK    1
#else
    #define USE_UART1_ROM   1
#endif

#if USE_UART_SDK

#if defined (CFG_PRINTF_UART2)
    #define UART        UART2
#elif defined (CFG_UART1_SDK)
    #define UART        UART1
#endif

#if UART_DMA_SUPPORT
    #define UART_OP     UART_OP_DMA
#else
    #define UART_OP     UART_OP_INTR
#endif

#endif // USE_UART_SDK

static printf_msg *printf_msg_list      __SECTION_ZERO("retention_mem_area0");
static bool defer_sending               __SECTION_ZERO("retention_mem_area0");

#define PRINT_SZ 256

static char current_msg_buffer[PRINT_SZ]; // not retained - flushed at end of each main loop iteration
static uint8_t current_msg_offset       __SECTION_ZERO("retention_mem_area0");
static volatile bool uart_busy          __SECTION_ZERO("retention_mem_area0");

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_ARCH_CONSOLE__)

/*
 * List management functions
 */

// pull one from the beginning
static printf_msg *remove_from_list(printf_msg **list)
{
    printf_msg *node = NULL;

    if (*list)
    {
        node = *list;
        *list = node->pNext;
        node->pNext = NULL;
    }

    return node;
}

// push one at the end
static int put_to_list(printf_msg **list, printf_msg *node)
{
    printf_msg *p;

    if (!node)
        return 0;

    if (!(*list))
    {
        *list = node;
    }
    else
    {
        for (p = *list; p->pNext != NULL; p = p->pNext);

        p->pNext = node;
        node->pNext = NULL;
    }

    return 1;
}


/*
 * Message functions
 */
static printf_msg* create_msg(uint32_t sz)
{
    printf_msg *node = (printf_msg *)ke_malloc(sizeof(printf_msg) + sz,
                                               KE_MEM_NON_RETENTION);
    node->pNext = NULL;

    return node;
}

static int delete_msg(printf_msg *node)
{
    if (!node)
        return 0;

    ke_free(node);

    return 1;
}


/*
 * String manipulation functions
 */
static uint32_t arch_strlen(const char *s)
{
    unsigned int length = 0;

    while (s[length] != '\0')
        length++;

    return length;
}


static uint32_t arch_itoa(int32_t value, uint32_t radix, uint32_t uppercase,
                          char *buf, int32_t pad)
{
    char *pbuf = buf;
    bool negative = false;
    uint32_t i, length;

    // Check for unusual radixes
    if (radix > 16)
        return 0;

    if (value < 0 && radix == 10) {
        negative = true;
        value = -value;
    }

    // Build the string here starting from the end...
    do {
        int digit = value % radix;
        *(pbuf++) = (digit < 10 ? '0' + digit : (uppercase ? 'A' : 'a') + digit - 10);
        value /= radix;
    } while (value > 0);

    length = (pbuf - buf);
    for (i = length; i < pad; i++)
        *(pbuf++) = '0';

    // Add '-' sign if need be...
    if (negative)
        *(pbuf++) = '-';

    // Mark EOF
    *(pbuf) = '\0';

    // Reverse the string now...
    length = (pbuf - buf);
    for (i = 0; i < length / 2; i++)
    {
        char ch = buf[i];

        buf[i] = buf[length - i - 1];
        buf[length - i - 1] = ch;
    }

    return length;
}


static inline int _putc(char **wr_ptr, char *buf, uint32_t length, char ch)
{
    if ((uint32_t)((*wr_ptr - buf) + 1) >= length)
        return 0;

    **wr_ptr = ch;
    (*wr_ptr)++;
    **wr_ptr = '\0';

    return 1;
}


static inline int _puts(char **wr_ptr, char *buf, uint32_t length, char *s, uint32_t str_len)
{
    uint32_t i;

    if (length - (*wr_ptr - buf) - 1 < str_len)
        str_len = length - (*wr_ptr - buf) - 1;

    /* Copy to buffer */
    for (i = 0; i < str_len; i++)
    {
        **wr_ptr = s[i];
        (*wr_ptr)++;
    }
    **wr_ptr = '\0';

    return length;
}


static int32_t arch_vsnprintf(char *buffer, uint32_t buffer_len, const char *fmt, va_list va)
{
    char *pbuffer = buffer;
    char bf[24];
    char ch;
    char qual = '\0';

    ch = *(fmt++);
    do {
        if ((unsigned int)((pbuffer - buffer) + 1) >= buffer_len)
            break;

        if (ch != '%')
        {
            _putc(&pbuffer, buffer, buffer_len, ch);
        }
        else
        {
            char zero_pad = 0;
            char *ptr;
            unsigned int len;

            ch =* (fmt++);

            /* Zero padding requested */
            if (ch == '0')
            {
                ch =* (fmt++);
                if (ch == '\0')
                    goto end;
                if (ch >= '0' && ch <= '9')
                    zero_pad = ch - '0';
                ch =* (fmt++);
            }

            if(ch == 'l' || ch == 'h')
            {
               qual = ch;
                ch =* (fmt++);
            }

            switch (ch) {
            case 0:
                goto end;

            case 'i':
            case 'd':
            {
                long val;

                if (qual == 'h')
                {
                    val = va_arg(va, int/*short*/);
                }
                else if (qual == 'l')
                {
                    val = va_arg(va, long);
                }
                else
                {
                    val = va_arg(va, int);
                }

                len = arch_itoa(val, 10, 0, bf, zero_pad);
                _puts(&pbuffer, buffer, buffer_len, bf, len);
                break;
            }
            case 'u':
            case 'x':
            case 'X':
            {
                unsigned long val;

                if(qual == 'h')
                {
                    val = va_arg(va, unsigned int/*unsigned short*/);
                }
                else if(qual == 'l')
                {
                    val = va_arg(va, unsigned long);
                }
                else
                {
                    val = va_arg(va, unsigned int);
                }

                if (ch == 'u')
                    len = arch_itoa(val, 10, 0, bf, zero_pad);
                else
                    len = arch_itoa(val, 16, (ch=='X'), bf, zero_pad);

                _puts(&pbuffer, buffer, buffer_len, bf, len);
                break;
            }

            case 'c' :
                ch = va_arg(va, int32_t);
                _putc(&pbuffer, buffer, buffer_len, ch);
                break;

            case 's' :
                ptr = va_arg(va, char*);
                _puts(&pbuffer, buffer, buffer_len, ptr, arch_strlen(ptr));
                break;

            default:
                _puts(&pbuffer, buffer, buffer_len, "FATAL: unsupported printf character: ", 37);
                _putc(&pbuffer, buffer, buffer_len, ch);
                _puts(&pbuffer, buffer, buffer_len, ".\n", 2);
                break;
            }
        }
        ch = *(fmt++);
    } while(ch);
end:
    return pbuffer - buffer;
}


static int32_t arch_snprintf(char* buffer, uint32_t buffer_len, const char *fmt, va_list va)
{
    int ret;

    ret = arch_vsnprintf(buffer, buffer_len, fmt, va);

    return ret;
}

/* Note: App should not modify the sleep mode until all messages have been printed out */
static void uart_callback(uint8_t res)
{
    printf_msg* msg = remove_from_list(&printf_msg_list);
    delete_msg(msg);

    uart_busy = false;

#if USE_UART1_ROM
    uart_finish_transfers();
#else
    uart_wait_tx_finish(UART);
#endif

    if (printf_msg_list)
    {
        char *p = printf_msg_list->pBuf;

        uart_busy = true;

#if USE_UART1_ROM
        uart_write((uint8_t *)p, arch_strlen(p), uart_callback);
#else
        uart_send(UART,(uint8_t *)p, arch_strlen(p), UART_OP);
#endif
    }
    else
    {
        arch_restore_sleep_mode();
    }
}

void arch_printf_flush(void)
{
    printf_msg *msg = create_msg(current_msg_offset + 1);
    memcpy(msg->pBuf, current_msg_buffer, current_msg_offset);
    msg->pBuf[current_msg_offset] = '\0';

    // Critical section
    GLOBAL_INT_DISABLE();

    // if printf_msg_list is NULL we can send the msg immediately (in the synchrouns part of the loop), else
    // the callback function will handle it
    if (printf_msg_list == NULL)
        defer_sending = true;

    if (!put_to_list(&printf_msg_list, msg))
    {
        delete_msg(msg);
        if (printf_msg_list == NULL)
            defer_sending = false;
    }
    else
    {
        //send to UART only if XTAL16 is used else it crashes: defer_sending will trigger the procedure!
    }
    // End of critical section
    GLOBAL_INT_RESTORE();

    current_msg_offset = 0;
}

int arch_printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    int res = arch_vprintf(fmt, args);
    va_end(args);

    return res;
}

int arch_vprintf(const char *fmt, va_list args)
{
    char my_buf[PRINT_SZ];

    int32_t written = arch_snprintf(my_buf, sizeof(my_buf), fmt, args);

    if ((written > 0))
    {
        // If not enough space for appending to current message - send it and create new one
        if ((current_msg_offset + written + 1) > PRINT_SZ)
        {
            arch_printf_flush();
        }

        // Only copy the message and defer sending - arch_printf_process() will send it
        memcpy(current_msg_buffer + current_msg_offset, my_buf, written);
        current_msg_offset += written;
    }

    return 1;
}

void arch_puts(const char *s)
{
    arch_printf("%s", s);
}

void arch_printf_process(void)
{
    // Submit pending message
    if (current_msg_offset)
    {
        arch_printf_flush();
    }
    if (defer_sending)
    {
        arch_force_active_mode();

        if (!uart_busy)
        {
#if USE_UART1_ROM
            uart_write((uint8_t *)printf_msg_list->pBuf, arch_strlen(printf_msg_list->pBuf), uart_callback);
#else
            uart_register_tx_cb(UART, (uart_cb_t) uart_callback);
            uart_send(UART,(uint8_t *) printf_msg_list->pBuf, arch_strlen(printf_msg_list->pBuf), UART_OP);
#endif
            defer_sending = false;
        }
    }
}

#else

static void uart_write_adapt(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t))
{
#if USE_UART1_ROM
    uart_write(bufptr, size, callback);
#else
    uart_register_tx_cb(UART, (uart_cb_t) callback);
    uart_send(UART, bufptr, size, UART_OP);
#endif
}

static void uart_finish_transfers_adapt(void)
{
#if USE_UART1_ROM
    uart_finish_transfers();
#else
    uart_wait_tx_finish(UART);
#endif
}

const rom_console_cfg_t rom_console_cfg =
{
    .current_msg_buffer          = current_msg_buffer,
    .current_msg_offset          = &current_msg_offset,
    .uart_busy                   = &uart_busy,
    .printf_msg_list             = &printf_msg_list,
    .defer_sending               = &defer_sending,
    .uart_write                  = uart_write_adapt,
    .uart_finish                 = uart_finish_transfers_adapt,
    .arch_restore_sleep_mode     = arch_restore_sleep_mode,
    .arch_force_active_mode      = arch_force_active_mode,
};

#endif

#endif // CFG_PRINTF
