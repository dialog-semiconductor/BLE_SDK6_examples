/**
 ****************************************************************************************
 *
 * @file anc_common.h
 *
 * @brief Header file - Apple Notification Center Service common types.
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

#ifndef ANC_COMMON_H_
#define ANC_COMMON_H_

/**
 ****************************************************************************************
 * @addtogroup ANC Apple Notification Center Service
 * @ingroup ANC
 * @brief Apple Notification Center Service Client
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_ANC_SERVER || BLE_ANC_CLIENT)

#include "prf_types.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Error Codes
// Command not recognized by the NP - Protocol
#define ANC_CMD_UNKNOWN               (0xA0)
// Command improperly formatted - Proprietary
#define ANC_CMD_INVALID               (0xA1)
// One of the parameters does not refer to an existing object on the NP - Proprietary
#define ANC_PARAM_INVALID             (0xA2)
// The action was not performed
#define ANC_ACTION_FAILED             (0xA3)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Category ID Values
enum
{
    /// Other
    CAT_ID_OTHER             = 0,
    /// Incoming Call
    CAT_ID_CALL              = 1,
    /// Missed Call
    CAT_ID_MISSED_CALL       = 2,
    /// Voice Mail
    CAT_ID_VOICE_MAIL        = 3,
    /// Social network
    CAT_ID_SOCIAL            = 4,
    /// Schedule
    CAT_ID_SCHEDULE          = 5,
    /// Email
    CAT_ID_EMAIL             = 6,
    /// News Feed
    CAT_ID_NEWS              = 7,
    /// Health and Fitness
    CAT_ID_HEALTH_FITNESS    = 8,
    /// Business and Finance
    CAT_ID_BUSINESS_FINANCE  = 9,
    /// Location
    CAT_ID_LOCATION          = 10,
    /// Entertainment
    CAT_ID_ENTERTAINMENT     = 11,

    CAT_ID_NB,

    /// All supported categories
    CAT_ID_ALL_SUPPORTED_CAT = 255,
};

/// Event ID Values
enum
{
    /// Notification Added
    EVT_ID_NTF_ADDED        = 0,
    /// Notification Modified
    EVT_ID_NTF_MODIFIED     = 1,
    /// Notification Removed
    EVT_ID_NTF_REMOVED      = 2,

    EVT_ID_NB,

    EVT_ID_ALL_SUPPORTED_EVT = 255,
};

/// Event Flags
enum
{
    /// Silent
    ANC_EVT_FLAG_SILENT                 = (1 << 0),
    /// Important
    ANC_EVT_FLAG_IMPORTANT              = (1 << 1),
    /// PreExisting
    ANC_EVT_FLAG_PRE_EXISTING           = (1 << 2),
    /// Positive Action
    ANC_EVT_FLAG_POS_ACT                = (1 << 3),
    /// Negative Action
    ANC_EVT_FLAG_NEG_ACT                = (1 << 4),

    ANC_EVT_FLAG_NB,

    ANC_EVT_FLAG_ALL_SUPPORTED_EVT_FLAG = (1 << 7),
};

/// Command ID Values
enum
{
    /// Get Notification Attributes
    CMD_ID_GET_NTF_ATTS      = 0,
    /// Get App Attributes
    CMD_ID_GET_APP_ATTS      = 1,
    /// Perform Notification Action
    CMD_ID_PERF_NTF_ACT      = 2,

    CMD_ID_NB,

    /// All supported commands
    CMD_ID_ALL_SUPPORTED_CMD = 255,
};

/// Notification Attribute ID Values
enum
{
    /// App Identifier
    NTF_ATT_ID_APP_ID        = 0,
    /// Title (Needs to be followed by a 2-bytes max length parameter)
    NTF_ATT_ID_TITLE         = 1,
    /// Subtitle (Needs to be followed by a 2-bytes max length parameter)
    NTF_ATT_ID_SUBTITLE      = 2,
    /// Message (Needs to be followed by a 2-bytes max length parameter)
    NTF_ATT_ID_MSG           = 3,
    /// Message Size
    NTF_ATT_ID_MSG_SIZE      = 4,
    /// Date
    NTF_ATT_ID_DATE          = 5,
    /// Positive Action Label
    NTF_ATT_ID_POS_ACT_LBL   = 6,
    /// Negative Action Label
    NTF_ATT_ID_NEG_ACT_LBL   = 7,

    NTF_ATT_ID_NB,

    /// All supported Notification Attributes
    NTF_ATT_ID_ALL_SUPPORTED_NTF_ATT = 255,
};

/// ActionID Values
enum
{
    /// ActionID Postive
    ACT_ID_POS       = 0,
    /// Action ID Negative
    ACT_ID_NEG       = 1,

    ACT_ID_NB,

    ACT_ID_ALL_SUPPORTED_ID = 255,
};

/// Notification Attribute Bit Mask Flags
enum ancc_ntf_attr_present
{
    /// App Identifier present
    NTF_ATT_ID_APP_ID_PRESENT        = 0x01,
    /// Title present
    NTF_ATT_ID_TITLE_PRESENT         = 0x02,
    /// Subtitle present
    NTF_ATT_ID_SUBTITLE_PRESENT      = 0x04,
    /// Message present
    NTF_ATT_ID_MSG_PRESENT           = 0x08,
    /// Message Size present
    NTF_ATT_ID_MSG_SIZE_PRESENT      = 0x10,
    /// Date present
    NTF_ATT_ID_DATE_PRESENT          = 0x20,
};

/// App Attribute ID Values
enum
{
    /// Display Name present
    APP_ATT_ID_DISPLAY_NAME  = 0,

    APP_ADD_ID_NB,

    /// All supported App Attributes
    APP_ATT_ID_ALL_SUPPORTED_APP_ATT = 255,
};

// App Attribute Bit Mask Flags
enum
{
    /// App Identifier
    APP_ATT_ID_DISPLAY_NAME_PRESENT  = 0x01,

};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Notification Source Characteristic Value Structure
struct anc_ntf_src
{
    /// Event ID
    uint8_t event_id;

    /// Event Flags
    uint8_t event_flags;

    /// Category ID
    uint8_t cat_id;

    /// Category Count: Number of active notifications in the given category
    uint8_t cat_cnt;

    /// Notification UID
    uint32_t ntf_uid;
};

#endif // (BLE_ANC_SERVER || BLE_ANC_CLIENT)

/// @} anc_common

#endif // (ANC_COMMON_H_)
