/**
 ****************************************************************************************
 *
 * @file nvds.c
 *
 * @brief Non Volatile Data Storage (NVDS) driver
 *
 * Copyright (C) RivieraWaves 2009-2013
 * Copyright (C) 2017-2019 Modified by Dialog Semiconductor
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup NVDS
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <string.h>      // string definitions
#include <stddef.h>      // standard definitions
#include "nvds.h"        // nvds definitions
#include "arch.h"        // main
#include "co_math.h"     // math operations
#include "co_bt.h"
#include "co_utils.h"
#include "rwip_config.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct nvds_data_struct
{
    uint16_t    lpclk_drift;
    uint8_t     bd_address[6];
    uint16_t    ble_ca_timer_dur;
    uint8_t     ble_cra_timer_dur;
    uint8_t     ble_ca_min_rssi;
    uint8_t     ble_ca_nb_pkt;
    uint8_t     ble_ca_nb_bad_pkt;
};

/*
 * LOCAL VARIABLES
 ****************************************************************************************
 */

static const struct nvds_data_struct nvds_data_storage =
{
    .lpclk_drift                   = CFG_NVDS_TAG_LPCLK_DRIFT,
    .bd_address                    = CFG_NVDS_TAG_BD_ADDRESS,
    /// Default Channel Assessment Timer duration (20s - Multiple of 10ms)
    .ble_ca_timer_dur              = CFG_NVDS_TAG_BLE_CA_TIMER_DUR,
    /// Default Channel Reassessment Timer duration (Multiple of Channel Assessment Timer duration)
    .ble_cra_timer_dur             = CFG_NVDS_TAG_BLE_CRA_TIMER_DUR,
    /// Default minimum RSSI Threshold - -48dBm
    .ble_ca_min_rssi               = CFG_NVDS_TAG_BLE_CA_MIN_RSSI,
    /// Default number of packets to receive for statistics
    .ble_ca_nb_pkt                 = CFG_NVDS_TAG_BLE_CA_NB_PKT,
    /// Default number of bad packets needed to remove a channel
    .ble_ca_nb_bad_pkt             = CFG_NVDS_TAG_BLE_CA_NB_BAD_PKT,
};

#if defined (__DA14531__)
static const uint8_t blank_otp_bdaddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_NVDS__)

