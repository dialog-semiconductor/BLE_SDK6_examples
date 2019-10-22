/**
 ****************************************************************************************
 *
 * @file app_ancc.h
 *
 * @brief Apple Notification Center Service Client Application entry point.
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
 *
 ****************************************************************************************
 */

#ifndef APP_ANCC_H_
#define APP_ANCC_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief Apple Notification Center Service Client Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_ANC_CLIENT)

#include "anc_common.h"
#include "ancc.h"

/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */

struct app_ancc_cb
{
    void (*on_ancc_enable)(uint8_t conidx, uint8_t status, const struct ancc_content *anc);
    void (*on_wr_cfg_ntf_src_rsp)(uint8_t condix, uint8_t status);
    void (*on_wr_cfg_data_src_rsp)(uint8_t condix, uint8_t status);
    void (*on_rd_cfg_ntf_src_rsp)(uint8_t condix, uint8_t status, bool enabled);
    void (*on_rd_cfg_data_src_rsp)(uint8_t condix, uint8_t status, bool enabled);
    void (*on_ntf_src_ind)(uint8_t conidx, const struct anc_ntf_src *ntf);
    void (*on_ntf_att_ind)(uint8_t conidx, uint32_t uid, uint8_t att_id, uint8_t *val);
    void (*on_app_att_ind)(uint8_t conidx, uint8_t att_id, uint8_t *app_id, uint8_t *att_val);
    void (*on_get_ntf_atts_cmd_cmp)(uint8_t condix, uint8_t status);
    void (*on_get_app_atts_cmd_cmp)(uint8_t condix, uint8_t status);
    void (*on_perf_ntf_act_cmd_cmp)(uint8_t condix, uint8_t status);
};

/*
 ****************************************************************************************
 * @brief Initialize ANC Service Application
 * @return void
 ****************************************************************************************
 */
void app_ancc_init(void);

/**
 ****************************************************************************************
 * @brief Add an ANC Service instance to the DB
 * @return void
 ****************************************************************************************
 */
void app_ancc_create_db(void);

/**
 ****************************************************************************************
 * @brief Enable ANC Service client
 * @param[in] conidx            The connection id number
 * @return void
 ****************************************************************************************
 */
void app_ancc_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Write Notification Source configuration
 * @param[in] conidx            The connection id number
 * @param[in] enable            Notification enable or disable
 * @return void
 ****************************************************************************************
 */
void app_ancc_wr_cfg_ntf_src(uint8_t conidx, bool enable);

/**
 ****************************************************************************************
 * @brief Write Data Source configuration
 * @param[in] conidx            The connection id number
 * @param[in] enable            Notification enable or disable
 * @return void
 ****************************************************************************************
 */
void app_ancc_wr_cfg_data_src(uint8_t conidx, bool enable);

/**
 ****************************************************************************************
 * @brief Read Notification Source configuration
 * @param[in] conidx            The connection id number
 * @return void
 ****************************************************************************************
 */
void app_ancc_rd_cfg_ntf_src(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Read Data Source configuration
 * @param[in] conidx            The connection id number
 * @return void
 ****************************************************************************************
 */
void app_ancc_rd_cfg_data_src(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Execute Get notification attributes command
 * @param[in] conidx            The connection id number
 * @param[in] uid               Notification UID
 * @param[in] atts              Requested attributes mask
 * @param[in] title_len         Length of Title attribute
 * @param[in] sub_len           Length of Subtitle attribute
 * @param[in] msg_len           Length of Message attribute
 * @return void
 ****************************************************************************************
 */
void app_ancc_get_ntf_atts(uint8_t conidx, uint32_t uid, uint8_t atts, uint16_t title_len,
                               uint16_t sub_len, uint16_t msg_len);

/**
 ****************************************************************************************
 * @brief Execute Get application attributes command
 * @param[in] conidx            The connection id number
 * @param[in] atts              Requested attributes mask
 * @param[in] app_id            Application ID - must be null terminated
 * @return void
 ****************************************************************************************
 */
void app_ancc_get_app_atts(uint8_t conidx, uint8_t atts, uint8_t *app_id);

/**
 ****************************************************************************************
 * @brief Execute Get application attributes command
 * @param[in] conidx            The connection id number
 * @param[in] uid               Notification UID
 * @param[in] act_postitive     True if action should be positivie
 * @return void
 ****************************************************************************************
 */
void app_ancc_ntf_action(uint8_t conidx, uint32_t uid, bool act_positive);


#endif // (BLE_ANC_CLIENT)

/// @} APP

#endif // APP_ANCC_H_
