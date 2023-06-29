/**
 ****************************************************************************************
 *
 * @file i2c_impl.c
 *
 * @brief I2C Acccel implementation project source code.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 #include "i2c_impl.h"
 #include "i2c.h"
 #include "gpio.h"
 #include "lis2dh_driver.h"
 #include "accel_lis2dh_impl.h"
 #include "user_periph_setup.h"
 
i2c_complete_cb_t i2c_cb 					__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY 


status_t i2c_accel_read_reg(uint32_t address, uint8_t *byte)
{
    uint16_t bytes_read = 1;
    i2c_abort_t abrt_code = I2C_ABORT_NONE;
    
    GLOBAL_INT_DISABLE();
    //Write Command + SUB Address  
    i2c_write_byte(address);
    //Read Command and - I2C_STOP after one byte read - and Restart before the read
#if defined (__DA14531__)
    i2c_write_byte(I2C_CMD | I2C_STOP | I2C_RESTART );
#else 
		i2c_write_byte(I2C_CMD);
#endif
    while(!i2c_is_rx_fifo_not_empty())
    {
        abrt_code = i2c_get_abort_source();
        ASSERT_ERROR(!abrt_code);
            
    };
    *byte = i2c_read_byte();

    // Wait until no master activity
    while (i2c_is_master_busy());
    GLOBAL_INT_RESTORE();
    
    // Read tx abort source
    abrt_code = i2c_get_abort_source();
    if (abrt_code)
    {
            
                    // Clear tx abort
                    i2c_reset_int_tx_abort();

    }
    
    if(bytes_read != 1 || abrt_code != I2C_ABORT_NONE)
            return MEMS_ERROR;
    
    return MEMS_SUCCESS;
}

status_t i2c_accel_write_reg(uint32_t address, uint8_t byte)
{
    uint16_t bw;
    i2c_abort_t abrt_code = I2C_ABORT_NONE;

    uint8_t data[2];
    data[0] = address;
    data[1] = byte;

    GLOBAL_INT_DISABLE();


    bw = i2c_master_transmit_buffer_sync(&data[0], 2, &abrt_code, I2C_F_WAIT_FOR_STOP);

    GLOBAL_INT_RESTORE();

    if(bw != 2 || abrt_code != I2C_ABORT_NONE)
                    return MEMS_ERROR;

            
    return MEMS_SUCCESS;
	
}

status_t i2c_accel_read_fifo(uint8_t Reg_start, AxesRaw_t Data[32], uint16_t burst_num)
{
	
    i2c_abort_t abort_code = I2C_ABORT_NONE;
    uint8_t *data_ptr = (uint8_t *)&Data[0];
    
    GLOBAL_INT_DISABLE();
    i2c_write_byte(Reg_start | 0x80);
    while(!i2c_is_tx_fifo_empty() );
    GLOBAL_INT_RESTORE();
	
	 // Read tx abort source
    abort_code = i2c_get_abort_source();
    if (abort_code)
    {
                    // Clear tx abort
            i2c_reset_int_tx_abort();
            
            return MEMS_ERROR;
    }
                    
	#ifdef CFG_I2C_DMA_SUPPORT
		#if defined(__DA14531__)
    i2c_master_receive_buffer_dma(I2C_DMA_CHANNEL_PAIR_1, data_ptr, burst_num, i2c_cb, NULL, I2C_F_ADD_STOP);
		#else
		i2c_master_receive_buffer_dma(I2C_DMA_CHANNEL_PAIR_1, data_ptr, burst_num, i2c_cb, NULL, I2C_F_NONE);
		#endif 
	#else
		#if defined(__DA14531__)
    i2c_master_receive_buffer_async(data_ptr, burst_num, i2c_cb, NULL, I2C_F_ADD_STOP);
		#else
		i2c_master_receive_buffer_async(data_ptr, burst_num, i2c_cb, NULL, I2C_F_NONE); //DA14585 adds stop condition
		#endif 
	#endif

    return MEMS_SUCCESS;

	
		
	
}

void i2c_accel_set_cb(i2c_complete_cb_t cb)
{
    i2c_cb = cb;
}

void i2c_accel_configure_pins_for_sleep()
{
    GPIO_ConfigurePin(I2C_SCL_PORT, I2C_SCL_PIN, INPUT, PID_GPIO, true);
    GPIO_ConfigurePin(I2C_SDA_PORT, I2C_SDA_PIN, INPUT, PID_GPIO, true);	
}
 
 
 /// @} APP
