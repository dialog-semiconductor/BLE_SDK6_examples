/**
 ****************************************************************************************
 *
 * @file suotar.h
 *
 * @brief Header file - SUOTAR.
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

#ifndef SUOTAR_H_
#define SUOTAR_H_

/**
 ****************************************************************************************
 * @addtogroup SUOTAR SUOTA Receiver
 * @ingroup SUOTAR
 * @brief SUOTA Profile Reciver.
 *
 * The SUOTAR is responsible for providing SUOTA reporter functionalities to
 * upper layer module or application. The device using this profile takes the role
 * of a SUOTA Receiver role.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_SUOTA_RECEIVER)

#include "ke_task.h"
#include "atts.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "gattc.h"

#define SUOTA_VERSION_1_0       10
#define SUOTA_VERSION_1_3       13

#ifndef SUOTA_VERSION
#define SUOTA_VERSION       SUOTA_VERSION_1_3
#endif

/*
 * DEFINES
 ****************************************************************************************
 */
#define SUOTAR_IDX_MAX          (1)

#if SUOTA_VERSION >= SUOTA_VERSION_1_3
#define SUOTA_PD_CHAR_SIZE      244
#else
#define SUOTA_PD_CHAR_SIZE      20
#endif

#define SUOTA_OVERALL_PD_SIZE   0x200


/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// SUOTA Handles offsets
enum
{
    SUOTA_IDX_SVC,

    SUOTA_IDX_PATCH_MEM_DEV_CHAR,
    SUOTA_IDX_PATCH_MEM_DEV_VAL,
//    SUOTA_IDX_PATCH_MEM_DEV_VAL_DESCR,

    SUOTA_IDX_GPIO_MAP_CHAR,
    SUOTA_IDX_GPIO_MAP_VAL,
//    SUOTA_IDX_GPIO_MAP_VAL_DESCR,

    SUOTA_IDX_PATCH_MEM_INFO_CHAR,
    SUOTA_IDX_PATCH_MEM_INFO_VAL,
//    SUOTA_IDX_PATCH_MEM_INFO_VAL_DESCR,

    SUOTA_IDX_PATCH_LEN_CHAR,
    SUOTA_IDX_PATCH_LEN_VAL,
//    SUOTA_IDX_PATCH_LEN_VAL_DESCR,

    SUOTA_IDX_PATCH_DATA_CHAR,
    SUOTA_IDX_PATCH_DATA_VAL,
//    SUOTA_IDX_PATCH_DATA_VAL_DESCR,

    SUOTA_IDX_PATCH_STATUS_CHAR,
    SUOTA_IDX_PATCH_STATUS_VAL,
    SUOTA_IDX_PATCH_STATUS_NTF_CFG,
//    SUOTA_IDX_PATCH_STATUS_VAL_DESCR,

#if SUOTA_VERSION >= SUOTA_VERSION_1_3
    SUOTA_IDX_VERSION_CHAR,
    SUOTA_IDX_VERSION_VAL,

    SUOTA_IDX_PATCH_DATA_CHAR_SIZE_CHAR,
    SUOTA_IDX_PATCH_DATA_CHAR_SIZE_VAL,

    SUOTA_IDX_MTU_CHAR,
    SUOTA_IDX_MTU_VAL,
#endif

    SUOTA_IDX_NB,
};



///Characteristics Code for Write Indications
enum
{
    SUOTAR_ERR_CHAR,
    SUOTAR_PATCH_MEM_DEV_CHAR,
    SUOTAR_GPIO_MAP_CHAR,
    SUOTAR_PATCH_LEN_CHAR,
    SUOTAR_PATCH_DATA_CHAR,
    SUOTA_PATCH_STATUS_NTF_CFG,
};

///Alert Level Values, to blink LED when SUOTA is active
enum
{
    SUOTAR_ALERT_NONE,
    SUOTAR_ALERT_MILD,
    SUOTAR_ALERT_HIGH,
};

