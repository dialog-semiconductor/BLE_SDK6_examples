/**
 ****************************************************************************************
 *
 * \file app_hogpd.c
 *
 * \brief Keyboard (HID) over GATT Application entry point.
 *
 * Copyright (C) 2020-2021 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/**
 * \addtogroup USER
 * \{
 * \addtogroup PROFILE
 * \{
 * \addtogroup APP_HOGPD
 *
 * \{
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration

#if (BLE_HID_DEVICE)

#include <user_hogpd_config.h>
#include "app_hogpd.h"
#include "prf_utils.h"
//#include "port_platform.h"
#include "app_prf_perm_types.h"
#include "arch_console.h"

uint16_t report_ntf;
uint8_t hogpd_conidx;

#define REPORT_TO_MASK(index) (HOGPD_CFG_REPORT_NTF_EN << index)

#if HID_NUM_OF_REPORTS > HOGPD_NB_REPORT_INST_MAX
    #error "Maximum munber of HID reports exceeded. Please increase HOGPD_NB_REPORT_INST_MAX"
#endif

/**
 ****************************************************************************************
 * @brief Enables the HOGPD profile 
 *
 * @param   None
 *
 * @return  void
 ****************************************************************************************
 */
void app_hogpd_enable(uint8_t conidx)
{   
    // Allocate the message

	//arch_printf("app_hogpd_enable\n");
    struct hogpd_enable_req * req = KE_MSG_ALLOC(HOGPD_ENABLE_REQ, prf_get_task_from_id(TASK_ID_HOGPD), 
                                                 TASK_APP,
                                                 hogpd_enable_req);
    
    hogpd_conidx = conidx;
    // Fill in the parameter structure
    req->conidx = hogpd_conidx;


    report_ntf = 0;
	//report_ntf = 1;


//#ifdef ALWAYS_ENALBE_REPORTS   
    int i;
    for (i=0; i<HID_NUM_OF_REPORTS; i++) {
        if((hogpd_reports[i].cfg & HOGPD_CFG_REPORT_IN) == HOGPD_CFG_REPORT_IN) {
            report_ntf |= REPORT_TO_MASK(i); 
        }
    }        
//#endif


    req->ntf_cfg[0] = report_ntf;

    // Send the message
    ke_msg_send(req);    
}

/**
 ****************************************************************************************
 * \brief Creates the HID over GATT database
 *
 * \param   None
 ****************************************************************************************
 */
void app_hogpd_create_db(void)
{   
    struct hogpd_db_cfg* db_cfg;

		arch_printf("app_hogpd_create_db\n");

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM, 
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd, 
                                                             sizeof(struct hogpd_db_cfg));
 
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->prf_task_id = TASK_ID_HOGPD;
    req->app_task = TASK_APP;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_HOGPD);
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct hogpd_db_cfg*) req->param;
    struct hogpd_hids_cfg *cfg = &db_cfg->cfg[0];              

    db_cfg->hids_nb = 1;
    
    struct hids_hid_info *hid_info = &cfg->hid_info;        

    cfg->svc_features |= HOGPD_CFG_BOOT_KB_WR;

    cfg->report_nb = HID_NUM_OF_REPORTS;

    uint8_t i;
    for(i = 0; i < HID_NUM_OF_REPORTS; i++) {
        cfg->report_id[i]=hogpd_reports[i].id;
        cfg->report_char_cfg[i]=hogpd_reports[i].cfg;
    }


	for(i = HID_NUM_OF_REPORTS; i< HOGPD_NB_REPORT_INST_MAX;i++)
	{
		cfg->report_id[i]=0;
        cfg->report_char_cfg[i]=0;
	}
    
    hid_info->bcdHID = 0x100;
    hid_info->bCountryCode = 0;
	hid_info->flags = HIDS_REMOTE_WAKE_CAPABLE; 
	
    ke_msg_send(req);      
}

bool app_hogpd_send_report(uint8_t report_idx, uint8_t *data, uint16_t length, enum hogpd_report_type type)
{
    struct hogpd_report_upd_req *req;
    
    // Allocate the message
    req = KE_MSG_ALLOC_DYN(HOGPD_REPORT_UPD_REQ, 
                           prf_get_task_from_id(TASK_ID_HOGPD), 
                           TASK_APP, 
                           hogpd_report_upd_req, 
                           length);
    
    
    if (!req) {
        return false;
    }

    req->conidx = hogpd_conidx;

    struct hogpd_report_info *report = &req->report;
    
    // Fill in the parameter structure
    // TODO: find the index from the report number
    report->hid_idx = 0;
    
    ASSERT_ERROR((type != HOGPD_BOOT_KEYBOARD_INPUT_REPORT && type != HOGPD_BOOT_MOUSE_INPUT_REPORT) || report_idx == 0 );
    ASSERT_ERROR((type != HOGPD_BOOT_KEYBOARD_INPUT_REPORT && type != HOGPD_BOOT_MOUSE_INPUT_REPORT) || length <= HOGPD_BOOT_REPORT_MAX_LEN);
    ASSERT_ERROR((type == HOGPD_BOOT_KEYBOARD_INPUT_REPORT || type == HOGPD_BOOT_MOUSE_INPUT_REPORT) || length <= HOGPD_REPORT_MAX_LEN);

    report->type = type;
    report->idx =  report_idx;
    
    report->length = length;
    memcpy(report->value, data, length);            
    
    ke_msg_send(req);    
    
    return true;
}

uint8_t app_hogpd_get_protocol_mode(void)
{
    struct hogpd_env_tag* hogpd_env = PRF_ENV_GET(HOGPD, hogpd);
    return hogpd_env->svcs[0].proto_mode;
}

uint16_t app_hogpd_report_handle(uint8_t report_nb)
{
    ASSERT_WARNING(report_nb<HOGPD_NB_REPORT_INST_MAX);
    struct hogpd_env_tag* hogpd_env = PRF_ENV_GET(HOGPD, hogpd);
    return hogpd_get_att_handle(hogpd_env, 0, HOGPD_IDX_REPORT_VAL, report_nb);    
}

#endif

/**
 * \}
 * \}
 * \}
 */
