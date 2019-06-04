/**
 ****************************************************************************************
 *
 * @file user_accelerometer.c
 *
 * @brief Accelerometer application source code.
 *
 * Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs included in this package ("Software") include confidential, proprietary information of Dialog Semiconductor. All Rights Reserved.
 *
 * THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR (‘DIALOG’) AND MAY ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND.  THIS SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS.  RECIPIENT SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT DIALOG’S PRIOR WRITTEN PERMISSION.
 *
 * UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED BY THE FCC OR OTHER LIKE AGENCIES.
 *
 * IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
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
#include "custs1_task.h"
#include "m_drv_mc36xx.h"
#include <math.h>

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

#define NOTIFICATION_DELAY 50 //The time interval between sent notifications

// Manufacturer Specific Data ADV structure type
struct mnf_specific_data_ad_structure
{
    uint8_t ad_structure_size;
    uint8_t ad_structure_type;
    uint8_t company_id[APP_AD_MSD_COMPANY_ID_LEN];
    uint8_t proprietary_data[APP_AD_MSD_DATA_LEN];
};

typedef struct {
    short RawData[FIFO_DEEP][M_DRV_MC_UTL_AXES_NUM];
    float fRawData[FIFO_DEEP][M_DRV_MC_UTL_AXES_NUM];
    short DataLen;
    uint32_t TimeStamp[FIFO_DEEP]; //ms
}AccData;    //FIFO DEEP

static AccData GensorData;

#ifdef SNIFF_DEMO
static uint8_t m_polling_counter = 0;
static uint8_t sniff_delay = 1;
#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint8_t app_connection_idx                      __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
timer_hnd app_adv_data_update_timer_used        __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
timer_hnd app_param_update_request_timer_used   __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
int16_t X_data                 __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
int16_t Y_data                 __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
int16_t Z_data                 __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t X_string[8]               __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t Y_string[8]               __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t Z_string[8]               __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t X_timer                   __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t Y_timer                   __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t Z_timer                   __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t g_timer                   __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
// Retained variables
struct mnf_specific_data_ad_structure mnf_data  __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
// Index of manufacturer data in advertising data or scan response data (when MSB is 1)
uint8_t mnf_data_index                          __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_adv_data_len                     __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data_len                __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_adv_data[ADV_DATA_LEN]           __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data[SCAN_RSP_DATA_LEN] __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

uint8_t user_int_to_string(int16_t input, uint8_t *s){
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
    uint8_t fifo_size = 1;
#ifdef FIFO_DEMO
    fifo_size = FIFO_THRESHOLD;
#endif
    M_DRV_MC36XX_ReadRawData(GensorData.RawData, fifo_size);

#ifdef SNIFF_DEMO       //Set sensor into sniff mode after data is updated.
    if(m_polling_counter<(32*sniff_delay)) {
        m_polling_counter++;
    } else {
        M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_STANDBY);      // Set MC36XX to stanby mode
        M_DRV_MC36XX_SetGain(E_M_DRV_MC36XX_WAKE_GAIN_LOW, E_M_DRV_MC36XX_SNIFF_GAIN_HIGH);
        /* Orion 2X SNIFF threshold*/
        M_DRV_MC36XX_SetSniffThreshold(M_DRV_MC36XX_AXIS_X, 3);
        M_DRV_MC36XX_SetSniffThreshold(M_DRV_MC36XX_AXIS_Y, 3);
        M_DRV_MC36XX_SetSniffThreshold(M_DRV_MC36XX_AXIS_Z, 3);
        /* Sniff count set */
        M_DRV_MC36XX_SetSniffDetectCount(M_DRV_MC36XX_AXIS_X, 1);
        M_DRV_MC36XX_SetSniffDetectCount(M_DRV_MC36XX_AXIS_Y, 1);
        M_DRV_MC36XX_SetSniffDetectCount(M_DRV_MC36XX_AXIS_Z, 1);
        M_DRV_MC36XX_SetSampleRate(E_M_DRV_MC36XX_CWAKE_SR_UL_26Hz, E_M_DRV_MC36XX_SNIFF_SR_DEFAULT_6Hz);
        M_DRV_MC36XX_ConfigINT(0,0,0,0,1);
        M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_SNIFF);    // Initialize MC36XX
        m_polling_counter = 0;
    }
