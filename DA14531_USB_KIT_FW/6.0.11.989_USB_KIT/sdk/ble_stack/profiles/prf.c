/**
 ****************************************************************************************
 *
 * @file prf.c
 *
 * @brief Entry point of profile source file.
 *
 * Used to manage life cycle of profiles
 *
 * Copyright (C) RivieraWaves 2009-2015
 * Copyright (C) 2018 Modified by Dialog Semiconductor
 *
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @addtogroup PRF
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_PROFILES)
#include "prf.h"


#if (BLE_HT_THERMOM)
#include "htpt.h"
#endif // (BLE_HT_THERMOM)

#if (BLE_HT_COLLECTOR)
#include "htpc.h"
#endif // (BLE_HT_COLLECTOR)

#if (BLE_DIS_SERVER)
#include "diss.h"
#endif // (BLE_HT_THERMOM)

#if (BLE_DIS_CLIENT)
#include "disc.h"
#endif // (BLE_DIS_CLIENT)

#if (BLE_BP_SENSOR)
#include "blps.h"
#endif // (BLE_BP_SENSOR)

#if (BLE_BP_COLLECTOR)
#include "blpc.h"
#endif // (BLE_BP_COLLECTOR)

#if (BLE_TIP_SERVER)
#include "tips.h"
#endif // (BLE_TIP_SERVER)

#if (BLE_TIP_CLIENT)
#include "tipc.h"
#endif // (BLE_TIP_CLIENT)

#if (BLE_HR_SENSOR)
#include "hrps.h"
#endif // (BLE_HR_SENSOR)

#if (BLE_HR_COLLECTOR)
#include "hrpc.h"
#endif // (BLE_HR_COLLECTOR)

#if (BLE_FINDME_LOCATOR)
#include "findl.h"
#endif // (BLE_FINDME_LOCATOR)

#if (BLE_FINDME_TARGET)
#include "findt.h"
#endif // (BLE_FINDME_TARGET)

#if (BLE_PROX_MONITOR)
#include "proxm.h"
#endif // (BLE_PROX_MONITOR)

#if (BLE_PROX_REPORTER)
#include "proxr.h"
#endif // (BLE_PROX_REPORTER)

#if (BLE_SP_CLIENT)
#include "scppc.h"
#endif // (BLE_SP_CLENT)

#if (BLE_SP_SERVER)
#include "scpps.h"
#endif // (BLE_SP_SERVER)

#if (BLE_BATT_CLIENT)
#include "basc.h"
#endif // (BLE_BATT_CLIENT)

#if (BLE_BATT_SERVER)
#include "bass.h"
#endif // (BLE_BATT_SERVER)

#if (BLE_HID_DEVICE)
#include "hogpd.h"
#endif // (BLE_HID_DEVICE)

#if (BLE_HID_BOOT_HOST)
#include "hogpbh.h"
#endif // (BLE_HID_BOOT_HOST)

#if (BLE_HID_REPORT_HOST)
#include "hogprh.h"
#endif // (BLE_HID_REPORT_HOST)

#if (BLE_GL_COLLECTOR)
#include "glpc.h"
#endif // (BLE_GL_COLLECTOR)

#if (BLE_GL_SENSOR)
#include "glps.h"
#endif // (BLE_GL_SENSOR)

#if (BLE_RSC_COLLECTOR)
#include "rscpc.h"
#endif // (BLE_RSC_COLLECTOR)

#if (BLE_RSC_SENSOR)
#include "rscps.h"
#endif // (BLE_RSC_COLLECTOR)

#if (BLE_CSC_COLLECTOR)
#include "cscpc.h"
#endif // (BLE_CSC_COLLECTOR)

#if (BLE_CSC_SENSOR)
#include "cscps.h"
#endif // (BLE_CSC_COLLECTOR)

#if (BLE_AN_CLIENT)
#include "anpc.h"
#endif // (BLE_AN_CLIENT)

#if (BLE_AN_SERVER)
#include "anps.h"
#endif // (BLE_AN_SERVER)

#if (BLE_PAS_CLIENT)
#include "paspc.h"
#endif // (BLE_PAS_CLIENT)

#if (BLE_PAS_SERVER)
#include "pasps.h"
#endif // (BLE_PAS_SERVER)

#if (BLE_CP_COLLECTOR)
#include "cppc.h"
#endif //(BLE_CP_COLLECTOR)

#if (BLE_CP_SENSOR)
#include "cpps.h"
#endif //(BLE_CP_SENSOR)

#if (BLE_LN_COLLECTOR)
#include "lanc.h"
#endif //(BLE_CP_COLLECTOR)

#if (BLE_LN_SENSOR)
#include "lans.h"
#endif //(BLE_CP_SENSOR)

#if (BLE_BMS_CLIENT)
#include "bmsc.h"
#endif // (BLE_BMS_CLIENT)

#if (BLE_BMS_SERVER)
#include "bmss.h"
#endif // (BLE_BMS_SERVER)

#if (BLE_BCS_CLIENT)
#include "bcsc.h"
#endif // (BLE_BCS_CLIENT)

#if (BLE_BCS_SERVER)
#include "bcss.h"
#endif // (BLE_BCS_SERVER)

#if (BLE_UDS_CLIENT)
#include "udsc.h"
#endif // (BLE_UDS_CLIENT)

#if (BLE_UDS_SERVER)
#include "udss.h"
#endif // (BLE_UDS_SERVER)

#if (BLE_WSS_COLLECTOR)
#include "wssc.h"
#endif // (BLE_WSS_COLLECTOR)

#if (BLE_WSS_SERVER)
#include "wsss.h"
#endif // (BLE_WSS_SERVER)

#if (BLE_CTS_SERVER)
#include "ctss.h"
#endif //(BLE_CTS_SERVER)

#if (BLE_CTS_CLIENT)
#include "ctsc.h"
#endif //(BLE_CTS_CLIENT)

#if (BLE_GATT_CLIENT)
#include "gatt_client.h"
#endif //(BLE_GATT_CLIENT)

#if (BLE_SUOTA_RECEIVER)
#include "suotar.h"
#endif // (BLE_SUOTA_RECEIVER)

#if (BLE_CUSTOM1_SERVER)
#include "custs1.h"
#endif //(BLE_CUSTOM1_SERVER)

#if (BLE_CUSTOM2_SERVER)
#include "custs2.h"
#endif //(BLE_CUSTOM2_SERVER)

#if (BLE_ANC_CLIENT)
#include "ancc.h"
#endif //(BLE_ANC_CLIENT)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * MACROS
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_PRF__)

struct prf_env_tag prf_env __SECTION_ZERO("retention_mem_area0");

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve profile interface
 ****************************************************************************************
 */
