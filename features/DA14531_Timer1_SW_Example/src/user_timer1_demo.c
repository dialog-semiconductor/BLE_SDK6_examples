/**
 ****************************************************************************************
 *
 * @file user_timer1_demo.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (C) 2012-2021 Renesas Electronics Corporation and/or its affiliates
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

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "arch_system.h"
#include "user_timer1_demo.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "timer1.h"
#include "arch_console.h"
#include "demo_config.h"

#if ( ENABLE_FREQ_COUNTING && PWM_TIMER0_ENABLE )
#include "timer0_2.h"
#include "timer0.h"
#endif

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
 
// Timer1 counting configuration structure
static timer1_count_options_t timer1_config =
{	
	/*Timer1 input clock*/
	.input_clk 	= 	(tim1_clk_src_t)INPUT_CLK,
	
	/*Timer1 free run off*/
	.free_run 	= 	FREE_RUN,
	
	/*Timer1 IRQ mask*/
	.irq_mask	= 	INTERRUPT_MASK_TMR,
	
	/*Timer1 count direction*/
	.count_dir	= 	COUNT_DIRECTION,
	
	/*Timer1 reload value*/
	.reload_val	= 	RELOAD_VALUE
};

#if ( ENABLE_PULSE_MEASURING || ENABLE_FREQ_COUNTING)
//Timer1 capture / period count configuration structure
static timer1_event_options_t timer1_event_config_ch1 = 
{	
	/*Timestamp type to be stored upon each event*/
	.stamp_type			=		STAMP_TYPE,
	
	/*Number of periods to be counted*/
	.period_count		=		PERIOD_COUNT,
	
	/*Timer1 event IRQ mask*/
	.irq_mask			=		INTERRUPT_MASK_EVT,
	
	/*Timer1 event type*/
	.event_type			=		EVENT_TYPE,
	
	/*Timer1 edge type*/
	.edge_type			=		EDGE_TYPE,
	
	/*GPIO pin to be monitored*/
	.gpio_pin			=		EVENT_PIN,
};
#endif

#if ENABLE_PULSE_MEASURING
static timer1_event_options_t timer1_event_config_ch2 = 
{
    /*Timestamp type to be stored upon each event*/
	.stamp_type			=		STAMP_TYPE,
	
	/*Number of periods to be counted*/
	.period_count		=		TIM1_PERIOD_MAX,
	
	/*Timer1 event IRQ mask*/
	.irq_mask			=		INTERRUPT_MASK_EVT,
	
	/*Timer1 event type*/
	.event_type			=		EVENT_TYPE,
	
	/*Timer1 edge type*/
	.edge_type			=		(tim1_event_edge_t)!EDGE_TYPE,
	
	/*GPIO pin to be monitored*/
	.gpio_pin			=		EVENT_PIN,
};
#endif

#if ENABLE_FREQ_COUNTING
//Timer0 clock configuration structure 
static tim0_2_clk_div_config_t clk_div_config =
{	
	/*Input clock division factor*/
	.clk_div  	=	(tim0_2_clk_div_t)TIMER0_CLOCK_DIV 
};
#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
// Retained variables

#if (ENABLE_PULSE_MEASURING)
uint16_t capture_val_evt1                       __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint16_t capture_val_evt2                       __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint32_t event_in_cycles	                    __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint32_t number_of_reloads                      __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/* Pulse measurement related variables */
enum pulse_meas_sm_state sm_state               __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
enum pulse_meas_sm_state idle_state             __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

#endif

#if (ENABLE_FREQ_COUNTING)
bool first_measurement                          __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
#endif

#if (ENABLE_TMR_COUNTING)
uint8_t timer1_cnt_ovf                          __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
bool led_state                                  __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
#endif
extern rcx_time_data_t rcx_time_data;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
#if (ENABLE_FREQ_COUNTING)
static void perform_freq_measurement(void);
#endif

#if (ENABLE_PULSE_MEASURING || ENABLE_TMR_COUNTING )
static void timer1_overflow(void);
#endif

