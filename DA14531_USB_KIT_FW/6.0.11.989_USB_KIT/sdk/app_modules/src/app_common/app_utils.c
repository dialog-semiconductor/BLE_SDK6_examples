/**
 ****************************************************************************************
 *
 * @file app_utils.c
 *
 * @brief Application generic utilities helper functions source file.
 *
 * Copyright (c) 2017-2018 Dialog Semiconductor. All rights reserved.
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

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_UTILS__)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_utils.h"
#include "co_bt.h"
#include "co_math.h"
#include "gap.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum app_addr_types app_get_address_type(uint8_t addr_type, struct bd_addr addr)
{
    if (addr_type == ADDR_PUBLIC)
    {
        return APP_PUBLIC_ADDR_TYPE;
    }
    if ((addr_type == ADDR_RAND) && ((addr.addr[BD_ADDR_LEN-1] & 0xC0) == GAP_STATIC_ADDR))
    {
        return APP_RANDOM_STATIC_ADDR_TYPE;
    }
    if ((addr_type == ADDR_RAND) && ((addr.addr[BD_ADDR_LEN-1] & 0xC0) == GAP_RSLV_ADDR))
    {
        return APP_RANDOM_PRIVATE_RESOLV_ADDR_TYPE;
    }
    if ((addr_type == ADDR_RAND) && ((addr.addr[BD_ADDR_LEN-1] & 0xC0) == GAP_NON_RSLV_ADDR))
    {
        return APP_RANDOM_PRIVATE_NONRESOLV_ADDR_TYPE;
    }
    if ((addr_type == ADDR_RPA_PUBLIC) || (addr_type == ADDR_RPA_RAND))
    {
        return APP_ID_ADDR_TYPE;
    }
    return APP_ERROR_ADDR_TYPE;
}

void app_fill_random_byte_array(uint8_t *dst, uint8_t key_size, uint8_t array_size)
{
    uint32_t rand_val;
    uint8_t rand_bytes_cnt = 0;
    uint8_t remaining_bytes;
    uint8_t i;

    // key_size must not be greater than array_size
    ASSERT_ERROR(array_size >= key_size);

    // key_size = M*4+N
    // Randomize the M most significant bytes of the array
    for (i = 0; i < key_size-3; i+=4)
    {
        // Calculate a random 32 bit value
        rand_val = co_rand_word();
        // Assign each of the 4 rand bytes to the byte array
        dst[array_size - (i+0)-1] = (rand_val >> 24) & 0xFF;
        dst[array_size - (i+1)-1] = (rand_val >> 16) & 0xFF;
        dst[array_size - (i+2)-1] = (rand_val >> 8) & 0xFF;
        dst[array_size - (i+3)-1] = (rand_val >> 0) & 0xFF;
        // Count randomized bytes
        rand_bytes_cnt += 4;
    }

    // Randomize the remaining N most significant bytes of the array. (Max N = 3)
    remaining_bytes = key_size - rand_bytes_cnt;
    if (remaining_bytes)
    {
        rand_val = co_rand_word();
        for (i = 0; i < remaining_bytes; i++)
        {
            dst[array_size -(rand_bytes_cnt+i)-1] = (rand_val >> ((3-i)<<3)) & 0xFF;
        }
    }

    // Fill the least significant bytes of the array with zeroes
    remaining_bytes = array_size - key_size;
    for (i = 0; i < remaining_bytes; i++)
    {
        dst[array_size - (key_size + i)-1] = 0;
    }
}

#endif

/// @} APP