#endif
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
static void adv_data_update_timer_cb()
{
    // If mnd_data_index has MSB set, manufacturer data is stored in scan response
    uint8_t *mnf_data_storage = (mnf_data_index & 0x80) ? stored_scan_rsp_data : stored_adv_data;

    // Update manufacturer data
    mnf_data_update();

    // Update the selected fields of the advertising data (manufacturer data)
    memcpy(mnf_data_storage + (mnf_data_index & 0x7F), &mnf_data, sizeof(struct mnf_specific_data_ad_structure));

    // Update advertising data on the fly
    app_easy_gap_update_adv_data(stored_adv_data, stored_adv_data_len, stored_scan_rsp_data, stored_scan_rsp_data_len);

    // Restart timer for the next advertising update
    app_adv_data_update_timer_used = app_easy_timer(APP_ADV_DATA_UPDATE_TO, adv_data_update_timer_cb);
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
 ****************************************************************************************
 * @brief User application callback function to set initial device configuration.
 * @return void
 ****************************************************************************************
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
    X_timer = EASY_TIMER_INVALID_TIMER; //Initialise timer handlers
    Y_timer = EASY_TIMER_INVALID_TIMER; //Initialise timer handlers
    Z_timer = EASY_TIMER_INVALID_TIMER; //Initialise timer handlers
    g_timer = EASY_TIMER_INVALID_TIMER; //Initialise timer handlers
    default_app_on_init();
}

/**
 ****************************************************************************************
 * @brief User application callback function to configure device advertisement.
 * @return void
 ****************************************************************************************
*/
void user_app_adv_start(void)
{
    // Schedule the next advertising data update
    app_adv_data_update_timer_used = app_easy_timer(APP_ADV_DATA_UPDATE_TO, adv_data_update_timer_cb);

    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();

    // Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);

    app_easy_gap_undirected_advertise_start();
}

/**
 ****************************************************************************************
 * @brief User application callback function upon device connection is established.
 * @return void
 ****************************************************************************************
*/
void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;

        // Stop the advertising data update timer
        app_easy_timer_cancel(app_adv_data_update_timer_used);

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
 ****************************************************************************************
 * @brief User appliation callback function upon advertisement completion.
 * @return void
 ****************************************************************************************
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
 ****************************************************************************************
 * @brief User application callback function upon device disconnect.
 * @return void
 ****************************************************************************************
*/
void user_app_disconnect(struct gapc_disconnect_ind const *param)
{
    // Cancel the parameter update request timer
    if (app_param_update_request_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_param_update_request_timer_used);
        app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    }

    // Stop the notification timers
    if(X_timer != EASY_TIMER_INVALID_TIMER){
        app_easy_timer_cancel(X_timer);
        X_timer = EASY_TIMER_INVALID_TIMER;
    }
    if(Y_timer != EASY_TIMER_INVALID_TIMER){
        app_easy_timer_cancel(Y_timer);
        Y_timer = EASY_TIMER_INVALID_TIMER;
    }
    if(Z_timer != EASY_TIMER_INVALID_TIMER){
        app_easy_timer_cancel(Z_timer);
        Z_timer = EASY_TIMER_INVALID_TIMER;
    }
    if(g_timer != EASY_TIMER_INVALID_TIMER){
        app_easy_timer_cancel(g_timer);
        g_timer = EASY_TIMER_INVALID_TIMER;
    }

    // Update manufacturer data for the next advertsing event
    mnf_data_update();
    // Restart Advertising
    user_app_adv_start();
}

