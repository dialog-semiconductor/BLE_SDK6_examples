/**
 ****************************************************************************************
 *
 * \file port_kbd.c
 *
 * \brief Keyboard module platform adaptation source file
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
 * \addtogroup PORT_KEYBOARD
 * \{
 ****************************************************************************************	 
 */

#ifdef HAS_KBD 


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_task.h"
#include "systick.h"
#include "app_kbd.h"
#include "port_kbd.h"
#include "port_timer.h"
#include "port_wkup.h"
#include <app_kbd_config.h>
#include <port_timer_config.h>

#ifdef HAS_CONNECTION_FSM 
    #include "app_con_fsm.h"
#endif

#include <user_periph_setup.h>


#ifndef HAS_WKUP
    #error "HAS_WKUP must be defined for keyboard scan to work"
#endif

#ifndef HAS_POWERUP_BUTTON
    #define WKUP_KBD_POWER_BUTTON_CHANNEL (NUM_OF_WKUP_CHANNELS) // This will generate an assertion when wkup channel is used
    #define POWER_BUTTON_ROW    0 // dummy value
    #define POWER_BUTTON_COLUMN 0 // dummy value
#endif

#if defined(ARRAYS_AS_CONST_DATA)
    #define KBD_TYPE_QUALIFIER  const
    #define KBD_ARRAY_ATTRIBUTE  
#else
    #define KBD_TYPE_QUALIFIER  
    #define KBD_ARRAY_ATTRIBUTE __attribute__((section(".data")))
#endif
    
// Masks for the initialization of the KBD controller
#define MASK_P0				(0x4000	| SET_MASK0_FROM_COLUMN(0)   | SET_MASK0_FROM_COLUMN(1)   | SET_MASK0_FROM_COLUMN(2)   | SET_MASK0_FROM_COLUMN(3)   \
									| SET_MASK0_FROM_COLUMN(4)   | SET_MASK0_FROM_COLUMN(5)   | SET_MASK0_FROM_COLUMN(6)   | SET_MASK0_FROM_COLUMN(7)   \
									| SET_MASK0_FROM_COLUMN(8)   | SET_MASK0_FROM_COLUMN(9)   | SET_MASK0_FROM_COLUMN(10)  | SET_MASK0_FROM_COLUMN(11)  \
									| SET_MASK0_FROM_COLUMN(12)  | SET_MASK0_FROM_COLUMN(13)  | SET_MASK0_FROM_COLUMN(14)  | SET_MASK0_FROM_COLUMN(15))
                                    
#define MASK_P12			(0x0000	| SET_MASK12_FROM_COLUMN(0)  | SET_MASK12_FROM_COLUMN(1)  | SET_MASK12_FROM_COLUMN(2)  | SET_MASK12_FROM_COLUMN(3)  \
									| SET_MASK12_FROM_COLUMN(4)  | SET_MASK12_FROM_COLUMN(5)  | SET_MASK12_FROM_COLUMN(6)  | SET_MASK12_FROM_COLUMN(7)  \
									| SET_MASK12_FROM_COLUMN(8)  | SET_MASK12_FROM_COLUMN(9)  | SET_MASK12_FROM_COLUMN(10) | SET_MASK12_FROM_COLUMN(11) \
									| SET_MASK12_FROM_COLUMN(12) | SET_MASK12_FROM_COLUMN(13) | SET_MASK12_FROM_COLUMN(14) | SET_MASK12_FROM_COLUMN(15))
                                    
#define MASK_P3				(0x0000	| SET_MASK3_FROM_COLUMN(0)   | SET_MASK3_FROM_COLUMN(1)   | SET_MASK3_FROM_COLUMN(2)   | SET_MASK3_FROM_COLUMN(3)   \
									| SET_MASK3_FROM_COLUMN(4)   | SET_MASK3_FROM_COLUMN(5)   | SET_MASK3_FROM_COLUMN(6)   | SET_MASK3_FROM_COLUMN(7)   \
									| SET_MASK3_FROM_COLUMN(8)   | SET_MASK3_FROM_COLUMN(9)   | SET_MASK3_FROM_COLUMN(10)  | SET_MASK3_FROM_COLUMN(11)  \
									| SET_MASK3_FROM_COLUMN(12)  | SET_MASK3_FROM_COLUMN(13)  | SET_MASK3_FROM_COLUMN(14)  | SET_MASK3_FROM_COLUMN(15))

#define CHECK_KBD_CTRL_MASKS       (  (MASK_P0 & CHECK_MASK0_FROM_COLUMN(KBD_NR_COLUMN_INPUTS-1))     \
                                    | (MASK_P12 & CHECK_MASK12_FROM_COLUMN(KBD_NR_COLUMN_INPUTS-1))   \
                                    | (MASK_P3 & CHECK_MASK3_FROM_COLUMN(KBD_NR_COLUMN_INPUTS-1)) )

#if (!CHECK_KBD_CTRL_MASKS)
	#error "KBD controller masks not defined properly!"
#endif

// used for cycle optimization

#define CHECK_COLUMN_INPUT(x) (COLUMN_0_PORT == x  || COLUMN_1_PORT == x  || COLUMN_2_PORT == x  || COLUMN_3_PORT == x  || \
                               COLUMN_4_PORT == x  || COLUMN_5_PORT == x  || COLUMN_6_PORT == x  || COLUMN_7_PORT == x  || \
                               COLUMN_8_PORT == x  || COLUMN_9_PORT == x  || COLUMN_1x_PORT == x || COLUMN_11_PORT == x || \
                               COLUMN_12_PORT == x || COLUMN_13_PORT == x || COLUMN_14_PORT == x || COLUMN_15_PORT == x) ? 1 :0
                                
#define P0_HAS_INPUT  CHECK_COLUMN_INPUT(0) 
#define P1_HAS_INPUT  CHECK_COLUMN_INPUT(1) 
#define P2_HAS_INPUT  CHECK_COLUMN_INPUT(2) 
#define P3_HAS_INPUT  CHECK_COLUMN_INPUT(3) 


/*
 * Debouncing internals
 ****************************************************************************************
 */
#define DEBOUNCE_COUNTER_PRESS      ((int)(1 + ( ((kbd_params.press_debounce_counter_in_us - kbd_params.full_scan_cycle_in_us) / kbd_params.partial_scan_cycle_in_us) + 0.999 ) ) - 1)

#define DEBOUNCE_COUNTER_RELEASE    ((int)( (kbd_params.release_debounce_counter_in_us / kbd_params.partial_scan_cycle_in_us) + 0.999 ) - 1)
    
enum DEBOUNCE_STATE {
    IDLE = 0,
    PRESS_DEBOUNCING,
    WAIT_RELEASE,
    RELEASE_DEBOUNCING,
};

struct debounce_counter_t {
    enum DEBOUNCE_STATE state;
    uint8_t cnt;
};


/*
 * Scanning 
 ****************************************************************************************
 */
 
enum key_scan_states current_scan_state __PORT_RETAINED;
static int scanning_substate            __PORT_RETAINED;

scan_t kbd_scandata[KBD_NR_ROW_OUTPUTS];                                // last reported status of the keyboard
scan_t kbd_new_scandata[KBD_NR_ROW_OUTPUTS];                            // current status of the keyboard
bool kbd_active_row[KBD_NR_ROW_OUTPUTS];                                // last known status of the keyboard
bool kbd_new_key_detected;                                              // flag to indicate that a new key was detected during the last scan

