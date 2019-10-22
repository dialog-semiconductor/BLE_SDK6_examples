/**
 ****************************************************************************************
 *
 * @file app_security.c
 *
 * @brief Application Security Entry Point
 *
 * Copyright (C) RivieraWaves 2009-2013
 * Copyright (C) 2017 Modified by Dialog Semiconductor
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP_SECURITY
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_APP_SEC)

#include "co_bt.h"
#include "co_math.h"
#include "app_api.h"            // Application task Definition
#include "app_security.h"       // Application Security API Definition

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Application Security Environment Structure
struct app_sec_bond_data_env_tag app_sec_env[APP_EASY_MAX_ACTIVE_CONNECTION] __SECTION_ZERO("retention_mem_area0"); // @RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

uint32_t app_sec_gen_tk(void)
{
    // Generate a PIN Code (Between 100000 and 999999)
    return (100000 + (co_rand_word()%900000));
}

void app_sec_gen_ltk(uint8_t conidx, uint8_t key_size)
{
    app_sec_env[conidx].ltk.key_size = key_size;

    // Randomly generate the LTK and the Random Number
    app_fill_random_byte_array(app_sec_env[conidx].ltk.randnb.nb, RAND_NB_LEN, RAND_NB_LEN);

    // Randomly generate the end of the LTK
    app_fill_random_byte_array(app_sec_env[conidx].ltk.ltk.key, key_size, KEY_LEN);

    // Randomly generate the EDIV
    app_sec_env[conidx].ltk.ediv = co_rand_word()%65536;

    app_sec_env[conidx].valid_keys |= LTK_PRESENT;
}

void app_sec_gen_csrk(uint8_t conidx)
{
    // Randomly generate the CSRK
    app_fill_random_byte_array(app_sec_env[conidx].lcsrk.key, KEY_LEN, KEY_LEN);
    app_sec_env[conidx].valid_keys |= LCSRK_PRESENT;
}

#endif // (BLE_APP_SEC)

/// @} APP_SECURITY
