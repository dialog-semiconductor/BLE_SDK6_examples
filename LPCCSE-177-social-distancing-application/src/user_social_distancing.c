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

/*
 * DEFINES
 ****************************************************************************************
 */
extern struct mem_usage_log heap_usage_env;


#define USER_CON_INTV           30  //ms
#define USER_CON_RSSI_MAX_NB    4

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint8_t app_connection_idx                      __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
bool user_is_advertiser                         __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

timer_hnd app_param_update_request_timer_used   __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

timer_hnd user_switch_adv_scan_timer            __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd user_poll_conn_rssi_timer             __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd user_initiator_timer                  __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd user_disconnect_to_timer              __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

static bool is_user_connected                   __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

static const int8_t user_prox_zones_rssi[USER_PROX_ZONE_COUNT] = {-30, -40, -50, -60}; 

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

static void user_adv_rssi_add_node_rssi(struct gapm_adv_report_ind const * adv_report)
{
    struct user_adv_rssi_node* p;
    struct user_adv_rssi_node* temp;
    
    temp = user_adv_rssi_create_node();
    if (temp != NULL)
    {
        memcpy(&temp->adv_addr_type, &adv_report->report.adv_addr_type, sizeof(temp->adv_addr_type));
        memcpy(&temp->adv_addr, &adv_report->report.adv_addr, sizeof(temp->adv_addr));
        memcpy(&temp->mean_rssi, &adv_report->report.rssi, sizeof(temp->mean_rssi));
        temp->accessed = false;
        (temp->count)++;
        temp->next = NULL;
        
        if (user_adv_rep_rssi_head == NULL) // HEAD is NULL
        {
            user_adv_rep_rssi_head = temp;
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
            else
            {
                p->next = temp;
            }
        }
    }
}

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

static struct user_adv_rssi_node* user_adv_rssi_get_max_rssi_node()
{
    struct user_adv_rssi_node* p;
    struct user_adv_rssi_node* ret = NULL;

    int8_t max_rssi = -128;     // Choose minimum negative value. Could be optimized later if needed.
    
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
        arch_printf("\r\nIn list clear, memory is: %d", heap_usage_env.used_sz);
    }
    
    user_adv_rep_rssi_head = NULL;
}

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

static void param_update_request_timer_cb()
{
    app_easy_gap_param_update_start(app_connection_idx);
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
}

static void user_switch_adv_scan_timer_cb()
{
    user_switch_adv_scan_timer = EASY_TIMER_INVALID_TIMER;

    app_easy_gap_advertise_stop(); 
}

static void user_poll_conn_rssi_timer_cb()
{
    if (ke_state_get(TASK_APP) == APP_CONNECTED)
    {
    struct gapc_get_info_cmd *pkt = KE_MSG_ALLOC(GAPC_GET_INFO_CMD,
                                                KE_BUILD_ID(TASK_GAPC, app_connection_idx),
                                                TASK_APP, gapc_get_info_cmd);

    pkt->operation = GAPC_GET_CON_RSSI;
    ke_msg_send(pkt);
    
    arch_printf("\r\nOn RSSI timer callback: Set RSSI timer");
    }
    user_poll_conn_rssi_timer = app_easy_timer(USER_UPD_CONN_RSSI_TO, user_poll_conn_rssi_timer_cb);
}

static void user_disconnect_to_timer_cb()
{
    struct gapm_cancel_cmd *cmd = app_gapm_cancel_msg_create();
    // Send the message
    app_gapm_cancel_msg_send(cmd);
    
    
    arch_printf("\r\nDisconnect timer called");

    user_disconnect_to_timer = EASY_TIMER_INVALID_TIMER;
}

static void user_initiator_timer_cb()
{
    struct user_adv_rssi_node* p;
    
    p = user_adv_rssi_get_max_rssi_node(); // Change get max rssi to CHECK FOR ACCESSED

    arch_printf("\r\nOn initiate:");
    user_adv_rssi_print_list();
    
    if (p != NULL && (ke_state_get(TASK_APP) == APP_CONNECTABLE))
    {
       
        user_initiator_timer = app_easy_timer(USER_INITIATOR_TO, user_initiator_timer_cb);
        user_disconnect_to_timer = app_easy_timer(USER_DISCONNECT_TO_TO, user_disconnect_to_timer_cb);
        
        p->accessed = true;
        
        // Get the state
        arch_printf("\r\nState in initiator timer: %x", ke_state_get(TASK_APP));
        
        app_easy_gap_start_connection_to_set(p->adv_addr_type, (uint8_t *)&p->adv_addr.addr, MS_TO_DOUBLESLOTS(USER_CON_INTV));
        app_easy_gap_start_connection_to();
    }
    else if (ke_state_get(TASK_APP) == APP_CONNECTED)
    {
        arch_printf("\r\nState in initiator timer: %x", ke_state_get(TASK_APP));
        user_initiator_timer = app_easy_timer(USER_INITIATOR_TO, user_initiator_timer_cb);
        app_easy_gap_disconnect(app_connection_idx);
    }        
    else
    {
        user_initiator_timer = EASY_TIMER_INVALID_TIMER;
        user_adv_rssi_list_clear();
        arch_printf("\r\nOn list clear:");
        user_adv_rssi_print_list();

        user_app_adv_start();
    }
}

