/**
 ****************************************************************************************
 *
 * @file bcss.c
 *
 * @brief Body Composition Service Server implementation.
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

/**
 ****************************************************************************************
 * @addtogroup BCSS
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_BCS_SERVER)

#include <string.h>

#include "attm.h"
#include "bcss.h"
#include "bcss_task.h"
#include "gattc.h"
#include "prf_utils.h"
#include "prf.h"


/*
 * BCS ATTRIBUTES
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the BCSS module.
 * This function performs all the initializations of the Profile module.
 *  - Creation of database (if it's a service)
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    env        Client or Service allocated environment data.
 * @param[in|out] start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     app_task   Application task number.
 * @param[in]     sec_lvl    Security level (AUTH, EKS and MI field of @enum attm_svc_perm_mask)
 * @param[in]     params     Configuration parameters of profile collector or service (32 bits aligned)
 *
 * @return status code to know if profile initialization succeed or not.
 ****************************************************************************************
 */
static uint8_t bcss_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                         uint8_t sec_lvl, struct bcss_db_cfg* params)
{
    uint8_t status;
    uint8_t perm = (sec_lvl & PERM_MASK_SVC_AUTH) | (sec_lvl & PERM_MASK_SVC_EKS) | PERM(SVC_MI, DISABLE);
    uint8_t cfg_flag = BCSS_MANDATORY_MASK;

    /// Full BCS Database Description - Used to add attributes into the database
    struct attm_desc bcss_att_db[BCS_IDX_NB] =
    {
        // Weight Scale Service Declaration
        [BCS_IDX_SVC]          =    {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0},
        [BCS_IDX_FEAT_CHAR]    =    {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
        [BCS_IDX_FEAT_VAL]     =    {ATT_CHAR_BC_FEAT, PERM(RD, ENABLE), PERM(RI, ENABLE) | sizeof(uint32_t)},
        [BCS_IDX_MEAS_CHAR]    =    {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0},
        [BCS_IDX_MEAS_VAL]     =    {ATT_CHAR_BC_MEAS, PERM(IND, ENABLE), 0},
        [BCS_IDX_MEAS_IND_CFG] =    {ATT_DESC_CLIENT_CHAR_CFG, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), PERM(RI, ENABLE)},
    };

    // In case of instantiating as a secondary service
    if (!params->is_primary)
    {
        bcss_att_db[BCS_IDX_SVC].uuid = ATT_DECL_SECONDARY_SERVICE;
    }
    else
    {
        perm |= PERM(SVC_PRIMARY, ENABLE);
    }

    struct bcss_env_tag* bcss_env =
            (struct bcss_env_tag* ) ke_malloc(sizeof(struct bcss_env_tag), KE_MEM_ENV);
    memset(bcss_env, 0, sizeof(struct bcss_env_tag));

    status = attm_svc_create_db(start_hdl, ATT_SVC_BODY_COMPOSITION, &cfg_flag, BCS_IDX_NB,
                                NULL, env->task, &bcss_att_db[0], perm);

    if (status == ATT_ERR_NO_ERROR)
    {
        env->env = (prf_env_t*)bcss_env;
        bcss_env->shdl = *start_hdl;
        bcss_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        bcss_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);
        bcss_env->bc_feat = params->features;
        bcss_env->meas = NULL;
        memset(bcss_env->ind_cfg, 0 , sizeof(bcss_env->ind_cfg));

        // initialize environment variable
        env->id                     = TASK_ID_BCSS;
        env->desc.idx_max           = BCSS_IDX_MAX;
        env->desc.state             = bcss_env->state;
        env->desc.state_handler     = NULL;
        env->desc.default_handler   = &bcss_default_handler;

        ke_state_set(env->task, BCSS_IDLE);
    }
    else
    {
        ke_free(bcss_env);
    }

    return status;
}

/**
 ****************************************************************************************
 * @brief Destruction of the BCSS module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 ****************************************************************************************
 */
