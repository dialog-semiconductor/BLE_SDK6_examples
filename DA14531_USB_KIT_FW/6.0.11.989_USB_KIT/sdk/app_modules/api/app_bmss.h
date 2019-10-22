/**
 ****************************************************************************************
 *
 * @file app_bmss.h
 *
 * @brief Bond Management Service Server Application header file
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

#ifndef APP_BMSS_H_
#define APP_BMSS_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_profiles_config.h"

#if BLE_BMS_SERVER

#include "bms_common.h"

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

struct app_bmss_cb
{
    void (*on_bmss_del_bond_req_ind)(uint8_t con_idx,
                                        const struct bms_cntl_point_op *operation);
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes BMSS Apllication.
 *
 * @return      void
 ****************************************************************************************
 */
void app_bmss_init(void);

/**
 ****************************************************************************************
 * @brief Creates BMSS service database.
 *
 * @param[in]   void
 *
 * @return      void
 ****************************************************************************************
 */
void app_bmss_create_db(void);

/**
 ****************************************************************************************
 * @brief Sends bond operation status to Bond Management Service.
 * @param[in] status          Operation status
 * @return void
 ****************************************************************************************
 */
void app_bmss_del_bond_cfm(uint8_t conidx, uint8_t status);

#endif //BLE_BMS_SERVER

/// @} APP

#endif // APP_BMSS_H_
