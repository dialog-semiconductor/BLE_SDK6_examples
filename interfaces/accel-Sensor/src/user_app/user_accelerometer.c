/**
 ****************************************************************************************
 *
 * @file user_accelerometer.c
 *
 * @brief Accelerometer project source code.
 *
 * Copyright (c) 2012-2021 Dialog Semiconductor
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration
#include "gap.h"
#include "app_easy_timer.h"
#include "user_accelerometer.h"
#include "co_bt.h"
#include "gpio.h"
#include "app_api.h"
#include "app.h"
#include "prf_utils.h"
#include "custs1.h"
#include "user_custs1_def.h"
#include "user_periph_setup.h"
#include "user_ADXL345.h"
#include "custs1_task.h"
#include <math.h>

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
#define X_AXIS_BITFLAG      (1 << 0)
#define Y_AXIS_BITFLAG      (1 << 1)
#define Z_AXIS_BITFLAG      (1 << 2)
#define G_DATA_BITFLAG      (1 << 3)

// Manufacturer Specific Data ADV structure type
struct mnf_specific_data_ad_structure
{
    uint8_t ad_structure_size;
    uint8_t ad_structure_type;
    uint8_t company_id[APP_AD_MSD_COMPANY_ID_LEN];
    uint8_t proprietary_data[APP_AD_MSD_DATA_LEN];
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint8_t app_connection_idx                      __attribute__((section(".bss."))); //@RETENTION MEMORY
timer_hnd app_param_update_request_timer_used   __attribute__((section(".bss."))); //@RETENTION MEMORY

struct mnf_specific_data_ad_structure mnf_data  __attribute__((section(".bss."))); //@RETENTION MEMORY
// Index of manufacturer data in advertising data or scan response data (when MSB is 1)
uint8_t mnf_data_index                          __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t stored_adv_data_len                     __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data_len                __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t stored_adv_data[ADV_DATA_LEN]           __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data[SCAN_RSP_DATA_LEN] __attribute__((section(".bss."))); //@RETENTION MEMORY

// Timer set for pushing notifications for the ntf enabled characteristics
timer_hnd axis_ntf_timer_handler                __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
// Bit field variable for holding the enabled notification subscriptions
uint8_t axis_active_subscriptions               __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
// Bit field variable for holding the pending confirmations for the released notifications
uint8_t axis_ntf_cfm_pending                    __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief Initialize Manufacturer Specific Data
 * @return void
 ****************************************************************************************
 */
static void mnf_data_init()
{
    mnf_data.ad_structure_size = sizeof(struct mnf_specific_data_ad_structure ) - sizeof(uint8_t); // minus the size of the ad_structure_size field
    mnf_data.ad_structure_type = GAP_AD_TYPE_MANU_SPECIFIC_DATA;
    mnf_data.company_id[0] = APP_AD_MSD_COMPANY_ID & 0xFF; // LSB
    mnf_data.company_id[1] = (APP_AD_MSD_COMPANY_ID >> 8 )& 0xFF; // MSB
    mnf_data.proprietary_data[0] = 0;
    mnf_data.proprietary_data[1] = 0;
}

/**
 ****************************************************************************************
 * @brief Update Manufacturer Specific Data
 * @return void
 ****************************************************************************************
 */
static void mnf_data_update()
{    
    ADXL345_read_XYZ(mnf_data.proprietary_data);
}

/**
 ****************************************************************************************
 * @brief Add an AD structure in the Advertising or Scan Response Data of the 
  *       GAPM_START_ADVERTISE_CMD parameter struct.
 * @param[in] cmd               GAPM_START_ADVERTISE_CMD parameter struct
 * @param[in] ad_struct_data    AD structure buffer
 * @param[in] ad_struct_len     AD structure length
 * @param[in] adv_connectable   Connectable advertising event or not. It controls whether 
 *                              the advertising data use the full 31 bytes length or only 
 *                              28 bytes (Document CCSv6 - Part 1.3 Flags). 
 * @return void
 */
