#ifndef _BOOT_DA14531_H_
#define _BOOT_DA14531_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

/*******************************************************************************************************/
/*TYPE  DEFINITIONS ************************************************************************************/
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
