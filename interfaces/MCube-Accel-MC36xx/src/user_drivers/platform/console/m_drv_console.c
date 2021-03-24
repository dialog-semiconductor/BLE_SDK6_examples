/******************************************************************************
 *
 * Copyright (c) 2018 mCube, Inc.  All rights reserved.
 *
 * This source is subject to the mCube Software License.
 * This software is protected by Copyright and the information and source code
 * contained herein is confidential. The software including the source code
 * may not be copied and the information contained herein may not be used or
 * disclosed except with the written permission of mCube Inc.
 *
 * All other rights reserved.
 *****************************************************************************/

/**
 * @file    m_drv_console.c
 * @author  mCube
 * @date    19 June 2018
 * @brief   Console interface for mCube drivers.
 * @see     http://www.mcubemems.com
 */

/*******************************************************************************
 *** INCLUDE FILES
 *******************************************************************************/
#include <stdarg.h>
/** Please include interface driver from platform SDK */
#include <stdint.h>
#include <string.h>
#include "gpio.h"
#include "uart.h"
#include "arch_console.h"

/** mCube functions include */
#include "m_drv_console.h"

/******************************************************************************
 *** FUNCTION
 *****************************************************************************/
void mcube_printf(const char *str, ...)
{
    /** Function hook by customer. */
    uart2_write((uint8_t *)str, strlen(str), NULL); // send next string character
    uart2_finish_transfers();
}

/* reverse:  reverse string s in place */
void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* itoa:  convert n to characters in s */
void itoa(short n, char s[])
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

void printf_dec(short a)
{
    // print in decimal format
    char temp_buf[10];

    itoa(a, temp_buf);
}

