/**
 ****************************************************************************************
 *
 * @file bmp388.c
 *
 * @brief Driver implementation for BMP388 pressure/temperature sensor.
 *
 * Copyright (c) 2012-2021 Renesas Electronics Corporation and/or its affiliates
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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "spi.h"
#include "i2cm.h"
#include "bmp388.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/* BMP388 Register Map */
#define BMP388_REG_ADDR_CHIP_ID                     (0x00)

#define BMP388_REG_ADDR_ERROR                       (0x02)
#define BMP388_REG_ERROR_FATAL_Pos                  (0UL)
#define BMP388_REG_ERROR_FATAL_Msk                  (0x01)
#define BMP388_REG_ERROR_CMD_Pos                    (1UL)
#define BMP388_REG_ERROR_CMD_Msk                    (0x02)
#define BMP388_REG_ERROR_CONF_Pos                   (2UL)
#define BMP388_REG_ERROR_CONF_Msk                   (0x04)

#define BMP388_REG_ADDR_STATUS                      (0x03)
#define BMP388_REG_STATUS_CMD_RDY_Pos               (4UL)
#define BMP388_REG_STATUS_CMD_RDY_Msk               (0x10)
#define BMP388_REG_STATUS_DRDY_PRESS_Pos            (5UL)
#define BMP388_REG_STATUS_DRDY_PRESS_Msk            (0x20)
#define BMP388_REG_STATUS_DRDY_TEMP_Pos             (6UL)
#define BMP388_REG_STATUS_DRDY_TEMP_Msk             (0x40)

#define BMP388_REG_ADDR_DATA_0                      (0x04)
#define BMP388_REG_ADDR_DATA_1                      (0x05)
#define BMP388_REG_ADDR_DATA_2                      (0x06)
#define BMP388_REG_ADDR_DATA_3                      (0x07)
#define BMP388_REG_ADDR_DATA_4                      (0x08)
#define BMP388_REG_ADDR_DATA_5                      (0x09)

#define BMP388_REG_ADDR_SENSOR_TIME_0               (0x0C)
#define BMP388_REG_ADDR_SENSOR_TIME_1               (0x0D)
#define BMP388_REG_ADDR_SENSOR_TIME_2               (0x0E)
#define BMP388_REG_ADDR_SENSOR_TIME_3               (0x0F)

#define BMP388_REG_ADDR_EVENT                       (0x10)
#define BMP388_REG_EVENT_POR_DETECTED_Pos           (0UL)
#define BMP388_REG_EVENT_POR_DETECTED_Msk           (0x01)

#define BMP388_REG_ADDR_INT_STATUS                  (0x11)
#define BMP388_REG_INT_STATUS_FWM_INT_Pos           (0UL)
#define BMP388_REG_INT_STATUS_FWM_INT_Msk           (0x01)
#define BMP388_REG_INT_STATUS_FFULL_INT_Pos         (1UL)
#define BMP388_REG_INT_STATUS_FFULL_INT_Msk         (0x02)
#define BMP388_REG_INT_STATUS_DRDY_Pos              (3UL)
#define BMP388_REG_INT_STATUS_DRDY_Msk              (0x08)

#define BMP388_FIFO_LENGTH_0                        (0x12)
#define BMP388_FIFO_LENGTH_1                        (0x13)
#define BMP388_FIFO_DATA                            (0x14)
#define BMP388_FIFO_WTM_0                           (0x15)
#define BMP388_FIFO_WTM_1                           (0x16)
#define BMP388_FIFO_CONFIG_1                        (0x17)
#define BMP388_FIFO_CONFIG_2                        (0x18)

#define BMP388_REG_ADDR_INT_CTRL                    (0x19)
#define BMP388_REG_INT_CTRL_INT_OD_Pos              (0UL)
#define BMP388_REG_INT_CTRL_INT_OD_Msk              (0x01)
#define BMP388_REG_INT_CTRL_INT_LEVEL_Pos           (1UL)
#define BMP388_REG_INT_CTRL_INT_LEVEL_Msk           (0x02)
#define BMP388_REG_INT_CTRL_INT_LATCH_Pos           (2UL)
#define BMP388_REG_INT_CTRL_INT_LATCH_Msk           (0x04)
#define BMP388_REG_INT_CTRL_FWTM_EN_Pos             (3UL)
#define BMP388_REG_INT_CTRL_FWTM_EN_Msk             (0x08)
#define BMP388_REG_INT_CTRL_FFULL_EN_Pos            (4UL)
#define BMP388_REG_INT_CTRL_FFULL_EN_Msk            (0x10)
#define BMP388_REG_INT_CTRL_DRDY_EN_Pos             (6UL)
#define BMP388_REG_INT_CTRL_DRDY_EN_Msk             (0x40)

