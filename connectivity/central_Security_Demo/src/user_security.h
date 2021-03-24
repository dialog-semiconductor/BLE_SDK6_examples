/**
 ****************************************************************************************
 *
 * @file user_security.h
 *
 * @brief Security functions header file.
 *
 * Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs
 * included in this package ("Software") include confidential, proprietary information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR ("DIALOG") AND MAY
 * ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND.  
 * THIS SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS.  
 * RECIPIENT SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT 
 * DIALOG'S PRIOR WRITTEN PERMISSION.
 *
 * UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT
 * TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED "AS IS", WITHOUT 
 * WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, 
 * BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, 
 * MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM 
 * COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS 
 * AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY 
 * REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED
 * SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, 
 * AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY 
 * OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED 
 * BY THE FCC OR OTHER LIKE AGENCIES.
 *
 ****************************************************************************************
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
