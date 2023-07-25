/**
 ****************************************************************************************
 *
 * \file port_platform.h
 *
 * \brief 
 *
 * Copyright (C) 2017-2021 Dialog Semiconductor
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */
 
/**
 ****************************************************************************************
 * \addtogroup APP_UTILS
 * \{
 * \addtogroup PLATFORM
 * \{
 * \addtogroup PORT_PLATFORM
 * \{
 ****************************************************************************************	 
 */ 

#ifndef PORT_PLATFORM_H_
#define PORT_PLATFORM_H_

#include "rwble_config.h"
#include "app.h"
#include "arch_api.h"
#include "co_bt.h"
#include "gpio.h"

#include "app_task.h"
#include "port_debug.h"

#include <stdint.h>          // standard integer definition

#include "reg_blecore.h"

#if (RWBLE_SW_VERSION_MAJOR >= 8)
    #include "da1458x_scatter_config.h"
    #define MAX_TX_BUFS (__SCT_BLE_TX_DESC_DATA)
#else
    #define MAX_TX_BUFS (18)
#endif

typedef ke_task_id_t task_id_t;

#define __PORT_RETAINED __attribute__((section(".bss.")))

#define BLE_TASK TASK_APP

#define BLE_ERROR_TIMEOUT  0x0A
#define BLE_STATUS_OK      0x00
#define BLE_ERROR_CANCELED 0x0C

#if (RWBLE_SW_VERSION_MAJOR < 8)   
    #define KBRD_IRQ_IN_SEL2_REG (0x50001416) // GPIO interrupt selection for KBRD_IRQ for P3
#endif    

enum GPIO_POLARITY {
    GPIO_ACTIVE_HIGH = 1,
    GPIO_ACTIVE_LOW  = 0
};

enum power_status {
    APP_GOTO_SLEEP   = 0,
    APP_KEEP_POWERED = 1,
    APP_BLE_WAKEUP   = 2,
};

typedef const struct pin_type_s {
    uint8_t port:2;
    uint8_t pin:4;
    uint8_t high:1;
    uint16_t mode_function; 
} pin_type_t;

typedef struct pin_configuration_s {
    pin_type_t *pins;
    uint8_t    num_of_pins;
} pin_configuration_t;

typedef struct module_config_s {
    void (*init_gpios)(void);
    void (*init)(void);
    void (*on_disconnect)(void);
    bool (*on_ble_powered)(void);
    uint8_t (*on_system_powered)(void);
    pin_configuration_t pins_config;
} module_config_t;

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]  pins
 *
 * \return
 ****************************************************************************************
 */
#define SET_PIN_CONFIGURATION(pins) {pins, sizeof(pins)/sizeof(pin_type_t)}

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]  pin_conf
 ****************************************************************************************
 */
#define PORT_RESERVE_GPIO(pin_conf) RESERVE_GPIO( 0, pin_conf.port, pin_conf.pin, 0)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]  pin_conf
 ****************************************************************************************
 */
#define PORT_CONFIGURE_GPIO_DEFAULT(pin_conf) GPIO_ConfigurePin((GPIO_PORT)pin_conf.port,                       \
                                                                (GPIO_PIN)pin_conf.pin,                         \
                                                                (GPIO_PUPD)(pin_conf.mode_function & 0xFF00),   \
                                                                (GPIO_FUNCTION)(pin_conf.mode_function & 0xFF), \
                                                                (bool)pin_conf.high)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]  pin_conf
 * \param[in]  mode
 * \param[in]  function
 * \param[in]  high
 ****************************************************************************************
 */
#define PORT_CONFIGURE_GPIO(pin_conf, mode, function, high) GPIO_ConfigurePin((GPIO_PORT)pin_conf.port,                       \
                                                                 (GPIO_PIN)pin_conf.pin, mode, function, high)
                                                            
/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]  pin_conf
 ****************************************************************************************
 */
#define PORT_SET_PIN_FUNCTION_DEFAULT(pin_conf) GPIO_SetPinFunction((GPIO_PORT)pin_conf.port,                     \
                                                                 (GPIO_PIN)pin_conf.pin,                      \
                                                                 (GPIO_PUPD)(pin_conf.mode_function & 0xFF00), \
                                                                 (GPIO_FUNCTION)(pin_conf.mode_function & 0xFF))

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]  pin_conf
 * \param[in]  mode
 * \param[in]  function
 ****************************************************************************************
 */
#define PORT_SET_PIN_FUNCTION(pin_conf, mode, function) GPIO_SetPinFunction((GPIO_PORT)pin_conf.port,                     \
                                                                 (GPIO_PIN)pin_conf.pin, mode, function)