#define BMP388_REG_ADDR_IF_CTRL                     (0x1A)

#define BMP388_REG_ADDR_PWR_CTRL                    (0x1B)
#define BMP388_REG_PWR_CTRL_PRESS_EN_Pos            (0UL)
#define BMP388_REG_PWR_CTRL_PRESS_EN_Msk            (0x01)
#define BMP388_REG_PWR_CTRL_TEMP_EN_Pos             (1UL)
#define BMP388_REG_PWR_CTRL_TEMP_EN_Msk             (0x02)
#define BMP388_REG_PWR_CTRL_MODE_Pos                (4UL)
#define BMP388_REG_PWR_CTRL_MODE_Msk                (0x30)

#define BMP388_REG_ADDR_OSR                         (0x1C)
#define BMP388_REG_OSR_OSR_P_Pos                    (0UL)
#define BMP388_REG_OSR_OSR_P_Msk                    (0x07)
#define BMP388_REG_OSR_OSR_T_Pos                    (3UL)
#define BMP388_REG_OSR_OSR_T_Msk                    (0x38)

#define BMP388_REG_ADDR_ODR                         (0x1D)
#define BMP388_REG_ODR_ODR_SEL_Pos                  (0UL)
#define BMP388_REG_ODR_ODR_SEL_Msk                  (0x1F)

#define BMP388_REG_ADDR_CONFIG                      (0x1F)

#define BMP388_REG_ADDR_NVM_PAR_T1                  (0x31)
#define BMP388_REG_ADDR_NVM_PAR_T2                  (0x33)
#define BMP388_REG_ADDR_NVM_PAR_T3                  (0x35)
#define BMP388_REG_ADDR_NVM_PAR_P1                  (0x36)
#define BMP388_REG_ADDR_NVM_PAR_P2                  (0x38)
#define BMP388_REG_ADDR_NVM_PAR_P3                  (0x3A)
#define BMP388_REG_ADDR_NVM_PAR_P4                  (0x3B)
#define BMP388_REG_ADDR_NVM_PAR_P5                  (0x3C)
#define BMP388_REG_ADDR_NVM_PAR_P6                  (0x3E)
#define BMP388_REG_ADDR_NVM_PAR_P7                  (0x40)
#define BMP388_REG_ADDR_NVM_PAR_P8                  (0x41)
#define BMP388_REG_ADDR_NVM_PAR_P9                  (0x42)
#define BMP388_REG_ADDR_NVM_PAR_P10                 (0x44)
#define BMP388_REG_ADDR_NVM_PAR_P11                 (0x45)

#define BMP388_REG_ADDR_CMD                         (0x7E)

/* Commands that can be written to the CMD register */
#define BMP388_CMD_NOP                              (0x00)
#define BMP388_CMD_FIFO_FLUSH                       (0xB0)
#define BMP388_CMD_SOFT_RESET                       (0xB6)

#define BMP388_CHIP_ID                              0x50

/* BMP388 timings */
#define BMP388_TSTARTUP_MAX_uS                      2000

/* Register access types */
#define BMP388_REG_READ_ONLY                        0
#define BMP388_REG_WRITE_ONLY                       1
#define BMP388_REG_READ_WRITE                       2

#define BMP388_I2C_SLAVE_ADDR                       (0x76)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
struct bmp388_reg {
  uint8_t addr;
  uint8_t rw;
};

struct bmp388_calib_data {
  float par_t1_f;
  float par_t2_f;
  float par_t3_f;
  float par_p1_f;
  float par_p2_f;
  float par_p3_f;
  float par_p4_f;
  float par_p5_f;
  float par_p6_f;
  float par_p7_f;
  float par_p8_f;
  float par_p9_f;
  float par_p10_f;
  float par_p11_f;
  float t_lin;
};

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
static struct bmp388_calib_data calib_data __SECTION_ZERO("retention_mem_area0");

