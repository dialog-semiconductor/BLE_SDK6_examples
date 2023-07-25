/**
 ****************************************************************************************
 *
 * @file user_reset_mechanism.c
 *
 * @brief Reset Mechanism SW Example source code.
 *
 * Copyright (c) 2015-2021 Dialog Semiconductor
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration
#include "gap.h"
#include "app_easy_timer.h"
#include "user_reset_mechanism.h"
#include "user_custs1_impl.h"
#include "user_custs1_def.h"
#include "co_bt.h"
#include "arch_console.h"

#if defined (CFG_SPI_FLASH_ENABLE)
#include "user_periph_setup.h"
#include "spi_flash.h"
#endif

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint8_t app_connection_idx                      	__SECTION_ZERO("retention_mem_area0");
timer_hnd app_param_update_request_timer_used   	__SECTION_ZERO("retention_mem_area0");

/* 
 * Holds the latest reset status of the device
 */
uint16_t latest_reset_fault_status 					__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
/* 
 * Holds the flags that indicate that either the NMI or the Hardfault handler has occurred
 * the flags should be kept in an un-initialized area in order not to be wiped out by the
 * scatterload procedure when rebooting and downloading fresh fw
 */
uint8_t latest_fault_status[9]                      __SECTION_ZERO("retention_mem_area_uninit");
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief Check if the we can trust the un-initialized area for determine the reset reason
 * @return void
 ****************************************************************************************
*/
static uint8_t user_error_check(void)
{
    uint8_t fault_status = 0;
    // Read the magic values from the un-initialized area
    uint32_t magic_num_1 = co_read32p(&latest_fault_status[MAGIC_NUM_1_INDEX]);
    uint32_t magic_num_2 = co_read32p(&latest_fault_status[MAGIC_NUM_2_INDEX]);
    
    // Check if the magic values are valid
    if (magic_num_1 == MAGIC_NUM_1 && magic_num_2 == MAGIC_NUM_2)
    {
        fault_status = latest_fault_status[FAULT_INDEX];
    }
    else
    {
        co_write32p(&latest_fault_status[MAGIC_NUM_1_INDEX], MAGIC_NUM_1);
        co_write32p(&latest_fault_status[MAGIC_NUM_2_INDEX], MAGIC_NUM_2);
    }
   /* 
    * The latest status is kept in the fault status variable,
    * re-initialize the value changed in the hardfault or NMI
    */
    latest_fault_status[FAULT_INDEX] = INVALID_FAULT;
    
    return fault_status;
}

/**
 ****************************************************************************************
 * @brief Notifies application that system has been reset.
 * @param[in] reset_stat The value of @c RESET_STAT_REG; if reset_stat is zero,
 *                       system has woken up from hibernation.
 * @note Application shall define this function or get notified when a reset occurs.
 ****************************************************************************************
 */
void reset_indication(uint16_t reset_stat)
{	
#if defined (__DA14531__)
	latest_reset_fault_status = reset_stat ;
#else
    // The reset value of the POR_TIMER_REG is not retained only when POR occurs 
    if(reset_stat == 0x18)
    {
        SetWord16(POR_TIMER_REG, POR_TIME & POR_TIME_VAL);
        latest_reset_fault_status = PORESET_VAL;
    }
#if (USE_POWER_OPTIMIZATIONS)
    // The default value of the TRIM_CTRL_REG is 0xA2 and switched to 0x00 when POWER_OPTIMIZATIONS are used 
    // if TRIM_CTRL_REG has switched back to default a HW reset has occurred.
    // TODO: Assert warning if power optimizations are not used
    else if(GetWord8(TRIM_CTRL_REG) == XTAL16M_TRIM_DELAY_SETTING)
        latest_reset_fault_status = HWRESET_VAL;
#else
    #warning: "having the USE_POWER_OPTIMIZATIONS disabled doesn't allow tracking a HW reset on DA14585/586" 
#endif
    else
        latest_reset_fault_status = SWRESET_VAL;
#endif
    latest_reset_fault_status |= (uint16_t)user_error_check() << 8;
}

void user_set_hardfault_flag(void)
{
    latest_fault_status[FAULT_INDEX] |= HARDFAULT_OCCURED;
}

void user_set_watchdog_flag(void)
{
    latest_fault_status[FAULT_INDEX] |= NMI_OCCURED;
}

/**
 ****************************************************************************************
 * @brief Prints out the source of reset and fault that had just occurred
 * @return void
 ****************************************************************************************
*/
static void print_reset_fault_reason(void)
{
    uint8_t reset = latest_reset_fault_status & 0xFF;
    uint8_t fault = (latest_reset_fault_status >> 8);
    
	switch(reset)
	{
		case WDOGRESET_VAL:	
			arch_printf( "\n\r****** HW WDOG RESET ******\n\r" ) ;		
		break;
		
		case SWRESET_VAL:
			arch_printf( "\n\r****** SOFTWARE RESET ******\n\r" ) ;
		break;
		
		case HWRESET_VAL:
			arch_printf( "\n\r****** HARDWARE RESET ******\n\r" ) ;
		break;
		
		case PORESET_VAL:
			arch_printf( "\n\r****** POWER ON RESET ******\n\r" ) ;
		break;

		default:
			arch_printf( "\n\r****** NO RESET ******\n\r" ) ;
		break;
    }
    
    switch(fault)
    {
        case INVALID_FAULT:
            arch_printf( "****** NO FAULT DETECTED ******\n\r" );
            break;
        case (HARDFAULT_OCCURED | NMI_OCCURED):
            arch_printf("****** HARDFAULT HAS OCCURRED ******\n\r");
            break;
        case NMI_OCCURED:
            arch_printf("****** NMI WATCHDOG HAS OCCURRED ******\n\r");
            break;
        default:
            arch_printf( "****** FAULT DATA CORRUPTED ******\n\r" );
            break;
    }

    arch_printf("Reset reason: 0x%02x, Fault reason 0x%02x\n\r", reset, fault);
}