static const struct prf_task_cbs * prf_itf_get(uint16_t task_id)
{
    const struct prf_task_cbs* prf_cbs = NULL;

    switch(KE_TYPE_GET(task_id))
    {
        #if (BLE_HT_THERMOM)
        case TASK_ID_HTPT:
            prf_cbs = htpt_prf_itf_get();
            break;
        #endif // (BLE_HT_THERMOM)

        #if (BLE_HT_COLLECTOR)
        case TASK_ID_HTPC:
            prf_cbs = htpc_prf_itf_get();
            break;
        #endif // (BLE_HT_COLLECTOR)

        #if (BLE_DIS_SERVER)
        case TASK_ID_DISS:
            prf_cbs = diss_prf_itf_get();
            break;
        #endif // (BLE_DIS_SERVER)

        #if (BLE_DIS_CLIENT)
        case TASK_ID_DISC:
            prf_cbs = disc_prf_itf_get();
            break;
        #endif // (BLE_DIS_CLIENT)

        #if (BLE_BP_SENSOR)
        case TASK_ID_BLPS:
            prf_cbs = blps_prf_itf_get();
            break;
        #endif // (BLE_BP_SENSOR)

        #if (BLE_BP_COLLECTOR)
        case TASK_ID_BLPC:
            prf_cbs = blpc_prf_itf_get();
            break;
        #endif // (BLE_BP_COLLECTOR)

        #if (BLE_TIP_SERVER)
        case TASK_ID_TIPS:
            prf_cbs = tips_prf_itf_get();
            break;
        #endif // (BLE_TIP_SERVER)

        #if (BLE_TIP_CLIENT)
        case TASK_ID_TIPC:
            prf_cbs = tipc_prf_itf_get();
            break;
        #endif // (BLE_TIP_CLIENT)

        #if (BLE_HR_SENSOR)
        case TASK_ID_HRPS:
            prf_cbs = hrps_prf_itf_get();
            break;
        #endif // (BLE_HR_SENSOR)

        #if (BLE_HR_COLLECTOR)
        case TASK_ID_HRPC:
            prf_cbs = hrpc_prf_itf_get();
            break;
        #endif // (BLE_HR_COLLECTOR)

        #if (BLE_FINDME_LOCATOR)
        case TASK_ID_FINDL:
            prf_cbs = findl_prf_itf_get();
            break;
        #endif // (BLE_FINDME_LOCATOR)

        #if (BLE_FINDME_TARGET)
        case TASK_ID_FINDT:
            prf_cbs = findt_prf_itf_get();
            break;
        #endif // (BLE_FINDME_TARGET)

        #if (BLE_PROX_MONITOR)
        case TASK_ID_PROXM:
            prf_cbs = proxm_prf_itf_get();
            break;
        #endif // (BLE_PROX_MONITOR)

        #if (BLE_PROX_REPORTER)
        case TASK_ID_PROXR:
            prf_cbs = proxr_prf_itf_get();
            break;
        #endif // (BLE_PROX_REPORTER)

        #if (BLE_SP_SERVER)
        case TASK_ID_SCPPS:
            prf_cbs = scpps_prf_itf_get();
            break;
        #endif // (BLE_SP_SERVER)

        #if (BLE_SP_CLIENT)
        case TASK_ID_SCPPC:
            prf_cbs = scppc_prf_itf_get();
            break;
        #endif // (BLE_SP_CLIENT)

        #if (BLE_BATT_SERVER)
        case TASK_ID_BASS:
            prf_cbs = bass_prf_itf_get();
            break;
        #endif // (BLE_BATT_SERVER)

        #if (BLE_BATT_CLIENT)
        case TASK_ID_BASC:
            prf_cbs = basc_prf_itf_get();
            break;
        #endif // (BLE_BATT_CLIENT)

        #if (BLE_HID_DEVICE)
        case TASK_ID_HOGPD:
            prf_cbs = hogpd_prf_itf_get();
            break;
        #endif // (BLE_HID_DEVICE)

        #if (BLE_HID_BOOT_HOST)
        case TASK_ID_HOGPBH:
            prf_cbs = hogpbh_prf_itf_get();
            break;
        #endif // (BLE_HID_BOOT_HOST)

        #if (BLE_HID_REPORT_HOST)
        case TASK_ID_HOGPRH:
            prf_cbs = hogprh_prf_itf_get();
            break;
        #endif // (BLE_HID_REPORT_HOST)

        #if (BLE_GL_COLLECTOR)
        case TASK_ID_GLPC:
            prf_cbs = glpc_prf_itf_get();
            break;
        #endif // (BLE_GL_COLLECTOR)

        #if (BLE_GL_SENSOR)
        case TASK_ID_GLPS:
            prf_cbs = glps_prf_itf_get();
            break;
        #endif // (BLE_GL_SENSOR)

        #if (BLE_RSC_COLLECTOR)
        case TASK_ID_RSCPC:
            prf_cbs = rscpc_prf_itf_get();
            break;
        #endif // (BLE_RSC_COLLECTOR)

        #if (BLE_RSC_SENSOR)
        case TASK_ID_RSCPS:
            prf_cbs = rscps_prf_itf_get();
            break;
        #endif // (BLE_RSC_SENSOR)

        #if (BLE_CSC_COLLECTOR)
        case TASK_ID_CSCPC:
            prf_cbs = cscpc_prf_itf_get();
            break;
        #endif // (BLE_CSC_COLLECTOR)

        #if (BLE_CSC_SENSOR)
        case TASK_ID_CSCPS:
            prf_cbs = cscps_prf_itf_get();
            break;
        #endif // (BLE_CSC_SENSOR)

        #if (BLE_CP_COLLECTOR)
        case TASK_ID_CPPC:
            prf_cbs = cppc_prf_itf_get();
            break;
        #endif // (BLE_CP_COLLECTOR)

        #if (BLE_CP_SENSOR)
        case TASK_ID_CPPS:
            prf_cbs = cpps_prf_itf_get();
            break;
        #endif // (BLE_CP_SENSOR)

        #if (BLE_LN_COLLECTOR)
        case TASK_ID_LANC:
            prf_cbs = lanc_prf_itf_get();
            break;
        #endif // (BLE_LN_COLLECTOR)

        #if (BLE_LN_SENSOR)
        case TASK_ID_LANS:
            prf_cbs = lans_prf_itf_get();
            break;
        #endif // (BLE_LN_SENSOR)

        #if (BLE_AN_CLIENT)
        case TASK_ID_ANPC:
            prf_cbs = anpc_prf_itf_get();
            break;
        #endif // (BLE_AN_CLIENT)

        #if (BLE_AN_SERVER)
        case TASK_ID_ANPS:
            prf_cbs = anps_prf_itf_get();
            break;
        #endif // (BLE_AN_SERVER)

        #if (BLE_PAS_CLIENT)
        case TASK_ID_PASPC:
            prf_cbs = paspc_prf_itf_get();
            break;
        #endif // (BLE_PAS_CLIENT)

        #if (BLE_PAS_SERVER)
        case TASK_ID_PASPS:
            prf_cbs = pasps_prf_itf_get();
            break;
        #endif // (BLE_PAS_SERVER)

        #if (BLE_BMS_CLIENT)
        case TASK_ID_BMSC:
            prf_cbs = bmsc_prf_itf_get();
            break;
        #endif // (BLE_BMS_CLIENT)

        #if (BLE_BMS_SERVER)
        case TASK_ID_BMSS:
            prf_cbs = bmss_prf_itf_get();
            break;
        #endif // (BLE_BMS_SERVER)

        #if (BLE_CTS_SERVER)
        case TASK_ID_CTSS:
            prf_cbs = ctss_prf_itf_get();
            break;
        #endif // (BLE_CTS_SERVER)

        #if (BLE_CTS_CLIENT)
        case TASK_ID_CTSC:
            prf_cbs = ctsc_prf_itf_get();
            break;
        #endif // (BLE_CTS_CLIENT)

        #if (BLE_WSS_COLLECTOR)
        case TASK_ID_WSSC:
            prf_cbs = wssc_prf_itf_get();
            break;
        #endif // (BLE_WSS_COLLECTOR)

        #if (BLE_WSS_SERVER)
        case TASK_ID_WSSS:
            prf_cbs = wsss_prf_itf_get();
            break;
        #endif // (BLE_WSS_SERVER)

        #if (BLE_BCS_CLIENT)
        case TASK_ID_BCSC:
            prf_cbs = bcsc_prf_itf_get();
            break;
        #endif // (BLE_BCS_CLIENT)

        #if (BLE_BCS_SERVER)
        case TASK_ID_BCSS:
            prf_cbs = bcss_prf_itf_get();
            break;
        #endif // (BLE_BCS_SERVER)

        #if (BLE_UDS_CLIENT)
        case TASK_ID_UDSC:
            prf_cbs = udsc_prf_itf_get();
            break;
        #endif // (BLE_UDS_CLIENT)

        #if (BLE_UDS_SERVER)
        case TASK_ID_UDSS:
            prf_cbs = udss_prf_itf_get();
            break;
        #endif // (BLE_UDS_SERVER)

        #if (BLE_GATT_CLIENT)
        case TASK_ID_GATT_CLIENT:
            prf_cbs = gatt_client_prf_itf_get();
            break;
        #endif // (BLE_GATT_CLIENT)

        #if (BLE_SUOTA_RECEIVER)
        case TASK_ID_SUOTAR:
            prf_cbs = suotar_prf_itf_get();
            break;
        #endif // (BLE_SUOTA_RECEIVER)

        #if (BLE_CUSTOM1_SERVER)
        case TASK_ID_CUSTS1:
            prf_cbs = custs1_prf_itf_get();
            break;
        #endif // (BLE_PAS_SERVER)

        #if (BLE_CUSTOM2_SERVER)
        case TASK_ID_CUSTS2:
            prf_cbs = custs2_prf_itf_get();
            break;
        #endif // (BLE_PAS_SERVER)

        #if (BLE_ANC_CLIENT)
        case TASK_ID_ANCC:
            prf_cbs = ancc_prf_itf_get();
            break;
        #endif // (BLE_ANC_CLIENT)

        default: /* Nothing to do */ break;
    }

    return prf_cbs;
}

