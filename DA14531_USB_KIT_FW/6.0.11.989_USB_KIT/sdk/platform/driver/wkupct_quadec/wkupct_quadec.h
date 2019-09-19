/**
 ****************************************************************************************
 *
 * @file wucpt_quadec.h
 *
 * @brief Wakeup IRQ & Quadrature Decoder driver header file.
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

/* Important note: If, upon reception of interrupt from the wakeup controller or the quadrature
 *                 decoder, the system resumes from sleep mode and you wish to resume peripherals
 *                 functionality , it is necessary to include in your interrupt handler function(s)
 *                 - the ones you register using wkupct_register_callback() and/or
 *                                                quad_decoder_register_callback() -
 *                  the following lines:
 *
 *                    // Init System Power Domain blocks: GPIO, WD Timer, Sys Timer, etc.
 *                    // Power up and init Peripheral Power Domain blocks,
 *                    // and finally release the pad latches.
 *                    if(GetBits16(SYS_STAT_REG, PER_IS_DOWN))
 *                        periph_init();
 *
*/

#ifndef _WKUPCT_QUADEC_H_
#define _WKUPCT_QUADEC_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include "datasheet.h"

/*
 * DEFINES
 ****************************************************************************************
 */

enum
{
    WKUPCT_PIN_POLARITY_HIGH = 0,  // the event counter is incremented if the respective input goes HIGH
    WKUPCT_PIN_POLARITY_LOW  = 1,  // the event counter is incremented if the respective input goes LOW
};

/**
 ****************************************************************************************
 * @brief Helper macro for the "pin selection" bitmask expected by function wkupct_enable_irq().
 * @param[in] port    GPIO port
 * @param[in] pin     GPIO pin
 * @return The bitmask value that selects the given GPIO
 * @note When the macro is used with constant arguments an optimizing compiler will collapse it
 *       to a constant value.
 *       If the macro arguments contain an illegal port/pin pair then a compilation error
 *       will be generated.
 ****************************************************************************************
 */
#if defined (__DA14531__)
#define WKUPCT_PIN_SELECT(port, pin) ( 1U << ( port == GPIO_PORT_0 && pin <= GPIO_PIN_11 ? pin :        \
                                               -1                                                       \
                                             )                                                          \
                                     )
#else
#define WKUPCT_PIN_SELECT(port, pin) ( 1U << ( port == GPIO_PORT_0 && pin <= GPIO_PIN_7 ? pin :        \
                                                port == GPIO_PORT_1 && pin <= GPIO_PIN_5 ? 8 + pin :    \
                                                port == GPIO_PORT_2 && pin == GPIO_PIN_8 ? 14 :         \
                                                port == GPIO_PORT_2 && pin == GPIO_PIN_9 ? 15 :         \
                                                port == GPIO_PORT_2 && pin <= GPIO_PIN_7 ? 16 + pin :   \
                                                port == GPIO_PORT_3 && pin <= GPIO_PIN_7 ? 24 + pin :   \
                                               -1                                                       \
                                             )                                                          \
                                     )
#endif

/**
 ****************************************************************************************
 * @brief Helper macro for the "pin polarity" bitmask expected by function wkupct_enable_irq().
 * @param[in] port        GPIO port
 * @param[in] pin         GPIO pin
 * @param[in] polarity    The required polarity. Can be either WKUPCT_PIN_POLARITY_HIGH
 *                        or  WKUPCT_PIN_POLARITY_LOW.
 * @return The corresponding bitmask for the given GPIO's polarity.
 * @note When the macro is used with constant arguments an optimizing compiler will collapse it
 *       to a constant value.
 *       If the macro arguments contain an illegal port/pin pair then a compilation error
 *       will be generated.
 ****************************************************************************************
 */
#if defined (__DA14531__)
#define WKUPCT_PIN_POLARITY(port, pin, polarity) ( (polarity) << ( port == GPIO_PORT_0 && pin <= GPIO_PIN_11 ? pin :          \
                                                                   -1                                                        \
                                                                 )                                                           \
                                                 )
#else
#define WKUPCT_PIN_POLARITY(port, pin, polarity) ( (polarity) << ( port == GPIO_PORT_0 && pin <= GPIO_PIN_7 ? pin :          \
                                                                   port == GPIO_PORT_1 && pin <= GPIO_PIN_5 ? 8 + pin :      \
                                                                   port == GPIO_PORT_2 && pin == GPIO_PIN_8 ? 14 :           \
                                                                   port == GPIO_PORT_2 && pin == GPIO_PIN_9 ? 15 :           \
                                                                   port == GPIO_PORT_2 && pin <= GPIO_PIN_7 ? 16 + pin :     \
                                                                   port == GPIO_PORT_3 && pin <= GPIO_PIN_7 ? 24 + pin :     \
                                                                   -1                                                        \
                                                                 )                                                           \
                                                 )