uint32_t scan_cycle_time;                                               // time until the next wake up from SysTick
uint32_t scan_cycle_time_last;                                          // duration of the current scan cycle
bool full_scan;                                                         // when true a full keyboard scan is executed once. else only partial scan is done.
bool next_is_full_scan;                                                 // Got an interrupt (key press) during partial scanning
                                                                    
                                                                    
uint8_t kbd_bounce_active;                                              // flag indicating we are still in debouncing mode
uint16_t kbd_bounce_intersections[KBD_DEBOUNCE_BUFFER_SIZE];            // holds output - input pair (key) for which debouncing is on
scan_t kbd_bounce_rows[KBD_NR_ROW_OUTPUTS];                             // holds the key mask ('1' is active) for each row that is being debounced
struct debounce_counter_t kbd_bounce_counters[KBD_DEBOUNCE_BUFFER_SIZE];// counter for each intersection (key)
uint8_t kbd_global_deb_cnt;                                             // counts down for press debouncing time when after a scan no new key has been detected

uint8_t kbd_membrane_status;                                            // 0: outputs are low => SW is prohibited, 1: outputs are high-Z => SW can do scanning
bool kbd_cntrl_active;                                                  // flag to indicate the the Keyboard Controller is ON

#ifdef HAS_POWERUP_BUTTON
uint8_t force_power_off           __PORT_RETAINED;
#endif

enum delay_monitor_status {
    MONITOR_IDLE,
    MONITOR_PRESS,
    MONITOR_RELEASE,
};

enum delay_trigger_status {
    NO_TRIGGER,
    PRESSED_TRIGGER,
    RELEASED_TRIGGER,
    PRESSED_TRIGGER_SYNCED,
    RELEASED_TRIGGER_SYNCED,
};

struct hid_msg {
    enum delay_trigger_status trigger;
};

// DELAYED WAKEUP
enum delay_monitor_status monitor_kbd_delayed_start_st __PORT_RETAINED;  // State of delayed start monitoring
enum delay_trigger_status trigger_kbd_delayed_start_st __PORT_RETAINED;  // Trigger event from the delayed start monitoring
bool ble_is_woken_up                                   __PORT_RETAINED;  // Flag to indicate that the request to the BLE was triggered


KBD_TYPE_QUALIFIER uint8_t kbd_input_ports[KBD_NR_COLUMN_INPUTS] KBD_ARRAY_ATTRIBUTE =
{
#if (KBD_NR_COLUMN_INPUTS>0)    
	COL(0),                     // column 0
#endif    
#if (KBD_NR_COLUMN_INPUTS>1)    
	COL(1),                     // column 1
#endif    
#if (KBD_NR_COLUMN_INPUTS>2)    
	COL(2),                     // column 2
#endif    
#if (KBD_NR_COLUMN_INPUTS>3)    
	COL(3),                     // column 3
#endif    
#if (KBD_NR_COLUMN_INPUTS>4)    
	COL(4),                     // column 4
#endif    
#if (KBD_NR_COLUMN_INPUTS>5)    
	COL(5),                     // column 5
#endif    
#if (KBD_NR_COLUMN_INPUTS>6)    
	COL(6),                     // column 6
#endif    
#if (KBD_NR_COLUMN_INPUTS>7)    
	COL(7),                     // column 7
#endif    
#if (KBD_NR_COLUMN_INPUTS>8)    
	COL(8),                     // column 8
#endif    
#if (KBD_NR_COLUMN_INPUTS>9)    
	COL(9),                     // column 9
#endif    
#if (KBD_NR_COLUMN_INPUTS>10)    
	COL(10),                    // column 10
#endif    
#if (KBD_NR_COLUMN_INPUTS>11)    
	COL(11),                    // column 11
#endif    
#if (KBD_NR_COLUMN_INPUTS>12)    
	COL(12),                    // column 12
#endif    
#if (KBD_NR_COLUMN_INPUTS>13)    
	COL(13),                    // column 13
#endif    
#if (KBD_NR_COLUMN_INPUTS>14)    
	COL(14),                    // column 14
#endif    
#if (KBD_NR_COLUMN_INPUTS>15)    
	COL(15)                     // column 15
#endif      
};

KBD_TYPE_QUALIFIER uint8_t kbd_output_mode_regs[KBD_NR_ROW_OUTPUTS] KBD_ARRAY_ATTRIBUTE =
{
#if (KBD_NR_ROW_OUTPUTS>0)    
	SET_OUTPUT_MODE_REG(0),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>1)    
	SET_OUTPUT_MODE_REG(1),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>2)    
	SET_OUTPUT_MODE_REG(2),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>3)    
	SET_OUTPUT_MODE_REG(3),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>4)    
	SET_OUTPUT_MODE_REG(4),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>5)    
	SET_OUTPUT_MODE_REG(5),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>6)    
	SET_OUTPUT_MODE_REG(6),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>7)    
	SET_OUTPUT_MODE_REG(7),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>8)    
	SET_OUTPUT_MODE_REG(8),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>9)    
	SET_OUTPUT_MODE_REG(9),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>10)    
	SET_OUTPUT_MODE_REG(10),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>11)    
	SET_OUTPUT_MODE_REG(11),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>12)    
	SET_OUTPUT_MODE_REG(12),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>13)    
	SET_OUTPUT_MODE_REG(13),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>14)    
	SET_OUTPUT_MODE_REG(14),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>15)    
	SET_OUTPUT_MODE_REG(15)                  
#endif    
};

KBD_TYPE_QUALIFIER uint8_t kbd_output_reset_data_regs[KBD_NR_ROW_OUTPUTS] KBD_ARRAY_ATTRIBUTE =
{
#if (KBD_NR_ROW_OUTPUTS>0)    
	SET_RESET_REG(0),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>1)    
	SET_RESET_REG(1),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>2)    
	SET_RESET_REG(2),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>3)    
	SET_RESET_REG(3),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>4)    
	SET_RESET_REG(4),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>5)    
	SET_RESET_REG(5),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>6)    
	SET_RESET_REG(6),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>7)    
	SET_RESET_REG(7),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>8)    
	SET_RESET_REG(8),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>9)    
	SET_RESET_REG(9),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>10)    
	SET_RESET_REG(10),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>11)    
	SET_RESET_REG(11),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>12)    
	SET_RESET_REG(12),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>13)    
	SET_RESET_REG(13),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>14)    
	SET_RESET_REG(14),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>15)    
	SET_RESET_REG(15)                  
#endif    
};

KBD_TYPE_QUALIFIER uint16_t kbd_out_bitmasks[KBD_NR_ROW_OUTPUTS] KBD_ARRAY_ATTRIBUTE =
{
#if (KBD_NR_ROW_OUTPUTS>0)    
	SET_BITMAP(0),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>1)    
	SET_BITMAP(1),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>2)    
	SET_BITMAP(2),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>3)    
	SET_BITMAP(3),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>4)    
	SET_BITMAP(4),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>5)    
	SET_BITMAP(5),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>6)    
	SET_BITMAP(6),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>7)    
	SET_BITMAP(7),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>8)    
	SET_BITMAP(8),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>9)    
	SET_BITMAP(9),                  
#endif    
#if (KBD_NR_ROW_OUTPUTS>10)    
	SET_BITMAP(10),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>11)    
	SET_BITMAP(11),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>12)    
	SET_BITMAP(12),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>13)    
	SET_BITMAP(13),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>14)    
	SET_BITMAP(14),                 
#endif    
#if (KBD_NR_ROW_OUTPUTS>15)    
	SET_BITMAP(15)                  
#endif        
};

