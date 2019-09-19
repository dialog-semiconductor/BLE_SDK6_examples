/**
 ****************************************************************************************
 *
 * @file bcsc.h
 *
 * @brief Header file - Body Composition Service Client Declaration.
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

#ifndef BCSC_H_
#define BCSC_H_

/**
 ****************************************************************************************
 * @addtogroup BCSC Center Client
 * @ingroup BCSC
 * @brief Body Composition Service Client
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_BCS_CLIENT)

#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "bcs_common.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of ANCC task instances
#define BCSC_IDX_MAX                    (BLE_CONNECTION_MAX)

/// BCSC Characteristics
enum bcsc_bcs_chars
{
    /// Body Composition Feature
    BCSC_CHAR_BC_FEATURE,
    /// Body Composition Measurement
    BCSC_CHAR_BC_MEAS,
    /// Number of characteristics
    BCSC_CHAR_MAX,
};

/// Characteristic descriptors
enum
{
    /// Body Composition Measurement client cfg
    BCSC_DESC_MEAS_CLI_CFG,
    BCSC_DESC_MAX,
    BCSC_DESC_MASK = 0x10,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
///Structure containing the characteristics handles, value handles and descriptors

struct bcs_content
{
    /// service info
    struct prf_svc svc;
    /// If service is a secondary service
    bool is_secondary;
    /// characteristics
    struct prf_char_inf chars[BCSC_CHAR_MAX];
    /// descriptors
    struct prf_char_desc_inf descs[BCSC_DESC_MAX];
};

/// Environment variable for each Connections
struct bcsc_cnx_env
{
    /// Last requested UUID(to keep track of the two services and char)
    uint16_t last_uuid_req;
    /// Last Char. for which a read request has been sent
    uint8_t last_char_code;
    /// counter used to check service uniqueness
    uint8_t nb_svc;
    /// BCS characteristics
    struct bcs_content bcs;
};

/// Weight Scale Client environment variable
struct bcsc_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct bcsc_cnx_env* env[BCSC_IDX_MAX];
    /// State of different task instances
    ke_state_t state[BCSC_IDX_MAX];
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve BCS client profile interface
 *
 * @return BCS client profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* bcsc_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Send enable confirmation to application.
 * @param status Status of the enable: either OK or error encountered while discovery.
 ****************************************************************************************
 */
void bcsc_enable_rsp_send(struct bcsc_env_tag *bcsc_env, uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send indication configuration confirmation to application.
 * @param status Status of the operation
 ****************************************************************************************
 */
void bcsc_bc_meas_cfg_ind_rsp_send(struct bcsc_env_tag * bcsc_env, uint8_t conidx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Unpack Body Composition measurement data into a comprehensive structure.
 * @param[out] pmeas_val    Pointer to BC measurement structure destination.
 * @param[out] pmeas_flags  Pointer to BC measurement flags destination.
 * @param[in] packed_bp     Pointer of the packed data of Body Composition Measurement
 *                          information.
 ****************************************************************************************
 */
void bcsc_unpack_meas_value(bcs_meas_t *pmeas_val, uint16_t *pmeas_flags, uint8_t *packed_bp);

#endif // (BLE_BCS_CLIENT)

/// @} BCSC

#endif //BCSC_H_