#endif

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef enum
{
    WKUPCT_QUADEC_ERR_RESERVED = -1,
    WKUPCT_QUADEC_ERR_OK = 0,
} wkupct_quadec_error_t;

typedef void (*wakeup_handler_function_t)(void);

typedef void (*quad_encoder_handler_function_t)(int16_t qdec_xcnt_reg, int16_t qdec_ycnt_reg, int16_t qdec_zcnt_reg);

typedef enum
{
#if defined (__DA14531__)
    QUAD_DEC_CHXA_NONE_AND_CHXB_NONE = 0,
    QUAD_DEC_CHXA_P02_AND_CHXB_P05 = 1,
    QUAD_DEC_CHXA_P01_AND_CHXB_P04 = 2,
    QUAD_DEC_CHXA_P03_AND_CHXB_P10 = 3,
    QUAD_DEC_CHXA_P06_AND_CHXB_P07 = 4,
    QUAD_DEC_CHXA_P08_AND_CHXB_P09 = 5,
    QUAD_DEC_CHXA_P00_AND_CHXB_P11 = 6,
#else
    QUAD_DEC_CHXA_NONE_AND_CHXB_NONE = 0,
    QUAD_DEC_CHXA_P00_AND_CHXB_P01 = 1,
    QUAD_DEC_CHXA_P02_AND_CHXB_P03 = 2,
    QUAD_DEC_CHXA_P04_AND_CHXB_P05 = 3,
    QUAD_DEC_CHXA_P06_AND_CHXB_P07 = 4,
    QUAD_DEC_CHXA_P10_AND_CHXB_P11 = 5,
    QUAD_DEC_CHXA_P12_AND_CHXB_P13 = 6,
    QUAD_DEC_CHXA_P23_AND_CHXB_P24 = 7,
    QUAD_DEC_CHXA_P25_AND_CHXB_P26 = 8,
    QUAD_DEC_CHXA_P27_AND_CHXB_P28 = 9,
    QUAD_DEC_CHXA_P29_AND_CHXB_P20 = 10
#endif
} CHX_PORT_SEL_t;

#if defined (__DA14531__)
typedef enum
{
    QUAD_DEC_CHX_NORMAL_COUNTING = 0 << 9,
    QUAD_DEC_CHX_EDGE_COUNTING = 1 << 9
} CHX_EVENT_MODE_t;
#endif

typedef enum
{
#if defined (__DA14531__)
    QUAD_DEC_CHYA_NONE_AND_CHYB_NONE = 0 << 3,
    QUAD_DEC_CHYA_P02_AND_CHYB_P05 = 1 << 3,
    QUAD_DEC_CHYA_P01_AND_CHYB_P04 = 2 << 3,
    QUAD_DEC_CHYA_P03_AND_CHYB_P10 = 3 << 3,
    QUAD_DEC_CHYA_P06_AND_CHYB_P07 = 4 << 3,
    QUAD_DEC_CHYA_P08_AND_CHYB_P09 = 5 << 3,
    QUAD_DEC_CHYA_P00_AND_CHYB_P11 = 6 << 3
#else
    QUAD_DEC_CHYA_NONE_AND_CHYB_NONE = 0 << 4,
    QUAD_DEC_CHYA_P00_AND_CHYB_P01 = 1 << 4,
    QUAD_DEC_CHYA_P02_AND_CHYB_P03 = 2 << 4,
    QUAD_DEC_CHYA_P04_AND_CHYB_P05 = 3 << 4,
    QUAD_DEC_CHYA_P06_AND_CHYB_P07 = 4 << 4,
    QUAD_DEC_CHYA_P10_AND_CHYB_P11 = 5 << 4,
    QUAD_DEC_CHYA_P12_AND_CHYB_P13 = 6 << 4,
    QUAD_DEC_CHYA_P23_AND_CHYB_P24 = 7 << 4,
    QUAD_DEC_CHYA_P25_AND_CHYB_P26 = 8 << 4,
    QUAD_DEC_CHYA_P27_AND_CHYB_P28 = 9 << 4,
    QUAD_DEC_CHYA_P29_AND_CHYB_P20 = 10 << 4
#endif
} CHY_PORT_SEL_t;

#if defined (__DA14531__)
typedef enum
{
    QUAD_DEC_CHY_NORMAL_COUNTING = 0 << 10,
    QUAD_DEC_CHY_EDGE_COUNTING = 1 << 10
} CHY_EVENT_MODE_t;
#endif

