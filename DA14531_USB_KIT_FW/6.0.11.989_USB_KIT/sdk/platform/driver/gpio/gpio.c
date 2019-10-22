/**
 ****************************************************************************************
 *
 * @file gpio.c
 *
 * @brief Hardware GPIO abstruction layer.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "datasheet.h"
#include "arch.h"
#if defined (__DA14531__) && !defined (__NON_BLE_EXAMPLE__)
#include "arch_system.h"
#endif
#include "gpio.h"
#include "user_periph_setup.h"

#if DEVELOPMENT_DEBUG
    #ifndef GPIO_DRV_PIN_ALLOC_MON_DISABLED

int GPIO[NO_OF_PORTS][NO_OF_MAX_PINS_PER_PORT]              __SECTION_ZERO("retention_mem_area0");

#if defined (__DA14531__)
static const uint16_t p_mask[1] = { 0xFFF };
#else
static const uint16_t p_mask[4] = { 0xFF, 0x3F, 0x3FF, 0x1FF };
#endif

volatile uint64_t GPIO_status                               __SECTION_ZERO("retention_mem_area0");

    #endif //GPIO_DRV_PIN_ALLOC_MON_DISABLED
#endif //DEVELOPMENT_DEBUG

#define NO_OF_IRQS (GPIO4_IRQn - GPIO0_IRQn + 1)
GPIO_handler_function_t GPIOHandlerFunction[NO_OF_IRQS]     __SECTION_ZERO("retention_mem_area0");

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG && !defined(GPIO_DRV_PIN_ALLOC_MON_DISABLED) && defined(__DA14586__)
/**
 ****************************************************************************************
 * @brief GPIO reservations for DA14586 internal SPI flash.
 *
 * @return void
 ****************************************************************************************
 */
static void da14586_internal_flash_gpio_reservations(void)
{
    RESERVE_GPIO(DA14586_INTERNAL_FLASH_SPI_CLK,      GPIO_PORT_2, GPIO_PIN_0, PID_SPI_CLK);
    RESERVE_GPIO(DA14586_INTERNAL_FLASH_SPI_FLASH_DO, GPIO_PORT_2, GPIO_PIN_9, PID_SPI_DO);
    RESERVE_GPIO(DA14586_INTERNAL_FLASH_SPI_FLASH_DI, GPIO_PORT_2, GPIO_PIN_4, PID_SPI_DI);
    RESERVE_GPIO(DA14586_INTERNAL_FLASH_SPI_FLASH_EN, GPIO_PORT_2, GPIO_PIN_3, PID_SPI_EN);
}
#endif // #if DEVELOPMENT_DEBUG && !defined(GPIO_DRV_PIN_ALLOC_MON_DISABLED) && defined(__DA14586__)

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void GPIO_init(void)
{
#if DEVELOPMENT_DEBUG
    #ifndef GPIO_DRV_PIN_ALLOC_MON_DISABLED

    volatile int i, j;

    for (i = 0; i < NO_OF_PORTS; i++)
        for (j = 0; j < NO_OF_MAX_PINS_PER_PORT; j++)
            GPIO[i][j] = 0;

#if defined(__DA14586__)
    da14586_internal_flash_gpio_reservations();
#endif // defined(__DA14586__)

    GPIO_reservations();

#if defined (__DA14531__) && !defined (__NON_BLE_EXAMPLE__)
    if (arch_clk_is_XTAL32() == true)
    {
        // NOTE: As XTAL32K is connected to P03 and P04, neither P03 nor P04
        // shall be reserved when XTAL32K has been selected as LP clock source!!
        ASSERT_WARNING((GPIO[0][3] == 0) && (GPIO[0][4] == 0));
    }
#endif

    GPIO_status = 0;

    for (i = 0; i < NO_OF_PORTS; i++)
        for (j = 0; j < NO_OF_MAX_PINS_PER_PORT; j++) {
            uint16_t bitmask = (1 << j);

            if ( !(p_mask[i] & bitmask) ) // port pin does not exist! continue to next port...
                break;

            if (GPIO[i][j] == -1) {
                __asm("BKPT #0\n"); // this pin has been previously reserved!
            }

            if (GPIO[i][j] == 0)
                continue;

            GPIO_status |= ((uint64_t)GPIO[i][j] << j) << (i * 16);
        }
    #endif //GPIO_DRV_PIN_ALLOC_MON_DISABLED
#endif //DEVELOPMENT_DEBUG
}