static void app_add_ad_struct(struct gapm_start_advertise_cmd *cmd, void *ad_struct_data, uint8_t ad_struct_len, uint8_t adv_connectable)
{
    uint8_t adv_data_max_size = (adv_connectable) ? (ADV_DATA_LEN - 3) : (ADV_DATA_LEN);
    
    if ((adv_data_max_size - cmd->info.host.adv_data_len) >= ad_struct_len)
    {
        // Append manufacturer data to advertising data
        memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len], ad_struct_data, ad_struct_len);

        // Update Advertising Data Length
        cmd->info.host.adv_data_len += ad_struct_len;
        
        // Store index of manufacturer data which are included in the advertising data
        mnf_data_index = cmd->info.host.adv_data_len - sizeof(struct mnf_specific_data_ad_structure);
    }
    else if ((SCAN_RSP_DATA_LEN - cmd->info.host.scan_rsp_data_len) >= ad_struct_len)
    {
        // Append manufacturer data to scan response data
        memcpy(&cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len], ad_struct_data, ad_struct_len);

        // Update Scan Response Data Length
        cmd->info.host.scan_rsp_data_len += ad_struct_len;
        
        // Store index of manufacturer data which are included in the scan response data
        mnf_data_index = cmd->info.host.scan_rsp_data_len - sizeof(struct mnf_specific_data_ad_structure);
        // Mark that manufacturer data is in scan response and not advertising data
        mnf_data_index |= 0x80;
    }
    else
    {
        // Manufacturer Specific Data do not fit in either Advertising Data or Scan Response Data
        ASSERT_WARNING(0);
    }
    // Store advertising data length
    stored_adv_data_len = cmd->info.host.adv_data_len;
    // Store advertising data
    memcpy(stored_adv_data, cmd->info.host.adv_data, stored_adv_data_len);
    // Store scan response data length
    stored_scan_rsp_data_len = cmd->info.host.scan_rsp_data_len;
    // Store scan_response data
    memcpy(stored_scan_rsp_data, cmd->info.host.scan_rsp_data, stored_scan_rsp_data_len);
}

/**
 ****************************************************************************************
 * @brief Advertisement data update timer callback function.
 * @return void
 ****************************************************************************************
*/
void adv_data_update_timer_cb(void)
{
    if(ke_state_get(TASK_APP) == APP_CONNECTABLE)
    {
        // If mnd_data_index has MSB set, manufacturer data is stored in scan response
        uint8_t *mnf_data_storage = (mnf_data_index & 0x80) ? stored_scan_rsp_data : stored_adv_data;

        // Update manufacturer data
        mnf_data_update();

        // Update the selected fields of the advertising data (manufacturer data)
        memcpy(mnf_data_storage + (mnf_data_index & 0x7F), &mnf_data, sizeof(struct mnf_specific_data_ad_structure));

        // Update advertising data on the fly
        app_easy_gap_update_adv_data(stored_adv_data, stored_adv_data_len, stored_scan_rsp_data, stored_scan_rsp_data_len);
    }
}

/**
 ****************************************************************************************
 * @brief Parameter update request timer callback function.
 * @return void
 ****************************************************************************************
*/
static void param_update_request_timer_cb()
{
    app_easy_gap_param_update_start(app_connection_idx);
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
}

/**
 **************************************************************************************************
 * @brief User defined initialization function
 * @return void
 **************************************************************************************************
 */
void user_app_init(void)
{
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    
    // Initialize Manufacturer Specific Data
    mnf_data_init();
    
    // Initialize Advertising and Scan Response Data
    memcpy(stored_adv_data, USER_ADVERTISE_DATA, USER_ADVERTISE_DATA_LEN);
    stored_adv_data_len = USER_ADVERTISE_DATA_LEN;
    memcpy(stored_scan_rsp_data, USER_ADVERTISE_SCAN_RESPONSE_DATA, USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN);
    stored_scan_rsp_data_len = USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN;
    
    axis_ntf_timer_handler = EASY_TIMER_INVALID_TIMER;
    
    default_app_on_init();
}

