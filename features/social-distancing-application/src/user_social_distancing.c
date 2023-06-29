/**
 ****************************************************************************************
 *
 * @file user_social_distancing.c
 *
 * @brief Social distancing software example source code.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
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
#include "llc_task.h"
#include "llc.h"

/*
 * DEFINES
 ****************************************************************************************
 */
#define USER_RUNN_AVG_LENGTH    5   //Length of the running average filter
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
timer_hnd initiate_connection_timer             __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

#if (USER_SCANNING_TIMEOUT)
timer_hnd user_scan_timeout_timer               __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
#endif

bool is_initiator                               __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
bool init_con_pending                           __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t rssi_exchanged                          __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
char bd_addr[18]                                __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/*
 * FUNCTION DECLARATION
 ****************************************************************************************
 */
static void proximity_estimation_alert(void);

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
static struct user_adv_rssi_node* user_adv_rep_rssi_head  __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
static const int8_t user_prox_zones_rssi[USER_PROX_ZONE_COUNT] = {-59, -62, -70, -80};

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
            if (p->count < USER_RUNN_AVG_LENGTH)
            {
                p->mean_rssi = ((uint32_t)adv_report->report.rssi + (uint32_t)p->mean_rssi) / 2;
            }
            else
            {
                
                p->mean_rssi = ( (USER_RUNN_AVG_LENGTH - 1) * (uint32_t)p->mean_rssi + adv_report->report.rssi) / USER_RUNN_AVG_LENGTH ;
            }
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

    arch_printf("\r\nSTORED LIST:");
    
    p = user_adv_rep_rssi_head;
    while (p != NULL) 
    {
        arch_printf("\r\n LIST ENTRY:\t");
        arch_printf("BD ADDRESS:\t");
        arch_printf("%02x %02x %02x %02x %02x %02x\t",
        p->adv_addr.addr[5],
        p->adv_addr.addr[4],
        p->adv_addr.addr[3],
        p->adv_addr.addr[2],
        p->adv_addr.addr[1],
        p->adv_addr.addr[0]);
        arch_printf("RSSI: %d\t", (int8_t)p->mean_rssi);
        arch_printf("IS ACCESSED: %s\t", p->accessed? "True" : "False");
        arch_printf("FOUND: %d time(s)\t\r\n", (int8_t)p->count);
     
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
    arch_printf("\r\nCLEAR LIST \r\n");
    
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
 * @brief Receives RSSI value on connection from peer device
 * @param[in] msgid Id of the message received
 * @param[in] param Parameters of the @ref CUSTS1_VAL_WRITE_IND message
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id ID of the sending task instance
 ****************************************************************************************
 */
static void rssi_write_ind_handler(ke_msg_id_t const msgid,
                                      struct custs1_val_write_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    if(rssi_con_value < (int8_t) param->value[0])
    {
        rssi_con_value = (int8_t) param->value[0]; 
    }
    arch_printf(YELLOW(PLAIN) "\n\rRECEIVED RSSI VALUE %d \n\r" RESET_COLOUR,(int8_t)param->value[0]);
}

/**
 ****************************************************************************************
 * @brief Scanning timer callback for cancelling scanning operation 
 * @return void
 ****************************************************************************************
 */
#if USER_SCANNING_TIMEOUT
static void user_scanning_timeout_timer_cb(void)
{
    user_scan_timeout_timer = EASY_TIMER_INVALID_TIMER;
    
    struct gapm_cancel_cmd *cmd = app_gapm_cancel_msg_create();

    app_gapm_cancel_msg_send(cmd);
}
#endif

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
    
    arch_printf(CYAN(BOLD) "\r\n%s: SCANNING\n\r" RESET_COLOUR, bd_addr);
    
    // Send the message
    ke_msg_send(cmd);

    // We are now connectable
    ke_state_set(TASK_APP, APP_CONNECTABLE);
    
    #if USER_SCANNING_TIMEOUT
    int8_t rand_val = (int8_t) co_rand_byte();
    user_scan_timeout_timer = app_easy_timer(USER_SCANNING_TO + rand_val, user_scanning_timeout_timer_cb);
    #endif
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
    user_poll_conn_rssi_timer = EASY_TIMER_INVALID_TIMER;
    
    if (ke_state_get(KE_BUILD_ID(TASK_LLC, gapc_get_conhdl(app_connection_idx)))== LLC_CONNECTED)
    {   
        if (rssi_exchanged < USER_CON_RSSI_MAX_NB)
        {
            // Get the RSSI value
            int8_t local_rssi = (int8_t) llc_env[gapc_get_conhdl(app_connection_idx)]->rssi;
            
            if(rssi_con_value < local_rssi)
                rssi_con_value = local_rssi;
        
            rssi_exchanged++;
            // Write the received RSSI to the peer device
            perform_rssi_write_to_peer(local_rssi);
            // Restart the timer
            user_poll_conn_rssi_timer = app_easy_timer(USER_UPD_CONN_RSSI_TO, user_poll_conn_rssi_timer_cb);
            
            arch_printf("\r\nLOCAL RSSI VALUE %d\r\n", local_rssi);
        }
        else
        {
            rssi_exchanged = 0;
            proximity_estimation_alert();
        }
    }
}