typedef enum
{
#if defined (__DA14531__)
    QUAD_DEC_CHZA_NONE_AND_CHZB_NONE = 0 << 6,
    QUAD_DEC_CHZA_P02_AND_CHZB_P05 = 1 << 6,
    QUAD_DEC_CHZA_P01_AND_CHZB_P04 = 2 << 6,
    QUAD_DEC_CHZA_P03_AND_CHZB_P10 = 3 << 6,
    QUAD_DEC_CHZA_P06_AND_CHZB_P07 = 4 << 6,
    QUAD_DEC_CHZA_P08_AND_CHZB_P09 = 5 << 6,
    QUAD_DEC_CHZA_P00_AND_CHZB_P11 = 6 << 6
#else
    QUAD_DEC_CHZA_NONE_AND_CHZB_NONE = 0 << 8,
    QUAD_DEC_CHZA_P00_AND_CHZB_P01 = 1 << 8,
    QUAD_DEC_CHZA_P02_AND_CHZB_P03 = 2 << 8,
    QUAD_DEC_CHZA_P04_AND_CHZB_P05 = 3 << 8,
    QUAD_DEC_CHZA_P06_AND_CHZB_P07 = 4 << 8,
    QUAD_DEC_CHZA_P10_AND_CHZB_P11 = 5 << 8,
    QUAD_DEC_CHZA_P12_AND_CHZB_P13 = 6 << 8,
    QUAD_DEC_CHZA_P23_AND_CHZB_P24 = 7 << 8,
    QUAD_DEC_CHZA_P25_AND_CHZB_P26 = 8 << 8,
    QUAD_DEC_CHZA_P27_AND_CHZB_P28 = 9 << 8,
    QUAD_DEC_CHZA_P29_AND_CHZB_P20 = 10 << 8
#endif
} CHZ_PORT_SEL_t;

#if defined (__DA14531__)
typedef enum
{
    QUAD_DEC_CHZ_NORMAL_COUNTING = 0 << 11,
    QUAD_DEC_CHZ_EDGE_COUNTING = 1 << 11
} CHZ_EVENT_MODE_t;
#endif

typedef struct
{
#if defined (__DA14531__)
    CHX_EVENT_MODE_t chx_event_mode;
    CHY_EVENT_MODE_t chy_event_mode;
    CHZ_EVENT_MODE_t chz_event_mode;
#endif
    CHX_PORT_SEL_t chx_port_sel;
    CHY_PORT_SEL_t chy_port_sel;
    CHZ_PORT_SEL_t chz_port_sel;
    uint16_t qdec_clockdiv;
    uint8_t qdec_events_count_to_trigger_interrupt;
} QUAD_DEC_INIT_PARAMS_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Enable Wakeup IRQ.
 * @param[in] sel_pins      Select enabled inputs (0-disabled, 1-enabled)
 *                          For DA14585
 *                          - Bits 0-7   -> port 0(P00..P07)
 *                          - Bits 8-13  -> port 1(P10..P15)
 *                          - Bits 14-15 -> port 2(P28,P29)
 *                          - Bits 16-23 -> port 2(P20..P27)
 *                          - Bits 24-31 -> port 3(P30..P37)
 *                          For DA14531
 *                          - Bits 0-11  -> port 0(P00..P011)
 * @param[in] pol_pins      Inputs' polarity (0-high, 1-low)
 *                          For DA14585
 *                          - Bits 0-7   -> port 0(P00..P07)
 *                          - Bits 8-13  -> port 1(P10..P15)
 *                          - Bits 14-15 -> port 2(P28,P29)
 *                          - Bits 16-23 -> port 2(P20..P27)
 *                          - Bits 24-31 -> port 3(P30..P37)
 *                          For DA14531
 *                          - Bits 0-11  -> port 0(P00..P011)
 * @param[in] events_num    Number of events before wakeup interrupt. Max 255.
 * @param[in] deb_time      Debouncing time. Max 0x3F (63 msec)
 * @return void
 ****************************************************************************************
 */
void wkupct_enable_irq(uint32_t sel_pins, uint32_t pol_pins, uint16_t events_num, uint16_t deb_time);

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Enable Wakeup2 IRQ.
 * @param[in] sel_pins      Select enabled inputs (0-disabled, 1-enabled)
 *                          - Bits 0-11  -> port 0(P00..P011)
 * @param[in] pol_pins      Inputs' polarity (0-high, 1-low)
 *                          - Bits 0-11  -> port 0(P00..P011)
 * @param[in] events_num    Number of events before wakeup interrupt. Max 255.
 * @param[in] deb_time      Debouncing time. Max 0x3F (63 msec)
 * @return void
 ****************************************************************************************
 */
void wkupct2_enable_irq(uint32_t sel_pins, uint32_t pol_pins, uint16_t events_num, uint16_t deb_time);
#endif

/**
 ****************************************************************************************
 * @brief Unregisters WKUP_QUADEC_IRQn use by Wakeup controller and calls
 * wkupct_quad_disable_IRQ.
 * @note WKUP_QUADEC_IRQn interrupt will be disabled, only if also the Quadrature Decoder
 * has unregistered from using the IRQ.
 * @return WKUPCT_QUADEC_ERR_OK, WKUPCT_QUADEC_ERR_RESERVED
 ****************************************************************************************
 */
