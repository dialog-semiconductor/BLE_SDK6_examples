/*******************************************************************************************************/
/*INCLUDES *********************************************************************************************/
/*******************************************************************************************************/
#include "boot_da14531.h"
#include "boot_config.h"
#include "common_utils.h"

/*******************************************************************************************************/
/*DEFINES **********************************************************************************************/
/*******************************************************************************************************/
#define ACK 		(uint8_t)(0x06)
#define NACK		(uint8_t)(0x15)
#define STX			(uint8_t)(0x02)
#define SOH			(uint8_t)(0x01)
#define HEADER_SIZE		(size_t)(3)

extern volatile uint8_t g_uart_event;
#define UART_ERROR_EVENTS         (UART_EVENT_BREAK_DETECT | UART_EVENT_ERR_OVERFLOW | UART_EVENT_ERR_FRAMING | \
                                    UART_EVENT_ERR_PARITY)    /* UART Error event bits mapped in registers */
/*******************************************************************************************************/
/*FUNCTION PROTOTYPES **********************************************************************************/
/*******************************************************************************************************/
static codeless_boot_error_t wait_for_start_of_boot(sci_uart_instance_ctrl_t *);
static codeless_boot_error_t send_header(sci_uart_instance_ctrl_t * , uart_wire_t, uint16_t);
static codeless_boot_error_t send_data(sci_uart_instance_ctrl_t * , uart_wire_t, const uint8_t*, const uint16_t);
static codeless_boot_error_t check_crc(sci_uart_instance_ctrl_t * , uart_wire_t, const uint8_t);

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

