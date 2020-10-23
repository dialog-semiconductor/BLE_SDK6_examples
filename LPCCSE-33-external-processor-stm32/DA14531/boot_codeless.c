/*******************************************************************************************************/
/*INCLUDES *********************************************************************************************/
/*******************************************************************************************************/
#include "boot_codeless.h"
#include "codeless_config.h"

#if defined(CODELESS_SA)
#include "codeless_binaries/codeless_sa_bin.h"
#elif defined(CODELESS_SA_2)
#include "codeless_binaries/codeless_sa_2_bin.h"
#elif defined(CODLESS_DP)
#include "codeless_binaries/codeless_dp_bin.h"
#endif

/*******************************************************************************************************/
/*DEFINES **********************************************************************************************/
/*******************************************************************************************************/
#define ACK 		(uint8_t)(0x06)
#define NACK		(uint8_t)(0x15)
#define STX			(uint8_t)(0x02)
#define SOH			(uint8_t)(0x01)

#define HEADER_SIZE	(size_t)(3)

/*******************************************************************************************************/
/*FUNCTION PROTOTYPES **********************************************************************************/
/*******************************************************************************************************/
static uint8_t crc_calculate(const uint8_t *, uint16_t);
static codeless_boot_error_t wait_for_start_of_boot(UART_HandleTypeDef*, GPIO_TypeDef*, const uint16_t);
static codeless_boot_error_t send_header(UART_HandleTypeDef*, uart_wire_t, uint16_t);
static codeless_boot_error_t send_data(UART_HandleTypeDef*, uart_wire_t, const uint8_t*, const uint16_t);
static codeless_boot_error_t check_crc(UART_HandleTypeDef*, uart_wire_t, const uint8_t);

/*******************************************************************************************************/
/*FUNCTION DEFINITIONS *********************************************************************************/
/*******************************************************************************************************/

codeless_boot_error_t boot_codeless(UART_HandleTypeDef * uarth, uart_wire_t mode, GPIO_TypeDef* resetPort, uint16_t resetPin){
	const uint16_t codelessSize = sizeof(codelessData);
	uint8_t crc;

	crc = crc_calculate(codelessData, codelessSize);//Calculate crc for the bootcode

	if(wait_for_start_of_boot(uarth,resetPort,resetPin) == BOOT_ERROR) 		return NO_BOOT_RESPONSE;
	if(send_header(uarth, mode, codelessSize) == BOOT_ERROR) 				return HEADER_NACK;
	if(send_data(uarth, mode, codelessData, codelessSize) == BOOT_ERROR) 	return DATA_TRANSMIT_ERROR;
	if(check_crc(uarth, mode, crc) == BOOT_ERROR) 							return CRC_INCORRECT;

	return BOOT_OK;
}

static uint8_t crc_calculate(const uint8_t * pCodelessData, const uint16_t codelessSize){
	uint8_t crc;
	size_t i;

	crc = 0x00;
	for (i = 0; i < codelessSize; ++i) {
		crc ^= pCodelessData[i];
	}

	return crc;
}

static codeless_boot_error_t wait_for_start_of_boot(UART_HandleTypeDef * uarth, GPIO_TypeDef * resetPort, uint16_t resetPin){
	uint8_t uartBuffer;
	HAL_StatusTypeDef uartStatus;
	uint8_t bootAttempts;

	bootAttempts = 10;

	do{
		HAL_GPIO_WritePin(resetPort, resetPin, GPIO_PIN_SET);	//Reset the DA14531
		HAL_GPIO_WritePin(resetPort, resetPin, GPIO_PIN_RESET);
		HAL_Delay(10);
		uartStatus = HAL_UART_Receive(uarth, &uartBuffer, sizeof(uartBuffer), TIMEOUT_TIME_MS); //Get the start of boot code
	}while(uartBuffer != STX && -- bootAttempts > 0); //Check for boot code else restart process

	if(uartStatus != HAL_OK || uartBuffer != STX){
		return BOOT_ERROR;
	}

	HAL_GPIO_WritePin(resetPort, resetPin, RESET);

	return BOOT_OK;
}

static codeless_boot_error_t send_header(UART_HandleTypeDef * uarth, uart_wire_t mode, uint16_t codelessSize){
	HAL_StatusTypeDef uartStatus;
	uint8_t header[HEADER_SIZE];
	uint8_t buffer;
	size_t i;

	//Prepare header
	header[0] = SOH;
	header[1] = (uint8_t)(codelessSize & 0xFF); //Get lower byte of the size
	header[2] = (uint8_t)((codelessSize >> 8) & 0xFF); //Get upper byte of the size

	i = 0;
	while(i < sizeof(header)){
		HAL_UART_Transmit(uarth, &header[i], sizeof(uint8_t), TIMEOUT_TIME_MS);

		if(mode == ONE_WIRE){
			HAL_UART_Receive(uarth, &buffer, sizeof(buffer), TIMEOUT_TIME_MS);//Because of 1 wire UART the buffer needs to be cleared after a transmit
		}
		i++;
	}

	//Wait for ACK or NACK
	uartStatus = HAL_UART_Receive(uarth, &buffer, sizeof(buffer), TIMEOUT_TIME_MS);
	if(uartStatus != HAL_OK || buffer != ACK){
		return BOOT_ERROR;
	}

	return BOOT_OK;
}

static codeless_boot_error_t send_data(UART_HandleTypeDef* uarth, uart_wire_t mode, const uint8_t* pCodelessData, const uint16_t codelessSize){
	HAL_StatusTypeDef uartStatus;
	uint16_t i;
	uint8_t clearBuffer;

	i = 0;
	while(i < codelessSize){
		uartStatus = HAL_UART_Transmit(uarth, (unsigned char *)&pCodelessData[i], sizeof(uint8_t), TIMEOUT_TIME_MS);

		if(mode == ONE_WIRE){
			HAL_UART_Receive(uarth, &clearBuffer, sizeof(clearBuffer), TIMEOUT_TIME_MS);//Because of 1 wire UART the buffer needs to be cleared after a transmit
		}

		i++;
	}

	if(uartStatus != HAL_OK){
		return BOOT_ERROR;
	}

	return BOOT_OK;
}
static codeless_boot_error_t check_crc(UART_HandleTypeDef* uarth, uart_wire_t mode, const uint8_t crc){
	HAL_StatusTypeDef uartStatus;
	uint8_t crc_buffer;

	uartStatus = HAL_UART_Receive(uarth, &crc_buffer, sizeof(crc_buffer), TIMEOUT_TIME_MS); //Get the crc byte

	if(uartStatus != HAL_OK || crc_buffer != crc){
		return BOOT_ERROR;
	}else{
		crc_buffer = ACK;
		uartStatus = HAL_UART_Transmit(uarth, &crc_buffer, sizeof(crc_buffer), TIMEOUT_TIME_MS);

		if(mode == ONE_WIRE){
			HAL_UART_Receive(uarth, &crc_buffer, sizeof(crc_buffer), TIMEOUT_TIME_MS);//Because of 1 wire UART the buffer needs to be cleared after a transmit
		}
	}

	if(uartStatus != HAL_OK){
		return BOOT_ERROR;
	}

	return BOOT_OK;
}