/*
 * EXPORTED FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */
void prf_init_func(bool reset)
{
    uint8_t i;
    if(!reset)
    {
        // FW boot profile initialization
        for(i = 0; i < BLE_NB_PROFILES ; i++)
        {
            prf_env.prf[i].env  = NULL;
            prf_env.prf[i].task = TASK_RFU_5 + i +1;
            prf_env.prf[i].id   = TASK_ID_INVALID;

            // Initialize Task Descriptor
            prf_env.prf[i].desc.default_handler = NULL;
            prf_env.prf[i].desc.state           = NULL;
            prf_env.prf[i].desc.idx_max         = 0;

            prf_env.prf[i].desc.state_max       = 0;
            prf_env.prf[i].desc.state_handler   = NULL;

            ke_task_create(prf_env.prf[i].task, &(prf_env.prf[i].desc));
        }
    }
    else
    {
        // FW boot profile destruction
        for(i = 0; i < BLE_NB_PROFILES ; i++)
        {
            // Get Profile API
            const struct prf_task_cbs * cbs = prf_itf_get(prf_env.prf[i].id);
            if(cbs != NULL)
            {
                // request to destroy profile
                cbs->destroy(&(prf_env.prf[i]));
            }
            // unregister profile
            prf_env.prf[i].id   = TASK_ID_INVALID;
            prf_env.prf[i].desc.default_handler = NULL;
            prf_env.prf[i].desc.state           = NULL;
            prf_env.prf[i].desc.idx_max         = 0;

            // Request kernel to flush task messages
            ke_task_msg_flush(KE_TYPE_GET(prf_env.prf[i].task));
        }
    }
}