codeless_boot_error_t boot_da14531(sci_uart_instance_ctrl_t * p_ctrl , uart_wire_t mode, uint8_t crc, const uint8_t * pData, const uint16_t dataSize){

	if(wait_for_start_of_boot(&g_uart0_ctrl) == BOOT_ERROR) 						return NO_BOOT_RESPONSE;
	if(send_header(&g_uart0_ctrl, mode, dataSize) == BOOT_ERROR) 				return HEADER_NACK;
  if(send_data(&g_uart0_ctrl, mode, pData, dataSize) == BOOT_ERROR) 	return DATA_TRANSMIT_ERROR;
  if(check_crc(&g_uart0_ctrl, mode, crc) == BOOT_ERROR) 							return CRC_INCORRECT;

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
static codeless_boot_error_t wait_for_start_of_boot(sci_uart_instance_ctrl_t * p_ctrl ){

	#define TRANSFER_LENGTH_1            (1U)
uint8_t  g_dest_0;
	
//	#define BUF_LEN            (1U)
//	uint8_t  uartBuffer[BUF_LEN] = {0x00};
//	//fsp_err_t uartStatus  = FSP_SUCCESS; ;
//	uint16_t bootAttempts = BOOT_ATTEMPTS;
//	
//    /* Open the transfer instance with initial configuration. */
//    fsp_err_t uartStatus = R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
//	
//	do{
//		uartStatus = R_SCI_UART_Read(&g_uart0_ctrl,  uartBuffer, BUF_LEN); //Get the start of boot code
//		}while(uartBuffer != (uint8_t * const)STX && -- bootAttempts > 0); //Check for boot code else restart process

//	if(uartStatus != FSP_SUCCESS || uartBuffer != (uint8_t * const)STX){
//		return BOOT_ERROR;
//	}

//	return BOOT_OK;

//#define TRANSFER_LENGTH_1            (1U)
//uint8_t  g_dest[TRANSFER_LENGTH_1] = {0x00};
uint16_t bootAttempts = BOOT_ATTEMPTS;

//	uint8_t  uartBuffer = 0x00;
fsp_err_t uartStatus  = FSP_SUCCESS; ;
//	uint16_t bootAttempts = BOOT_ATTEMPTS;
	
    /* Open the transfer instance with initial configuration. */
    //fsp_err_t uartStatus = R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
		
    assert(FSP_SUCCESS == uartStatus);
	
    do{
		uartStatus = R_SCI_UART_Read(&g_uart0_ctrl, &g_dest_0, TRANSFER_LENGTH_1); //Get the start of boot code

		
		assert(FSP_SUCCESS == uartStatus);
			
		}while(g_dest_0 != STX && -- bootAttempts > 0); //Check for boot code else restart process

	if(uartStatus != FSP_SUCCESS || g_dest_0 != STX){
		
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
static codeless_boot_error_t send_header(sci_uart_instance_ctrl_t * p_ctrl, uart_wire_t mode, uint16_t dataSize){

#define TRANSFER_LENGTH_2            (1U)
uint8_t  g_src_h;
	
	fsp_err_t uartStatus  = FSP_SUCCESS;
	uint8_t header[HEADER_SIZE];

	//Prepare header
	header[0] = SOH;
	header[1] = (uint8_t)(dataSize & 0xFF); //Get lower byte of the size
	header[2] = (uint8_t)((dataSize >> 8) & 0xFF); //Get upper byte of the size
	size_t i;
//	/* Initialize p_src to known data */
//    for (uint32_t i = 0; i < HEADER_SIZE; i++)
//    {
//        g_src_h[i] = header[i];
//    }
  i = 0;
	while(i < HEADER_SIZE){
		uartStatus = R_SCI_UART_Write(&g_uart0_ctrl, &header[i], sizeof(uint8_t));
		i++;
	}
	
	    /* Check for event transfer complete */
    while ((UART_EVENT_TX_COMPLETE != g_uart_event))
    {
        /* Check if any error event occurred */
        if (UART_ERROR_EVENTS == g_uart_event)
        {
          APP_ERR_PRINT ("\r\n**  FUCK  ** \r\n");
        }
    }
	
	
	
	
	//uartStatus = R_SCI_UART_Write(&g_uart0_ctrl, g_src_h, TRANSFER_LENGTH_2);
	
//		if(mode == ONE_WIRE){
//	R_SCI_UART_Read(&g_uart0_ctrl, g_dest, sizeof(g_dest));//Because of 1 wire UART the buffer needs to be cleared after a transmit
//		
//	}
	
	//Wait for ACK or NACK
	uartStatus = R_SCI_UART_Read(&g_uart0_ctrl, &g_src_h, sizeof(g_src_h));

//	if(uartStatus != FSP_SUCCESS || g_dest != (uint8_t * const)ACK){
//		return BOOT_ERROR;
//	}

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
static codeless_boot_error_t send_data(sci_uart_instance_ctrl_t * p_ctrl, uart_wire_t mode, const uint8_t* pData, const uint16_t dataSize){

	
fsp_err_t uartStatus  = FSP_SUCCESS;
//uint8_t  g_src[dataSize];
	
	/* Initialize p_src to known data */
//    for (uint32_t i = 0; i < dataSize; i++)
//    {
//       uartStatus = R_SCI_UART_Write(&g_uart0_ctrl, pData, dataSize);
//    }	
	
	size_t i;
		

	i = 0;
	while(i < dataSize){
		       uartStatus = R_SCI_UART_Write(&g_uart0_ctrl, (unsigned char *)&pData[i], dataSize);

//		if(mode == ONE_WIRE){
//			R_SCI_UART_Read(p_ctrl, &clearBuffer, sizeof(clearBuffer));//Because of 1 wire UART the buffer needs to be cleared after a transmit
//		}

		i++;
	}
	
	
		    /* Check for event transfer complete */
    while ((UART_EVENT_TX_COMPLETE != g_uart_event))
    {
        /* Check if any error event occurred */
        if (UART_ERROR_EVENTS == g_uart_event)
        {
          APP_ERR_PRINT ("\r\n**  FUCK  ** \r\n");
        }
    }

//	if(uartStatus != FSP_SUCCESS){
//		return BOOT_ERROR;
//	}

	return BOOT_OK;
	
	
}

/**
*	@brief This function gets the CRC from the DA14531 and checks if it matches the given CRC
*	@param[in] uarth - a pointer to the UART handle that is used for the UART communication with the DA14531
*	@param[in] mode - sets the data mode for the UART communication, either ONE_WIRE or TWO_WIRE
*	@param[in] crc - the calculated crc for the data
*	@return returns a ok if everything went succesful, else it returns an error
*/
static codeless_boot_error_t check_crc(sci_uart_instance_ctrl_t * p_ctrl, uart_wire_t mode, const uint8_t crc){
//	fsp_err_t uartStatus  = FSP_SUCCESS; 
//	uint8_t crc_buffer;

//	uartStatus = R_SCI_UART_Read(p_ctrl, &crc_buffer, sizeof(crc_buffer)); //Get the crc byte

//	if(uartStatus != FSP_SUCCESS || crc_buffer != crc){
//		return BOOT_ERROR;
//	}else{
//		crc_buffer = ACK;
//		uartStatus = R_SCI_UART_Write(&p_ctrl, &crc_buffer, sizeof(crc_buffer));

//		if(mode == ONE_WIRE){
//			R_SCI_UART_Read(&p_ctrl, &crc_buffer, sizeof(crc_buffer));//Because of 1 wire UART the buffer needs to be cleared after a transmit
//		}
//	}

//	if(uartStatus != FSP_SUCCESS){
//		return BOOT_ERROR;
//	}

//	return BOOT_OK;
#define TRANSFER_LENGTH_3            (1U)
//uint8_t  g_dest_22[TRANSFER_LENGTH_3] = {0x00};


  fsp_err_t uartStatus  = FSP_SUCCESS; 

 //Get the crc byte
//  uartStatus = R_SCI_UART_Read(&g_uart0_ctrl, g_dest_22, TRANSFER_LENGTH_3);
	//uartStatus = R_SCI_UART_Read(&g_uart0_ctrl, g_dest_22, TRANSFER_LENGTH_3);
  
//	assert(FSP_SUCCESS == uartStatus);


//	if(uartStatus != FSP_SUCCESS || g_dest[0] != crc){
//		return BOOT_ERROR;
//	}else{
//		g_dest_22[0] = 0x06;
//		uartStatus = R_SCI_UART_Write(&g_uart0_ctrl, g_dest_22, TRANSFER_LENGTH_3);

//		if(mode == ONE_WIRE){
//			R_SCI_UART_Read(&g_uart0_ctrl, &crc_buffer, sizeof(crc_buffer));//Because of 1 wire UART the buffer needs to be cleared after a transmit
//		}
//	}

//	if(uartStatus != FSP_SUCCESS){
//		return BOOT_ERROR;
//	}

	uint8_t crc_buffer = 0;

	uartStatus = R_SCI_UART_Read(&g_uart0_ctrl, &crc_buffer, TRANSFER_LENGTH_3); //Get the crc byte
		
	if(uartStatus != FSP_SUCCESS || crc_buffer != crc){
		return BOOT_ERROR;
	}else{
		//crc_buffer = ACK;
		uartStatus = R_SCI_UART_Write(&g_uart0_ctrl, &crc_buffer, TRANSFER_LENGTH_3);

//		if(mode == ONE_WIRE){
//			R_SCI_UART_Read(&g_uart0_ctrl, &crc_buffer, sizeof(crc_buffer));//Because of 1 wire UART the buffer needs to be cleared after a transmit
//		}
	}

	if(uartStatus != FSP_SUCCESS){
		return BOOT_ERROR;
	}



	return BOOT_OK;


}

