/**
 ****************************************************************************************
 *
 * @file uds_common.h
 *
 * @brief Header File - User Data Service common types.
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

#ifndef UDS_COMMON_H_
#define UDS_COMMON_H_

/**
 ****************************************************************************************
 * @addtogroup UDS User Data Service
 * @ingroup UDS
 * @brief User Data Service
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_UDS_CLIENT || BLE_UDS_SERVER)

#include "prf_types.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// User control point packet max length
#define UDS_USER_CTRL_PT_MAX_LEN    (20)

/// User consent code minimum and maximum values
#define UDS_CONSENT_CODE_MIN_VAL    (0x0000)
#define UDS_CONSENT_CODE_MAX_VAL    (0x270F)

/// No consent procedure was performed
#define UDS_ERROR_DATA_ACCESS_NOT_PERMITTED (0x80)
/// Improperly configured
#define UDS_ERROR_IMPROPERLY_CONFIGURED     (0xFD)
/// Procedure in progress
#define UDS_ERROR_PROC_IN_PROGRESS          (0xFE)

/// User control point op codes
enum uds_ucp_op_code
{
    /// No opcode specified - reserved value
    UDS_NO_OP             = 0x00,
    /// Register new user
    UDS_REQ_REG_NEW_USER  = 0x01,
    /// Consent
    UDS_REQ_CONSENT       = 0x02,
    /// Delete current user
    UDS_REQ_DEL_USER_DATA = 0x03,
    /// Response code
    UDS_REQ_RSP_CODE      = 0x20,
};

/// User control point response value
enum uds_ucp_rsp_val
{
    /// Success
    UDS_RSP_SUCCESS           = 0x01,
    /// Operation code not supported
    UDS_RSP_OP_CODE_NOT_SUP   = 0x02,
    /// Invalid parameter
    UDS_RSP_INVALID_PARAMETER = 0x03,
    /// Operation failed
    UDS_RSP_OPERATION_FAILED  = 0x04,
    /// User not authorized
    UDS_RSP_USER_NOT_AUTH     = 0x05,
};

/// UDS database field flags
enum uds_char_flag
{
    /// First Name supported
    UDS_CHAR_FLAG_FIRST_NAME                    = 0x00000001,
    /// Last name supported
    UDS_CHAR_FLAG_LAST_NAME                     = 0x00000002,
    /// Email Address supported
    UDS_CHAR_FLAG_EMAIL_ADDRESS                 = 0x00000004,
    /// Age supported
    UDS_CHAR_FLAG_AGE                           = 0x00000008,
    /// Date of Birth supported
    UDS_CHAR_FLAG_DATE_OF_BIRTH                 = 0x00000010,
    /// Gender supported
    UDS_CHAR_FLAG_GENDER                        = 0x00000020,
    /// Weight supported
    UDS_CHAR_FLAG_WEIGHT                        = 0x00000040,
    /// Height supported
    UDS_CHAR_FLAG_HEIGHT                        = 0x00000080,
    /// VO2 Max (Maximal Oxygen Uptake) supported
    UDS_CHAR_FLAG_VO2_MAX                       = 0x00000100,
    /// Maximum Heart Rate supported
    UDS_CHAR_FLAG_HEART_RATE_MAX                = 0x00000200,
    /// Resting heart Rate supported
    UDS_CHAR_FLAG_RESTING_HEART_RATE            = 0x00000400,
    /// Maximum Recommended Heart Rate
    UDS_CHAR_FLAG_MAX_RECOMMENDED_HEART_RATE    = 0x00000800,
    /// Aerobic Threshold supported
    UDS_CHAR_FLAG_AEROBIC_THRESHOLD             = 0x00001000,
    /// Anaerobic Threshold supported
    UDS_CHAR_FLAG_ANAEROBIC_THRESHOLD           = 0x00002000,
    /// Sport Type for Aerobic and Anaerobic Thresholds supported
    UDS_CHAR_FLAG_SPORT_TYPE                    = 0x00004000,
    /// Date of Threshold Assessment supported
    UDS_CHAR_FLAG_DATE_OF_THRESHOLD_ASSESSMENT  = 0x00008000,
    /// Waist Circumference supported
    UDS_CHAR_FLAG_WAIST_CIRCUMFERENCE           = 0x00010000,
    /// Hip Circumference supported
    UDS_CHAR_FLAG_HIP_CIRCUMFERENCE             = 0x00020000,
    /// Fat Burn Heart Rate Lower Limit supported
    UDS_CHAR_FLAG_FAT_BURN_HEART_RATE_LOW_LIMIT = 0x00040000,
    /// Fat Burn Heart Rate Upper Limit supported
    UDS_CHAR_FLAG_FAT_BURN_HEART_RATE_UP_LIMIT  = 0x00080000,
    /// Aerobic Heart Rate Lower Limit supported
    UDS_CHAR_FLAG_AEROBIC_HEART_RATE_LOW_LIMIT  = 0x00100000,
    /// Aerobic Heart Rate Upper Limit supported
    UDS_CHAR_FLAG_AEROBIC_HEART_RATE_UP_LIMIT   = 0x00200000,
    /// Anaerobic Heart Rate Lower Limit supported
    UDS_CHAR_FLAG_ANEROBIC_HEART_RATE_LOW_LIMIT = 0x00400000,
    /// Anaerobic Heart Rate Upper Limit supported
    UDS_CHAR_FLAG_ANEROBIC_HEART_RATE_UP_LIMIT  = 0x00800000,
    /// Five Zone Heart Rate Limits supported
    UDS_CHAR_FLAG_FIVE_ZONE_HEART_RATE_LIMITS   = 0x01000000,
    /// Three Zone Heart Rate Limits supported
    UDS_CHAR_FLAG_THREE_ZONE_HEART_RATE_LIMITS  = 0x02000000,
    /// Two Zone Heart Rate Limit supported
    UDS_CHAR_FLAG_TWO_ZONE_HEART_RATE_LIMIT     = 0x04000000,
    /// Language supported
    UDS_CHAR_FLAG_LANGUAGE                      = 0x08000000,
};

/// Gender characteristic values
enum uds_gender
{
    /// Male
    UDS_GENDER_MALE         = 0x00,
    /// Female
    UDS_GENDER_FEMALE       = 0x01,
    /// Inspecified
    UDS_GENDER_UNSPECIFIED  = 0x02,
};

/// Sport Type for Aerobic and Anaerobic Thresholds characteristic values
enum uds_sport_type
{
    /// Unspecified
    UDS_SPORT_UNSPECIFIED           = 0x00,
    /// Running
    UDS_SPORT_RUNNING               = 0x01,
    /// Cycling
    UDS_SPORT_CYCLING               = 0x02,
    /// Rowing
    UDS_SPORT_ROWING                = 0x03,
    /// Cross Training
    UDS_SPORT_CROSS_TRAINING        = 0x04,
    /// Climbing
    UDS_SPORT_CLIMBING              = 0x05,
    /// Skiing
    UDS_SPORT_SKIING                = 0x06,
    /// Skating
    UDS_SPORT_SKATING               = 0x07,
    /// Arm Exercising
    UDS_SPORT_ARM_EXERCISING        = 0x08,
    /// Lower Body Exercising
    UDS_SPORT_LOWER_BODY_EXERCISING = 0x09,
    /// Lower Body Exercising
    UDS_SPORT_UPPER_BODY_EXERCISING = 0x0A,
    /// Whole Body Exercising
    UDS_SPORT_WHOLE_BODY_EXERCISING = 0x0B,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Date of Birth and Threshold Assessment Date
struct uds_date
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
};

/// Five Zone Heart Rate Limits
struct uds_hr_limit_5zone
{
    uint8_t verylight_to_light;
    uint8_t light_to_moderate;
    uint8_t moderate_to_hard;
    uint8_t hard_to_max;
};

/// Three Zone Heart Rate Limits
struct uds_hr_limit_3zone
{
    uint8_t light_to_moderate;
    uint8_t moderate_to_hard;
};

/// Two Zone Heart Rate Limits
struct uds_hr_limit_2zone
{
    uint8_t fat_to_fitness;
};

/// User control point request
struct uds_ucp_req
{
    /// Operation code
    uint8_t op_code;

    /// Parameter
    union
    {
        /// Register new user parameter
        struct
        {
            /// Consent code
            uint16_t consent_code;
        } reg_new_user;

        /// Consent parameter
        struct
        {
            /// User index
            uint8_t user_idx;
            /// Consent code
            uint16_t consent_code;
        } consent;
    } parameter;
};

/// User control point response
struct uds_ucp_rsp
{
    /// Operation code
    uint8_t op_code;

    /// Request operation code
    uint8_t req_op_code;

    /// Response value
    uint8_t rsp_val;

    /// Response parameter
    union
    {
        /// Structure for register new user response parameter
        struct
        {
            /// User index
            uint8_t user_idx;
        } reg_new_user;
    } parameter;
};

#endif // (BLE_UDS_CLIENT || BLE_UDS_SERVER)

/// @} uds_common

#endif // UDS_COMMON_H_