KBD_TYPE_QUALIFIER uint8_t kbd_input_mode_regs[KBD_NR_COLUMN_INPUTS] KBD_ARRAY_ATTRIBUTE =
{
#if (KBD_NR_COLUMN_INPUTS>0)    
	SET_INPUT_MODE_REG(0),                    
#endif    
#if (KBD_NR_COLUMN_INPUTS>1)    
	SET_INPUT_MODE_REG(1),                    
#endif    
#if (KBD_NR_COLUMN_INPUTS>2)    
	SET_INPUT_MODE_REG(2),                    
#endif    
#if (KBD_NR_COLUMN_INPUTS>3)    
	SET_INPUT_MODE_REG(3),                    
#endif    
#if (KBD_NR_COLUMN_INPUTS>4)    
	SET_INPUT_MODE_REG(4),                    
#endif    
#if (KBD_NR_COLUMN_INPUTS>5)    
	SET_INPUT_MODE_REG(5),                    
#endif    
#if (KBD_NR_COLUMN_INPUTS>6)    
	SET_INPUT_MODE_REG(6),                    
#endif    
#if (KBD_NR_COLUMN_INPUTS>7)    
	SET_INPUT_MODE_REG(7),                    
#endif    
#if (KBD_NR_COLUMN_INPUTS>8)    
	SET_INPUT_MODE_REG(8),                    
#endif    
#if (KBD_NR_COLUMN_INPUTS>9)    
	SET_INPUT_MODE_REG(9),                    
#endif    
#if (KBD_NR_COLUMN_INPUTS>10)    
	SET_INPUT_MODE_REG(10),                   
#endif    
#if (KBD_NR_COLUMN_INPUTS>11)    
	SET_INPUT_MODE_REG(11),                   
#endif    
#if (KBD_NR_COLUMN_INPUTS>12)    
	SET_INPUT_MODE_REG(12),                   
#endif    
#if (KBD_NR_COLUMN_INPUTS>13)    
	SET_INPUT_MODE_REG(13),                   
#endif    
#if (KBD_NR_COLUMN_INPUTS>14)    
	SET_INPUT_MODE_REG(14),                   
#endif    
#if (KBD_NR_COLUMN_INPUTS>15)    
	SET_INPUT_MODE_REG(15)                    
#endif      
};

typedef int kbd_input_ports_check[ (sizeof(kbd_input_ports) / sizeof(uint8_t)) == KBD_NR_COLUMN_INPUTS];                   // on error: the kbd_input_ports[] is not defined properly!
typedef int kbd_output_mode_regs_check[ (sizeof(kbd_output_mode_regs) / sizeof(uint8_t)) == KBD_NR_ROW_OUTPUTS];        // on error: the kbd_output_mode_regs[] is not defined properly!
typedef int kbd_output_reset_regs_check[ (sizeof(kbd_output_reset_data_regs) / sizeof(uint8_t)) == KBD_NR_ROW_OUTPUTS]; // on error: the kbd_output_reset_data_regs[] is not defined properly!
typedef int kbd_output_bitmasks_check[ (sizeof(kbd_out_bitmasks) / sizeof(uint16_t)) == KBD_NR_ROW_OUTPUTS];            // on error: the kbd_out_bitmasks[] is not defined properly!
typedef int kbd_output_input_mode_regs_check[ (sizeof(kbd_input_mode_regs) / sizeof(uint8_t)) == KBD_NR_COLUMN_INPUTS];    // on error: the kbd_input_mode_regs[] is not defined properly!


/*
 * NON RETAINED VARIABLE DECLARATIONS (GLOBAL + STATIC)
 ****************************************************************************************
 */
 
static bool systick_hit = false;
static bool wkup_hit = false;      // does not have to be a RETAINED var since the KBD application wakes up from Deep Sleep only from a key press!


/*
 * LOCAL FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * \brief It programs the Keyboard Controller to scan any key press 
 *                coming from the "inactive rows" and inform the system after 
 *                DEBOUNCE_TIME_PRESS msec (optionally) to do a full key scan.
 ****************************************************************************************
 */
static void kbd_scan_enable_kbd_irq(void)
{
    SetWord16(GPIO_DEBOUNCE_REG, 0x2000 | 0); // T ms debouncing

    SetWord16(KBRD_IRQ_IN_SEL2_REG, MASK_P3);
    SetWord16(KBRD_IRQ_IN_SEL1_REG, MASK_P12);
    SetWord16(KBRD_IRQ_IN_SEL0_REG, MASK_P0);

    SetWord16(GPIO_RESET_IRQ_REG, 0x20);    // clear any garbage
    NVIC_ClearPendingIRQ(KEYBRD_IRQn);      // clear it to be on the safe side...
    NVIC_SetPriority(KEYBRD_IRQn, 2);         
    NVIC_EnableIRQ(KEYBRD_IRQn);
    
    kbd_cntrl_active = true;
}


/**
 ****************************************************************************************
 * \brief Does debouncing for a key (called even when the key is considered as pressed
 *        after press debouncing has finished).
 *
 * \param[in] output
 * \param[in] input
 * \param[in] pressed
 *
 * \return  0, key is ignored
 *          1, key is accepted and should be checked for ghosting
 ****************************************************************************************
 */
static inline int kbd_scan_debounce_key(const uint16_t output, const uint16_t input, const bool pressed)
{
    const uint16_t my_intersection = (output << 8) | input;
    const scan_t imask = 1 << input;
    int i;
    
    // to check whether a key is valid or not we always check the default keymap (#0).
    // the assumption is that a key will definitely appear in the default  
    // keymap and may appear in secondary keymaps as well.
    // if this is not sufficient for a specific application then additional 
    // logic needs to be implemented.
    if (kbd_keymap[0][output][input] == 0) {
        return 0;                       // this key does not exist in the default 
    }                                    // keymap => it's a ghost!

    // *** DEBOUNCE ***
    
    // check if there's an entry for this intersection in the array. an entry is
    // reserved when a key is pressed. if an entry is reserved, 
    // then the entry remains reserved until the key is released!
    for (i = 0; i < KBD_DEBOUNCE_BUFFER_SIZE ; ++i) {
        if (my_intersection == kbd_bounce_intersections[i]) {
            break;
        }
    }
           
    // the debouncing counter is 2-field structure. The first field is the state. 
    // The second field is the debouncing counter that counts down when doing
    // press and release debouncing
    if (i != KBD_DEBOUNCE_BUFFER_SIZE) {     // entry found!
        struct debounce_counter_t *deb = &kbd_bounce_counters[i];
        
        switch(deb->state) {
        case IDLE:
            ASSERT_ERROR(0);         // cannot be in here!
            break;
        case PRESS_DEBOUNCING:
            if (deb->cnt == 0) {     // debouncing done! check key status...
                if (pressed) {       // go to WAIT_RELEASE and continue with deghosting
                    deb->state = WAIT_RELEASE;
                }
                else {
                    deb->state = IDLE;  // key is ignored since after debouncing is inactive
                    kbd_bounce_intersections[i] = 0xFFFF;
                    kbd_bounce_rows[output] &= ~imask;
                    return 0;
                }
            } 
            else {
                kbd_new_scandata[output] |= imask;  // this bit is still toggling! reset it to the previous stable state so that
                                        // it doesn't affect deghosting of other valid keys below
                                        
                return 0;               // still debouncing...
            }
            break;
        case WAIT_RELEASE:
            if (!pressed) {
                deb->state = RELEASE_DEBOUNCING;
                deb->cnt = DEBOUNCE_COUNTER_RELEASE;
                kbd_new_scandata[output] &= ~imask; // this bit is still toggling! reset it to the previous stable state so that
                                                    // is doesn't affect deghosting of other valid keys below
                                        
                return 0;                           // do debouncing...
            } 
            else {                                  // check if this key press has been reported (could be a ghost key)
                if ( !(kbd_scandata[output] & imask) )
                    return 0;                       // skip key press as it has already been reported!
                                                    // else, do deghosting so as to report the key press if need be
            }
            break;
        case RELEASE_DEBOUNCING:
            if (deb->cnt == 0) {        // debouncing done! check key status...
                if (pressed) {          // still pressed? fake release! return to WAIT_RELEASE
                    deb->state = WAIT_RELEASE;
                    return 0;
                } 
                else {
                    deb->state = IDLE;  // return to IDLE and continue to register the key release in the buffer if need be
                    kbd_bounce_intersections[i] = 0xFFFF;
                    kbd_bounce_rows[output] &= ~imask;
                    if (kbd_scandata[output] & imask) {   // check if the key press had been reported
                        return 0;       // it hadn't so skip the release report!
                    }
                }
            } 
            else {
                kbd_new_scandata[output] &= ~imask;  // this bit is still toggling! reset it to the previous stable state so that
                                                     // is doesn't affect deghosting of other valid keys below
                return 0;                            // still debouncing...
            }
            break;
        default:
            break;
        }
    } else {                                // find an empty place
        if (!pressed) {
            return 0;                       // if it's a release then there should be a valid entry for this key in the array!
        }                                   // if there isn't then this happened because the array was full and the key is ignored

        for (i = 0; i < KBD_DEBOUNCE_BUFFER_SIZE ; ++i) {
            if (kbd_bounce_intersections[i] == 0xFFFF) {
                if (kbd_bounce_counters[i].state != IDLE) {
                    ASSERT_ERROR(0);        // this should not happen!
                }
                break;
            }
        }
        if (i == KBD_DEBOUNCE_BUFFER_SIZE) {     // no space available!
            return 0;
        }
        
        kbd_bounce_intersections[i] = my_intersection;
        kbd_bounce_rows[output] |= imask;
        kbd_bounce_counters[i].cnt = DEBOUNCE_COUNTER_PRESS;
        kbd_bounce_counters[i].state = PRESS_DEBOUNCING;
        kbd_new_scandata[output] |= imask;  // this bit is still toggling! reset it to the previous stable state so that
                                            // is doesn't affect deghosting of other valid keys below
        return 0;                           // debouncing is started! the key will be put into the key buffer when it's finished!
    }
    // No bounce, continue to check for ghosting
    return 1;
}


