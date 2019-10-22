/**
 ****************************************************************************************
 *
 * @file suotar_task.h
 *
 * @brief Header file - SUOTARTASK.
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

#ifndef SUOTAR_TASK_H_
#define SUOTAR_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup SUOTARTASK SUOTA Receiver Task
 * @ingroup SUOTAR
 * @brief SUOTA Reciver Task
 *
 * The SUOTARTASK is responsible for handling the API messages from the Application or internal
 * tasks.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_SUOTA_RECEIVER)

#include <stdint.h>
#include "ke_task.h"
#include "suotar.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Possible states of the SUOTAR task
enum
{
    /// Idle state
    SUOTAR_IDLE,
    /// SUOTA in a busy state
    SUOTAR_BUSY,

    /// Number of defined states.
    SUOTAR_STATE_MAX,
};


/// Messages for SUOTA Receiver
enum suotar_msg_id
{
    // Patch Memory Device type indication
    SUOTAR_PATCH_MEM_DEV_IND = KE_FIRST_MSG(TASK_ID_SUOTAR),

    // GPIO map type indication
    SUOTAR_GPIO_MAP_IND,

    /// Patch Data length Indication
    SUOTAR_PATCH_LEN_IND,

    /// New Patch Data Indication
    SUOTAR_PATCH_DATA_IND,    

    // Request to update memory info in the db
    SUOTAR_PATCH_MEM_INFO_UPDATE_REQ,

    // Request to update status info in the db
    SUOTAR_STATUS_UPDATE_REQ,

    /// Error Indication
    SUOTAR_ERROR_IND,
};


/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

struct suotar_db_cfg
{
    /// Indicate if ext mem is supported or not
    uint8_t features;
};

/// Parameters of the @ref SUOTAR_PATCH_DATA_IND message
struct suotar_patch_data_ind
{
    /// Connection handle
    uint16_t conhdl;

    /// Char Code - Indicate whether patch data written successfully
    uint8_t char_code;

    uint8_t len;

    uint8_t pd[__ARRAY_EMPTY];
};

/// Parameters of the @ref SUOTAR_PATCH_MEM_DEV_IND message
struct suotar_patch_mem_dev_ind
{
    /// Connection handle
    uint16_t conhdl;

    /// Char Code - Indicate whether patch data written successfully
    uint8_t char_code;

    uint32_t mem_dev;
};

/// Parameters of the @ref SUOTAR_GPIO_MAP_IND message
struct suotar_gpio_map_ind
{
    /// Connection handle
    uint16_t conhdl;

    /// Char Code - Indicate whether patch data written successfully
    uint8_t char_code;

    uint32_t gpio_map;
};

/// Parameters of the @ref SUOTAR_PATCH_LEN_IND message
struct suotar_patch_len_ind
{
    /// Connection handle
    uint16_t conhdl;

    /// Char Code - Indicate whether patch data written successfully
    uint8_t char_code;

    uint16_t len;
};

/// Parameters of the @ref SUOTAR_STATUS_UPDATE_REQ message
struct suotar_status_update_req
{
    /// Connection handle
    uint16_t conhdl;

    /// SUOTAR Status
    uint8_t status;
};

/// Parameters of the @ref SUOTAR_PATCH_MEM_INFO_UPDATE_REQ message
struct suotar_patch_mem_info_update_req
{
    /// Memory info: 16MSbits show number of patches, 16LSbits overall mem len
    uint32_t mem_info;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler suotar_default_handler;

#endif //BLE_SUOTA_RECEIVER

/// @} SUOTARTASK

#endif // SUOTAR_TASK_H_
