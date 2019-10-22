/**
 ****************************************************************************************
 *
 * @file peripherals.c
 *
 * @brief Peripherals initialization functions
 *
 * Copyright (c) 2012-2018 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#include <string.h>
#include "gpio.h"
#include "uart.h"
#include "user_periph_setup.h"
#include "i2c_eeprom.h"
#include "syscntl.h"
#include "otp_hdr.h"
#include "hw_otpc.h"

#if defined (__DA14531__)
bool gpio_uses_p00;
bool spi_uses_p00;
bool i2c_uses_p00;
bool uart_uses_p00;
bool uart_uses_p05;
bool uart_uses_p01;
bool uart_uses_p10;
#endif

_uart_sel_pins uart_sel_pins;
_spi_sel_pins spi_sel_pins;
_i2c_sel_pins i2c_sel_pins;

void init_uart_pads(void);
void init_spi_pads(void);
void init_i2c_pads(void);

static void set_all_gpios_to_input_pullup(void)
{
    const uint8_t port_num[]    = { 0, 1,  2, 4 };
    const uint8_t num_of_pins[] = { 8, 6, 10, 8 };

    int data_reg;
    int mode_reg;

    for(int i = 0; i < 4; i++) {
        data_reg = GPIO_BASE + (port_num[i] << 5) + 6;
        for(int pin = 0; pin < num_of_pins[i]; pin++) {
            mode_reg = data_reg + (pin << 1);
            SetWord16(mode_reg, INPUT_PULLUP | PID_GPIO);
        }
    }
}

 /**
 ****************************************************************************************
 * @brief Enable pad and peripheral clocks assuming that peripheral power domain
 *        is down. The UART and SPI clocks are set.
 ****************************************************************************************
 */
void periph_init(void)  // set I2C, SPI, UART, UART2 serial clks
{
#if defined (__DA14531__)
#if defined (CFG_HW_RESET_P00)
    GPIO_Enable_HW_Reset();
#else
    GPIO_Disable_HW_Reset();
#endif

    // In Boost mode enable the DCDC converter to supply VBAT_HIGH for the used GPIOs
    syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);
#endif

#if !defined (__DA14531__)
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);     // exit peripheral power down
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
#endif
    
    GPIO_set_pad_latch_en(true);                  // open pads

    // Init pads
    set_all_gpios_to_input_pullup();
#ifdef USE_UART
    init_uart_pads();
#endif
    init_spi_pads();
    init_i2c_pads();
}

#if defined (__DA14531__)
void enable_hw_reset(void)
{
#if defined (CFG_HW_RESET_P00)
    // In case the P0_0 is assigned to SPI or I2C bus,
    // then it can be set to reset for the time the application does not need to use it.
    // Equivalent set_pad function will stop the reset functionality before the GPIo is used.
    // In case the pin is used as a GPIO, then it can not change state (e.g. power up flash) and the hw_reset functionality must not be restored.
    if ((spi_uses_p00 || i2c_uses_p00) && !(gpio_uses_p00 || uart_uses_p00))
    {
        GPIO_SetPinFunction(GPIO_PORT_0, GPIO_PIN_0, INPUT_PULLDOWN, PID_GPIO);
        // Wait 10usec for the line to be discharged
        for (int i = 0; i < 4 * 10; i++);
        GPIO_Enable_HW_Reset();
    }
#endif
}

void check_gpio_hw_reset(GPIO_PORT port, GPIO_PIN pin)
{
#if defined (CFG_HW_RESET_P00)
    gpio_uses_p00 = ((port == GPIO_PORT_0) && (pin == GPIO_PIN_0));
    if (gpio_uses_p00)
    {
        GPIO_Disable_HW_Reset();
    }
#endif
}
#endif

#ifdef USE_UART

/**
 ****************************************************************************************
 * @brief Set GPIO port function mode for the UART interface.
 ****************************************************************************************
 */
void init_uart_pads(void)
{
    memset(&uart_sel_pins, 0, sizeof(uart_sel_pins));
    uart_sel_pins.uart_tx_port = UART_GPIO_PORT;
    uart_sel_pins.uart_tx_pin = UART_TX_PIN;
    uart_sel_pins.uart_rx_port = UART_GPIO_PORT;
    uart_sel_pins.uart_rx_pin = UART_RX_PIN;

#if defined (__DA14531__)
    uart_uses_p00 = (UART_TX_PIN == GPIO_PIN_0) || (UART_RX_PIN == GPIO_PIN_0);
    uart_uses_p05 = (UART_TX_PIN == GPIO_PIN_5) || (UART_RX_PIN == GPIO_PIN_5);
    uart_uses_p01 = (UART_TX_PIN == GPIO_PIN_1)  || (UART_RX_PIN == GPIO_PIN_1);
    uart_uses_p10 = (UART_TX_PIN == GPIO_PIN_10) || (UART_RX_PIN == GPIO_PIN_10);
#endif
}