wkupct_quadec_error_t wkupct_disable_irq(void);

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Unregisters WKUP_QUADEC_IRQn use by Wakeup controller and calls
 * wkupct_quad_disable_IRQ.
 * @note WKUP_QUADEC_IRQn interrupt will be disabled, only if also the Quadrature Decoder
 * and Wakeup controller has unregistered from using the IRQ.
 * @return WKUPCT_QUADEC_ERR_OK, WKUPCT_QUADEC_ERR_RESERVED
 ****************************************************************************************
 */
wkupct_quadec_error_t wkupct2_disable_irq(void);
#endif

/**
 ****************************************************************************************
 * @brief Register Callback function for Wakeup IRQ.
 * @param[in] callback Callback function's reference
 * @return void
 ****************************************************************************************
 */
void wkupct_register_callback(wakeup_handler_function_t callback);

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Register Callback function for Wakeup2 IRQ.
 * @param[in] callback Callback function's reference
 * @return void
 ****************************************************************************************
 */
void wkupct2_register_callback(wakeup_handler_function_t callback);
#endif

/**
 ****************************************************************************************
 * @brief Register Callback function for Quadrature Decoder IRQ.
 * @param[in] callback Callback function's reference.
 * @return void
 ****************************************************************************************
 */
void quad_decoder_register_callback(quad_encoder_handler_function_t callback);

/**
 ****************************************************************************************
 * @brief Initialize Quadrature Decoder.
 * @param[in] quad_dec_init_params Pointer to QUAD_DEC_INIT_PARAMS_t structure
 * @return void
 ****************************************************************************************
 */
void quad_decoder_init(const QUAD_DEC_INIT_PARAMS_t *quad_dec_init_params);

/**
 ****************************************************************************************
 * @brief Release Quadrature Decoder.
 * @return void
 ****************************************************************************************
 */
void quad_decoder_release(void);

/**
 ****************************************************************************************
 * @brief Get Quadrature Decoder X counter.
 * @return counter value
 ****************************************************************************************
 */
__STATIC_FORCEINLINE int16_t quad_decoder_get_x_counter(void)
{
    return GetWord16(QDEC_XCNT_REG);
}

/**
 ****************************************************************************************
 * @brief Get Quadrature Decoder Y counter.
 * @return counter value
 ****************************************************************************************
 */
__STATIC_FORCEINLINE int16_t quad_decoder_get_y_counter(void)
{
    return GetWord16(QDEC_YCNT_REG);
}

/**
 ****************************************************************************************
 * @brief Get Quadrature Decoder Z counter.
 * @return counter value
 ****************************************************************************************
 */
__STATIC_FORCEINLINE int16_t quad_decoder_get_z_counter(void)
{
    return GetWord16(QDEC_ZCNT_REG);
}

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Get Quadrature Decoder event counter.
 * @return counter value
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t quad_decoder_get_event_counter(void)
{
    return (QDEC_EVENT_CNT & GetWord16(QDEC_EVENT_CNT_REG));
}

/**
 ****************************************************************************************
 * @brief Clear Quadrature Decoder event counter.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void quad_decoder_clear_event_counter(void)
{
    SetWord16(QDEC_CTRL_REG, QDEC_EVENT_CNT_CLR | (GetWord16(QDEC_CTRL_REG) & (~QDEC_IRQ_STATUS)));
}
#endif

/**
 ****************************************************************************************
 * @brief Enable Quadrature Decoder interrupt.
 * @param[in] event_count event counter
 * @return void
 ****************************************************************************************
 */
void quad_decoder_enable_irq(uint8_t event_count);

/**
 ****************************************************************************************
 * @brief Unregisters WKUP_QUADEC_IRQn use by Quadrature Decoder and calls
 * wkupct_quad_disable_IRQ.
 * @note WKUP_QUADEC_IRQn interrupt will be disabled, only if also the Wakeup controller
 * has unregistered from using the IRQ.
 * @return WKUPCT_QUADEC_ERR_OK, WKUPCT_QUADEC_ERR_RESERVED
 ****************************************************************************************
 */
wkupct_quadec_error_t quad_decoder_disable_irq(void);

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Clears respective pending interrupt from Quadrature Decoder and NVIC and masks
 * quad decoder interrupt.
 * @note By default the Quad Decoder is enabled by the hardware on power up. The Quad 
 * Decoder has to be stopped and its pending interrupt has to be cleared, if the Quad 
 * Decoder is not used by the application.
 * @return void
 ****************************************************************************************
 */
void quad_decoder_clear_irq(void);
#endif

#endif // _WKUPCT_QUADEC_H_
