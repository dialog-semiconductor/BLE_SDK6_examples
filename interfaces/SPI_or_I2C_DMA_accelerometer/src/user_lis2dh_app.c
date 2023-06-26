/**
 ****************************************************************************************
 *
 * @file user_empty_peripheral_template.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "user_lis2dh_app.h"
#include "accel_lis2dh_impl.h"
#include "lis2dh_driver.h"
#include "arch_console.h"
#include "i2c_impl.h"


typedef enum{
	ACC_INIT,
	ACC_FIFO_WM_REACHED,
	ACC_DATA_FETCH, 
	ACC_DATA_READY, 
	ACC_LP_MODE,
}app_accel_state_t;

typedef struct{
	
	
	uint8_t 							samples_in_fifo;
	AxesRaw_t 						buff_data[33];
	app_accel_state_t 		state;

}app_accel_env_t;


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

static void accel_int_cb(void);

app_accel_env_t app_accel_env                    __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

__STATIC_INLINE void app_goto_state(app_accel_state_t new_state)
{
	app_accel_env.state = new_state;
}

#if defined(__ACCEL_IF_I2C__)
static void i2c_async_complete(void *cb_data, uint16_t len, bool success)
{
    ASSERT_ERROR(success);

    accel_lis2dh_prepare_int1_pin(accel_int_cb);
    app_goto_state(ACC_DATA_READY);			
}
#elif defined(__ACCEL_IF_SPI__)
static void spi_receive_complete(uint16_t len)
{
	spi_cs_high();
	spi_disable();
	accel_lis2dh_prepare_int1_pin(accel_int_cb);
	app_goto_state(ACC_DATA_READY);		
}

#endif


static void accel_int_cb(void)
{
	app_goto_state(ACC_FIFO_WM_REACHED);
			
}

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}

void user_app_on_set_dev_config_complete(void)
{
	
    uint8_t accel_rsp;
    default_app_on_set_dev_config_complete();
#ifdef CFG_PRINTF
    arch_printf("%s\r\n", __func__);
#endif

    accel_rsp = accel_lis2dh_init(ACCEL_ODR, ACCEL_FIFO_WATERMARK, accel_int_cb);
    ASSERT_ERROR(accel_rsp == MEMS_SUCCESS);

    app_goto_state(ACC_INIT);	
}

void user_going_to_sleep(sleep_mode_t sleep_mode)
{
    if(sleep_mode == mode_ext_sleep){

#if defined(__ACCEL_IF_I2C__)
            i2c_accel_configure_pins_for_sleep();
#elif defined(__ACCEL_IF_SPI__)
#endif 
    }	
}

static void handle_accel_data(void)
{
    uint8_t i;
    for(i = 0; i < app_accel_env.samples_in_fifo; i++){ // convert the samples to right justified
      app_accel_env.buff_data[i].AXIS_X = app_accel_env.buff_data[i].AXIS_X >> 6;
			app_accel_env.buff_data[i].AXIS_Y = app_accel_env.buff_data[i].AXIS_Y >> 6;
			app_accel_env.buff_data[i].AXIS_Z = app_accel_env.buff_data[i].AXIS_Z >> 6;
    }
#ifdef CFG_PRINTF


    arch_printf("Samples Read from FIFO: %d \r\n", app_accel_env.samples_in_fifo);

    for(i = 0; i < app_accel_env.samples_in_fifo; i++){
			arch_printf(" x:%d, y:%d, z:%d\r\n", app_accel_env.buff_data[i].AXIS_X, app_accel_env.buff_data[i].AXIS_Y, app_accel_env.buff_data[i].AXIS_Z);
    }
#endif 
}

arch_main_loop_callback_ret_t user_main_hook(void){
	
	
	arch_main_loop_callback_ret_t sleep_ret = KEEP_POWERED;
	switch(app_accel_env.state)
	{
		case ACC_INIT:
		{
				uint8_t accel_rsp;
				accel_rsp = accel_lis2dh_on(ACCEL_POWER_MODE);

				ASSERT_ERROR(accel_rsp == MEMS_SUCCESS);			
			 //fall through to low power mode/state
				app_goto_state(ACC_LP_MODE);
		}
		case ACC_LP_MODE:
		{
			sleep_ret = GOTO_SLEEP;
			
		}break;	
		
		case ACC_FIFO_WM_REACHED:
		{
			//Starting the transfer, need chip awake until transfer is finished
			//Clear INT on accel, and get data size in FIFO
			accel_lis2dh_handle_watermark_int1(&app_accel_env.samples_in_fifo);
            
				
			if(app_accel_env.samples_in_fifo){
		    app_goto_state(ACC_DATA_FETCH);
				
#if defined(__ACCEL_IF_I2C__)
				accel_lis2dh_start_acquisition(app_accel_env.samples_in_fifo, app_accel_env.buff_data, i2c_async_complete);
#elif defined(__ACCEL_IF_SPI__)
				accel_lis2dh_start_acquisition(app_accel_env.samples_in_fifo, app_accel_env.buff_data, spi_receive_complete);
#endif 
			}
    		
		}break;
		
		case ACC_DATA_FETCH:
		{
			//Keep the device powered while the DMA or the i2C block is done collecting data
			sleep_ret = KEEP_POWERED;			
				
		}break;
		case ACC_DATA_READY:
		{
			//Callback from i2c/dma controller print data and wait for next int1 interrupt
			handle_accel_data();
			app_goto_state(ACC_LP_MODE);
			sleep_ret = GOTO_SLEEP;
				
		}break;
		
		default:
			break;
				
	}
	  
	if(sleep_ret == KEEP_POWERED){
		
		//If we don't go right to sleep, wdog could trip since the wdog is only reloaded once going to sleep.
#ifdef CFG_WDOG
		wdg_reload(WATCHDOG_DEFAULT_PERIOD);         
#endif 
		
		return KEEP_POWERED;
	}
	else{

			return sleep_ret;
	}	

}
 

/// @} APP