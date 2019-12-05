/**
 ****************************************************************************************
 *
 * @file user_hibernation_timer.c
 *
 * @brief Barebone project source code.
 *
 * Copyright (c) 2015-2019 Dialog Semiconductor. All rights reserved.
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

#include "rwip_config.h"             // SW configuration
#include "gap.h"
#include "app_easy_timer.h"
#include "user_hibernation_timer.h"
#include "co_bt.h"
#include "co_math.h"
#include "battery.h"

#include "user_periph_setup.h"
#include "spi_flash.h"

#ifdef CFG_DEEP_SLEEP_MODE
#include "rtc.h"
#endif

/*
 * DEFINITIONS
 ****************************************************************************************
 */
#define PREV_TEMPER_COUNT               5
#define PREV_TEMPER_INIT_VALUE          25000                                       //In milli-degrees of Celsius
#define PREV_TEMPER_SET_VALUE           27000                                       //In milli-degrees of Celsius

#define ADV_TIMER_CANCEL_TIMEOUT        2500                                        //In tens of milli-second (x10ms)
#define DONE_TIMER_TIMEOUT              1                                           //In tens of milli-second (x10ms)
#define RTC_TIMEOUT                     ADV_TIMER_CANCEL_TIMEOUT / 100              //In seconds

#define ADV_DATA_BATTERY_OFFSET         10                                          //Byte offset of battery level in Advertising Data
#define ADV_DATA_TEMPER_OFFSET          12                                          //Byte offset of temperature level in Advertising Data
#define ADV_DATA_ADV_COUNT_OFFSET       14                                          //Byte offset of advertising counter in Advertising Data

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

// Retained variables
int32_t saved_temper[PREV_TEMPER_COUNT]       __SECTION("retention_mem_area_uninit"); //Previous saved output temperatures
uint32_t adv_count                            __SECTION("retention_mem_area_uninit"); //Advertising counter          

timer_hnd adv_timer                           __SECTION("retention_mem_area_uninit"); //Advertising cancel event timer

uint16_t reset_stat_local                     __SECTION("retention_mem_area_uninit"); //Status of RESET_STAT_REG on wake-up or power-on

#ifdef CFG_HIBERNATION_MODE
timer_hnd done_timer                          __SECTION("retention_mem_area_uninit"); //Deassertion timer for the DONE signal of TPL5010
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief Saves the status of RESET_STAT_REG when the device is powered up or woken up 
 *        from sleep
 * @note Provided as a weak function implementation by the SDK. 
 * @return void
 ****************************************************************************************
 */
void reset_indication(uint16_t reset_stat)
{
    reset_stat_local = reset_stat;
}

/**
 ****************************************************************************************
 * @brief Output the next temperature value. The value is a mathematical random variable
 *        around the PREV_TEMP_SET_VALUE +/- 500mC.
 * @return Next temperature value in milli-degrees of Celsius
 ****************************************************************************************
 */
static int32_t user_update_temper(void)
{
    uint8_t rand_val;
    int32_t next_temper_value = 0;
    
    //Ask the TRNG for a random byte value
    rand_val = co_rand_byte();
    
    //Sum the previous PREV_TEMPER_COUNT-1 values
    int i;
    for (i=1; i<PREV_TEMPER_COUNT; i++)
    {
        uint8_t idx = (adv_count + PREV_TEMPER_COUNT - i) % PREV_TEMPER_COUNT;
        next_temper_value += saved_temper[idx];
    }
    
    //Add the next temperature value and filter it with a moving average
    next_temper_value += PREV_TEMPER_SET_VALUE + (1000 * rand_val)/255 - 500;
    next_temper_value /= PREV_TEMPER_COUNT;

    //Update advertising count and store the temperature
    adv_count++;
    saved_temper[adv_count % PREV_TEMPER_COUNT] = next_temper_value;
    
    return next_temper_value;
}

/**
 ****************************************************************************************
 * @brief Callback function to stop advertising
 * @return void
 ****************************************************************************************
 */
static void user_non_connectable_advertise_with_timeout_stop_cb(void)
{
    app_easy_gap_advertise_stop();
}

#ifdef CFG_HIBERNATION_MODE
/**
 ****************************************************************************************
 * @brief Callback function to deassert the DONE signal of TPL5010
 * @return void
 ****************************************************************************************
 */
static void done_timer_cb(void)
{
    GPIO_ConfigurePin(HIB_DONE_PORT, HIB_DONE_PIN, INPUT_PULLDOWN, PID_GPIO, false);
}
#endif

#if defined(CFG_DEEP_SLEEP_MODE)
/**
 ****************************************************************************************
 * @brief Real-time clock interrupt handler.
 * @note Stub function
 * @return void
 ****************************************************************************************
*/
static void rtc_interrupt_hdlr(uint8_t event)
{
   //Stub function
}
#endif

/**
 ****************************************************************************************
 * @brief Configures the real-time clock. The interrupt generated will wake us up from
 *        deep sleep
 * @return void
 ****************************************************************************************
*/
#ifdef CFG_DEEP_SLEEP_MODE
static void configure_rtc_wakeup(void)
{
    rtc_time_t alarm_time;

    // Init RTC
    rtc_reset();

    // Configure the RTC clock; RCX is the RTC clock source (14420 Hz)
    rtc_clk_config(RTC_DIV_DENOM_1000, 14420);
    rtc_clock_enable();

    rtc_config_t cfg = {.hour_clk_mode = RTC_HOUR_MODE_24H, .keep_rtc = 0};

    rtc_time_t time = {.hour_mode = RTC_HOUR_MODE_24H, .pm_flag = 0, .hour = 0,
                       .minute = 0, .sec = 0, .hsec = 00};

    // Alarm interrupt in ten seconds
    alarm_time = time;
    alarm_time.sec += RTC_TIMEOUT;

    // Initialize RTC, set time and data, register interrupt handler callback function and enable seconds interrupt
    rtc_init(&cfg);

    // Start RTC
    rtc_set_time_clndr(&time, NULL);
    rtc_set_alarm(&alarm_time, NULL, RTC_ALARM_EN_SEC);

    // Clear pending interrupts
    rtc_get_event_flags();
    rtc_register_intr(rtc_interrupt_hdlr, RTC_INTR_ALRM);
}
#endif

