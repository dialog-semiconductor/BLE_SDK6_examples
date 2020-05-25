/**
 ****************************************************************************************
 *
 * @file user_social_distancing.c
 *
 * @brief Social distancing software example source code.
 *
 * Copyright (c) 2015-2020 Dialog Semiconductor. All rights reserved.
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
#include "app_mid.h"
#include "app_user_config.h"
#include "app_easy_timer.h"
#include "user_social_distancing.h"
#include "user_custs1_def.h"
#include "co_bt.h"
#include "arch_console.h"
#include "user_led_alert.h"
#include "custs1_task.h"
#include "gattc_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define USER_CON_INTV           30  //BLE Connection Interval in ms
#define USER_CON_RSSI_MAX_NB    4   //Maximum number of RSSI measurements

#define RSSI_VAL_HANDLE         27  //Handle of RSSI value characteristic

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint8_t app_connection_idx                      __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
static int8_t rssi_con_value                    __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

timer_hnd user_switch_adv_scan_timer            __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd user_poll_conn_rssi_timer             __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd user_initiator_timer                  __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd user_disconnect_to_timer              __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

static const int8_t user_prox_zones_rssi[USER_PROX_ZONE_COUNT] = {-59, -62, -70, -80}; 

static struct user_adv_rssi_node* user_adv_rep_rssi_head  __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

struct scan_configuration {
    /// Operation code.
    uint8_t     code;
    /// Own BD address source of the device
    uint8_t     addr_src;
    /// Scan interval
    uint16_t    interval;
    /// Scan window size
    uint16_t    window;
    /// Scanning mode
    uint8_t     mode;
    /// Scan filter policy
    uint8_t     filt_policy;
    /// Scan duplicate filtering policy
    uint8_t     filter_duplic;
};

static const struct scan_configuration user_scan_conf ={
    /// Operation code.
    .code = GAPM_SCAN_PASSIVE,
    /// Own BD address source of the device
    .addr_src = GAPM_STATIC_ADDR,
    /// Scan interval
    .interval = (USER_ADVERTISING_INT_MAX * 3) / 2,
    /// Scan window size
    .window = USER_ADVERTISING_INT_MIN/3,
    /// Scanning mode
    .mode = GAP_GEN_DISCOVERY,
    /// Scan filter policy
    .filt_policy = SCAN_ALLOW_ADV_ALL,
    /// Scan duplicate filtering policy
    .filter_duplic = SCAN_FILT_DUPLIC_EN
};

static const uint8_t user_custom_srv_uuid[] = DEF_SVC1_UUID_128;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief Creates a node of an advertising report in the dynamic list
 * @return A pointer to the node created
 ****************************************************************************************
 */

static struct user_adv_rssi_node* user_adv_rssi_create_node()
{
    if (ke_check_malloc(sizeof(struct user_adv_rssi_node), KE_MEM_ENV))
    {
        struct user_adv_rssi_node* temp;
        
        temp = (struct user_adv_rssi_node *)ke_malloc(sizeof(struct user_adv_rssi_node), KE_MEM_ENV);
        memset(&temp->adv_addr, 0, sizeof(temp->adv_addr));
        memset(&temp->adv_addr_type, 0, sizeof(temp->adv_addr_type));
        memset(&temp->count, 0, sizeof(temp->count));
        memset(&temp->mean_rssi, 0, sizeof(temp->mean_rssi));
        temp->accessed = false;
        temp->next = NULL;
        
        return temp;
    }
    
    return NULL;
}

/**
 ****************************************************************************************
 * @brief Adds a node of an advertising report in the dynamic list
 * @param[in] adv_report The advertising report to be added as a node
 * @note If the advertising device exists in the list, it increments its count
 * @return void
 ****************************************************************************************
 */