/**
 ****************************************************************************************
 * \brief Processes scan results. If debouncing and deghosting allow it,
 *        the keycodes of the detected keys are placed in the keycode buffer. 
 ****************************************************************************************
 */
static inline void kbd_scan_process_scandata(void)
{	
    scan_t new_scan_status[KBD_NR_ROW_OUTPUTS];
    int i;
    uint8_t leading_zeros;
    
    // do debouncing for all keys
    for (i = 0; i < KBD_NR_ROW_OUTPUTS; ++i) {
        if (kbd_out_bitmasks[i]) {
            scan_t xorword = (kbd_scandata[i] ^ kbd_new_scandata[i]);
            
            new_scan_status[i] = kbd_new_scandata[i];
            
            // look into kbd_bounce_intersections[] for any keys that are being debounced 
            // that might not be reported in the xorword
            xorword |= kbd_bounce_rows[i];
            
            if (xorword) { // if any key state changed
                scan_t bit;
                scan_t mask;
                int press;
                
                kbd_active_row[i] = true;
                
                do {
                    leading_zeros = __clz((uint32_t)xorword);
                    if (leading_zeros == 32) {
                        break;
                    } else {
                        bit = 31 - leading_zeros;
                        mask = 1 << bit;
                    }
                        
                    press = !(new_scan_status[i] & mask);
                    if (!kbd_scan_debounce_key(i, bit, (press != 0))) {
                        // This key is ignored, copy the bit from kbd_scandata[i] to newscanword => always keep last key status!
                        new_scan_status[i] = (new_scan_status[i] & (~mask)) | (kbd_scandata[i] & mask);
                    }
                    xorword &= ~mask;
                    
                } while (xorword);
            } else {
                kbd_active_row[i] = false;
            }
        }
    }
        
    // do deghosting for valid debounced keys and record those that are valid
    for(i = 0; i < KBD_NR_ROW_OUTPUTS; ++i) {
        if (kbd_out_bitmasks[i]) {
            scan_t xorword = (kbd_scandata[i] ^ new_scan_status[i]);
            
            if (xorword) { // if any key state changed
                scan_t bit;
                scan_t mask;
                int press;
                
                do {
                    leading_zeros = __clz((uint32_t)xorword);
                    if (leading_zeros == 32) {
                        break;
                    } else {
                        bit = 31 - leading_zeros;
                        mask = 1 << bit;
                    }
                        
                    press = !(new_scan_status[i] & mask);
                    if (!app_kbd_record_key(i, bit, (press != 0))) {
                        // This keypress is ignored, copy the bit from kbd_scandata[i] to newscanword => always keep last key status!
                        new_scan_status[i] = (new_scan_status[i] & (~mask)) | (kbd_scandata[i] & mask);
                    }
                        
                    xorword &= ~mask;
                    
                } while (xorword);
            }
        }
    }

    for (i = 0; i < KBD_NR_ROW_OUTPUTS; i++) {
        kbd_scandata[i] = new_scan_status[i]; 
    }
}

/**
 ****************************************************************************************
 * \brief Programs the Wakeup Timer to scan a specific key press for a predefined
 *        duration and wakes-up the system.
 *
 * \param[in]   press
 ****************************************************************************************
 */
static void kbd_scan_enable_delayed_wakeup_irq(bool press)
{
    if (kbd_params.delayed_wakeup) {
        uint32_t pol;
        monitor_kbd_delayed_start_st = press ? MONITOR_PRESS : MONITOR_RELEASE;
        pol = (press) ? 0xFFFFFFFF : 0;
        port_wkup_enable_irq(WKUP_KBD_POWER_BUTTON_CHANNEL, pol);        
    }
}


enum key_scan_states { 
        KEY_SCAN_INACTIVE,  // State 1
        KEY_SCAN_IDLE,      // State 2
        KEY_SCANNING,       // State 3
        KEY_STATUS_UPD,     // State 4
};



/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */


/*
 * GPIO handling 
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   idx
 ****************************************************************************************
 */
static void kbd_scan_set_row_to_low(int idx)
{
    if (kbd_out_bitmasks[idx]) {
        SetWord16(kbd_output_reset_data_regs[idx] + P0_DATA_REG, kbd_out_bitmasks[idx]);  // output low
        SetWord16(kbd_output_mode_regs[idx] + P0_DATA_REG, 0x300);                        // mode gpio output
    }
}


/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   idx
 ****************************************************************************************
 */
static void kbd_scan_set_column_to_input_pullup(int idx)
{
    if (kbd_input_mode_regs[idx]) {
        SetWord16(kbd_input_mode_regs[idx] + P0_DATA_REG, 0x100);                         // mode gpio input pullup
    }
}


/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   idx
 ****************************************************************************************
 */
static void kbd_scan_set_row_to_input_highz(int idx)
{
    if (kbd_out_bitmasks[idx]) {
        SetWord16(kbd_output_mode_regs[idx] + P0_DATA_REG, 0x000);                        // mode gpio input highz
    }
}


/*
 * Keyboard "membrane" handling
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * \brief Set all columns to "input pull-up" state
 ****************************************************************************************
 */
