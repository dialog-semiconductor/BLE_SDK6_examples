/**
 ****************************************************************************************
 *
 * @file gpio.h
 *
 * @brief Hardware GPIO abstraction layer API.
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

#ifndef _GPIO_H_
#define _GPIO_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include "datasheet.h"
#include "arch.h"

/*
 * DEFINES
 ****************************************************************************************
 */

typedef enum {
    INPUT = 0,
    INPUT_PULLUP = 0x100,
    INPUT_PULLDOWN = 0x200,
    OUTPUT = 0x300,
} GPIO_PUPD;

typedef enum {
    GPIO_POWER_RAIL_3V = 0,
    GPIO_POWER_RAIL_1V = 1,
} GPIO_POWER_RAIL;

typedef enum {
    GPIO_PORT_0 = 0,
#if !defined (__DA14531__)
    GPIO_PORT_1 = 1,
    GPIO_PORT_2 = 2,
    GPIO_PORT_3 = 3,
    GPIO_PORT_3_REMAP = 4,
#endif
} GPIO_PORT;

typedef enum {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1 = 1,
    GPIO_PIN_2 = 2,
    GPIO_PIN_3 = 3,
    GPIO_PIN_4 = 4,
    GPIO_PIN_5 = 5,
    GPIO_PIN_6 = 6,
    GPIO_PIN_7 = 7,
    GPIO_PIN_8 = 8,
    GPIO_PIN_9 = 9,
#if defined (__DA14531__)
    GPIO_PIN_10 = 10,
    GPIO_PIN_11 = 11,
#endif
} GPIO_PIN;

#if defined (__DA14531__)
typedef enum {
    PID_GPIO = FUNC_GPIO,
    PID_UART1_RX = FUNC_UART_RX,
    PID_UART1_TX = FUNC_UART_TX,
    PID_UART2_RX = FUNC_UART2_RX,
    PID_UART2_TX = FUNC_UART2_TX,
    PID_SYS_CLK = FUNC_SYS_CLK,
    PID_LP_CLK = FUNC_LP_CLK,
    PID_I2C_SCL = FUNC_I2C_SCL,
    PID_I2C_SDA = FUNC_I2C_SDA,
    PID_PWM5 = FUNC_PWM5,
    PID_PWM6 = FUNC_PWM6,
    PID_PWM7 = FUNC_PWM7,
    PID_ADC = FUNC_ADC,
    PID_PWM0 = FUNC_PWM0,
    PID_PWM1 = FUNC_PWM1,
    PID_BLE_DIAG = FUNC_BLE_DIAG,
    PID_UART1_CTSN = FUNC_UART_CTSN,
    PID_UART1_RTSN = FUNC_UART_RTSN,
    PID_PWM2 = FUNC_PWM2,
    PID_PWM3 = FUNC_PWM3,
    PID_PWM4 = FUNC_PWM4,
    PID_SPI_DI = FUNC_SPI_DI,
    PID_SPI_DO = FUNC_SPI_DO,
    PID_SPI_CLK = FUNC_SPI_CLK,
    PID_SPI_EN = FUNC_SPI_CSN0,
    PID_SPI_EN1 = FUNC_SPI_CSN1,
} GPIO_FUNCTION;
#else // (__DA14585__, __DA14586__)
typedef enum {
    PID_GPIO = FUNC_GPIO,
    PID_UART1_RX = FUNC_UART_RX,
    PID_UART1_TX = FUNC_UART_TX,
    PID_UART2_RX = FUNC_UART2_RX,
    PID_UART2_TX = FUNC_UART2_TX,
    PID_SPI_DI = FUNC_SPI_DI,
    PID_SPI_DO = FUNC_SPI_DO,
    PID_SPI_CLK = FUNC_SPI_CLK,
    PID_SPI_EN = FUNC_SPI_EN,
    PID_I2C_SCL = FUNC_I2C_SCL,
    PID_I2C_SDA = FUNC_I2C_SDA,
    PID_UART1_IRDA_RX = FUNC_UART_IRDA_RX,
    PID_UART1_IRDA_TX = FUNC_UART_IRDA_TX,
    PID_UART2_IRDA_RX = FUNC_UART2_IRDA_RX,
    PID_UART2_IRDA_TX = FUNC_UART2_IRDA_TX,
    PID_ADC = FUNC_ADC,
    PID_PWM0 = FUNC_PWM0,
    PID_PWM1 = FUNC_PWM1,
    PID_BLE_DIAG = FUNC_BLE_DIAG,
    PID_UART1_CTSN = FUNC_UART_CTSN,
    PID_UART1_RTSN = FUNC_UART_RTSN,
    PID_UART2_CTSN = FUNC_UART2_CTSN,
    PID_UART2_RTSN = FUNC_UART2_RTSN,
    PID_PWM2 = FUNC_PWM2,
    PID_PWM3 =FUNC_PWM3,
    PID_PWM4 = FUNC_PWM4,
    PID_PCM_DI=FUNC_PCM_DI,
    PID_PCM_DO = FUNC_PCM_DO,
    PID_PCM_FSC = FUNC_PCM_FSC,
    PID_PCM_CLK = FUNC_PCM_CLK,
    PID_PDM_DATA = FUNC_PDM_DATA,
    PID_PDM_CLK = FUNC_PDM_CLK,
} GPIO_FUNCTION;
#endif

