/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.c
 *
 * @brief Peripheral project Custom1 Server implementation source code.
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gpio.h"
#include "app_api.h"
#include "app.h"
#include "prf_utils.h"
#include "custs1.h"
#include "custs1_task.h"
#include "user_custs1_def.h"
#include "user_custs1_impl.h"
#include "user_peripheral.h"
#include "user_periph_setup.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

ke_msg_id_t timer_used      __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint16_t indication_counter __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint16_t non_db_val_counter __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void user_svc1_ctrl_wr_ind_handler(ke_msg_id_t const msgid,
                                      struct custs1_val_write_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    uint8_t val = 0;
    memcpy(&val, &param->value[0], param->length);

    if (val != CUSTS1_CP_ADC_VAL1_DISABLE)
    {
        timer_used = app_easy_timer(APP_PERIPHERAL_CTRL_TIMER_DELAY, app_adcval1_timer_cb_handler);
    }
    else
    {
        if (timer_used != EASY_TIMER_INVALID_TIMER)
        {
            app_easy_timer_cancel(timer_used);
            timer_used = EASY_TIMER_INVALID_TIMER;
        }
    }
}

void user_svc1_led_wr_ind_handler(ke_msg_id_t const msgid,
                                     struct custs1_val_write_ind const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    uint8_t val = 0;
    memcpy(&val, &param->value[0], param->length);

    if (val == CUSTS1_LED_ON)
    {
        GPIO_SetActive(GPIO_LED_PORT, GPIO_LED_PIN);
    }
    else if (val == CUSTS1_LED_OFF)
    {
        GPIO_SetInactive(GPIO_LED_PORT, GPIO_LED_PIN);
    }
}

void user_svc1_long_val_cfg_ind_handler(ke_msg_id_t const msgid,
                                           struct custs1_val_write_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    // Generate indication when the central subscribes to it
    if (param->value[0])
    {
        uint8_t conidx = KE_IDX_GET(src_id);

        struct custs1_val_ind_req* req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_IND_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ind_req,
                                                          sizeof(indication_counter));

        req->conidx = app_env[conidx].conidx;
        req->handle = SVC1_IDX_INDICATEABLE_VAL;
        req->length = sizeof(indication_counter);
        req->value[0] = (indication_counter >> 8) & 0xFF;
        req->value[1] = indication_counter & 0xFF;

        indication_counter++;

        ke_msg_send(req);
    }
}

void user_svc1_long_val_wr_ind_handler(ke_msg_id_t const msgid,
                                          struct custs1_val_write_ind const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
}

void user_svc1_long_val_ntf_cfm_handler(ke_msg_id_t const msgid,
                                           struct custs1_val_write_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
}

void user_svc1_adc_val_1_cfg_ind_handler(ke_msg_id_t const msgid,
                                            struct custs1_val_write_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
}

void user_svc1_adc_val_1_ntf_cfm_handler(ke_msg_id_t const msgid,
                                            struct custs1_val_write_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
}

void user_svc1_button_cfg_ind_handler(ke_msg_id_t const msgid,
                                         struct custs1_val_write_ind const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
}

void user_svc1_button_ntf_cfm_handler(ke_msg_id_t const msgid,
                                         struct custs1_val_write_ind const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
}

void user_svc1_indicateable_cfg_ind_handler(ke_msg_id_t const msgid,
                                               struct custs1_val_write_ind const *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id)
{
}

void user_svc1_indicateable_ind_cfm_handler(ke_msg_id_t const msgid,
                                               struct custs1_val_write_ind const *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id)
{
}

void user_svc1_long_val_att_info_req_handler(ke_msg_id_t const msgid,
                                                struct custs1_att_info_req const *param,
                                                ke_task_id_t const dest_id,
                                                ke_task_id_t const src_id)
{
    struct custs1_att_info_rsp *rsp = KE_MSG_ALLOC(CUSTS1_ATT_INFO_RSP,
                                                   src_id,
                                                   dest_id,
                                                   custs1_att_info_rsp);
    // Provide the connection index.
    rsp->conidx  = app_env[param->conidx].conidx;
    // Provide the attribute index.
    rsp->att_idx = param->att_idx;
    // Provide the current length of the attribute.
    rsp->length  = 0;
    // Provide the ATT error code.
    rsp->status  = ATT_ERR_NO_ERROR;

    ke_msg_send(rsp);
}

void user_svc1_rest_att_info_req_handler(ke_msg_id_t const msgid,
                                            struct custs1_att_info_req const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    struct custs1_att_info_rsp *rsp = KE_MSG_ALLOC(CUSTS1_ATT_INFO_RSP,
                                                   src_id,
                                                   dest_id,
                                                   custs1_att_info_rsp);
    // Provide the connection index.
    rsp->conidx  = app_env[param->conidx].conidx;
    // Provide the attribute index.
    rsp->att_idx = param->att_idx;
    // Force current length to zero.
    rsp->length  = 0;
    // Provide the ATT error code.
    rsp->status  = ATT_ERR_WRITE_NOT_PERMITTED;

    ke_msg_send(rsp);
}

void app_adcval1_timer_cb_handler()
{
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_ADC_VAL_1_CHAR_LEN);

    // ADC value to be sampled
    static uint16_t sample      __SECTION_ZERO("retention_mem_area0");
    sample = (sample <= 0xffff) ? (sample + 1) : 0;

    //req->conhdl = app_env->conhdl;
    req->handle = SVC1_IDX_ADC_VAL_1_VAL;
    req->length = DEF_SVC1_ADC_VAL_1_CHAR_LEN;
    req->notification = true;
    memcpy(req->value, &sample, DEF_SVC1_ADC_VAL_1_CHAR_LEN);

    ke_msg_send(req);

    if (ke_state_get(TASK_APP) == APP_CONNECTED)
    {
        // Set it once again until Stop command is received in Control Characteristic
        timer_used = app_easy_timer(APP_PERIPHERAL_CTRL_TIMER_DELAY, app_adcval1_timer_cb_handler);
    }
}

void user_svc3_read_non_db_val_handler(ke_msg_id_t const msgid,
                                           struct custs1_value_req_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    // Increase value by one
    non_db_val_counter++;

    struct custs1_value_req_rsp *rsp = KE_MSG_ALLOC_DYN(CUSTS1_VALUE_REQ_RSP,
                                                        prf_get_task_from_id(TASK_ID_CUSTS1),
                                                        TASK_APP,
                                                        custs1_value_req_rsp,
                                                        DEF_SVC3_READ_VAL_4_CHAR_LEN);

    // Provide the connection index.
    rsp->conidx  = app_env[param->conidx].conidx;
    // Provide the attribute index.
    rsp->att_idx = param->att_idx;
    // Force current length to zero.
    rsp->length  = sizeof(non_db_val_counter);
    // Provide the ATT error code.
    rsp->status  = ATT_ERR_NO_ERROR;
    // Copy value
    memcpy(&rsp->value, &non_db_val_counter, rsp->length);
    // Send message
    ke_msg_send(rsp);
}