/**
 ****************************************************************************************
 * @brief User application function to send accelerometer X-axis data notification.
 * @return void
 ****************************************************************************************
*/
void user_svc1_accel_X_send_ntf()
{
    //Construct the string to send as a notification
    mnf_data_update();
    uint8_t string_length = user_int_to_string(GensorData.RawData[0][M_DRV_MC36XX_AXIS_X] * 1.95, X_string); //Read data and multipy by 1.95 to get acceleration in mg

    //Allocate a new message
    struct custs1_val_ntf_ind_req* req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,                   //Message id
                                        prf_get_task_from_id(TASK_ID_CUSTS1), //Target task
                                        TASK_APP,                             //Source of the message
                                        custs1_val_ntf_ind_req,               //The type of structure in the message,
                                                        //This structure should match the ID
                                                        //The ID's and strucures are found in custs1_task.h
                                        string_length);                       //How many bytes of data will be added

    req->conidx = 0;                            //Connection ID to send the data to (this application can only have one connection(0))
    req->notification = true;                   //Data is sent as a notification and not as indication
    req->handle = SVC1_IDX_ACCEL_X_DATA_VAL;    //The handle of the characteristic we want to write to
    req->length = string_length;                //Data length in bytes
    memcpy(req->value, X_string, string_length);//Copy the string to the message

    ke_msg_send(req);                           //Send the message to the task

    X_timer = app_easy_timer(NOTIFICATION_DELAY / 10, user_svc1_accel_X_send_ntf); //Set a timer for 100 ms (10*10)
}

/**
 ****************************************************************************************
 * @brief User application function to set X-Axis notification handler.
 * @return void
 ****************************************************************************************
*/
void user_svc1_accel_X_wr_ntf_handler(struct custs1_val_write_ind const *param){
    if(param->value[0]){
        //If the client subscribed to the notification
        if(X_timer == EASY_TIMER_INVALID_TIMER){
            //Start the X timer if it is not running
            app_easy_timer(10, user_svc1_accel_X_send_ntf);
        }
    }
    else{
        //If the client unsubscribed from the notification
        if(X_timer != EASY_TIMER_INVALID_TIMER){
            //Stop the X timer if it is running
            app_easy_timer_cancel(X_timer);
            X_timer = EASY_TIMER_INVALID_TIMER;
        }
    }
}

/**
 ****************************************************************************************
 * @brief User application function to send accelerometer Y-axis data notification.
 * @return void
 ****************************************************************************************
*/
void user_svc1_accel_Y_send_ntf()
{
    mnf_data_update();
    uint8_t string_length = user_int_to_string(GensorData.RawData[0][M_DRV_MC36XX_AXIS_Y] * 1.95, Y_string);

    struct custs1_val_ntf_ind_req* req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                             prf_get_task_from_id(TASK_ID_CUSTS1),
                             TASK_APP,
                             custs1_val_ntf_ind_req,
                             string_length);

    req->conidx = 0;
    req->notification = true;
    req->handle = SVC1_IDX_ACCEL_Y_DATA_VAL;
    req->length = string_length;
    for(uint8_t i = 0; i < string_length; i++){
        req->value[i] = Y_string[i];
    }

    ke_msg_send(req);

    Y_timer = app_easy_timer(NOTIFICATION_DELAY / 10, user_svc1_accel_Y_send_ntf); //Do this again in 100 ms (10*10)
}

/**
 ****************************************************************************************
 * @brief User application function to set Y-Axis notification handler.
 * @return void
 ****************************************************************************************
*/
void user_svc1_accel_Y_wr_ntf_handler(struct custs1_val_write_ind const *param){
    if(param->value[0]){
        if(Y_timer == EASY_TIMER_INVALID_TIMER){
            //Start the Y timer if it is not running
            Y_timer = app_easy_timer(10, user_svc1_accel_Y_send_ntf);
            }
        }
        else{
        if(Y_timer != EASY_TIMER_INVALID_TIMER){
            //Stop the Y timer if it is running
            app_easy_timer_cancel(Y_timer);
            Y_timer = EASY_TIMER_INVALID_TIMER;
        }
    }
}