/**
 **************************************************************************************************
 * @brief User defined function which which will be called when the device starts advertising
 * @return void
 **************************************************************************************************
 */
void user_app_adv_start(void)
{   
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();
    
    // Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);

    app_easy_gap_undirected_advertise_start();
}

/**
 **************************************************************************************************
 * @brief User defined function which which will be called when the device establishes a connection
 * @param[in] connection_idx Connection ID number
 * @param[in] param          Pointer to GAPC_CONNECTION_REQ_IND message
 * @return void
 **************************************************************************************************
 */
void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;

        // Check if the parameters of the established connection are the preferred ones.
        // If not then schedule a connection parameter update request.
        if ((param->con_interval < user_connection_param_conf.intv_min) ||
            (param->con_interval > user_connection_param_conf.intv_max) ||
            (param->con_latency != user_connection_param_conf.latency) ||
            (param->sup_to != user_connection_param_conf.time_out))
        {
            // Connection params are not these that we expect
            app_param_update_request_timer_used = app_easy_timer(APP_PARAM_UPDATE_REQUEST_TO, param_update_request_timer_cb);
        }
    }
    else
    {
        // No connection has been established, restart advertising
        user_app_adv_start();
    }

    default_app_on_connection(connection_idx, param);
}

/**
 *********************************************************************************************
 * @brief User defined function to handle the completion of undirected advertisement.
 * @param[in] status    Status code
 * @return void
 *********************************************************************************************
 */
void user_app_adv_undirect_complete(uint8_t status)
{
    // If advertising was canceled then update advertising data and start advertising again
    if (status == GAP_ERR_CANCELED)
    {
        user_app_adv_start();
    }
}

/**
 *********************************************************************************************
 * @brief User defined function which which will be called when the device closes a connection
 * @param[in] param Pointer to GAPC_DISCONNECT_IND message
 * @return void
 *********************************************************************************************
 */
void user_app_disconnect(struct gapc_disconnect_ind const *param)
{
    // Cancel the parameter update request timer
    if (app_param_update_request_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_param_update_request_timer_used);
        app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    }
    
    // Stop the notification timer
    if(axis_ntf_timer_handler != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(axis_ntf_timer_handler);
        axis_ntf_timer_handler = EASY_TIMER_INVALID_TIMER;
    }
			
    // Update manufacturer data for the next advertsing event
    mnf_data_update();
    // Restart Advertising
    user_app_adv_start();
}

/**
 ****************************************************************************************
 * @brief Helper function to convert a raw measurement to a string.
 * @param[in]  input   Input raw measurement value
 * @param[out] s       Pointer to the output string
 * @return Length of string
 ****************************************************************************************
 */
static uint8_t user_int_to_string(int16_t input, uint8_t *s){
	uint8_t length = 1;
	if(input < 0){
		s[0] = '-';
	} else {
		s[0] = ' ';
	}
	input = abs(input);
	if(input  >= 10000){
		s[length++] = '0' + ((input / 10000) % 10);
	}
	if(input  >= 1000){
		s[length++] = '0' + ((input / 1000) % 10);
	}
	if(input  >= 100){
		s[length++] = '0' + ((input / 100) % 10);
	}
	if(input  >= 10){
		s[length++] = '0' + ((input / 10) % 10);
	}
	
	s[length++] = '0' + (input% 10);
	return length;
}

/**
 ****************************************************************************************
 * @brief Set/Clear that for a specific characteristic the application waits for a 
 *        notification confirmation for the notification that was just send.
 * @param[in] the characteristic that has a pending notification
 * @param[in] set or clear the pending flag for the characteristic
 * @return void
 ****************************************************************************************
 */