/**
 ****************************************************************************************
 * @brief Timer callback function to cancel connection attempt
 * @return void
 ****************************************************************************************
 */
static void user_disconnect_to_timer_cb()
{
    struct gapm_cancel_cmd *cmd = app_gapm_cancel_msg_create();
    // Send the message
    app_gapm_cancel_msg_send(cmd);

    initiate_connection_timer = EASY_TIMER_INVALID_TIMER;
}

/**
 ****************************************************************************************
 * @brief Decides based on the collected rssi values if an alert should be started
 * @param[in] rssi_val The current RSSI value
 * @return void
 ****************************************************************************************
 */
static void proximity_estimation_alert(void)
{   
    arch_printf("\r\nINFO: STRONGEST RSSI IN CONNECTED STATE: %d\r\n", rssi_con_value);
    
    if (rssi_con_value > user_prox_zones_rssi[USER_PROX_ZONE_DANGER])
    {   
        alert_user_start(DANGER_ZONE, (is_initiator)?(app_easy_gap_disconnect) : (NULL), app_connection_idx);
        arch_printf(RED(BOLD) "\r\nINFO: %s IS IN DANGER ZONE\r\n" RESET_COLOUR, bd_addr);
    }
    else if (rssi_con_value > user_prox_zones_rssi[USER_PROX_ZONE_WARNING])
    {            
        alert_user_start(WARNING_ZONE, (is_initiator)?(app_easy_gap_disconnect) : (NULL), app_connection_idx);
        arch_printf(YELLOW(BOLD) "\r\nINFO: %s IS IN WARNING ZONE\r\n" RESET_COLOUR, bd_addr);
    }
    else if (rssi_con_value > user_prox_zones_rssi[USER_PROX_ZONE_COARSE])
    {               
        alert_user_start(COARSE_ZONE, (is_initiator)?(app_easy_gap_disconnect) : (NULL), app_connection_idx);
        arch_printf(GREEN(BOLD) "\r\nINFO: %s IS IN COARSE ZONE\r\n" RESET_COLOUR, bd_addr);
    }
    else if (is_initiator)
        app_easy_gap_disconnect(app_connection_idx);
    
    rssi_con_value = -128;
}

bool initiate_connection_attempt(void)
{
    bool found_node = false;
    
    struct user_adv_rssi_node* p;
    p = user_adv_rssi_get_max_rssi_node(); // Change get max rssi to CHECK FOR ACCESSED
    
    if (user_adv_rssi_list_has_candidate() && p != NULL && (ke_state_get(TASK_APP) == APP_CONNECTABLE))
    {
        arch_printf("\r\n%s: ATTEMPT FOR CONNECTION\r\n", bd_addr);
        /* If there is a candidate the connect to start the attempt of the connection */
        app_easy_gap_start_connection_to_set(p->adv_addr_type, (uint8_t *)&p->adv_addr.addr, MS_TO_DOUBLESLOTS(USER_CON_INTV));
        app_easy_gap_start_connection_to();
        p->accessed = true;
        /* Indicate that the device has started a connection attempt */
        init_con_pending = true;
        /* Start a timer to for the command to be cancelled in case there is no connection */
        initiate_connection_timer = app_easy_timer(USER_INITIATE_CONN_TO, user_disconnect_to_timer_cb);
        found_node = true;
    }
    return found_node;
}

/*
 * SDK configured callback functions
 ****************************************************************************************
 */

void user_app_init(void)
{
    user_switch_adv_scan_timer = EASY_TIMER_INVALID_TIMER;
    initiate_connection_timer = EASY_TIMER_INVALID_TIMER;
    #if USER_SCANNING_TIMEOUT
    user_scan_timeout_timer = EASY_TIMER_INVALID_TIMER;
    #endif
    
    rssi_exchanged = 0;
    
    rssi_con_value = -128;
    
    alert_user_init();
    
    default_app_on_init();
    

}

void user_app_on_scanning_completed(const uint8_t param)
{
    struct user_adv_rssi_node* p;
    
    arch_printf(CYAN(PLAIN) "\r\n%s: SCAN COMPLETED\r\n" RESET_COLOUR, bd_addr);
    
    user_adv_rssi_print_list();
    
    p = user_adv_rssi_get_max_rssi_node();
    
    if (p == NULL)
    {
        arch_printf("\r\nINFO: NO NODE FOUND\r\n");
    }
    else
    {
        arch_printf("\r\nINFO: THE STRONGEST NODE WITH RSSI %d FOUND\r\n", (int8_t)p->mean_rssi);
    }
    
    if(!initiate_connection_attempt())
    {
        /* No nodes found to be connected start advertising */
        user_app_adv_start();
    }
}

void user_app_adv_start(void)
{
    int8_t rand_val = (int8_t) co_rand_byte();
    user_switch_adv_scan_timer = app_easy_timer(USER_SWITCH_ADV_SCAN_TO + rand_val, user_switch_adv_scan_timer_cb);
    app_easy_gap_undirected_advertise_start();
    arch_printf(GREEN(BOLD) "\r\n%s: ADVERTISING\r\n" RESET_COLOUR, bd_addr);
}

