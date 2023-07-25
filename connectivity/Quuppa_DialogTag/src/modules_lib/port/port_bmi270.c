/**
 ****************************************************************************************
 *
 * @file port_bmi270.c
 *
 * @brief Port_bmi270.c project source code.
 *
 * Copyright (C) 2015-2021 Dialog Semiconductor
# The MIT License (MIT)
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
# OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */

#include "port_bmi270.h"
#include "port_bmi270_config.h"


#if(PORT_BMI270_INTERFACE!=PORT_BMI270_INTERFACE_I2C)
    #include "spi.h"
    #define _port_bmi270_itf_start()         spi_cs_low()
    #define _port_bmi270_itf_stop()          spi_cs_high()
#else   
    #define _port_bmi270_itf_start()          __asm("NOP")
    #define _port_bmi270_itf_stop()           __asm("NOP")
#endif // PORT_BMI270_INTERFACE
    
    
#if(PORT_BMI270_INTERFACE!=PORT_BMI270_INTERFACE_I2C)
static const spi_cfg_t _port_bmi270_spi_cfg = {
    .spi_ms = SPI_MS_MODE_MASTER,
    .spi_cp = SPI_CP_MODE_0,
    .spi_speed = SPI_SPEED_MODE_4MHz,
    .spi_wsz = SPI_MODE_8BIT,
    .spi_cs = SPI_CS_0,
    .cs_pad.port = PORT_BMI270_CS_PORT,
    .cs_pad.pin = PORT_BMI270_CS_PIN,
#if defined (__DA14531__)
    .spi_capture = SPI_MASTER_EDGE_CAPTURE,
#endif
};
#else
		
/// Configuration struct for I2C
i2c_cfg_t i2c_cfg = {
    .clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET,
    .clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET,
    .clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET,
    .clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET,
    .restart_en = I2C_RESTART_ENABLE,
    .speed = I2C_SPEED_FAST,
    .mode = I2C_MODE_MASTER,
    .addr_mode = I2C_ADDRESSING_7B,
    .address = PORT_BMI270_I2C_ADDR,
    .tx_fifo_level = 16,
    .rx_fifo_level = 16,
};
#endif

    
struct bmi2_dev port_bmi270_dev                             __SECTION_ZERO("retention_mem_area0");

int8_t port_bmi270_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t port_bmi270_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len);
void port_bmi270_delay_us( uint32_t period_us);

static inline int8_t _port_bmi270_itf_read(uint8_t reg, uint8_t *data, uint16_t size)
{
#if(PORT_BMI270_INTERFACE!=PORT_BMI270_INTERFACE_I2C)
    
    /* Issue the register address*/             
    spi_access(reg);

  #if(PORT_BMI270_INTERFACE==PORT_BMI270_INTERFACE_SPI_3W)
    GPIO_ConfigurePin(PORT_BMI270_SDX_PORT,  PORT_BMI270_SDX_PIN,  INPUT_PULLDOWN, PID_SPI_DI,  true);
  #endif     
    
  #if !PORT_BMI270_USE_BOSCH_DUMMY_BYTE
    spi_access(0);  
  #endif
    
    while(size--) {                                           
        *data = (uint8_t)spi_access(0);  
        ++data;
    }
    
  #if(PORT_BMI270_INTERFACE==PORT_BMI270_INTERFACE_SPI_3W)
     GPIO_ConfigurePin(PORT_BMI270_SDX_PORT,  PORT_BMI270_SDX_PIN,  OUTPUT, PID_SPI_DO,  true);
  #endif
    
#else
		i2c_master_transmit_buffer_sync(&reg, 1, NULL, I2C_F_WAIT_FOR_STOP);
		i2c_master_receive_buffer_sync(data, size, NULL, I2C_F_WAIT_FOR_STOP);
#endif
    return 0;
}

static inline int8_t _port_bmi270_itf_write(uint8_t reg, uint8_t *data, uint16_t size)
{
#if(PORT_BMI270_INTERFACE!=PORT_BMI270_INTERFACE_I2C)
    /* Issue the register address*/       
    spi_access(reg);
    while(size--) {
        spi_access(*data);
        ++data;
    }
#else
    
    // Critical section
    GLOBAL_INT_DISABLE();
		
		i2c_master_transmit_buffer_sync(&reg, 1, NULL, I2C_F_NONE);
		i2c_master_transmit_buffer_sync(data, size, NULL, I2C_F_WAIT_FOR_STOP);
    
    // End of critical section
    GLOBAL_INT_RESTORE();
#endif
    
    return 0;
}

static inline void _port_bmi270_delay_us(volatile uint32_t nof_us)
{
    while( nof_us-- ){
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
    }
}