uint8_t nvds_get_func(uint8_t tag, nvds_tag_len_t *lengthPtr, uint8_t *buf)
{
    extern struct bd_addr dev_bdaddr;
    uint8_t status = NVDS_FAIL;

    switch (tag)
    {
        case NVDS_TAG_BD_ADDRESS:
        {
#if defined (__DA14531__)
            //check if dev_bdaddr is not blank (ones)
            if(memcmp(&dev_bdaddr, &blank_otp_bdaddr, NVDS_LEN_BD_ADDRESS))
#else
            //check if dev_bdaddr is not blank (zeros)
            if(memcmp(&dev_bdaddr, &co_null_bdaddr, NVDS_LEN_BD_ADDRESS))
#endif
            {
                memcpy(buf, &dev_bdaddr, NVDS_LEN_BD_ADDRESS);
                *lengthPtr = NVDS_LEN_BD_ADDRESS;
                status = NVDS_OK;
            }
            else
            {
                memcpy(buf, nvds_data_storage.bd_address, NVDS_LEN_BD_ADDRESS);
                *lengthPtr = NVDS_LEN_BD_ADDRESS;
                status = NVDS_OK;
            }
        } break;

        case NVDS_TAG_LPCLK_DRIFT:
        {
            if (*lengthPtr < NVDS_LEN_LPCLK_DRIFT)
            {
                *lengthPtr = 0;
                status = NVDS_LENGTH_OUT_OF_RANGE;
            }
            else
            {
                memcpy(buf, &nvds_data_storage.lpclk_drift, NVDS_LEN_LPCLK_DRIFT);
                *lengthPtr = NVDS_LEN_LPCLK_DRIFT;
                status = NVDS_OK;
            }
        } break;

        case NVDS_TAG_BLE_CA_TIMER_DUR:
        {
            if (*lengthPtr < NVDS_LEN_BLE_CA_TIMER_DUR)
            {
                *lengthPtr = 0;
                status = NVDS_LENGTH_OUT_OF_RANGE;
            }
            else
            {
                memcpy(buf, &nvds_data_storage.ble_ca_timer_dur, NVDS_LEN_BLE_CA_TIMER_DUR);
                *lengthPtr = NVDS_LEN_BLE_CA_TIMER_DUR;
                status = NVDS_OK;
            }
        } break;

        case NVDS_TAG_BLE_CRA_TIMER_DUR:
        {
            if (*lengthPtr < NVDS_LEN_BLE_CRA_TIMER_DUR)
            {
                *lengthPtr = 0;
                status = NVDS_LENGTH_OUT_OF_RANGE;
            }
            else
            {
                memcpy(buf, &nvds_data_storage.ble_cra_timer_dur, NVDS_LEN_BLE_CRA_TIMER_DUR);
                *lengthPtr = NVDS_LEN_BLE_CRA_TIMER_DUR;
                status = NVDS_OK;
            }
        } break;

        case NVDS_TAG_BLE_CA_MIN_RSSI:
        {
            if (*lengthPtr < NVDS_LEN_BLE_CA_MIN_RSSI)
            {
                *lengthPtr = 0;
                status = NVDS_LENGTH_OUT_OF_RANGE;
            }
            else
            {
                memcpy(buf, &nvds_data_storage.ble_ca_min_rssi, NVDS_LEN_BLE_CA_MIN_RSSI);
                *lengthPtr = NVDS_LEN_BLE_CA_MIN_RSSI;
                status = NVDS_OK;
            }
        } break;

        case NVDS_TAG_BLE_CA_NB_PKT:
        {
            if (*lengthPtr < NVDS_LEN_BLE_CA_NB_PKT)
            {
                *lengthPtr = 0;
                status = NVDS_LENGTH_OUT_OF_RANGE;
            }
            else
            {
                memcpy(buf, &nvds_data_storage.ble_ca_nb_pkt, NVDS_LEN_BLE_CA_NB_PKT);
                *lengthPtr = NVDS_LEN_BLE_CA_NB_PKT;
                status = NVDS_OK;
            }
        } break;


        case NVDS_TAG_BLE_CA_NB_BAD_PKT:
        {
            if (*lengthPtr < NVDS_LEN_BLE_CA_NB_BAD_PKT)
            {
                *lengthPtr = 0;
                status = NVDS_LENGTH_OUT_OF_RANGE;
            }
            else
            {
                memcpy(buf, &nvds_data_storage.ble_ca_nb_bad_pkt, NVDS_LEN_BLE_CA_NB_BAD_PKT);
                *lengthPtr = NVDS_LEN_BLE_CA_NB_BAD_PKT;
                status = NVDS_OK;
            }
        } break;

    } // switch

    return status;
}

#if (NVDS_READ_WRITE == 1)
uint8_t nvds_init_func(uint8_t *base, uint32_t len)
{
    return NVDS_OK;
}

/// NVDS API implementation - required by ROM function table
uint8_t nvds_del_func(uint8_t tag)
{
    return NVDS_FAIL;
}

/// NVDS API implementation - required by ROM function table
uint8_t nvds_put_func(uint8_t tag, nvds_tag_len_t length, uint8_t *buf)
{
    return NVDS_FAIL;
}
#endif //(NVDS_READ_WRITE == 1)

#else

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern struct bd_addr dev_bdaddr;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

const rom_nvds_cfg_t rom_nvds_cfg =
{
    .nvds_data_storage = &nvds_data_storage,
    .dev_bdaddr        = &dev_bdaddr,
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

uint8_t patched_nvds_get_func(uint8_t tag, nvds_tag_len_t *lengthPtr, uint8_t *buf)
{
    if (tag == NVDS_TAG_BD_ADDRESS)
    {
            //check if dev_bdaddr is not blank (ones)
            if(memcmp(&dev_bdaddr, &blank_otp_bdaddr, NVDS_LEN_BD_ADDRESS))
            {
                memcpy(buf, &dev_bdaddr, NVDS_LEN_BD_ADDRESS);
                *lengthPtr = NVDS_LEN_BD_ADDRESS;
                return NVDS_OK;
            }
            else
            {
                memcpy(buf, nvds_data_storage.bd_address, NVDS_LEN_BD_ADDRESS);
                *lengthPtr = NVDS_LEN_BD_ADDRESS;
                return NVDS_OK;
            }
    }
    return nvds_get_func(tag, lengthPtr, buf);
}

#endif

/// @} NVDS