void set_pad_uart(void)
{
#if defined (__DA14531__)
    if (uart_uses_p00)
    {
        GPIO_Disable_HW_Reset();
    }
    else
    {
        enable_hw_reset();
    }

    if (uart_uses_p05) {
        if (GetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE) == 1) {
            while (GetBits16(SYS_STAT_REG, DBG_IS_UP) != 0);
            SetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE, 0);
        }
    }

    if (uart_uses_p01) {
        if (GetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE) == 2) {
            while (GetBits16(SYS_STAT_REG, DBG_IS_UP) != 0);
            SetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE, 0);
        }
    }

    if (uart_uses_p10) {
        if (GetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE) == 3) {
            while (GetBits16(SYS_STAT_REG, DBG_IS_UP) != 0);
            SetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE, 0);
        }
    }
#endif

    GPIO_ConfigurePin((GPIO_PORT) uart_sel_pins.uart_rx_port, (GPIO_PIN) uart_sel_pins.uart_rx_pin, INPUT, PID_UART1_RX, false);

    if ((uart_sel_pins.uart_tx_port != uart_sel_pins.uart_rx_port) || (uart_sel_pins.uart_tx_pin != uart_sel_pins.uart_rx_pin)) {
        // Do not configure Tx pin for 1-wire UART
        GPIO_ConfigurePin((GPIO_PORT) uart_sel_pins.uart_tx_port, (GPIO_PIN) uart_sel_pins.uart_tx_pin, OUTPUT, PID_UART1_TX, false);
    }
}

void update_uart_pads(GPIO_PORT tx_port, GPIO_PIN tx_pin, GPIO_PORT rx_port, GPIO_PIN rx_pin)
{
    // Change UART Tx/Rx pins to GPIOs if the incomming pin assignment is different
    // from previous one and these pins are not reserved by a different peripheral.
    if ((((GPIO_PIN) uart_sel_pins.uart_tx_pin != tx_pin) || ((GPIO_PORT) uart_sel_pins.uart_tx_port != tx_port)) &&
         ((GPIO_GetPinFunction((GPIO_PORT) uart_sel_pins.uart_tx_port, (GPIO_PIN) uart_sel_pins.uart_tx_pin) == PID_UART1_TX) ||
          (GPIO_GetPinFunction((GPIO_PORT) uart_sel_pins.uart_tx_port, (GPIO_PIN) uart_sel_pins.uart_tx_pin) == PID_UART2_TX)))
    {
        GPIO_ConfigurePin((GPIO_PORT) uart_sel_pins.uart_tx_port, (GPIO_PIN) uart_sel_pins.uart_tx_pin, INPUT_PULLUP, PID_GPIO, false);
    }

    if ((((GPIO_PIN) uart_sel_pins.uart_rx_pin != rx_pin) || ((GPIO_PORT) uart_sel_pins.uart_rx_port != rx_port)) &&
         ((GPIO_GetPinFunction((GPIO_PORT) uart_sel_pins.uart_rx_port, (GPIO_PIN) uart_sel_pins.uart_rx_pin) == PID_UART1_RX) ||
          (GPIO_GetPinFunction((GPIO_PORT) uart_sel_pins.uart_rx_port, (GPIO_PIN) uart_sel_pins.uart_rx_pin) == PID_UART2_RX)))
    {
        GPIO_ConfigurePin((GPIO_PORT) uart_sel_pins.uart_rx_port, (GPIO_PIN) uart_sel_pins.uart_rx_pin, INPUT_PULLUP, PID_GPIO, false);
    }

    uart_sel_pins.uart_tx_port = tx_port;
    uart_sel_pins.uart_tx_pin = tx_pin;
    uart_sel_pins.uart_rx_port = rx_port;
    uart_sel_pins.uart_rx_pin = rx_pin;

#if defined (__DA14531__)
    uart_uses_p00 = (tx_pin == GPIO_PIN_0) || (rx_pin == GPIO_PIN_0);
    uart_uses_p05 = (tx_pin == GPIO_PIN_5) || (rx_pin == GPIO_PIN_5);
    uart_uses_p01 = (tx_pin == GPIO_PIN_1) || (rx_pin == GPIO_PIN_1);
    uart_uses_p10 = (tx_pin == GPIO_PIN_10) || (rx_pin == GPIO_PIN_10);
#endif
}

#endif

/**
 ****************************************************************************************
 * @brief Set GPIO port function mode for SPI interface.
 ****************************************************************************************
 */
