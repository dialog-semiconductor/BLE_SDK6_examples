/**
 ****************************************************************************************
 *
 * @file bms_common.h
 *
 * @brief Header file - Bond Management Service common types.
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

#ifndef BMS_COMMON_H_
#define BMS_COMMON_H_

/**
 ****************************************************************************************
 * @addtogroup BMS Bond Management Service
 * @ingroup BMS
 * @brief Bond Management Service
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_BMS_CLIENT || BLE_BMS_SERVER)

#include "prf_types.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// BMS v1.0.0 - OpCode definitions for BM Control Point Characteristic
#define BMS_OPCODE_DEL_BOND                         (0x03)
#define BMS_OPCODE_DEL_ALL_BOND                     (0x06)
#define BMS_OPCODE_DEL_ALL_BOND_BUT_PEER            (0x09)

/// BMS v1.0.0 - BM Feature Bit Description
#define BMS_FEAT_DEL_BOND                           (0x00000010)
#define BMS_FEAT_DEL_BOND_AUTHZ                     (0x00000020)
#define BMS_FEAT_DEL_ALL_BOND                       (0x00000400)
#define BMS_FEAT_DEL_ALL_BOND_AUTHZ                 (0x00000800)
#define BMS_FEAT_DEL_ALL_BOND_BUT_PEER              (0x00010000)
#define BMS_FEAT_DEL_ALL_BOND_BUT_PEER_AUTHZ        (0x00020000)

/// BMS v1.0.0 - Attribute Protocol Error codes
#define BMS_ATT_OPCODE_NOT_SUPPORTED                (0x80)
#define BMS_ATT_OPERATION_FAILED                    (0x81)

#define BMS_FEAT_DEL_BOND_SUPPORTED                 (BMS_FEAT_DEL_BOND |\
                                                    BMS_FEAT_DEL_BOND_AUTHZ)

#define BMS_FEAT_DEL_ALL_BOND_SUPPORTED             (BMS_FEAT_DEL_ALL_BOND |\
                                                    BMS_FEAT_DEL_ALL_BOND_AUTHZ)

#define BMS_FEAT_DEL_ALL_BOND_BUT_PEER_SUPPORTED    (BMS_FEAT_DEL_ALL_BOND_BUT_PEER |\
                                                    BMS_FEAT_DEL_ALL_BOND_BUT_PEER_AUTHZ)

/*
 * STRUCTURES DEFINITIONS
 ****************************************************************************************
 */

struct bms_cntl_point_op
{
    /// Operation code
    uint8_t op_code;

    /// Operand length
    uint16_t operand_length;

    /// Operand
    uint8_t operand[__ARRAY_EMPTY];
};

#endif // (BLE_BMS_CLIENT || BLE_BMS_SERVER)

/// @} BMS_common

#endif // BMS_COMMON_H_
