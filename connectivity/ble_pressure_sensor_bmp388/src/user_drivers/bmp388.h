/**
 ****************************************************************************************
 *
 * @file bmp388.h
 *
 * @brief Driver interface for BMP388 pressure/temperature sensor.
 *
 * Copyright (c) 2012-2021 Dialog Semiconductor
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

#ifndef _BMP388_H_
#define _BMP388_H_

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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>
#include <stdbool.h>

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
struct bmp388_cfg {
  uint8_t odr;
  uint8_t osr_t;
  uint8_t osr_p;
};

typedef int (*bmp388_dbg_printf)(const char *fmt, ...);

/*
 * DEFINES
 ****************************************************************************************
 */
/* Select which interface (SPI or I2C) to use when communicating with the BMP388 */
#define BMP388_SPI                (1)
//#define BMP388_I2C                (0)
#define BMP388_INTERFACE          BMP388_SPI

/* BMP388 operating modes */
#define BMP388_MODE_SLEEP         (0)
#define BMP388_MODE_FORCED        (1)
#define BMP388_MODE_NORMAL        (3)

/* BMP388 oversampling settings */
#define BMP388_OVERSAMPLING_x1    (0x0)
#define BMP388_OVERSAMPLING_x2    (0x1)
#define BMP388_OVERSAMPLING_x4    (0x2)
#define BMP388_OVERSAMPLING_x8    (0x3)
#define BMP388_OVERSAMPLING_x16   (0x4)
#define BMP388_OVERSAMPLING_x32   (0x5)

/* BMP388 output data rate settings */
#define BMP388_ODR_200            (0x00) /* 5ms */
#define BMP388_ODR_100            (0x01) /* 10ms */
#define BMP388_ODR_50             (0x02) /* 20ms */
#define BMP388_ODR_25             (0x03) /* 40ms */
#define BMP388_ODR_12p5           (0x04) /* 80ms */
#define BMP388_ODR_6p25           (0x05) /* 160ms */
#define BMP388_ODR_3p1            (0x06) /* 320ms */
#define BMP388_ODR_1p5            (0x07) /* 640ms */
#define BMP388_ODR_0p78           (0x08) /* 1.280s */
#define BMP388_ODR_0p39           (0x09) /* 2.560s */
#define BMP388_ODR_0p2            (0x0A) /* 5.120s */
#define BMP388_ODR_0p1            (0x0B) /* 10.24s */
#define BMP388_ODR_0p05           (0x0C) /* 20.48s */
#define BMP388_ODR_0p02           (0x0D) /* 40.96s */
#define BMP388_ODR_0p01           (0x0E) /* 81.92s */
#define BMP388_ODR_0p006          (0x0F) /* 163.84s */
#define BMP388_ODR_0p003          (0x10) /* 327.68s */
#define BMP388_ODR_0p0015         (0x11) /* 655.36s */

/* BMP388 interrupt level */
#define BMP388_INT_LVL_ACT_LO     (0)
#define BMP388_INT_LVL_ACT_HI     (1)

/* BMP388 interrupt status */
#define BMP388_INT_STATUS_FWM     (0x01)
#define BMP388_INT_STATUS_FFULL   (0x02)
#define BMP388_INT_STATUS_DRDY    (0x08)

/* BMP388 measurement control */
#define BMP388_TEMP_MEAS_DISABLE  (0)
#define BMP388_TEMP_MEAS_ENABLE   (1)
#define BMP388_PRESS_MEAS_DISABLE (0)
#define BMP388_PRESS_MEAS_ENABLE  (1)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
int bmp388_init(void);
int bmp388_config(struct bmp388_cfg * const cfg);
int bmp388_dump_regs(bmp388_dbg_printf dbg_printf);
int bmp388_start_meas(uint8_t mode, uint8_t press_en, uint8_t temp_en);
int bmp388_sw_reset(void);
int bmp388_set_mode(uint8_t mode);
int bmp388_enable_temp_meas(void);
int bmp388_enable_press_meas(void);
bool bmp388_drdy_temp(void);
bool bmp388_drdy_press(void);
int bmp388_get_temperature(int16_t * const temp);
int bmp388_get_pressure(uint32_t * const press);
int bmp388_get_chip_id(uint8_t * const chip_id);
int bmp388_set_press_osr(uint8_t osr);
int bmp388_set_temp_osr(uint8_t osr);
int bmp388_set_odr(uint8_t odr);
int bmp388_get_int_status(uint8_t * const status);
int bmp388_set_int_active_level(uint8_t level);
int bmp388_enable_drdy_int(void);

/// @} APP

#endif // _BMP388_H_
