/**
 ****************************************************************************************
 *
 * \file app_kbd_defs.h
 *
 * \brief Keyboard module definitions
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
 * \addtogroup APP_UTILS
 * \{
 * \addtogroup KEYBOARD
 * \{
 * \addtogroup APP_KEYBOARD
 * \brief Keyboard (HID) Application
 *
 * \{
 ****************************************************************************************
 */
 
#ifndef APP_KBD_DEFS_H_
#define APP_KBD_DEFS_H_

#ifdef HAS_KBD

#include <stdint.h>          // standard integer definition

#define KBD_NOTIFY_CODE_KEY_PRESSED_MASK 0x20
#define KBD_KEY_TO_NOTIFY_CODE(key, pressed) ((enum kbd_notification)(key | (pressed ? KBD_NOTIFY_CODE_KEY_PRESSED_MASK : 0)))
#define KBD_NOTIFY_CODE_TO_KEY(code) (code & ((KBD_NOTIFY_CODE_KEY_PRESSED_MASK<<1)-1))

// unknown key code - nothing is sent to the other side but the key is examined for ghosting
#define K_CODE      (0xF4FF)

#define KEY_UNUSED  (0x0000)

// if output is set to MULTI_KEY_NOT_USED then the key is ignored
#define MULTI_KEY_NOT_USED      0xFF    

#define KBD_CUSTOM_KEY(x)   (0xF4F0 | x)
#define KBD_FN_LOCK_KEY(x)  (0xF800 | x)
#define KBD_SPECIAL_KEY(x)  (0xF400 | x)
#define KBD_MODIFIER_KEY(x) (0xFC00 | x)

struct multi_key_t {
    uint8_t output;
    uint8_t input;
};
 
enum kbd_notification {
        // 0 to 2*KBD_NOTIFY_CODE_KEY_PRESSED_MASK-1 reserver for custom keys
        KBD_KEY_ACTION = 2 * KBD_NOTIFY_CODE_KEY_PRESSED_MASK,
        KBD_PASSCODE_ENTERED,
        KBD_FN_LOCK_PRESSED,
        KBD_FN_LOCK_RELEASED,
};

typedef void (app_kbd_send_key_notification_t)(enum kbd_notification notification);
typedef void (app_kbd_key_detect_cb_t)(uint16_t row, uint16_t column, bool press);

typedef struct {
    bool scan_always_active;
    bool delayed_wakeup;
    bool has_fn_lock;
    uint16_t passcode_enter_key;
    app_kbd_send_key_notification_t *notify_callback;
    app_kbd_key_detect_cb_t *key_detect_callback;
    uint32_t row_scan_time_in_us;
    uint32_t full_scan_cycle_in_us;
    uint32_t partial_scan_cycle_in_us;
    uint32_t press_debounce_counter_in_us;
    uint32_t release_debounce_counter_in_us;
    uint32_t delayed_wakeup_time_in_us;
} kbd_params_t;

typedef void (*app_kbd_scan_init_t)(void);
typedef void (*app_kbd_scan_stop_t)(void);
typedef bool (*app_kbd_scan_is_idle_t)(void);                                   
typedef bool (*app_kbd_scan_is_active_t)(void);
typedef void (*app_kbd_init_scan_gpios_t)(void);
typedef uint16_t (*app_kbd_scan_get_new_scan_data_t)(uint8_t output);
typedef uint16_t (*app_kbd_scan_get_scan_data_t)(uint8_t output);
typedef void (*app_kbd_scan_fsm_update_t)(void);
typedef void (*app_kbd_scan_enable_delayed_scanning_t)(bool press);
typedef void (*app_kbd_scan_delayed_start_proc_t)(void);

typedef struct {
    app_kbd_scan_init_t                     init;
    app_kbd_scan_stop_t                     stop;
    app_kbd_scan_is_idle_t                  is_idle;
    app_kbd_scan_is_active_t                is_active;
    app_kbd_init_scan_gpios_t               init_scan_gpios;
    app_kbd_scan_get_new_scan_data_t        get_new_scan_data;
    app_kbd_scan_get_scan_data_t            get_scan_data;
    app_kbd_scan_fsm_update_t               fsm_update;
    app_kbd_scan_enable_delayed_scanning_t  enable_delayed_scanning;
    app_kbd_scan_delayed_start_proc_t       delayed_start_proc;
} kbd_scan_util_funcs_t;

#endif // HAS_KBD

#endif // APP_KBD_DEFS_H_

/**
 * \}
 * \}
 * \}
 */