static void user_collect_conn_rssi(uint8_t rssi_val)
{
    static uint8_t idx;
    static uint8_t rssi_values[USER_CON_RSSI_MAX_NB];
    
    if (idx < USER_CON_RSSI_MAX_NB)
    {
        arch_printf("\r\n Connection RSSI:%d", (int8_t) rssi_val);
        rssi_values[idx] = rssi_val;
        idx++;
    }
    else
    {
        int32_t mean_con_rssi = 0;
        
        int i;
        for (i=0; i<USER_CON_RSSI_MAX_NB; i++)
            mean_con_rssi += (int8_t) rssi_values[i];
        mean_con_rssi /= USER_CON_RSSI_MAX_NB;
        
        arch_printf("\r\n Mean connection RSSI:%d", mean_con_rssi);
        
        idx = 0;
        memset(rssi_values, 0, sizeof(rssi_values));
        
        if (mean_con_rssi > user_prox_zones_rssi[USER_PROX_ZONE_DANGER])
            ;//placeholder for LED danger alert
        else if (mean_con_rssi > user_prox_zones_rssi[USER_PROX_ZONE_WARNING])
            ;//placeholder for LED warning alert
        else if (mean_con_rssi > user_prox_zones_rssi[USER_PROX_ZONE_COARSE])
            ;//placeholder for LED coarse alert
        
        if (user_poll_conn_rssi_timer != EASY_TIMER_INVALID_TIMER)
        {
            arch_printf("\r\nOn RSSI collect: Cancel RSSI timer if not invalid");
            app_easy_timer_cancel(user_poll_conn_rssi_timer);
        }
        arch_printf("\r\nOn RSSI collect: Invalidate RSSI timer");
        user_poll_conn_rssi_timer = EASY_TIMER_INVALID_TIMER;
        
        app_easy_gap_disconnect(app_connection_idx);
    }
        
}

void user_app_init(void)
{
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    user_switch_adv_scan_timer = EASY_TIMER_INVALID_TIMER;
    user_initiator_timer = EASY_TIMER_INVALID_TIMER;
    user_disconnect_to_timer = EASY_TIMER_INVALID_TIMER;
    
    default_app_on_init();
}

void user_app_on_scanning_completed(const uint8_t param)
{
    bool has_conn_candidate;
    struct user_adv_rssi_node* p;
    
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

    user_is_advertiser = true;
    user_switch_adv_scan_timer = app_easy_timer(USER_SWITCH_ADV_SCAN_TO + rand_val, user_switch_adv_scan_timer_cb);

    app_easy_gap_undirected_advertise_start();
}

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
        
//        if (user_is_advertiser == true)
//        {
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
                arch_printf("\r\nOn connection: Set RSSI timer");
                user_poll_conn_rssi_timer = app_easy_timer(USER_UPD_CONN_RSSI_TO, user_poll_conn_rssi_timer_cb);
//            }
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
        user_is_advertiser = false;

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
    // Cancel the parameter update request timer
    if (app_param_update_request_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_param_update_request_timer_used);
        app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    }
    
    ke_state_set(TASK_APP, APP_CONNECTABLE);
    
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
        case GAPC_PARAM_UPDATED_IND:
        {
            // Cast the "param" pointer to the appropriate message structure
            struct gapc_param_updated_ind const *msg_param = (struct gapc_param_updated_ind const *)(param);

            // Check if updated Conn Params filled to preferred ones
            if ((msg_param->con_interval >= user_connection_param_conf.intv_min) &&
                (msg_param->con_interval <= user_connection_param_conf.intv_max) &&
                (msg_param->con_latency == user_connection_param_conf.latency) &&
                (msg_param->sup_to == user_connection_param_conf.time_out))
            {
            }
        } break;
        case GAPC_CON_RSSI_IND:
        {
            struct gapc_con_rssi_ind const *msg_param = (struct gapc_con_rssi_ind const *)(param);
            user_collect_conn_rssi(msg_param->rssi);
        } break;

        default:
            break;
    }
}

/**
 ****************************************************************************************
 * @brief Handles advertise reports
 * @param[in]   param Parameters of advertise report message
 * @return void
 ****************************************************************************************
 */
void user_app_on_adv_report_ind(struct gapm_adv_report_ind const * param)
{
    uint8_t report_data[ADV_DATA_LEN + 1];
    
    memcpy(report_data, param->report.data, param->report.data_len);
    report_data[param->report.data_len] = '\0';
    
    arch_printf("\r\n%02x %02x %02x %02x %02x %02x\t",
    param->report.adv_addr.addr[5],
    param->report.adv_addr.addr[4],
    param->report.adv_addr.addr[3],
    param->report.adv_addr.addr[2],
    param->report.adv_addr.addr[1],
    param->report.adv_addr.addr[0]);
    arch_printf("%d\t", (int8_t)param->report.rssi);
    
    if(!memcmp(report_data + 9, user_custom_srv_uuid, ATT_UUID_128_LEN))
    {
        arch_printf("TRUE");
    }
    else
        arch_printf("FALSE");

    // Populate advertiser report list
    user_adv_rssi_add_node_rssi(param);
    user_adv_rssi_print_list();
}
/// @} APP
