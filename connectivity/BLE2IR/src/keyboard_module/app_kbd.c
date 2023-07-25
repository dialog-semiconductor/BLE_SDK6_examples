/**
 ****************************************************************************************
 *
 * \file app_kbd.c
 *
 * \brief Keyboard module source file
 *
 * Copyright (C) 2017-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifdef HAS_KBD

#include <string.h>
#include "app_kbd.h"
#include <port_kbd.h>
#include <app_kbd_config.h>

#define KEY_STATUS_MASK    (0x10)    // pressed or released
#define KEY_FN_SET_MASK    (0x0F)    // mask for fn modifier
#define KEY_FN_LOCK_MASK   (0x20)    // pressed or released

#define KBD_CALL_CALBACK(func, param)     if(app_kbd_scan_funcs.func!=NULL) app_kbd_scan_funcs.func(param)
#define KBD_CALL_CALBACK_RET(func, param) app_kbd_scan_funcs.func(param)
#define KBD_CALL_CALBACK_VOID(func)       if(app_kbd_scan_funcs.func!=NULL) app_kbd_scan_funcs.func()
#define KBD_CALL_CALBACK_VOID_RET(func)   app_kbd_scan_funcs.func()

struct keycode_buffer_tag {
        uint8_t flags;
        uint8_t output;
        uint8_t input;
};

enum REPORT_MODE {
        REPORTS_DISABLED,   // PassCode mode
        REPORTS_ENABLED,    // Normal mode
        REPORTS_PAUSED,     // Disconnected mode
};

enum REPORT_MODE kbd_reports_en                                       __PORT_RETAINED;   // flag to switch between key reporting and passcode entry modes
uint32_t passcode                                                     __PORT_RETAINED;   // used to store the passcode in passcode mode
struct keycode_buffer_tag kbd_keycode_buffer[KBD_KEYCODE_BUFFER_SIZE] __PORT_RETAINED;   // Buffer to hold the scan results for the key presses / releases
#if KBD_KEYCODE_BUFFER_SIZE < 256
    uint8_t kbd_keycode_buffer_head                                   __PORT_RETAINED;   // Read pointer for accessing the data of the keycode buffer
    uint8_t kbd_keycode_buffer_tail                                   __PORT_RETAINED;   // Write pointer for writing data to the keycode buffer
#else                                                              
    uint16_t kbd_keycode_buffer_head                                  __PORT_RETAINED;   // Read pointer for accessing the data of the keycode buffer
    uint16_t kbd_keycode_buffer_tail                                  __PORT_RETAINED;   // Write pointer for writing data to the keycode buffer
#endif                                                             
bool keycode_buf_overflow                                             __PORT_RETAINED;   // Flag to indicate that the key buffer is full!
bool fn_locked                                                        __PORT_RETAINED;
bool key_pressed_after_fn_lock                                        __PORT_RETAINED;
uint8_t kbd_fn_modifier                                               __PORT_RETAINED;   // Fn key has been pressed
                                                                   
#ifdef ASCII_DEBUG
    // Quick and dirty conversion table to allow debugging
    static const uint8_t kbd_hut_to_ascii_table[128] = 
    {
        '?', '?', '?', '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',  // 0x00
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2',  // 0x10
        '3', '4', '5', '6', '7', '8', '9', '0', '\n', 27,  8, '\t', ' ', '-', '=', '[',  // 0x20
        ']', '\\','#', ';', '\'','`', ',', '.', '/', '^', '1', '2', '3', '4', '5', '6',  // 0x30
        '7', '8', '9', 'A', 'B', 'C', 'P', 'S', 'P', 'I', 'H', 'U', 127, 'E', 'D', 'R',  // 0x40
        'L', 'D', 'U', 'N', '/', '*', '-', '+', '\n','1', '2', '3', '4', '5', '6', '7',  // 0x50
        '8', '9', '0', '.', '\\','A', 'P', '=', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',  // 0x60
        'L', 'M', 'N', 'O', 'E', 'H', 'M', 'S', 'S', 'A', 'U', 'C', 'C', 'P', 'F', 'M'   // 0x70
    };

    // note: must be power of 2
    #define MAX_KEYBUFFER (16)
    uint8_t kbd_buffer_ascii[MAX_KEYBUFFER];
    uint16_t kbd_keybuffer_idx;
#endif // ASCII_DEBUG
    
#ifdef MULTI_KEY_COMBINATIONS_ON
        uint8_t multi_key_pressed[sizeof(multi_key_combinations)/sizeof(struct multi_key_combinations_t)] __PORT_RETAINED;
#endif

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in] notification
 ****************************************************************************************
 */
