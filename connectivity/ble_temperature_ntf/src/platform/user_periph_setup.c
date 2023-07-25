/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (c) 2015-2021 Renesas Electronics Corporation and/or its affiliates
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
#include "syscntl.h"
#include "MCP9808.h"
/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
//i2c configuration

#if !defined(CFG_USE_INTERNAL_TEMP_SENSOR)
static const i2c_cfg_t i2c_cfg = {
    .clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET,
    .clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET,
    .clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET,
    .clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET,
    .restart_en = I2C_RESTART_ENABLE,
    .speed = I2C_SPEED_MODE,
    .mode = I2C_MODE_MASTER,
    .addr_mode = I2C_ADDRESS_MODE,
    .address = I2C_SLAVE_ADDRESS,
    .tx_fifo_level = 1,
    .rx_fifo_level = 1,
};
#endif

/**
 ****************************************************************************************
 * @brief Each application reserves its own GPIOs here.
 *
 * @return void
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{

#if defined (CFG_PRINTF_UART2)
    RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
#endif

#if !defined(CFG_USE_INTERNAL_TEMP_SENSOR)
	RESERVE_GPIO(SDA,MCP9808_SDA_PORT, MCP9808_SDA_PIN, PID_I2C_SDA);
	RESERVE_GPIO(SCL,MCP9808_SCL_PORT, MCP9808_SCL_PIN, PID_I2C_SCL);
#endif 
}

#endif

void set_pad_functions(void)
{
#if defined (__DA14586__)
    // Disallow spontaneous DA14586 SPI Flash wake-up
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#endif

#if !defined(CFG_USE_INTERNAL_TEMP_SENSOR)
		// Configure I2C pin functionality
    GPIO_ConfigurePin(MCP9808_SCL_PORT, MCP9808_SCL_PIN, INPUT_PULLUP, PID_I2C_SCL, false);
    GPIO_ConfigurePin(MCP9808_SDA_PORT, MCP9808_SDA_PIN, INPUT_PULLUP, PID_I2C_SDA, false);
#endif
}

void periph_init(void)
{
#if defined (__DA14531__)
	SetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE, 0);
#endif
	
#if defined (__DA14531__)
    // In Boost mode enable the DCDC converter to supply VBAT_HIGH for the used GPIOs
    syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);

    // In ES1 DA14531 the debugger is left disabled by the booter when it loads the
    // application image to RAM.
#if defined (__ES1_DA14531__)
    syscntl_load_debugger_cfg();
#endif

#else
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif

    // ROM patch
    patch_func();

#if !defined(CFG_USE_INTERNAL_TEMP_SENSOR)
		i2c_init(&i2c_cfg);
#endif
		
		//GPIO_reservations();
		
    // Set pad functionality
    set_pad_functions();

    // Enable the pads
    GPIO_set_pad_latch_en(true);
		
#if !defined(CFG_USE_INTERNAL_TEMP_SENSOR)		
		// Must be done after initializing GPIO because this function communicates with the MCP9808 via I2C
		MCP9808_init();
#endif
}