typedef enum {
    GPIO_IRQ_INPUT_LEVEL_HIGH = 0,
    GPIO_IRQ_INPUT_LEVEL_LOW = 1,
} GPIO_IRQ_INPUT_LEVEL;

typedef enum {
    GPIO_POR_PIN_POLARITY_LOW = 0,
    GPIO_POR_PIN_POLARITY_HIGH = 1,
} GPIO_POR_PIN_POLARITY;

// GPIO base address
#define GPIO_BASE   P0_DATA_REG

/**
 ****************************************************************************************
 * @brief Helper macro that calculates the value to be written into POR_PIN_REG register.
 * @param[in] port          GPIO port.
 * @param[in] pin           GPIO pin.
 * @param[in] polarity      0 = Active low, 1 = Active high.
 * @return The value to be written into POR_PIN_REG register. If the port/pin/polarity
 *         values are illegal, the returned value is POR_PIN_REG_RESET (reset value).
 ****************************************************************************************
 */
#if defined (__DA14531__)
#define GPIO_POR_PIN_REG(port, pin, polarity)                                                                                            \
        ( ((port == GPIO_PORT_0) && (pin <= GPIO_PIN_11) && (polarity <= GPIO_POR_PIN_POLARITY_HIGH)) ? ((polarity << 7) | (pin + 1))  : \
        POR_PIN_REG_RESET )
#else
#define GPIO_POR_PIN_REG(port, pin, polarity)                                                                                            \
        ( ((port == GPIO_PORT_0) && (pin <= GPIO_PIN_7) && (polarity <= GPIO_POR_PIN_POLARITY_HIGH)) ? ((polarity << 7) | (pin + 1))  :  \
          ((port == GPIO_PORT_1) && (pin <= GPIO_PIN_5) && (polarity <= GPIO_POR_PIN_POLARITY_HIGH)) ? ((polarity << 7) | (pin + 9))  :  \
          ((port == GPIO_PORT_2) && (pin <= GPIO_PIN_9) && (polarity <= GPIO_POR_PIN_POLARITY_HIGH)) ? ((polarity << 7) | (pin + 15)) :  \
          ((port == GPIO_PORT_3) && (pin <= GPIO_PIN_7) && (polarity <= GPIO_POR_PIN_POLARITY_HIGH)) ? ((polarity << 7) | (pin + 25)) :  \
        POR_PIN_REG_RESET )
#endif

//
// Macro for pin definition structure
//      name: usage and/or module using it
//      func: GPIO, UART1_RX, UART1_TX, etc.
//
#if DEVELOPMENT_DEBUG && !defined(GPIO_DRV_PIN_ALLOC_MON_DISABLED)
#define RESERVE_GPIO( name, port, pin, func )   { GPIO[port][pin] = (GPIO[port][pin] != 0) ? (-1) : 1; GPIO_status |= ((uint64_t)GPIO[port][pin] << pin) << (port * 16);}
#else
#define RESERVE_GPIO( name, port, pin, func )   {}
#endif //DEVELOPMENT_DEBUG && !defined(GPIO_DRV_PIN_ALLOC_MON_DISABLED)

#if DEVELOPMENT_DEBUG
#ifndef GPIO_DRV_PIN_ALLOC_MON_DISABLED

#if defined (__DA14531__)
#define NO_OF_PORTS 1   // cannot be bigger than 1
// NOTE: Modify for small package!!
#define NO_OF_MAX_PINS_PER_PORT 12  // cannot be bigger than 12
#else
#define NO_OF_PORTS 4   // cannot be bigger than 4
#define NO_OF_MAX_PINS_PER_PORT 10  // cannot be bigger than 10
#endif

