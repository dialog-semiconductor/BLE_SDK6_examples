/**
 ****************************************************************************************
 *
 * @file app_prf_types.h
 *
 * @brief app - profiles related header file.
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

#ifndef APP_PRF_TYPES_H_
#define APP_PRF_TYPES_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

 /// Format of a profile initialization function
typedef void (*prf_func_void_t)(void);
typedef void (*prf_func_uint8_t)(uint8_t);
typedef uint8_t (*prf_func_validate_t)(uint16_t, uint16_t, uint16_t, uint8_t*);

/// Structure of profile call back function table.
struct prf_func_callbacks
{
    /// Profile Task ID.
    enum KE_API_ID       task_id;
    /// Pointer to the database create function
    prf_func_void_t         db_create_func;
    /// Pointer to the profile enable function
    prf_func_uint8_t       enable_func;
};

/// Structure of custom profile call back function table.
struct cust_prf_func_callbacks
{
    /// Profile Task ID.
    enum KE_API_ID             task_id;
    /// pointer to the custom database table defined by user
    const struct attm_desc_128 *att_db;
    /// max number of attributes in custom database
    const uint8_t              max_nb_att;
    /// Pointer to the custom database create function defined by user
    prf_func_void_t            db_create_func;
    /// Pointer to the custom profile enable function defined by user
    prf_func_uint8_t           enable_func;
    /// Pointer to the custom profile initialization function
    prf_func_void_t            init_func;
    /// Pointer to the validation function defined by user
    prf_func_validate_t        value_wr_validation_func;
};

/// @} APP

#endif //APP_PRF_TYPES_H_
