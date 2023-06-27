#ifndef _SPI_IMPL_H_
#define _SPI_IMPL_H_

#include "spi.h"
#include "lis2dh_driver.h"

 void spi_accel_set_cb(spi_cb_t cb);
 status_t spi_accel_write_reg(uint8_t address, uint8_t byte);
 status_t spi_accel_read_reg(uint8_t address, uint8_t *byte);
 status_t spi_accel_read_fifo(uint8_t Reg_start, AxesRaw_t Data[32], uint16_t burst_num);
#endif //_SPI_IMPL_H_

