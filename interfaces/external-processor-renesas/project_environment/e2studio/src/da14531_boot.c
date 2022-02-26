/***********************************************************************************************************************
 * File Name    : da14531_boot.c
 * Description  : Contains UART functions definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "da14531_boot.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"

#if defined(CODELESS_IMAGE)
#include "da14531_codeless_image.h" 
#elif defined(PRX_REPORTER_IMAGE)
#include "da14531_prox_reporter_image.h"
#endif

#define ACK 		(uint8_t)(0x06)
#define NACK		(uint8_t)(0x15)
#define STX			(uint8_t)(0x02)
#define SOH			(uint8_t)(0x01)
#define HEADER_SIZE		(size_t)(3)

enum uart_boot
{
	DA14531_BOOT_SEND_LENGTH ,
	DA14531_BOOT_SEND_DATA,
	DA14531_BOOT_CHECK_CRC
};

enum uart_boot uart_state;

/*******************************************************************************************************************//**
 * @addtogroup boot_da14531
 * @{
 **********************************************************************************************************************/

/*
 * Private global variables
 */
/* Flag for user callback */
volatile uint8_t g_uart_event = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief       Initialize  UART.
 * @param[in]   None
 * @retval      FSP_SUCCESS         Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t uart_initialize(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Initialize UART channel with baud rate 115200 */
	err = R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);
	if (FSP_SUCCESS != err)
	{
		APP_ERR_PRINT("\r\n**  R_SCI_UART_Open API failed  **\r\n");
	}

	return err;
}

/*******************************************************************************************************************//**
 *  @brief       Deinitialize SCI UART module
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void deinit_uart(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Close module */
	err = R_SCI_UART_Close (&g_uart0_ctrl);
	if (FSP_SUCCESS != err)
	{
		APP_ERR_PRINT("\r\n**  R_SCI_UART_Close API failed  ** \r\n");
	}
}

/*****************************************************************************************************************
 *  @brief      UART user callback
 *  @param[in]  p_args
 *  @retval     None
 ****************************************************************************************************************/

void user_uart_callback(uart_callback_args_t * p_args)
{
	fsp_err_t uartStatus = FSP_SUCCESS;
	uint8_t header[HEADER_SIZE ];

	uint8_t crc_buffer;

	if (UART_EVENT_RX_CHAR == p_args->event)
	{
		if (STX == p_args->data && 0 == DA14531_BOOT_SEND_LENGTH)
		{

			//Prepare header
			header[0] = SOH;
#if defined(CODELESS_IMAGE)
			header[1] = (uint8_t) (sizeof(CODELESS) & 0xFF); //Get lower byte of the size
			header[2] = (uint8_t) ((sizeof(CODELESS) >> 8) & 0xFF); //Get upper byte of the size
#elif defined(PRX_REPORTER_IMAGE)
			header[1] = (uint8_t) (sizeof(PROXREPORTER) & 0xFF); //Get lower byte of the size
			header[2] = (uint8_t) ((sizeof(PROXREPORTER) >> 8) & 0xFF); //Get upper byte of the size
#endif

			uartStatus = R_SCI_UART_Write (&g_uart0_ctrl, header, HEADER_SIZE);
			if (FSP_SUCCESS != uartStatus)
			{
				APP_ERR_PRINT("\r\n**  Write failed  **\r\n");
			}

			APP_PRINT("\r\n**  Done: sends the header for the data to the DA14531 and waits for the ACK  ** \r\n");
			uart_state = DA14531_BOOT_SEND_DATA;
		}

		else if (DA14531_BOOT_SEND_DATA == uart_state && ACK == p_args->data)
		{
#if defined(CODELESS_IMAGE)
			uartStatus = R_SCI_UART_Write (&g_uart0_ctrl, CODELESS, sizeof(CODELESS));
#elif defined(PRX_REPORTER_IMAGE)
			uartStatus = R_SCI_UART_Write (&g_uart0_ctrl, PROXREPORTER, sizeof(PROXREPORTER));
#endif

			APP_PRINT("\r\n**  Done: sends the data to the DA14531  ** \r\n");
			uart_state = DA14531_BOOT_CHECK_CRC;
		}
#if defined(CODELESS_IMAGE)
		else if (CODELESS_CRC == p_args->data)
#elif defined(PRX_REPORTER_IMAGE)
			else if (PROX_CRC == p_args->data)
#endif
			{
				crc_buffer = ACK;
				uartStatus = R_SCI_UART_Write (&g_uart0_ctrl, &crc_buffer, sizeof(crc_buffer));
				APP_PRINT("\r\n**  Done: gets the CRC from the DA14531 and checks if it matches the given CRC  ** \r\n");
			}
	}

	if (UART_EVENT_RX_COMPLETE == p_args->event)
	{
		g_uart_event = UART_EVENT_RX_COMPLETE;
	}

	if (UART_EVENT_TX_COMPLETE == p_args->event)
	{
		/* Toggle GPIO when the Transmission is done */
		R_BSP_PinAccessEnable ();
		R_BSP_PinWrite (BSP_IO_PORT_09_PIN_14, 1U);
		R_BSP_PinAccessDisable ();
		R_BSP_PinAccessEnable ();
		R_BSP_PinWrite (BSP_IO_PORT_09_PIN_14, 0U);
		R_BSP_PinAccessDisable ();
		g_uart_event = UART_EVENT_TX_COMPLETE;
	}
}

	/*******************************************************************************************************************//**
	 * @} (end addtogroup boot_da14531)
	 **********************************************************************************************************************/