static void kbd_scan_membrane_input_setup(void)
{	
	for (int i = 0; i < KBD_NR_COLUMN_INPUTS; ++i) 
        kbd_scan_set_column_to_input_pullup(i);
}


/**
 ****************************************************************************************
 * \brief Set all rows to "high-Z" state to enable SW scanning
 ****************************************************************************************
 */
static void kbd_scan_membrane_output_wakeup(void)
{
	for (int i = 0; i < KBD_NR_ROW_OUTPUTS; ++i)
        kbd_scan_set_row_to_input_highz(i);
	
	kbd_membrane_status = 1;
}


/**
 ****************************************************************************************
 * \brief Set all rows to "low" state to enable HW scanning (WKUP-TIMER)
 ****************************************************************************************
 */
static void kbd_scan_membrane_output_sleep(void)
{	
	kbd_membrane_status = 0;
	
	// Pull all outputs low to quickly discharge the membrane.
	for (int i = 0; i < KBD_NR_ROW_OUTPUTS; ++i) {
        kbd_scan_set_row_to_low(i);
	}
	
	// The inputs are pullup, so their transition to low will trigger the keyboard interrupt
}


/**
 ****************************************************************************************
 * \brief
 ****************************************************************************************
 */
static void port_kbd_scan_init_vars(void)
{
    int i;  

    full_scan = false;
    next_is_full_scan = false;
    
	kbd_membrane_status = 0;

    
	for (i = 0; i < KBD_DEBOUNCE_BUFFER_SIZE ; ++i) {
		kbd_bounce_intersections[i] = 0xFFFF;
		kbd_bounce_counters[i].cnt = 0;
        kbd_bounce_counters[i].state = IDLE;
	}
    kbd_global_deb_cnt = 0;
	kbd_bounce_active = 0;
	
	for (i = 0; i < KBD_NR_ROW_OUTPUTS; ++i) {
        const scan_t val = (1 << KBD_NR_COLUMN_INPUTS) - 1;
        
		kbd_scandata[i] = val;
        kbd_active_row[i] = false;
        kbd_bounce_rows[i] = 0;
    }
    
    kbd_cntrl_active = false;

    kbd_new_key_detected = false;
}

/**
 ****************************************************************************************
 * \brief stops the systick timer
 ****************************************************************************************
 */
static void kbd_scan_timer_stop(void)
{
	// leave systick in a known state
	SetWord32(0xE000E010, 0x00000000);      // disable systick
	GetWord32(0xE000E010);                  // make sure COUNTFLAG is reset
    systick_hit = false;
}

/**
 ****************************************************************************************
 * \brief
 ****************************************************************************************
 */
static void kbd_scan_init_wkup(void)
{
    port_wkup_disable_irq(WKUP_KBD_CHANNEL);
    // Clear status
    wkup_hit = false;    
}

/**
 ****************************************************************************************
 * \brief   Handler of a dummy TASK_APP msg sent to trigger the timer
 *
 * \remarks In case of delayed start, a dummy message is sent to the TASK_APP.
 *          This msg is put in the queue when the BLE is woken up. When the
 *          handler is called, it is certain that the BLE is running and 
 *          the timer may start.
 ****************************************************************************************
 */                                    
static void port_kbd_scan_msg_handler(void)
{
    if (kbd_params.delayed_wakeup) {
        dbg_puts(DBG_KBD_LVL, "HID_MSG handler\r\n");        
        if (trigger_kbd_delayed_start_st == PRESSED_TRIGGER_SYNCED) {           
#ifdef HAS_CONNECTION_FSM
            ASSERT_WARNING(app_con_fsm_get_state() == IDLE_ST);          
#endif
            trigger_kbd_delayed_start_st = NO_TRIGGER;

            port_timer_set(APP_KBD_TIMER, 0, kbd_params.delayed_wakeup_time_in_us);
            
            // Set WKUPCT to monitor key release
            port_kbd_scan_enable_delayed_scanning(false);
        }
        if (trigger_kbd_delayed_start_st == RELEASED_TRIGGER_SYNCED) {           
#ifdef HAS_CONNECTION_FSM
            ASSERT_WARNING(app_con_fsm_get_state() == IDLE_ST);         
#endif
            port_timer_clear(APP_KBD_TIMER, 0);
            trigger_kbd_delayed_start_st = NO_TRIGGER;
            
            // Put BLE in permanent sleep mode
            arch_ble_ext_wakeup_on();
            
            // Set WKUPCT to monitor key press
            port_kbd_scan_enable_delayed_scanning(true);
        }   
    }
}


void port_kbd_scan_init(void)
{
    kbd_scan_timer_stop();                 // Make sure SysTick is stopped
    kbd_scan_init_wkup();
    
    port_kbd_scan_init_vars();
#ifdef HAS_CONNECTION_FSM    
    if (kbd_params.delayed_wakeup) {
    #ifndef HAS_POWERUP_BUTTON
        ASSERT_ERROR(0); // HAS_POWERUP_BUTTON must be defined to use delayed_wakeup
    #endif
    }
#endif    
}    


/*
 * Keyboard scanning
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * \brief Handles the scanning of the key matrix and the processing of the results
 *
 * \param[in] row - number of row to scan
 *
 * \return  false, when scanning of the rows in the current scan cycle is not completed
 *          true, when all rows have been scanned
 ****************************************************************************************
 */