void GPIO_SetPinFunction(GPIO_PORT port, GPIO_PIN pin, GPIO_PUPD mode, GPIO_FUNCTION function)
{
#if DEVELOPMENT_DEBUG
    #ifndef GPIO_DRV_PIN_ALLOC_MON_DISABLED
    if ( !(GPIO_status & ( ((uint64_t)1 << pin) << (port * 16) )) )
        __asm("BKPT #0\n"); // this pin has not been previously reserved!
    #endif //GPIO_DRV_PIN_ALLOC_MON_DISABLED
#endif //DEVELOPMENT_DEBUG
 
#if !defined (__DA14531__)
    if (port == GPIO_PORT_3)
        port = GPIO_PORT_3_REMAP; // Set to 4 due to P30_MODE_REG address (0x50003086 instead of 0x50003066)
#endif

    const int data_reg = GPIO_BASE + (port << 5);
    const int mode_reg = data_reg + 0x6 + (pin << 1);

    SetWord16(mode_reg, mode | function);
}

GPIO_FUNCTION GPIO_GetPinFunction(GPIO_PORT port, GPIO_PIN pin)
{
#if !defined (__DA14531__)
    if (port == GPIO_PORT_3)
        port = GPIO_PORT_3_REMAP; // Set to 4 due to P30_MODE_REG address (0x50003086 instead of 0x50003066)
#endif

    const int data_reg = GPIO_BASE + (port << 5);
    const int mode_reg = data_reg + 0x6 + (pin << 1);

    return (GPIO_FUNCTION) GetBits16(mode_reg, PID);
}

void GPIO_ConfigurePin(GPIO_PORT port, GPIO_PIN pin, GPIO_PUPD mode, GPIO_FUNCTION function,
                        const bool high)
{
#if DEVELOPMENT_DEBUG
    #ifndef GPIO_DRV_PIN_ALLOC_MON_DISABLED
    if ( !(GPIO_status & ( ((uint64_t)1 << pin) << (port * 16) )) )
        __asm("BKPT #0\n"); // this pin has not been previously reserved!
    #endif //GPIO_DRV_PIN_ALLOC_MON_DISABLED
#endif //DEVELOPMENT_DEBUG

    if (high)
        GPIO_SetActive( port, pin );
    else
        GPIO_SetInactive( port, pin );

    GPIO_SetPinFunction( port, pin, mode, function );
}

void GPIO_ConfigurePinPower(GPIO_PORT port, GPIO_PIN pin, GPIO_POWER_RAIL power_rail)
{
#if DEVELOPMENT_DEBUG
    #ifndef GPIO_DRV_PIN_ALLOC_MON_DISABLED
    if ( !(GPIO_status & ( ((uint64_t)1 << pin) << (port * 16) )) )
        __asm("BKPT #0\n"); // this pin has not been previously reserved!
    #endif //GPIO_DRV_PIN_ALLOC_MON_DISABLED
#endif //DEVELOPMENT_DEBUG

#if defined (__DA14531__)
    // reg holds the address of the PAD_WEAK_CTRL_REG.
    const int reg = PAD_WEAK_CTRL_REG;

    // bit_offset holds the bit offset in reg that configures the power source of the given pin.
    const uint8_t bit_offset = pin;    
#else
    // reg_offset holds the half-word offset of P<x>_PADPWR_CTRL_REG that corresponds to the given port
    // relative to P01_PADPWR_CTRL_REG's address.
    const int reg_offset = (port == GPIO_PORT_0) ? 0: (port - 1);

    // reg holds the address of the corresponding P<x>_PADPWR_CTRL_REG for the given port.
    const int reg = P01_PADPWR_CTRL_REG + 2* reg_offset;

    // bit_offset holds the bit offset in reg that configures the power source of the given pin.
    const uint8_t bit_offset = (port == GPIO_PORT_1) ? (pin + 8) : pin;
#endif

    uint32_t regval = 0;

    regval = GetWord16(reg);
    regval &= ~(1 << bit_offset);        // reset bit
    regval |= power_rail << bit_offset;  // write bit
    SetWord16(reg, regval);
}

