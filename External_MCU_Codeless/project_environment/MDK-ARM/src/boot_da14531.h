#ifndef _BOOT_DA14531_H_
#define _BOOT_DA14531_H_

#include <stdint.h>
#include "hal_data.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"

/*******************************************************************************************************/
/*TYPE  DEFINITIONS ************************************************************************************/
/*******************************************************************************************************/
#define UART_ERROR_EVENTS         (UART_EVENT_BREAK_DETECT | UART_EVENT_ERR_OVERFLOW | UART_EVENT_ERR_FRAMING | \
                                    UART_EVENT_ERR_PARITY)    /* UART Error event bits mapped in registers */
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

codeless_boot_error_t boot_da14531(sci_uart_instance_ctrl_t * p_ctrl , uart_wire_t mode, uint8_t crc, const uint8_t * pData, const uint16_t dataSize);
uint8_t crc_calculate(const uint8_t *, uint16_t);

#endif
