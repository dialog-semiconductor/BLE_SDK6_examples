/**
 ****************************************************************************************
 *
 * @file user_midi.c
 *
 * @brief All in one project source code.
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
#include "co_bt.h"
#include "app.h"
#include "app_easy_timer.h"
#include "app_prf_perm_types.h"
#include "app_easy_security.h"
#include "app_security.h"
#include "app_task.h"
#include "app_utils.h"
#include "user_midi.h"
#include "user_custs1_def.h"
#include "user_custs1_impl.h"
#include "user_periph_setup.h"

#if (BLE_SUOTA_RECEIVER)
#include "app_suotar.h"
#if (!SUOTAR_SPI_DISABLE)
#include "spi_flash.h"
#endif
#endif

#if defined (CFG_PRINTF)
#include "arch_console.h"
#endif

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

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

uint8_t app_connection_idx                    __attribute__((section(".bss."))); // @RETENTION MEMORY
timer_hnd app_adv_data_update_timer_used      __attribute__((section(".bss."))); // @RETENTION MEMORY
timer_hnd app_param_update_request_timer_used __attribute__((section(".bss."))); // @RETENTION MEMORY

// Retained variables
// Manufacturer Specific Data
struct mnf_specific_data_ad_structure mnf_data  __attribute__((section(".bss."))); //@RETENTION MEMORY
// Index of manufacturer data in advertising data or scan response data (when MSB is 1)
uint8_t mnf_data_index                          __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t stored_adv_data_len                     __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data_len                __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t stored_adv_data[ADV_DATA_LEN]           __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data[SCAN_RSP_DATA_LEN] __attribute__((section(".bss."))); //@RETENTION MEMORY

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
    uint16_t data;

    data = mnf_data.proprietary_data[0] | (mnf_data.proprietary_data[1] << 8);
    data += 1;
    mnf_data.proprietary_data[0] = data & 0xFF;
    mnf_data.proprietary_data[1] = (data >> 8) & 0xFF;

    if (data == 0xFFFF) {
         mnf_data.proprietary_data[0] = 0;
         mnf_data.proprietary_data[1] = 0;
    }
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

    default_app_on_init();

#if (BLE_APP_SEC)
    // Set service security requirements
    app_set_prf_srv_perm(TASK_ID_CUSTS1, APP_CUSTS1_SEC_REQ);
    // Fetch bond data from the external memory
    app_easy_security_bdb_init();
	
#if (BLE_SUOTA_RECEIVER)
    app_set_prf_srv_perm(TASK_ID_SUOTAR, APP_SUOTA_SEC_REQ);
#endif

#endif // (BLE_APP_SEC)

}

void user_app_adv_start(void)
{
    // Schedule the next advertising data update
    app_adv_data_update_timer_used = app_easy_timer(APP_ADV_DATA_UPDATE_TO, adv_data_update_timer_cb);

    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();

    // Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);

    // Set extended sleep with OTP copy during advertising
    arch_set_extended_sleep(true);

    app_easy_gap_undirected_advertise_start();
}

#if (BLE_SUOTA_RECEIVER)
void on_suotar_status_change( const uint8_t suotar_event)
{
#if (!SUOTAR_SPI_DISABLE)
    int8_t man_dev_id = 0;

    man_dev_id = spi_flash_enable(SPI_EN_GPIO_PORT, SPI_EN_GPIO_PIN);
    if (man_dev_id == SPI_FLASH_AUTO_DETECT_NOT_DETECTED)
    {
        // The device was not identified. The default parameters are used.
        // Alternatively, an error can be asserted here.
        spi_flash_init(SPI_FLASH_DEFAULT_SIZE, SPI_FLASH_DEFAULT_PAGE);
    }

    if( suotar_event == SUOTAR_END )
    {
        // Power down SPI Flash
        spi_flash_power_down();
    }
#endif
}
#endif // (BLE_SUOTA_RECEIVER)

void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;

        // Stop the advertising data update timer
       // app_easy_timer_cancel(app_adv_data_update_timer_used);

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
        user_app_enable_periphs();

        // Set extended sleep without OTP copy during connection
        arch_set_extended_sleep(false);
    }
		
		else
    {
        // No connection has been established, restart advertising
        user_app_adv_start();
    }

    default_app_on_connection(connection_idx, param);
}

void user_app_disconnect(struct gapc_disconnect_ind const *param)
{
    // Cancel the parameter update request timer
    if (app_param_update_request_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_param_update_request_timer_used);
        app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    }

    user_app_disable_periphs();
    // Update manufacturer data for the next advertsing event
    mnf_data_update();
    user_app_adv_start();

#if (BLE_SUOTA_RECEIVER)
    // Issue a platform reset when it is requested by the suotar procedure
    if (suota_state.reboot_requested)
    {
        // Reboot request will be served
        suota_state.reboot_requested = 0;

        // Platform reset
        platform_reset(RESET_AFTER_SUOTA_UPDATE);
    }
#endif
}

#if (BLE_APP_SEC)

void user_app_on_tk_exch(uint8_t conidx,
                         struct gapc_bond_req_ind const * param)
{
    bool accept = true;

    if (param->data.tk_type == GAP_TK_OOB)
    {
        uint8_t oob_tk[KEY_LEN] = APP_SECURITY_OOB_TK_VAL;
        app_easy_security_tk_exch(conidx, (uint8_t*) oob_tk, KEY_LEN, accept);
    }
    else if (param->data.tk_type == GAP_TK_DISPLAY)
    {
        #if defined (CFG_PRINTF)
        uint32_t passkey = app_sec_gen_tk();
        arch_printf("\r\n Enter Passkey: %u", passkey);
        #else
        uint32_t passkey = APP_SECURITY_MITM_PASSKEY_VAL;
        #endif

        app_easy_security_tk_exch(conidx, (uint8_t*) &passkey, sizeof(passkey), accept);
    }
    else if (param->data.tk_type == GAP_TK_KEY_ENTRY)
    {
        uint32_t passkey = APP_SECURITY_MITM_PASSKEY_VAL;

        #if defined (CFG_PRINTF)
        arch_printf("\r\n Passkey Entered: %u", passkey);
        #endif

        app_easy_security_tk_exch(conidx, (uint8_t*) &passkey, sizeof(passkey), accept);
    }
    else if (param->data.tk_type == GAP_TK_KEY_CONFIRM)
    {
        #if defined (CFG_PRINTF)
        uint32_t passkey;
        // Print the 6 Least Significant Digits of Passkey
        char buf[6];
        passkey = (param->tk.key[0] << 0 ) | (param->tk.key[1] << 8 ) |
                  (param->tk.key[2] << 16) | (param->tk.key[3] << 24);
        arch_printf("\r\n Confirmation Value: ");
        for (uint8_t i=0; i<6; i++){
            buf[5-i] = passkey%10;
            passkey /= 10;
        }
        for (uint8_t i=0; i<6; i++){
            arch_printf("%u",buf[i]);
        }
        #endif

        app_easy_security_tk_exch(conidx, (uint8_t *) &param->tk, sizeof(param->tk), accept);
    }
}
#endif // (BLE_APP_SEC)

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

        default:
            break;
    }
}

void user_app_adv_undirect_complete(const uint8_t status)
{
    // Disable wakeup for BLE and timer events. Only external (GPIO) wakeup events can wakeup processor.
    if (status == GAP_ERR_CANCELED)
    {
        user_app_adv_start();
    }
}

/// @} APP