static void ntf_send_set_clear_pending(uint8_t ntf, bool clear)
{
    clear ? (axis_ntf_cfm_pending &= ~ntf) : (axis_ntf_cfm_pending |= ntf);
}

/**
 ****************************************************************************************
 * @brief Send a notification for the 3 available axis data.
 * @return void
 ****************************************************************************************
 */
static void user_svc1_accel_data_send_ntf(int16_t axis_data, uint16_t handle)
{
    uint8_t axis_val[8];
    //Construct the string to send as a notification
    uint8_t string_length = user_int_to_string(axis_data * 3.9, axis_val);    //Read data and multiply by 3.9 to get acceleration in mg
    
    struct custs1_val_ntf_ind_req* req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                      prf_get_task_from_id(TASK_ID_CUSTS1),
                                                      TASK_APP,
                                                      custs1_val_ntf_ind_req,
                                                      string_length);
    // Initialize message fields
    req->conidx = 0;
    req->notification = true;
    req->handle = handle;
    req->length = string_length;
    memcpy(req->value, axis_val, string_length);
    
    //Send the message to the task
    ke_msg_send(req);
}

/**
 ****************************************************************************************
 * @brief Send a notification for the for the g data.
 * @return void
 ****************************************************************************************
 */
static void user_svc2_g_data_send_ntf(void)
{
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC2_G_DATA_CHAR_LEN);
    uint8_t xyz[DEF_SVC2_G_DATA_CHAR_LEN];
    
    //Read 3-axes measurement
    ADXL345_read_XYZ(xyz);
    
    //Initialize message fields
    req->handle = SVC2_IDX_G_DATA_VAL;
    req->length = DEF_SVC2_G_DATA_CHAR_LEN;
    req->notification = true;
    memcpy(req->value, xyz, DEF_SVC2_G_DATA_CHAR_LEN);
    
    //Send the message to the task
    ke_msg_send(req);
}

/**
 ****************************************************************************************
 * @brief Timer handler for the main timer responsible for the notification sending.
 * @return void
 ****************************************************************************************
 */
static void send_notifications_timer_handler(void)
{
    if((axis_active_subscriptions & X_AXIS_BITFLAG) && !(axis_ntf_cfm_pending & X_AXIS_BITFLAG))
    {
        user_svc1_accel_data_send_ntf(ADXL345_read_X(), SVC1_IDX_ACCEL_X_DATA_VAL);
        ntf_send_set_clear_pending(X_AXIS_BITFLAG, false);
    }
    if(axis_active_subscriptions & Y_AXIS_BITFLAG && !(axis_ntf_cfm_pending & Y_AXIS_BITFLAG))
    {
        user_svc1_accel_data_send_ntf(ADXL345_read_Y(), SVC1_IDX_ACCEL_Y_DATA_VAL);
        ntf_send_set_clear_pending(Y_AXIS_BITFLAG, false);
    }
    if(axis_active_subscriptions & Z_AXIS_BITFLAG && !(axis_ntf_cfm_pending & Z_AXIS_BITFLAG))
    {
        user_svc1_accel_data_send_ntf(ADXL345_read_Z(), SVC1_IDX_ACCEL_Z_DATA_VAL);
        ntf_send_set_clear_pending(Z_AXIS_BITFLAG, false);
    }
    if(axis_active_subscriptions & G_DATA_BITFLAG && !(axis_ntf_cfm_pending & G_DATA_BITFLAG))
    {
        user_svc2_g_data_send_ntf();
        ntf_send_set_clear_pending(G_DATA_BITFLAG, false);
    }
    // Restart the timer
    axis_ntf_timer_handler = app_easy_timer(APP_NTF_DATA_SEND_INTV, send_notifications_timer_handler);
}

