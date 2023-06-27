 /**
 ***************************************************************************************
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
#ifndef _SPI_IMPL_H_
#define _SPI_IMPL_H_

#include "spi.h"
#include "lis2dh_driver.h"

 void spi_accel_set_cb(spi_cb_t cb);
 status_t spi_accel_write_reg(uint8_t address, uint8_t byte);
 status_t spi_accel_read_reg(uint8_t address, uint8_t *byte);
 status_t spi_accel_read_fifo(uint8_t Reg_start, AxesRaw_t Data[32], uint16_t burst_num);
#endif //_SPI_IMPL_H_