/**
 ****************************************************************************************
 * \brief
 ****************************************************************************************
 */
void app_kbd_long_keypress_timer_callback(void);

 /**
  ****************************************************************************************
  * \brief
  *
  * \return
  ****************************************************************************************
  */
uint32_t port_get_time(void);

/**
 ****************************************************************************************
  * \brief
  *
  * \param[in]  delay (in us)
 ****************************************************************************************
 */
//void port_delay_usec(uint32_t delay);
            
#if (RWBLE_SW_VERSION_MAJOR >= 8)
/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   conidx
 * \param       start_hdl
 * \param[in]   end_hdl
 * \param[in]   uuid_len
 * \param       uuid
 * \param[in]   service
 *
 * \return
 ****************************************************************************************
 */
uint8_t port_attmdb_find_by_uuid(uint8_t conidx, uint16_t *start_hdl, uint16_t end_hdl, uint8_t uuid_len, uint8_t *uuid, bool service);
#endif

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   conidx
 * \param       start_hdl
 * \param[in]   end_hdl
 * \param[in]   uuid_len
 * \param       uuid
 *
 * \return
 ****************************************************************************************
 */
uint8_t port_atts_find_value_by_uuid(uint8_t conidx, uint16_t *start_hdl, uint16_t end_hdl, uint8_t uuid_len, uint8_t *uuid);

/**
 ****************************************************************************************
 * \brief  Get last BLE event
 * \return last BLE event
 ****************************************************************************************
 */
__INLINE last_ble_evt port_get_last_ble_event(void)
{
    extern last_ble_evt arch_rwble_last_event;
    return arch_rwble_last_event;   
}

/**
 ****************************************************************************************
 * \brief
 ****************************************************************************************
 */
__INLINE void port_force_active_mode(void)
{
    arch_force_active_mode();
}

/**
 ****************************************************************************************
 * \brief
 ****************************************************************************************
 */
__INLINE void port_restore_sleep_mode(void)
{
    arch_restore_sleep_mode();
}

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   port
 * \param[in]   pin
 *
 * \return
 ****************************************************************************************
 */
__INLINE bool port_gpio_get_pin_status(uint8_t port, uint8_t pin)
{
    return GPIO_GetPinStatus((GPIO_PORT)port, (GPIO_PIN)pin);
}

#ifdef DEBUG_EMULATE_PACKET_LOSS
/**
 ****************************************************************************************
 * \brief
 ****************************************************************************************
 */
void port_enable_radio (void);

/**
 ****************************************************************************************
 * \brief
 ****************************************************************************************
 */
void port_disable_radio (void);

/**
 ****************************************************************************************
 * \brief
 ****************************************************************************************
 */
void port_handle_radio_ctrl(void);
#endif

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   port
 * \param[in]   pin
 * \param[in]   high
 ****************************************************************************************
 */
__INLINE void port_gpio_set_pin_state(uint8_t port, uint8_t pin, bool high) 
{
    if(high) {
        GPIO_SetActive((GPIO_PORT)port, (GPIO_PIN)pin);    
    }        
    else {
        GPIO_SetInactive((GPIO_PORT)port, (GPIO_PIN)pin);       
    }
}

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   length
 * \param[in]   handle
 *
 * \return
 ****************************************************************************************
 */
struct l2cc_pdu_send_req *port_create_l2cc_pdu(uint16_t length, uint16_t handle);

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   handle
 * \param       data
 * \param[in]   length
 * \param[in]   low_priority
 *
 * \return
 ****************************************************************************************
 */
bool port_send_notification(uint16_t handle, uint8_t *data, uint16_t length, bool low_priority);

/**
 ****************************************************************************************
 * \brief Read a 16-bit integer from an unaligned address
 *
 * \param[in] addr Read address
 *
 * \return    The 16-bit integer read
 ****************************************************************************************
 */  
//uint16_t port_read16(const uint8_t *addr)
//{
//    ASSERT_ERROR(addr != NULL);
//    
//    uint16_t data;
//    data = *addr++;
//    data |= *addr << 8;
//    return data;
//}

/**
 ****************************************************************************************
 * \brief Write a 16-bit integer to an unaligned address
 *
 * \param[in] addr Write address
 * \param[in] data The 16-bit  integer to write
 ****************************************************************************************
 */  
//void port_write16(uint8_t *addr, uint16_t data)
//{
//    ASSERT_ERROR(addr != NULL);
//    
//    *addr++ = data & 0x00FF;
//    *addr = data >> 8;
//}

#endif // PORT_PLATFORM_H_

/**
 * \}
 * \}
 * \}
 */
