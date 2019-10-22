/**
 ****************************************************************************************
 *
 * @file aes_api.c
 *
 * @brief AES.
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
 ****************************************************************************************
 */

#include <stdint.h>
#include "rwip.h"
#include "sw_aes.h"
#include "gapm_task.h"
#include "aes.h"
#include "aes_api.h"
#include "llm.h"
#include "reg_blecore.h"
#include "co_bt.h"

/* using a dummy IV vector filled with zeroes for the software decryption since the chip does not use IV for encryption */
extern uint8_t IV[];

int aes_set_key(const uint8_t *userKey, const uint32_t bits, AES_KEY *key, uint8_t enc_dec)
{
    if(enc_dec == AES_ENCRYPT)
    {
        uint32_t *rk;
        rk = key->ks;
        rk[0] = GETU32(userKey     );
        rk[1] = GETU32(userKey +  4);
        rk[2] = GETU32(userKey +  8);
        rk[3] = GETU32(userKey + 12);

        return 0;
    }
    if (enc_dec == AES_DECRYPT)
    {
        if (bits == 128)
           AES_set_key(key, userKey, IV, AES_MODE_128);
      else if (bits == 256)
           AES_set_key(key, userKey, IV, AES_MODE_256);

      AES_convert_key(key);
    }
    return 0; /* now we always return 0 since the handling functions changed to void */
}


static void reverse_uint8memcpy(uint8_t * dst, const uint8_t * src, size_t n)
{
    while (n--) *dst++ = *src--;
}

int aes_enc_dec(uint8_t *in, uint8_t *out, AES_KEY *key, uint8_t enc_dec, uint8_t ble_flags)
{
    if(ble_flags & BLE_SAFE_MASK)
        rwip_schedule();

    if(enc_dec == AES_ENCRYPT)
    {
        // Check if an encryption is pending
        if (llm_le_env.enc_pend)
            return -1;

        // copy the key in the register dedicated for the encryption
        ble_aeskey31_0_set(key->ks[3]);
        ble_aeskey63_32_set(key->ks[2]);
        ble_aeskey95_64_set(key->ks[1]);
        ble_aeskey127_96_set(key->ks[0]);

        // copy data from sys ram to exchange memory (in reverse order)
        reverse_uint8memcpy((uint8_t *)(EM_BLE_ENC_PLAIN_OFFSET + EM_BASE_ADDR), &in[15], ENC_DATA_LEN);

        // set the pointer on the data to encrypt.
        ble_aesptr_set(EM_BLE_ENC_PLAIN_OFFSET);

        // start the encryption
        ble_aescntl_set(BLE_AES_START_BIT);

        while(GetWord32(BLE_AESCNTL_REG)==1)
            if(ble_flags & BLE_SAFE_MASK)
                rwip_schedule();

        // copy data from em to sys ram
        em_rd(&out[0], EM_BLE_ENC_CIPHER_OFFSET, ENC_DATA_LEN);
    }
    else
    {
#if USE_AES_DECRYPT
        uint8_t j;

        uint32_t temp[4];
        
        for(j = 0; j < ENC_DATA_LEN; j += 4)
        {
            temp[j/4] = GETU32(&in[j]);
        }
        
        AES_decrypt(key, &temp[0]);
        //decrypt is performed in place, so copy to out afterwards
        for(j = 0 ; j < ENC_DATA_LEN ; j += 4)
        {
            PUTU32(&out[j], temp[j/4]);
        }
        
#else
        return -6;
#endif
    }

    return 0;
}