static inline void app_kbd_send_notification(enum kbd_notification notification)
{
    if (kbd_params.notify_callback) {
        (*kbd_params.notify_callback)(notification);
    }
}

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in] row
 * \param[in] column
 * \param[in] pressed
 ****************************************************************************************
 */
static inline void app_kbd_send_key_detection(uint16_t row, uint16_t column, bool pressed)
{
    if (kbd_params.key_detect_callback) {
        (*kbd_params.key_detect_callback)(row, column, pressed);
    }
}

/**
 ****************************************************************************************
 * \brief Handles the initialization of all retained variables used for key scanning
 ****************************************************************************************
 */
static void kbd_init_retained_vars(void)
{
        kbd_keycode_buffer_head = 0;
        kbd_keycode_buffer_tail = 0;
        keycode_buf_overflow = false;
}

void app_kbd_init_vars(void)
{
//        kbd_fn_modifier = 0;

#ifdef ASCII_DEBUG
        kbd_keybuffer_idx = 0;
#endif    
}

void app_kbd_stop(void)
{
    if (KBD_CALL_CALBACK_VOID_RET(is_active)) {
        KBD_CALL_CALBACK_VOID(stop); 
        app_kbd_flush_buffer();     // reset buffer
    }    
}

/**
 ****************************************************************************************
 * \brief Processes multi-key combinations.
 *
 * \param[in] output
 * \param[in] input
 * \param[in] pressed
 *
 * \return true if combination has been found
 ****************************************************************************************
 */
static bool process_multi_key_combinations(const uint16_t output, const uint16_t input, const bool pressed)
{
        bool found = false;

#ifdef MULTI_KEY_COMBINATIONS_ON    
        int i, j;
        struct multi_key_combinations_t key_combination;

        for (i=0; i<sizeof(multi_key_combinations)/sizeof(struct multi_key_combinations_t); i++) {
                key_combination=multi_key_combinations[i];

                for (j=0; j<MULTI_KEY_NUM_OF_KEYS; j++) {
                        if (key_combination.key[j].output == output && key_combination.key[j].input == input) {
                                if (pressed) {
                                        multi_key_pressed[i] |= 1 << j;
                                }
                                else {
                                        multi_key_pressed[i] &= ~(1 << j);
                                }
                        }
                        else {
                                if (key_combination.key[j].output == MULTI_KEY_NOT_USED) {
                                        // This code could be omitted if the bits of multi_key_pressed
                                        // corresponding to the unused keys are set to "1" during initialization
                                        multi_key_pressed[i] |= 1 << j;
                                }
                        }
                }
                if (multi_key_pressed[i] == (1 << MULTI_KEY_NUM_OF_KEYS)-1) {
                        // if all keys of the combination are pressed
                        // send the proper custom key notification
                        app_kbd_send_notification(key_combination.notification);
                        found = true;
                }
        }
#endif
        return found;
}

