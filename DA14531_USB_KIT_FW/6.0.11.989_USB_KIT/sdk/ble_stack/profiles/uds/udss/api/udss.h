/**
 ****************************************************************************************
 *
 * @file udss.h
 *
 * @brief Header file - User Data Service Server Declaration.
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

#ifndef UDSS_H_
#define UDSS_H_

/**
 ****************************************************************************************
 * @addtogroup UDSS User Data Service
 * @ingroup UDSS
 * @brief User Data Service
 * @{
 ****************************************************************************************
 */

#if (BLE_UDS_SERVER)

#include "prf_types.h"
#include "prf_utils.h"
#include "uds_common.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of User Data Service Server task instances
#define UDS_IDX_MAX         (1)
/// Maximal length for Long Characteristic values - 255
#define UDS_VAL_MAX_LEN     (0xFF)
/// Maximum length of notified UCP value
#define UDS_UCP_NTF_MAX_LEN (0x04)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// User Data Service Attribute Table Indices
enum
{
    /// User Data Service
    UDS_IDX_SVC,
    
    // Optional characteristics:
    /// First Name characteristic
    UDS_IDX_USER_FIRST_NAME_CHAR,
    UDS_IDX_USER_FIRST_NAME_VAL,
    /// Last Name characteristic
    UDS_IDX_USER_LAST_NAME_CHAR,
    UDS_IDX_USER_LAST_NAME_VAL,
    /// Email Address characteristic
    UDS_IDX_USER_EMAIL_ADDRESS_CHAR,
    UDS_IDX_USER_EMAIL_ADDRESS_VAL,
    /// Age characteristic
    UDS_IDX_USER_AGE_CHAR,
    UDS_IDX_USER_AGE_VAL,
    /// Date of Birth characteristic
    UDS_IDX_USER_DATE_OF_BIRTH_CHAR,
    UDS_IDX_USER_DATE_OF_BIRTH_VAL,
    /// Gender characteristic
    UDS_IDX_USER_GENDER_CHAR,
    UDS_IDX_USER_GENDER_VAL,
    /// Weight characteristic
    UDS_IDX_USER_WEIGHT_CHAR,
    UDS_IDX_USER_WEIGHT_VAL,
    /// Height characteristic
    UDS_IDX_USER_HEIGHT_CHAR,
    UDS_IDX_USER_HEIGHT_VAL,
    /// VO2 Max (Maximal Oxygen Uptake) characteristic
    UDS_IDX_USER_VO2_MAX_CHAR,
    UDS_IDX_USER_VO2_MAX_VAL,
    /// Maximum Heart Rate characteristic
    UDS_IDX_USER_HEART_RATE_MAX_CHAR,
    UDS_IDX_USER_HEART_RATE_MAX_VAL,
    /// Resting heart Rate characteristic
    UDS_IDX_USER_RESTING_HEART_RATE_CHAR,
    UDS_IDX_USER_RESTING_HEART_RATE_VAL,
    /// Maximum Recommended Heart characteristic
    UDS_IDX_USER_MAX_RECOMMENDED_HEART_RATE_CHAR,
    UDS_IDX_USER_MAX_RECOMMENDED_HEART_RATE_VAL,
    /// Aerobic Threshold characteristic
    UDS_IDX_USER_AEROBIC_THRESHOLD_CHAR,
    UDS_IDX_USER_AEROBIC_THRESHOLD_VAL,
    /// Anaerobic Threshold characteristic
    UDS_IDX_USER_ANAEROBIC_THRESHOLD_CHAR,
    UDS_IDX_USER_ANAEROBIC_THRESHOLD_VAL,
    /// Sport Type for Aerobic and Anaerobic Thresholds characteristic
    UDS_IDX_USER_SPORT_TYPE_CHAR,
    UDS_IDX_USER_SPORT_TYPE_VAL,
    /// Date of Threshold Assessment characteristic
    UDS_IDX_USER_DATE_OF_THRESHOLD_ASSESSMENT_CHAR,
    UDS_IDX_USER_DATE_OF_THRESHOLD_ASSESSMENT_VAL,
    /// Waist Circumference characteristic
    UDS_IDX_USER_WAIST_CIRCUMFERENCE_CHAR,
    UDS_IDX_USER_WAIST_CIRCUMFERENCE_VAL,
    /// Hip Circumference characteristic
    UDS_IDX_USER_HIP_CIRCUMFERENCE_CHAR,
    UDS_IDX_USER_HIP_CIRCUMFERENCE_VAL,
    /// Fat Burn Heart Rate Lower Limit characteristic
    UDS_IDX_USER_FAT_BURN_HEART_RATE_LOW_LIMIT_CHAR,
    UDS_IDX_USER_FAT_BURN_HEART_RATE_LOW_LIMIT_VAL,
    /// Fat Burn Heart Rate Upper Limit characteristic
    UDS_IDX_USER_FAT_BURN_HEART_RATE_UP_LIMIT_CHAR,
    UDS_IDX_USER_FAT_BURN_HEART_RATE_UP_LIMIT_VAL,
    /// Aerobic Heart Rate Lower Limit characteristic
    UDS_IDX_USER_AEROBIC_HEART_RATE_LOW_LIMIT_CHAR,
    UDS_IDX_USER_AEROBIC_HEART_RATE_LOW_LIMIT_VAL,
    /// Aerobic Heart Rate Upper Limit characteristic
    UDS_IDX_USER_AEROBIC_HEART_RATE_UP_LIMIT_CHAR,
    UDS_IDX_USER_AEROBIC_HEART_RATE_UP_LIMIT_VAL,
    /// Anaerobic Heart Rate Lower Limit characteristic
    UDS_IDX_USER_ANAEROBIC_HEART_RATE_LOW_LIMIT_CHAR,
    UDS_IDX_USER_ANAEROBIC_HEART_RATE_LOW_LIMIT_VAL,
    /// Anaerobic Heart Rate Upper Limit characteristic
    UDS_IDX_USER_ANAEROBIC_HEART_RATE_UP_LIMIT_CHAR,
    UDS_IDX_USER_ANAEROBIC_HEART_RATE_UP_LIMIT_VAL,
    /// Five Zone Heart Rate Limits characteristic
    UDS_IDX_USER_FIVE_ZONE_HEART_RATE_LIMITS_CHAR,
    UDS_IDX_USER_FIVE_ZONE_HEART_RATE_LIMITS_VAL,
    /// Three Zone Heart Rate Limits characteristic
    UDS_IDX_USER_THREE_ZONE_HEART_RATE_LIMITS_CHAR,
    UDS_IDX_USER_THREE_ZONE_HEART_RATE_LIMITS_VAL,
    /// Two Zone Heart Rate Limit characteristic
    UDS_IDX_USER_TWO_ZONE_HEART_RATE_LIMIT_CHAR,
    UDS_IDX_USER_TWO_ZONE_HEART_RATE_LIMIT_VAL,
    /// Language characteristic
    UDS_IDX_USER_LANGUAGE_CHAR,
    UDS_IDX_USER_LANGUAGE_VAL,

