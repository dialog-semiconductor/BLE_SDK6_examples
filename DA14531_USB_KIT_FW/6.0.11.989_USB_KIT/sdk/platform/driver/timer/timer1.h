/**
 ****************************************************************************************
 *
 * @file timer1.h
 *
 * @brief DA14531 Timer1 driver header file.
 *
 * Copyright (C) 2018 Dialog Semiconductor. All rights reserved.
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

#ifndef _TIMER1_H_
#define _TIMER1_H_

#include <stdint.h>
#include <stdbool.h>
#include "datasheet.h"

/// Maximum reload value of Timer1
// TIMER1_RELOAD field in TIMER1_CTRL_REG is 11 bits long.
#define TIM1_RELOAD_MAX     ((1U << 11U) - 1U)
/// Maximum number of periods which Timer1 may count
// TIMER1_IN1_PERIOD_MAX and TIMER1_IN2_PERIOD_MAX fields in TIMER1_CAPTURE_REG
// are 6 bits long each.
#define TIM1_PERIOD_MAX     (1U << 6U)

/// Timer1 clock disable/enable
typedef enum
{
    /// Timer1 clock disabled
    TIM1_CLK_OFF = 0,

    /// Timer1 clock enabled
    TIM1_CLK_ON  = 1
} tim1_clk_t;

/// Timer1 clock source
typedef enum
{
    /// Timer1 uses the LP clock
    TIM1_CLK_SRC_LP  = 0,

    /// Timer1 uses the system clock
    TIM1_CLK_SRC_SYS = 1
} tim1_clk_src_t;

/// Timer1 free run (only when Timer1 counts up)
typedef enum
{
    /// Timer1 goes to zero when it reaches the reload value
    TIM1_FREE_RUN_OFF = 0,

    /// Timer1 goes to zero when it reaches the max value
    TIM1_FREE_RUN_ON  = 1
} tim1_free_run_t;

/// Timer1 IRQ mask
typedef enum
{
    /// Timer1 IRQ masked
    TIM1_IRQ_MASK_ON  = 0,

    /// Timer1 IRQ unmasked
    TIM1_IRQ_MASK_OFF = 1
} tim1_irq_mask_t;

/// Timer1 count direction
typedef enum
{
    /// Timer1 counts up
    TIM1_CNT_DIR_UP   = 0,

    /// Timer1 counts down
    TIM1_CNT_DIR_DOWN = 1
} tim1_cnt_dir_t;

/// Timer1 disable/enable
typedef enum
{
    /// Timer1 disabled
    TIM1_OFF = 0,

    /// Timer1 enabled
    TIM1_ON  = 1
} tim1_t;

/// Timer1 event timestamp
typedef enum
{
    /// On each event, Timer1 stores the (internal) counter value
    TIM1_EVENT_STAMP_CNT = 0,

    /// On each event, Timer1 stores the RTC timestamp
    TIM1_EVENT_STAMP_RTC = 1
} tim1_event_stamp_t;

/// Timer1 event IRQ mask
typedef enum
{
    /// Timer1 event IRQ masked
    TIM1_EVENT_IRQ_MASK_ON  = 0,

    /// Timer1 event IRQ unmasked
    TIM1_EVENT_IRQ_MASK_OFF = 1
} tim1_event_irq_mask_t;

/// Timer1 event type
typedef enum
{
    /// Capture mode
    TIM1_EVENT_TYPE_CAP = 0,

    /// Period count mode
    TIM1_EVENT_TYPE_CNT = 1
} tim1_event_type_t;

/// Timer1 event edge
typedef enum
{
    /// Rising-edge event
    TIM1_EVENT_EDGE_RISING  = 0,

    /// Falling-edge event
    TIM1_EVENT_EDGE_FALLING = 1
} tim1_event_edge_t;

/// GPIO pin [0, 1, ..., 11] to be monitored
typedef enum
{
    /// P00 pin
    TIM1_EVENT_GPIO_PIN_0  = 1,

    /// P01 pin
    TIM1_EVENT_GPIO_PIN_1  = 2,

    /// P02 pin
    TIM1_EVENT_GPIO_PIN_2  = 3,

    /// P03 pin
    TIM1_EVENT_GPIO_PIN_3  = 4,

    /// P04 pin
    TIM1_EVENT_GPIO_PIN_4  = 5,

    /// P05 pin
    TIM1_EVENT_GPIO_PIN_5  = 6,

    /// P06 pin
    TIM1_EVENT_GPIO_PIN_6  = 7,

    /// P07 pin
    TIM1_EVENT_GPIO_PIN_7  = 8,

    /// P08 pin
    TIM1_EVENT_GPIO_PIN_8  = 9,

    /// P09 pin
    TIM1_EVENT_GPIO_PIN_9  = 10,

    /// P010 pin
    TIM1_EVENT_GPIO_PIN_10 = 11,

    /// P011 pin
    TIM1_EVENT_GPIO_PIN_11 = 12
} tim1_event_gpio_pin_t;

/// Timer1 count configuration
typedef struct
{
    /// Timer1 input clock
    tim1_clk_src_t      input_clk;

    /// Timer1 free run (only when Timer1 counts up)
    tim1_free_run_t     free_run;

    /// Timer1 IRQ mask
    tim1_irq_mask_t     irq_mask;

    /// Timer1 count direction
    tim1_cnt_dir_t      count_dir;

    /// Timer1 reload value; max value is TIM1_RELOAD_MAX
    uint16_t            reload_val;
} timer1_count_options_t;

/// Timer1 capture / period count configuration
typedef struct
{
    /// Timestamp type to be stored upon each event
    tim1_event_stamp_t      stamp_type;

    /// Number of periods to be counted; max value is TIM1_PERIOD_MAX
    uint8_t                 period_count;

    /// Timer1 event IRQ mask
    tim1_event_irq_mask_t   irq_mask;

    /// Timer1 event type
    tim1_event_type_t       event_type;

    /// Timer1 edge type
    tim1_event_edge_t       edge_type;

    /// GPIO pin [0,1,..., 11] to be monitored
    tim1_event_gpio_pin_t   gpio_pin;
} timer1_event_options_t;

/// Callback type to be associated with the Timer source of SWTIM1_IRQn.
typedef void (*timer1_count_handler_t)(void);

/// Callback type to be associated with the Capture1/Capture2 sources of SWTIM1_IRQn.
typedef void (*timer1_event_handler_t)(void);

/**
 ****************************************************************************************
 * @brief Registers a callback function to be called within SWTIM1_Handler
 * (Timer event source).
 * @param[in] callback The callback function.
 * @return void.
 ****************************************************************************************
 */