bool app_kbd_record_key(const uint16_t output, const uint16_t input, const bool pressed)
{
        const scan_t imask = 1 << input;

        ASSERT_WARNING(input < KBD_NR_COLUMN_INPUTS);
        ASSERT_ERROR(NUM_OF_CUSTOM_KEYS_PLUS_ONE <= 16);


        // to check whether a key is valid or not we always check the default keymap (#0).
        // the assumption is that a key will definitely appear in the default
        // keymap and may appear in secondary keymaps as well.
        // if this is not sufficient for a specific application then additional
        // logic needs to be implemented.
        if (kbd_keymap[0][output][input] == 0) {
                return false;                       // this key does not exist in the default
        }                                    // keymap => it's a ghost!

        if (pressed) {
                // Deghosting
                //
                // Three distinct cases:
                // A.     |  |                         |  |
                //        |  |                         |  |
                //     ---x--+---                   ---x--o---
                //        |  |            ---->        |  |
                //     ---x--+---                   ---x--+---
                //        |  |                         |  |
                //        |  |                         |  |
                //
                // B.     |  |                         |  |
                //        |  |                         |  |
                //     ---x--x---                   ---x--x---
                //        |  |            ---->        |  |
                //     ---+--+---                   ---o--+---
                //        |  |                         |  |
                //        |  |                         |  |
                //
                // C.     |  |                         |  |
                //        |  |                         |  |
                //     ---x--+---                   ---x--+---
                //        |  |            ---->        |  |
                //     ---+--x---                   ---o--x---
                //        |  |                         |  |
                //        |  |                         |  |
                //
                // In all cases, the press of the 3rd key results in a "ghost" (the 4th key
                // of the square) because the three keys share both a row and a column.
                // Thus, the press of the key must be ignored until either of the other two
                // keys is released. This is called "2-key roll-over (2KRO)". Obviously,
                // the key press must be ignored ONLY if the 4th key is valid (there is a
                // valid entry in the kbd_keymap[]; if there isn't then there is no key at
                // all at this position and only the specific third key could have caused
                // this situation). Else, it should be reported normally.
                //

                int i, o;
                scan_t scandata;
                scan_t mask = 1;

                // also, skip this key if:
                // a. a "square of active keys" in formed in the newly scanned matrix that
                //    includes this key
                // b. the corresponding "square" in the last reported status contains only 1
                //    active key. this means that there's another new key in the row we're
                //    examining that creates this "ghost" status (implicit 'B' and 'C' cases)
                scandata = KBD_CALL_CALBACK_RET(get_new_scan_data, output) & ~imask;
                if (scandata != ~imask) {            // other columns are being driven in the row under examination
                        for (i = 0; i < KBD_NR_COLUMN_INPUTS; ++i, mask <<= 1) {
                                if (input == i) {
                                        continue;               // skip this column (it's the input under investigation)
                                }
                                if (!(scandata & mask)) { // column (input) found
                                        // check if any other newly scanned row reports a key pressed in any of the two columns { input - i }
                                        // if so, then a "square" is formed.
                                        for (o = 0; o < KBD_NR_ROW_OUTPUTS; ++o) {
                                                if (output == o)
                                                        continue;       // skip this row (it's the output under investigation)

                                                scan_t combined_mask = (mask | imask);
                                                scan_t new_scandata = ((~KBD_CALL_CALBACK_RET(get_new_scan_data, o)) & ((1 << KBD_NR_COLUMN_INPUTS) - 1));
                                                if ((new_scandata & combined_mask) != 0) { // if any is used => "square" of { (output, input) - (o, i) }
                                                        // the other "corner" is not detected because the scanning of the rows
                                                        // is done in series (one after the other). thus, one row may be scanned
                                                        // just before the key is released and the other just after. they are
                                                        // scanned during the same scan cycle but the first row does not
                                                        // indicate the current status of the matrix but the previous one!
                                                        int cnt = 0;

                                                        if (kbd_keymap[0][output][i]) {
                                                                cnt++;
                                                        }
                                                        if (kbd_keymap[0][o][input]) {
                                                                cnt++;
                                                        }
                                                        if (kbd_keymap[0][o][i]) {
                                                                cnt++;
                                                        }
                                                        if (cnt == 3) {
                                                                return false;
                                                        }
                                                }
                                        }
                                }
                        }
                }

                // scan all rows once more just in case an input is "missed" in the row under examination
                for (o = 0; o < KBD_NR_ROW_OUTPUTS; ++o) {
                        if (output == o) {
                                continue;                   // skip this row (it's the row under investigation)
                        }
                        scandata = KBD_CALL_CALBACK_RET(get_new_scan_data, o);
                        if (!(scandata & imask)) {    // this row has the same column (input) driven
                                // check if any other column has a key pressed in any of the two rows { output - o } (or both)
                                // if this is the case then a "square" is formed.
                                mask = 1;
                                for (i = 0; i < KBD_NR_COLUMN_INPUTS; ++i, mask <<= 1) {
                                        if (input == i) {
                                                continue;           // skip this column (it's the input under investigation)
                                        }
                                        scan_t d1 = KBD_CALL_CALBACK_RET(get_scan_data, output) & mask;
                                        scan_t d2 = scandata & mask;

                                        if (!d1 || !d2) {      // at least one row uses an extra column. a "square" is formed
                                                int cnt = 0;

                                                if (kbd_keymap[0][output][i]) {
                                                        cnt++;
                                                }
                                                if (kbd_keymap[0][o][input]) {
                                                        cnt++;
                                                }
                                                if (kbd_keymap[0][o][i]) {
                                                        cnt++;
                                                }
                                                if (cnt == 3) {
                                                        return false;
                                                }
                                        }
                                }
                        }
                }
        }

        // if no ghosting, then continue to buffer.
        bool block_key = false;

        if (process_multi_key_combinations(output, input, pressed) == true) {
                return true;
        }

        // Check for custom keys
        if ((kbd_keymap[kbd_fn_modifier][output][input] & 0xFFF0) == 0xF4F0) {
                app_kbd_send_notification(KBD_KEY_TO_NOTIFY_CODE((kbd_keymap[kbd_fn_modifier][output][input] & 0x0F), pressed));
                app_kbd_send_key_detection(output, input, pressed);
                if (app_kbd_passcode_mode() == false) {
                        // Custom keys are only logged into the buffer in passcode mode
                        return true;   
                }
        }

        if ((kbd_keymap[kbd_fn_modifier][output][input] >> 8) == 0xF8) { // 'Fn'
                uint8_t keychar = kbd_keymap[kbd_fn_modifier][output][input] & 0xFF;
                if (kbd_params.has_fn_lock) {
                        if (pressed) {
                                kbd_fn_modifier = (kbd_fn_modifier & (~keychar)) | keychar;
                                key_pressed_after_fn_lock = false;
                        }
                        else {
                                if (key_pressed_after_fn_lock == false) {
                                        fn_locked = !fn_locked;
                                        kbd_fn_modifier = (kbd_fn_modifier & (~keychar)) | (fn_locked ? keychar : 0);
                                }
                                else {
                                        fn_locked = false;
                                        kbd_fn_modifier = kbd_fn_modifier & (~keychar);
                                }
                                app_kbd_send_notification(fn_locked ? KBD_FN_LOCK_PRESSED : KBD_FN_LOCK_RELEASED);				
                                    
                        }
                }
                else {
                    kbd_fn_modifier = (kbd_fn_modifier & (~keychar)) | (pressed ? keychar : 0);
                }
                block_key = true;   // do not send event for 'Fn' press / release
                                    // advertising does not start by pressing 'Fn' since
                                    // we are waiting to see if a "Clear storage memory" command follows
        }

        if (!block_key) {
                if (kbd_params.has_fn_lock) {
                        key_pressed_after_fn_lock = true;
                }
        }

        // 'Fn' key is written to the buffer. If we are connected an HID full release needs to be sent!
        app_kbd_buffer_add_key(output, input, pressed);
        app_kbd_send_notification(KBD_KEY_ACTION);
        app_kbd_send_key_detection(output, input, pressed);
        return true;
}