uint8_t prf_add_profile_func(struct gapm_profile_task_add_cmd * params, ke_task_id_t* prf_task)
{
    uint8_t i;
    uint8_t status = GAP_ERR_NO_ERROR;

    // retrieve profile callback
    const struct prf_task_cbs * cbs = prf_itf_get(params->prf_task_id);
    if(cbs == NULL)
    {
        // profile API not available
        status = GAP_ERR_INVALID_PARAM;
    }

    // check if profile not already present in task list
    if(status == GAP_ERR_NO_ERROR)
    {
        for(i = 0; i < BLE_NB_PROFILES ; i++)
        {
            if(prf_env.prf[i].id == params->prf_task_id)
            {
                status = GAP_ERR_NOT_SUPPORTED;
                break;
            }
        }
    }

    if(status == GAP_ERR_NO_ERROR)
    {
        // find fist available task
        for(i = 0; i < BLE_NB_PROFILES ; i++)
        {
            // available task found
            if(prf_env.prf[i].id == TASK_ID_INVALID)
            {
                // initialize profile
                status = cbs->init(&(prf_env.prf[i]), &(params->start_hdl), params->app_task, params->sec_lvl, params->param);

                // initialization succeed
                if(status == GAP_ERR_NO_ERROR)
                {
                    // register profile
                    prf_env.prf[i].id = params->prf_task_id;
                    *prf_task = prf_env.prf[i].task;
                }
                break;
            }
        }

        if(i == BLE_NB_PROFILES)
        {
            status = GAP_ERR_INSUFF_RESOURCES;
        }
    }

    return status;
}