extern int GPIO[NO_OF_PORTS][NO_OF_MAX_PINS_PER_PORT];

extern volatile uint64_t GPIO_status;
#endif //GPIO_DRV_PIN_ALLOC_MON_DISABLED
#endif //DEVELOPMENT_DEBUG

typedef void (*GPIO_handler_function_t)(void);

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Register Callback function for GPIO IRQ.
 * @param[in] irq       The handler of this IRQ will call the function
 * @param[in] callback  Callback function's reference.
 * @return void
 ****************************************************************************************
 */
void GPIO_RegisterCallback(IRQn_Type irq, GPIO_handler_function_t callback);

/**
 ****************************************************************************************
 * @brief Function to set the interrupt generated by the GPIO pin.
 * @param[in] port          GPIO port
 * @param[in] pin           GPIO pin
 * @param[in] irq           GPIO IRQ
 * @param[in] low_input     TRUE generates an IRQ if the input is low
 * @param[in] release_wait  TRUE waits for key release after interrupt was reset
 * @param[in] debounce_ms   duration of a debounce sequence before an IRQ is generated
 * @return void
 ****************************************************************************************
 */
void GPIO_EnableIRQ(GPIO_PORT port,
                    GPIO_PIN pin,
                    IRQn_Type irq,
                    bool low_input,
                    bool release_wait,
                    uint8_t debounce_ms);

/**
 ****************************************************************************************
 * @brief Returns the GPIO input level that triggers a GPIO IRQ.
 * @param[in] irq   GPIO IRQ number (e.g. GPIO2_IRQn)
 * @return input level that triggers the GPIO IRQ (see GPIO_IRQ_INPUT_LEVEL)
 ****************************************************************************************
 */
GPIO_IRQ_INPUT_LEVEL GPIO_GetIRQInputLevel(IRQn_Type irq);

/**
 ****************************************************************************************
 * @brief Set the GPIO input level that triggers a GPIO IRQ.
 * @param[in] irq       GPIO IRQ (e.g. GPIO2_IRQn)
 * @param[in] level     Input level that triggers the GPIO IRQ (see GPIO_IRQ_INPUT_LEVEL)
 * @return void
 ****************************************************************************************
 */
void GPIO_SetIRQInputLevel(IRQn_Type irq, GPIO_IRQ_INPUT_LEVEL level);

/**
 ****************************************************************************************
 * @brief Function to reset the interrupt generated by the GPIO pin.
 * @param[in] irq   GPIO IRQ
 * @return void
 ****************************************************************************************
 */
void GPIO_ResetIRQ(IRQn_Type irq);

/**
 ****************************************************************************************
 * @brief Initialize the GPIO assignment check variables.
 * @return void
 ****************************************************************************************
 */
void GPIO_init(void);

/**
 ****************************************************************************************
 * @brief Set the pin type and mode
 * @param[in] port     GPIO port
 * @param[in] pin      GPIO pin
 * @param[in] mode     GPIO pin mode. INPUT = 0, INPUT_PULLUP = 0x100, INPUT_PULLDOWN = 0x200, OUTPUT = 0x300,
 * @param[in] function GPIO pin usage. GPIO_FUNCTION enumeration.
 * @return void
 ****************************************************************************************
 */
void GPIO_SetPinFunction(GPIO_PORT port, GPIO_PIN pin, GPIO_PUPD mode,
                                GPIO_FUNCTION function);

/**
 ****************************************************************************************
 * @brief Get the function of the pin
 * @param[in] port     GPIO port
 * @param[in] pin      GPIO pin
 * @return the function of the pin
 ****************************************************************************************
 */
GPIO_FUNCTION GPIO_GetPinFunction(GPIO_PORT port, GPIO_PIN pin);

/**
 ****************************************************************************************
 * @brief Combined function to set the state and the type and mode of the GPIO pin.
 * @param[in] port     GPIO port
 * @param[in] pin      GPIO pin
 * @param[in] mode     GPIO pin mode. INPUT = 0, INPUT_PULLUP = 0x100, INPUT_PULLDOWN = 0x200, OUTPUT = 0x300,
 * @param[in] function GPIO pin usage. GPIO_FUNCTION enumaration.
 * @param[in] high     set to TRUE to set the pin into high else low
 * @return void
 ****************************************************************************************
 */
void GPIO_ConfigurePin(GPIO_PORT port, GPIO_PIN pin, GPIO_PUPD mode,
                              GPIO_FUNCTION function, const bool high);