void app_kbd_flush_buffer(void)
{
        kbd_keycode_buffer_tail = kbd_keycode_buffer_head;
}

bool app_kbd_buffer_add_key(const uint16_t output, const uint16_t input, const bool pressed)
{
        int next_tail;

        next_tail = (kbd_keycode_buffer_tail + 1) % KBD_KEYCODE_BUFFER_SIZE;
        if (next_tail != kbd_keycode_buffer_head) {
                kbd_keycode_buffer[kbd_keycode_buffer_tail].flags = (pressed ? KEY_STATUS_MASK : 0) | 
                                                                    (fn_locked ? KEY_FN_LOCK_MASK : 0) |
                                                                    (kbd_fn_modifier & KEY_FN_SET_MASK);
                kbd_keycode_buffer[kbd_keycode_buffer_tail].input = input;
                kbd_keycode_buffer[kbd_keycode_buffer_tail].output = output;
                kbd_keycode_buffer_tail = next_tail;
                return true;
        }
        else {
                keycode_buf_overflow = true;
                return false;
        }
}

bool app_kbd_buffer_has_data(void)
{
        return (kbd_keycode_buffer_head != kbd_keycode_buffer_tail);
}

#ifdef ASCII_DEBUG
/**
 ****************************************************************************************
 * \brief
 *
 * \param[in] hut
 *
 * \return
 ****************************************************************************************
 */
static inline uint8_t kbd_hut_to_ascii(const uint8_t hut)
{
	// roughly
	return (hut & 0x80) ? '?' : kbd_hut_to_ascii_table[hut];
}
#endif

/**
 ****************************************************************************************
 * \brief Process a keycode that has been placed into the keycode_buffer 
 *
 * \param[in] keycode_idx The keycode index
 *
 * \return    16 LSBs contain the keycode. 16 MSBs contain the following flags:
 *            KBD_KEY_FULL_RELEASE, KBD_CUNSUME_KEY, KBD_KEY_PRESSED_FLAG 
 ****************************************************************************************
 */
