/**
 ****************************************************************************************
 *
 * @file app_customs_common.c
 * 
 * @brief Application Custom Service profile common file.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include "rwble_config.h"              // SW configuration
#if (BLE_CUSTOM_SERVER)
#include <stdint.h>
#include "app_customs.h"
#include "app_prf_types.h"
#include "prf_types.h"
#include "attm_db.h"
#include "attm_db_128.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if (BLE_CUSTOM1_SERVER)
uint8_t app_custs1_val_wr_validate(uint16_t att_idx, uint16_t offset, uint16_t len, uint8_t *value)
{
    return ATT_ERR_NO_ERROR;
}
#endif // (BLE_CUSTOM1_SERVER)

#if (BLE_CUSTOM2_SERVER)
uint8_t app_custs2_val_wr_validate(uint16_t att_idx, uint16_t offset, uint16_t len, uint8_t *value)
{
    return ATT_ERR_NO_ERROR;
}
#endif // (BLE_CUSTOM2_SERVER)

#endif // (BLE_CUSTOM_SERVER)
