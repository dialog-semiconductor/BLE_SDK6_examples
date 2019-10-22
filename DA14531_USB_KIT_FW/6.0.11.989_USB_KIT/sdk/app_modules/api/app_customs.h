/**
 ****************************************************************************************
 *
 * @file app_customs.h
 *
 * @brief Custom Service Application entry point.
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef _APP_CUSTOMS_H_
#define _APP_CUSTOMS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include "user_profiles_config.h"

#if (BLE_CUSTOM_SERVER)

#include <stdint.h>
#include "prf_types.h"
#include "attm_db_128.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Custom Service Application.
 * @return void
 ****************************************************************************************
 */
void app_custs1_init(void);

/**
 ****************************************************************************************
 * @brief Add a Custom Service instance to the DB.
 * @return void
 ****************************************************************************************
 */
void app_custs1_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable the Custom Service.
 * @param[in] conhdl    Connection handle
 * @return void
 ****************************************************************************************
 */
void app_custs1_enable(uint16_t conhdl);

/**
 ****************************************************************************************
 * @brief It is called to validate the characteristic value before it is written
 *        to profile database.
 * @param[in] att_idx Attribute index to be validated before it is written to database.
 * @param[in] offset  Offset at which the data has to be written (used only for values
 *                    that are greater than MTU).
 * @param[in] len     Data length to be written.
 * @param[in] value   Pointer to data to be written to attribute database.
 * @return validation status (high layer error code)
 ****************************************************************************************
 */
uint8_t app_custs1_val_wr_validate(uint16_t att_idx, uint16_t offset, uint16_t len, uint8_t *value);

/**
 ****************************************************************************************
 * @brief Initialize Custom Service Application.
 * @return void
 ****************************************************************************************
 */
void app_custs2_init(void);

/**
 ****************************************************************************************
 * @brief Add a Custom Service instance to the DB.
 * @return void
 ****************************************************************************************
 */
void app_custs2_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable the Custom Service.
 * @param[in] conhdl    Connection handle
 * @return void
 ****************************************************************************************
 */
void app_custs2_enable(uint16_t conhdl);

/**
 ****************************************************************************************
 * @brief It is called to validate the characteristic value before it is written
 *        to profile database.
 * @param[in] att_idx Attribute index to be validated before it is written to database.
 * @param[in] offset  Offset at which the data has to be written (used only for values
 *                    that are greater than MTU).
 * @param[in] len     Data length to be written.
 * @param[in] value   Pointer to data to be written to attribute database.
 * @return validation status (high layer error code)
 ****************************************************************************************
 */
uint8_t app_custs2_val_wr_validate(uint16_t att_idx, uint16_t offset, uint16_t len, uint8_t *value);

/**
 ****************************************************************************************
 * @brief Function returns user callback structure for custom tasks.
 * @param[in] taks_id   Task id TASK_ID_CUSTS1 or TASK_ID_CUSTS2
 * @return status       user callback structure or NULL if not present
 ****************************************************************************************
 */
const struct cust_prf_func_callbacks *custs_get_func_callbacks(enum KE_API_ID task_id);

#endif // (BLE_CUSTOM_SERVER)

#endif // _APP_CUSTOMS_H_