void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        
        if (init_con_pending)           // Check if the device has started a connection request
        {
            is_initiator = true;        // If a connection request has started it means that the device is the initiator
            init_con_pending = false;   // No pending connection request is submitted
        }
        
        if(is_initiator)
        {
            arch_printf(YELLOW(BOLD) "\r\n%s: CONNECTED AS INITIATOR\r\n" RESET_COLOUR, bd_addr);
        }
        else
        {
            arch_printf(YELLOW(BOLD) "\r\n%s: CONNECTED AS SLAVE\r\n" RESET_COLOUR, bd_addr);
        }
        
        app_connection_idx = connection_idx;
      
        if(user_switch_adv_scan_timer != EASY_TIMER_INVALID_TIMER)
        {    
            app_easy_timer_cancel(user_switch_adv_scan_timer);
            user_switch_adv_scan_timer = EASY_TIMER_INVALID_TIMER;
        }
        
        if(initiate_connection_timer != EASY_TIMER_INVALID_TIMER)
        {      
            app_easy_timer_cancel(initiate_connection_timer);
            initiate_connection_timer = EASY_TIMER_INVALID_TIMER;
        }
        
        /* Start the RSSI polling timer upon connection */
        user_poll_conn_rssi_timer = app_easy_timer(USER_UPD_CONN_RSSI_TO, user_poll_conn_rssi_timer_cb);
    }
    else
    {
        /* 
        * No connection has been established, either initiate a new connection or start advertising
        * If the device was an initiator and additional nodes need to be checked the connection initiation will start or start advertising
        * In case the device was a slave the initiate_connection_attempt will return false this it will start advertising
        */
        if(!initiate_connection_attempt())
        {
            /* Clear the list */
            user_adv_rssi_list_clear();
            /* No more nodes to connect to, start advertising */
            user_app_adv_start();
        }
    }

    default_app_on_connection(connection_idx, param);
}

void user_app_adv_undirect_complete(uint8_t status)
{
    arch_printf(GREEN(PLAIN)"\r\n%s: ADVERTISING COMPLETED\r\n" RESET_COLOUR, bd_addr);
    
    /* 
    * If the callback occurs with status GAP_ERR_CANCELED it means that the adv timer has elapsed
    * thus there is no need checking and cancelling. If a connection occurs the adv timer will
    * be cancelled from the connection handler
    */
    if (status == GAP_ERR_CANCELED)
        user_scan_start();
}

void user_app_disconnect(struct gapc_disconnect_ind const *param)
{
    ke_state_set(TASK_APP, APP_CONNECTABLE);
    
    arch_printf(YELLOW(BOLD)"\r\n%s: DISCONNECTED WITH REASON %02x\r\n" RESET_COLOUR, bd_addr, param->reason);
    
    /* 
    * If the current device was the initiator and a disconnection occurred regardless the reason
    * move to next node to attempt a new connection for the next available node.
    */
    if(is_initiator)
    {   
        user_adv_rssi_print_list();
        
        if(!initiate_connection_attempt())
        {
            /* Clear the list */
            user_adv_rssi_list_clear();
            /* No more nodes to connect to, start advertising */
            user_app_adv_start();
        }
    }
    else
    {
        /* Always start scan if disconnected as slave, no matter the disconnection reason */
        user_scan_start();
    }
    
    /* Always check if the rssi polling timer is active, if its active make sure to cancel it */
    if (user_poll_conn_rssi_timer != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(user_poll_conn_rssi_timer);
        user_poll_conn_rssi_timer = EASY_TIMER_INVALID_TIMER;
    }
    
    /* Clear the initiator flag  */
    is_initiator = false;
    
}

void user_app_on_connect_failed(void)
{
    arch_printf("\r\n%s: CONNECTION INITIATION FAILED\r\n", bd_addr);
    
    init_con_pending = false;
    /* Check if there is another node to be connected to */
    if(!initiate_connection_attempt())
    {
        /* Clear the list */
        user_adv_rssi_list_clear();
        
        user_app_adv_start();
    }
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
        
        default:
            break;
    }
}

void user_app_on_adv_report_ind(struct gapm_adv_report_ind const * param)
{
    uint8_t report_data[ADV_DATA_LEN + 1];
    const uint8_t user_custom_srv_uuid[] = DEF_SVC1_UUID_128;

    memcpy(report_data, param->report.data, param->report.data_len);
    report_data[param->report.data_len] = '\0';
    
    if(!memcmp(report_data + 5, user_custom_srv_uuid, ATT_UUID_128_LEN))
    {
        // Populate advertiser report list
        user_adv_rssi_add_node_rssi(param);
    }
}

void user_on_app_generate_unique_static_random_addr(struct bd_addr *addr)
{
    default_app_generate_unique_static_random_addr(addr);
    sprintf(bd_addr, "%02x:%02x:%02x:%02x:%02x:%02x", addr->addr[5], addr->addr[4],addr->addr[3],addr->addr[2],addr->addr[1],addr->addr[0]);
}
/// @} APP