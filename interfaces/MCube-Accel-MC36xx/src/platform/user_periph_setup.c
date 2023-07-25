/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (C) 2018-2021 Dialog Semiconductor
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

#include "rwip_config.h"             // SW configuration
#include "user_periph_setup.h"       // peripheral configuration
#include "gpio.h"
#include "uart.h"                    // UART initialization
#include "m_drv_console.h"
#include "m_drv_interface.h"
#include "m_drv_mc36xx.h"
#include "arch_console.h"
#include "user_accelerometer.h"
#include "system_library.h"
#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
* Globally reserved GPIOs reservation
*/

/*
* Application specific GPIOs reservation. Used only in Development mode (#if DEVELOPMENT_DEBUG)

i.e.
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);    //Reserve P_01 as Generic Purpose I/O
*/

#ifdef CFG_PRINTF_UART2
    RESERVE_GPIO(UART2_TX, UART2_TX_GPIO_PORT, UART2_TX_GPIO_PIN, PID_UART2_TX);
    RESERVE_GPIO(UART2_RX, UART2_RX_GPIO_PORT, UART2_RX_GPIO_PIN, PID_UART2_RX);
#endif
}
#endif //DEVELOPMENT_DEBUG

void set_pad_functions(void)        // set gpio port function mode
{
/*
* Configure application ports.
i.e.
    GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false ); // Set P_01 as Generic purpose Output
*/

#ifdef __DA14586__
    // disallow spontaneous flash wake-up
    GPIO_ConfigurePin(SPI_EN_GPIO_PORT, SPI_EN_GPIO_PIN, OUTPUT, PID_GPIO, true);
#endif

#ifdef CFG_PRINTF_UART2
    GPIO_ConfigurePin(UART2_TX_GPIO_PORT, UART2_TX_GPIO_PIN, OUTPUT, PID_UART2_TX, false);
    GPIO_ConfigurePin(UART2_RX_GPIO_PORT, UART2_RX_GPIO_PIN, INPUT, PID_UART2_RX, false);
#endif
}

void periph_init(void)
{
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));

    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);

    //rom patch
     patch_func();

    //Init pads
    set_pad_functions();

    // (Re)Initialize peripherals
    // i.e.
    //  uart_init(UART_BAUDRATE_115K2, UART_FRAC_BAUDRATE_115K2, UART_CHARFORMAT_8);

#ifdef CFG_PRINTF_UART2
    SetBits16(CLK_PER_REG, UART2_ENABLE, 1);
    uart2_init(UART_BAUDRATE_115K2, UART_FRAC_BAUDRATE_115K2, UART_CHARFORMAT_8);
#endif

   // Enable the pads
    SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 1);

    RESERVE_GPIO(,I2C_GPIO_PORT, I2C_SDA_PIN, PID_I2C_SDA);
    RESERVE_GPIO(,I2C_GPIO_PORT, I2C_SCL_PIN, PID_I2C_SCL);
    // Init I2C
    GPIO_ConfigurePin(I2C_GPIO_PORT, I2C_SCL_PIN, INPUT, PID_I2C_SCL, false);
    GPIO_ConfigurePin(I2C_GPIO_PORT, I2C_SDA_PIN, INPUT, PID_I2C_SDA, false);

    m_drv_i2c_init();

    M_DRV_MC36XX_Init();
#ifdef FIFO_DEMO
    M_DRV_MC36XX_EnableFIFO(E_M_DRV_MC36XX_FIFO_CTL_ENABLE, E_M_DRV_MC36XX_FIFO_MODE_WATERMARK, FIFO_THRESHOLD);
#endif
    M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_CWAKE);
}