/* BMP388 Register Map */
static const struct bmp388_reg bmp388_reg_map[] = {
  { .addr = BMP388_REG_ADDR_CHIP_ID,       .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_ERROR,         .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_STATUS,        .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_DATA_0,        .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_DATA_1,        .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_DATA_2,        .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_DATA_3,        .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_DATA_4,        .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_DATA_5,        .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_SENSOR_TIME_0, .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_SENSOR_TIME_1, .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_SENSOR_TIME_2, .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_SENSOR_TIME_3, .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_EVENT,         .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_REG_ADDR_INT_STATUS,    .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_FIFO_LENGTH_0,          .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_FIFO_LENGTH_1,          .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_FIFO_DATA,              .rw = BMP388_REG_READ_ONLY,  },
  { .addr = BMP388_FIFO_WTM_0,             .rw = BMP388_REG_READ_WRITE, },
  { .addr = BMP388_FIFO_WTM_1,             .rw = BMP388_REG_READ_WRITE, },
  { .addr = BMP388_FIFO_CONFIG_1,          .rw = BMP388_REG_READ_WRITE, },
  { .addr = BMP388_FIFO_CONFIG_2,          .rw = BMP388_REG_READ_WRITE, },
  { .addr = BMP388_REG_ADDR_INT_CTRL,      .rw = BMP388_REG_READ_WRITE, },
  { .addr = BMP388_REG_ADDR_IF_CTRL,       .rw = BMP388_REG_READ_WRITE, },
  { .addr = BMP388_REG_ADDR_PWR_CTRL,      .rw = BMP388_REG_READ_WRITE, },
  { .addr = BMP388_REG_ADDR_OSR,           .rw = BMP388_REG_READ_WRITE, },
  { .addr = BMP388_REG_ADDR_ODR,           .rw = BMP388_REG_READ_WRITE, },
  { .addr = BMP388_REG_ADDR_CONFIG,        .rw = BMP388_REG_READ_WRITE, },
  { .addr = BMP388_REG_ADDR_CMD,           .rw = BMP388_REG_READ_WRITE, },
};