#if (PWM_TIMER0_ENABLE && ENABLE_FREQ_COUNTING)
/**
 ****************************************************************************************
 * @brief Timer0 PWM setup function
 * @return void
 ****************************************************************************************
 */
void timer0_pwm_setup(void)
{
    // Enable the Timer0/Timer2 input clock
    timer0_2_clk_enable();

    // Set the Timer0/Timer2 input clock division factor to 8, so 16 MHz / 8 = 2 MHz input clock
    timer0_2_clk_div_set(&clk_div_config);

    // initialize PWM with the desired settings
    timer0_init(TIM0_CLK_FAST, PWM_MODE_ONE, TIM0_CLK_NO_DIV);

    // set pwm Timer0 'On', Timer0 'high' and Timer0 'low' reload values
    timer0_set(TIMER_ON, PWM_HIGH, PWM_LOW);

    // register callback function for SWTIM_IRQn irq
    timer0_register_callback(NULL);

    // enable SWTIM_IRQn irq
    timer0_enable_irq();
	
	arch_printf("TIMER0 starts") ;	
	arch_printf("\n\r") ;
	
	// start pwm0
    timer0_start();
}
#endif

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}

void user_app_init(void)
{
    default_app_on_init();
    
    timer1_initialize();

#if (ENABLE_TMR_COUNTING)
    timer1_clear_all_events();
    timer1_enable_irq();
    timer1_start();
#endif
    
#if (ENABLE_PULSE_MEASURING)
    initialize_pulse_length_measure();
#endif
}

void user_app_adv_start(void)
{
#if (ENABLE_FREQ_COUNTING)
    // The frequency measurement is triggered periodically via a sw timer
    app_easy_timer(FREQ_CNT_INTERVAL, perform_freq_measurement);  
#endif
    
    default_advertise_operation();  
}

void timer1_initialize(void)
{
    /*Enable PD_TIM power domain*/
	SetBits16(PMU_CTRL_REG, TIM_SLEEP, 0);                  // Enable the PD_TIM
#if (ENABLE_PULSE_MEASURING | ENABLE_TMR_COUNTING)
    timer1_count_config(&timer1_config, timer1_overflow);   // Set the capture counting configurations for the timer
#elif (ENABLE_FREQ_COUNTING)
    timer1_count_config(&timer1_config, NULL);              // Set the frequency counting configurations for the timer
#endif

#ifdef CFG_PRINTF
    arch_printf("Timer 1 is initialized \n\r") ;
#endif // CFG_PRINTF
}

#if (ENABLE_TMR_COUNTING)

static void user_timer_wakeup_ble(void)
{
#ifdef CFG_PRINTF
    arch_printf("BLE is awake via timer 1\n\r");
#endif
}

static void toggle_led(void)
{
    if(led_state)
    {
        GPIO_SetInactive(GPIO_LED_PORT, GPIO_LED_PIN);
        led_state = false;
    }
    else
    {
        GPIO_SetActive(GPIO_LED_PORT, GPIO_LED_PIN);
        led_state = true;
    }
}

static void timer1_overflow(void)
{
    if (timer1_cnt_ovf == TIMER1_OVF_COUNTER)
    {
        toggle_led();
        
        timer1_cnt_ovf = 0;
        /* Wake up the device and print out the measured time */
        arch_ble_force_wakeup();                // Force the BLE to wake up
        app_easy_wakeup_set(user_timer_wakeup_ble);
        app_easy_wakeup();                      // Send the message to print
    }
    
    timer1_cnt_ovf++;
}

#endif

#if (ENABLE_FREQ_COUNTING)

void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msgid)
    {	
        case FREQ_MEAS_READY_MSG:
        {
            struct freq_meas_ready *meas = (struct freq_meas_ready*)param;
            uint32_t frequency;
            
            // Calculate the measured frequency of the device
            if (timer1_config.input_clk == TIM1_CLK_SRC_SYS)
                frequency = (timer1_event_config_ch1.period_count * 16000000) / meas->captrure_val;
            else
                frequency = (timer1_event_config_ch1.period_count * rcx_time_data.rcx_freq) / meas->captrure_val;
            
            // Print out the results
            arch_printf("Counting edges %d\n\r", timer1_event_config_ch1.period_count);
            arch_printf("Pulses during measurement period %d\n\r", meas->captrure_val) ;
            arch_printf("Frequency measured is %d Hz\n\r", frequency) ;
            
#if (PWM_TIMER0_ENABLE && ENABLE_FREQ_COUNTING)            
            timer0_release();
            arch_set_extended_sleep(false);
#endif  // PWM_TIMER0_ENABLE
            
            app_easy_timer(FREQ_CNT_INTERVAL, perform_freq_measurement);      // Reset the timer for the next measurement
        }break;

        default:
            break;
    }
}