void prf_create_func(uint8_t conidx)
{
    uint8_t i;
    /* simple connection creation handler, nothing to do. */

    // execute create function of each profiles
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        // Get Profile API
        const struct prf_task_cbs * cbs = prf_itf_get(prf_env.prf[i].id);
        if(cbs != NULL)
        {
            // call create callback
            cbs->create(&(prf_env.prf[i]), conidx);
        }
    }
}


void prf_cleanup_func(uint8_t conidx, uint8_t reason)
{
    uint8_t i;
    /* simple connection creation handler, nothing to do. */

    // execute create function of each profiles
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        // Get Profile API
        const struct prf_task_cbs * cbs = prf_itf_get(prf_env.prf[i].id);
        if(cbs != NULL)
        {
            // call cleanup callback
            cbs->cleanup(&(prf_env.prf[i]), conidx, reason);
        }
    }
}


prf_env_t* prf_env_get(uint16_t prf_id)
{
    prf_env_t* env = NULL;
    uint8_t i;
    // find if profile present in profile tasks
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        // check if profile identifier is known
        if(prf_env.prf[i].id == prf_id)
        {
            env = prf_env.prf[i].env;
            break;
        }
    }

    return env;
}

ke_task_id_t prf_src_task_get(prf_env_t* env, uint8_t conidx)
{
    ke_task_id_t task = PERM_GET(env->prf_task, PRF_TASK);

    if(PERM_GET(env->prf_task, PRF_MI))
    {
        task = KE_BUILD_ID(task, conidx);
    }

    return task;
}