static int write(uint8_t addr, uint8_t const * const data, uint8_t len);
static int read(uint8_t addr, uint8_t * const data, uint8_t len);
static int write_reg(uint8_t addr, uint8_t data);
static int read_reg(uint8_t addr, uint8_t * const data);
static int set_reg_field(uint8_t addr, uint8_t pos, uint8_t msk, uint8_t val);
static int get_reg_field(uint8_t addr, uint8_t pos, uint8_t msk, uint8_t * const val);
static int read_calib_data(struct bmp388_calib_data *calib_data);
static void us_delay(uint32_t delay_us);
static float compenstate_temperature(uint32_t uncomp_temp, struct bmp388_calib_data *calib_data);
static float compenstate_pressure(uint32_t uncomp_press, struct bmp388_calib_data *calib_data);

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
/**
 ****************************************************************************************
 * @brief   Initialize the BMP388 driver and the device.
 *
 * @param   None.
 *
 * @return  0 on success, and non-zero error code on failure.
 ****************************************************************************************
*/
int bmp388_init(void)
{
  int ret;
  uint8_t id;

  /* First get device into a known state */
  bmp388_sw_reset();

  /* Check if communication with the device is possible */
  if ((ret = bmp388_get_chip_id(&id)) == 0) {
    if (id == BMP388_CHIP_ID) {
      ret = read_calib_data(&calib_data);
    }
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Configure the BMP388 sensor
 *
 * @param   cfg - Pointer to the configuration to be applied.
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int bmp388_config(struct bmp388_cfg * const cfg)
{
  int ret;

  /* Set the temperature oversampling */
  if ((ret = bmp388_set_temp_osr(cfg->osr_t)) == 0) {
    /* Set the pressure oversampling */
    if ((ret = bmp388_set_press_osr(cfg->osr_p)) == 0) {
      /* Set the output data rate */
      ret = bmp388_set_odr(cfg->odr);
    }
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Start a measurement.
 *
 * @param   mode - Power control mode.
 *
 * @return  0 on success, and non-zero error code on failure.
 ****************************************************************************************
*/
int bmp388_start_meas(uint8_t mode, uint8_t press_en, uint8_t temp_en)
{
  uint8_t tmp = 0;

  tmp = (mode << BMP388_REG_PWR_CTRL_MODE_Pos) & BMP388_REG_PWR_CTRL_MODE_Msk;  
  tmp = tmp | ((press_en == BMP388_PRESS_MEAS_ENABLE) ? BMP388_REG_PWR_CTRL_PRESS_EN_Msk : 0);
  tmp = tmp | ((temp_en == BMP388_TEMP_MEAS_ENABLE) ? BMP388_REG_PWR_CTRL_TEMP_EN_Msk : 0);

  return write_reg(BMP388_REG_ADDR_PWR_CTRL, tmp);
}

/**
 ****************************************************************************************
 * @brief   Dump the contents of the BMP388 registers
 *
 * @param   dbg_printf - Pointer to printf function used to output debug data.
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int bmp388_dump_regs(bmp388_dbg_printf dbg_printf)
{
  int ret = -1;
  uint8_t i;

  if (dbg_printf != NULL) {
    for (i = 0; i < (sizeof(bmp388_reg_map) / sizeof(struct bmp388_reg)); i++) {
      struct bmp388_reg *reg = (struct bmp388_reg *)&bmp388_reg_map[i];
      /* No point wasting time reading write-only registers.. */
      if ((reg->rw == BMP388_REG_READ_ONLY) || (reg->rw == BMP388_REG_READ_WRITE)) {
        uint8_t tmp;
        dbg_printf("\n\rAddr: 0x%02X - ", reg->addr);
        if ((ret = read_reg(reg->addr, &tmp)) == 0) {
          dbg_printf("0x%02X", tmp);
        } else {
          dbg_printf("Failed: %d", ret);
          break;
        }
      }
    }
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Get interrupt status.
 *
 * @param   status
 *
 * @return  0 on success, and non-zero error code on failure.
 ****************************************************************************************
*/
int bmp388_get_int_status(uint8_t * const status)
{
  return read_reg(BMP388_REG_ADDR_INT_STATUS, status);  
}

/**
 ****************************************************************************************
 * @brief   Get pressure data ready status.
 *
 * @param   None.
 *
 * @return  True if pressure ready, otherwise false.
 ****************************************************************************************
*/
bool bmp388_drdy_press(void)
{
  bool ret = false;
  uint8_t tmp;

  if (get_reg_field(BMP388_REG_ADDR_STATUS,
                    BMP388_REG_STATUS_DRDY_PRESS_Pos,
                    BMP388_REG_STATUS_DRDY_PRESS_Msk,
                    &tmp) == 0) {
    if (tmp > 0) {
      ret = true;
    }
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Get temperature data ready status.
 *
 * @return  True if temperature data ready, otherwise false.
 ****************************************************************************************
*/
bool bmp388_drdy_temp(void)
{
  bool ret = false;
  uint8_t tmp;

  if (get_reg_field(BMP388_REG_ADDR_STATUS,
                    BMP388_REG_STATUS_DRDY_TEMP_Pos,
                    BMP388_REG_STATUS_DRDY_TEMP_Msk,
                    &tmp) == 0) {
    if (tmp > 0) {
      ret = true;
    }
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Software reset puts the device into default state.
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int bmp388_sw_reset(void)
{
  int ret;
  uint8_t tmp;

  ret = read_reg(BMP388_REG_ADDR_STATUS, &tmp);

  if ((ret == 0) && (tmp & BMP388_REG_STATUS_CMD_RDY_Msk)) {
    if ((ret = write_reg(BMP388_REG_ADDR_CMD, BMP388_CMD_SOFT_RESET)) == 0) {
      /* Wait for device to complete reset cycle */
      us_delay(BMP388_TSTARTUP_MAX_uS);
      /* Check for errors */
      ret = read_reg(BMP388_REG_ADDR_ERROR, &tmp);
      if ((ret != 0) || (tmp & BMP388_REG_ERROR_CMD_Msk)) {
        ret = -1;
      }
    }
  } else {
    ret = -1;
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Gets the device operating mode
 *
 * @param   Pointer to variable used to return the mode
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int bmp388_get_mode(uint8_t * const mode)
{
  return get_reg_field(BMP388_REG_ADDR_PWR_CTRL,
                       BMP388_REG_PWR_CTRL_MODE_Pos,
                       BMP388_REG_PWR_CTRL_MODE_Msk,
                       mode);
}

/**
 ****************************************************************************************
 * @brief   Sets the operating mode
 *
 * @param   The required mode
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int bmp388_set_mode(uint8_t mode)
{
  return set_reg_field(BMP388_REG_ADDR_PWR_CTRL,
                       BMP388_REG_PWR_CTRL_MODE_Pos,
                       BMP388_REG_PWR_CTRL_MODE_Msk,
                       mode);
}

/**
 ****************************************************************************************
 * @brief   Enable data ready interrupt.
 *
 * @param   None.
 *
 * @return  0 on success, and non-zero error code on failure.
 ****************************************************************************************
*/
int bmp388_enable_drdy_int(void)
{
  return set_reg_field(BMP388_REG_ADDR_INT_CTRL,
                       BMP388_REG_INT_CTRL_DRDY_EN_Pos,
                       BMP388_REG_INT_CTRL_DRDY_EN_Msk,
                       1);
}

/**
 ****************************************************************************************
 * @brief   Disable data ready interrupt.
 *
 * @param   None.
 *
 * @return  0 on success, and non-zero error code on failure.
 ****************************************************************************************
*/
int bmp388_disable_drdy_int(void)
{
  return set_reg_field(BMP388_REG_ADDR_INT_CTRL,
                       BMP388_REG_INT_CTRL_DRDY_EN_Pos,
                       BMP388_REG_INT_CTRL_DRDY_EN_Msk,
                       0);
}

/**
 ****************************************************************************************
 * @brief   Set interrupt output level.
 *
 * @param   level - active high or low.
 *
 * @return  0 on success, and non-zero error code on failure.
 ****************************************************************************************
*/
int bmp388_set_int_active_level(uint8_t level)
{
  return set_reg_field(BMP388_REG_ADDR_INT_CTRL,
                       BMP388_REG_INT_CTRL_INT_LEVEL_Pos,
                       BMP388_REG_INT_CTRL_INT_LEVEL_Msk,
                       level);
}

/**
 ****************************************************************************************
 * @brief   Enable temperature measurement.
 *
 * @param   None.
 *
 * @return  0 on success, and non-zero error code on failure.
 ****************************************************************************************
*/
int bmp388_enable_temp_meas(void)
{
  return set_reg_field(BMP388_REG_ADDR_PWR_CTRL,
                       BMP388_REG_PWR_CTRL_TEMP_EN_Pos,
                       BMP388_REG_PWR_CTRL_TEMP_EN_Msk,
                       1);
}

/**
 ****************************************************************************************
 * @brief   Enable pressure measurement.
 *
 * @param   None.
 *
 * @return  0 on success, and non-zero error code on failure.
 ****************************************************************************************
*/
int bmp388_enable_press_meas(void)
{
  return set_reg_field(BMP388_REG_ADDR_PWR_CTRL,
                       BMP388_REG_PWR_CTRL_PRESS_EN_Pos,
                       BMP388_REG_PWR_CTRL_PRESS_EN_Msk,
                       1);
}

/**
 ****************************************************************************************
 * @brief   Sets the temperature over sampling
 *
 * @param   The required over sampling
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int bmp388_set_temp_osr(uint8_t osr)
{
  if (osr > BMP388_OVERSAMPLING_x32) {
    osr = BMP388_OVERSAMPLING_x32;
  }
  return set_reg_field(BMP388_REG_ADDR_OSR,
                       BMP388_REG_OSR_OSR_T_Pos,
                       BMP388_REG_OSR_OSR_T_Msk,
                       osr);
}

/**
 ****************************************************************************************
 * @brief   Sets the pressure over sampling
 *
 * @param   The required over sampling
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int bmp388_set_press_osr(uint8_t osr)
{
int ret;

  if (osr <= BMP388_OVERSAMPLING_x32) {
    ret = set_reg_field(BMP388_REG_ADDR_OSR, BMP388_REG_OSR_OSR_P_Pos,
                        BMP388_REG_OSR_OSR_P_Msk, osr);
  } else {
    ret = -1;
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Set output data reate.
 *
 * @param   odr - output data rate.
 *
 * @return  0 on success, and non-zero error code on failure.
 ****************************************************************************************
*/
int bmp388_set_odr(uint8_t odr)
{
  return set_reg_field(BMP388_REG_ADDR_ODR,
                       BMP388_REG_ODR_ODR_SEL_Pos,
                       BMP388_REG_ODR_ODR_SEL_Msk,
                       odr);
}

/**
 ****************************************************************************************
 * @brief   Gets the chip id
 *
 * @param   Pointer to variable used to return the chip id
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int bmp388_get_chip_id(uint8_t * const chip_id)
{
  int ret;
  uint8_t tmp;

  ret = read_reg(BMP388_REG_ADDR_CHIP_ID, &tmp);

  if (!ret) {
    *chip_id = tmp;
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Get calibrate temperature value.
 *
 * @param   temp - temperature.
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int bmp388_get_temperature(int16_t * const temp)
{
  int ret;
  uint8_t raw_data[3];

  if ((ret = read(BMP388_REG_ADDR_DATA_3, raw_data, 3)) == 0) {
      uint32_t uncomp_temp = (uint32_t)((((uint32_t)(raw_data[2])) << 16) |
                                        (((uint32_t)(raw_data[1])) << 8)  |
                                         ((uint32_t)(raw_data[0])));

    float temperature = compenstate_temperature(uncomp_temp, &calib_data);
    *temp = (int16_t)(temperature * 100);
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Get calibrated pressure value.
 *
 * @param   press - pressure value.
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int bmp388_get_pressure(uint32_t * const press)
{
  int ret;
  uint8_t raw_data[3];

  if ((ret = read(BMP388_REG_ADDR_DATA_0, raw_data, 3)) == 0) {
      uint32_t uncomp_press = (uint32_t)((((uint32_t)(raw_data[2])) << 16) |
                                         (((uint32_t)(raw_data[1])) << 8)  |
                                          ((uint32_t)(raw_data[0])));

    float pressure = compenstate_pressure(uncomp_press, &calib_data);
    *press = (uint32_t)pressure;
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Read calibration data from BMP388
 *
 * @param   calib_data - storage for calibration data.
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int read_calib_data(struct bmp388_calib_data *calib_data)
{
  int ret = 0;

  uint16_t par_t1, par_t2;
  int8_t par_t3;

  /* Read temperature calibration coefficients from NVM memory on the BMP388 */
  (void)read(BMP388_REG_ADDR_NVM_PAR_T1, (uint8_t *)&par_t1, sizeof(uint16_t));
  (void)read(BMP388_REG_ADDR_NVM_PAR_T2, (uint8_t *)&par_t2, sizeof(uint16_t));
  (void)read(BMP388_REG_ADDR_NVM_PAR_T3, (uint8_t *)&par_t3, sizeof(int8_t));

  int16_t par_p1, par_p2, par_p9;
  uint16_t par_p5, par_p6;
  int8_t par_p3, par_p4, par_p7, par_p8, par_p10, par_p11;

  /* Read pressure calibration coefficients from NVM memory on the BMP388 */
  (void)read(BMP388_REG_ADDR_NVM_PAR_P1, (uint8_t *)&par_p1, sizeof(int16_t));
  (void)read(BMP388_REG_ADDR_NVM_PAR_P2, (uint8_t *)&par_p2, sizeof(int16_t));
  (void)read(BMP388_REG_ADDR_NVM_PAR_P3, (uint8_t *)&par_p3, sizeof(int8_t));
  (void)read(BMP388_REG_ADDR_NVM_PAR_P4, (uint8_t *)&par_p4, sizeof(int8_t));
  (void)read(BMP388_REG_ADDR_NVM_PAR_P5, (uint8_t *)&par_p5, sizeof(uint16_t));
  (void)read(BMP388_REG_ADDR_NVM_PAR_P6, (uint8_t *)&par_p6, sizeof(uint16_t));
  (void)read(BMP388_REG_ADDR_NVM_PAR_P7, (uint8_t *)&par_p7, sizeof(int8_t));
  (void)read(BMP388_REG_ADDR_NVM_PAR_P8, (uint8_t *)&par_p8, sizeof(int8_t));
  (void)read(BMP388_REG_ADDR_NVM_PAR_P9, (uint8_t *)&par_p9, sizeof(int16_t));
  (void)read(BMP388_REG_ADDR_NVM_PAR_P10, (uint8_t *)&par_p10, sizeof(int8_t));
  (void)read(BMP388_REG_ADDR_NVM_PAR_P11, (uint8_t *)&par_p11, sizeof(int8_t));

  /* Compensation formula is implemented in floating point so convert coefficients to
   this format */
  calib_data->par_t1_f = ((float)par_t1) / pow(2, -8);
  calib_data->par_t2_f = ((float)par_t2) / pow(2, 30);
  calib_data->par_t3_f = ((float)par_t3) / pow(2, 48);
  calib_data->par_p1_f = (((float)par_p1) - pow(2, 14)) / pow(2, 20);
  calib_data->par_p2_f = (((float)par_p2) - pow(2, 14)) / pow(2, 29);
  calib_data->par_p3_f = ((float)par_p3) / pow(2, 32);
  calib_data->par_p4_f = ((float)par_p4) / pow(2, 37);
  calib_data->par_p5_f = ((float)par_p5) / pow(2, -3);
  calib_data->par_p6_f = ((float)par_p6) / pow(2, 6);
  calib_data->par_p7_f = ((float)par_p7) / pow(2, 8);
  calib_data->par_p8_f = ((float)par_p8) / pow(2, 15);
  calib_data->par_p9_f = ((float)par_p9) / pow(2, 48);
  calib_data->par_p10_f = ((float)par_p10) / pow(2, 48);
  calib_data->par_p11_f = ((float)par_p11) / pow(2, 65);

  return ret;
}

/**
 ****************************************************************************************
 * @brief   Calculated compentated temperature value.
 *
 * @param   uncomp_temp - raw temperature value
 *
 * @return  Calibrated temperature value
 ****************************************************************************************
*/
static float compenstate_temperature(uint32_t uncomp_temp, struct bmp388_calib_data *calib_data)
{
  float partial_data1;
  float partial_data2;

  partial_data1 = ((float)uncomp_temp) - calib_data->par_t1_f;
  partial_data2 = partial_data1 * calib_data->par_t2_f;

  calib_data->t_lin = partial_data2 + (partial_data1 * partial_data1) * calib_data->par_t3_f;

  return calib_data->t_lin;
}

/**
 ****************************************************************************************
 * @brief   Calculated compentated pressure value.
 *
 * @param   uncomp_temp - raw pressure value
 *
 * @return  Calibrated pressure value
 ****************************************************************************************
*/
static float compenstate_pressure(uint32_t uncomp_press, struct bmp388_calib_data *calib_data)
{
  volatile float partial_data1;
  volatile float partial_data2;
  volatile float partial_data3;
  volatile float partial_data4;
  volatile float partial_out1;
  volatile float partial_out2;

  partial_data1 = calib_data->par_p6_f * calib_data->t_lin;
  partial_data2 = calib_data->par_p7_f * (calib_data->t_lin * calib_data->t_lin);
  partial_data3 = calib_data->par_p8_f * (calib_data->t_lin * calib_data->t_lin * calib_data->t_lin);
  partial_out1 = calib_data->par_p5_f + partial_data1 + partial_data2 + partial_data3;

  partial_data1 = calib_data->par_p2_f * calib_data->t_lin;
  partial_data2 = calib_data->par_p3_f * (calib_data->t_lin * calib_data->t_lin);
  partial_data3 = calib_data->par_p4_f * (calib_data->t_lin * calib_data->t_lin * calib_data->t_lin);
  partial_out2 = (float)uncomp_press * (calib_data->par_p1_f + partial_data1 + partial_data2 + partial_data3);

  partial_data1 = (float)uncomp_press * (float)uncomp_press;
  partial_data2 = calib_data->par_p9_f + calib_data->par_p10_f * calib_data->t_lin;
  partial_data3 = partial_data1 * partial_data2;
  partial_data4 = partial_data3 + ((float)uncomp_press * (float)uncomp_press * (float)uncomp_press) * calib_data->par_p11_f;

  return partial_out1 + partial_out2 + partial_data4;
}

/**
 ****************************************************************************************
 * @brief   Microsecond delay, interrupts not disabled so could be longer than requested.
 *
 * @param   Number of microseconds to delay
 *
 * @return  None
 ****************************************************************************************
 */
static void us_delay(uint32_t delay_us)
{
  while( delay_us-- ){
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
  }
}

/**
 ****************************************************************************************
 * @brief   Perform read-modify-write on a bit field within a register.
 *
 * @param   addr - register address
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int set_reg_field(uint8_t addr, uint8_t pos, uint8_t msk, uint8_t val)
{
  int ret = 0;
  uint8_t tmp;

  ret = read_reg(addr, &tmp);

  if (!ret) {
    tmp = (tmp & ~msk);
    tmp = tmp | ((val << pos) & msk);
    ret = write_reg(addr, tmp);
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Get a register field.
 *
 * @param   addr - register address
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int get_reg_field(uint8_t addr, uint8_t pos, uint8_t msk, uint8_t * const val)
{
  int ret;
  uint8_t tmp;

  if ((ret = read_reg(addr, &tmp)) == 0) {
    *val = (tmp & msk) >> pos;
  }
  return ret;
}

/*
 ****************************************************************************************
 * @brief   Read data from a BMP388 register.
 *
 * @param   Memory address
 * @param   Buffer in which data is placed
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int read_reg(uint8_t addr, uint8_t * const data)
{
  return read(addr, data, sizeof(uint8_t));
}

/**
 ****************************************************************************************
 * @brief   Write data to a BMP388 register.
 *
 * @param   Memory address
 * @param   Data to be written
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int write_reg(uint8_t addr, uint8_t data)
{
  return write(addr, &data, sizeof(uint8_t));
}

#if (BMP388_INTERFACE == BMP388_SPI)
/*
 ****************************************************************************************
 * @brief   Read data from the BMP388 using the SPI interface.
 *
 * @param   Memory address
 * @param   Buffer in which data is placed
 * @param   Number of bytes of data to read
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int read(uint8_t addr, uint8_t * const data, uint8_t len)
{
  int ret = 0;

  /* Read operation is signalled by setting bit 7 high */
  addr = addr | 0x80;

  /* Enable sensor */
  spi_cs_low();

  /* Set register address to read from */
  if ((ret = spi_send(&addr, 1, SPI_OP_BLOCKING)) == 0) {
    /* Read dummy byte output by sensor */
    if ((ret = spi_receive(data, 1, SPI_OP_BLOCKING)) == 0) {
      /* Read data (address is auto-incremented by the sensor) */
      ret = spi_receive(data, len, SPI_OP_BLOCKING);
    }
  }

  /* Return sensor to sleep mode */
  spi_cs_high();

  return ret;
}

/**
 ****************************************************************************************
 * @brief   Write data to the BMP388 using the SPI interface.
 *
 * @param   Memory address
 * @param   Variable length payload
 * @param   Number of bytes within payload to write
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int write(uint8_t addr, uint8_t const * const data, uint8_t len)
{
  int ret = 0;
  uint8_t offset = 0;
  uint8_t tmp[2];

  /* Enable sensor */
  spi_cs_low();

  while ((len-- > 0) && (ret == 0)) {
    /* Send address first, write operation is signalled by setting bit 7 low */
    tmp[0] = addr & 0x7F;
    tmp[1] = data[offset++];

    if ((ret = spi_send(&tmp, 2, SPI_OP_BLOCKING)) == 0) {
      /* Sensor does not increment the address when writing */
      addr++;
    }
  }

  /* Return sensor to sleep mode */
  spi_cs_high();

  return ret;
}
#else
/*
 ****************************************************************************************
 * @brief   Read data from the BMP388 using the I2C interface.
 *
 * @param   Memory address
 * @param   Buffer in which data is placed
 * @param   Number of bytes of data to read
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int read(uint8_t addr, uint8_t * const data, uint8_t len)
{
  int ret;

  /* Set register address to read from */
  if ((ret = i2cm_write(BMP388_I2C_SLAVE_ADDR, &addr, sizeof(addr), false)) == 0) {
    /* Read data at the address */
    ret = i2cm_read(BMP388_I2C_SLAVE_ADDR, data, len, true);
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Write data to the BMP388 using the I2C interface.
 *
 * @param   Memory address
 * @param   Variable length payload
 * @param   Number of bytes within payload to write
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int write(uint8_t addr, uint8_t const * const data, uint8_t len)
{
  int ret = -1;
  uint8_t payload[16];

  if (len <= (sizeof(payload) - 1)) {
    /* Start address to write must be first byte of payload */
    memset(payload, 0, sizeof(payload));
    memcpy(&payload[1], data, len);
    payload[0] = addr;

    /* Write data (if any) to address */
    ret = i2cm_write(BMP388_I2C_SLAVE_ADDR, payload, len + 1, true);
  }
  return ret;
}
#endif

/// @} APP