static void user_adv_rssi_add_node_rssi(struct gapm_adv_report_ind const * adv_report)
{
    struct user_adv_rssi_node* p;
    struct user_adv_rssi_node* temp;
    
    if (user_adv_rep_rssi_head == NULL)
    {
        temp = user_adv_rssi_create_node();
        if (temp != NULL)
        {
            // Copy advertising report to node and initialize
            memcpy(&temp->adv_addr_type, &adv_report->report.adv_addr_type, sizeof(temp->adv_addr_type));
            memcpy(&temp->adv_addr, &adv_report->report.adv_addr, sizeof(temp->adv_addr));
            memcpy(&temp->mean_rssi, &adv_report->report.rssi, sizeof(temp->mean_rssi));
            temp->accessed = false;
            (temp->count)++;
            temp->next = NULL;
            
            user_adv_rep_rssi_head = temp;
        }
    }
    else
    {
        p = user_adv_rep_rssi_head;
        while(memcmp(&p->adv_addr, &adv_report->report.adv_addr, sizeof(p->adv_addr))
              || p->adv_addr_type != adv_report->report.adv_addr_type) 
        {
            if (p->next != NULL)
                p = p->next;
            else
                break;
        }
        
        if (!memcmp(&p->adv_addr, &adv_report->report.adv_addr, sizeof(p->adv_addr))
            && p->adv_addr_type == adv_report->report.adv_addr_type)
        {
            p->mean_rssi = ((uint32_t)p->count * (uint32_t)p->mean_rssi + adv_report->report.rssi) / (p->count + 1);
            (p->count)++;
        }
        else // This is a new node
        {
            temp = user_adv_rssi_create_node();
            if (temp != NULL)
            {
                // Copy advertising report to node and initialize
                memcpy(&temp->adv_addr_type, &adv_report->report.adv_addr_type, sizeof(temp->adv_addr_type));
                memcpy(&temp->adv_addr, &adv_report->report.adv_addr, sizeof(temp->adv_addr));
                memcpy(&temp->mean_rssi, &adv_report->report.rssi, sizeof(temp->mean_rssi));
                temp->accessed = false;
                (temp->count)++;
                temp->next = NULL;
                
                p->next = temp;
            }
        }
    }
}
/**
 ****************************************************************************************
 * @brief Traverses the list and prints out its nodes
 * @return void
 ****************************************************************************************
 */
static void user_adv_rssi_print_list()
{
    struct user_adv_rssi_node* p;

    p = user_adv_rep_rssi_head;
    while (p != NULL) 
    {
        arch_printf("\r\n List entry:\t");
        arch_printf("%02x %02x %02x %02x %02x %02x\t",
        p->adv_addr.addr[5],
        p->adv_addr.addr[4],
        p->adv_addr.addr[3],
        p->adv_addr.addr[2],
        p->adv_addr.addr[1],
        p->adv_addr.addr[0]);
        arch_printf("RSSI: %d\t", (int8_t)p->mean_rssi);
        arch_printf("Accessed: %d\t", p->accessed? 1: 0);
        arch_printf("Count: %d\t", (int8_t)p->count);
     
        p = p->next;
    }
}

/**
 ****************************************************************************************
 * @brief Retrieve the node with the maximum RSSI value
 * @return A pointer to the maximum RSSI node
 ****************************************************************************************
 */
static struct user_adv_rssi_node* user_adv_rssi_get_max_rssi_node()
{
    struct user_adv_rssi_node* p;
    struct user_adv_rssi_node* ret = NULL;

    int8_t max_rssi = -128;     // Choose minimum negative value.
    
    p = user_adv_rep_rssi_head;
    while (p != NULL) 
    {
        if ((int8_t)p->mean_rssi > max_rssi && !(p->accessed))
        {
            max_rssi = (int8_t)p->mean_rssi;
            ret = p;
        }
        
        p = p->next;
    }
    
    return ret;
}

/**
 ****************************************************************************************
 * @brief Checks the list for a connection candidate
 * @return true if there is a connection candidate, false otherwise
 ****************************************************************************************
 */
static bool user_adv_rssi_list_has_candidate()
{
    struct user_adv_rssi_node* p;
    
    p = user_adv_rep_rssi_head;
    while (p != NULL) 
    {
        if ((int8_t)p->mean_rssi > user_prox_zones_rssi[USER_PROX_ZONE_COARSE] && !(p->accessed))
        {
            return true;
        }
        
        p = p->next;
    }
    
    return false;
}

