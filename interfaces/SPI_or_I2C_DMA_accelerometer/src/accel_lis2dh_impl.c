/**
 ****************************************************************************************
 *
 * @file accel_lis2dh_impl.c
 *
 * @brief Accel lis2dh Implementation project source code.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
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
#include "wkupct_quadec.h"
#include "user_periph_setup.h"
#include "accel_lis2dh_impl.h"
#include "gpio.h"

#if defined(__ACCEL_IF_I2C__)
#include "i2c_impl.h"
#elif defined(__ACCEL_IF_SPI__)
#include "spi_impl.h"
#endif


accel_config_t accel_config 				__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY


#if defined(__ACCEL_IF_I2C__)
uint8_t accel_lis2dh_start_acquisition(uint8_t acq_len, AxesRaw_t buff[32], i2c_complete_cb_t cb)
#elif defined(__ACCEL_IF_SPI__)
uint8_t accel_lis2dh_start_acquisition(uint8_t acq_len, AxesRaw_t buff[32], spi_cb_t cb)
#endif 
{
    uint8_t response;

#if defined(__ACCEL_IF_I2C__)
    i2c_accel_set_cb(cb);
#elif defined(__ACCEL_IF_SPI__)
		spi_accel_set_cb(cb);
#endif 
    response = LIS2DH_GetAccAxesRawBurst(buff, acq_len);

    return response;
}

void accel_lis2dh_handle_watermark_int1(uint8_t * data)
{
    LIS2DH_ResetInt1Latch();
    LIS2DH_GetFifoSourceFSS(data);
	
}
void accel_lis2dh_prepare_int1_pin(wakeup_handler_function_t callback)
{
    uint32_t wakeup_pin_select = 0;
    uint32_t wakeup_pin_polarity = 0;

    wakeup_pin_select |= WKUPCT_PIN_SELECT(LIS2DH_INT1_PORT, LIS2DH_INT1_PIN);
    wakeup_pin_polarity |= WKUPCT_PIN_POLARITY(LIS2DH_INT1_PORT, LIS2DH_INT1_PIN, WKUPCT_PIN_POLARITY_HIGH);

    
    // Prepare for wakeup on Int1Pin Activity 
    wkupct_register_callback(callback);
    wkupct_enable_irq
    (
            wakeup_pin_select, // select pin 
            wakeup_pin_polarity, // polarity low
            1, // 1 event
            0 //debounce (MAX 0x3F) - time in milliseconds
    );
}

uint8_t accel_lis2dh_init(LIS2DH_ODR_t odr, uint8_t watermark, accel_interrupt_handler_cb_t cb)
{
	
    uint8_t response = MEMS_SUCCESS;

    accel_config.mode 					= LIS2DH_POWER_DOWN;
    accel_config.odr 					= odr;
    accel_config.state 					= CMD_ACCEL_STATE_OFF;
    accel_config.cb					= cb;
    accel_config.fifo_watermark                         = watermark;
    
    //Initialize in Bypass Mode in case FIFO was full from previous development - this clears the FIFO
    response = LIS2DH_FIFOModeEnable(LIS2DH_FIFO_BYPASS_MODE);
    
    return response;
}

uint8_t accel_lis2dh_on(LIS2DH_Mode_t mode)
{
    uint8_t response = MEMS_ERROR;

    if(accel_config.state == CMD_ACCEL_STATE_OFF)
    {
			      uint8_t chipID;
            LIS2DH_GetWHO_AM_I(&chipID);
            if(accel_config.mode == LIS2DH_POWER_DOWN)
                    accel_config.mode = LIS2DH_NORMAL;
            else
                    accel_config.mode = mode;
    
            response |= LIS2DH_SetMode(accel_config.mode);
            response = LIS2DH_SetODR(accel_config.odr);
            response |= LIS2DH_SetFullScale(LIS2DH_FULLSCALE_2);
            response |= LIS2DH_SetAxis(LIS2DH_X_ENABLE | LIS2DH_Y_ENABLE | LIS2DH_Z_ENABLE);	
            accel_lis2dh_prepare_int1_pin(accel_config.cb);
            // set FIFO watermark, FIFO_CTRL_REG = 0x18
            response |= LIS2DH_SetWaterMark(accel_config.fifo_watermark);
            // set Fifo watermark interrupt on INT1, CTRL_REG3 = 0x04
            response |= LIS2DH_SetInt1Pin(LIS2DH_CLICK_ON_PIN_INT1_DISABLE | LIS2DH_I1_INT1_ON_PIN_INT1_DISABLE |              
                                                                            LIS2DH_I1_INT2_ON_PIN_INT1_DISABLE | LIS2DH_I1_DRDY1_ON_INT1_DISABLE | LIS2DH_I1_DRDY2_ON_INT1_DISABLE |
                                                                            LIS2DH_WTM_ON_INT1_ENABLE | LIS2DH_INT1_OVERRUN_DISABLE   );

            // Interrupt 1 Latch enable, CTRL_REG5 = 0x08
            response |= LIS2DH_Int1LatchEnable(MEMS_ENABLE);
            response |= LIS2DH_SetTriggerInt(LIS2DH_TRIG_INT1);
            
            response |= LIS2DH_FIFOModeEnable(LIS2DH_FIFO_TRIGGER_MODE);
            
            accel_config.state = CMD_ACCEL_STATE_ON;
            
            return response;
            
    }

    return response;
}


/// @} APP