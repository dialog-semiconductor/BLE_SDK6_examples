/**
 ****************************************************************************************
 *
 * @file aes_cmac.h
 *
 * @brief AES-CMAC header file.
 *
 * Copyright (c) 2018 Dialog Semiconductor. All rights reserved.
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

#ifndef AES_CMAC_H_
#define AES_CMAC_H_

/*
 * DEFINES
 ****************************************************************************************
 */

// Input block size (128 bits)
#define AES_CMAC_BLK_SIZE_128 (16)

/*
 * PUBLIC FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Generate tag using AES-CMAC
 * @param[in] payload           Input payload
 * @param[in] payload_len       Payload length (in bytes)
 * @param[in] key               Key (128bit)
 * @param[out] mac              MAC output buffer
 * @param[in] mac_len           MAC buffer length (in bytes)
 * @return                      Error code if something goes wrong, 0 otherwise
 ****************************************************************************************
 */
uint8_t aes_cmac_generate(const uint8_t *payload, uint16_t payload_len,
                          const uint8_t *key, uint8_t *mac, uint8_t mac_len);

/**
 ****************************************************************************************
 * @brief Verify tag of the message using AES-CMAC
 * @param[in] payload           Input payload
 * @param[in] payload_len       Payload length (in bytes)
 * @param[in] key               Key (128bit)
 * @param[in] mac               MAC to verify
 * @param[in] mac_len           MAC buffer length (in bytes)
 * @return                      True if provided mac was successfully verified, False otherwise
 ****************************************************************************************
 */
bool aes_cmac_verify(const uint8_t *payload, uint16_t payload_len,
                     const uint8_t *key, const uint8_t *mac, uint8_t mac_len);

#endif // AES_CMAC_H_
