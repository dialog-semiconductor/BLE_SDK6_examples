/**
 ****************************************************************************************
 *
 * @file user_security.c
 *
 * @brief Security related functions and handlings
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
#include "app_bond_db.h"
#include "user_security.h"

#if defined (CFG_PRINTF)
#include "arch_console.h"
#endif

uint8_t ret_conidx __attribute__((section(".bss."))); //@RETENTION MEMORY


static struct gapc_bond_cmd *gapc_bond_req[APP_EASY_MAX_ACTIVE_CONNECTION] __attribute__((section(".bss.")));


/**
 ****************************************************************************************
 * @brief Initial pairing->bonding process. to connected device
 *
 ****************************************************************************************
 */
void user_security_send_bond_req(uint8_t conidx, struct security_configuration sec_conf){
	arch_printf("\r\nSend GAPC_BOND_REQ\r\n");	
	struct gapc_bond_cmd * cmd = KE_MSG_ALLOC(GAPC_BOND_CMD,
                KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP, gapc_bond_cmd);
		cmd->operation = GAPC_BOND;
		cmd->pairing.iocap = sec_conf.iocap;
		cmd->pairing.oob = sec_conf.oob;
		cmd->pairing.auth = sec_conf.auth;
		cmd->pairing.key_size = sec_conf.key_size;
		cmd->pairing.ikey_dist = sec_conf.ikey_dist;
		cmd->pairing.rkey_dist = sec_conf.rkey_dist;
		cmd->pairing.sec_req = sec_conf.sec_req;
		ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Initial security encryption to the connection
 * if the connected device is not paired, go to the pairing process.
 ****************************************************************************************
 */
void user_security_send_encrypt_cmd(uint8_t conidx){
	//Check if the device is already bonded
	const struct app_sec_bond_data_env_tag *pbd = NULL;
	pbd = app_easy_security_bdb_search_entry(SEARCH_BY_BDA_TYPE, (void *)app_env[conidx].peer_addr.addr, BD_ADDR_LEN);
	if(pbd){
		ret_conidx = conidx;
		arch_printf("Found bond info in db.\n\r");
		struct gapc_encrypt_cmd * cmd = KE_MSG_ALLOC(GAPC_ENCRYPT_CMD,
								KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP, gapc_encrypt_cmd);
		cmd->operation = GAPC_ENCRYPT;
		cmd->ltk = pbd->ltk;
		ke_msg_send(cmd);
		arch_printf("Send GAPC_ENCRYPT_CMD\n\r");	
	}
	else{ //Device not bonded, run pairing procedule
		user_security_send_bond_req(conidx, user_security_conf);
	}
}

/**
 ****************************************************************************************
 * @brief Call back when peripheral missing previously stored bonding info and refused the encryption
 *
 ****************************************************************************************
 */
void user_on_enc_key_missing(void){
	arch_printf("Encryption key missing on periph side.\n\r");
	default_app_bdb_remove_entry(SEARCH_BY_BDA_TYPE,
                                 REMOVE_THIS_ENTRY,
                                 (void *)app_env[ret_conidx].peer_addr.addr, BD_ADDR_LEN);
	//try to pair up again if peripheral device does not disconnect immediately
	user_security_send_bond_req(ret_conidx, user_security_conf);
}

/**
 ****************************************************************************************
 * @brief Indication of a successful encryption on the connection
 *
 ****************************************************************************************
 */
void user_on_encrypt_ind(const uint8_t conidx, const uint8_t auth){
	arch_printf("\n\rEncrypted link established.\n\r");
}

/**
 ****************************************************************************************
 * @brief Callback when received security request from periheral device
 *
 ****************************************************************************************
 */
void user_on_security_req_ind(uint8_t auth){
#ifndef CFG_SECURITY_ACTIVE
	arch_printf("\n\rRecieved security request.\n\r");
	user_security_send_encrypt_cmd(ret_conidx);
#endif
}

/**
 ****************************************************************************************
 * @brief TK exchange handler, same as in the peripheral side
 *
 ****************************************************************************************
 */
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
        arch_printf("\r\nEnter Passkey: %u", passkey);
        #else
        uint32_t passkey = APP_SECURITY_MITM_PASSKEY_VAL;
        #endif

        app_easy_security_tk_exch(conidx, (uint8_t*) &passkey, sizeof(passkey), accept);
    }
    else if (param->data.tk_type == GAP_TK_KEY_ENTRY)
    {
        uint32_t passkey = APP_SECURITY_MITM_PASSKEY_VAL;

        #if defined (CFG_PRINTF)
        arch_printf("\r\nPasskey Entered: %u", passkey);
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
        arch_printf("\r\nConfirmation Value: ");
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

/**
 ****************************************************************************************
 * @brief Callback when a pairing process successfully comepleted
 *
 ****************************************************************************************
 */
void user_on_pairing_succeeded(uint8_t conidx){
	arch_printf("\r\nPairing succeeded.");
	default_app_on_pairing_succeeded(conidx);
}