void init_spi_pads(void)
{
    memset(&spi_sel_pins, 0, sizeof(_spi_sel_pins));
    spi_sel_pins.spi_cs_port = SPI_CS_PORT;
    spi_sel_pins.spi_cs_pin = SPI_CS_PIN;
    spi_sel_pins.spi_clk_port = SPI_CLK_PORT;
    spi_sel_pins.spi_clk_pin = SPI_CLK_PIN;
    spi_sel_pins.spi_do_port = SPI_DO_PORT;
    spi_sel_pins.spi_do_pin = SPI_DO_PIN;
    spi_sel_pins.spi_di_port = SPI_DI_PORT;
    spi_sel_pins.spi_di_pin = SPI_DI_PIN;

#if defined (__DA14531__)
    spi_uses_p00 = (SPI_CS_PIN == GPIO_PIN_0) || (SPI_CLK_PIN == GPIO_PIN_0) || (SPI_DO_PIN == GPIO_PIN_0) || (SPI_DI_PIN == GPIO_PIN_0);
#endif
}

void set_pad_spi(void)
{
#if defined (__DA14531__)
    if (spi_uses_p00 && !(gpio_uses_p00 || uart_uses_p00))
    {
        GPIO_Disable_HW_Reset();
    }
#endif

    GPIO_ConfigurePin( (GPIO_PORT) spi_sel_pins.spi_cs_port, (GPIO_PIN) spi_sel_pins.spi_cs_pin, OUTPUT, PID_SPI_EN, true );
    GPIO_ConfigurePin( (GPIO_PORT) spi_sel_pins.spi_clk_port, (GPIO_PIN) spi_sel_pins.spi_clk_pin, OUTPUT, PID_SPI_CLK, false );
    GPIO_ConfigurePin( (GPIO_PORT) spi_sel_pins.spi_do_port, (GPIO_PIN) spi_sel_pins.spi_do_pin, OUTPUT, PID_SPI_DO, false );
    GPIO_ConfigurePin( (GPIO_PORT) spi_sel_pins.spi_di_port, (GPIO_PIN) spi_sel_pins.spi_di_pin, INPUT, PID_SPI_DI, false );
}

void update_spi_pads(uint8_t *pin_buffer)
{
    spi_sel_pins.spi_cs_port  = pin_buffer[0];
    spi_sel_pins.spi_cs_pin   = pin_buffer[1];
    spi_sel_pins.spi_clk_port = pin_buffer[2];
    spi_sel_pins.spi_clk_pin  = pin_buffer[3];
    spi_sel_pins.spi_do_port  = pin_buffer[4];
    spi_sel_pins.spi_do_pin   = pin_buffer[5];
    spi_sel_pins.spi_di_port  = pin_buffer[6];
    spi_sel_pins.spi_di_pin   = pin_buffer[7];
    
#if defined (__DA14531__)
    spi_uses_p00 = false;
    for (int i = 1; i < 8; i+= 2) {
        if (pin_buffer[i] == GPIO_PIN_0) {
            spi_uses_p00 = true;
            break;
        }
    }
#endif
}

/**
 ****************************************************************************************
 * @brief Set GPIO port function mode for EEPROM interface.
 ****************************************************************************************
 */
void init_i2c_pads(void)
{
    memset(&i2c_sel_pins, 0, sizeof(_i2c_sel_pins));
    i2c_sel_pins.i2c_scl_port = I2C_SCL_PORT;
    i2c_sel_pins.i2c_scl_pin = I2C_SCL_PIN;
    i2c_sel_pins.i2c_sda_port = I2C_SDA_PORT;
    i2c_sel_pins.i2c_sda_pin = I2C_SDA_PIN;
    
#if defined (__DA14531__)
    i2c_uses_p00 = (I2C_SCL_PIN == GPIO_PIN_0) || (I2C_SDA_PIN == GPIO_PIN_0);
#endif
}

void set_pad_eeprom(void)
{
#if defined (__DA14531__)
    if (i2c_uses_p00 && !(gpio_uses_p00 || uart_uses_p00))
    {
        GPIO_Disable_HW_Reset();
    }
#endif

    GPIO_ConfigurePin( (GPIO_PORT) i2c_sel_pins.i2c_scl_port, (GPIO_PIN) i2c_sel_pins.i2c_scl_pin, INPUT, PID_I2C_SCL, false);
    GPIO_ConfigurePin( (GPIO_PORT) i2c_sel_pins.i2c_sda_port, (GPIO_PIN) i2c_sel_pins.i2c_sda_pin, INPUT, PID_I2C_SDA, false);
}

void update_eeprom_pads(uint8_t *pin_buffer)
{
    i2c_sel_pins.i2c_scl_port = pin_buffer[0];
    i2c_sel_pins.i2c_scl_pin  = pin_buffer[1];
    i2c_sel_pins.i2c_sda_port = pin_buffer[2];
    i2c_sel_pins.i2c_sda_pin  = pin_buffer[3];

#if defined (__DA14531__)
    i2c_uses_p00 = (pin_buffer[1] == GPIO_PIN_0) || (pin_buffer[3] == GPIO_PIN_0);
#endif
}
