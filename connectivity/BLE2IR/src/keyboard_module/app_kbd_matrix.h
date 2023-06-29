/**
 ****************************************************************************************
 *
 * \file app_kbd_matrix.h
 *
 * \brief HID Keyboard matrix.
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

#ifndef _APP_KBD_MATRIX_H_
#define _APP_KBD_MATRIX_H_

/**
 ****************************************************************************************
 * \addtogroup CONFIGURATION
 * \{
 * \addtogroup MODULE_CONFIG
 * \{
 * \addtogroup KEYBOARD_MATRIX_CFG
 *
 * \brief Keyboard module matrix configuration
 * \{
 ****************************************************************************************
 */

#include <stdint.h>
#include <stddef.h>

#ifdef CFG_PRF_HOGPD
    #include <user_hogpd_config.h>
#endif
#ifdef HAS_CONNECTION_FSM
	#include <app_con_fsm_config.h>
#endif
#include "app_kbd_defs.h"
#include "app_kbd_scan_matrix.h"

/**
 ****************************************************************************************
 * \brief Enable multi-key combinations
 ****************************************************************************************
 */
#define MULTI_KEY_COMBINATIONS_ON

/**
 ****************************************************************************************
 * \brief Maximum number of keys for key combination (maximum value is 8)
 ****************************************************************************************
 */
#define MULTI_KEY_NUM_OF_KEYS       2       // maximum 8

/**
 ****************************************************************************************
 * \brief Custom key definition. These keys are not treated as normal keys. No HID
 *        report is created. Instead kbd_params.notify_callback will be called when the
 *        corresponding key or key combinations is pressed. These definitions can be
 *        used to define keys in the key matrix or can be used in multi_key_combinations
 *        table definition.
 ****************************************************************************************
 */

enum custom_keys {
        CUSTOM_KEY_PAIR = 1,
        CUSTOM_KEY_CLEAR_BONDING_DATA,
        CUSTOM_KEY_AUDIO,
        CUSTOM_KEY_MOTION,
        CUSTOM_KEY_MOTION_CLICK,
#ifdef HAS_POWERUP_BUTTON     
        CUSTOM_KEY_POWER,
#endif    
        CUSTOM_KEY_SWITCH_TO_HOST1,
        CUSTOM_KEY_SWITCH_TO_HOST2,
        CUSTOM_KEY_SWITCH_TO_HOST3,
        CUSTOM_KEY_PAIR_WITH_HOST1,
        CUSTOM_KEY_PAIR_WITH_HOST2,
        CUSTOM_KEY_PAIR_WITH_HOST3,
#ifdef HAS_SPECIAL_ADVERTISING
        CUSTOM_KEY_ONOFF_HOST,
#endif
#ifdef HAS_IR    
        CUSTOM_KEY_IR,
#endif    
#ifdef AUDIO_TEST_MODE    
        CUSTOM_KEY_AUDIO_TEST_MODE,
#endif        
#ifdef MOUSE_GENERATE_TEST_PATTERN    
        CUSTOM_KEY_MOUSE_TEST,
#endif    
#ifdef DEBUG_EMULATE_PACKET_LOSS
        CUSTOM_KEY_INC_PACKET_LOSS,
        CUSTOM_KEY_DEC_PACKET_LOSS,
#endif    
        NUM_OF_CUSTOM_KEYS_PLUS_ONE
};

enum modifier_keys {
        MOD_KEY_MOTION = 1,
        NUM_OF_MOD_KEYS_PLUS_ONE
};

#define PAIR_KEY           KBD_CUSTOM_KEY(CUSTOM_KEY_PAIR)
#define CLRP_KEY           KBD_CUSTOM_KEY(CUSTOM_KEY_CLEAR_BONDING_DATA)
#define AUDIO_KEY          KBD_CUSTOM_KEY(CUSTOM_KEY_AUDIO)
#define MOTION_KEY         KBD_CUSTOM_KEY(CUSTOM_KEY_MOTION)
#define MOTION_CLICK       KBD_CUSTOM_KEY(CUSTOM_KEY_MOTION_CLICK)
#define POWER_KEY          KBD_CUSTOM_KEY(CUSTOM_KEY_POWER)
                           
#ifdef CFG_PRF_HOGPD       
    #define VOL_PLUS_KEY   KBD_SPECIAL_KEY(VOL_PLUS_KEY_CODE)
    #define VOL_MINUS_KEY  KBD_SPECIAL_KEY(VOL_MINUS_KEY_CODE)
    #define BACK_KEY       KBD_SPECIAL_KEY(BACK_KEY_CODE)