/**
 ****************************************************************************************
 * @brief Clears the list and frees up the allocated heap
 * @return void
 ****************************************************************************************
 */
static void user_adv_rssi_list_clear()
{
    struct user_adv_rssi_node* p;
    
    p = user_adv_rep_rssi_head;
    while (p != NULL) 
    {
        struct user_adv_rssi_node* next;
        
        next = p->next;
        ke_free(p);
        p = next;
    }
    
    user_adv_rep_rssi_head = NULL;
}


/**
 ****************************************************************************************
 * @brief Performs a write operation to the peer device
 * @param[in] rssi The RSSI value to be written
 * @return void
 ****************************************************************************************
 */
static void perform_rssi_write_to_peer(uint8_t rssi)
{  
    struct gattc_write_cmd *wr_char = KE_MSG_ALLOC_DYN(GATTC_WRITE_CMD,
                KE_BUILD_ID(TASK_GATTC, 0), KE_BUILD_ID(TASK_APP, 0),
                gattc_write_cmd, sizeof(uint8_t));
    
    // Offset
    wr_char->offset         = 0x0000;
    // cursor always 0
    wr_char->cursor         = 0x0000;
    // Write Type
    wr_char->operation      = GATTC_WRITE;
    // Characteristic Value attribute handle
    wr_char->handle         = RSSI_VAL_HANDLE;
    // Value Length
    wr_char->length         = sizeof(uint8_t);
    // Auto Execute
    wr_char->auto_execute   = true;
    // Value
    memcpy(&wr_char->value[0], &rssi, sizeof(uint8_t));

    // Send the message
    ke_msg_send(wr_char);
    
}

/**
 ****************************************************************************************
 * @brief Collects RSSI values and decides the proximity zone
 * @param[in] rssi_val The current RSSI value
 * @return void
 ****************************************************************************************
 */
static void rssi_write_ind_handler(ke_msg_id_t const msgid,
                                      struct custs1_val_write_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    if(rssi_con_value < (int8_t) param->value[0])
        rssi_con_value = (int8_t) param->value[0]; 
}

/**
 ****************************************************************************************
 * @brief Start scanning for devices 
 * @return void
 ****************************************************************************************
 */
static void user_scan_start(void)
{
    struct gapm_start_scan_cmd* cmd = KE_MSG_ALLOC(GAPM_START_SCAN_CMD,
                                                   TASK_GAPM,
                                                   TASK_APP,
                                                   gapm_start_scan_cmd);

    cmd->op.code = user_scan_conf.code;
    cmd->op.addr_src = user_scan_conf.addr_src;
    cmd->interval = user_scan_conf.interval;
    cmd->window = user_scan_conf.window;
    cmd->mode = user_scan_conf.mode;
    cmd->filt_policy = user_scan_conf.filt_policy;
    cmd->filter_duplic =user_scan_conf.filter_duplic;

    // Send the message
    ke_msg_send(cmd);

    // We are now connectable
    ke_state_set(TASK_APP, APP_CONNECTABLE);
}

/**
 ****************************************************************************************
 * @brief Timer callback function to stop advertising
 * @return void
 ****************************************************************************************
 */
static void user_switch_adv_scan_timer_cb()
{
    user_switch_adv_scan_timer = EASY_TIMER_INVALID_TIMER;

    app_easy_gap_advertise_stop(); 
}

/**
 ****************************************************************************************
 * @brief Timer callback function to poll the connection RSSI
 * @return void
 ****************************************************************************************
 */
static void user_poll_conn_rssi_timer_cb()
{
    if (ke_state_get(TASK_APP) == APP_CONNECTED)
    {
        struct gapc_get_info_cmd *pkt = KE_MSG_ALLOC(GAPC_GET_INFO_CMD,
                                                    KE_BUILD_ID(TASK_GAPC, app_connection_idx),
                                                    TASK_APP, gapc_get_info_cmd);

        pkt->operation = GAPC_GET_CON_RSSI;
        ke_msg_send(pkt);
    }
    
    user_poll_conn_rssi_timer = app_easy_timer(USER_UPD_CONN_RSSI_TO, user_poll_conn_rssi_timer_cb);
}