void GPIO_SetActive(GPIO_PORT port, GPIO_PIN pin)
{
#if DEVELOPMENT_DEBUG
    #ifndef GPIO_DRV_PIN_ALLOC_MON_DISABLED
    if ( !(GPIO_status & ( ((uint64_t)1 << pin) << (port * 16) )) )
        __asm("BKPT #0\n"); // this pin has not been previously reserved!
    #endif //GPIO_DRV_PIN_ALLOC_MON_DISABLED
#endif //DEVELOPMENT_DEBUG

#if !defined (__DA14531__)
    if (port == GPIO_PORT_3)
        port = GPIO_PORT_3_REMAP; // Set to 4 due to P30_MODE_REG address (0x50003086 instead of 0x50003066)
#endif

    const int data_reg = GPIO_BASE + (port << 5);
    const int set_data_reg = data_reg + 2;

    SetWord16(set_data_reg, 1 << pin);
}

void GPIO_SetInactive(GPIO_PORT port, GPIO_PIN pin)
{
#if DEVELOPMENT_DEBUG
    #ifndef GPIO_DRV_PIN_ALLOC_MON_DISABLED
    if ( !(GPIO_status & ( ((uint64_t)1 << pin) << (port * 16) )) )
        __asm("BKPT #0\n"); // this pin has not been previously reserved!
    #endif //GPIO_DRV_PIN_ALLOC_MON_DISABLED
#endif //DEVELOPMENT_DEBUG

#if !defined (__DA14531__)
    if (port == GPIO_PORT_3)
        port = GPIO_PORT_3_REMAP; // Set to 4 due to P30_MODE_REG address (0x50003086 instead of 0x50003066)
#endif

    const int data_reg = GPIO_BASE + (port << 5);
    const int reset_data_reg = data_reg + 4;

    SetWord16(reset_data_reg, 1 << pin);
}

bool GPIO_GetPinStatus(GPIO_PORT port, GPIO_PIN pin)
{
#if DEVELOPMENT_DEBUG
    #ifndef GPIO_DRV_PIN_ALLOC_MON_DISABLED
    if ( !(GPIO_status & ( ((uint64_t)1 << pin) << (port * 16) )) )
        __asm("BKPT #0\n"); // this pin has not been previously reserved!
    #endif //GPIO_DRV_PIN_ALLOC_MON_DISABLED
#endif //DEVELOPMENT_DEBUG

#if !defined (__DA14531__)
    if (port == GPIO_PORT_3)
        port = GPIO_PORT_3_REMAP; // Set to 4 due to P30_MODE_REG address (0x50003086 instead of 0x50003066)
#endif

    const int data_reg = GPIO_BASE + (port << 5);

    return ( (GetWord16(data_reg) & (1 << pin)) != 0 );
}

