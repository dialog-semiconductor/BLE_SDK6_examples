/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Entry function.
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
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "da14531_boot.h"

/*******************************************************************************************************************//**
 * @addtogroup boot_da14531
 * @{
 **********************************************************************************************************************/

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
	fsp_err_t err = FSP_SUCCESS;
	fsp_pack_version_t version = {RESET_VALUE};

	/* Version get API for FLEX pack information */
	R_FSP_VersionGet(&version);

	/* Initializing UART */
	err = uart_initialize();
	if (FSP_SUCCESS != err)
	{
		APP_PRINT ("\r\n ** UART INIT FAILED ** \r\n");
		deinit_uart();
	}
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
	if (BSP_WARM_START_RESET == event)
	{
#if BSP_FEATURE_FLASH_LP_VERSION != 0

		/* Enable reading from data flash. */
		R_FACI_LP->DFLCTL = 1U;

		/* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
		 * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
	}

	/*RESET_DA14531_Pin RX P2_6 (RA) <--> P0_0 (DA14531) To UART RX */
	R_BSP_PinAccessEnable(); R_BSP_PinWrite(BSP_IO_PORT_02_PIN_06 , 1U); R_BSP_PinAccessDisable();
	R_BSP_PinAccessEnable(); R_BSP_PinWrite(BSP_IO_PORT_02_PIN_06 , 0U); R_BSP_PinAccessDisable();

	/* C runtime environment and system clocks are setup. */
	if (BSP_WARM_START_POST_C == event)
	{
		/* Configure pins. */
		R_IOPORT_Open (&g_ioport_ctrl, &g_bsp_pin_cfg);
	}
}


/*******************************************************************************************************************//**
 * @} (end addtogroup boot_da14531)
 **********************************************************************************************************************/