/**
 ****************************************************************************************
 * @brief Timer callback function to disconnect in case a connection request times out
 * @return void
 ****************************************************************************************
 */
static void user_disconnect_to_timer_cb()
{
    struct gapm_cancel_cmd *cmd = app_gapm_cancel_msg_create();
    // Send the message
    app_gapm_cancel_msg_send(cmd);

    user_disconnect_to_timer = EASY_TIMER_INVALID_TIMER;
}

/**
 ****************************************************************************************
 * @brief Timer callback function to initiate connections with peer devices
 * @return void
 ****************************************************************************************
 */
static void user_initiator_timer_cb()
{
    struct user_adv_rssi_node* p;
    
    p = user_adv_rssi_get_max_rssi_node(); // Change get max rssi to CHECK FOR ACCESSED

    user_adv_rssi_print_list();
    
    if (p != NULL && (ke_state_get(TASK_APP) == APP_CONNECTABLE))
    {
       
        user_initiator_timer = app_easy_timer(USER_INITIATOR_TO, user_initiator_timer_cb);
        user_disconnect_to_timer = app_easy_timer(USER_DISCONNECT_TO_TO, user_disconnect_to_timer_cb);
        
        p->accessed = true;
        
        app_easy_gap_start_connection_to_set(p->adv_addr_type, (uint8_t *)&p->adv_addr.addr, MS_TO_DOUBLESLOTS(USER_CON_INTV));
        app_easy_gap_start_connection_to();
    }
    else if (ke_state_get(TASK_APP) == APP_CONNECTED)
    {
        user_initiator_timer = app_easy_timer(USER_INITIATOR_TO, user_initiator_timer_cb);
        app_easy_gap_disconnect(app_connection_idx);
    }        
    else
    {
        user_initiator_timer = EASY_TIMER_INVALID_TIMER;
        user_adv_rssi_list_clear();
        user_adv_rssi_print_list();

        user_app_adv_start();
    }
}

/**
 ****************************************************************************************
 * @brief Collects RSSI values and decides the proximity zone
 * @param[in] rssi_val The current RSSI value
 * @return void
 ****************************************************************************************
 */
static void user_collect_conn_rssi(uint8_t rssi_val)
{
    static uint8_t idx;

    if (idx < USER_CON_RSSI_MAX_NB)
    {
        if (rssi_con_value < (int8_t) rssi_val)
            rssi_con_value = (int8_t) rssi_val;
        idx++;
    }
    else
    {      
        arch_printf("\r\n Maximum connection RSSI:%d", rssi_con_value);
        
        idx = 0;
        
        if (rssi_con_value > user_prox_zones_rssi[USER_PROX_ZONE_DANGER])
        {
            arch_printf("\r\nIn danger zone");
            alert_user_start(DANGER_ZONE);
        }
        else if (rssi_con_value > user_prox_zones_rssi[USER_PROX_ZONE_WARNING])
        {
            arch_printf("\r\nIn warning zone");
            alert_user_start(WARNING_ZONE);
        }
        else if (rssi_con_value > user_prox_zones_rssi[USER_PROX_ZONE_COARSE])
        {   
            arch_printf("\r\nIn coarse zone");
            alert_user_start(COARSE_ZONE);
        }
        
        if (user_poll_conn_rssi_timer != EASY_TIMER_INVALID_TIMER)
        {
            app_easy_timer_cancel(user_poll_conn_rssi_timer);
        }
        
        rssi_con_value = -128;
        
        user_poll_conn_rssi_timer = EASY_TIMER_INVALID_TIMER;
        
        app_easy_gap_disconnect(app_connection_idx);
    }
        
}

/*
 * SDK configured callback functions
 ****************************************************************************************
 */


void user_app_init(void)
{
    user_switch_adv_scan_timer = EASY_TIMER_INVALID_TIMER;
    user_initiator_timer = EASY_TIMER_INVALID_TIMER;
    user_disconnect_to_timer = EASY_TIMER_INVALID_TIMER;
    
    rssi_con_value = -128;
    
    alert_user_init();
    
    default_app_on_init();
}

