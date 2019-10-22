/**
 ****************************************************************************************
 *
 * @file app_udss.h
 *
 * @brief User Data Service Server Application header file
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

#ifndef APP_UDSS_H_
#define APP_UDSS_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief UDS Server Application header file.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_profiles_config.h"

#if BLE_UDS_SERVER

#include "udss.h"
#include "uds_common.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Supported User Data Characteristics Defaults
#ifndef APP_UDS_ENABLED_UDS_CHARS
#define APP_UDS_ENABLED_UDS_CHARS (UDS_CHAR_FLAG_FIRST_NAME | UDS_CHAR_FLAG_LAST_NAME | UDS_CHAR_FLAG_GENDER)
#endif

/// If Host is able to update characteristic values - enables DB Change Increment CCC descriptor
#ifndef APP_UDS_LOCAL_CHAR_UPDATE_ENABLED
#define APP_UDS_LOCAL_CHAR_UPDATE_ENABLED (false)
#else
#undef APP_UDS_LOCAL_CHAR_UPDATE_ENABLED
#define APP_UDS_LOCAL_CHAR_UPDATE_ENABLED (true)
#endif

/*
 * VARIABLES DECLARATION
 ****************************************************************************************
 */

struct app_udss_cb
{
    void (*on_udss_cntl_point_write_ind)(uint8_t conidx, const struct uds_ucp_req *req);
    void (*on_udss_char_val_req)(uint8_t conidx, uint8_t char_code);
    void (*on_udss_char_val_ind)(uint8_t conidx, uint8_t char_code, uint8_t val_len, const uint8_t *val);
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes UDSS Apllication.
 *
 * @return      void
 ****************************************************************************************
 */
void app_udss_init(void);

/**
 ****************************************************************************************
 * @brief Creates UDSS service database.
 *
 * @param[in]   void
 *
 * @return      void
 ****************************************************************************************
 */
void app_udss_create_db(void);

/**
 ****************************************************************************************
 * @brief Sends control point operation status to User Data Service Task.
 * @param[in] conidx        Connection index
 * @param[in] rsp           Control Point operation response
 * @return void
 ****************************************************************************************
 */
void app_udss_cntl_point_operation_cfm(uint8_t conidx, const struct uds_ucp_rsp *rsp);

/**
 ****************************************************************************************
 * @brief Sends characteristic value read response to User Data Service Task.
 * @param[in] conidx        Connection index
 * @param[in] char_code     Characteristic code
 * @param[in] length        Value length
 * @param[in] value         Value pointer
 * @return void
 ****************************************************************************************
 */
void app_udss_char_val_rsp(uint8_t conidx, uint8_t char_code, uint8_t length,
                           const uint8_t *value);

/**
 ****************************************************************************************
 * @brief Sends set characteristic value confirmation to User Data Service Task.
 * @param[in] conidx        Connection index
 * @param[in] char_code     Characteristic code
 * @param[in] status        Status code
 * @return void
 ****************************************************************************************
 */
void app_udss_set_char_val_cfm(uint8_t conidx, uint8_t char_code, uint8_t status);

/**
 ****************************************************************************************
 * @brief Notify Service Task that User Data Service Database has been updated.
 * @param[in] conidx                Connection index
 * @param[in] db_change_incr_val    Database change counter
 * @return void
 ****************************************************************************************
 */
void app_udss_db_updated_notify(uint8_t conidx, uint32_t db_change_incr_val);

#endif // BLE_UDS_SERVER

/// @} APP

#endif // APP_UDSS_H_