/**
 ****************************************************************************************
 * @brief Starts non-connectable advertising and sets a timer to stop it.
 * @return void
 ****************************************************************************************
*/
static void user_non_connectable_advertise_with_timeout_start(void)
{
    //Set up the timer which will cancel the advertising
    adv_timer = app_easy_timer(ADV_TIMER_CANCEL_TIMEOUT, user_non_connectable_advertise_with_timeout_stop_cb);
    
    //Start non-connectable advertising
    app_easy_gap_non_connectable_advertise_start();
}

/**
 ****************************************************************************************
 * @brief Handles the update of the Advertising Data fields and starts advertising.
 * @return void
 ****************************************************************************************
*/
void user_app_adv_start(void)
{
    uint8_t stored_adv_data[ADV_DATA_LEN];
    int32_t next_temper;
    uint16_t next_battery;
    
    //Get the active advertising configuration struct
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_non_connectable_advertise_get_active();
    
    //Store advertising data
    memcpy(stored_adv_data, cmd->info.host.adv_data, cmd->info.host.adv_data_len);

    //Update advertising battery voltage
    next_battery = battery_get_voltage(BATT_CR2032);
    
    stored_adv_data[ADV_DATA_BATTERY_OFFSET]     = (uint8_t) ((next_battery & 0xFF00) >> 8);
    stored_adv_data[ADV_DATA_BATTERY_OFFSET + 1] = (uint8_t) (next_battery & 0x00FF);
        
    //Update advertising temperature
    next_temper = user_update_temper();

    stored_adv_data[ADV_DATA_TEMPER_OFFSET]     = next_temper / 1000;
    stored_adv_data[ADV_DATA_TEMPER_OFFSET + 1] = (next_temper - next_temper / 1000) / 10;
    
    //Update the advertising count
    stored_adv_data[ADV_DATA_ADV_COUNT_OFFSET]     = (uint8_t) ((adv_count & 0xFF000000) >> 24);
    stored_adv_data[ADV_DATA_ADV_COUNT_OFFSET + 1] = (uint8_t) ((adv_count & 0x00FF0000) >> 16);
    stored_adv_data[ADV_DATA_ADV_COUNT_OFFSET + 2] = (uint8_t) ((adv_count & 0x0000FF00) >> 8);
    stored_adv_data[ADV_DATA_ADV_COUNT_OFFSET + 3] = (uint8_t) (adv_count & 0x000000FF);

    //Copy the updated fields into the advertising data
    memcpy(cmd->info.host.adv_data, stored_adv_data, cmd->info.host.adv_data_len);
  
    //Start non-connectable advertising
    user_non_connectable_advertise_with_timeout_start();
}

/**
 ****************************************************************************************
 * @brief Initializes the temperature values and the advertising counter on a power-on 
 *        reset
 * @return void
 ****************************************************************************************
*/
void user_app_on_init(void)
{
    //Check if the system was power-cycled
    if(reset_stat_local == 15) {
        //Initialize temperature values to PREV_TEMPER_INIT_VALUE
        int i;
        for (i=0; i<PREV_TEMPER_COUNT; i++)
            saved_temper[i] = PREV_TEMPER_INIT_VALUE;
        
        //...and advertising count to zero
        adv_count = 0;
    }

#ifdef CFG_HIBERNATION_MODE    
    //Send a DONE pulse as an acknowledgement to TPL5010
    GPIO_ConfigurePin(HIB_DONE_PORT, HIB_DONE_PIN, OUTPUT, PID_GPIO, true); 
    done_timer = app_easy_timer(ADV_TIMER_CANCEL_TIMEOUT, done_timer_cb);
#endif
    
    //Call the default initialization app
    default_app_on_init();
}

/**
 ****************************************************************************************
 * @brief Function called when we have stopped advertising. 
          Sets up the configured low-power mode.
 * @return void
 ****************************************************************************************
*/
void user_app_on_adv_nonconn_complete(uint8_t status)
{   
    //Check if advertising was stopped properly
    if (status == GAP_ERR_CANCELED)
    {
#ifdef CFG_HIBERNATION_MODE    
        //Put the system into hibernation mode 
        arch_set_hibernation(HIB_WAKE_UP_PIN_MASK,
                             PD_SYS_DOWN_RAM_ON,
                             PD_SYS_DOWN_RAM_ON,
                             PD_SYS_DOWN_RAM_ON,
                             REMAP_ADDR0_TO_RAM1,
                             false);
#endif

#ifdef CFG_DEEP_SLEEP_MODE
        // Ensure PD_TIM is open
        SetBits16(PMU_CTRL_REG, TIM_SLEEP, 0);
        // Wait until PD_TIM is opened
        while ((GetWord16(SYS_STAT_REG) & TIM_IS_UP) != TIM_IS_UP);
        
        // Set up the real-time clock to wake us up from sleep
        configure_rtc_wakeup();
        // Put system into deep sleep mode
        arch_set_deep_sleep(PD_SYS_DOWN_RAM_ON,
                            PD_SYS_DOWN_RAM_ON,
                            PD_SYS_DOWN_RAM_ON,
                            false);
#endif
    }
}

/// @} APP
