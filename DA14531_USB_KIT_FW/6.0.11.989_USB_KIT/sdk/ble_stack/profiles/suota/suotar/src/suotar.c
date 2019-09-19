/**
 ****************************************************************************************
 *
 * @file suotar.c
 *
 * @brief Software Programming Over The Air (SUOTA) Receiver Implementation.
 *
 * Copyright (c) 2013-2018 Dialog Semiconductor. All rights reserved.
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
 * @addtogroup SUOTAR
 * @{
 ****************************************************************************************
 */

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_SUOTAR__)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwble_config.h"

#if (BLE_SUOTA_RECEIVER)
#include <string.h>

#include "suotar.h"
#include "suotar_task.h"

/*
 * SUOTA PROFILE ATTRIBUTES DEFINITION
 ****************************************************************************************
 */

// SIG has assigned a 16bit UUID for SUOTAR service (0xFEF5). However we use proprietary UUIDs for the characteristics.
#define ATT_SVC_SUOTAR                   {0xF5, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define SUOTA_MEM_DEV_UUID               {0x34, 0xcc, 0x54, 0xb9, 0xf9, 0x56, 0xc6, 0x91, 0x21, 0x40, 0xa6, 0x41, 0xa8, 0xca, 0x82, 0x80}
#define SUOTA_GPIO_MAP_UUID              {0x51, 0x86, 0xf0, 0x5a, 0x34, 0x42, 0x04, 0x88, 0x5f, 0x4b, 0xc3, 0x5e, 0xf0, 0x49, 0x42, 0x72}
#define SUOTA_MEM_INFO_UUID              {0xd4, 0x4f, 0x33, 0xfb, 0x92, 0x7c, 0x22, 0xa0, 0xfe, 0x45, 0xa1, 0x47, 0x25, 0xdb, 0x53, 0x6c}
#define SUOTA_PATCH_LEN_UUID             {0x31, 0xda, 0x3f, 0x67, 0x5b, 0x85, 0x83, 0x91, 0xd8, 0x49, 0x0c, 0x00, 0xa3, 0xb9, 0x84, 0x9d}
#define SUOTA_PATCH_DATA_UUID            {0xb2, 0x9c, 0x7b, 0xb1, 0xd0, 0x57, 0x16, 0x91, 0xa1, 0x4c, 0x16, 0xd5, 0xe8, 0x71, 0x78, 0x45}
#define SUOTA_SERV_STATUS_UUID           {0x88, 0x5c, 0x06, 0x6a, 0xeb, 0xb3, 0x0a, 0x99, 0xf5, 0x46, 0x8c, 0x79, 0x94, 0xdf, 0x78, 0x5f}

#if SUOTA_VERSION >= SUOTA_VERSION_1_3
#define SUOTA_VERSION_UUID               {0x3a, 0x91, 0x3b, 0xdb, 0xc8, 0xac, 0x1d, 0xa2, 0x1b, 0x40, 0xe5, 0x0d, 0xb5, 0xe8, 0xb4, 0x64}
#define SUOTA_PATCH_DATA_CHAR_SIZE_UUID  {0x3b, 0xfb, 0x67, 0x52, 0x87, 0x8f, 0x54, 0x84, 0x9c, 0x4d, 0xbe, 0x77, 0xdd, 0xdf, 0xc3, 0x42}
#define SUOTA_MTU_UUID                   {0x3c, 0xe2, 0xfc, 0x3d, 0x90, 0xc4, 0xaf, 0xa3, 0xbb, 0x43, 0x3d, 0x82, 0xea, 0x1e, 0xde, 0xb7}
#endif

