/**
 ****************************************************************************************
 *
 * @file boot_codeless.h
 *
 * @brief Codeless boot process header file.
 *
 * Copyright (c) 2012-2020 Dialog Semiconductor. All rights reserved.
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

#ifndef _BOOT_CODELESS_H_
#define _BOOT_CODELESS_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef enum{
	BOOT_OK, 				//The boot went successful
	BOOT_ERROR,				//The boot went wrong for unknown reasons
	NO_BOOT_RESPONSE,	//Could not find the start of boot code
	HEADER_NACK,			//The header was not acknowledged by the DA14531
	DATA_TRANSMIT_ERROR,	//An error occurred during the transfer of the codeless binary
	CRC_INCORRECT			//The CRC did not match the one from the DA14531
}codeless_boot_error_t;

typedef enum{
	ONE_WIRE,
	TWO_WIRE
}uart_wire_t;

/**
 ****************************************************************************************
 * @brief Start the boot process to boot codeless to the DA14531
 * @param[in] uarth - pointer to a HAL UART handle
 * @param[in] mode - select which uart mode to use to boot codeless
 * @param[in] resetPort - pointer to the port of the reset pin
 * @param[in] resetPin - pin number of the reset pin
 * @return an ok or error for the boot process
 ****************************************************************************************
 */
codeless_boot_error_t boot_codeless(UART_HandleTypeDef*, uart_wire_t, GPIO_TypeDef*, uint16_t);

#endif

/// @} APP