/**
 ****************************************************************************************
 * @brief Function checks if the value received is valid for enabling subscription if at
 *        least one subscription is enabled the timer is running.
 * @param[in] param   pointer to the struct that holds the received data
 * @return void
 ****************************************************************************************
 */
static void user_individual_axis_subscr_ntf_handler(struct custs1_val_write_ind const *param)
{    
    if(param->handle == SVC1_IDX_ACCEL_X_NTF_CFG)
        co_read16p(param->value) == PRF_CLI_START_NTF ? (axis_active_subscriptions |= X_AXIS_BITFLAG) : (axis_active_subscriptions &= ~X_AXIS_BITFLAG);
    
    if(param->handle == SVC1_IDX_ACCEL_Y_NTF_CFG)
        co_read16p(param->value) == PRF_CLI_START_NTF ? (axis_active_subscriptions |= Y_AXIS_BITFLAG) : (axis_active_subscriptions &= ~Y_AXIS_BITFLAG);
    
    if(param->handle == SVC1_IDX_ACCEL_Z_NTF_CFG)
        co_read16p(param->value) == PRF_CLI_START_NTF ? (axis_active_subscriptions |= Z_AXIS_BITFLAG) : (axis_active_subscriptions &= ~Z_AXIS_BITFLAG);
    
    if(param->handle == SVC2_IDX_G_NTF_CFG)
        co_read16p(param->value) == PRF_CLI_START_NTF ? (axis_active_subscriptions |= G_DATA_BITFLAG) : (axis_active_subscriptions &= ~G_DATA_BITFLAG);
    
    // If still there are no active subscriptions cancel the timer
    if(axis_active_subscriptions == 0 && axis_ntf_timer_handler != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(axis_ntf_timer_handler);
        axis_ntf_timer_handler = EASY_TIMER_INVALID_TIMER;
    }
    else if (axis_active_subscriptions != 0 && axis_ntf_timer_handler == EASY_TIMER_INVALID_TIMER)
        axis_ntf_timer_handler = app_easy_timer(APP_NTF_DATA_SEND_INTV, send_notifications_timer_handler);
}

/**
 ****************************************************************************************
 * @brief User defined function which processes Write Notifications.
 * @param[in] msgid   Type of the message
 * @param[in] param   Pointer to the message to be processed
 * @param[in] dest_id Destination task id
 * @param[in] src_id  Source task id
 * @return void
 ****************************************************************************************
 */
void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msgid)
    {		
		case CUSTS1_VAL_WRITE_IND:
        {
            struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

            switch (msg_param->handle)
            {
                case SVC1_IDX_ACCEL_X_NTF_CFG:
                case SVC1_IDX_ACCEL_Y_NTF_CFG:
                case SVC1_IDX_ACCEL_Z_NTF_CFG:
                case SVC2_IDX_G_NTF_CFG:
                    user_individual_axis_subscr_ntf_handler(msg_param);
                    break;
                default:
                    break;
            }
        } break;
        
        case CUSTS1_VAL_NTF_CFM:
        {
            struct custs1_val_ntf_cfm const *msg_param = (struct custs1_val_ntf_cfm const *)(param);
            
            switch (msg_param->handle)
            {
                case SVC1_IDX_ACCEL_X_DATA_VAL:
                    ntf_send_set_clear_pending(X_AXIS_BITFLAG, true);
                    break;
                case SVC1_IDX_ACCEL_Y_DATA_VAL:
                    ntf_send_set_clear_pending(Y_AXIS_BITFLAG, true);
                    break;
                case SVC1_IDX_ACCEL_Z_DATA_VAL:
                    ntf_send_set_clear_pending(Z_AXIS_BITFLAG, true);
                    break;
                case SVC2_IDX_G_DATA_VAL:
                    ntf_send_set_clear_pending(G_DATA_BITFLAG, true);
                    break;
                default:
                    break;
            }
        } break;
        
        default:
            break;
    }
}

/// @} APP