ke_task_id_t prf_dst_task_get(prf_env_t* env, uint8_t conidx)
{
    ke_task_id_t task = PERM_GET(env->app_task, PRF_TASK);

    if(PERM_GET(env->app_task, PRF_MI))
    {
        task = KE_BUILD_ID(task, conidx);
    }

    return task;
}


ke_task_id_t prf_get_id_from_task_func(ke_msg_id_t task)
{
    ke_task_id_t id = TASK_ID_INVALID;
    uint8_t idx = KE_IDX_GET(task);
    uint8_t i;
    task = KE_TYPE_GET(task);

    // find if profile present in profile tasks
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        // check if profile identifier is known
        if(prf_env.prf[i].task == task)
        {
            id = prf_env.prf[i].id;
            break;
        }
    }

    return KE_BUILD_ID(id, idx);
}

ke_task_id_t prf_get_task_from_id_func(ke_msg_id_t id)
{
    ke_task_id_t task = TASK_NONE;
    uint8_t idx = KE_IDX_GET(id);
    uint8_t i;
    id = KE_TYPE_GET(id);

    // find if profile present in profile tasks
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        // check if profile identifier is known
        if(prf_env.prf[i].id == id)
        {
            task = prf_env.prf[i].task;
            break;
        }
    }

    return KE_BUILD_ID(task, idx);
}

void prf_reset_func(ke_task_id_t task_id, uint8_t conidx)
{
    uint8_t i;
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        // Get Profile API
        if (prf_env.prf[i].id == task_id)
        {
            const struct prf_task_cbs * cbs = prf_itf_get(prf_env.prf[i].id);
            if(cbs != NULL)
            {
                // call cleanup callback
                cbs->cleanup(&(prf_env.prf[i]), conidx, 0);
                // call create callback
                cbs->create(&(prf_env.prf[i]), conidx);
            }
            break;
        }
    }
}

#else

