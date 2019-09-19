/**
 ****************************************************************************************
 *
 * @file attm_db_128.h
 *
 * @brief Header file of service database of 128bits long UUID.
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

#ifndef __ATTM_DB_128_H
#define __ATTM_DB_128_H

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "ke_task.h"
#include "att.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Attribute description (used to create database)
struct attm_desc_128
{
    /// Element UUID
    const uint8_t *uuid;
    /// UUID size
    uint8_t uuid_size;
    /// Attribute Permissions (@see enum attm_perm_mask)
    att_perm_type perm;
    /// Attribute Max Size (@see enum attm_value_perm_mask)
    /// note: for characteristic declaration contains handle offset
    /// note: for included service, contains target service handle
    att_size_t max_length;

    /// Current length of the element
    att_size_t length;
    /// Element value array
    uint8_t *value;
};

 /// Service Value Descriptor - 128-bit
typedef uint8_t att_svc_desc128_t[ATT_UUID_128_LEN];

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles creation of service and corresponding attributes with 128-bit UUIDs
 *        within the database.
 * @param[in]  svc_idx         service index number
 * @param[out] shdl            base handle of the database (start handle)
 * @param[in]  cfg_flag        configuration flag
 * @param[in]  max_nb_att      max number of database attributes
 * @param[out] att_tbl         if not NULL, it returns the attribute handles
 * @param[in]  dest_id         task ID of the created task
 * @param[in]  att_db          pointer to the database description table
 * @return status error code
 ****************************************************************************************
 */
uint8_t attm_svc_create_db_128(uint8_t svc_idx,
                               uint16_t *shdl,
                               uint8_t *cfg_flag,
                               uint8_t max_nb_att,
                               uint8_t *att_tbl,
                               ke_task_id_t const dest_id,
                               const struct attm_desc_128 *att_db,
                               uint8_t svc_perm);

#endif // __ATTM_DB_128_H
