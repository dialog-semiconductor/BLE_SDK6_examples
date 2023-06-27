 /**
 ***************************************************************************************
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
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
#ifndef WKUP_KEYS_H
#define WKUP_KEYS_H

#define WKUP_KEY_ACTIVE_HIGH 1
#define WKUP_KEY_ACTIVE_LOW  0

#define WKUP_KEYS_CHANNEL_1     (0)
#define WKUP_KEYS_CHANNEL_2     (1)
#define WKUP_KEYS_CHANNEL_NUM   (2)

#include "wkup_keys_config.h"

#define PORT_UNUSED  0xFF
#define PIN_UNUSED   0xFF
#define CHANNEL_NONE 0xFF

#if (!defined(WKUP_KEY_0_PORT) || !defined(WKUP_KEY_0_PIN))
    #define WKUP_KEY_0_PORT     (PORT_UNUSED)
    #define WKUP_KEY_0_PIN      (PIN_UNUSED)
    #define WKUP_KEY_0_CHANNEL  (CHANNEL_NONE)
#endif

#if (!defined(WKUP_KEY_1_PORT) || !defined(WKUP_KEY_1_PIN))
    #define WKUP_KEY_1_PORT     (PORT_UNUSED)
    #define WKUP_KEY_1_PIN      (PIN_UNUSED)
    #define WKUP_KEY_1_CHANNEL  (CHANNEL_NONE)
#endif

#if (!defined(WKUP_KEY_2_PORT) || !defined(WKUP_KEY_2_PIN))
    #define WKUP_KEY_2_PORT     (PORT_UNUSED)
    #define WKUP_KEY_2_PIN      (PIN_UNUSED)
    #define WKUP_KEY_2_CHANNEL  (CHANNEL_NONE)
#endif

#if (!defined(WKUP_KEY_3_PORT) || !defined(WKUP_KEY_3_PIN))
    #define WKUP_KEY_3_PORT     (PORT_UNUSED)
    #define WKUP_KEY_3_PIN      (PIN_UNUSED)
    #define WKUP_KEY_3_CHANNEL  (CHANNEL_NONE)
#endif

#if (!defined(WKUP_KEY_4_PORT) || !defined(WKUP_KEY_4_PIN))
    #define WKUP_KEY_4_PORT     (PORT_UNUSED)
    #define WKUP_KEY_4_PIN      (PIN_UNUSED)
    #define WKUP_KEY_4_CHANNEL  (CHANNEL_NONE)
#endif

#if (!defined(WKUP_KEY_5_PORT) || !defined(WKUP_KEY_5_PIN))
    #define WKUP_KEY_5_PORT     (PORT_UNUSED)
    #define WKUP_KEY_5_PIN      (PIN_UNUSED)
    #define WKUP_KEY_5_CHANNEL  (CHANNEL_NONE)
#endif

#if (!defined(WKUP_KEY_6_PORT) || !defined(WKUP_KEY_6_PIN))
    #define WKUP_KEY_6_PORT     (PORT_UNUSED)
    #define WKUP_KEY_6_PIN      (PIN_UNUSED)
    #define WKUP_KEY_6_CHANNEL  (CHANNEL_NONE)
#endif

#if (!defined(WKUP_KEY_7_PORT) || !defined(WKUP_KEY_7_PIN))
    #define WKUP_KEY_7_PORT         (PORT_UNUSED)
    #define WKUP_KEY_7_PIN          (PIN_UNUSED)
    #define WKUP_KEY_7_CHANNEL      (CHANNEL_NONE)
#endif

#if (!defined(WKUP_KEY_8_PORT) || !defined(WKUP_KEY_8_PIN))
    #define WKUP_KEY_8_PORT         (PORT_UNUSED)
    #define WKUP_KEY_8_PIN          (PIN_UNUSED)
    #define WKUP_KEY_8_CHANNEL      (CHANNEL_NONE)
#endif

#if (!defined(WKUP_KEY_9_PORT) || !defined(WKUP_KEY_9_PIN))
    #define WKUP_KEY_9_PORT         (PORT_UNUSED)
    #define WKUP_KEY_9_PIN          (PIN_UNUSED)
    #define WKUP_KEY_9_CHANNEL      (CHANNEL_NONE)
#endif

#if (!defined(WKUP_KEY_10_PORT) || !defined(WKUP_KEY_10_PIN))
    #define WKUP_KEY_10_PORT        (PORT_UNUSED)
    #define WKUP_KEY_10_PIN         (PIN_UNUSED)
    #define WKUP_KEY_10_CHANNEL     (CHANNEL_NONE)
#endif

#if (!defined(WKUP_KEY_11_PORT) || !defined(WKUP_KEY_11_PIN))
    #define WKUP_KEY_11_PORT        (PORT_UNUSED)
    #define WKUP_KEY_11_PIN         (PIN_UNUSED)
    #define WKUP_KEY_11_CHANNEL     (CHANNEL_NONE)
#endif

#if !WKUP_KEYS_USE_BOTH_CHANNELS
#if (WKUP_KEY_0_CHANNEL==WKUP_KEYS_CHANNEL_2)
#error 'You have to define WKUP_KEYS_USE_BOTH_CHANNELS macro to 1 in order to use both wakup channels'
#endif

#if (WKUP_KEY_1_CHANNEL==WKUP_KEYS_CHANNEL_2)
#error 'You have to define WKUP_KEYS_USE_BOTH_CHANNELS macro to 1 in order to use both wakup channels'
#endif

#if (WKUP_KEY_2_CHANNEL==WKUP_KEYS_CHANNEL_2)
#error 'You have to define WKUP_KEYS_USE_BOTH_CHANNELS macro to 1 in order to use both wakup channels'
#endif

#if (WKUP_KEY_3_CHANNEL==WKUP_KEYS_CHANNEL_2)
#error 'You have to define WKUP_KEYS_USE_BOTH_CHANNELS macro to 1 in order to use both wakup channels'
#endif

#if (WKUP_KEY_4_CHANNEL==WKUP_KEYS_CHANNEL_2)
#error 'You have to define WKUP_KEYS_USE_BOTH_CHANNELS macro to 1 in order to use both wakup channels'
#endif

#if (WKUP_KEY_5_CHANNEL==WKUP_KEYS_CHANNEL_2)
#error 'You have to define WKUP_KEYS_USE_BOTH_CHANNELS macro to 1 in order to use both wakup channels'
#endif

#if (WKUP_KEY_6_CHANNEL==WKUP_KEYS_CHANNEL_2)
#error 'You have to define WKUP_KEYS_USE_BOTH_CHANNELS macro to 1 in order to use both wakup channels'
#endif

#if (WKUP_KEY_7_CHANNEL==WKUP_KEYS_CHANNEL_2)
#error 'You have to define WKUP_KEYS_USE_BOTH_CHANNELS macro to 1 in order to use both wakup channels'
#endif

#if (WKUP_KEY_8_CHANNEL==WKUP_KEYS_CHANNEL_2)
#error 'You have to define WKUP_KEYS_USE_BOTH_CHANNELS macro to 1 in order to use both wakup channels'
#endif

#if (WKUP_KEY_9_CHANNEL==WKUP_KEYS_CHANNEL_2)
#error 'You have to define WKUP_KEYS_USE_BOTH_CHANNELS macro to 1 in order to use both wakup channels'
#endif

#if (WKUP_KEY_10_CHANNEL==WKUP_KEYS_CHANNEL_2)
#error 'You have to define WKUP_KEYS_USE_BOTH_CHANNELS macro to 1 in order to use both wakup channels'
#endif

#if (WKUP_KEY_11_CHANNEL==WKUP_KEYS_CHANNEL_2)
#error 'You have to define WKUP_KEYS_USE_BOTH_CHANNELS macro to 1 in order to use both wakup channels'
#endif
#endif

#define SET_WKUP_MASK_FROM_GPIO(c, m, x) (((WKUP_KEY_##x##_PORT == m) && (WKUP_KEY_##x##_CHANNEL == c))?(1UL << WKUP_KEY_##x##_PIN):0)

// Masks for the initialization of the WKUP controller
#define WKUP_KEY_MASK_1    (  SET_WKUP_MASK_FROM_GPIO(0, 0, 0)  | SET_WKUP_MASK_FROM_GPIO(0, 0, 1)  | SET_WKUP_MASK_FROM_GPIO(0, 0, 2)  | SET_WKUP_MASK_FROM_GPIO(0, 0, 3)    \
                              | SET_WKUP_MASK_FROM_GPIO(0, 0, 4)  | SET_WKUP_MASK_FROM_GPIO(0, 0, 5)  | SET_WKUP_MASK_FROM_GPIO(0, 0, 6)  | SET_WKUP_MASK_FROM_GPIO(0, 0, 7)  \
                              | SET_WKUP_MASK_FROM_GPIO(0, 0, 8)  |  SET_WKUP_MASK_FROM_GPIO(0, 0, 9)  | SET_WKUP_MASK_FROM_GPIO(0, 0, 10) | SET_WKUP_MASK_FROM_GPIO(0, 0, 11))
                              
// Masks for the initialization of the WKUP controller
#define WKUP_KEY_MASK_2    (  SET_WKUP_MASK_FROM_GPIO(1, 0, 0)  | SET_WKUP_MASK_FROM_GPIO(1, 0, 1)  | SET_WKUP_MASK_FROM_GPIO(1, 0, 2)  | SET_WKUP_MASK_FROM_GPIO(1, 0, 3)    \
                              | SET_WKUP_MASK_FROM_GPIO(1, 0, 4)  | SET_WKUP_MASK_FROM_GPIO(1, 0, 5)  | SET_WKUP_MASK_FROM_GPIO(1, 0, 6)  | SET_WKUP_MASK_FROM_GPIO(1, 0, 7) \
                              | SET_WKUP_MASK_FROM_GPIO(1, 0, 8)  |  SET_WKUP_MASK_FROM_GPIO(1, 0, 9)  | SET_WKUP_MASK_FROM_GPIO(1, 0, 10) | SET_WKUP_MASK_FROM_GPIO(1, 0, 11))   

typedef struct {
    uint8_t channel:2;
    uint8_t port:1;
    uint8_t pin:4;
    uint8_t high:1;
    uint16_t mode_function; 
} wkup_key_pin_type_t;

static const wkup_key_pin_type_t wkup_keys_pins[WKUP_KEYS_NUM] = {
#if (WKUP_KEYS_NUM)
#ifdef WKUP_KEY_0_MODE
    {.channel = WKUP_KEY_0_CHANNEL,  .port = WKUP_KEY_0_PORT, .pin = WKUP_KEY_0_PIN, .high = (WKUP_KEY_0_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = WKUP_KEY_0_MODE | PID_GPIO },
#else    
    {.channel = WKUP_KEY_0_CHANNEL,  .port = WKUP_KEY_0_PORT, .pin = WKUP_KEY_0_PIN, .high = (WKUP_KEY_0_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = ((WKUP_KEY_0_POLARITY == WKUP_KEY_ACTIVE_LOW) ?  INPUT_PULLUP : INPUT_PULLDOWN) | PID_GPIO },
#endif
#endif        
#if (WKUP_KEYS_NUM>1)
#ifdef WKUP_KEY_1_MODE
    {.channel = WKUP_KEY_1_CHANNEL,  .port = WKUP_KEY_1_PORT, .pin = WKUP_KEY_1_PIN, .high = (WKUP_KEY_1_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = WKUP_KEY_1_MODE | PID_GPIO },
#else    
    {.channel = WKUP_KEY_1_CHANNEL,  .port = WKUP_KEY_1_PORT, .pin = WKUP_KEY_1_PIN, .high = (WKUP_KEY_1_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = ((WKUP_KEY_1_POLARITY == WKUP_KEY_ACTIVE_LOW) ?  INPUT_PULLUP : INPUT_PULLDOWN) | PID_GPIO },
#endif
#endif        
#if (WKUP_KEYS_NUM>2)
#ifdef WKUP_KEY_2_MODE
    {.channel = WKUP_KEY_2_CHANNEL,  .port = WKUP_KEY_2_PORT, .pin = WKUP_KEY_2_PIN, .high = (WKUP_KEY_2_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = WKUP_KEY_2_MODE | PID_GPIO },
#else    
    {.channel = WKUP_KEY_2_CHANNEL,  .port = WKUP_KEY_2_PORT, .pin = WKUP_KEY_2_PIN, .high = (WKUP_KEY_2_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = ((WKUP_KEY_2_POLARITY == WKUP_KEY_ACTIVE_LOW) ?  INPUT_PULLUP : INPUT_PULLDOWN) | PID_GPIO },
#endif
#endif        
#if (WKUP_KEYS_NUM>3)
#ifdef WKUP_KEY_3_MODE
    {.channel = WKUP_KEY_3_CHANNEL,  .port = WKUP_KEY_3_PORT, .pin = WKUP_KEY_3_PIN, .high = (WKUP_KEY_3_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = WKUP_KEY_3_MODE | PID_GPIO },
#else        
    {.channel = WKUP_KEY_3_CHANNEL,  .port = WKUP_KEY_3_PORT, .pin = WKUP_KEY_3_PIN, .high = (WKUP_KEY_3_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = ((WKUP_KEY_3_POLARITY == WKUP_KEY_ACTIVE_LOW) ?  INPUT_PULLUP : INPUT_PULLDOWN) | PID_GPIO },
#endif 
#endif        
#if (WKUP_KEYS_NUM>4)                                                                   
#ifdef WKUP_KEY_4_MODE
    {.channel = WKUP_KEY_4_CHANNEL,  .port = WKUP_KEY_4_PORT, .pin = WKUP_KEY_4_PIN, .high = (WKUP_KEY_4_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = WKUP_KEY_4_MODE | PID_GPIO },
#else
    {.channel = WKUP_KEY_4_CHANNEL,  .port = WKUP_KEY_4_PORT, .pin = WKUP_KEY_4_PIN, .high = (WKUP_KEY_4_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = ((WKUP_KEY_4_POLARITY == WKUP_KEY_ACTIVE_LOW) ?  INPUT_PULLUP : INPUT_PULLDOWN) | PID_GPIO },
#endif                                                                                     
#endif
#if (WKUP_KEYS_NUM>5)                                                                   
#ifdef WKUP_KEY_5_MODE
    {.channel = WKUP_KEY_5_CHANNEL,  .port = WKUP_KEY_5_PORT, .pin = WKUP_KEY_5_PIN, .high = (WKUP_KEY_5_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = WKUP_KEY_5_MODE | PID_GPIO },
#else
    {.channel = WKUP_KEY_5_CHANNEL,  .port = WKUP_KEY_5_PORT, .pin = WKUP_KEY_5_PIN, .high = (WKUP_KEY_5_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = ((WKUP_KEY_5_POLARITY == WKUP_KEY_ACTIVE_LOW) ?  INPUT_PULLUP : INPUT_PULLDOWN) | PID_GPIO },
#endif
#endif                                                                                     
#if (WKUP_KEYS_NUM>6)                                                                   
#ifdef WKUP_KEY_6_MODE
    {.channel = WKUP_KEY_6_CHANNEL,  .port = WKUP_KEY_6_PORT, .pin = WKUP_KEY_6_PIN, .high = (WKUP_KEY_6_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = WKUP_KEY_6_MODE | PID_GPIO },
#else
    {.channel = WKUP_KEY_6_CHANNEL,  .port = WKUP_KEY_6_PORT, .pin = WKUP_KEY_6_PIN, .high = (WKUP_KEY_6_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = ((WKUP_KEY_6_POLARITY == WKUP_KEY_ACTIVE_LOW) ?  INPUT_PULLUP : INPUT_PULLDOWN) | PID_GPIO },
#endif
#endif                                                                                     
#if (WKUP_KEYS_NUM>7)                                                                   
#ifdef WKUP_KEY_7_MODE
    {.channel = WKUP_KEY_7_CHANNEL,  .port = WKUP_KEY_7_PORT, .pin = WKUP_KEY_7_PIN, .high = (WKUP_KEY_7_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = WKUP_KEY_7_MODE | PID_GPIO },
#else
    {.channel = WKUP_KEY_7_CHANNEL,  .port = WKUP_KEY_7_PORT, .pin = WKUP_KEY_7_PIN, .high = (WKUP_KEY_7_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = ((WKUP_KEY_7_POLARITY == WKUP_KEY_ACTIVE_LOW) ?  INPUT_PULLUP : INPUT_PULLDOWN) | PID_GPIO },
#endif
#endif
#if (WKUP_KEYS_NUM>8)                                                                   
#ifdef WKUP_KEY_8_MODE
    {.channel = WKUP_KEY_8_CHANNEL,  .port = WKUP_KEY_8_PORT, .pin = WKUP_KEY_8_PIN, .high = (WKUP_KEY_8_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = WKUP_KEY_8_MODE | PID_GPIO },
#else
    {.channel = WKUP_KEY_8_CHANNEL,  .port = WKUP_KEY_8_PORT, .pin = WKUP_KEY_8_PIN, .high = (WKUP_KEY_8_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = ((WKUP_KEY_8_POLARITY == WKUP_KEY_ACTIVE_LOW) ?  INPUT_PULLUP : INPUT_PULLDOWN) | PID_GPIO },
#endif
#endif
#if (WKUP_KEYS_NUM>9)                                                                   
#ifdef WKUP_KEY_9_MODE
    {.channel = WKUP_KEY_9_CHANNEL,  .port = WKUP_KEY_9_PORT, .pin = WKUP_KEY_9_PIN, .high = (WKUP_KEY_9_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = WKUP_KEY_9_MODE | PID_GPIO },
#else
    {.channel = WKUP_KEY_9_CHANNEL,  .port = WKUP_KEY_9_PORT, .pin = WKUP_KEY_9_PIN, .high = (WKUP_KEY_9_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = ((WKUP_KEY_9_POLARITY == WKUP_KEY_ACTIVE_LOW) ?  INPUT_PULLUP : INPUT_PULLDOWN) | PID_GPIO },
#endif
#endif
#if (WKUP_KEYS_NUM>10)                                                                   
#ifdef WKUP_KEY_10_MODE
    {.channel = WKUP_KEY_10_CHANNEL,  .port = WKUP_KEY_10_PORT, .pin = WKUP_KEY_10_PIN, .high = (WKUP_KEY_10_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = WKUP_KEY_10_MODE | PID_GPIO },
#else
    {.channel = WKUP_KEY_10_CHANNEL,  .port = WKUP_KEY_10_PORT, .pin = WKUP_KEY_10_PIN, .high = (WKUP_KEY_10_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = ((WKUP_KEY_10_POLARITY == WKUP_KEY_ACTIVE_LOW) ?  INPUT_PULLUP : INPUT_PULLDOWN) | PID_GPIO },
#endif
#endif
#if (WKUP_KEYS_NUM>11)                                                                   
#ifdef WKUP_KEY_11_MODE
    {.channel = WKUP_KEY_11_CHANNEL,  .port = WKUP_KEY_11_PORT, .pin = WKUP_KEY_11_PIN, .high = (WKUP_KEY_11_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = WKUP_KEY_11_MODE | PID_GPIO },
#else
    {.channel = WKUP_KEY_11_CHANNEL,  .port = WKUP_KEY_11_PORT, .pin = WKUP_KEY_11_PIN, .high = (WKUP_KEY_11_POLARITY == WKUP_KEY_ACTIVE_LOW), .mode_function = ((WKUP_KEY_11_POLARITY == WKUP_KEY_ACTIVE_LOW) ?  INPUT_PULLUP : INPUT_PULLDOWN) | PID_GPIO },
#endif
#endif    
};

void wkup_keys_modify_irq(uint8_t key, bool enable);
uint8_t wkup_keys_is_pressed(uint8_t key);
bool wkup_keys_get_state(uint8_t key);
void wkup_keys_init(bool init_pins_only);
void wkup_keys_setup(void);
#endif // 