void timer1_count_register_callback(timer1_count_handler_t callback);

/**
 ****************************************************************************************
 * @brief Registers a callback function to be called within SWTIM1_Handler
 * (IN1 event source).
 * @param[in] callback The callback function.
 * @return void.
 ****************************************************************************************
 */
void timer1_event1_register_callback(timer1_event_handler_t callback);

/**
 ****************************************************************************************
 * @brief Registers a callback function to be called within SWTIM1_Handler
 * (IN2 event source).
 * @param[in] callback The callback function.
 * @return void.
 ****************************************************************************************
 */
void timer1_event2_register_callback(timer1_event_handler_t callback);

/**
 ****************************************************************************************
 * @brief Configures Timer1 to count according to @p count_options.
 * @param[in] count_options The timer (count) configuration.
 * @param[in] callback The callback function.
 * @return void.
 ****************************************************************************************
 */
void timer1_count_config(timer1_count_options_t *count_options, timer1_count_handler_t callback);

/**
 ****************************************************************************************
 * @brief Configures Timer1 to capture event or count periods
 *        (IN1 monitored channel).
 * @note timer1_start() function should have been called first.
 * @note For period count:
 *       - minimum measured frequency = (number of periods to be counted) * timer frequency / 2047,
 *       - maximum measured frequency = timer frequency / 4.
 * @note TIMER1_IN1_EVENT_FALL_EN shall be written when TIMER1_GPIO1_CONF = 0
 *       to prevent false events.
 * @param[in] event_options The timer (capture / period count) configuration.
 * @param[in] callback The callback function.
 * @return void.
 ****************************************************************************************
 */
void timer1_event1_config(timer1_event_options_t *event_options, timer1_event_handler_t callback);

/**
 ****************************************************************************************
 * @brief Configures Timer1 to capture event or count periods
 *        (IN2 monitored channel).
 * @note timer1_start() function should have been called first.
 * @note For period count:
 *       - minimum measured frequency = (number of periods to be counted) * timer frequency / 2047,
 *       - maximum measured frequency = timer frequency / 4.
 * @note TIMER1_IN2_EVENT_FALL_EN shall be written when TIMER1_GPIO2_CONF = 0
 *       to prevent false events.
 * @param[in] event_options The timer (capture / period count) configuration.
 * @param[in] callback The callback function.
 * @return void.
 ****************************************************************************************
 */
void timer1_event2_config(timer1_event_options_t *event_options, timer1_event_handler_t callback);

/**
 ****************************************************************************************
 * @brief Stops GPIO1 (IN1) input monitoring.
 * @note Shall be called before calling the timer1_event1_config() function
 *       in order to prevent false events.
 * @return void.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer1_event1_gpio_reset(void)
{
    SetBits32(TIMER1_CAPTURE_REG, TIMER1_GPIO1_CONF, 0);
}

/**
 ****************************************************************************************
 * @brief Stops GPIO2 (IN2) input monitoring.
 * @note Shall be called before calling the timer1_event2_config() function
 *       in order to prevent false events.
 * @return void.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer1_event2_gpio_reset(void)
{
    SetBits32(TIMER1_CAPTURE_REG, TIMER1_GPIO2_CONF, 0);
}

/**
 ****************************************************************************************
 * @brief Enables the timer clock as well as the timer itself.
 * @return void.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer1_start(void)
{
    SetBits32(TIMER1_CTRL_REG, TIMER1_CLK_EN, TIM1_CLK_ON);
    SetBits32(TIMER1_CTRL_REG, TIMER1_ENABLE, TIM1_ON);
}

/**
 ****************************************************************************************
 * @brief Disables the timer as well as its clock.
 * @return void.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void timer1_stop(void)
{
    SetBits32(TIMER1_CTRL_REG, TIMER1_ENABLE, TIM1_OFF);
    SetBits32(TIMER1_CTRL_REG, TIMER1_CLK_EN, TIM1_CLK_OFF);
}

/**
 ****************************************************************************************
 * @brief Enables the SWTIM1_IRQn interrupt.
 * @return void.
 ****************************************************************************************
 */
void timer1_enable_irq(void);

/**
 ****************************************************************************************
 * @brief Disables the SWTIM1_IRQn interrupt.
 * @return void.
 ****************************************************************************************
 */
void timer1_disable_irq(void);

/**
 ****************************************************************************************
 * @brief Indicates whether a new IN1 event occurred while interrupt was pending.
 * @return A boolean flag.
 ****************************************************************************************
 */
bool timer1_is_in1_ovrflw_occurred(void);

/**
 ****************************************************************************************
 * @brief Indicates whether a new IN2 event occurred while interrupt was pending.
 * @return A boolean flag.
 ****************************************************************************************
 */
bool timer1_is_in2_ovrflw_occurred(void);

#endif /* _TIMER1_H_ */
