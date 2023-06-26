/**
 ****************************************************************************************
 *
 * \file app_hogpd_task.c
 *
 * \brief HID Keyboard handlers.
 *
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
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

#include "rwip_config.h"               // SW configuration

#if (BLE_HID_DEVICE)

#include "app_hogpd_task.h"
#include <user_hogpd_config.h>
#include "app_entry_point.h"

extern uint16_t report_ntf;

#define REPORT_MAP_LEN sizeof(report_map)
    

/**
 ****************************************************************************************
 * \brief   
 ****************************************************************************************
 */
static void app_hogpd_report_upd_rsp_handler(void const *param)
{
    struct hogpd_report_upd_rsp *par = (struct hogpd_report_upd_rsp *)param;
    
    //Clear pending ack's for param->report_nb == 0 (normal key report) and == 2 (ext. key report)
    switch (par->status) {
        case PRF_ERR_UNEXPECTED_LEN:
            break;
        case PRF_ERR_NTF_DISABLED:
            break;
        case PRF_ERR_REQ_DISALLOWED:
            break;
        case PRF_ERR_INVALID_PARAM:
            break;
        case PRF_ERR_FEATURE_NOT_SUPPORTED:
            break;
    }        
}

__INLINE void app_hogpd_call_store_attibute_callback(uint16_t uuid, int attr_num, int value)
{
    //if(hogpd_params.store_attribute_callback !=NULL) {
    //    hogpd_params.store_attribute_callback(uuid, attr_num, value);
    //}
}


/**
 ****************************************************************************************
 * \brief   
 ****************************************************************************************
 */
static void app_hogpd_proto_mode_ind_handler(void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct hogpd_proto_mode_req_ind *par = (struct hogpd_proto_mode_req_ind *)param;
        
    if (hogpd_params.boot_protocol_mode) {
        app_hogpd_call_store_attibute_callback(ATT_CHAR_PROTOCOL_MODE, 0, par->proto_mode);
    } else if (par->proto_mode == HOGP_BOOT_PROTOCOL_MODE) {
        ASSERT_WARNING(0);
    }
    
    struct hogpd_proto_mode_cfm *prot_cfm = KE_MSG_ALLOC(HOGPD_PROTO_MODE_CFM, src_id, dest_id,
                                                         hogpd_proto_mode_cfm);
    prot_cfm->conidx = par->conidx;
    prot_cfm->status = GAP_ERR_NO_ERROR;
    prot_cfm->hid_idx = par->hid_idx;
    prot_cfm->proto_mode = par->proto_mode;
    ke_msg_send(prot_cfm);
}


/**
 ****************************************************************************************
 * \brief   
 ****************************************************************************************
 */
static void app_hogpd_ntf_config_ind_handler(void const *param)
{
    struct hogpd_ntf_cfg_ind *ind = (struct hogpd_ntf_cfg_ind *)param;
    
    uint16_t i;
    
    uint16_t new_ntf = ind->ntf_cfg[0];
    uint16_t ntf_xor = new_ntf ^ report_ntf;
    
    
    if(ntf_xor & 0x01) {
        app_hogpd_call_store_attibute_callback(ATT_CHAR_BOOT_KB_IN_REPORT, 0, new_ntf & 0x01);
    }
    
    uint8_t index=9;
    
    for (i=0x8000; i!=0x0020; i >>= 1 ) {
        if(ntf_xor & i) {
            app_hogpd_call_store_attibute_callback(ATT_CHAR_REPORT, index, (new_ntf & i) ? 1 : 0);
        }
        index--;
    }
    report_ntf = new_ntf;
}

/**
 ****************************************************************************************
 * \brief   
 ****************************************************************************************
 */
static void app_hogpd_ctnl_pt_ind_handler(void const *param)
{
    struct hogpd_ctnl_pt_ind *ind = (struct hogpd_ctnl_pt_ind *)param;
    
    app_hogpd_call_store_attibute_callback(ATT_CHAR_HID_CTNL_PT, 0, ind->hid_ctnl_pt);
}

/**
 ****************************************************************************************
 * \brief   
 ****************************************************************************************
 */
static void app_hogpd_report_ind_handler(void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct hogpd_report_req_ind *par = (struct hogpd_report_req_ind *)param;
    uint8_t dummy_data[] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    uint8_t i;
    uint16_t report_length = 0;
    uint8_t *report_data = NULL;
    
    if(par->operation == HOGPD_OP_REPORT_READ) {        
        switch(par->report.type) {
        case HOGPD_REPORT:
            for(i = 0; i < HID_NUM_OF_REPORTS; i++) {
                if( i == par->report.idx ) {
                    report_data = dummy_data;
                    report_length = hogpd_reports[par->report.idx].size;
                    if(hogpd_reports[i].read_callback != NULL) {
                        (hogpd_reports[i].read_callback)(KE_IDX_GET(src_id), report_data, &report_length);
                    }
                    break;
                }
            }            
            break;
        case HOGPD_REPORT_MAP:
							report_length = REPORT_MAP_LEN;
							report_data = (uint8_t *)report_map;
            break;
        case HOGPD_BOOT_KEYBOARD_INPUT_REPORT:
        case HOGPD_BOOT_KEYBOARD_OUTPUT_REPORT:
        default: 
            break;            
        }                
    }
    
    if(par->operation == HOGPD_OP_REPORT_WRITE && par->report.type == HOGPD_REPORT) {
        for(i = 0; i < HID_NUM_OF_REPORTS; i++) {
            if( i == par->report.idx ) {
                if(hogpd_reports[i].write_callback != NULL) {
                    (hogpd_reports[i].write_callback)(KE_IDX_GET(src_id), &par->report);
                }
                break;
            }
        }            
    }
    
    struct hogpd_report_cfm * cfm = KE_MSG_ALLOC_DYN(HOGPD_REPORT_CFM,
                                                src_id, dest_id,
                                                hogpd_report_cfm,
                                                report_length);

    cfm->conidx         = KE_IDX_GET(src_id);
    cfm->status         = ATT_ERR_NO_ERROR;
    cfm->operation      = par->operation;
    cfm->report.hid_idx = par->report.hid_idx;
    cfm->report.idx     = par->report.idx;
    cfm->report.type    = par->report.type;

    cfm->report.length = report_length;
    memcpy(cfm->report.value, report_data, report_length);
    ke_msg_send(cfm);
}

enum process_event_response app_hogpd_process_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    switch(msgid) {
    case HOGPD_REPORT_UPD_RSP:
        app_hogpd_report_upd_rsp_handler(param);
        break;
    case HOGPD_PROTO_MODE_REQ_IND:
        app_hogpd_proto_mode_ind_handler(param, dest_id, src_id);
        break;
    case HOGPD_NTF_CFG_IND:
        app_hogpd_ntf_config_ind_handler(param);
        break;
    case HOGPD_CTNL_PT_IND:
        app_hogpd_ctnl_pt_ind_handler(param);
        break;
    case HOGPD_REPORT_REQ_IND:
        app_hogpd_report_ind_handler(param, dest_id, src_id);
        break;
    default:
        return PR_EVENT_UNHANDLED;
    }
    
    return PR_EVENT_HANDLED;
}

#endif

/**
 * \}
 * \}
 * \}
 */