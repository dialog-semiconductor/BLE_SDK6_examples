/**
 ****************************************************************************************
 *
 * @file uart_utils.h
 *
 * @brief UART utilities header file.
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

#ifndef _UART_UTILS_H_
#define _UART_UTILS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "uart.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief print a Byte in hex format
 * @param[in] uart_id           Identifies which UART to use
 * @param[in] ch                Character to print
 ****************************************************************************************
 */
void printf_byte(uart_t *uart, uint8_t ch);

/**
 ****************************************************************************************
 * @brief UART print string function
 * @param[in] uart_id           Identifies which UART to use
 * @param[in] ch                String to print
 ****************************************************************************************
 */
void printf_string(uart_t *uart, char *str);

/**
 ****************************************************************************************
 * @brief Prints a (16-bit) half-word in hex format using printf_byte
 * @param[in] uart_id           Identifies which UART to use
 * @param[in] aHalfWord         16-bit half-word to print
 ****************************************************************************************
 */
void print_hword(uart_t *uart, uint16_t aHalfWord);

 /**
 ****************************************************************************************
 * @brief Prints a (32-bit) word in hex format using printf_byte
 * @param[in] uart_id           Identifies which UART to use
 * @param[in] aWord             32-bit half-word to print
 ****************************************************************************************
 */
void print_word(uart_t *uart, uint32_t aWord);

 /**
 ****************************************************************************************
 * @brief prints a byte in dec format
 * @param[in] uart_id           Identifies which UART to use
 * @param[in] a                 Byte to print
 ****************************************************************************************
 */
void printf_byte_dec(uart_t *uart, int a);

#endif //_UART_PRINT_H_
