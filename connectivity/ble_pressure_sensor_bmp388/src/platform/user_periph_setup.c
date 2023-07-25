/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (c) 2015-2021 Dialog Semiconductor
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
# OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_periph_setup.h"
#include "datasheet.h"
#include "system_library.h"
#include "rwip_config.h"
#include "gpio.h"
#include "uart.h"
#include "spi.h"
#include "i2c.h"
#include "syscntl.h"
#include "bmp388.h"
#include "user_app.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
#if (BMP388_INTERFACE == BMP388_SPI)
  RESERVE_GPIO(BMP388_SPI_MISO, BMP388_SPI_MISO_PORT, BMP388_SPI_MISO_PIN, PID_SPI_DI);
  RESERVE_GPIO(BMP388_SPI_MOSI, BMP388_SPI_MOSI_PORT, BMP388_SPI_MOSI_PIN, PID_SPI_DO);
  RESERVE_GPIO(BMP388_SPI_CLK, BMP388_SPI_CLK_PORT, BMP388_SPI_CLK_PIN, PID_SPI_CLK);
  RESERVE_GPIO(BMP388_SPI_CS, BMP388_SPI_CS_PORT, BMP388_SPI_CS_PIN, PID_SPI_EN);
#else
  RESERVE_GPIO(BMP388_I2C_SCL, BMP388_I2C_SCL_PORT, BMP388_I2C_SCL_PIN, PID_I2C_SCL);
  RESERVE_GPIO(BMP388_I2C_SDA, BMP388_I2C_SDA_PORT, BMP388_I2C_SDA_PIN, PID_I2C_SDA);
#endif

  RESERVE_GPIO(BMP388_INT, BMP388_INT_PORT, BMP388_INT_PIN, PID_GPIO);

#if defined (CFG_PRINTF_UART2)
  RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
#endif
}

#endif

void set_pad_functions(void)
{
#if (BMP388_INTERFACE == BMP388_SPI)
	
#if defined (__DA14531__)
	// Disable HW RST on P0_0 so it can be used as SPI MOSI.
  GPIO_Disable_HW_Reset();
#endif
	
  // Configure pins used for BMP388 SPI interface
  GPIO_ConfigurePin(BMP388_SPI_MISO_PORT, BMP388_SPI_MISO_PIN, INPUT, PID_SPI_DI, false);  
  GPIO_ConfigurePin(BMP388_SPI_MOSI_PORT, BMP388_SPI_MOSI_PIN, OUTPUT, PID_SPI_DO, false);  
  GPIO_ConfigurePin(BMP388_SPI_CLK_PORT, BMP388_SPI_CLK_PIN, OUTPUT, PID_SPI_CLK, false); 
  GPIO_ConfigurePin(BMP388_SPI_CS_PORT, BMP388_SPI_CS_PIN, OUTPUT, PID_SPI_EN, false); 
#else
  GPIO_ConfigurePin(BMP388_I2C_SCL_PORT, BMP388_I2C_SCL_PIN, INPUT, PID_I2C_SCL, false);  
  GPIO_ConfigurePin(BMP388_I2C_SDA_PORT, BMP388_I2C_SDA_PIN, INPUT, PID_I2C_SDA, false);
#endif

   GPIO_ConfigurePin(BMP388_INT_PORT, BMP388_INT_PIN, INPUT, PID_GPIO, false); 

#if defined (CFG_PRINTF_UART2)
  // Configure UART2 TX Pad
  GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
#endif

}

#if defined (CFG_PRINTF_UART2)
// Configuration struct for UART2
static const uart_cfg_t uart_cfg = {
  .baud_rate = UART_BAUDRATE_115200,
  .data_bits = UART_DATABITS_8,
  .parity = UART_PARITY_NONE,
  .stop_bits = UART_STOPBITS_1,
  .auto_flow_control = UART_AFCE_DIS,
  .use_fifo = UART_FIFO_EN,
  .tx_fifo_tr_lvl = UART_TX_FIFO_LEVEL_0,
  .rx_fifo_tr_lvl = UART_RX_FIFO_LEVEL_0,
  .intr_priority = 2,
};
#endif

#if (BMP388_INTERFACE == BMP388_SPI)
/* Default SPI configuration */
static const spi_cfg_t spi_cfg = {
  .spi_ms = SPI_MS_MODE_MASTER,
  .spi_cp = SPI_CP_MODE_0,
  .spi_speed = SPI_SPEED_MODE_2MHz,
  .spi_wsz = SPI_MODE_8BIT,
  .spi_cs = SPI_CS_0,
  .spi_irq = SPI_IRQ_DISABLED,
  .cs_pad.port = BMP388_SPI_CS_PORT,
  .cs_pad.pin = BMP388_SPI_CS_PIN,
};
#else
/* Default I2C interface configuration */
static const i2c_cfg_t i2c_cfg = {
  .clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET,
  .clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET,
  .clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET,
  .clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET,
  .restart_en = I2C_RESTART_ENABLE,
  .speed = I2C_SPEED_STANDARD,
  .mode = I2C_MODE_MASTER,
  .addr_mode = I2C_ADDRESSING_7B,
  /* Device address specified when read/write takes place allowing 
     multiple devices to be present on the same I2C bus */
  .address = 0,
  .tx_fifo_level = 1,
  .rx_fifo_level = 1,
};
#endif

void periph_init(void)
{
#if defined (__DA14531__)
  // In Boost mode enable the DCDC converter to supply VBAT_HIGH for the used GPIOs
  syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);
#else
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif
	
  // ROM patch
  patch_func();

  // Initialize peripherals
#if defined (CFG_PRINTF_UART2)
  // Initialize UART2
  uart_initialize(UART2, &uart_cfg);
#endif

  // Set pad functionality
  set_pad_functions();
	
  // Enable the pads
  GPIO_set_pad_latch_en(true);

#if (BMP388_INTERFACE == BMP388_SPI)
  // Initialise SPI
  spi_initialize(&spi_cfg);
#else
  // Initialise I2C
  i2c_init(&i2c_cfg);
#endif

}