const struct profile_interface prf_if[] = {
#if (BLE_HT_THERMOM)
    {TASK_ID_HTPT,    (prfif_func_t)htpt_prf_itf_get},
#endif // (BLE_HT_THERMOM)

#if (BLE_HT_COLLECTOR)
    {TASK_ID_HTPC,    (prfif_func_t)htpc_prf_itf_get},
#endif // (BLE_HT_COLLECTOR)

#if (BLE_DIS_SERVER)
    {TASK_ID_DISS,    (prfif_func_t)diss_prf_itf_get},
#endif // (BLE_DIS_SERVER)

#if (BLE_DIS_CLIENT)
    {TASK_ID_DISC,    (prfif_func_t)disc_prf_itf_get},
#endif // (BLE_DIS_CLIENT)

#if (BLE_BP_SENSOR)
    {TASK_ID_BLPS,    (prfif_func_t)blps_prf_itf_get},
#endif // (BLE_BP_SENSOR)

#if (BLE_BP_COLLECTOR)
    {TASK_ID_BLPC,    (prfif_func_t)blpc_prf_itf_get},
#endif // (BLE_BP_COLLECTOR)

#if (BLE_TIP_SERVER)
    {TASK_ID_TIPS,    (prfif_func_t)tips_prf_itf_get},
#endif // (BLE_TIP_SERVER)

#if (BLE_TIP_CLIENT)
    {TASK_ID_TIPC,    (prfif_func_t)tipc_prf_itf_get},
#endif // (BLE_TIP_CLIENT)

#if (BLE_HR_SENSOR)
    {TASK_ID_HRPS,    (prfif_func_t)hrps_prf_itf_get},
#endif // (BLE_HR_SENSOR)

#if (BLE_HR_COLLECTOR)
    {TASK_ID_HRPC,    (prfif_func_t)hrpc_prf_itf_get},
#endif // (BLE_HR_COLLECTOR)

#if (BLE_FINDME_LOCATOR)
    {TASK_ID_FINDL,    (prfif_func_t)findl_prf_itf_get},
#endif // (BLE_FINDME_LOCATOR)

#if (BLE_FINDME_TARGET)
    {TASK_ID_FINDT,    (prfif_func_t)findt_prf_itf_get},
#endif // (BLE_FINDME_TARGET)

#if (BLE_PROX_MONITOR)
    {TASK_ID_PROXM,    (prfif_func_t)proxm_prf_itf_get},
#endif // (BLE_PROX_MONITOR)

#if (BLE_PROX_REPORTER)
    {TASK_ID_PROXR,   (prfif_func_t)proxr_prf_itf_get},
#endif
#if (BLE_SP_SERVER)
    {TASK_ID_SCPPS,   (prfif_func_t)scpps_prf_itf_get},
#endif // (BLE_SP_SERVER)

#if (BLE_SP_CLIENT)
    {TASK_ID_SCPPC,   (prfif_func_t)scppc_prf_itf_get},
#endif // (BLE_SP_CLIENT)

#if (BLE_BATT_SERVER)
    {TASK_ID_BASS,    (prfif_func_t)bass_prf_itf_get},
#endif
 #if (BLE_BATT_CLIENT)
    {TASK_ID_BASC,    (prfif_func_t)basc_prf_itf_get},
#endif // (BLE_BATT_CLIENT)

#if (BLE_HID_DEVICE)
    {TASK_ID_HOGPD,    (prfif_func_t)hogpd_prf_itf_get},
#endif // (BLE_HID_DEVICE)

#if (BLE_HID_BOOT_HOST)
    {TASK_ID_HOGPBH,    (prfif_func_t)hogpbh_prf_itf_get},
#endif // (BLE_HID_BOOT_HOST)

#if (BLE_HID_REPORT_HOST)
    {TASK_ID_HOGPRH,    (prfif_func_t)hogprh_prf_itf_get},
#endif // (BLE_HID_REPORT_HOST)

#if (BLE_GL_COLLECTOR)
    {TASK_ID_GLPC,    (prfif_func_t)glpc_prf_itf_get},
#endif // (BLE_GL_COLLECTOR)

#if (BLE_GL_SENSOR)
    {TASK_ID_GLPS,    (prfif_func_t)glps_prf_itf_get},
#endif // (BLE_GL_SENSOR)

#if (BLE_RSC_COLLECTOR)
    {TASK_ID_RSCPC,    (prfif_func_t)rscpc_prf_itf_get},
#endif // (BLE_RSC_COLLECTOR)

#if (BLE_RSC_SENSOR)
    {TASK_ID_RSCPS,    (prfif_func_t)rscps_prf_itf_get},
#endif // (BLE_RSC_SENSOR)

#if (BLE_CSC_COLLECTOR)
    {TASK_ID_CSCPC,    (prfif_func_t)cscpc_prf_itf_get},
#endif // (BLE_CSC_COLLECTOR)

#if (BLE_CSC_SENSOR)
    {TASK_ID_CSCPS,    (prfif_func_t)cscps_prf_itf_get},
#endif // (BLE_CSC_SENSOR)

#if (BLE_CP_COLLECTOR)
    {TASK_ID_CPPC,    (prfif_func_t)cppc_prf_itf_get},
#endif // (BLE_CP_COLLECTOR)

#if (BLE_CP_SENSOR)
    {TASK_ID_CPPS,    (prfif_func_t)cpps_prf_itf_get},
#endif // (BLE_CP_SENSOR)

#if (BLE_LN_COLLECTOR)
    {TASK_ID_LANC,    (prfif_func_t)lanc_prf_itf_get},
#endif // (BLE_LN_COLLECTOR)

#if (BLE_LN_SENSOR)
    {TASK_ID_LANS,    (prfif_func_t)lans_prf_itf_get},
#endif // (BLE_LN_SENSOR)

#if (BLE_AN_CLIENT)
    {TASK_ID_ANPC,    (prfif_func_t)anpc_prf_itf_get},
#endif // (BLE_AN_CLIENT)

#if (BLE_AN_SERVER)
    {TASK_ID_ANPS,    (prfif_func_t)anps_prf_itf_get},
#endif // (BLE_AN_SERVER)

#if (BLE_PAS_CLIENT)
    {TASK_ID_PASPC,    (prfif_func_t)paspc_prf_itf_get},
#endif // (BLE_PAS_CLIENT)

#if (BLE_PAS_SERVER)
    {TASK_ID_PASPS,    (prfif_func_t)pasps_prf_itf_get},
#endif // (BLE_PAS_SERVER)

#if (BLE_BMS_CLIENT)
    {TASK_ID_BMSC,    (prfif_func_t)bmsc_prf_itf_get},
#endif // (BLE_BMS_CLIENT)

#if (BLE_BMS_SERVER)
    {TASK_ID_BMSS,    (prfif_func_t)bmss_prf_itf_get},
#endif // (BLE_BMS_SERVER)

#if (BLE_CTS_SERVER)
    {TASK_ID_CTSS,    (prfif_func_t)ctss_prf_itf_get},
#endif // (BLE_CTS_SERVER)

#if (BLE_CTS_CLIENT)
    {TASK_ID_CTSC,    (prfif_func_t)ctsc_prf_itf_get},
#endif // (BLE_CTS_CLIENT)

#if (BLE_WSS_COLLECTOR)
    {TASK_ID_WSSC,    (prfif_func_t)wssc_prf_itf_get},
#endif // (BLE_WSS_COLLECTOR)

#if (BLE_WSS_SERVER)
    {TASK_ID_WSSS,    (prfif_func_t)wsss_prf_itf_get},
#endif // (BLE_WSS_SERVER)

#if (BLE_BCS_CLIENT)
    {TASK_ID_BCSC,    (prfif_func_t)bcsc_prf_itf_get},
#endif // (BLE_BCS_CLIENT)

#if (BLE_BCS_SERVER)
    {TASK_ID_BCSS,    (prfif_func_t)bcss_prf_itf_get},
#endif // (BLE_BCS_SERVER)

#if (BLE_UDS_CLIENT)
    {TASK_ID_UDSC,    (prfif_func_t)udsc_prf_itf_get},
#endif // (BLE_UDS_CLIENT)

#if (BLE_UDS_SERVER)
    {TASK_ID_UDSS,    (prfif_func_t)udss_prf_itf_get},
#endif // (BLE_UDS_SERVER)

#if (BLE_GATT_CLIENT)
    {TASK_ID_GATT_CLIENT,    (prfif_func_t)gatt_client_prf_itf_get},
#endif // (BLE_GATT_CLIENT)

#if (BLE_SUOTA_RECEIVER)
    {TASK_ID_SUOTAR,  (prfif_func_t)suotar_prf_itf_get},
#endif

#if (BLE_CUSTOM1_SERVER)
    {TASK_ID_CUSTS1,  (prfif_func_t)custs1_prf_itf_get},
#endif

#if (BLE_CUSTOM2_SERVER)
    {TASK_ID_CUSTS2,    (prfif_func_t)custs2_prf_itf_get},
#endif // (BLE_PAS_SERVER)

#if (BLE_ANC_CLIENT)
    {TASK_ID_ANCC,    (prfif_func_t)ancc_prf_itf_get},
#endif // (BLE_ANC_CLIENT)

    {TASK_ID_INVALID, NULL}
};

#if (BLE_SERVER_PRF || BLE_CLIENT_PRF)
#define BLE_NB_USED_PROFILES    ((sizeof(prf_if) / sizeof(struct profile_interface)) - 1)

/// Profile Manager environment structure
struct prf_env_tag
{
    /// Array of profile tasks that can be managed by Profile manager.
    struct prf_task_env prf[BLE_NB_USED_PROFILES];
};

struct prf_env_tag prf_env      __SECTION_ZERO("retention_mem_area0");

const rom_prf_cfg_t rom_prf_cfg =
{
    prf_if,                     // pointer to table defining profile id and prf_itf_get() function per profile in use
    &prf_env,                   // pointer to profile environment
    BLE_NB_USED_PROFILES,       // number of BLE profiles per application
};

#else
#define BLE_NB_USED_PROFILES    (1)

const rom_prf_cfg_t rom_prf_cfg =
{
    NULL,                       // pointer to table defining profile id and prf_itf_get() function per profile in use
    NULL,                       // pointer to profile environment
    0,                          // number of BLE profiles per application
};

#endif

#endif

#endif // (BLE_PROFILES)

/// @} PRF