    // Mandatory characteristics:
    /// Database Change Increment characteristic
    UDS_IDX_USER_DB_CHANGE_INCR_CHAR,
    UDS_IDX_USER_DB_CHANGE_INCR_VAL,
    UDS_IDX_USER_DB_CHANGE_INCR_CFG,
    /// User Index characteristic
    UDS_IDX_USER_INDEX_CHAR,
    UDS_IDX_USER_INDEX_VAL,
    /// User Control Point characteristic
    UDS_IDX_USER_CNTL_POINT_CHAR,
    UDS_IDX_USER_CNTL_POINT_VAL,
    UDS_IDX_USER_CNTL_POINT_CFG,

    UDS_IDX_NB,
};

/// Characteristics Codes for exchanging characteristic values with Host.
enum uds_user_char_code
{
    UDS_USER_FIRST_NAME_CHAR_CODE                       = UDS_IDX_USER_FIRST_NAME_VAL,
    UDS_USER_LAST_NAME_CHAR_CODE                        = UDS_IDX_USER_LAST_NAME_VAL,
    UDS_USER_EMAIL_ADDRESS_CHAR_CODE                    = UDS_IDX_USER_EMAIL_ADDRESS_VAL,
    UDS_USER_AGE_CHAR_CODE                              = UDS_IDX_USER_AGE_VAL,
    UDS_USER_DATE_OF_BIRTH_CHAR_CODE                    = UDS_IDX_USER_DATE_OF_BIRTH_VAL,
    UDS_USER_GENDER_CHAR_CODE                           = UDS_IDX_USER_GENDER_VAL,
    UDS_USER_WEIGHT_CHAR_CODE                           = UDS_IDX_USER_WEIGHT_VAL,
    UDS_USER_HEIGHT_CHAR_CODE                           = UDS_IDX_USER_HEIGHT_VAL,
    UDS_USER_VO2_MAX_CHAR_CODE                          = UDS_IDX_USER_VO2_MAX_VAL,
    UDS_USER_HEART_RATE_MAX_CHAR_CODE                   = UDS_IDX_USER_HEART_RATE_MAX_VAL,
    UDS_USER_RESTING_HEART_RATE_CHAR_CODE               = UDS_IDX_USER_RESTING_HEART_RATE_VAL,
    UDS_USER_MAX_RECOMMENDED_HEART_RATE_CHAR_CODE       = UDS_IDX_USER_MAX_RECOMMENDED_HEART_RATE_VAL,
    UDS_USER_AEROBIC_THRESHOLD_CHAR_CODE                = UDS_IDX_USER_AEROBIC_THRESHOLD_VAL,
    UDS_USER_ANAEROBIC_THRESHOLD_CHAR_CODE              = UDS_IDX_USER_ANAEROBIC_THRESHOLD_VAL,
    UDS_USER_SPORT_TYPE_CHAR_CODE                       = UDS_IDX_USER_SPORT_TYPE_VAL,
    UDS_USER_DATE_OF_THRESHOLD_ASSESSMENT_CHAR_CODE     = UDS_IDX_USER_DATE_OF_THRESHOLD_ASSESSMENT_VAL,
    UDS_USER_WAIST_CIRCUMFERENCE_CHAR_CODE              = UDS_IDX_USER_WAIST_CIRCUMFERENCE_VAL,
    UDS_USER_HIP_CIRCUMFERENCE_CHAR_CODE                = UDS_IDX_USER_HIP_CIRCUMFERENCE_VAL,
    UDS_USER_FAT_BURN_HEART_RATE_LOW_LIMIT_CHAR_CODE    = UDS_IDX_USER_FAT_BURN_HEART_RATE_LOW_LIMIT_VAL,
    UDS_USER_FAT_BURN_HEART_RATE_UP_LIMIT_CHAR_CODE     = UDS_IDX_USER_FAT_BURN_HEART_RATE_UP_LIMIT_VAL,
    UDS_USER_AEROBIC_HEART_RATE_LOW_LIMIT_CHAR_CODE     = UDS_IDX_USER_AEROBIC_HEART_RATE_LOW_LIMIT_VAL,
    UDS_USER_AEROBIC_HEART_RATE_UP_LIMIT_CHAR_CODE      = UDS_IDX_USER_AEROBIC_HEART_RATE_UP_LIMIT_VAL,
    UDS_USER_ANAEROBIC_HEART_RATE_LOW_LIMIT_CHAR_CODE   = UDS_IDX_USER_ANAEROBIC_HEART_RATE_LOW_LIMIT_VAL,
    UDS_USER_ANAEROBIC_HEART_RATE_UP_LIMIT_CHAR_CODE    = UDS_IDX_USER_ANAEROBIC_HEART_RATE_UP_LIMIT_VAL,
    UDS_USER_FIVE_ZONE_HEART_RATE_LIMITS_CHAR_CODE      = UDS_IDX_USER_FIVE_ZONE_HEART_RATE_LIMITS_VAL,
    UDS_USER_THREE_ZONE_HEART_RATE_LIMITS_CHAR_CODE     = UDS_IDX_USER_THREE_ZONE_HEART_RATE_LIMITS_VAL,
    UDS_USER_TWO_ZONE_HEART_RATE_LIMIT_CHAR_CODE        = UDS_IDX_USER_TWO_ZONE_HEART_RATE_LIMIT_VAL,
    UDS_USER_LANGUAGE_CHAR_CODE                         = UDS_IDX_USER_LANGUAGE_VAL,
    UDS_USER_DB_CHANGE_INCR_CHAR_CODE                   = UDS_IDX_USER_DB_CHANGE_INCR_VAL,
    UDS_USER_DB_CHANGE_INCR_CFG_CODE                    = UDS_IDX_USER_DB_CHANGE_INCR_CFG,
    UDS_USER_INDEX_CHAR_CODE                            = UDS_IDX_USER_INDEX_VAL,
    UDS_USER_CNTL_POINT_CHAR_CODE                       = UDS_IDX_USER_CNTL_POINT_VAL,
    UDS_USER_CNTL_POINT_CFG_CODE                        = UDS_IDX_USER_CNTL_POINT_CFG,

