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

codeless_boot_error_t boot_codeless(UART_HandleTypeDef*, uart_wire_t, GPIO_TypeDef*, uint16_t);

#endif