/// SUOTA Status values
enum
{
    SUOTAR_RESERVED         = 0x00,     // Value zero must not be used !! Notifications are sent when status changes.
    SUOTAR_SRV_STARTED      = 0x01,     // Valid memory device has been configured by initiator. No sleep state while in this mode
    SUOTAR_CMP_OK           = 0x02,     // SUOTA process completed successfully.
    SUOTAR_SRV_EXIT         = 0x03,     // Forced exit of SUOTAR service.
    SUOTAR_CRC_ERR          = 0x04,     // Overall Patch Data CRC failed
    SUOTAR_PATCH_LEN_ERR    = 0x05,     // Received patch Length not equal to PATCH_LEN characteristic value
    SUOTAR_EXT_MEM_WRITE_ERR= 0x06,     // External Mem Error (Writing to external device failed)
    SUOTAR_INT_MEM_ERR      = 0x07,     // Internal Mem Error (not enough space for Patch)
    SUOTAR_INVAL_MEM_TYPE   = 0x08,     // Invalid memory device
    SUOTAR_APP_ERROR        = 0x09,     // Application error

    // SUOTAR application specific error codes
    SUOTAR_IMG_STARTED      = 0x10,     // SUOTA started for downloading image (SUOTA application)
    SUOTAR_INVAL_IMG_BANK   = 0x11,     // Invalid image bank
    SUOTAR_INVAL_IMG_HDR    = 0x12,     // Invalid image header
    SUOTAR_INVAL_IMG_SIZE   = 0x13,     // Invalid image size
    SUOTAR_INVAL_PRODUCT_HDR= 0x14,     // Invalid product header
    SUOTAR_SAME_IMG_ERR     = 0x15,     // Same Image Error
    SUOTAR_EXT_MEM_READ_ERR = 0x16,     // Failed to read from external memory device
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// SUOTA receiver environment variable
struct suotar_env_tag
{
    /// Profile environment
    prf_env_t prf_env;

    /// Indicate if ext mem is supported or not
    uint8_t features;

    /// SUOTA Start Handle
    uint16_t suota_shdl;

    /// State of different task instances
    ke_state_t state[SUOTAR_IDX_MAX];

    /// CCC value for patch status value
    uint16_t ntf_cfg;
};

/*
 * SUOTA PROFILE ATTRIBUTES DECLARATION
 ****************************************************************************************
 */

/// SUOTA Database Description
extern const struct attm_desc suotar_att_db[SUOTA_IDX_NB];


/*
 *  SUOTA PROFILE ATTRIBUTES VALUES DECLARATION
 ****************************************************************************************
 */

extern const uint8_t suotar_service[];

/// SUOTA Service - Patch Memory Device Characteristic
extern struct att_char128_desc suotar_patch_mem_dev_char;
extern const struct att_uuid_128 suotar_patch_mem_dev_val;

/// SUOTA Service - GPIO mapping Characteristic
extern struct att_char128_desc suotar_gpio_map_char;
extern const struct att_uuid_128 suotar_gpio_map_val;

/// SUOTA Service - Patch Memory Device Information Characteristic
extern struct att_char128_desc suotar_patch_mem_info_char;
extern const struct att_uuid_128 suotar_patch_mem_info_val;

/// SUOTA Service - Patch Length Characteristic
extern struct att_char128_desc suotar_patch_len_char;
extern const struct att_uuid_128 suotar_patch_len_val;

/// SUOTA Service - Patch Data Characteristic
extern struct att_char128_desc suotar_patch_data_char;
extern const struct att_uuid_128 suotar_patch_data_val;

/// SUOTA Service - Patch Status Characteristic
extern struct att_char128_desc suotar_patch_status_char;
extern const struct att_uuid_128 suotar_patch_status_val;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
****************************************************************************************
* @brief Retrieve Proximity service profile interface
*
* @return Proximity service profile interface
****************************************************************************************
*/
const struct prf_task_cbs* suotar_prf_itf_get(void);

#endif //BLE_SUOTA_RECEIVER

/// @} SUOTAR

#endif // SUOTAR_H_
