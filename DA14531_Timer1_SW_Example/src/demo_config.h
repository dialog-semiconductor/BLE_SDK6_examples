/**
 ****************************************************************************************
 *
 * @file demo_config.h
 *
 * @brief Demo configuration file.
 *
 * Copyright (C) 2015-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _DEMO_CONFIG_H_
#define _DEMO_CONFIG_H_

#include "timer1.h"

/*
 ****************************************************************************************
 *                               SW EXAMPLE DEMO SETTINGS                               *
 ****************************************************************************************
 */
 
/****************************************************************************************
* Enable timer 1 in free run counting
****************************************************************************************/
#define ENABLE_TMR_COUNTING             (0)

/*****************************************************************************************
* Measure a frequency applied on a GPIO. If no external frequency is available PWM 
* from timer 0 can be used for the demo (PWM_TIMER0_ENABLE). Set the MAX frequency 
* applied to the GPIO for the sw to properly set the counting period MAX_APPLIED_FREQUENCY_KHZ.
*****************************************************************************************/
#define ENABLE_FREQ_COUNTING            (0)
#define PWM_TIMER0_ENABLE               (0)
#define EXTERNAL_FREQUENCY_HZ           (0) // Set to the value of the external applied frequency else 0

/*****************************************************************************************
* Measure a low pulse length
*****************************************************************************************/
#define ENABLE_PULSE_MEASURING          (1)                  

/*
 * SW EXAMPLE TIMER 1 SETTINGS
 ****************************************************************************************
 */
#if (ENABLE_FREQ_COUNTING) && (ENABLE_PULSE_MEASURING)
#error "Enabling Timer 1 in counter and capture mode is not supported by the demo"
#endif

#if (ENABLE_FREQ_COUNTING)
#if (!PWM_TIMER0_ENABLE) 
    #if (EXTERNAL_FREQUENCY_HZ == 0)
    #error "External frequency measurement is enabled, no external maximum frequency is applied"
    #else
    #define MAX_APPLIED_FREQUENCY_HZ    EXTERNAL_FREQUENCY_HZ
    #endif
#else
    #define MAX_APPLIED_FREQUENCY_HZ    ((16000000 / (1 << TIMER0_CLOCK_DIV)) / (PWM_HIGH + PWM_LOW + 2))
#endif  // PWM_TIMER0_ENABLE

#endif  // ENABLE_FREQ_COUNTING

/* Define settings for Timer 1 */
#define TIMER1_ON_SYSCLK                1
#define TIMER1_ON_LPCLK                 0
/* Define settings for Timer 0 */
#define TIMER0_DIV_BY_1                 0
#define TIMER0_DIV_BY_2                 1
#define TIMER0_DIV_BY_4                 2
#define TIMER0_DIV_BY_8                 3

/* Generic timer settings */
#define FREE_RUN                        TIM1_FREE_RUN_ON    
#define COUNT_DIRECTION                 TIM1_CNT_DIR_UP
#define RELOAD_VALUE                    TIM1_RELOAD_MAX

/* Generic event Settings */
#define STAMP_TYPE                      TIM1_EVENT_STAMP_CNT        
#define EVENT_PIN                       (tim1_event_gpio_pin_t)(GPIO_BUTTON_PIN + 1)

/* Timer and Event Settings when frequency counting is enabled */
#if ENABLE_FREQ_COUNTING
    #define INPUT_CLK                   TIMER1_ON_LPCLK
    #define TIMER1_CLK_USED_HZ          (INPUT_CLK) ? 16000000UL : 15000UL
    #define PERIOD_COUNT                ((((RELOAD_VALUE * MAX_APPLIED_FREQUENCY_HZ) / (TIMER1_CLK_USED_HZ)) > TIM1_PERIOD_MAX) ? \
                                        TIM1_PERIOD_MAX : ((RELOAD_VALUE * MAX_APPLIED_FREQUENCY_HZ) / (TIMER1_CLK_USED_HZ)))
    #define INTERRUPT_MASK_TMR          TIM1_IRQ_MASK_ON
    #define INTERRUPT_MASK_EVT          TIM1_EVENT_IRQ_MASK_OFF
    #define EVENT_TYPE                  TIM1_EVENT_TYPE_CNT
    #define EDGE_TYPE                   TIM1_EVENT_EDGE_RISING
    #define FREQ_CNT_INTERVAL           500
    /* Timer 0 PWM Settings */
    #if PWM_TIMER0_ENABLE
        #define TIMER0_CLOCK_DIV        TIMER0_DIV_BY_8    
        #define TIMER_ON        	    1000
        #define PWM_HIGH            	500
        #define PWM_LOW         	    500
    #endif
    
#if (PERIOD_COUNT < 2)  // You need at least 2 edges to count before timer1 overflows 
    #error "The frequency under measurement is two slow for measuring with the selected clock"
#endif

#if ((2 * MAX_APPLIED_FREQUENCY_HZ) > (TIMER1_CLK_USED_HZ))
    #error "The sampling frequency should be at least two times the external frequency"
#endif
    
#elif ENABLE_PULSE_MEASURING
    #define INPUT_CLK               TIMER1_ON_LPCLK
    #define PERIOD_COUNT            TIM1_PERIOD_MAX
    #define INTERRUPT_MASK_TMR      TIM1_IRQ_MASK_ON
    #define INTERRUPT_MASK_EVT      TIM1_EVENT_IRQ_MASK_OFF
    #define EVENT_TYPE              TIM1_EVENT_TYPE_CAP
    #define EDGE_TYPE               TIM1_EVENT_EDGE_FALLING
#else
    #define INTERRUPT_MASK_TMR      TIM1_IRQ_MASK_ON
#endif

#endif  // _DEMO_CONFIG_H_