static bool kbd_scan_matrix(int *row)
{
	// 1. Find which row has the pressed key 
	// 2. Drive this row 'low' for 1 scan cycle
	// In the next scan cycle: 
	// 3. Read inputs
	// 4. Turn the row to 'high-Z'
	// 5. Check if there's another row with a pressed key and if so go to 2
	// 6. if no other rows remain to be scanned
    //    a. process results (debounce and deghost here)
    //    b. decrement debounce counters
    //    c. Program Keyboard Controller to scan the rest of the rows
	// 7. Exit
	
	uint8_t i = *row;
    int j;
	int8_t prev_line = -1;
	const scan_t scanmask = (1 << KBD_NR_COLUMN_INPUTS) - 1;

	ASSERT_ERROR(kbd_membrane_status != 0);
    
	// Update SysTick next tick time
    scan_cycle_time -= kbd_params.row_scan_time_in_us;
	
	// Step 0. Check if we've already driven a row to low. In that case we should scan the inputs...
    if ( (i > 0) && (GetWord16(kbd_output_mode_regs[i-1] + P0_DATA_REG) == 0x300) ) {
		prev_line = i - 1;
    }
	
	// Step 1. Find which row has the pressed key
	if (!full_scan) {
		for (; i < KBD_NR_ROW_OUTPUTS; ++i) {
			if(kbd_out_bitmasks[i]       // valid output
			   && kbd_active_row[i]) {   // This line has a pressed key
               break;
			}
		}
    }
    
	if ( prev_line != -1 ) {
		scan_t scanword = 0;
        uint16_t kbd_gpio_in[5];
		
        kbd_gpio_in[4] = 0xFFFF;
        
		// Step 3. Read the input registers.
#ifdef P0_HAS_INPUT					
		kbd_gpio_in[0] = GetWord16(P0_DATA_REG);
#endif
#ifdef P1_HAS_INPUT					
		kbd_gpio_in[1] = GetWord16(P1_DATA_REG);
#endif
#ifdef P2_HAS_INPUT					
		kbd_gpio_in[2] = GetWord16(P2_DATA_REG);
#endif
#ifdef P3_HAS_INPUT					
		kbd_gpio_in[3] = GetWord16(P3_DATA_REG);
#endif

		// Step 4. Turn previous output to "high-Z" (input)
		if (kbd_out_bitmasks[prev_line]) {
            SetWord16(kbd_output_mode_regs[prev_line] + P0_DATA_REG, 0x000);    // mode gpio input highz
        }

		// Process input data
		// we're waiting for the signals to stabilize for the the next readout
		// so this is a great time to spend some cycles to process the previous readout
			
		// fill in the scanword bits (we have plenty of time to burn these cycles
		for (j = KBD_NR_COLUMN_INPUTS - 1; j >= 0; --j) {
			const uint8_t input_port = kbd_input_ports[j];
			scanword = (scanword << 1) | ((kbd_gpio_in[input_port >> 4] >> (input_port & 0x0F)) & 1);
		}
        
        // if key press and there's no debouncing counter set for the key => new key press is detected!
        if ( ((~scanword & scanmask) != kbd_bounce_rows[prev_line]) ) {  // if the button is not being debounced
            kbd_new_key_detected = true;
        }
        // update scan status
		kbd_new_scandata[prev_line] = scanword;
	} 

	// Step 2. Pull next "used" output low hard
	if (i < KBD_NR_ROW_OUTPUTS && kbd_out_bitmasks[i]) {
        SetWord16(kbd_output_reset_data_regs[i] + P0_DATA_REG, kbd_out_bitmasks[i]);  // level 0
        SetWord16(kbd_output_mode_regs[i] + P0_DATA_REG, 0x300);                      // mode gpio output
	}

	*row = i + 1;
	
    // 6. processing of the results has to be done just after the last row has been scanned
    // no need to have a separate state for this! idle time follows the completion of the processing!
    if (i == KBD_NR_ROW_OUTPUTS) {
        // a. process scan results
        kbd_scan_process_scandata();
        
        // b. Update debouncing counters
        kbd_bounce_active = 0;
        for (j = KBD_DEBOUNCE_BUFFER_SIZE-1; j >= 0 ; j--) {
            if (0xFFFF != kbd_bounce_intersections[j]) {
                kbd_bounce_active = 1;
                
                if (kbd_bounce_counters[j].cnt) {
                    --kbd_bounce_counters[j].cnt;
                }
            }
        }
        
        // update the global debouncing counter as well
        if (kbd_global_deb_cnt) {
            kbd_global_deb_cnt--;
            kbd_bounce_active++;        // 1 if no keys are being debounced/pressed, 2 if there are other keys being debounced/pressed
        }
                
        // d. Program Keyboard Controller to scan the inactive rows
        
        //All outputs are in high-Z now. Enable Keyboard Controller
        if (!kbd_cntrl_active) {
            kbd_scan_enable_kbd_irq();
        } else {
            ASSERT_ERROR(0);
        }
        
        //Find which rows have not any key being pressed, drive them low and calculate for how long the Keyboard Controller will be active
        for (j = 0; j < KBD_NR_ROW_OUTPUTS; ++j) {
            if ( (!kbd_active_row[j]) && (kbd_new_scandata[j] == scanmask) ) {  //This line is "used" and has no pressed keys
                // Pull output low hard
                SetWord16(kbd_output_reset_data_regs[j] + P0_DATA_REG, kbd_out_bitmasks[j]);  // level 0
                SetWord16(kbd_output_mode_regs[j] + P0_DATA_REG, 0x300);                      // mode gpio output
            }
        }
            
        ASSERT_WARNING(scan_cycle_time > 0);
        
        if (scan_cycle_time <= 0) {
            scan_cycle_time = 20;
        }
        if (GetWord16(CLK_CTRL_REG) & RUNNING_AT_RC16M) {                    // drift is around +10% of the elapsed time
            scan_cycle_time = scan_cycle_time - (scan_cycle_time_last >> 4) - (scan_cycle_time_last >> 5);
        }
        ASSERT_WARNING(scan_cycle_time > 0);
        
        if (scan_cycle_time <= 0) {
            scan_cycle_time = 20;
        }
        scan_cycle_time += GetWord32(0xE000E018);
        systick_stop();
        systick_start(scan_cycle_time, 1);
    }
    
	return (i == KBD_NR_ROW_OUTPUTS); 
}


/**
 ****************************************************************************************
 * \brief Updates the value of the scan cycle time and starts SysTick.
 ****************************************************************************************
 */
static void kbd_scan_update_scan_times(void)
{
    if (kbd_params.full_scan_cycle_in_us < kbd_params.partial_scan_cycle_in_us) {
        // Wrong scanning parameters!
        ASSERT_ERROR(0);
    }
    
    // Re-init scan period time
    if (HAS_ALTERNATIVE_SCAN_TIMES) {
        if (full_scan) {
            scan_cycle_time = kbd_params.full_scan_cycle_in_us - kbd_params.row_scan_time_in_us;
        } else {
            scan_cycle_time = kbd_params.partial_scan_cycle_in_us - kbd_params.row_scan_time_in_us;                
        }
    } else {
        scan_cycle_time = kbd_params.full_scan_cycle_in_us - kbd_params.row_scan_time_in_us;
    }

    scan_cycle_time_last = scan_cycle_time;
    systick_start(kbd_params.row_scan_time_in_us, 1);
}


/**
 ****************************************************************************************
 * \brief Update full_scan status according to whether an interrupt was
 *        reported from the keyboard controller or not. Decide whether the next state
 *        will be KEY_SCANNING (because there's keyboard activity) or INACTIVE.
 *
 * \return true, if scanning will continue
 *         false, if scanning stops and IDLE is the next state
 ****************************************************************************************
 */
static bool kbd_scan_update_status(void)
{
    bool ret;
    
    // Stop SysTick
    systick_stop();

    GLOBAL_INT_DISABLE();
    if (kbd_cntrl_active) {
        // Stop KBD Controller
        kbd_cntrl_active = false;
        SetWord16(KBRD_IRQ_IN_SEL2_REG, 0);
        SetWord16(KBRD_IRQ_IN_SEL1_REG, 0);
        SetWord16(KBRD_IRQ_IN_SEL0_REG, 0x4000);
        NVIC_DisableIRQ(KEYBRD_IRQn);
    } 
    GLOBAL_INT_RESTORE();
    
    if (full_scan) {
        if (!kbd_new_key_detected) {
            // set a global debounce counter. until it reaches zero or a new key is detected
            // (whatever happens first) we remain in full scan mode and keep on scanning!
            full_scan = true;
            
            if (kbd_global_deb_cnt == 0) {
                kbd_bounce_active = 1;
                kbd_global_deb_cnt = DEBOUNCE_COUNTER_PRESS + 1; 
            } else { 
                if (kbd_global_deb_cnt == 1) {
                    kbd_bounce_active--;
                    kbd_global_deb_cnt = 0;
                    full_scan = false;
                }
            }
        } else {
            // reset global debounce counter
            kbd_global_deb_cnt = 0;
            
            // disable full scan unless a keyboard interrupt occurred during the last partial 
            // scan. in this case do a full scan next time!
            full_scan = next_is_full_scan;
        }
    } else {
        full_scan = next_is_full_scan;
    }

    kbd_new_key_detected = false;
    
    // Start SysTick
    kbd_scan_update_scan_times();
    
    // Turn all outputs to 'high-Z' so as to be ready for the next scan and to preserve some power in the meantime
    for (int i = 0; i < KBD_NR_ROW_OUTPUTS; ++i) {
        kbd_scan_set_row_to_input_highz(i);
    }
    // if a keyboard interrupt occurred during the last partial scan do a full scan next time!
    if (next_is_full_scan) {
        ret = true;
        next_is_full_scan = false;
    } else {
        if (kbd_bounce_active) { // debouncing (press or release) or a key is being pressed
            ret = true;          // scan again.
        } else {
            if (kbd_params.scan_always_active) {
                ret = true;
            } else {
                systick_stop();
                systick_hit = false;
                ret = false;
            }
        }
    }
    return ret;
}


