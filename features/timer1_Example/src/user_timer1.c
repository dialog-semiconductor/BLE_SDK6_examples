/**
 ****************************************************************************************
 *
 * @file user_timer1.c
 *
 * @brief User SW Timer1 functions
 *
 * Copyright (c) 2012-2019 Dialog Semiconductor. All rights reserved.
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

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_timer1.h"
#include <stdio.h>
#include <stdint.h>
#include "arch_system.h"
#include "user_periph_setup.h"
#include "timer1.h"
#include "timer0_2.h"
#include "timer0.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
 
#if defined (COUNTING_MODE)

// Timer1 counting configuration structure
static timer1_count_options_t timer1_config =
{	
	/*Timer1 input clock*/
	.input_clk 	= 	TIM1_CLK_SRC_LP  , 	
	
	/*Timer1 free run off*/
	.free_run 	= 	TIM1_FREE_RUN_OFF,
	
	/*Timer1 IRQ mask*/
	.irq_mask	= 	TIM1_IRQ_MASK_OFF,
	
	/*Timer1 count direction*/
	.count_dir	= 	TIM1_CNT_DIR_DOWN,
	
	/*Timer1 reload value*/
	.reload_val	= 	TIM1_RELOAD_MAX
};

#endif


#if defined ( CAPTURING_MODE )

#if defined ( TIMER0_SOURCE )

//Timer0 clock configuration structure 
static tim0_2_clk_div_config_t clk_div_config =
{	
	/*Input clock division factor*/
	.clk_div  	=	TIM0_2_CLK_DIV_8 
};

#endif

//Timer1 capture / period count configuration structure
static timer1_event_options_t timer1_event_1 = 
{	
	/*Timestamp type to be stored upon each event*/
	.stamp_type			=		TIM1_EVENT_STAMP_CNT,
	
	/*Number of periods to be counted*/
	.period_count		=		100,
	
	/*Timer1 event IRQ mask*/
	.irq_mask			=		TIM1_EVENT_IRQ_MASK_OFF,
	
	/*Timer1 event type*/
	.event_type			=		TIM1_EVENT_TYPE_CAP,
	
	/*Timer1 edge type*/
	.edge_type			=		TIM1_EVENT_EDGE_RISING,
	
	/*GPIO pin to be monitored*/
	.gpio_pin			=		TIM1_EVENT_GPIO_PIN_1,
};

#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

volatile uint16_t timer0_pwm_setup_expiration_counter;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

#if defined (CAPTURING_MODE)

#if defined ( TIMER0_SOURCE )

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

    // enable SWTIM_IRQn irq
    timer0_enable_irq();   

    // start pwm0
    timer0_start();
}

#endif


/**
 ****************************************************************************************
 * @brief Timer1 event1 user callback function
 *
 ****************************************************************************************
 */
static void timer1_event1_user_callback_function(void)
{ 	
	if(timer0_pwm_setup_expiration_counter > 0)	 
	{	
		if (GPIO_GetPinStatus(LED_PORT, LED_PIN))
		{
			GPIO_SetInactive(LED_PORT, LED_PIN);
		}
		else
		{
			GPIO_SetActive(LED_PORT, LED_PIN);
		}
		
		timer0_pwm_setup_expiration_counter--;
	}
	else
	{
		GPIO_SetInactive(LED_PORT, LED_PIN);
		
		//Disable Timer1 IRQ
		timer1_disable_irq();
		
		//Stop Timer1
		timer1_stop();
		
#if defined (TIMER0_SOURCE)
		
		//Stop Timer0
		timer0_stop();

		// Disable the Timer0/Timer2 input clock
		timer0_2_clk_disable();
		
#endif
		
	}		
}

/**
 ****************************************************************************************
 * @brief Timer1 setup capturing finction
 * @param[in] void
 * @return void
 ****************************************************************************************
 */
void timer1_capturing_setup()
{			
	timer0_pwm_setup_expiration_counter = COUNTER_RELOAD_VALUE;
	
	//Start Timer1
	timer1_start();
	
	// enable SWTIM_IRQn irq
	timer1_enable_irq();	
	
#if defined (TIMER0_SOURCE)
	
	//Start Timer0
	timer0_pwm_setup();
	
#endif
	
	// Timer1 configurations and register callback function for SWTIM_IRQn irq
	timer1_event1_config(&timer1_event_1,timer1_event1_user_callback_function);
}
#endif


#if defined (COUNTING_MODE)

/**
 ****************************************************************************************
 * @brief Timer0 general user callback function
 *
 ****************************************************************************************
 */
static void timer1_general_user_callback_function(void)
{
	
	static uint8_t n = 0;
	
    // when pass  10 * 100ms
    if ( 100 == n )
    {
        n = 0;
        if (GPIO_GetPinStatus(LED_PORT, LED_PIN))
        {
            GPIO_SetInactive(LED_PORT, LED_PIN);
        }
        else
        {
            GPIO_SetActive(LED_PORT, LED_PIN);
        }
     }
     n++;	
}


 /**
 ****************************************************************************************
 * @brief Main routine of the Timer1 Software Example
 *
 ****************************************************************************************
 */
void timer1_general_test(void)
{	
	//Stop Timer1	
	timer1_stop();
	
	// Timer1 configurations and register callback function for SWTIM_IRQn irq
	timer1_count_config(&timer1_config, timer1_general_user_callback_function);
	
	// enable SWTIM_IRQn irq
	timer1_enable_irq();
	
	//Start Timer1
	timer1_start();	
}

#endif
