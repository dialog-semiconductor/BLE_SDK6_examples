/**
 ****************************************************************************************
 *
 * @file i2cm.c
 *
 * @brief Wrapper to provide simplified API to SDK I2C master driver.
 *
 * Copyright (c) 2023 Dialog Semiconductor. All rights reserved.
 * 
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 * 
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
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
#include <stdint.h>
#include <stdbool.h>

#include "i2c.h"

/*
 * DEFINES
 ****************************************************************************************
 */

 /*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
*/
static void set_dev_addr(uint8_t addr);

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief   Sets device I2C address.
 *
 * @param   addr - I2C address
 *
 * @return  None.
 ****************************************************************************************
*/
static void set_dev_addr(uint8_t addr)
{
  /* The device address can only be set when the controller is disabled */
  i2c_set_controller_status(I2C_CONTROLLER_DISABLE);
  /* There is a two ic_clk delay when enabling or disabling the controller */
  while(i2c_get_controller_status() != I2C_CONTROLLER_DISABLE);

  i2c_set_target_address(addr);

  i2c_set_controller_status(I2C_CONTROLLER_ENABLE);
  /* There is a two ic_clk delay when enabling or disabling the controller */
  while ((i2c_get_controller_status() != I2C_CONTROLLER_ENABLE));
}

/**
 ****************************************************************************************
 * @brief   Perform an I2C read.
 *
 * @param   dev_addr - Device I2C address
 *
 * @return  0 on success, otherwise fail.
 ****************************************************************************************
*/
int i2cm_read(uint8_t dev_addr, uint8_t * const data, uint8_t len, bool add_stop)
{
  int ret = 0;
  uint16_t bytes_read;
  i2c_abort_t abrt_code;

  uint32_t flags = I2C_F_NONE;
	
#if defined (__DA14531__)
  if (add_stop) {
    flags = I2C_F_ADD_STOP;
  }
#endif
	
  /* Each I2C device on the bus has a unique address, set this before we communicate */
  set_dev_addr(dev_addr);

  /* Performing blocking read */
  bytes_read = i2c_master_receive_buffer_sync(data, len, &abrt_code, flags);

  if ((abrt_code != I2C_ABORT_NONE) || (bytes_read != len)) {
    ret = -1;
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Perform an I2C write.
 *
 * @param   dev_addr - Device I2C address.
 *
 * @return  0 on success, otherwise fail.
 ****************************************************************************************
*/
int i2cm_write(uint8_t dev_addr, uint8_t const * const buffer, uint8_t len, bool add_stop)
{
  int ret = 0;
  uint16_t bytes_written;
  i2c_abort_t abrt_code;

  uint32_t flags = I2C_F_NONE;
#if defined (__DA14531__)
  if (add_stop) {
    flags = I2C_F_WAIT_FOR_STOP | I2C_F_ADD_STOP;
  }
#endif
  /* Each I2C device on the bus has a unique address, set this before we communicate */
  set_dev_addr(dev_addr);

  /* Perform blocking write */
  bytes_written = i2c_master_transmit_buffer_sync(buffer, len, &abrt_code, flags);

  if ((I2C_ABORT_NONE != abrt_code) || (bytes_written != len)) {
    ret = -1;
  }
  return ret;
}

/// @} APP