/*
 * SysTick handler
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * \brief ISR of the SysTick IRQ - Forces execution of the state machine once every
 *        SysTick interval
 ****************************************************************************************
 */
static void port_kbd_scan_systick_Handler(void) 
{ 
	ASSERT_ERROR(kbd_membrane_status != 0);
    
    if (current_scan_state != KEY_SCAN_INACTIVE)
        systick_hit = true;
}

/**
 ****************************************************************************************
 * \brief Activates key scanning hardware and state machine (by enabling the SysTick IRQ).
 ****************************************************************************************
 */
static void port_kbd_scan_start(void)
{
    port_kbd_scan_init_vars();
    
    dbg_puts(DBG_KBD_LVL, "KBD SW scan active\r\n");

    full_scan = true;                   // Force a full key scan
    
    systick_register_callback(port_kbd_scan_systick_Handler);
    kbd_scan_update_scan_times();                // Start SysTick
    GLOBAL_INT_DISABLE();
    kbd_scan_membrane_output_wakeup();       // Set outputs to 'high-Z' to enable SW scanning

    NVIC_SetPriority(SysTick_IRQn, 3);         
    NVIC_EnableIRQ(SysTick_IRQn);
    GLOBAL_INT_RESTORE();
}

/**
 ****************************************************************************************
 * \brief Programs the Wakeup Timer to scan any key press and wake-up the system
 *        (optionally after DEBOUNCE_TIME_PRESS msec)
 ****************************************************************************************
 */
static void kbd_scan_enable_wakeup_irq(void)
{
#ifdef HAS_POWERUP_BUTTON	
    if (force_power_off == 1) {
        port_wkup_enable_irq(WKUP_KBD_POWER_BUTTON_CHANNEL, KBD_POWER_BUTTON_MASK);
    } else 
#endif	
	{
        port_wkup_enable_irq(WKUP_KBD_CHANNEL, KBD_WKUP_MASK);
    }
}

void port_kbd_scan_stop(void)
{
    arch_restore_sleep_mode();
    kbd_scan_timer_stop(); 
    current_scan_state = KEY_SCAN_IDLE;
    port_kbd_init_scan_gpios();
    kbd_scan_enable_wakeup_irq();       // Setup the wakeup irq after setting up the GPIOs to avoid getting an IRQ
}


/*
 * Keyboard Controller
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * \brief ISR of the Keyboard Controller IRQ. It clears the interrupt and triggers
 *        a full key scan.
 ****************************************************************************************
 */
void KEYBRD_Handler(void)
{
    // Clear it first
    SetWord16(KBRD_IRQ_IN_SEL0_REG, 0); 
    SetWord16(KBRD_IRQ_IN_SEL1_REG, 0);
    SetWord16(GPIO_RESET_IRQ_REG, 0x20); 

    // No more KEYBRD interrupts from now on
    NVIC_DisableIRQ(KEYBRD_IRQn); 

    // We do not know which row has the key. Rescan all rows!
    next_is_full_scan = true;
        
    kbd_cntrl_active = false;
}


/*
 * Wakeup timer
 ****************************************************************************************
 */

void port_kbd_scan_wakeup_handler(void)
{	
	/*
	* Init System Power Domain blocks: GPIO, WD Timer, Sys Timer, etc.
	* Power up and init Peripheral Power Domain blocks,
	* and finally release the pad latches.
	*/
#ifdef HAS_CONNECTION_FSM     
    if(kbd_params.delayed_wakeup) {
        /*
        * Notify HID Application to start delay monitoring
        */
        if (app_con_fsm_get_state() == IDLE_ST) {
            ASSERT_WARNING(monitor_kbd_delayed_start_st != MONITOR_IDLE);
            
            if (monitor_kbd_delayed_start_st == MONITOR_PRESS) {    // press
                trigger_kbd_delayed_start_st = PRESSED_TRIGGER;
            } else {                                                // release
                trigger_kbd_delayed_start_st = RELEASED_TRIGGER;
            }
            monitor_kbd_delayed_start_st = MONITOR_IDLE;
        }
        else {
            trigger_kbd_delayed_start_st = NO_TRIGGER;
            wkup_hit = true;
        }
    } else 
#endif    
    {
        /*
        * Notify HID Application to start scanning
        */
        wkup_hit = true;
    }
#ifdef HAS_POWERUP_BUTTON      
    if(force_power_off == 1) {        
        force_power_off = 2;
    }   
#endif    
}

void port_kbd_scan_enable_delayed_scanning(bool press)
{
    if (kbd_params.delayed_wakeup) {
        int i;
        
        dbg_puts(DBG_KBD_LVL, "KBD active (HW scan)\r\n");
        
        // Read current status. If we are looking for release and column is high then report it immediately!
        ASSERT_WARNING(kbd_out_bitmasks[POWER_BUTTON_ROW]);
        
        kbd_scan_membrane_input_setup();                                     // Setup the "inputs" of the key-matrix
        for (i = 0; i < KBD_NR_ROW_OUTPUTS; ++i) {
            kbd_scan_set_row_to_input_highz(i);                                  // Set all rows to 'high-Z'
        }
        kbd_scan_enable_delayed_wakeup_irq(press);                       // Setup the wakeup irq
        
        kbd_scan_set_row_to_low(POWER_BUTTON_ROW);                        // Setup the "output" of the key-matrix

        GLOBAL_INT_DISABLE();
        if (!press) {
            uint16_t const *base = (uint16_t *)P0_DATA_REG;
        
            int port = kbd_input_ports[POWER_BUTTON_COLUMN];
            const int databit = (port & 0xF);

            ASSERT_WARNING(port < 0x40);
                
            if (port >= 0x30) {
                port += 0x10; // fix for the mapping of port 3 regs!
            }
            uint16_t const *data_reg = &base[port & 0xF0];

            uint16_t status = *data_reg;
            
            if ( ( (status & (1 << databit)) != 0) && (GetWord16(WKUP_COUNTER_REG) == 0) ) {
                // Emulate interrupt hit
                monitor_kbd_delayed_start_st = MONITOR_IDLE;
                trigger_kbd_delayed_start_st = RELEASED_TRIGGER;
                
                port_wkup_disable_irq(WKUP_KBD_CHANNEL);
           }
        }
        GLOBAL_INT_RESTORE();
    }
}

/**
 ****************************************************************************************
 * \brief Sets up the hardware for key scanning. Normally, the keyboard scanning is
 *        active only when connected or disconnected but bonded
 ****************************************************************************************
 */
static void kbd_scan_enable_scanning(void)
{
    arch_restore_sleep_mode();
    dbg_puts(DBG_KBD_LVL, "KBD active (HW scan)\r\n");

    kbd_scan_membrane_input_setup();    // Setup the "inputs" of the key-matrix
    kbd_scan_enable_wakeup_irq();       // Setup the wakeup irq

#ifdef HAS_POWERUP_BUTTON    
    if (force_power_off == 1) {
        for (int i = 0; i < KBD_NR_ROW_OUTPUTS; ++i) {
            set_row_to_input_highz(i);
        }
        set_row_to_low(POWER_BUTTON_ROW);    
    } else 
#endif	
	{
        kbd_scan_membrane_output_sleep();    // Setup the "outputs" of the key-matrix
    }
}