    UDS_USER_CHAR_MAX,
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// User Data Service environment variable
struct udss_env_tag
{
    /// Profile environment
    prf_env_t prf_env;
    /// Database configuration
    uint32_t enabled_chars;
    /// UDS Start Handle
    uint16_t shdl;
    /// If consent procedure was successfull for all connected peer device
    bool consent[BLE_CONNECTION_MAX];
    /// Database Change Characteristic Indication/Notification configurations
    uint16_t db_change_ccc_val[BLE_CONNECTION_MAX];
    /// User Control Point Characteristic Indication/Notification configurations
    uint16_t ucp_ccc_val[BLE_CONNECTION_MAX];
    /// State of different task instances
    ke_state_t state[UDS_IDX_MAX];
    /// Attribute table
    uint8_t att_tbl[UDS_IDX_NB];
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/// Full UDS Database Description
extern const struct attm_desc uds_att_db[UDS_IDX_NB];

/// User Data Service
extern const att_svc_desc_t uds_svc;

/// User Data Service Example Characteristic
extern const struct att_char_desc uds_example_char;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve User Data Service profile interface
 *
 * @return User Data Service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* udss_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Unpack User Control request
 * @param[in] packed_val User Control Point value packed
 * @param[in] length Packed User Control Point value length
 * @param[out] ucp_req User Control Point Request value
 * @return status of unpacking
 ****************************************************************************************
 */
uint8_t udss_unpack_ucp_req(const uint8_t *packed_val, uint16_t length,
                            struct uds_ucp_req* ucp_req);


/**
 ****************************************************************************************
 * @brief Pack User Control response
 * @param[out] packed_val User Control Point value packed
 * @param[in] ucp_rsp User Control Response value
 * @return size of packed data
 ****************************************************************************************
 */
uint8_t udss_pack_ucp_rsp(uint8_t *packed_val, const struct uds_ucp_rsp* ucp_rsp);

/**
 ****************************************************************************************
 * @brief Send User Control response
 * @param[in] ucp_rsp User Control Response value
 * @param[in] conidx  Connection index
 * @return void
 ****************************************************************************************
 */
void udss_send_ucp_rsp(const struct uds_ucp_rsp *ucp_rsp, uint8_t conidx);

#endif // (BLE_UDS_SERVER)

/// @} UDSS

#endif // UDSS_H_