bool GPIO_is_valid(GPIO_PORT port, GPIO_PIN pin)
{
#if defined (__DA14531__)
    const GPIO_PORT MAX_GPIO_PORT = GPIO_PORT_0;
    const GPIO_PIN MAX_GPIO_PIN[] = {GPIO_PIN_11};
#else
    const GPIO_PORT MAX_GPIO_PORT = GPIO_PORT_3;
    const GPIO_PIN MAX_GPIO_PIN[] = {GPIO_PIN_7, GPIO_PIN_5, GPIO_PIN_9, GPIO_PIN_7};
#endif

    return ((port <= MAX_GPIO_PORT) && (pin <= MAX_GPIO_PIN[port]));
}

#ifndef GPIO_DRV_IRQ_HANDLING_DISABLED

/**
 ****************************************************************************************
 * @brief Find the offset of the first '1' in a uint16_t mask
 *
 * @param[in] f        uint16_t bit mask
 *                     http://graphics.stanford.edu/~seander/bithacks.html
 * @return c           the number of trailing zeros in the right
 ****************************************************************************************
 */
static uint16_t gpioshift16(uint16_t f)
{
    uint16_t c;  // output: c will count f's trailing zero bits,

    if (f)
    {
        f = (f ^ (f - 1)) >> 1;  // Set f's trailing 0s to 1s and zero rest
        for (c = 0; f; c++)
        {
            f >>= 1;
        }
    }
    else
    {
        c = 8 * sizeof(f);
    }
    return c;
}

#define GPIOSetBits16(a,f,d)    ( SetWord16( (a), (GetWord16(a)&(~(uint16_t)(f))) | (((uint16_t)(d))<<gpioshift16((f))) ))

void GPIO_EnableIRQ(GPIO_PORT port, GPIO_PIN pin, IRQn_Type irq, bool low_input,
                     bool release_wait, uint8_t debounce_ms)
{
    
#if defined (__DA14531__)
    const uint16_t KBRD_IRQn_SEL = 0x000F;
#else
    const uint16_t KBRD_IRQn_SEL = 0x003F;
#endif

    #if DEVELOPMENT_DEBUG
        #ifndef GPIO_DRV_PIN_ALLOC_MON_DISABLED
    if ( !(GPIO_status & ( ((uint64_t)1 << pin) << (port * 16) )) )
        __asm("BKPT #0\n"); // this pin has not been previously reserved!
        #endif //GPIO_DRV_PIN_ALLOC_MON_DISABLED
    #endif //DEVELOPMENT_DEBUG

#if defined (__DA14531__)
    const uint8_t   KBRD_IRQn_SEL_BASE[] = {
        1,
    };
#else
    const uint8_t   KBRD_IRQn_SEL_BASE[] = {
        1,
        9,
        15,
        25
    };
#endif

    GPIOSetBits16(GPIO_DEBOUNCE_REG, (DEB_ENABLE0 << (irq-GPIO0_IRQn)), (debounce_ms > 0) );
    GPIOSetBits16(GPIO_DEBOUNCE_REG, DEB_VALUE, debounce_ms);
    GPIOSetBits16(GPIO_INT_LEVEL_CTRL_REG, EDGE_LEVELn0 << (irq-GPIO0_IRQn), release_wait);
    GPIOSetBits16(GPIO_INT_LEVEL_CTRL_REG, INPUT_LEVEL0 << (irq-GPIO0_IRQn), low_input);
    GPIOSetBits16(GPIO_IRQ0_IN_SEL_REG + 2*(irq-GPIO0_IRQn), KBRD_IRQn_SEL, KBRD_IRQn_SEL_BASE[port] + pin);

    NVIC_SetPriority(irq, 2);
    NVIC_EnableIRQ(irq);
}

GPIO_IRQ_INPUT_LEVEL GPIO_GetIRQInputLevel(IRQn_Type irq)
{
    return (GPIO_IRQ_INPUT_LEVEL) GetBits16(GPIO_INT_LEVEL_CTRL_REG, (INPUT_LEVEL0 << (irq-GPIO0_IRQn)));
}