void port_kbd_scan_disable_scanning(void)
{
    arch_restore_sleep_mode();

	//Deactivate Wakeup Timer interrupt
    port_wkup_disable_irq(WKUP_KBD_CHANNEL);

    // Clear status
    wkup_hit = false;

    // Set outputs to 'high-Z' to disable current flow
    kbd_scan_membrane_output_wakeup();
}

void port_kbd_init_scan_gpios(void)
{
    if (!kbd_params.delayed_wakeup) {
        kbd_scan_membrane_input_setup();             // setup inputs (pull-up)        
#ifdef HAS_POWERUP_BUTTON    
        if (force_power_off == 1) {
            for (int i = 0; i < KBD_NR_ROW_OUTPUTS; ++i) {
                set_row_to_input_highz(i);
            }
            set_row_to_low(POWER_BUTTON_ROW);    
        }
        else 
#endif	
        {
            kbd_scan_membrane_output_sleep();    // setup outputs 'low'. they are latched but have to be reinitialized
        }        
    }
    else {
        kbd_scan_membrane_input_setup();             // setup inputs (pull-up)
#ifdef HAS_CONNECTION_FSM
        if (app_con_fsm_get_state() != IDLE_ST) {
            kbd_scan_membrane_output_sleep();        // setup outputs 'low'. they are latched but have to be reinitialized
        } else 
#endif        
        {
            // Not active
            for (int i = 0; i < KBD_NR_ROW_OUTPUTS; ++i) {
                kbd_scan_set_row_to_input_highz(i);          // Set all rows to 'high-Z'
            }
            kbd_scan_set_row_to_low(POWER_BUTTON_ROW);// Setup the "output" of the key-matrix
        }
    }
}

void port_kbd_scan_delayed_start_proc(void)
{
    if (kbd_params.delayed_wakeup)
    {
        if ( ( (trigger_kbd_delayed_start_st == PRESSED_TRIGGER) || (trigger_kbd_delayed_start_st == RELEASED_TRIGGER) ) 
            && !ble_is_woken_up)
        {
            if (trigger_kbd_delayed_start_st == PRESSED_TRIGGER)
                trigger_kbd_delayed_start_st = PRESSED_TRIGGER_SYNCED;
            else
                trigger_kbd_delayed_start_st = RELEASED_TRIGGER_SYNCED;
            
            // Put a message to the queue for the BLE to handle. The message
            // is handled by the Kbd app and starts or stops the monitor timer.
            ke_msg_id_t kbd_scan_msg = app_easy_msg_set(port_kbd_scan_msg_handler);
            ASSERT_WARNING(kbd_scan_msg != 0xFFFF);
            if(kbd_scan_msg != 0xFFFF) {
                ke_msg_send_basic(kbd_scan_msg, TASK_APP, 0);
            }
            arch_ble_force_wakeup();
            arch_ble_ext_wakeup_off();

            ble_is_woken_up = true;
        }
    }
}
   
void port_kbd_scan_timer_handler(void)
{
	dbg_puts(DBG_KBD_LVL, "HID timer exp\r\n");
    	
    // Timer elapsed! 
    if (kbd_params.delayed_wakeup) {
        ASSERT_ERROR(monitor_kbd_delayed_start_st == MONITOR_RELEASE);
        
        GLOBAL_INT_DISABLE();
        // Stop WKUP
        SetBits16(WKUP_CTRL_REG, WKUP_ENABLE_IRQ, 0); //No more interrupts of this kind
        GLOBAL_INT_RESTORE();
        // Any WKUP int would hit here
        
        trigger_kbd_delayed_start_st = NO_TRIGGER;
        
        wkup_hit = true;
    }
}

//bool main_fsm_changed = true;   /* use if the key scanning needs to be disabled */

void port_kbd_scan_fsm_update(void)
{
    switch(current_scan_state) {
    case KEY_SCAN_INACTIVE:
        if (DEVELOPMENT_DEBUG && (systick_hit || wkup_hit)) {
            ASSERT_ERROR(0);
        }

        if (kbd_params.delayed_wakeup) {
            port_kbd_scan_enable_delayed_scanning(true);
        } else {
            kbd_scan_enable_scanning();
        }
        current_scan_state = KEY_SCAN_IDLE;     // Transition from KEY_SCAN_INACTIVE -> KEY_SCAN_IDLE
        break;
    case KEY_SCAN_IDLE:
        if (DEVELOPMENT_DEBUG && systick_hit) {
            ASSERT_ERROR(0);
        }

        if (wkup_hit) {
        //	dbg_puts(DBG_KBD_LVL, "KEY_SCAN_IDLE -> Wakeup! -> KEY_SCANNING\r\n");
            scanning_substate = 0;
            GLOBAL_INT_DISABLE();
            current_scan_state = KEY_SCANNING;              // Transition from KEY_SCAN_IDLE -> KEY_SCANNING
            app_kbd_init_vars();
            arch_force_active_mode();    										//  Prevent ble from sleeping to avoid delay during wake-up
            port_kbd_scan_start();    											// start SW scanning
            wkup_hit = false;                                   
            GLOBAL_INT_RESTORE();
        }
        break;
    case KEY_STATUS_UPD:
        if (DEVELOPMENT_DEBUG && wkup_hit) {
            ASSERT_ERROR(0);
        }

        // check if there are key events in the buffer
        if (app_kbd_passcode_mode() == true) { // passcode mode
            app_kbd_prepare_passcode();
        }
        // The handling of 'Paused' and 'Normal' modes has been moved to a separate function that is called synchronously

        if (systick_hit) {
            systick_hit = false;
            if (kbd_scan_update_status()) {
                scanning_substate = 0;
                current_scan_state = KEY_SCANNING;          // Transition from KEY_STATUS_UPD -> KEY_SCANNING
                // scan once to save time!
                if (kbd_scan_matrix(&scanning_substate)) {
                    current_scan_state = KEY_STATUS_UPD;    // Transition from KEY_SCANNING -> KEY_STATUS_UPD
                }
            } else {
                GLOBAL_INT_DISABLE();
                kbd_scan_enable_scanning();
                current_scan_state = KEY_SCAN_IDLE;         // Transition from KEY_STATUS_UPD -> KEY_SCAN_IDLE
                GLOBAL_INT_RESTORE();
            }
        }
        break;
    case KEY_SCANNING:
        if (DEVELOPMENT_DEBUG && wkup_hit) {
            ASSERT_ERROR(0);
        }

        if (systick_hit) {
            systick_hit = false;
            if (kbd_scan_matrix(&scanning_substate)) {
                current_scan_state = KEY_STATUS_UPD;        // Transition from KEY_SCANNING -> KEY_STATUS_UPD
            } // else the state remains unchanged and next time we will scan the next row
        }
        break;
    default:
            break;
    }
}

uint16_t port_kbd_scan_get_new_scan_data(uint8_t output)
{
    return kbd_new_scandata[output];
}

uint16_t port_kbd_scan_get_scan_data(uint8_t output)
{
    return kbd_scandata[output];
}

bool port_kbd_scan_is_idle(void)
{
    return current_scan_state == KEY_SCAN_IDLE;
}

bool port_kbd_scan_is_active(void)
{
    return (current_scan_state == KEY_STATUS_UPD) || (current_scan_state == KEY_SCANNING);
}

void port_kbd_scan_before_sleep(void)
{
    if (wkup_hit) { // make sure we do not miss any wake up interrupts!
        port_kbd_scan_fsm_update();
    }
    if (kbd_params.delayed_wakeup) {
        port_kbd_scan_delayed_start_proc();
    }    
}

#endif // HAS_KBD

/**
 * \}
 * \}
 * \}
 */
