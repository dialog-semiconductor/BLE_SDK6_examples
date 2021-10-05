/*******************************************************************************************************/
/*INCLUDES *********************************************************************************************/
/*******************************************************************************************************/
#include "boot_da14531.h"
#include "boot_config.h"
#include "common_utils.h"

/*******************************************************************************************************/
/*DEFINES **********************************************************************************************/
/*******************************************************************************************************/


extern volatile uint8_t g_uart_event;

/*******************************************************************************************************/
/*FUNCTION PROTOTYPES **********************************************************************************/
/*******************************************************************************************************/
//static codeless_boot_error_t wait_for_start_of_boot(sci_uart_instance_ctrl_t *);
//static codeless_boot_error_t send_header(sci_uart_instance_ctrl_t * , uart_wire_t, uint16_t);
//static codeless_boot_error_t send_data(sci_uart_instance_ctrl_t * , uart_wire_t, const uint8_t*, const uint16_t);
//static codeless_boot_error_t check_crc(sci_uart_instance_ctrl_t * , uart_wire_t, const uint8_t);

/*******************************************************************************************************/
/*FUNCTION DEFINITIONS *********************************************************************************/
///*******************************************************************************************************/

///**
//*	@brief The main function used for booting the DA14531
//*	@param[in] uarth - a pointer to the UART handle that is used for the UART communication with the DA14531
//*	@param[in] mode - sets the data mode for the UART communication, either ONE_WIRE or TWO_WIRE
//*	@param[in] crc - the CRC for the given data
//*	@param[in] pData - a pointer to the data that needs to be send to the DA14531
//*	@parma[in] dataSize - the size of the data to be send
//*	@return returns a ok if everything went succesful, else it returns an error
//*/

//codeless_boot_error_t boot_da14531(sci_uart_instance_ctrl_t * p_api_ctrl , uart_wire_t mode, uint8_t crc, const uint8_t * pData, const uint16_t dataSize){


//	if(wait_for_start_of_boot(p_api_ctrl) == BOOT_ERROR) 						return NO_BOOT_RESPONSE;
//	if(send_header(p_api_ctrl, mode, dataSize) == BOOT_ERROR) 				return HEADER_NACK;
//  if(send_data(p_api_ctrl, mode, pData, dataSize) == BOOT_ERROR) 	return DATA_TRANSMIT_ERROR;
//  if(check_crc(p_api_ctrl, mode, crc) == BOOT_ERROR) 							return CRC_INCORRECT;

//	return BOOT_OK;
//}

///**
//*	@brief A function used to calculate the CRC of the given data
//*	@param[in] pData - a pointer to the data for which the CRC needs to be calculated
//*	@param[in] dataSize - the size of the data
//*	@return returns the calculated CRC
//*/
//uint8_t crc_calculate(const uint8_t * pData, const uint16_t dataSize){
//	uint8_t crc = 0x00;

//	size_t i;

//	for (i = 0; i < dataSize; ++i) {
//		crc ^= pData[i];
//	}

//	return crc;
//}

///**
//*	@brief This function waits for the start of transmission byte from the DA14531
//*	@param[in] uarth - a pointer to the UART handle that is used for the UART communication with the DA14531
//*	@return returns a ok if everything went succesful, else it returns an error
//*/
//static codeless_boot_error_t wait_for_start_of_boot(sci_uart_instance_ctrl_t * p_api_ctrl ){

//	sci_uart_instance_ctrl_t * p_ctrl = (sci_uart_instance_ctrl_t *) p_api_ctrl;
//		
//	#define TRANSFER_LENGTH_1            (1U)
//	uint8_t  g_dest_0;
//	

//	fsp_err_t uartStatus  = FSP_SUCCESS;

//	uartStatus = R_SCI_UART_Read(p_ctrl, &g_dest_0, TRANSFER_LENGTH_1); //Get the start of boot code
//	assert(FSP_SUCCESS == uartStatus);

//	while(UART_EVENT_RX_COMPLETE != g_uart_event);
//	assert(g_dest_0 == STX );
//	

//	return BOOT_OK;
//	
//}

///**
//*	@brief This function sends the header for the data to the DA14531 and waits for the ACK
//*	@param[in] uarth - a pointer to the UART handle that is used for the UART communication with the DA14531
//*	@param[in] mode - sets the data mode for the UART communication, either ONE_WIRE or TWO_WIRE
//*	@param[in] dataSize - the size of the data that is the be send
//*	@return returns a ok if everything went succesful, else it returns an error
//*/
//static codeless_boot_error_t send_header(sci_uart_instance_ctrl_t * p_api_ctrl, uart_wire_t mode, uint16_t dataSize){

//sci_uart_instance_ctrl_t * p_ctrl = (sci_uart_instance_ctrl_t *) p_api_ctrl;
//	
//#define TRANSFER_LENGTH_2            (1U)
//uint8_t  g_src_h;

//	/* Reset callback capture variable */
//  g_uart_event = RESET_VALUE;
//	#define HEADER_SIZE		(size_t)(3)
//	fsp_err_t uartStatus  = FSP_SUCCESS;
//	uint8_t header[HEADER_SIZE];

//	//Prepare header
//	header[0] = SOH;
//	header[1] = (uint8_t)(dataSize & 0xFF); //Get lower byte of the size
//	header[2] = (uint8_t)((dataSize >> 8) & 0xFF); //Get upper byte of the size