static void perform_freq_measurement(void)
{       
#if (PWM_TIMER0_ENABLE && ENABLE_FREQ_COUNTING)
    // Disable Sleep, Timer 0 is not active while sleep
    arch_disable_sleep();
    // Start Timer0
    timer0_pwm_setup();
#endif
    start_frequency_counting();                         // Start the frequency counting
}

static void frequency_measurement_end_cb(void)
{    
    TOGGLE_CURSOR(GPIO_PIN_6)
    
    if(!first_measurement)
    {
        // Send the acquired measurement to the application
        struct freq_meas_ready *ind = KE_MSG_ALLOC(FREQ_MEAS_READY_MSG, TASK_APP, TASK_APP, freq_meas_ready);
        ind->captrure_val = GetBits16(TIMER1_CAPCNT1_VALUE_REG, TIMER1_CAPCNT1_VALUE) + 1;
        ke_msg_send(ind);
        // Stop the measurement
        stop_frequency_counting();
    }
    else
        first_measurement = false;
}

void start_frequency_counting(void)
{
    // enable SWTIM_IRQn irq
	timer1_enable_irq();
    // start the timer for the frequency counting
    timer1_start();
    // enable the frequency counting event
    timer1_event1_config(&timer1_event_config_ch1, frequency_measurement_end_cb);
    /* 
     * The first measurement isn't accurate, so it will be discarded. All the following measurements are proper
     * The timer will count the pulses from starting the timer up to the edges defined
     */
    first_measurement = true;
}

void stop_frequency_counting(void)
{
    timer1_stop();                  // Stop the timer
    timer1_disable_irq();           // Disable timer 1 interrupts
    timer1_clear_all_events();      // Clear all pending events
}

#endif // ENABLE_FREQ_COUNTING

#if (ENABLE_PULSE_MEASURING)

static void pulse_measure_sm(enum pulse_meas_sm_events event);

/**
 ****************************************************************************************
 * @brief Timer 1 overflow callback
 ****************************************************************************************
*/
static void timer1_overflow(void)
{
    TOGGLE_CURSOR(TIMER_OVFL)
    /* 
     * The ovf int is checked first in the ISR, check if an event has occurred before that
     * if it did, dont increase the overflow counting
    */
    if(!GetBits32(TIMER1_STATUS_REG, TIMER1_IN2_EVENT) || !GetBits32(TIMER1_STATUS_REG, TIMER1_IN1_EVENT))
        number_of_reloads++;
    
    if(sm_state == idle_state)
    {
        number_of_reloads--;
        timer1_enable_overflow_intr(false);
        event_in_cycles = (idle_state == HIGH_WAIT_LOW_ST) ? 
        (((number_of_reloads << 11) + capture_val_evt2) - capture_val_evt1) : 
        (((number_of_reloads << 11) + capture_val_evt1) - capture_val_evt2);
        
        capture_val_evt1 = 0;
        capture_val_evt2 = 0;
        number_of_reloads = 0;

        /* Wake up the device and print out the measured time */
        arch_ble_force_wakeup();    // Force the BLE to wake up
        app_easy_wakeup();          // Send the message to print
    }
}