void GPIO_SetIRQInputLevel(IRQn_Type irq, GPIO_IRQ_INPUT_LEVEL level)
{
    GPIOSetBits16(GPIO_INT_LEVEL_CTRL_REG, (INPUT_LEVEL0 << (irq-GPIO0_IRQn)), level);
}

void GPIO_ResetIRQ( IRQn_Type irq )
{
    GPIOSetBits16(GPIO_RESET_IRQ_REG, RESET_GPIO0_IRQ << (irq-GPIO0_IRQn), 1);
}

void GPIO_RegisterCallback(IRQn_Type irq, GPIO_handler_function_t callback)
{
    if ((irq<=GPIO4_IRQn) && (irq>=GPIO0_IRQn))
        GPIOHandlerFunction[irq - GPIO0_IRQn] = callback;
}

/**
 ****************************************************************************************
 * @brief GPIO IRQn Handler
 * @param[in] irq   The IRQ that this handler services
 * @return void
 ****************************************************************************************
 */
void GPIOn_Handler(IRQn_Type irq)
{
    if ((irq<=GPIO4_IRQn) && (irq>=GPIO0_IRQn))
    {
        GPIO_ResetIRQ(irq);
        GPIO_handler_function_t callback = GPIOHandlerFunction[irq-GPIO0_IRQn];
        if (callback != NULL)
            callback();
        else
            NVIC_ClearPendingIRQ(irq);
    }
}

#if !defined (__EXCLUDE_GPIO0_HANDLER__)
/**
 ****************************************************************************************
 * @brief GPIO IRQ0 Handler
 *
 * @return void
 ****************************************************************************************
 */
void GPIO0_Handler(void)
{
    GPIOn_Handler(GPIO0_IRQn);
}
#endif

#if !defined (__EXCLUDE_GPIO1_HANDLER__)
/**
 ****************************************************************************************
 * @brief GPIO IRQ1 Handler
 *
 * @return void
 ****************************************************************************************
 */
void GPIO1_Handler(void)
{
    GPIOn_Handler(GPIO1_IRQn);
}
#endif

#if !defined (__EXCLUDE_GPIO2_HANDLER__)
/**
 ****************************************************************************************
 * @brief GPIO IRQ2 Handler
 *
 * @return void
 ****************************************************************************************
 */
void GPIO2_Handler(void)
{
    GPIOn_Handler(GPIO2_IRQn);
}
#endif

#if !defined (__EXCLUDE_GPIO3_HANDLER__)
/**
 ****************************************************************************************
 * @brief GPIO IRQ3 Handler
 *
 * @return void
 ****************************************************************************************
 */
void GPIO3_Handler(void)
{
    GPIOn_Handler(GPIO3_IRQn);
}
#endif

#if !defined (__EXCLUDE_GPIO4_HANDLER__)
/**
 ****************************************************************************************
 * @brief GPIO IRQ4 Handler
 *
 * @return void
 ****************************************************************************************
 */
void GPIO4_Handler(void)
{
    GPIOn_Handler(GPIO4_IRQn);
}
#endif

#endif //GPIO_DRV_IRQ_HANDLING_DISABLED

void GPIO_EnablePorPin(GPIO_PORT port, GPIO_PIN pin, GPIO_POR_PIN_POLARITY polarity, uint8_t por_time)
{
    ASSERT_WARNING(GPIO_POR_PIN_REG(port, pin, polarity))
    SetWord16(POR_TIMER_REG, POR_TIME & por_time);
    SetWord16(POR_PIN_REG, GPIO_POR_PIN_REG(port, pin, polarity));
}

void GPIO_DisablePorPin()
{
    SetWord16(POR_PIN_REG, 0);
}

void GPIO_SetPorTime(uint8_t por_time)
{
    SetWord16(POR_TIMER_REG, POR_TIME & por_time);
}

uint8_t GPIO_GetPorTime()
{
    return (POR_TIME & GetWord16(POR_TIMER_REG));
}