static inline void _port_bmi270_comm_init()
{
#if(PORT_BMI270_INTERFACE!=PORT_BMI270_INTERFACE_I2C)
    
    // BMI270 SPI pin configuration
    GPIO_ConfigurePin(PORT_BMI270_CS_PORT,  PORT_BMI270_CS_PIN,  OUTPUT, PID_SPI_EN, true);
    GPIO_ConfigurePin(PORT_BMI270_SCLK_PORT, PORT_BMI270_SCLK_PIN, OUTPUT, PID_SPI_CLK, false);
    GPIO_ConfigurePin(PORT_BMI270_SDX_PORT, PORT_BMI270_SDX_PIN, OUTPUT, PID_SPI_DO, false);
  #if(PORT_BMI270_INTERFACE==PORT_BMI270_INTERFACE_SPI)
    GPIO_ConfigurePin(PORT_BMI270_SDO_PORT, PORT_BMI270_SDI_PIN, INPUT, PID_SPI_DI, false);
  #endif
    
    spi_initialize(&_port_bmi270_spi_cfg);
#else
    GPIO_ConfigurePin(PORT_BMI270_I2C_SDA_PORT,  PORT_BMI270_I2C_SDA_PIN,  INPUT, PID_I2C_SDA, true);
    GPIO_ConfigurePin(PORT_BMI270_I2C_SCL_PORT, PORT_BMI270_I2C_SCL_PIN, OUTPUT, PID_I2C_SCL, true);
	
		i2c_init(&i2c_cfg);
#endif
}

static void _port_bmi270_comm_release()
{
#if(PORT_BMI270_INTERFACE!=PORT_BMI270_INTERFACE_I2C)
    
    // BMI270 SPI pin de-configuration
  #ifndef PORT_BMI270_SPI_PINS_MULTIPLEXED
    GPIO_ConfigurePin(PORT_BMI270_CS_PORT,  PORT_BMI270_CS_PIN,  OUTPUT, PID_GPIO, true);
    GPIO_ConfigurePin(PORT_BMI270_SCLK_PORT, PORT_BMI270_SCLK_PIN, OUTPUT, PID_GPIO, false);
    GPIO_ConfigurePin(PORT_BMI270_SDX_PORT, PORT_BMI270_SDX_PIN, OUTPUT, PID_GPIO, false);
  #else
    GPIO_ConfigurePin(PORT_BMI270_CS_PORT,  PORT_BMI270_CS_PIN,  OUTPUT, PID_GPIO, true);
    GPIO_ConfigurePin(PORT_BMI270_SCLK_PORT, PORT_BMI270_SCLK_PIN, INPUT, PID_GPIO, false);
    GPIO_ConfigurePin(PORT_BMI270_SDX_PORT, PORT_BMI270_SDX_PIN, INPUT, PID_GPIO, false);
  #endif
  #if(PORT_BMI270_INTERFACE==PORT_BMI270_INTERFACE_SPI)
    GPIO_ConfigurePin(PORT_BMI270_SDI_PORT, PORT_BMI270_SDI_PIN, INPUT_PULLDOWN, PID_GPIO, false);
  #endif
    
    spi_release();
#else
    GPIO_ConfigurePin(PORT_BMI270_I2C_SDA_PORT,  PORT_BMI270_I2C_SDA_PIN,  INPUT, PID_I2C_SDA, true);
    GPIO_ConfigurePin(PORT_BMI270_I2C_SCL_PORT, PORT_BMI270_I2C_SCL_PIN, INPUT, PID_I2C_SCL, true);
	
		i2c_release();
#endif
}


/*********************** User function definitions ****************************/
/*!
 * @brief This API reads the data from the given register address
 * of sensor.
 */
int8_t port_bmi270_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    _port_bmi270_itf_start();
    _port_bmi270_itf_read(reg_addr, data, len);
    _port_bmi270_itf_stop();

    return 0;
}

/*!
 * @brief This API writes the given data to the register address
 * of sensor.
 */
int8_t port_bmi270_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    _port_bmi270_itf_start();
    _port_bmi270_itf_write(reg_addr, data, len);
    _port_bmi270_itf_stop();

    return 0;
}

void port_bmi270_delay_us(uint32_t period_us)
{
    _port_bmi270_delay_us(period_us);
}

int8_t port_bmi270_init(void)
{
    port_bmi270_dev.read = (bmi2_read_fptr_t)port_bmi270_read;
    port_bmi270_dev.write = (bmi2_write_fptr_t)port_bmi270_write;
    port_bmi270_dev.delay_us = (bmi2_delay_fptr_t)port_bmi270_delay_us;
#if(PORT_BMI270_INTERFACE==PORT_BMI270_INTERFACE_I2C)
    port_bmi270_dev.intf = BMI2_I2C_INTERFACE;
    port_bmi270_dev.dev_id = BMI2_I2C_PRIM_ADDR;
    port_bmi270_dev.read_write_len = 256;
#else
  #if(PORT_BMI270_INTERFACE==PORT_BMI270_INTERFACE_I2C)
    port_bmi270_dev.intf = BMI2_SPI_INTERFACE;
  #else
    port_bmi270_dev.intf = BMI2_SPI_3W_INTERFACE;
  #endif
    port_bmi270_dev.dev_id = 0;
    port_bmi270_dev.dummy_byte = PORT_BMI270_USE_BOSCH_DUMMY_BYTE;
//    port_bmi270_dev.read_write_len = 4096; // FPS
    port_bmi270_dev.read_write_len = 256;	
#endif

//    port_bmi270_dev.config_file_ptr = NULL;
    port_bmi270_dev.config_file_addr = 0;

    /* Initialize BMI2  */
    return bmi270_init(&port_bmi270_dev);
}

void port_bmi270_itf_begin()
{
    _port_bmi270_comm_init();
}

void port_bmi270_itf_end()
{
    _port_bmi270_comm_release();
}

struct bmi2_dev* port_bmi270_dev_ptr(void)
{
    return &port_bmi270_dev;
}