static uint32_t kbd_process_keycode(struct keycode_buffer_tag *keycode_idx)
{
        uint32_t ret = KBD_CUNSUME_KEY;
        uint8_t fn_mod = 0;
        const uint8_t pressed = keycode_idx->flags & KEY_STATUS_MASK;
        const uint8_t output = keycode_idx->output;
        const uint8_t input = keycode_idx->input;

        if (kbd_params.has_fn_lock && fn_locked) {
                fn_mod = ((uint8_t)kbd_keymap[1][output][input] != 0) ? 1 : 0;
        }
        else {
                fn_mod = keycode_idx->flags & KEY_FN_SET_MASK;
        }
        
        const uint16_t keycode = (uint16_t)kbd_keymap[fn_mod][output][input];
        const uint8_t keychar = keycode & 0xFF;

        if (keycode) {
                const uint8_t keymode = keycode >> 8;

#ifdef ASCII_DEBUG
                if (pressed) {
                        switch(keymode & 0xFE) {
                        case 0x00: // normal key
                                kbd_buffer_ascii[kbd_keybuffer_idx] = kbd_hut_to_ascii(keychar);
                                break;
                        default:// Other key
                                kbd_buffer_ascii[kbd_keybuffer_idx] = '~';
                                break;
                        }
                        arch_printf("[%c]\r\n", kbd_buffer_ascii[kbd_keybuffer_idx]);
                        kbd_keybuffer_idx = (kbd_keybuffer_idx + 1) & (MAX_KEYBUFFER-1);
                }
#endif
                switch (keymode & 0xFC) {
                case 0x00: // normal key
                case 0xFC: // modifier key
                        ret = keycode | (pressed ? KBD_KEY_PRESSED_FLAG : 0);
                        break;
                case 0xF8: // Fn Modifier
                        // When Fn key is pressed or released all key state must be reset
                        // On release, optionally, a reset of kbd_scandata and a full key scan can be done to
                        // re-detect any other keys that were pressed with the Fn key!

                        // add a "full release" report in the trm list.
                        ret = KBD_KEY_FULL_RELEASE;
                        break;
                case 0xF4: // Special function
                {
                        uint8_t byte = keychar >> 4;

                        if (byte != 0xF) {    // Ignore custom and unknown keys
                            ret = keycode | (pressed ? KBD_KEY_PRESSED_FLAG : 0);
                        }
                        break;
                }
                }

        }
        return ret;
}

/**
 ****************************************************************************************
 * \brief Processes the keycodes that have been written in the keycode_buffer
 *        and prepares the necessary HID reports. 
 *
 * \return    16 LSBs contain the keycode. 16 MSBs contain the following flags:
 *            KBD_KEY_FULL_RELEASE, KBD_CUNSUME_KEY, KBD_KEY_PRESSED_FLAG 
 ****************************************************************************************
 */
static uint32_t get_kbd_keycode(void)
{
        while(app_kbd_buffer_has_data() == true) {
                uint32_t ret = kbd_process_keycode(&kbd_keycode_buffer[kbd_keycode_buffer_head]);
                kbd_keycode_buffer_head = (kbd_keycode_buffer_head + 1) % KBD_KEYCODE_BUFFER_SIZE;
            
                if(ret != KBD_CUNSUME_KEY) {
                    return ret;
                }
        }

        return KBD_KEY_BUFFER_EMPTY;
}

void app_kbd_init(void)
{
        kbd_init_retained_vars();   // Initialize retained variables
        app_kbd_init_vars();        // Initialize non-retained variables

        kbd_reports_en = REPORTS_PAUSED;            // reporting mode is 'Disconnected' => Keys are buffered but no HID reports are generated

        KBD_CALL_CALBACK_VOID(init);        // Initialize keyboard scan engine
}

void app_kbd_start_reporting(void)
{
        // may be called multiple times
        dbg_puts(DBG_KBD_LVL, "KBD HID rep ON\r\n");

        kbd_reports_en = REPORTS_ENABLED;
}

void app_kbd_start_passcode(void)
{
        dbg_puts(DBG_KBD_LVL, "KBD HID rep OFF (Passcode)\r\n");

        passcode = 0;
        if(fn_locked == true) {
                fn_locked = false;
                kbd_fn_modifier = false;
                app_kbd_send_notification(KBD_FN_LOCK_RELEASED);
        }
        kbd_reports_en = REPORTS_DISABLED;
}

