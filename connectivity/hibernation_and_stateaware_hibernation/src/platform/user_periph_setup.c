/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (C) 2012-2021 Dialog Semiconductor
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
#include "datasheet.h"
#include "user_periph_setup.h"       // peripheral configuration
#include "system_library.h"
#include "user_profiles_config.h"
#include "gpio.h"
#include "uart.h"                    // UART initialization
#include "app_proxr.h"
#include "user_hibernation.h"
#include "syscntl.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

#if defined (CFG_SPI_FLASH_ENABLE)
#include "spi.h"
#include "spi_flash.h"
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
    i.e. to reserve P0_1 as Generic Purpose I/O:
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
*/
    // Push Button
    RESERVE_GPIO(PUSH_BUTTON, GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, PID_GPIO);

#if defined (__DA14531__) && (defined (CFG_APP_GOTO_HIBERNATION) || defined (CFG_APP_GOTO_STATEFUL_HIBERNATION))
    // Wake up from hibernation pin
    RESERVE_GPIO(HIB_WAKE_UP, HIB_WAKE_UP_PORT, HIB_WAKE_UP_PIN, PID_GPIO);
#endif

#if BLE_PROX_REPORTER
    // Alert LED
    RESERVE_GPIO(GREEN_LED, GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, PID_GPIO);
#endif

#if defined (CFG_SPI_FLASH_ENABLE) && !defined (__DA14586__)
    // SPI Flash
    RESERVE_GPIO(SPI_EN, SPI_EN_PORT, SPI_EN_PIN, PID_SPI_EN);
    RESERVE_GPIO(SPI_CLK, SPI_CLK_PORT, SPI_CLK_PIN, PID_SPI_CLK);
    RESERVE_GPIO(SPI_DO, SPI_DO_PORT, SPI_DO_PIN, PID_SPI_DO);
    RESERVE_GPIO(SPI_DI, SPI_DI_PORT, SPI_DI_PIN, PID_SPI_DI);
#endif

}
#endif //DEVELOPMENT_DEBUG

void set_pad_functions(void)        // set gpio port function mode
{
/*
    i.e. to set P0_1 as Generic purpose Output:
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
*/

#if defined (__DA14586__)
    // Disallow spontaneous DA14586 SPI Flash wake-up
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#endif

    // Push Button
    GPIO_ConfigurePin(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, INPUT_PULLUP, PID_GPIO, false);

#if defined (__DA14531__) && (defined (CFG_APP_GOTO_HIBERNATION) || defined (CFG_APP_GOTO_STATEFUL_HIBERNATION))
    // Wake up from hibernation pin
    GPIO_ConfigurePin(HIB_WAKE_UP_PORT, HIB_WAKE_UP_PIN, INPUT_PULLUP, PID_GPIO, false);
#endif

#if (BLE_PROX_REPORTER)
    // Alert LED
    GPIO_ConfigurePin(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, OUTPUT, PID_GPIO, false);
#endif

#if defined (CFG_SPI_FLASH_ENABLE)
    // SPI Flash
    GPIO_ConfigurePin(SPI_EN_PORT, SPI_EN_PIN, OUTPUT, PID_SPI_EN, true);
    GPIO_ConfigurePin(SPI_CLK_PORT, SPI_CLK_PIN, OUTPUT, PID_SPI_CLK, false);
    GPIO_ConfigurePin(SPI_DO_PORT, SPI_DO_PIN, OUTPUT, PID_SPI_DO, false);
    GPIO_ConfigurePin(SPI_DI_PORT, SPI_DI_PIN, INPUT, PID_SPI_DI, false);
#endif

}


#if defined (CFG_SPI_FLASH_ENABLE)
// Configuration struct for SPI
static const spi_cfg_t spi_cfg = {
    .spi_ms = SPI_MS_MODE,
    .spi_cp = SPI_CP_MODE,
    .spi_speed = SPI_SPEED_MODE,
    .spi_wsz = SPI_WSZ,
    .spi_cs = SPI_CS,
    .cs_pad.port = SPI_EN_PORT,
    .cs_pad.pin = SPI_EN_PIN,
#if defined (__DA14531__)
    .spi_capture = SPI_EDGE_CAPTURE,
#endif
};

// Configuration struct for SPI FLASH
static const spi_flash_cfg_t spi_flash_cfg = {
    .chip_size = SPI_FLASH_DEV_SIZE,
};
#endif


void periph_init(void)  // set i2c, spi, uart, uart2 serial clks
{
#if defined (__DA14531__)
    // Disable HW Reset functionality of P0_0
    GPIO_Disable_HW_Reset();

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


#if defined (CFG_SPI_FLASH_ENABLE)
    // Configure SPI Flash environment
    spi_flash_configure_env(&spi_flash_cfg);

    // Initialize SPI
    spi_initialize(&spi_cfg);
#endif

    // Set pad functionality
    set_pad_functions();

#if (BLE_PROX_REPORTER)
    app_proxr_port_reinit(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN);
    app_button_enable();
#endif

    // Enable the pads
    GPIO_set_pad_latch_en(true);
}
