/**
 ****************************************************************************************
 *
 * @file uart_utils.c
 *
 * @brief UART utilities
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

#include <string.h>
#include "uart_utils.h"

/**
 ****************************************************************************************
 * @brief Reverse string s in place
 * @param[in,out] s        String to reverse
 ****************************************************************************************
 */
static void reverse(char s[])
 {
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
 }

/**
 ****************************************************************************************
 * @brief convert n to characters in s
 * @param[in] n            integer to convert
 * @param[in] s            converted character
 ****************************************************************************************
 */
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

void printf_byte(uart_t *uart, uint8_t ch)
{
    // print a Byte in hex format
    char b;
    b = ((0xF0 & ch) >> 4);
    b += (b < 10) ? 48 : 55;

    uart_send(uart, (uint8_t *)&b, 1, UART_OP_BLOCKING);
    b = (0xF & ch);
    b += (b < 10) ? 48 : 55;
    uart_send(uart, (uint8_t *)&b, 1, UART_OP_BLOCKING);
}

void printf_string(uart_t *uart, char *str)
{
    uart_send(uart, (uint8_t *) str, strlen(str), UART_OP_BLOCKING);
}

void print_hword(uart_t *uart, uint16_t aHalfWord)
{
    printf_byte(uart, (aHalfWord >> 8) & 0xFF);
    printf_byte(uart, (aHalfWord) & 0xFF);
}

void print_word(uart_t *uart, uint32_t aWord)
{
    printf_byte(uart, (aWord >> 24) & 0xFF);
    printf_byte(uart, (aWord >> 16) & 0xFF);
    printf_byte(uart, (aWord >> 8) & 0xFF);
    printf_byte(uart, (aWord) & 0xFF);
}

void printf_byte_dec(uart_t *uart, int a)
{
    char temp_buf[4];

    if (a > 255)
    return;

    itoa(a, temp_buf);
    printf_string(uart, temp_buf);
}