static void bcss_destroy(struct prf_task_env* env)
{
    struct bcss_env_tag* bcss_env = (struct bcss_env_tag*) env->env;

    // Free profile environment variables
    env->env = NULL;
    if (bcss_env->meas)
    {
        ke_free(bcss_env->meas);
        bcss_env->meas = NULL;
    }

    ke_free(bcss_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void bcss_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Nothing to do */
}

/**
 ****************************************************************************************
 * @brief Clean-up connection dedicated environment parameters
 * This function performs cleanup of ongoing operations
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void bcss_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct bcss_env_tag* bcss_env = (struct bcss_env_tag*) env->env;

    if (bcss_env->meas)
    {
        ke_free(bcss_env->meas);
        bcss_env->meas = NULL;
    }

    ke_state_set(KE_BUILD_ID(env->task, conidx), BCSS_IDLE);
}

/**
 ****************************************************************************************
 * @brief Helper to get additional MTU size requirements for the specific features
 * @param[in] flag Feature flag.
 * @return Feature's additional space requirements.
 ****************************************************************************************
 */
static uint8_t get_feature_size(uint16_t flag)
{
    uint8_t size = 0;

    switch (flag)
    {
        case BCM_FLAG_UNIT_IMPERIAL:
            size = 0;
            break;
        case BCM_FLAG_TIME_STAMP:
            size = 7;
            break;
        case BCM_FLAG_USER_ID:
            size = 1;
            break;
        case BCM_FLAG_BASAL_METABOLISM:
            size = 2;
            break;
        case BCM_FLAG_MUSCLE_PERCENTAGE:
            size = 2;
            break;
        case BCM_FLAG_MUSCLE_MASS:
            size = 2;
            break;
        case BCM_FLAG_FAT_FREE_MASS:
            size = 2;
            break;
        case BCM_FLAG_SOFT_LEAN_MASS:
            size = 2;
            break;
        case BCM_FLAG_BODY_WATER_MASS:
            size = 2;
            break;
        case BCM_FLAG_IMPEDANCE:
            size = 2;
            break;
        case BCM_FLAG_WEIGHT:
            size = 2;
            break;
        case BCM_FLAG_HEIGHT:
            size = 2;
            break;
        case BCM_FLAG_MULTIPLE_PACKET:
            size = 0;
            break;
        default:
            break;
    }

    return size;
}

/**
 ****************************************************************************************
 * @brief Helper function to translate measurement flags to BCS features.
 * @param[in] flags Measurement flags.
 * @return BCS Features flag.
 ****************************************************************************************
 */
static uint16_t flags_to_features(uint16_t flags)
{
    uint16_t feat = 0x00;

    if (flags & BCM_FLAG_BASAL_METABOLISM)
        feat |= BCS_FEAT_BASAL_METABOLISM;

    if (flags & BCM_FLAG_MUSCLE_PERCENTAGE)
        feat |= BCS_FEAT_MUSCLE_PERCENTAGE;

    if (flags & BCM_FLAG_MUSCLE_MASS)
        feat |= BCS_FEAT_MUSCLE_MASS;

    if (flags & BCM_FLAG_FAT_FREE_MASS)
        feat |= BCS_FEAT_FAT_FREE_MASS;

    if (flags & BCM_FLAG_SOFT_LEAN_MASS)
        feat |= BCS_FEAT_SOFT_LEAN_MASS;

    if (flags & BCM_FLAG_BODY_WATER_MASS)
        feat |= BCS_FEAT_BODY_WATER_MASS;

    if (flags & BCM_FLAG_IMPEDANCE)
        feat |= BCS_FEAT_IMPEDANCE;

    if (flags & BCM_FLAG_WEIGHT)
        feat |= BCS_FEAT_WEIGHT;

    if (flags & BCM_FLAG_HEIGHT)
        feat |= BCS_FEAT_HEIGHT;

    return feat;
}

/**
 ****************************************************************************************
 * @brief Helper function to check if value fits the available free space in PDU.
 * @param[in] buf Pointer to the whole buffer for packed measurement.
 * @param[in] length Whole packed measurement buffer size.
 * @param[in] val Pointer to empty space in buffer for the packed measurement field.
 * @param[in] flags Measurement flag indicating currently packed.
 * @param[in] meas Measurement to be packed.
 * @return BCS Features flag.
 ****************************************************************************************
 */
static uint16_t verify_value_pack(const uint8_t *buf, uint16_t length, uint8_t **val,
                                  uint16_t flag, const bcs_meas_t *meas)
{
    struct bcss_env_tag *bcss_env = PRF_ENV_GET(BCSS, bcss);

    // Check if packed value exceeds the available free space
    if (*val + get_feature_size(flag) > buf + length)
    {
        // Set bit for the feature to be sent in next packet
        bcss_env->ind_cont_feat |= flags_to_features(flag);
        return 0;
    }

    // Turn the bit off as feature fits the mtu
    bcss_env->ind_cont_feat &= ~flags_to_features(flag);
    return flag;
}

/**
 ****************************************************************************************
 * @brief Helper function that splits and pack measurements into multiple packets.
 * @param[in] bcf Measurement flags.
 * @param[in] meas Measurement that should be packed before sending.
 * @param[out] value Buffer for storing the packed measurement value.
 * @param[in] length Packed Measurement buffer size.
 * @return Length of the packed measurement value.
 ****************************************************************************************
 */
static uint8_t pack_attribute_value(uint16_t bcf, const bcs_meas_t *meas,
                                    uint8_t *value, uint16_t length)
{
    uint8_t *ptr = &value[2];
    uint16_t flags = 0;
    struct bcss_env_tag *bcss_env = PRF_ENV_GET(BCSS, bcss);

    if (meas->measurement_unit == BCS_UNIT_IMPERIAL)
    {
            flags |= BCM_FLAG_UNIT_IMPERIAL;
    }

    // Set flag and get features to be sent if its indication continuation
    if (bcss_env->ind_cont_feat)
    {
        flags |= BCM_FLAG_MULTIPLE_PACKET;
    }

    co_write16p(ptr, meas->body_fat_percentage);
    ptr += sizeof(uint16_t);

    if (!bcss_env->ind_cont_feat && (bcf & BCS_FEAT_TIME_STAMP))
    {
            flags |= verify_value_pack(value, length, &ptr, BCM_FLAG_TIME_STAMP, meas);
            ptr += prf_pack_date_time(ptr, &meas->time_stamp);
    }

    if (!bcss_env->ind_cont_feat && (bcf & BCS_FEAT_MULTIPLE_USERS))
    {
            flags |= verify_value_pack(value, length, &ptr, BCM_FLAG_USER_ID, meas);
            co_write8(ptr, meas->user_id);
            ptr += sizeof(uint8_t);
    }

    if ((bcf & BCS_FEAT_BASAL_METABOLISM) && meas->basal_metabolism > 0)
    {
            flags |= verify_value_pack(value, length, &ptr, BCM_FLAG_BASAL_METABOLISM, meas);
            co_write16p(ptr, meas->basal_metabolism);
            ptr += sizeof(uint16_t);
    }

    if ((bcf & BCS_FEAT_MUSCLE_PERCENTAGE) && meas->muscle_percentage > 0)
    {
            flags |= verify_value_pack(value, length, &ptr, BCM_FLAG_MUSCLE_PERCENTAGE, meas);
            co_write16p(ptr, meas->muscle_percentage);
            ptr += sizeof(uint16_t);
    }

    if ((bcf & BCS_FEAT_MUSCLE_MASS) && meas->muscle_mass > 0)
    {
            flags |= verify_value_pack(value, length, &ptr, BCM_FLAG_MUSCLE_MASS, meas);
            co_write16p(ptr, meas->muscle_mass);
            ptr += sizeof(uint16_t);
    }

    if ((bcf & BCS_FEAT_FAT_FREE_MASS) && meas->fat_free_mass > 0)
    {
            flags |= verify_value_pack(value, length, &ptr, BCM_FLAG_FAT_FREE_MASS, meas);
            co_write16p(ptr, meas->fat_free_mass);
            ptr += sizeof(uint16_t);
    }

    if ((bcf & BCS_FEAT_SOFT_LEAN_MASS) && meas->soft_lean_mass > 0)
    {
            flags |= verify_value_pack(value, length, &ptr, BCM_FLAG_SOFT_LEAN_MASS, meas);
            co_write16p(ptr, meas->soft_lean_mass);
            ptr += sizeof(uint16_t);
    }

    if ((bcf & BCS_FEAT_BODY_WATER_MASS) && meas->body_water_mass > 0)
    {
            flags |= verify_value_pack(value, length, &ptr, BCM_FLAG_BODY_WATER_MASS, meas);
            co_write16p(ptr, meas->body_water_mass);
            ptr += sizeof(uint16_t);
    }

    if ((bcf & BCS_FEAT_IMPEDANCE) && meas->impedance > 0)
    {
            flags |= verify_value_pack(value, length, &ptr, BCM_FLAG_IMPEDANCE, meas);
            co_write16p(ptr, meas->impedance);
            ptr += sizeof(uint16_t);
    }

    if ((bcf & BCS_FEAT_WEIGHT) && meas->weight > 0)
    {
            flags |= verify_value_pack(value, length, &ptr, BCM_FLAG_WEIGHT, meas);
            co_write16p(ptr, meas->weight);
            ptr += sizeof(uint16_t);
    }

    if ((bcf & BCS_FEAT_HEIGHT) && meas->height > 0)
    {
            flags |= verify_value_pack(value, length, &ptr, BCM_FLAG_HEIGHT, meas);
            co_write16p(ptr, meas->height);
            ptr += sizeof(uint16_t);
    }

    // Update flag for indication continuation packets
    if (bcss_env->ind_cont_feat)
    {
        flags |= BCM_FLAG_MULTIPLE_PACKET;
    }

    co_write16p(value, flags);

    return ptr - value;
}


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// BCSS Task interface required by profile manager
const struct prf_task_cbs bcss_itf =
{
    (prf_init_fnct) bcss_init,
    bcss_destroy,
    bcss_create,
    bcss_cleanup,
};


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* bcss_prf_itf_get(void)
{
   return &bcss_itf;
}

void bcss_ind_rsp_send(uint8_t conidx, uint8_t status)
{
    struct bcss_env_tag *bcss_env = PRF_ENV_GET(BCSS, bcss);

    struct bcss_meas_val_ind_rsp *rsp = KE_MSG_ALLOC(BCSS_MEAS_VAL_IND_RSP,
                                                     prf_dst_task_get(&bcss_env->prf_env, conidx),
                                                     prf_src_task_get(&bcss_env->prf_env, conidx),
                                                     bcss_meas_val_ind_rsp);

    rsp->status = status;
    // Go back to IDLE state
    ke_state_set(prf_src_task_get(&(bcss_env->prf_env), conidx), BCSS_IDLE);

    ke_msg_send(rsp);
}

void bcss_indicate(uint8_t conidx, uint16_t features, const bcs_meas_t *meas)
{
    att_size_t att_length;
    uint16_t mtu = gattc_get_mtu(conidx);
    uint8_t att_value[mtu - 3];

    struct bcss_env_tag *bcss_env = PRF_ENV_GET(BCSS, bcss);

    if (meas == NULL)
    {
        bcss_ind_rsp_send(conidx, ATT_ERR_APP_ERROR);
        return;
    }

    // Copy the measurement before slicing it for MTU-3 sized chunks
    if(bcss_env->meas == NULL)
    {
        bcss_env->meas = (bcs_meas_t*)ke_malloc(sizeof(bcs_meas_t), KE_MEM_NON_RETENTION);
        *(bcss_env->meas) = *meas;
    }

    att_length = pack_attribute_value(features, meas, att_value, mtu - 3);
    // Notify current value, task will receive the confirmation
    struct gattc_send_evt_cmd *evt_cmd = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                                                          KE_BUILD_ID(TASK_GATTC, conidx),
                                                          prf_src_task_get(&(bcss_env->prf_env),0),
                                                          gattc_send_evt_cmd, att_length);

    // Fill in the parameter structure
    evt_cmd->operation = GATTC_INDICATE;
    evt_cmd->handle = bcss_env->shdl + BCS_IDX_MEAS_VAL;
    evt_cmd->length = att_length;
    // Pack the Wt Measurement value
    memcpy(evt_cmd->value, att_value, att_length);

    ke_msg_send(evt_cmd);
}

#endif // (BLE_BCS_SERVER)

/// @} BCSS