#if defined (CFG_SPI_FLASH_ENABLE)
// Configuration struct for SPI
static const spi_cfg_t spi_cfg = {
    .spi_ms = SPI_MS_MODE,
    .spi_cp = SPI_CP_MODE,
    .spi_speed = SPI_SPEED_MODE,
    .spi_wsz = SPI_WSZ,
    .spi_cs = SPI_CS,
    .cs_pad.port = SPI_EN_PORT,
    .cs_pad.pin = SPI_EN_PIN,
#if defined (__DA14531__)
    .spi_capture = SPI_EDGE_CAPTURE,
#endif
};

// Configuration struct for SPI FLASH
static const spi_flash_cfg_t spi_flash_cfg = {
    .chip_size = SPI_FLASH_DEV_SIZE,
};
#endif

void user_app_init(void)
{
/*Configure the SPI interface in order to shut down the flash after start up*/
#if defined (CFG_SPI_FLASH_ENABLE)
    GPIO_ConfigurePin(SPI_CLK_PORT, SPI_CLK_PIN, OUTPUT, PID_SPI_CLK, false);
    GPIO_ConfigurePin(SPI_DO_PORT,  SPI_DO_PIN,  OUTPUT, PID_SPI_DO,  false);
    GPIO_ConfigurePin(SPI_DI_PORT,  SPI_DI_PIN,  INPUT,  PID_SPI_DI,  false);

    // Configure SPI Flash environment
    spi_flash_configure_env(&spi_flash_cfg);
    // Initialize SPI
    spi_initialize(&spi_cfg);
    spi_flash_power_down();
    spi_disable();
#if !defined(__DA14531__)    
    // Configure UART2 TX Pad
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
#endif
#endif
    
#if defined (CFG_PRINTF)  
	print_reset_fault_reason();
#endif		
	
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    
    default_app_on_init();
}

/**
 ****************************************************************************************
 * @brief Parameter update request timer callback function.
 * @return void
 ****************************************************************************************
*/
static void param_update_request_timer_cb()
{
    app_easy_gap_param_update_start(app_connection_idx);
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
}

static void update_db_reset_status(uint8_t conidx)
{
    struct custs1_val_set_req *req = KE_MSG_ALLOC(CUSTS1_VAL_SET_REQ, prf_get_task_from_id(TASK_ID_CUSTS1), TASK_APP, custs1_val_set_req);
    req->conidx = conidx;
    req->handle = SVC1_IDX_RESET_SOURCE_VAL;
    req->length = DEF_SVC1_RESET_SOURCE_CHAR_LEN;
    memcpy(req->value, &latest_reset_fault_status, req->length);
    
    ke_msg_send(req);
}

void user_app_adv_start(void)
{
    app_easy_gap_undirected_advertise_start();
}

void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;

        // Update the database with the new reset value
        update_db_reset_status(connection_idx);
        
        // Check if the parameters of the established connection are the preferred ones.
        // If not then schedule a connection parameter update request.
        if ((param->con_interval < user_connection_param_conf.intv_min) ||
            (param->con_interval > user_connection_param_conf.intv_max) ||
            (param->con_latency != user_connection_param_conf.latency) ||
            (param->sup_to != user_connection_param_conf.time_out))
        {
            // Connection params are not these that we expect
            app_param_update_request_timer_used = app_easy_timer(APP_PARAM_UPDATE_REQUEST_TO, param_update_request_timer_cb);
        }
    }
    else
    {
        // No connection has been established, restart advertising
        user_app_adv_start();
    }

    default_app_on_connection(connection_idx, param);
}

void user_app_adv_undirect_complete(uint8_t status)
{
    // If advertising was canceled then update advertising data and start advertising again
    if (status == GAP_ERR_CANCELED)
    {
        user_app_adv_start();
    }
}

void user_app_disconnect(struct gapc_disconnect_ind const *param)
{
    // Cancel the parameter update request timer
    if (app_param_update_request_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_param_update_request_timer_used);
        app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    }

    // Restart Advertising
    user_app_adv_start();
}

void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msgid)
    {
        case CUSTS1_VAL_WRITE_IND:
        {
            struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

            switch (msg_param->handle)
            {
                case SVC1_IDX_CONTROL_POINT_VAL:
                    user_svc1_ctrl_wr_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                default:
                    break;
            }
        } break;
        
        case GAPC_PARAM_UPDATED_IND:
        {
            // Cast the "param" pointer to the appropriate message structure
            struct gapc_param_updated_ind const *msg_param = (struct gapc_param_updated_ind const *)(param);

            // Check if updated Conn Params filled to preferred ones
            if ((msg_param->con_interval >= user_connection_param_conf.intv_min) &&
                (msg_param->con_interval <= user_connection_param_conf.intv_max) &&
                (msg_param->con_latency == user_connection_param_conf.latency) &&
                (msg_param->sup_to == user_connection_param_conf.time_out))
            {
            }
        } break;

        default:
            break;
    }
}

/// @} APP