const char pdm_value_descr[] = "Mem Dev: B3=Mem, B[2:0]=Addr";
const char pdgpio_value_descr[] = "GPIO: b[3:0]= Pin, b[7:4]= Port Number";
const char pdmi_value_descr[] = "Mem Info: B[3:2]=#Patches, B[1:0]=Entire_Length";
const char pd_value_descr[] = "Patch Data: 20 bytes";
const char pdl_value_descr[] = "New patch len: 16 bits";
const char pds_value_descr[] = "SUOTA serv status: 8 bits";

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if SUOTA_VERSION >= SUOTA_VERSION_1_3
static void suotar_set_dle_params_in_db(uint16_t suota_shdl)
{
    uint8_t version = SUOTA_VERSION;
    uint16_t pd_char_size = SUOTA_PD_CHAR_SIZE;
    //Set SUOTA version into DB
    attmdb_att_set_value(suota_shdl + SUOTA_IDX_VERSION_VAL,
                         sizeof(uint8_t),
                         0,
                         &version);
    //Set PATCH_DATA_CHAR_SIZE value
    attmdb_att_set_value(suota_shdl + SUOTA_IDX_PATCH_DATA_CHAR_SIZE_VAL,
                         sizeof(uint16_t),
                         0,
                         (uint8_t*)&pd_char_size);
}
#endif

static uint8_t suotar_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                            uint8_t sec_lvl, struct suotar_db_cfg* params)
{
    struct suotar_env_tag* suotar_env = NULL;
    struct gattm_svc_desc *svc_desc = NULL;
    uint8_t att_idx = 0;
    uint8_t status;

    // Allocate database descriptor
    svc_desc = (struct gattm_svc_desc *) ke_malloc(sizeof(struct gattm_svc_desc) + \
                                // No need for svc declaration, thus (SUOTA_IDX_NB - 1)
                                (SUOTA_IDX_NB - 1) * sizeof(struct gattm_att_desc), \
                                KE_MEM_NON_RETENTION);

    /*---------------------------------------------------*
     * SUOTAR Service Creation
     *---------------------------------------------------*/
    svc_desc->start_hdl = *start_hdl;
    svc_desc->task_id = env->task;
    svc_desc->perm = ((sec_lvl & PERM_MASK_SVC_AUTH) | (sec_lvl & PERM_MASK_SVC_EKS) | \
                        PERM(SVC_PRIMARY, ENABLE) | PERM(SVC_MI, DISABLE) | \
                        PERM_VAL(SVC_UUID_LEN, PERM_UUID_16));
    struct att_uuid_128 temp =
    {
        .uuid = ATT_SVC_SUOTAR
    };
    memcpy((struct att_uuid_128 *)(&svc_desc->uuid), &temp, sizeof(svc_desc->uuid) / sizeof(svc_desc->uuid[0]));

    // Char. decl. and value attribute for patch_mem_dev
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_MEM_DEV_UUID,
            .perm = PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE) | \
                    PERM_VAL(UUID_LEN, PERM_UUID_128),
            .max_len = sizeof(uint32_t) };

    // Char. decl. and value attribute for gpio_map
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_GPIO_MAP_UUID,
            .perm = PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE) | \
                    PERM_VAL(UUID_LEN, PERM_UUID_128),
            .max_len = sizeof(uint32_t) };

    // Char. decl. and value attribute for patch_mem info
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_MEM_INFO_UUID,
            .perm = PERM(RD, ENABLE) | PERM_VAL(UUID_LEN, PERM_UUID_128),
            .max_len = sizeof(uint32_t) };

    // Char. decl. and value attribute for patch_len
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_PATCH_LEN_UUID,
            .perm = PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE) | \
                    PERM_VAL(UUID_LEN, PERM_UUID_128),
            .max_len = sizeof(uint16_t) };

    // Char. decl. and value attribute for patch_data
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_PATCH_DATA_UUID,
            .perm = PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE) | \
                    PERM(WRITE_COMMAND, ENABLE) | \
                    PERM_VAL(UUID_LEN, PERM_UUID_128),
            .max_len = sizeof(uint8_t) * SUOTA_PD_CHAR_SIZE };

    // Char. decl., value attribute and CCC for patch_status
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_SERV_STATUS_UUID,
            .perm = PERM(RD, ENABLE) | PERM(NTF, ENABLE) | \
                    PERM_VAL(UUID_LEN, PERM_UUID_128),
            .max_len = sizeof(uint8_t) };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DESC_CLIENT_CHAR_CFG),
            .perm = PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE) | \
                    PERM(WRITE_COMMAND, ENABLE) | PERM_VAL(UUID_LEN, PERM_UUID_16),
            .max_len = 0 };

