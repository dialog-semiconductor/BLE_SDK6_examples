#include "fdc2x1x.h"
#include "i2c.h"
#include "gpio.h"
#include "arch_console.h"

uint16_t device_id = 0;

void init_fdc2x1x(void){	
	RESERVE_GPIO(,FDC2X1X_SDA_PORT, FDC2X1X_SDA_PIN, PID_I2C_SDA);
	RESERVE_GPIO(,FDC2X1X_SCL_PORT, FDC2X1X_SCL_PIN, PID_I2C_SCL);
	
	GPIO_ConfigurePin(FDC2X1X_SDA_PORT, FDC2X1X_SDA_PIN, INPUT_PULLUP, PID_I2C_SDA, true);
	GPIO_ConfigurePin(FDC2X1X_SCL_PORT, FDC2X1X_SCL_PIN, INPUT_PULLUP, PID_I2C_SCL, true);
	
	i2c_cfg_t i2c_init_struct = {0};
	i2c_init_struct.clock_cfg.ss_hcnt = 0x48; //Default value from the datasheet
	i2c_init_struct.clock_cfg.ss_lcnt = 0x4F; //Default value from the datasheet
	i2c_init_struct.clock_cfg.fs_hcnt = 0x08; //Default value from the datasheet
	i2c_init_struct.clock_cfg.fs_lcnt = 0x17; //Default value from the datasheet
	i2c_init_struct.restart_en        = I2C_RESTART_DISABLE;
	i2c_init_struct.speed             = I2C_SPEED_FAST;
	i2c_init_struct.mode              = I2C_MODE_MASTER;
	i2c_init_struct.addr_mode         = I2C_ADDRESSING_7B;
	i2c_init_struct.address           = FDC2X1X_I2C_ADDRESS;
	i2c_init_struct.tx_fifo_level     = 1;
	i2c_init_struct.rx_fifo_level     = 1;
	i2c_init(&i2c_init_struct);
	
	//Read the DEVICE_ID register
	uint8_t data[] = {DEVICE_ID, 0, 0};
	i2c_abort_t abrt_code;
	i2c_master_transmit_buffer_sync(data, 1, &abrt_code, I2C_F_WAIT_FOR_STOP);
	i2c_master_receive_buffer_sync(data, 2, &abrt_code, I2C_F_WAIT_FOR_STOP);
	
	device_id = (((uint16_t)data[0]) << 8) | data[1];
	
	//Settings for the click board
	data[0] = MUX_CONFIG;
	data[1] = 0x02; 
	data[2] = 0x09; //Set the glitch filter to 1Mhz(sensor oscillates @100khz)
	i2c_master_transmit_buffer_sync(data, 3, &abrt_code, I2C_F_WAIT_FOR_STOP);
	
	data[0] = RESET_DEV;
	data[1] = 0x06; //Set the gain to 16x
	data[2] = 0x00; 
	i2c_master_transmit_buffer_sync(data, 3, &abrt_code, I2C_F_WAIT_FOR_STOP);
	
	data[0] = CLOCK_DIVIDERS_CH0;
	data[1] = 1 << 4; //Sensor under 8 Mhz
	data[2] = 124; // Devide master clock by 124 (about 322khz, four times sensor resonance)
	i2c_master_transmit_buffer_sync(data, 3, &abrt_code, I2C_F_WAIT_FOR_STOP);
		
	data[0] = OFFSET_CH0;
	data[1] = 0x38; 
	data[2] = 0xB3;//Should offset result (For higher gain)
	i2c_master_transmit_buffer_sync(data, 3, &abrt_code, I2C_F_WAIT_FOR_STOP);
		
	data[0] = DRIVE_CURRENT_CH0;
	data[1] = 0xA0; //Datasheet value TODO: tweak this
	data[2] = 0;
	i2c_master_transmit_buffer_sync(data, 3, &abrt_code, I2C_F_WAIT_FOR_STOP);
	
	data[0] = SETTLECOUNT_CH0;
	data[1] = 0;
	data[2] = 0x0A;
	i2c_master_transmit_buffer_sync(data, 3, &abrt_code, I2C_F_WAIT_FOR_STOP);
	
	data[0] = RCOUNT_CH0;
	data[1] = 0x00;
	data[2] = 0x51*2;
	i2c_master_transmit_buffer_sync(data, 3, &abrt_code, I2C_F_WAIT_FOR_STOP);
}

