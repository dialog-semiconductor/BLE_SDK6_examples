/**
 ****************************************************************************************
 *
 * \file app_kbd.h
 *
 * \brief This provides a set of functions to handle and process events coming from a 
 * keypad matrix. Also function for detecting and avoiding key ghosting as usually 
 * happens when implementing a keypad scanner.
 *
 * Define symbol HAS_KBD to include this module in the application.
 *
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

#ifndef APP_KBD_H_
#define APP_KBD_H_

#ifdef HAS_KBD

#include <port_platform.h>

#include <stdint.h>          // standard integer definition
#include <co_bt.h>
#include "att.h"
#include "gap.h"

#include <app_kbd_scan_matrix.h>
#include <app_kbd_matrix.h>

#if KBD_NR_COLUMN_INPUTS < 9
        typedef uint8_t scan_t;
#elif KBD_NR_COLUMN_INPUTS < 17
        typedef uint16_t scan_t;
#else
        typedef uint32_t scan_t;
#endif

#define KBD_KEY_BUFFER_EMPTY 0xFFFF0000
#define KBD_KEY_FULL_RELEASE 0x00100000
#define KBD_CUNSUME_KEY      0x00200000
#define KBD_KEY_PRESSED_FLAG 0x00010000

/**
 ****************************************************************************************
 * \brief Initialize keycode buffer and keyreport variables
 ****************************************************************************************
 */        
void app_kbd_init(void);

/**
 ****************************************************************************************
 * \brief Activate key scanning hardware and state machine. 
 ****************************************************************************************
 */
void app_kbd_init_vars(void);

/**
 ****************************************************************************************
 * \brief Stop key scanning
 ****************************************************************************************
 */
void app_kbd_stop(void);

/**
 ****************************************************************************************
 * \brief Do deghosting for the given key. If everything is in order, adds the
 *        corresponding keycode in the keycode_buffer.
 *
 * \param[in] output
 * \param[in] input
 * \param[in] pressed
 *
 * \return false, if the key is ignored due to ghosting
 *         true, if the keycode has been accepted (passed debouncing, deghosting).
 *
 * If the key code buffer is not full, the keycode is added else
 * the flag keycode_buf_overflow is raised.
 ****************************************************************************************
 */
bool app_kbd_record_key(const uint16_t output, const uint16_t input, bool pressed);

/**
 ****************************************************************************************
 * \brief Add a key in the keycode_buffer
 *
 * \return  true if key has been successfully added.
 *          false if buffer is full.
 ****************************************************************************************
 */
bool app_kbd_buffer_add_key(const uint16_t output, const uint16_t input, const bool pressed);

/**
 ****************************************************************************************
 * \brief Check if there is data available in the keycode_buffer
 *
 * \return  false, if buffer is empty
 *          true, if data exists in the buffer
 ****************************************************************************************
 */
bool app_kbd_buffer_has_data(void);

/**
 ****************************************************************************************
 * \brief Activate reporting mode 
 ****************************************************************************************
 */
void app_kbd_start_reporting(void);

/**
 ****************************************************************************************
 * \brief Stop HID reporting
 ****************************************************************************************
 */
void app_kbd_stop_reporting(void);

/**
 ****************************************************************************************
 * \brief Activate passcode mode (default)
 ****************************************************************************************
 */
void app_kbd_start_passcode(void);

/**
 ****************************************************************************************
 * \brief Check passcode mode
 *
 * \return  true if keyboard is in passcode mode
 ****************************************************************************************
 */
bool app_kbd_passcode_mode(void);

/**
 ****************************************************************************************
 * \brief Get the passcode entred in the keyboard
 *
 * \return  The passcode in integer format
 ****************************************************************************************
 */
uint32_t app_kbd_get_passcode(void);

/**
 ****************************************************************************************
 * \brief Flush all the entries in the keycode_buffer
 ****************************************************************************************
 */
void app_kbd_flush_buffer(void);

/**
 ****************************************************************************************
 * \brief Process key pressed to geerate the passcode
 ****************************************************************************************
 */
void app_kbd_prepare_passcode(void);

/**
 ****************************************************************************************
 * \brief Get the key code from the kbd_buffer
 *
 * \return  16 LSBs contain the keycode. 16 MSBs contain the following flags:
 *          KBD_KEY_FULL_RELEASE, KBD_CUNSUME_KEY, KBD_KEY_PRESSED_FLAG 
 *          Return value is KBD_KEY_BUFFER_EMPTY when buffer is empty.
 ****************************************************************************************
 */
uint32_t app_kbd_get_key_code(void);

/**
 ****************************************************************************************
 * \brief Add a full release report to the report list
 *
 * \return  true if a full release report has been added to the report list successfully
 ****************************************************************************************
 */
bool app_kbd_add_full_release_reports(void);

/**
 ****************************************************************************************
 * \brief Re-initialize the scan matrix GPIOs after wakeup
 ****************************************************************************************
 */
void app_kbd_init_scan_gpios(void);

/**
 ****************************************************************************************
 * \brief Check if keyboard scanning is idle
 *
 * \return  true if keyboard scanning is idle
 ****************************************************************************************
 */
bool app_kbd_scan_is_idle(void);

/**
 ****************************************************************************************
 * \brief Check if keyboard scanning is active
 *
 * \return  true if keyboard scanning is active
 ****************************************************************************************
 */
bool app_kbd_scan_is_active(void);

/**
 ****************************************************************************************
 * \brief Enable the delayed scanning
 *
 * \param[in]   press
 ****************************************************************************************
 */
void app_kbd_scan_enable_delayed_scanning(bool press);

/**
 ****************************************************************************************
 * \brief   Process tasks while BLE is powered
 *
 * \return  true device sleep must be blocked
 ****************************************************************************************
 */
bool app_kbd_on_ble_powered(void);

/**
 ****************************************************************************************
 * \brief Process tasks while system is powered
 *
 * \return  APP_GOTO_SLEEP if the system can go to sleep
 *          APP_KEEP_POWERED if the system must remain active
 *          APP_BLE_WAKEUP if the BLE must be woken up
 ****************************************************************************************
 */
uint8_t app_kbd_on_system_powered(void);

#endif // HAS_KBD

#endif // APP_KBD_H_

/**
 * \}
 * \}
 * \}
 */