/**
 ****************************************************************************************
 * @brief Sets a pin high. The GPIO should have been previously configured as output!
 * @param[in] port     GPIO port
 * @param[in] pin      GPIO pin
 * @return void
 ****************************************************************************************
 */
void GPIO_SetActive(GPIO_PORT port, GPIO_PIN pin);

/**
 ****************************************************************************************
 * @brief Sets the GPIO low. The GPIO should have been previously configured as output!
 * @param[in] port     GPIO port
 * @param[in] pin      GPIO pin
 * @return void
 ****************************************************************************************
 */
void GPIO_SetInactive(GPIO_PORT port, GPIO_PIN pin);

/**
 ****************************************************************************************
 * @brief Gets the GPIO status. The GPIO should have been previously configured as input!
 * @param[in] port     GPIO port
 * @param[in] pin      GPIO pin
 * @return bool        TRUE if the pin is high, FALSE if low.
 ****************************************************************************************
*/
bool GPIO_GetPinStatus(GPIO_PORT port, GPIO_PIN pin);

/**
 ****************************************************************************************
 * @brief Set the power source of the GPIO pin.
 * @param[in] port          GPIO port
 * @param[in] pin           GPIO pin
 * @param[in] power_rail    GPIO power rail. See GPIO_POWER_RAIL enumeration.
 * @return void
 ****************************************************************************************
 */
void GPIO_ConfigurePinPower(GPIO_PORT port, GPIO_PIN pin, GPIO_POWER_RAIL power_rail);

/**
 ****************************************************************************************
 * @brief Function to verify if the GPIO port and pin combination is valid.
 * @param[in] port      GPIO port
 * @param[in] pin       GPIO pin
 * @return bool         TRUE if the combination is valid, FALSE if it is not.
 ****************************************************************************************
 */
bool GPIO_is_valid(GPIO_PORT port, GPIO_PIN pin);

/**
 ****************************************************************************************
 * @brief       Controls the latch configuration of the GPIO pads
 * @param[in]   pad_en          When set to true, the mode and state of the pad registers
 *                              will be passed directly to the respective pads.
 *                              When set to false, the state of the pads will be retained.
 *                              Any change on the mode and state of the pad registers will
 *                              not affect the pads until PAD_LATCH_EN field is set to 1.
 * @return      void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void GPIO_set_pad_latch_en(bool pad_en)
{
#if defined (__DA14531__)
    SetBits16(PAD_LATCH_REG, PAD_LATCH_EN, pad_en);
#else
    SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, pad_en);
#endif
}

/**
 ****************************************************************************************
 * @brief Enable the GPIO Power-On Reset (POR) source.
 * @param[in] port          GPIO port
 * @param[in] pin           GPIO pin
 * @param[in] polarity      GPIO por pin polarity. 0 = Active low, 1 = Active high.
 * @param[in] por_time      Time for the Power-On Reset to happen. The time is calculated
 *                          based on the following equation:
 *                          Time = por_time x 4096 x RC32 clock period
 * @return void
 ****************************************************************************************
 */
void GPIO_EnablePorPin(GPIO_PORT port, GPIO_PIN pin, GPIO_POR_PIN_POLARITY polarity, uint8_t por_time);

/**
 ****************************************************************************************
 * @brief Disable the GPIO Power-On Reset (POR) source.
 * @return void
 ****************************************************************************************
 */
void GPIO_DisablePorPin(void);

/**
 ****************************************************************************************
 * @brief Set the Power-On Reset (POR) time.
 * @param[in] por_time      Time for the Power-On Reset to happen. The time is calculated
 *                          based on the following equation:
 *                          Time = por_time x 4096 x RC32 clock period
 * @return void
 ****************************************************************************************
 */
void GPIO_SetPorTime(uint8_t por_time);

/**
 ****************************************************************************************
 * @brief Get the Power-On Reset (POR) time.
 * @return The por_time value.
 *         The time for the POR to happen is: Time = por_time x 4096 x RC32 clock period.
 ****************************************************************************************
 */
uint8_t GPIO_GetPorTime(void);

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Disable hardware reset functionality on P00.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void GPIO_Disable_HW_Reset(void)
{
    SetWord16(HWR_CTRL_REG, 1);
}

/**
 ****************************************************************************************
 * @brief Enable hardware reset functionality on P00.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void GPIO_Enable_HW_Reset(void)
{
    SetWord16(HWR_CTRL_REG, 0);
}
#endif

#endif // _GPIO_H_