uint32_t read_fdc2x1x(uint8_t channel){
	if(channel>3){
		ASSERT_WARNING(0); //The channel can't be higher than 3
		return 0;
	}
	uint8_t data[4];
	i2c_abort_t abrt_code;
	data[0] = DATA_CH0 + 2 * channel;
	
	enable_fdc2x1x(channel);
	
	while (!data_ready_fdc2x1x(channel)){
	}
	
	if(device_id == 0x3054){ //12 bit resolution FDC211x
	  i2c_master_transmit_buffer_sync(data, 1, &abrt_code, I2C_F_WAIT_FOR_STOP);
	  i2c_master_receive_buffer_sync(data, 2, &abrt_code, I2C_F_WAIT_FOR_STOP);
		disable_fdc2x1x();
		return (((uint16_t)data[0]) << 8) | data[1];
	} else if(device_id == 0x3055){ //28 bit resolution FDC221x
	  i2c_master_transmit_buffer_sync(data, 1, &abrt_code, I2C_F_WAIT_FOR_STOP);
	  i2c_master_receive_buffer_sync(data, 4, &abrt_code, I2C_F_WAIT_FOR_STOP);
		disable_fdc2x1x();
		return (((uint32_t)data[0]) << 24) | (((uint32_t)data[1]) << 16) | (((uint32_t)data[2]) << 8) | data[3];
	} else {
		disable_fdc2x1x();
		ASSERT_WARNING(0); //Incorrect device ID, please make sure the sensor is connected, powered and the right adress is defined (FDC2X1X_I2C_ADDRESS)
		return 0;		
	}
}

void enable_fdc2x1x(uint8_t channel){
	if(channel>3){
		ASSERT_WARNING(0); //The channel can't be higher than 3
		return;
	}
	uint8_t data[3];
	i2c_abort_t abrt_code;
	
	data[0] = CONFIG;
  i2c_master_transmit_buffer_sync(data, 1, &abrt_code, I2C_F_WAIT_FOR_STOP);
	i2c_master_receive_buffer_sync(data, 2, &abrt_code, I2C_F_WAIT_FOR_STOP);
	data[2] = data[1];
	data[1] = (data[0] & 0x1F) | 0x14 | (channel << 6);
	data[0] = CONFIG;
  i2c_master_transmit_buffer_sync(data, 3, &abrt_code, I2C_F_WAIT_FOR_STOP);	
}

uint8_t data_ready_fdc2x1x(uint8_t channel){
	if(channel>3){
		ASSERT_WARNING(0); //The channel can't be higher than 3
		return 0;
	}
	uint8_t data[3];
	i2c_abort_t abrt_code;
	
	data[0] = STATUS;
  i2c_master_transmit_buffer_sync(data, 1, &abrt_code, I2C_F_WAIT_FOR_STOP);
	i2c_master_receive_buffer_sync(data, 2, &abrt_code, I2C_F_WAIT_FOR_STOP);
	return data[1] & (0x08 >> channel);
}

void disable_fdc2x1x(void){
	uint8_t data[3];
	i2c_abort_t abrt_code;
	
	data[0] = CONFIG;
  i2c_master_transmit_buffer_sync(data, 1, &abrt_code, I2C_F_WAIT_FOR_STOP);
	i2c_master_receive_buffer_sync(data, 2, &abrt_code, I2C_F_WAIT_FOR_STOP);
	data[2] = data[1];
	data[1] = (data[0] & 0x1F) | 0x34;
	data[0] = CONFIG;
  i2c_master_transmit_buffer_sync(data, 3, &abrt_code, I2C_F_WAIT_FOR_STOP);	
}

void test_fdc2x1x(void){
	init_fdc2x1x();
	wdg_freeze();
		read_fdc2x1x(0);
		//arch_printf("Capacitance: %i\r\n", read_fdc2x1x(0));
		//arch_printf_flush();

}

