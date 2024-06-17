/**
 ****************************************************************************************
 *
 * @file user_accel_ble_impl.h
 *
 * @brief User Accel BLE project header file.
 *
 * Copyright (C) 2012-2021 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

#ifndef _USER_ACCEL_BLE_IMPL_H_
#define _USER_ACCEL_BLE_IMPL_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief 
 *
 * @{
 ****************************************************************************************
 */
#include "lis2dh_driver.h"
#include "wkupct_quadec.h"

#if defined(__ACCEL_IF_I2C__)
#include "i2c.h"
#elif defined(__ACCEL_IF_SPI__)
#include "spi.h"
#endif


#define ACCEL_FIFO_WATERMARK 	(30)
#define ACCEL_ODR							(LIS2DH_ODR_10Hz)
#define ACCEL_POWER_MODE			(LIS2DH_NORMAL)


typedef void (*accel_interrupt_handler_cb_t)(void);

typedef enum{
	CMD_TYPE_ACCEL_STATE = 0x00,
	CMD_TYPE_ACCEL_ODR,
	CMD_TYPE_ACCEL_MODE,
}write_cmd_type_t;

typedef enum{
	CMD_ACCEL_STATE_OFF = 0x00,
	CMD_ACCEL_STATE_ON,
}accel_state_t;

struct write_cmd_if_t{
	uint8_t header;
	uint8_t cmd_type;
	uint8_t cmd;
};

typedef struct{
	LIS2DH_ODR_t 		odr;
	LIS2DH_Mode_t 	mode;
	accel_state_t 	state;
	accel_interrupt_handler_cb_t cb;
	uint8_t fifo_watermark;
} accel_config_t;


void accel_lis2dh_handle_watermark_int1(uint8_t * data);

#if defined(__ACCEL_IF_I2C__)
uint8_t accel_lis2dh_start_acquisition(uint8_t acq_len, AxesRaw_t buff[32], i2c_complete_cb_t cb);
#elif defined(__ACCEL_IF_SPI__)
uint8_t accel_lis2dh_start_acquisition(uint8_t acq_len, AxesRaw_t buff[32], spi_cb_t cb);
#endif 

uint8_t accel_lis2dh_init(LIS2DH_ODR_t odr, uint8_t watermark, accel_interrupt_handler_cb_t cb);

uint8_t accel_lis2dh_on(LIS2DH_Mode_t mode);

void accel_lis2dh_prepare_int1_pin( wakeup_handler_function_t callback );
 
 /// @} APP

#endif // _USER_ACCEL_BLE_IMPL_H_
