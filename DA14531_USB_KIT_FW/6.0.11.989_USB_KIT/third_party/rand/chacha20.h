/**
 ****************************************************************************************
 *
 * @file chacha20.h
 *
 * @brief chacha20 random generator implementation API.
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

#ifndef _CHACHA20_H_
#define _CHACHA20_H_

#if (USE_CHACHA20_RAND)

#if defined (__DA14531__) && !defined (__EXCLUDE_ROM_CHACHA20__)
typedef struct
{
    uint32_t *key;
    uint64_t *counter;
    uint32_t *random_output;
    uint8_t *random_output_left;
} rom_chacha20_cfg_t;
#endif

/**
 ****************************************************************************************
 * @brief Initialize chacha20 seed
 * @param[in] key initialization value
 * @return none
 ****************************************************************************************
 */
void csprng_seed(const uint8_t key[16]);

/**
 ****************************************************************************************
 * @brief Get chacha20 random 32-bit number
 * @return Random number
 ****************************************************************************************
 */
uint32_t csprng_get_next_uint32(void);

#endif
#endif