void user_app_on_scanning_completed(const uint8_t param)
{
    bool has_conn_candidate;
    struct user_adv_rssi_node* p;
    
    // Disable Scanning
    struct gapm_cancel_cmd *cmd = app_gapm_cancel_msg_create();
    // Send the message
    app_gapm_cancel_msg_send(cmd);
    
    p = user_adv_rssi_get_max_rssi_node();
    
    if (p == NULL)
        arch_printf("\r\n No nodes stored");
    else
    {
        arch_printf("\r\n Strongest node RSSI: %d", (int8_t)p->mean_rssi);
    }
    
    has_conn_candidate = user_adv_rssi_list_has_candidate();
    
    if (has_conn_candidate)
    {
        user_initiator_timer = app_easy_timer(USER_INITIATOR_TO, user_initiator_timer_cb);
    }
    else
        user_app_adv_start();
}

void user_app_adv_start(void)
{
    int8_t rand_val = (int8_t) co_rand_byte();
    user_switch_adv_scan_timer = app_easy_timer(USER_SWITCH_ADV_SCAN_TO + rand_val, user_switch_adv_scan_timer_cb);

    app_easy_gap_undirected_advertise_start();
}

void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;
      
        if(user_switch_adv_scan_timer != EASY_TIMER_INVALID_TIMER)
        {    
            app_easy_timer_cancel(user_switch_adv_scan_timer);
            user_switch_adv_scan_timer = EASY_TIMER_INVALID_TIMER;
        }
        
        if(user_disconnect_to_timer != EASY_TIMER_INVALID_TIMER)
        {      
            app_easy_timer_cancel(user_disconnect_to_timer);
            user_disconnect_to_timer = EASY_TIMER_INVALID_TIMER;
        }
        
        if (user_poll_conn_rssi_timer == EASY_TIMER_INVALID_TIMER)
        {
            user_poll_conn_rssi_timer = app_easy_timer(USER_UPD_CONN_RSSI_TO, user_poll_conn_rssi_timer_cb);
        }
    }
    else
    {
        // No connection has been established, restart advertising
        user_app_adv_start();
    }

    default_app_on_connection(connection_idx, param);
}

void user_app_adv_undirect_complete(uint8_t status)
{
    if (status == GAP_ERR_CANCELED)
    {
        if (user_switch_adv_scan_timer != EASY_TIMER_INVALID_TIMER)
        {
            app_easy_timer_cancel(user_switch_adv_scan_timer);
            user_switch_adv_scan_timer = EASY_TIMER_INVALID_TIMER;
        }
        user_scan_start();
    }
}

void user_app_disconnect(struct gapc_disconnect_ind const *param)
{  
    ke_state_set(TASK_APP, APP_CONNECTABLE);
    alert_user_stop();
    
    // Restart Advertising
    user_app_adv_start();
}

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
            
            switch(msg_param->handle)
            {
                case SVC1_IDX_REMOTE_WRITE_VAL:
                    rssi_write_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;
            }
        } break;   
        case GAPC_CON_RSSI_IND:
        {
            struct gapc_con_rssi_ind const *msg_param = (struct gapc_con_rssi_ind const *)(param);
            
            // Write the received RSSI to the peer device
            perform_rssi_write_to_peer((int8_t) msg_param->rssi);
            
            user_collect_conn_rssi(msg_param->rssi);
        } break;
        case GATTC_CMP_EVT:
        {
            //Placeholder upon GATT Complete Event
        } break;
        
        default:
            break;
    }
}

void user_app_on_adv_report_ind(struct gapm_adv_report_ind const * param)
{
    uint8_t report_data[ADV_DATA_LEN + 1];
    
    memcpy(report_data, param->report.data, param->report.data_len);
    report_data[param->report.data_len] = '\0';
    
    if(!memcmp(report_data + 5, user_custom_srv_uuid, ATT_UUID_128_LEN))
    {
        // Populate advertiser report list
        user_adv_rssi_add_node_rssi(param);
        user_adv_rssi_print_list();
    }
}
/// @} APP