/**
 ****************************************************************************************
 * @brief Executes as soon as the device is awake and a measurements has finished 
 ****************************************************************************************
*/
static void report_measurement_result(void)
{
    TOGGLE_CURSOR(REPORT_TIME)
    
    uint64_t time;
    
    if(timer1_config.input_clk == TIM1_CLK_SRC_SYS)
        time = ((uint64_t)event_in_cycles * 625)/10000;
    else if (timer1_config.input_clk == TIM1_CLK_SRC_LP)
        time = ((uint64_t)event_in_cycles * 1000000)/rcx_time_data.rcx_freq;
    
#ifdef CFG_PRINTF
    arch_printf("Pin asserted for %d cycles \n\r", event_in_cycles);
    arch_printf("Time in asserted state %d us \n\r", (uint32_t)time);
#endif //CFG_PRINTF
    
    // Set the message to be send when the measurement ends
    app_easy_wakeup_set(report_measurement_result);
}

/**
 ****************************************************************************************
 * @brief Event 1 callback
 ****************************************************************************************
*/
static void timer1_start_of_measurement_cb(void)
{
    pulse_measure_sm(FALLING_EDGE_EVT);
}

/**
 ****************************************************************************************
 * @brief Event 2 callback
 ****************************************************************************************
*/
static void timer1_end_of_measurement_cb(void)
{
    pulse_measure_sm(RISING_EDGE_EVT);
}

/**
 ****************************************************************************************
 * @brief Initialize the pulse measurement state machine
 ****************************************************************************************
*/
static void initialize_pulse_length_sm(void)
{
    idle_state = (GPIO_GetPinStatus(GPIO_SIGNAL_PORT, GPIO_SIGNAL_PIN)) ? HIGH_WAIT_LOW_ST : LOW_WAIT_HIGH_ST;
    sm_state = idle_state;
}

void initialize_pulse_length_measure(void)
{
    // Set the message to be send when the measurement ends
    app_easy_wakeup_set(report_measurement_result);
    // Initialize the state machine
    initialize_pulse_length_sm();
    // Start the timer before setting the events
    timer1_start();
    // Initialize the 1st event (falling)
    timer1_event1_config(&timer1_event_config_ch1, timer1_start_of_measurement_cb);
    // Initialize the 2nd event (rising)
    timer1_event2_config(&timer1_event_config_ch2, timer1_end_of_measurement_cb);
    // Clear any pending interrupts in timer 
    timer1_clear_all_events();
    // Clear any pending interrupt in the NVIC
    NVIC_ClearPendingIRQ(SWTIM1_IRQn);
    // enable SWTIM_IRQn irq
	timer1_enable_irq();
}

static void pulse_measure_sm(enum pulse_meas_sm_events event)
{   
    switch(sm_state)
    {
        case HIGH_WAIT_LOW_ST:
        {
            switch(event)
            {
                case RISING_EDGE_EVT:
                    TOGGLE_CURSOR(RISING_EDGE)
                    break;
                case FALLING_EDGE_EVT:
                {
                    if(!GPIO_GetPinStatus(GPIO_SIGNAL_PORT, GPIO_SIGNAL_PIN) && capture_val_evt1 == 0 )
                    {
                        TOGGLE_CURSOR(FALLING_EDGE)
                        capture_val_evt1 = timer1_get_event_ch1_capture_value();
                        
                        if(idle_state != LOW_WAIT_HIGH_ST)
                            timer1_enable_overflow_intr(true);
                        
                        sm_state = LOW_WAIT_HIGH_ST;
                    }
                }
                    break;
            }
        }
            break;
        
        case LOW_WAIT_HIGH_ST:
        {
            switch(event)
            {
                case RISING_EDGE_EVT:
                {
                    if (GPIO_GetPinStatus(GPIO_SIGNAL_PORT, GPIO_SIGNAL_PIN) && capture_val_evt2 == 0)
                    {
                        TOGGLE_CURSOR(RISING_EDGE)
                        capture_val_evt2 = timer1_get_event_ch2_capture_value();
                        
                        if(idle_state != HIGH_WAIT_LOW_ST)
                            timer1_enable_overflow_intr(true);          // start measuring
                        
                        sm_state = HIGH_WAIT_LOW_ST;
                    }
                }
                    break;
                case FALLING_EDGE_EVT:
                    TOGGLE_CURSOR(FALLING_EDGE)
                    break;
            }
        }
            break;
        
        default:
            break;
    }
}

#endif

/// @} APP
