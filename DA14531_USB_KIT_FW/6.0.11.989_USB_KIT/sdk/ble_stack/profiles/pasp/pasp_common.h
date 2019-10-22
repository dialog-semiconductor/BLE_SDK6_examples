/**
 ****************************************************************************************
 *
 * @file pasp_common.h
 *
 * @brief Header File - Phone Alert Status Profile Common types.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 * $ Rev $
 *
 ****************************************************************************************
 */


#ifndef _PASP_COMMON_H_
#define _PASP_COMMON_H_

/**
 ****************************************************************************************
 * @addtogroup PASP Phone Alert Status Profile
 * @ingroup PROFILE
 * @brief Phone Alert Status Profile
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwble_config.h"

#if (BLE_PAS_SERVER || BLE_PAS_CLIENT)

/*
 * DEFINES
 ****************************************************************************************
 */

/// Alert Status Flags
#define PASP_RINGER_ACTIVE              (0x01)
#define PASP_VIBRATE_ACTIVE             (0x02)
#define PASP_DISP_ALERT_STATUS_ACTIVE   (0x04)
#define PASP_ALERT_STATUS_VAL_MAX       (0x07)

/// Ringer Settings Keys
#define PASP_RINGER_SILENT              (0)
#define PASP_RINGER_NORMAL              (1)

/// Ringer Control Point Keys
#define PASP_SILENT_MODE                (1)
#define PASP_MUTE_ONCE                  (2)
#define PASP_CANCEL_SILENT_MODE         (3)

#endif //(BLE_PAS_SERVER || BLE_PAS_CLIENT)

/// @} pasp_common

#endif //(_HTP_COMMON_H_)
