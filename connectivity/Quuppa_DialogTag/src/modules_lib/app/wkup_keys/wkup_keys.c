/**
 ****************************************************************************************
 *
 * @file wkup_keys.c
 *
 * @brief Wakeup keys project source code.
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
 
#ifdef HAS_WKUP_KEYS
#include "wkup_keys.h"
#include "wkupct_quadec.h"

#define WKUP_KEYS_DEBOUNCE_TIME_MS   (WKUP_KEYS_CFG_DEBOUNCE_TIME_MS)

typedef struct
{
   bool pressed;
   bool notif_pending;
} wkup_key_async_notif_t;

bool wkup_key_last_state[WKUP_KEYS_NUM]                  __SECTION_ZERO("retention_mem_area0");
wkup_key_async_notif_t wkup_key_notif[WKUP_KEYS_NUM]     __SECTION_ZERO("retention_mem_area0");
uint16_t wkup_key_pol_cfg[WKUP_KEYS_CHANNEL_NUM]     __SECTION_ZERO("retention_mem_area0");
uint16_t wkup_key_pin_cfg[WKUP_KEYS_CHANNEL_NUM]     __SECTION_ZERO("retention_mem_area0");  
uint16_t wkup_key_mask[WKUP_KEYS_CHANNEL_NUM]       __SECTION_ZERO("retention_mem_area0");

typedef void (*wkup_keys_notification_t)(enum wkup_keys key, bool pressed);
static const wkup_keys_notification_t wkup_keys_notification = WKUP_KEYS_NOTIFICATION_CB;

static void _wkup_interrupt_handler(uint8_t channel);
static void _wkup_keys_handler(void);
#if WKUP_KEYS_USE_BOTH_CHANNELS
static void _wkup_keys_handler_2(void);
#endif // WKUP_KEYS_USE_BOTH_CHANNELS
static bool _wkup_get_pin_state(uint8_t pin);
static void _wkup_keys_send_async_notification(enum wkup_keys key, bool pressed);
static void _wkup_keys_process_async_notification(void);
static void _wkup_keys_enable_irq(uint8_t channel);
static void _wkup_keys_handle_key_channel_event(uint8_t wkup_channel);

static void _wkup_keys_enable_channel_irq(uint8_t chan, uint16_t mask)
{
    wkup_key_mask[chan] |= mask;
}

static void _wkup_keys_disable_channel_irq(uint8_t chan, uint16_t mask)
{
    wkup_key_mask[chan] &= ~mask;
}


static void _wkup_interrupt_handler(uint8_t channel)
{
    wkup_keys_init(true);
    
    arch_ble_force_wakeup();
    
    _wkup_keys_handle_key_channel_event(channel);
    
    _wkup_keys_enable_irq(channel);
}

static void _wkup_keys_handler()
{
    _wkup_interrupt_handler(WKUP_KEYS_CHANNEL_1);
}

#if WKUP_KEYS_USE_BOTH_CHANNELS
static void _wkup_keys_handler_2()
{
    _wkup_interrupt_handler(WKUP_KEYS_CHANNEL_2);
}
#endif

static bool _wkup_keys_interrupts_enabled()
{
    return (GetBits16(WKUP_CTRL_REG, WKUP_ENABLE_IRQ)!=0);
}

#if WKUP_KEYS_USE_BOTH_CHANNELS
static bool _wkup_keys_interrupts_enabled_2()
{
    return (GetBits16(WKUP_CTRL_REG, WKUP2_ENABLE_IRQ)!=0);
}
#endif

static bool _wkup_get_pin_state(uint8_t i)
{
    return GPIO_GetPinStatus((GPIO_PORT)wkup_keys_pins[i].port, (GPIO_PIN)wkup_keys_pins[i].pin); 
}

static void _wkup_keys_process_async_notification()
{
    uint8_t i;
    for (i = 0; i < WKUP_KEYS_NUM; i++)
    {
        // Search for the first key that there is a notification pending
        if(wkup_key_notif[i].notif_pending) {
            
            // Set the pending flag to false
            wkup_key_notif[i].notif_pending = false;
            
            // And call the user callback for that key
            wkup_keys_notification((enum wkup_keys)i, wkup_key_notif[i].pressed);   
            return;
        }
    }
}

static void _wkup_keys_send_async_notification(enum wkup_keys key, bool pressed)
{
    ASSERT_WARNING((int)key <= WKUP_KEYS_NUM);

    // Set the notification pending to true
    wkup_key_notif[key].notif_pending = true;
    
    // Pass the current button status
    wkup_key_notif[key].pressed = pressed;
    
    // Create a message to process the event on normal context
    ke_msg_id_t msg = app_easy_msg_set(_wkup_keys_process_async_notification);
    ke_msg_send_basic(msg, TASK_APP, 0);
}


static void _wkup_keys_enable_irq(uint8_t channel)
{
    uint8_t i;
    uint16_t pol[2] = {0, 0};
    bool pressed;
    
    for(i = 0; i < WKUP_KEYS_NUM; i++) {
        if(wkup_keys_pins[i].channel == channel) {
            pressed = wkup_key_last_state[i];
            if( (wkup_keys_pins[i].high == 0) ? pressed : !pressed) {
                pol[wkup_keys_pins[i].channel] |= WKUPCT_PIN_SELECT(wkup_keys_pins[i].port, wkup_keys_pins[i].pin);
            }
        }
    }   
    
    if(channel == WKUP_KEYS_CHANNEL_1) {
        wkup_key_pin_cfg[WKUP_KEYS_CHANNEL_1] = wkup_key_mask[WKUP_KEYS_CHANNEL_1];
        wkup_key_pol_cfg[WKUP_KEYS_CHANNEL_1]  = (wkup_key_pol_cfg[WKUP_KEYS_CHANNEL_1] & ~wkup_key_mask[WKUP_KEYS_CHANNEL_1]) | (pol[WKUP_KEYS_CHANNEL_1] & wkup_key_mask[WKUP_KEYS_CHANNEL_1]);
        wkupct_enable_irq(wkup_key_pin_cfg[WKUP_KEYS_CHANNEL_1], wkup_key_pol_cfg[WKUP_KEYS_CHANNEL_1], 1, WKUP_KEYS_DEBOUNCE_TIME_MS);
        return;
    }
    
#if WKUP_KEYS_USE_BOTH_CHANNELS
    if (channel == WKUP_KEYS_CHANNEL_2) {
        wkup_key_pin_cfg[WKUP_KEYS_CHANNEL_2] = wkup_key_mask[WKUP_KEYS_CHANNEL_2];
        wkup_key_pol_cfg[WKUP_KEYS_CHANNEL_2]  = (wkup_key_pol_cfg[WKUP_KEYS_CHANNEL_2] & ~wkup_key_mask[WKUP_KEYS_CHANNEL_2]) | (pol[WKUP_KEYS_CHANNEL_2] & wkup_key_mask[WKUP_KEYS_CHANNEL_2]);
        wkupct2_enable_irq(wkup_key_pin_cfg[WKUP_KEYS_CHANNEL_2], wkup_key_pol_cfg[WKUP_KEYS_CHANNEL_2], 1, WKUP_KEYS_DEBOUNCE_TIME_MS);
        return;
    }
#endif
}


static void _wkup_keys_handle_key_channel_event(uint8_t wkup_channel)
{
    uint8_t current_state,i;
    
    for(i = 0; i < WKUP_KEYS_NUM; i++) {
        if(wkup_channel == wkup_keys_pins[i].channel) {
            current_state = _wkup_get_pin_state(i);       
            current_state = (wkup_keys_pins[i].high == 0)?(current_state):(!current_state);
            if(wkup_key_last_state[i] != current_state) {
                wkup_key_last_state[i] = current_state;
                _wkup_keys_send_async_notification((enum wkup_keys)i, current_state);
            }
        }
    }
    
    _wkup_keys_enable_irq(wkup_channel);
}

void wkup_keys_modify_irq(uint8_t key, bool enable)
{
    
    if(enable) {
        _wkup_keys_enable_channel_irq(wkup_keys_pins[key].channel, 
                        WKUPCT_PIN_SELECT(wkup_keys_pins[key].port, wkup_keys_pins[key].pin));
    }
    else {
        _wkup_keys_disable_channel_irq(wkup_keys_pins[key].channel, 
                        WKUPCT_PIN_SELECT(wkup_keys_pins[key].port, wkup_keys_pins[key].pin));
    }
    
    if(wkup_keys_pins[key].channel == WKUP_KEYS_CHANNEL_1 && _wkup_keys_interrupts_enabled()) {
        _wkup_keys_enable_irq(WKUP_KEYS_CHANNEL_1);
        return;
    }
#if WKUP_KEYS_USE_BOTH_CHANNELS
    if(wkup_keys_pins[key].channel == WKUP_KEYS_CHANNEL_2 && _wkup_keys_interrupts_enabled_2()) {
        _wkup_keys_enable_irq(WKUP_KEYS_CHANNEL_2);
        return;
    }
#endif
}

uint8_t wkup_keys_is_pressed(uint8_t key)
{
    return wkup_key_last_state[key];
}

bool wkup_keys_get_state(uint8_t key)
{   
    uint8_t status;
    
    status = _wkup_get_pin_state(key);
    
    wkup_key_last_state[key] = (wkup_keys_pins[key].high == 0) ? status : !status;
    return (bool)wkup_key_last_state[key];
}

void wkup_keys_init(bool init_pins_only)
{
    uint8_t i;

    if(!init_pins_only) {
        for(i = 0; i < WKUP_KEYS_NUM; i++) {
            wkup_key_last_state[i] =  _wkup_get_pin_state(i);
            if(wkup_keys_pins[i].high) {
                wkup_key_last_state[i] = !wkup_key_last_state[i];
            }
        }
        
        _wkup_keys_enable_irq(WKUP_KEYS_CHANNEL_1);
        
#if WKUP_KEYS_USE_BOTH_CHANNELS
        _wkup_keys_enable_irq(WKUP_KEYS_CHANNEL_2);
#endif // WKUP_KEYS_USE_BOTH_CHANNELS
    }
    
    for(i = 0; i < WKUP_KEYS_NUM; i++) {
        GPIO_SetPinFunction((GPIO_PORT)wkup_keys_pins[i].port, (GPIO_PIN)wkup_keys_pins[i].pin, 
            (GPIO_PUPD)(wkup_keys_pins[i].mode_function & 0xFF00),  (GPIO_FUNCTION)(wkup_keys_pins[i].mode_function & 0xFF));
    }
}

void wkup_keys_setup()
{
    uint8_t i;
    
    wkup_key_mask[WKUP_KEYS_CHANNEL_1] = WKUP_KEY_MASK_1;
    wkup_key_mask[WKUP_KEYS_CHANNEL_2] = WKUP_KEY_MASK_2;
    
    for(i = 0; i < WKUP_KEYS_CHANNEL_NUM; i++) {
        wkup_key_pin_cfg[i] = 0;
        wkup_key_pol_cfg[i] = 0;
    }
    
    wkupct_disable_irq();
    wkupct_register_callback(_wkup_keys_handler);
    
#if WKUP_KEYS_USE_BOTH_CHANNELS
    wkupct2_disable_irq();
    wkupct2_register_callback(_wkup_keys_handler_2);
#endif // WKUP_KEYS_USE_BOTH_CHANNELS
    
    wkup_keys_init(false);
}
#endif // HAS_WKUP_KEYS