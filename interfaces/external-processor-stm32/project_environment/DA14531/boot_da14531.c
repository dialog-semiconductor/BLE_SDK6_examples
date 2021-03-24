/*******************************************************************************************************/
/*INCLUDES *********************************************************************************************/
/*******************************************************************************************************/
#include "boot_da14531.h"
#include "boot_config.h"

/*******************************************************************************************************/
/*DEFINES **********************************************************************************************/
/*******************************************************************************************************/
#define ACK 		(uint8_t)(0x06)
#define NACK		(uint8_t)(0x15)
#define STX			(uint8_t)(0x02)
#define SOH			(uint8_t)(0x01)
#define HEADER_SIZE		(size_t)(3)

/*******************************************************************************************************/
/*FUNCTION PROTOTYPES **********************************************************************************/
/*******************************************************************************************************/
static codeless_boot_error_t wait_for_start_of_boot(UART_HandleTypeDef*);
static codeless_boot_error_t send_header(UART_HandleTypeDef*, uart_wire_t, uint16_t);
static codeless_boot_error_t send_data(UART_HandleTypeDef*, uart_wire_t, const uint8_t*, const uint16_t);
static codeless_boot_error_t check_crc(UART_HandleTypeDef*, uart_wire_t, const uint8_t);

/*******************************************************************************************************/
/*FUNCTION DEFINITIONS *********************************************************************************/
/*******************************************************************************************************/

/**
*	@brief The main function used for booting the DA14531
*	@param[in] uarth - a pointer to the UART handle that is used for the UART communication with the DA14531
*	@param[in] mode - sets the data mode for the UART communication, either ONE_WIRE or TWO_WIRE
*	@param[in] crc - the CRC for the given data
*	@param[in] pData - a pointer to the data that needs to be send to the DA14531
*	@parma[in] dataSize - the size of the data to be send
*	@return returns a ok if everything went succesful, else it returns an error
*/
codeless_boot_error_t boot_da14531(UART_HandleTypeDef * uarth, uart_wire_t mode, uint8_t crc, const uint8_t * pData, const uint16_t dataSize){

	if(wait_for_start_of_boot(uarth) == BOOT_ERROR) 						return NO_BOOT_RESPONSE;
	if(send_header(uarth, mode, dataSize) == BOOT_ERROR) 				return HEADER_NACK;
	if(send_data(uarth, mode, pData, dataSize) == BOOT_ERROR) 	return DATA_TRANSMIT_ERROR;
	if(check_crc(uarth, mode, crc) == BOOT_ERROR) 							return CRC_INCORRECT;

	return BOOT_OK;
}

/**
*	@brief A function used to calculate the CRC of the given data
*	@param[in] pData - a pointer to the data for which the CRC needs to be calculated
*	@param[in] dataSize - the size of the data
*	@return returns the calculated CRC
*/
uint8_t crc_calculate(const uint8_t * pData, const uint16_t dataSize){
	uint8_t crc = 0x00;

	size_t i;

	for (i = 0; i < dataSize; ++i) {
		crc ^= pData[i];
	}

	return crc;
}

/**
*	@brief This function waits for the start of transmission byte from the DA14531
*	@param[in] uarth - a pointer to the UART handle that is used for the UART communication with the DA14531
*	@return returns a ok if everything went succesful, else it returns an error
*/
static codeless_boot_error_t wait_for_start_of_boot(UART_HandleTypeDef * uarth){
	uint8_t uartBuffer = 0x0;
	HAL_StatusTypeDef uartStatus;
	uint16_t bootAttempts = BOOT_ATTEMPTS;

	do{
		uartStatus = HAL_UART_Receive(uarth, &uartBuffer, sizeof(uartBuffer), TIMEOUT_TIME); //Get the start of boot code
	}while(uartBuffer != STX && -- bootAttempts > 0); //Check for boot code else restart process

	if(uartStatus != HAL_OK || uartBuffer != STX){
		return BOOT_ERROR;
	}

	return BOOT_OK;
}