/**
 ****************************************************************************************
 * @brief User application function to send accelerometer Z-axis data notification.
 * @return void
 ****************************************************************************************
*/
void user_svc1_accel_Z_send_ntf()
{
    mnf_data_update();
    uint8_t string_length = user_int_to_string(GensorData.RawData[0][M_DRV_MC36XX_AXIS_Z] * 1.95, Z_string);

    struct custs1_val_ntf_ind_req* req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                             prf_get_task_from_id(TASK_ID_CUSTS1),
                             TASK_APP,
                             custs1_val_ntf_ind_req,
                             string_length);


    req->conidx = 0;
    req->notification = true;
    req->handle = SVC1_IDX_ACCEL_Z_DATA_VAL;
    req->length = string_length;
    for(uint8_t i = 0; i < string_length; i++){
        req->value[i] = Z_string[i];
    }

    ke_msg_send(req);

    Z_timer = app_easy_timer(NOTIFICATION_DELAY / 10, user_svc1_accel_Z_send_ntf); //Do this again in 100 ms (10*10)
}

/**
 ****************************************************************************************
 * @brief User application function to set Z-Axis notification handler.
 * @return void
 ****************************************************************************************
*/
void user_svc1_accel_Z_wr_ntf_handler(struct custs1_val_write_ind const *param){
    if(param->value[0]){
        if(Z_timer == EASY_TIMER_INVALID_TIMER){
            //Start the Z timer if it is not running
            Z_timer = app_easy_timer(10, user_svc1_accel_Z_send_ntf);
        }
    }
    else{
        if(Z_timer != EASY_TIMER_INVALID_TIMER){
            //Stop the Z timer if it is running
            app_easy_timer_cancel(Z_timer);
            Z_timer = EASY_TIMER_INVALID_TIMER;
        }
    }
}

/**
 ****************************************************************************************
 * @brief User application timer callback function to handle Gyro data service.
 * @return void
 ****************************************************************************************
*/
void user_svc2_g_timer_cb_handler(void)
{
    struct custs1_val_ntf_ind_req *req =
                    KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                    prf_get_task_from_id(TASK_ID_CUSTS1),
                    TASK_APP,
                    custs1_val_ntf_ind_req,
                    DEF_SVC2_G_DATA_CHAR_LEN);
    mnf_data_update();
    req->handle = SVC2_IDX_G_DATA_VAL;
    req->length = DEF_SVC2_G_DATA_CHAR_LEN;
    req->notification = true;
    memcpy(req->value, GensorData.RawData, DEF_SVC2_G_DATA_CHAR_LEN);
    ke_msg_send(req);
    g_timer = app_easy_timer(NOTIFICATION_DELAY / 10, user_svc2_g_timer_cb_handler);
}

/**
 ****************************************************************************************
 * @brief User application function to set Gyro data notification handler.
 * @return void
 ****************************************************************************************
*/
void user_svc2_g_wr_ntf_handler(struct custs1_val_write_ind const *param){
    if(param->value[0]){
        if(g_timer == EASY_TIMER_INVALID_TIMER){
            //Start the g timer if it is not running
            g_timer = app_easy_timer(10, user_svc2_g_timer_cb_handler);
        }
    }
    else{
        if(g_timer != EASY_TIMER_INVALID_TIMER){
            //Stop the g timer if it is running
            app_easy_timer_cancel(g_timer);
            g_timer = EASY_TIMER_INVALID_TIMER;
        }
    }
}

/**
 ****************************************************************************************
 * @brief User application callback function to handle BLE GAP operations.
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

    case CUSTS1_VAL_WRITE_IND:
        {
            struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

#ifdef SNIFF_DEMO
            if (msg_param->value[0])
                sniff_delay++;
            else
                sniff_delay--;
#endif

            switch (msg_param->handle)
            {
                case SVC1_IDX_ACCEL_X_NTF_CFG:
                    user_svc1_accel_X_wr_ntf_handler(msg_param);
                    break;

                case SVC1_IDX_ACCEL_Y_NTF_CFG:
                    user_svc1_accel_Y_wr_ntf_handler(msg_param);
                    break;

                case SVC1_IDX_ACCEL_Z_NTF_CFG:
                    user_svc1_accel_Z_wr_ntf_handler(msg_param);
                    break;

                case SVC2_IDX_G_NTF_CFG:
                    user_svc2_g_wr_ntf_handler(msg_param);
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
                    break;

                case SVC1_IDX_ACCEL_Y_DATA_VAL:
                    break;

                case SVC1_IDX_ACCEL_Z_DATA_VAL:
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
