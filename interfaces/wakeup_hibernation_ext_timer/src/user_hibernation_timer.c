/**
 ****************************************************************************************
 *
 * @file user_hibernation_timer.c
 *
 * @brief BLE Eddystone beacon with hibernation or deep sleep project source code.
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
#include "arch_hibernation.h"
#include "co_math.h"
#include "battery.h"
#include "adc.h"
#include "arch_hibernation.h"

#include "user_periph_setup.h"
#include "spi_flash.h"

#ifdef CFG_DEEP_SLEEP_MODE
#include "rtc.h"
#endif

/*
 * DEFINITIONS
 ****************************************************************************************
 */
#define ADV_TIMER_CANCEL_TIMEOUT        1000                                        //In tens of milli-second (x10ms)

#ifdef CFG_HIBERNATION_MODE
#define DONE_TIMER_TIMEOUT              1                                           //In tens of milli-second (x10ms)
#endif

#ifdef CFG_DEEP_SLEEP_MODE
#define RTC_TIMEOUT                     ADV_TIMER_CANCEL_TIMEOUT / 100              //In seconds
#endif

#define ADV_DATA_BATTERY_OFFSET         10                                          //Byte offset of battery level in Advertising Data
#define ADV_DATA_TEMPER_OFFSET          12                                          //Byte offset of temperature level in Advertising Data
#define ADV_DATA_ADV_COUNT_OFFSET       14                                          //Byte offset of advertising counter in Advertising Data

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

// Retained variables
uint32_t wakeup_count                         __SECTION_ZERO("retention_mem_area_uninit"); //Wake-up counter
uint32_t adv_count                            __SECTION_ZERO("retention_mem_area_uninit"); //Advertising counter          

timer_hnd adv_timer                           __SECTION_ZERO("retention_mem_area_uninit"); //Advertising cancel event timer

uint16_t reset_stat_local                     __SECTION_ZERO("retention_mem_area_uninit"); //Status of RESET_STAT_REG on wake-up or power-on

#ifdef CFG_HIBERNATION_MODE
timer_hnd done_timer                          __SECTION_ZERO("retention_mem_area_uninit"); //Deassertion timer for the DONE signal of TPL5010
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


/**
 ****************************************************************************************
 * @brief Configures the real-time clock. The interrupt generated will wake us up from
 *        deep sleep
 * @return void
 ****************************************************************************************
*/

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
 * @brief Callback function to stop advertising
 * @return void
 ****************************************************************************************
 */
static void user_non_connectable_advertise_with_timeout_stop_cb(void)
{
    app_easy_gap_advertise_stop();
}

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
    adc_config_t cfg =
    {
        .input_mode = ADC_INPUT_MODE_SINGLE_ENDED,
        .input      = ADC_INPUT_SE_TEMP_SENS,
        .continuous = false
    };

    // Initialize and enable ADC
    adc_init(&cfg);
    
    next_temper = (uint32_t) adc_get_temp();

    stored_adv_data[ADV_DATA_TEMPER_OFFSET]     = next_temper;
    stored_adv_data[ADV_DATA_TEMPER_OFFSET + 1] = 0;
    
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
        //Initialize advertising and wake-up count to zero
        wakeup_count = 0;
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
        arch_set_deep_sleep(PD_SYS_DOWN_RAM_OFF,
                            PD_SYS_DOWN_RAM_OFF,
                            PD_SYS_DOWN_RAM_ON,
                            false);
#endif
    }
}

/**
 ****************************************************************************************
 * @brief Function called when BLE is powered. 
          Sets up the configured low-power mode.
 * @return GOTO_SLEEP
 * @note Return type allows the device to enter sleep.
 ****************************************************************************************
*/
arch_main_loop_callback_ret_t user_on_ble_powered(void)
{
    uint8_t last_ble_event;

    last_ble_event = arch_last_rwble_evt_get();

    if (last_ble_event  == BLE_EVT_END)
    {
        adv_count = adv_count/3;
        adv_count++;
        adv_count = 3*adv_count;
    }

    return GOTO_SLEEP;
}

/// @} APP