/**
*	@brief This function sends the header for the data to the DA14531 and waits for the ACK
*	@param[in] uarth - a pointer to the UART handle that is used for the UART communication with the DA14531
*	@param[in] mode - sets the data mode for the UART communication, either ONE_WIRE or TWO_WIRE
*	@param[in] dataSize - the size of the data that is the be send
*	@return returns a ok if everything went succesful, else it returns an error
*/
static codeless_boot_error_t send_header(UART_HandleTypeDef * uarth, uart_wire_t mode, uint16_t dataSize){
	HAL_StatusTypeDef uartStatus;
	uint8_t header[HEADER_SIZE];
	uint8_t buffer;
	size_t i;

	//Prepare header
	header[0] = SOH;
	header[1] = (uint8_t)(dataSize & 0xFF); //Get lower byte of the size
	header[2] = (uint8_t)((dataSize >> 8) & 0xFF); //Get upper byte of the size

	i = 0;
	while(i < HEADER_SIZE){
		uartStatus = HAL_UART_Transmit(uarth, &header[i], sizeof(uint8_t), TIMEOUT_TIME);

		if(mode == ONE_WIRE){
			HAL_UART_Receive(uarth, &buffer, sizeof(buffer), TIMEOUT_TIME);//Because of 1 wire UART the buffer needs to be cleared after a transmit
		}
		i++;
	}

	//Wait for ACK or NACK
	uartStatus = HAL_UART_Receive(uarth, &buffer, sizeof(buffer), TIMEOUT_TIME);
	if(uartStatus != HAL_OK || buffer != ACK){
		return BOOT_ERROR;
	}

	return BOOT_OK;
}

/**
*	@brief This function sends the data to the DA14531
*	@param[in] uarth - a pointer to the UART handle that is used for the UART communication with the DA14531
*	@param[in] mode - sets the data mode for the UART communication, either ONE_WIRE or TWO_WIRE
*	@param[in] pData - a pointer to the data that needs to be send to the DA14531
*	@param[in] dataSize - the size of the data that is the be send
*	@return returns a ok if everything went succesful, else it returns an error
*/
static codeless_boot_error_t send_data(UART_HandleTypeDef* uarth, uart_wire_t mode, const uint8_t* pData, const uint16_t dataSize){
	HAL_StatusTypeDef uartStatus;
	uint16_t i;
	uint8_t clearBuffer;

	i = 0;
	while(i < dataSize){
		uartStatus = HAL_UART_Transmit(uarth, (unsigned char *)&pData[i], sizeof(uint8_t), TIMEOUT_TIME);

		if(mode == ONE_WIRE){
			HAL_UART_Receive(uarth, &clearBuffer, sizeof(clearBuffer), TIMEOUT_TIME);//Because of 1 wire UART the buffer needs to be cleared after a transmit
		}

		i++;
	}

	if(uartStatus != HAL_OK){
		return BOOT_ERROR;
	}

	return BOOT_OK;
}

/**
*	@brief This function gets the CRC from the DA14531 and checks if it matches the given CRC
*	@param[in] uarth - a pointer to the UART handle that is used for the UART communication with the DA14531
*	@param[in] mode - sets the data mode for the UART communication, either ONE_WIRE or TWO_WIRE
*	@param[in] crc - the calculated crc for the data
*	@return returns a ok if everything went succesful, else it returns an error
*/
static codeless_boot_error_t check_crc(UART_HandleTypeDef* uarth, uart_wire_t mode, const uint8_t crc){
	HAL_StatusTypeDef uartStatus;
	uint8_t crc_buffer;

	uartStatus = HAL_UART_Receive(uarth, &crc_buffer, sizeof(crc_buffer), TIMEOUT_TIME); //Get the crc byte

	if(uartStatus != HAL_OK || crc_buffer != crc){
		return BOOT_ERROR;
	}else{
		crc_buffer = ACK;
		uartStatus = HAL_UART_Transmit(uarth, &crc_buffer, sizeof(crc_buffer), TIMEOUT_TIME);

		if(mode == ONE_WIRE){
			HAL_UART_Receive(uarth, &crc_buffer, sizeof(crc_buffer), TIMEOUT_TIME);//Because of 1 wire UART the buffer needs to be cleared after a transmit
		}
	}

	if(uartStatus != HAL_OK){
		return BOOT_ERROR;
	}

	return BOOT_OK;
}

