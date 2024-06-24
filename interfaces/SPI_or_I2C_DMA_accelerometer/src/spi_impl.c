 #include "spi_impl.h"
 #include "spi.h"
 #include "spi_531.h"

 #include "gpio.h"
 #include "lis2dh_driver.h"
 #include "accel_lis2dh_impl.h"
 #include "user_periph_setup.h"
 
 spi_cb_t callback = NULL; // Transfer callback

 status_t spi_accel_read_reg(uint8_t address, uint8_t *byte)
 {
	 
	 uint8_t write_val[2];
	 
	 //Bit 0 is RW bit, address is bit 2-7
	 write_val[0] = (address & 0x3F) | (1 << 7);
	 write_val[1] = 0x00;
	 uint8_t readbuffer[2];
	 spi_enable();
	 spi_cs_low();
	 
	 int8_t status = spi_master_transfer(write_val, readbuffer, 2, SPI_OP_BLOCKING);
	 spi_cs_high();
	 spi_disable();
	 byte[0] = readbuffer[1];
	 
	 if(status != SPI_STATUS_ERR_OK)
		return MEMS_ERROR;
	 
	 return MEMS_SUCCESS;
 }
 
 
 status_t spi_accel_write_reg(uint8_t address, uint8_t byte)
 {
	 
	 uint8_t reg_write[2];
	 
	 reg_write[0] = address & 0x3F;
	 reg_write[1] = byte;
	 
	 spi_enable();
	 spi_cs_low();
	 
	 int8_t status = spi_send(reg_write, 2, SPI_OP_BLOCKING);
	 
	 spi_cs_high();
	 spi_disable();
	 
	 return MEMS_SUCCESS;
 }
 
 
 status_t spi_accel_read_fifo(uint8_t Reg_start, AxesRaw_t Data[32], uint16_t burst_num)
 {
	 
		uint16_t *temp_data = (uint16_t *)&Data[0];
		burst_num = burst_num; //Divide by 2 because we will read 16bits at a time
	 
		//Set the register increment bit for read sequentially
		uint8_t write_val = (0x3<<6 )| (Reg_start &0x3F);
	  uint8_t discard_val;
	 
		spi_enable();
		spi_cs_low();
	 
		int8_t status = spi_master_transfer(&write_val, &discard_val, 1, SPI_OP_BLOCKING);
	 
		spi_disable();
	 
		//Accel x, y, z are 16 bit values, makes it cleaner on the transfer
		spi_set_bitmode(SPI_MODE_8BIT);
	 
		spi_enable();
	 
#if defined(__ACCEL_USE_DMA__)
		status = spi_receive(temp_data, burst_num, SPI_OP_DMA);
#else
		status = spi_receive(temp_data, burst_num, SPI_OP_BLOCKING);
		if(callback != NULL){
			callback(burst_num);
		}
#endif 
	

		return MEMS_SUCCESS;
 }
 
 void spi_accel_set_cb(spi_cb_t cb)
{
    spi_register_receive_cb(cb);
	  callback = cb;
}
 