#else                      
    #define VOL_PLUS_KEY   KEY_UNUSED
    #define VOL_MINUS_KEY  KEY_UNUSED
    #define BACK_KEY       KEY_UNUSED
#endif

#define MOTION_LOCK_KEY KBD_FN_LOCK_KEY(MOD_KEY_MOTION)

// extra sets for 'hidden modifiers', e.g. the 'Fn' key
#define KBD_NR_SETS (1)

/**
 ****************************************************************************************
 * The key map.
 * 00xx:   regular key
 * FCxx:   modifier key.
 * F8xx:   FN Modifier.
 * F4xy:   special function (x = no of byte in the extended key report, y no of bit set).
 * F4Fy:   custom key
 * K_CODE: unknown key code - nothing is sent to the other side but the key is examined 
 *         for ghosting
 * KEY_UNUSED: if the key is not used
 ****************************************************************************************
 */
static const uint16_t kbd_keymap[KBD_NR_SETS][KBD_NR_ROW_OUTPUTS][KBD_NR_COLUMN_INPUTS] =
{
  {
  /* No Fn key pressed 
      0          1         2      
      -------------------------------------
      1          2         3       
      4          5         6       
      7          8         9       
      AUDIO      0         Fn (motion)   
      -------------------------------------*/
    { 0x0031,    0x0032,   0x0033,  0x0041 }, // 0
    { 0x0034,    0x0035,   0x0036,  0x0042 }, // 1
    { 0x0037,    0x0038,   0x0039,  0x0043 }, // 2
    { 0x002A,    0x0030,   0x0023, 	0x0044 }, // 3
  }
};

/**
 *******************************************************************************************
 * Definition of multi-key combinations. For each combination a row is added in the     
 * multi_key_combinations matrix. In each row the following members must be defined:    
 *    i) an array with MULTI_KEY_NUM_OF_KEYS elements of type multi_key_t containing    
 *       the row and column of each key used in the key combination. If a key is not     
 *       used in the key combination (e.g. two keys are needed when MULTI_KEY_NUM_OF_KEYS 
 *       is 3) then row and column must be set to MULTI_KEY_NOT_USED
 *   ii) The action executed when key combination is pressed. Available actions are
 *       defined in custom_keys. Macro KBD_KEY_TO_NOTIFY_CODE can be used to generate the
 *       appropriate notification code.
 ******************************************************************************************
 */
#ifdef MULTI_KEY_COMBINATIONS_ON

struct multi_key_combinations_t {
    struct multi_key_t key[MULTI_KEY_NUM_OF_KEYS];
    enum kbd_notification notification;
};

static const struct multi_key_combinations_t multi_key_combinations[] = {
    //    row1 col1   row2 col2                          Custom key                     pressed
    { { { 2,   0 }, { 2,   2 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_CLEAR_BONDING_DATA, true) },
    { { { 0,   0 }, { 0,   2 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_PAIR,               true) },
    { { { 2,   0 }, { 0,   0 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_SWITCH_TO_HOST1,    true) },
    { { { 2,   0 }, { 0,   1 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_SWITCH_TO_HOST2,    true) },
    { { { 2,   0 }, { 0,   2 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_SWITCH_TO_HOST3,    true) },
    { { { 2,   2 }, { 0,   0 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_PAIR_WITH_HOST1,    true) },
    { { { 2,   2 }, { 0,   1 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_PAIR_WITH_HOST2,    true) },
    { { { 2,   2 }, { 0,   2 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_PAIR_WITH_HOST3,    true) },
#ifdef HAS_SPECIAL_ADVERTISING
    { { { 0,   0 }, { 3,   1 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_ONOFF_HOST,         true) },
#endif
#ifdef AUDIO_TEST_MODE    
    { { { 0,   1 }, { 3,   1 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_AUDIO_TEST_MODE,    true) },
#endif
#ifdef MOUSE_GENERATE_TEST_PATTERN    
    { { { 1,   0 }, { 1,   1 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_MOUSE_TEST,         true) },
#endif    
#ifdef HAS_IR    
    { { { 1,   0 }, { 1,   2 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_IR,                 true) },
#endif 
#ifdef DEBUG_EMULATE_PACKET_LOSS
    { { { 2,   1 }, { 2,   2 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_INC_PACKET_LOSS,    true) },
    { { { 2,   0 }, { 2,   1 } }, KBD_KEY_TO_NOTIFY_CODE(CUSTOM_KEY_DEC_PACKET_LOSS,    true) },
#endif    
};   
#endif

/**
 * \}
 * \}
 * \}
 */

#endif // _APP_KBD_MATRIX_H_