//	uartStatus = R_SCI_UART_Write(p_ctrl, &header[0],HEADER_SIZE);
//	
//	
//	    /* Check for event transfer complete */
//    while ((UART_EVENT_TX_COMPLETE != g_uart_event))
//    {
//        /* Check if any error event occurred */
//        if (UART_ERROR_EVENTS == g_uart_event)
//        {
//            APP_ERR_PRINT ("\r\n**  FUCK  **\r\n");
//            
//        }
//    }
//		
////		if(mode == ONE_WIRE){
////	R_SCI_UART_Read(p_ctrl, &g_src_h, sizeof(g_src_h));//Because of 1 wire UART the buffer needs to be cleared after a transmit
////		
////	}
//	

////	/* Check for event transfer complete */
////	while ((UART_EVENT_RX_COMPLETE != g_uart_event))
////	{
////			/* Check if any error event occurred */
////			if (UART_ERROR_EVENTS == g_uart_event)
////			{
////					APP_ERR_PRINT ("\r\n**  FUCK  **\r\n");
////					
////			}
////	}
//		
//		R_BSP_PinAccessEnable(); R_BSP_PinWrite(BSP_IO_PORT_09_PIN_15 , 1U); R_BSP_PinAccessDisable();
//		R_BSP_PinAccessEnable(); R_BSP_PinWrite(BSP_IO_PORT_09_PIN_15 , 0U); R_BSP_PinAccessDisable();
//		

//		//Wait for ACK or NACK
//	uartStatus = R_SCI_UART_Read(p_ctrl, &g_src_h, sizeof(g_src_h));

//	R_BSP_PinAccessEnable(); R_BSP_PinWrite(BSP_IO_PORT_09_PIN_15 , 1U); R_BSP_PinAccessDisable();
//	R_BSP_PinAccessEnable(); R_BSP_PinWrite(BSP_IO_PORT_09_PIN_15 , 0U); R_BSP_PinAccessDisable();

//	if(uartStatus != FSP_SUCCESS || g_src_h != ACK){
//		SEGGER_RTT_printf(0,"header %d %d \n\n",(const char *)uartStatus,g_src_h);
//		return BOOT_ERROR;
//	}

//	return BOOT_OK;


//}

///**
//*	@brief This function sends the data to the DA14531
//*	@param[in] uarth - a pointer to the UART handle that is used for the UART communication with the DA14531
//*	@param[in] mode - sets the data mode for the UART communication, either ONE_WIRE or TWO_WIRE
//*	@param[in] pData - a pointer to the data that needs to be send to the DA14531
//*	@param[in] dataSize - the size of the data that is the be send
//*	@return returns a ok if everything went succesful, else it returns an error
//*/
//static codeless_boot_error_t send_data(sci_uart_instance_ctrl_t * p_api_ctrl, uart_wire_t mode, const uint8_t* pData, const uint16_t dataSize){

//	sci_uart_instance_ctrl_t * p_ctrl = (sci_uart_instance_ctrl_t *) p_api_ctrl;
//		uint8_t clearBuffer;
//   /* Reset callback capture variable */
//    g_uart_event = RESET_VALUE;
//	
//fsp_err_t uartStatus  = FSP_SUCCESS;
//size_t j;
//j = 0;
//	while(j < dataSize){
//		       uartStatus = R_SCI_UART_Write(p_ctrl, (unsigned char *)&pData[j], dataSize);

//		if(mode == ONE_WIRE){
//			R_SCI_UART_Read(p_ctrl, &clearBuffer, sizeof(clearBuffer));//Because of 1 wire UART the buffer needs to be cleared after a transmit
//		}

//		j++;
//	}
//	
//		    /* Check for event transfer complete */
//    while ((UART_EVENT_TX_COMPLETE != g_uart_event))
//    {
//        /* Check if any error event occurred */
//        if (UART_ERROR_EVENTS == g_uart_event)
//        {
//            APP_ERR_PRINT ("\r\n**  FUCK  **\r\n");

//        }
//    }
//	
////	if(uartStatus != FSP_SUCCESS){
////		return BOOT_ERROR;
////	}

//	return BOOT_OK;
//	
//	
//}

///**
//*	@brief This function gets the CRC from the DA14531 and checks if it matches the given CRC
//*	@param[in] uarth - a pointer to the UART handle that is used for the UART communication with the DA14531
//*	@param[in] mode - sets the data mode for the UART communication, either ONE_WIRE or TWO_WIRE
//*	@param[in] crc - the calculated crc for the data
//*	@return returns a ok if everything went succesful, else it returns an error
//*/
//static codeless_boot_error_t check_crc(sci_uart_instance_ctrl_t * p_api_ctrl, uart_wire_t mode, const uint8_t crc){

//	sci_uart_instance_ctrl_t * p_ctrl = (sci_uart_instance_ctrl_t *) p_api_ctrl;
//	
//#define TRANSFER_LENGTH_3            (1U)

//fsp_err_t uartStatus  = FSP_SUCCESS; 

//uint8_t crc_buffer = 0;

//  uartStatus = R_SCI_UART_Read(p_ctrl, &crc_buffer, TRANSFER_LENGTH_3); //Get the crc byte

//	if(uartStatus != FSP_SUCCESS || crc_buffer != crc){
//		return BOOT_ERROR;
//	}else{
//		crc_buffer = ACK;
//		uartStatus = R_SCI_UART_Write(p_ctrl, &crc_buffer, TRANSFER_LENGTH_3);

//		if(mode == ONE_WIRE){
//			R_SCI_UART_Read(p_ctrl, &crc_buffer, sizeof(crc_buffer));//Because of 1 wire UART the buffer needs to be cleared after a transmit
//		}
//	}

//	if(uartStatus != FSP_SUCCESS){
//		return BOOT_ERROR;
//	}


//	return BOOT_OK;

//}

