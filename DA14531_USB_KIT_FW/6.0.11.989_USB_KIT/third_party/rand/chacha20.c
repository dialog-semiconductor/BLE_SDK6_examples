/**
 ****************************************************************************************
 *
 * @file chacha20.c
 *
 * @brief chacha20 random generator implementation.
 *
 * CSPRNG based on http://cr.yp.to/chacha/chacha-20080128.pdf
 * Uses 16-byte key
 * Optimized for size
 *
 * Source code downloaded from https://gist.github.com/Emill/d8e8df7269f75b9485a2
 * Author  (Github user name): Emill
 *
 ****************************************************************************************
 */

#if (USE_CHACHA20_RAND)

/**
 ****************************************************************************************
 * @addtogroup chacha20
 * @{
 ****************************************************************************************
 */

#include <stdint.h>
#include <string.h>
#include "chacha20.h"

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

static uint32_t chacha_key[4]               __SECTION_ZERO("retention_mem_area0");
static uint64_t chacha_counter              __SECTION_ZERO("retention_mem_area0");
static uint32_t chacha_random_output[16]    __SECTION_ZERO("retention_mem_area0");
static uint8_t chacha_random_output_left    __SECTION_ZERO("retention_mem_area0");

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_CHACHA20__)
// This is ASCII of "expand 16-byte k"
static const uint32_t chacha_constants[4] = {0x61707865, 0x3120646e, 0x79622d36, 0x6b206574};

static const uint8_t chacha_order[8][4] = {
    {0, 4, 8, 12},
    {1, 5, 9, 13},
    {2, 6, 10, 14},
    {3, 7, 11, 15},
    {0, 5, 10, 15},
    {1, 6, 11, 12},
    {2, 7, 8, 13},
    {3, 4, 9, 14}
};

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static void chacha_quarterround(const uint8_t indices[4]) {
    uint32_t* s = chacha_random_output;
    int a = indices[0];
    int b = indices[1];
    int c = indices[2];
    int d = indices[3];
    s[a] += s[b]; s[d] ^= s[a]; s[d] = (s[d] << 16) | (s[d] >> 16);
    s[c] += s[d]; s[b] ^= s[c]; s[b] = (s[b] << 12) | (s[b] >> 20);
    s[a] += s[b]; s[d] ^= s[a]; s[d] = (s[d] << 8) | (s[d] >> 24);
    s[c] += s[d]; s[b] ^= s[c]; s[b] = (s[b] << 7) | (s[b] >> 25);
}

static void chacha_run(void) {
    uint32_t state[16];
    memcpy(state, chacha_constants, sizeof(chacha_constants));
    memcpy(state + 4, chacha_key, sizeof(chacha_key));
    memcpy(state + 8, chacha_key, sizeof(chacha_key));
    memset(state + 12, 0, 2 * sizeof(uint32_t));
    ++chacha_counter;
    memcpy(state + 14, &chacha_counter, sizeof(chacha_counter));

    memcpy(chacha_random_output, state, sizeof(state));

    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 8; j++) {
            chacha_quarterround(chacha_order[j]);
        }
    }

    for(int i=0; i<16; i++) {
        chacha_random_output[i] += state[i];
    }
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void csprng_seed(const uint8_t key[16]) {
    memcpy(chacha_key, key, sizeof(chacha_key));
    chacha_counter = 0;
    chacha_random_output_left = 0;
}

uint32_t csprng_get_next_uint32(void) {
    if (chacha_random_output_left == 0) {
        chacha_run();
        chacha_random_output_left = 16;
    }
    return chacha_random_output[--chacha_random_output_left];
}

#else

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

const rom_chacha20_cfg_t rom_chacha20_cfg =
{
    .key                = chacha_key,
    .counter            = &chacha_counter,
    .random_output      = chacha_random_output,
    .random_output_left = &chacha_random_output_left,
};

#endif

/// @} chacha20
#endif
