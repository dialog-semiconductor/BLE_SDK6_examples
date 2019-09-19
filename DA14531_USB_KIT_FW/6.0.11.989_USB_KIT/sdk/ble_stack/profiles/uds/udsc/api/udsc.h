/**
 ****************************************************************************************
 *
 * @file udsc.h
 *
 * @brief Header file - User Data Service Client Declaration.
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

#ifndef UDSC_H_
#define UDSC_H_

/**
 ****************************************************************************************
 * @addtogroup UDS User Data Service Client
 * @ingroup UDS
 * @brief User Data Service Client
 * @{
 ****************************************************************************************
 */

#if (BLE_UDS_CLIENT)

#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of User Data Service Client task instances
#define UDSC_IDX_MAX        (BLE_CONNECTION_MAX)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Characteristics
enum
{
    UDSC_CHAR_USER_DB_CHANGE_INCR,
    UDSC_CHAR_USER_INDEX,
    UDSC_CHAR_USER_CONTROl_POINT,

    UDSC_CHAR_USER_AEROBIC_HR_LOW_LIM,
    UDSC_CHAR_USER_AEROBIC_THRESHOLD,
    UDSC_CHAR_USER_AGE,
    UDSC_CHAR_USER_ANAEROBIC_HR_LOW_LIM,
    UDSC_CHAR_USER_ANAEROBIC_HR_UP_LIM,
    UDSC_CHAR_USER_ANAEROBIC_THRESHOLD,
    UDSC_CHAR_USER_AEROBIC_HR_UP_LIM,
    UDSC_CHAR_USER_DATE_OF_BIRTH,
    UDSC_CHAR_USER_DATE_OF_THRESHOLD_ASS,
    UDSC_CHAR_USER_EMAIL_ADDR,
    UDSC_CHAR_USER_FAT_BURN_HR_LOW_LIM,
    UDSC_CHAR_USER_FAT_BURN_HR_UP_LIM,
    UDSC_CHAR_USER_FIRST_NAME,
    UDSC_CHAR_USER_5ZONE_HR_LIM,
    UDSC_CHAR_USER_GENDER,
    UDSC_CHAR_USER_HEART_RATE_MAX,
    UDSC_CHAR_USER_HEIGHT,
    UDSC_CHAR_USER_HIP_CIRCUMFERENCE,
    UDSC_CHAR_USER_LAST_NAME,
    UDSC_CHAR_USER_MAX_REC_HEART_RATE,
    UDSC_CHAR_USER_RESTING_HEART_RATE,
    UDSC_CHAR_USER_THRESHOLDS_SPORT_TYPE,
    UDSC_CHAR_USER_3ZONE_HR_LIM,
    UDSC_CHAR_USER_2ZONE_HR_LIM,
    UDSC_CHAR_USER_VO2_MAX,
    UDSC_CHAR_USER_WAIST_CIRCUMFERENCE,
    UDSC_CHAR_USER_WEIGHT,
    UDSC_CHAR_USER_LANGUAGE,

    UDSC_CHAR_MAX,
};

/// Descriptors
enum
{
    UDSC_DESC_DB_CHANGE_INCR_CFG,
    UDSC_DESC_CNTL_POINT_CFG,
    UDSC_DESC_MAX,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

///Structure containing the characteristics handles, value handles and descriptors
struct uds_content
{
    /// Service info
    struct prf_svc svc;
    /// Characteristic info
    struct prf_char_inf chars[UDSC_CHAR_MAX];
    /// Descriptors
    struct prf_char_desc_inf descs[UDSC_DESC_MAX];
};

/// Environment variable for each Connections
struct udsc_cnx_env
{
    /// Last requested UUID(to keep track of the two services and char)
    uint16_t last_uuid_req;
    /// Last Char. for which a read request has been sent
    uint8_t last_char_code;
    /// counter used to check service uniqueness
    uint8_t nb_svc;
    /// Ongoing UCP operation code
    uint8_t op_code;
    /// User data entry operation state
    uint8_t db_op_state;
    /// Value of recently notified or read DB change increment characteristic
    uint32_t db_change_incr;
    /// UDS characteristics
    struct uds_content uds;
};

/// User Data Service environment variable
struct udsc_env_tag
{
    /// Profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct udsc_cnx_env* env[UDSC_IDX_MAX];
    /// State of different task instances
    ke_state_t state[UDSC_IDX_MAX];
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve User Data Service client profile interface
 *
 * @return User Data Service client profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* udsc_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Send enable confirmation to application.
 * @param[in] udsc_env Pointer to UDSC environment variables structure
 * @param[in] conidx Connection index.
 * @param[in] status Status of the enable: either OK or error encountered while discovery.
 ****************************************************************************************
 */
void udsc_enable_rsp_send(struct udsc_env_tag *udsc_env, uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Check if client request is possible or not
 * @param[in] udsc_env Pointer to UDSC environment variables structure
 * @param[in] conidx Connection index.
 * @param[in] char_code Characteristic code.
 * @return PRF_ERR_OK if request can be performed, error code else.
 ****************************************************************************************
 */
uint8_t udsc_validate_request(struct udsc_env_tag *udsc_env, uint8_t conidx,
                              uint8_t char_code);

/**
 ****************************************************************************************
 * @brief Send error indication from profile to Host, with proprietary status codes.
 * @param[in] udsc_env Pointer to UDSC environment variables structure
 * @param[in] conidx Connection index.
 * @param[in] status Status code of error
 * @return void
 ****************************************************************************************
 */
void udsc_error_ind_send(struct udsc_env_tag *udsc_env, uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send indication from profile to Host, with proprietary status codes.
 * @param[in] udsc_env User Data Service client environment variable.
 * @param[in] conidx Connection index.
 * @param[in] status Status code of error.
 ****************************************************************************************
 */
void udsc_cmp_evt_ind_send(struct udsc_env_tag *udsc_env, uint8_t conidx,
                           uint8_t status);

/**
 ****************************************************************************************
 * @brief Send characteristic value indication from profile to Host.
 * @param[in] udsc_env User Data Service client environment variable.
 * @param[in] conidx Connection index.
 * @param[in] status Status code of error.
 * @param[in] char_code Code of the read characteristic.
 * @param[in] length Length of the Characteristic Value.
 * @param[in] value Characteristic value.
 ****************************************************************************************
 */
void udsc_read_char_val_rsp_send(struct udsc_env_tag *udsc_env, uint8_t conidx,
                                 uint8_t status, uint8_t char_code, uint16_t length,
                                 const uint8_t *value);

/**
 ****************************************************************************************
 * @brief Send characteristic value write result from profile to Host.
 * @param[in] udsc_env User Data Service client environment variable.
 * @param[in] conidx Connection index.
 * @param[in] status Status code of error.
 ****************************************************************************************
 */
void udsc_set_char_val_cfm_send(struct udsc_env_tag *udsc_env, uint8_t conidx,
                                uint8_t status);

#endif // (BLE_UDS_CLIENT)

/// @} UDS

#endif // UDSC_H_
