#ifndef _BOOT_DA14531_H_
#define _BOOT_DA14531_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

/*******************************************************************************************************/
/*TYPE  DEFINITIONS ************************************************************************************/
/*******************************************************************************************************/

/**
*	@brief An enumaration of the ok and error codes for the booting process
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
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
/*******************************************************************************************************/

/**
*	@brief An enumaration of the ok and error codes for the booting process
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
 * 
 * This software ("Software") is owned by Renesas Electronics Corporation. By using this
 * Software you agree that Renesas Electronics Corporation retains all intellectual
 * property and proprietary rights in and to this Software and any use, reproduction,
 * disclosure or distribution of the Software without express written permission or a
 * license agreement from Renesas Electronics Corporation is strictly prohibited. This
 * Software is solely for use on or in conjunction with Renesas Electronics Corporation
 * products.
 * 
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL RENESAS
 * ELECTRONICS CORPORATION BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ***************************************************************************************
 */
/*******************************************************************************************************/

/**
*	@brief An enumaration of the ok and error codes for the booting process
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
 * 
 * This software ("Software") is owned by Renesas Electronics Corporation. By using this
 * Software you agree that Renesas Electronics Corporation retains all intellectual
 * property and proprietary rights in and to this Software and any use, reproduction,
 * disclosure or distribution of the Software without express written permission or a
 * license agreement from Renesas Electronics Corporation is strictly prohibited. This
 * Software is solely for use on or in conjunction with Renesas Electronics Corporation
 * products.
 * 
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL RENESAS
 * ELECTRONICS CORPORATION BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ***************************************************************************************
 */
/*******************************************************************************************************/

/**
*	@brief An enumaration of the ok and error codes for the booting process
*/
typedef enum{
	BOOT_OK, 				///The boot went successful
	BOOT_ERROR,				///The boot went wrong for unknown reasons
	NO_BOOT_RESPONSE,		///Could not find the start of boot code
	HEADER_NACK,			///The header was not acknowledged by the DA14531
	DATA_TRANSMIT_ERROR,	///An error occurred during the transfer of the codeless binary
	CRC_INCORRECT			///The CRC did not match the one from the DA14531
}codeless_boot_error_t;

/**
*	@brief An enumaration of the UART modes
*/
typedef enum{
	ONE_WIRE,	///Use one wire UART
	TWO_WIRE	///Use two wire UART
}uart_wire_t;

/*******************************************************************************************************/
/*FUNCTION DECLARATIONS ********************************************************************************/
/*******************************************************************************************************/

codeless_boot_error_t boot_da14531(UART_HandleTypeDef*, uart_wire_t, uint8_t, const uint8_t *, const uint16_t);
uint8_t crc_calculate(const uint8_t *, uint16_t);

#endif