bool app_kbd_passcode_mode(void)
{
        return kbd_reports_en == REPORTS_DISABLED;
}

void app_kbd_stop_reporting(void)
{
        dbg_puts(DBG_KBD_LVL, "KBD HID rep OFF\r\n");

        passcode = 0;
        kbd_reports_en = REPORTS_PAUSED;
}

void app_kbd_prepare_passcode(void)
{
        if (kbd_reports_en != REPORTS_DISABLED) {
                return;
        }

        if (app_kbd_buffer_has_data()) {
                // passcode mode
                while (kbd_keycode_buffer_head != kbd_keycode_buffer_tail) {
                        int fn_mod = kbd_keycode_buffer[kbd_keycode_buffer_head].flags & KEY_FN_SET_MASK;
                        uint8_t pressed = kbd_keycode_buffer[kbd_keycode_buffer_head].flags & KEY_STATUS_MASK;
                        uint8_t output = kbd_keycode_buffer[kbd_keycode_buffer_head].output;
                        uint8_t input = kbd_keycode_buffer[kbd_keycode_buffer_head].input;
                        uint16_t keycode = kbd_keymap[fn_mod][output][input];
                        const char keymode = keycode >> 8;
                        const char keychar = keycode & 0xFF;

                        if (!pressed) {
                                if(keycode == kbd_params.passcode_enter_key || keycode == 0x0028 || keycode == 0x0058) {
                                        dbg_printf(DBG_KBD_LVL, "MITM code:%d\r\n", passcode);
                                        app_kbd_send_notification(KBD_PASSCODE_ENTERED);
                                        app_kbd_start_reporting(); // start sending notifications
                                }
                                else {
                                        if ((keymode & 0xFC) == 0x00) {  // normal key
                                                int num = -1;

                                                if (keychar >= 0x1E && keychar <= 0x27) {
                                                        num = keychar - 0x1D;
                                                }
                                                else {
                                                        if (keychar >= 0x59 && keychar <= 0x62) {
                                                                num = keychar - 0x58;
                                                        }
                                                }
                                                if (num == 10) {
                                                        num = 0;
                                                }
                                                if (num != -1) {
                                                        dbg_printf(DBG_KBD_LVL, "MITM %d digit pressed\r\n", num);
                                                        passcode *= 10;
                                                        passcode += num;
                                                }
                                        }
                                }
                        }
                        kbd_keycode_buffer_head = (kbd_keycode_buffer_head + 1) % KBD_KEYCODE_BUFFER_SIZE;
                }
        }
}

uint32_t app_kbd_get_key_code(void)
{   
        if (kbd_reports_en == REPORTS_PAUSED) {
                return KBD_KEY_BUFFER_EMPTY;
        }

    
        if (keycode_buf_overflow) {
                app_kbd_flush_buffer();     // reset buffer
                // clear flag
                keycode_buf_overflow = false;
                return KBD_KEY_FULL_RELEASE;
        }
        if (kbd_reports_en == REPORTS_ENABLED) {
                // normal mode
                return get_kbd_keycode();
        }

        return KBD_KEY_BUFFER_EMPTY;
}

uint32_t app_kbd_get_passcode(void)
{
        return passcode;
}

void app_kbd_init_scan_gpios(void)
{
    if (KBD_CALL_CALBACK_VOID_RET(is_idle)) {    
        KBD_CALL_CALBACK_VOID(init_scan_gpios);
    }
}

bool app_kbd_scan_is_idle(void)
{
    return KBD_CALL_CALBACK_VOID_RET(is_idle);
}

bool app_kbd_scan_is_active(void)
{
    return KBD_CALL_CALBACK_VOID_RET(is_active);
}

void app_kbd_scan_enable_delayed_scanning(bool press)
{
    KBD_CALL_CALBACK(enable_delayed_scanning, press);
}

bool app_kbd_on_ble_powered(void)
{
    if (kbd_params.delayed_wakeup) {
        ble_is_woken_up = false;
    }    
    return false;
}

uint8_t app_kbd_on_system_powered(void)
{    
    KBD_CALL_CALBACK_VOID(fsm_update);
    if (kbd_params.delayed_wakeup) {
        KBD_CALL_CALBACK_VOID(delayed_start_proc);
    }  

    return  app_kbd_buffer_has_data() ? APP_BLE_WAKEUP : APP_GOTO_SLEEP;
}

#endif // HAS_KBD

/**
 * \}
 * \}
 * \}
 */
