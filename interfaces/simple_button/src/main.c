/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Simple_Button example
 *
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
 */
#include "systick.h" 
#include "uart_utils.h"
#include "user_periph_setup.h"
#include "arch_system.h"
 
/****************************************************************************************/
/* User constants                                                                   		  	  				*/
/****************************************************************************************/
const uint32_t BLINK_INTERVAL	= 500000;	// In usec.
const uint32_t LONG_PRESS_TIME	= 3000000;	// In usec.
const uint8_t DEBOUNCE_MS	= 30;	// In ms.
const bool INPUT_LEVEL = true;	// Input will generate IRQ if input is low.
const bool EDGE_LEVEL = true;	// Wait for key release after interrupt was set.
 
// Interrupt function declarations
void systick_isr(void);
void buttonRelease_isr(void);
void buttonPress_isr(void);
	
void interruptsInit(void);
void interruptsDeinit(void);

void setTimer_ir(void);
void getTimer_ir(void);
void resetTimer_ir(void);

void setButtonPress_ir(void);
void getButtonPress_ir(void);
void resetButtonPress_ir(void);

void setButtonRelease_ir(void);
void getButtonRelease_ir(void);
void resetButtonRelease_ir(void);

// Flag for detecting long (3s=) or short press
bool three_second_push = 0;

void LED_Blink(void);
 
/**
 ****************************************************************************************
 * @brief Main routine of the Simple_Button example.
 * @return void
 ****************************************************************************************
 */
int main (void)
{
	system_init();
	periph_init();
	interruptsInit();

    while(1);
}

/**
 ****************************************************************************************
 * @brief Toggles LED (Light Emitting Diode) GPIO (General Purpose In/Out) using systick().
 * @brief Toggle time = BLINK_INTERVAL_T (time in us).
 * @return void
 ****************************************************************************************
 */
void LED_Blink(void)
{
		systick_stop();
		systick_register_callback(LED_Blink);
		if (GPIO_GetPinStatus(LED_PORT, LED_PIN))
		{
				GPIO_SetInactive(LED_PORT, LED_PIN);
				printf_string(UART,"\n\r=> LED OFF <=");
		}
		else
		{
				GPIO_SetActive(LED_PORT, LED_PIN);
				printf_string(UART,"\n\r=> LED ON  <=");
		}
		systick_start(BLINK_INTERVAL, true);
}

/**
 ****************************************************************************************
 * @brief	systick ISR (Interrupt Service Routine) handler.
 * @return	void
 ****************************************************************************************
 */
void systick_isr(void) 
{
		systick_stop();
		three_second_push = 1;
		printf_string(UART,"\n\n\rLong Press");
		LED_Blink();
}

/**
 ****************************************************************************************
 * @brief Button release ISR (Interrupt Service Routine) handler.
 * @return void
 ****************************************************************************************
 */
void buttonRelease_isr(void)
{
		// Prevents interrupt from triggering at startup
		if (GPIO_GetIRQInputLevel(GPIO1_IRQn) == GPIO_IRQ_INPUT_LEVEL_LOW)
		{
				GPIO_SetIRQInputLevel(GPIO1_IRQn, GPIO_IRQ_INPUT_LEVEL_HIGH);
				return;
		}

		if (!three_second_push)
		{
			systick_stop();
			printf_string(UART,"\n\n\rShort press");
				if (GPIO_GetPinStatus(LED_PORT, LED_PIN))
				{
						GPIO_SetInactive(LED_PORT, LED_PIN);
						printf_string(UART,"\n\r=> LED OFF <=");
				}
				else
				{
						GPIO_SetActive(LED_PORT, LED_PIN);
						printf_string(UART,"\n\r=> LED ON  <=");
				}
		}
}

/**
 ****************************************************************************************
 * @brief Button press ISR (Interrupt Service Routine) handler.
 * @brief Generates a timer. If LONG_PRESS_TIME time is passed an exception is generated.
 * @return void
 ****************************************************************************************
 */
void buttonPress_isr(void)
{
		three_second_push = 0; 																													
		systick_stop(); 																																				
		systick_register_callback(systick_isr);																			
		systick_start(LONG_PRESS_TIME, true);		
}
	
/**
 ****************************************************************************************
 * @brief Interrupts initialization.
 * @return void
 ****************************************************************************************
 */
void interruptsInit(void)
{
		setTimer_ir();
		setButtonPress_ir();
		setButtonRelease_ir();
}

/**
 ****************************************************************************************
 * @brief Interrupts deinitialization. For future use.
 * @return void
 ****************************************************************************************
 */
void interruptsDeinit(void)
{
}

/**
 ****************************************************************************************
 * @brief Set timer IR (interrupt).
 * @return void
 ****************************************************************************************
 */
void setTimer_ir(void) 
{
		systick_register_callback(systick_isr);
}

/**
 ****************************************************************************************
 * @brief Get timer IR (interrupt). For future use.
 * @return void
 ****************************************************************************************
 */
void getTimer_ir(void) 
{
}

/**
 ****************************************************************************************
 * @brief Reset timer IR (interrupt). For future use.
 * @return void
 ****************************************************************************************
 */
void resetTimer_ir(void)
{
}

/**
 ****************************************************************************************
 * @brief Set button (GPIO) press IR (interrupt).
 * @return void
 ****************************************************************************************
 */
void setButtonPress_ir(void)
{
		GPIO_EnableIRQ(GPIO_SW_PORT, GPIO_SW_PIN, GPIO0_IRQn, INPUT_LEVEL, EDGE_LEVEL, DEBOUNCE_MS);
		GPIO_SetIRQInputLevel(GPIO0_IRQn, GPIO_IRQ_INPUT_LEVEL_LOW);
		GPIO_RegisterCallback(GPIO0_IRQn, buttonPress_isr);
}

/**
 ****************************************************************************************
 * @brief Get button (GPIO) press IR (interrupt). For future use.
 * @return void
 ****************************************************************************************
 */
void getButtonPress_ir(void)
{
}

/**
 ****************************************************************************************
 * @brief Reset button (GPIO) press IR (interrupt). For future use.
 * @return void
 ****************************************************************************************
 */
void resetButtonPress_ir(void)
{
}

/**
 ****************************************************************************************
 * @brief Set button (GPIO) release IR (interrupt).
 * @return void
 ****************************************************************************************
 */
void setButtonRelease_ir(void)
{
		GPIO_EnableIRQ(GPIO_SW_PORT, GPIO_SW_PIN, GPIO1_IRQn, INPUT_LEVEL, EDGE_LEVEL, DEBOUNCE_MS);
		GPIO_SetIRQInputLevel(GPIO1_IRQn, GPIO_IRQ_INPUT_LEVEL_LOW);
		GPIO_RegisterCallback(GPIO1_IRQn, buttonRelease_isr);
}

/**
 ****************************************************************************************
 * @brief Get button (GPIO) release IR (interrupt). For future use.
 * @return void
 ****************************************************************************************
 */
void GetButtonRelease_ir(void)
{
}

/**
 ****************************************************************************************
 * @brief Reset button (GPIO) release IR (interrupt). For future use.
 * @return void
 ****************************************************************************************
 */
void resetButtonRelease_ir(void)
{
}