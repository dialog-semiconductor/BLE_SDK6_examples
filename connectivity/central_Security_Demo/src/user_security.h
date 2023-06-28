/**
 ****************************************************************************************
 *
 * @file user_security.h
 *
 * @brief Security functions header file.
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


#ifndef _USER_SECURITY_H_
#define _USER_SECURITY_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_APP_SEC)

#include "co_bt.h"
#include "gap.h"                // GAP API Definition
#include "gapc_task.h"          // GAP Controller Task API Definition
#include "gapm_task.h"          // GAP Manager Task API Definition
#include "app_utils.h"
#include "app_security.h"


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct extra_app_callbacks
{
	void (*app_on_encrypt_key_missing)(void);
};

/// Passkey that is presented to the user and is entered on the peer device (MITM)
#define APP_SECURITY_MITM_PASSKEY_VAL    (123456)

/// OOB data
#define APP_SECURITY_OOB_TK_VAL          {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,\
                                          0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}


void user_security_send_bond_req(uint8_t conidx, struct security_configuration sec_conf);

void user_app_on_tk_exch(uint8_t conidx,
                         struct gapc_bond_req_ind const * param);
void user_security_send_encrypt_cmd(uint8_t conidx);
												 
void user_on_pairing_succeeded(uint8_t conidx);

void user_on_encrypt_ind(const uint8_t conidx, const uint8_t auth);

void user_on_security_req_ind(uint8_t conidx);

void user_on_enc_key_missing(void);
								
#endif // (BLE_APP_SEC)

#endif // _USER_SECURITY_H_