#if SUOTA_VERSION >= SUOTA_VERSION_1_3
    // Char. decl. and value attribute for version
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_VERSION_UUID,
            .perm = PERM(RD, ENABLE) | PERM_VAL(UUID_LEN, PERM_UUID_128),
            .max_len = sizeof(uint8_t) };

    // Char. decl. and value attribute for patch_data_char_size
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_PATCH_DATA_CHAR_SIZE_UUID,
            .perm = PERM(RD, ENABLE) | PERM_VAL(UUID_LEN, PERM_UUID_128),
            .max_len = sizeof(uint16_t) };

    // Char. decl. and value attribute for mtu
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_MTU_UUID,
            .perm = PERM(RD, ENABLE) | PERM_VAL(UUID_LEN, PERM_UUID_128),
            .max_len = PERM(RI, ENABLE) | sizeof(uint16_t) };
#endif

    // Count all attributes and create database
    svc_desc->nb_att = att_idx;
    status = attmdb_add_service(svc_desc);

    if (status == ATT_ERR_NO_ERROR)
    {
        // Allocate SUOTAR required environment variable
        suotar_env = (struct suotar_env_tag*) \
                        ke_malloc(sizeof(struct suotar_env_tag), KE_MEM_ENV);
        memset(suotar_env, 0 , sizeof(struct suotar_env_tag));

        suotar_env->suota_shdl        = svc_desc->start_hdl;
        suotar_env->features          = params->features;
        suotar_env->ntf_cfg           = GATT_CCC_STOP_NTFIND;
        suotar_env->prf_env.app_task  = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        suotar_env->prf_env.prf_task  = env->task | PERM(PRF_MI, DISABLE);

        // Initialize environment variable
        env->id                     = TASK_ID_SUOTAR;
        env->desc.idx_max           = SUOTAR_IDX_MAX;
        env->desc.state             = suotar_env->state;
        env->desc.default_handler   = &suotar_default_handler;
        env->env = (prf_env_t*) suotar_env;

        // Set start handle to first allocated service value
        *start_hdl = suotar_env->suota_shdl;

        // Service is ready, go into an Idle state
        ke_state_set(env->task, SUOTAR_IDLE);

#if SUOTA_VERSION >= SUOTA_VERSION_1_3
        suotar_set_dle_params_in_db(suotar_env->suota_shdl);
#endif
    }

    ke_free(svc_desc);
    return status;
}

static void suotar_destroy(struct prf_task_env* env)
{
    struct suotar_env_tag* suotar_env = (struct suotar_env_tag*) env->env;

    // Free profile environment variables
    env->env = NULL;
    ke_free(suotar_env);
}

static void suotar_create(struct prf_task_env* env, uint8_t conidx)
{
    // Do nothing here
}

static void suotar_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct suotar_env_tag* suotar_env = (struct suotar_env_tag*) env->env;
    uint8_t suota_patch_status = SUOTAR_RESERVED;
    uint8_t status;

    // Reset patch status CCC value
    suotar_env->ntf_cfg = GATT_CCC_STOP_NTFIND;

    // Reset patch status value
    status = attmdb_att_set_value(suotar_env->suota_shdl + SUOTA_IDX_PATCH_STATUS_VAL,
                                  sizeof(uint8_t), 0, (uint8_t *)(&suota_patch_status));
    ASSERT_ERROR(status == ATT_ERR_NO_ERROR);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// SUOTA Receiver Task interface required by profile manager
const struct prf_task_cbs suotar_itf =
{
    (prf_init_fnct) suotar_init,
    suotar_destroy,
    suotar_create,
    suotar_cleanup,
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* suotar_prf_itf_get(void)
{
   return &suotar_itf;
}

#endif //BLE_SUOTA_RECEIVER

#endif

/// @} SUOTAR
