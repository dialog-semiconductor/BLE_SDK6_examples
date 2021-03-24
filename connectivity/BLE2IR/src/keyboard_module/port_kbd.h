/**
 ****************************************************************************************
 *
 * \file port_kbd_scan.h
 *
 * \brief Keyboard scanner implementation.
 *
 * Copyright (C) 2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information  
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com>
 *
 ****************************************************************************************
 */

 /**
 ****************************************************************************************
 * \addtogroup APP_UTILS
 * \{
 * \addtogroup KEYBOARD
 * \{
 * \addtogroup PORT_KEYBOARD
 * \brief Keyboard scanner implementation.
 * \{
 ****************************************************************************************	 
 */
 
#ifndef PORT_KBD_SCAN_H_
#define PORT_KBD_SCAN_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwble_config.h"
#include "port_platform.h"
#include <app_kbd_scan_matrix.h>
#include "port_kbd_macros.h"

#include <stdint.h>          // standard integer definition
#include <co_bt.h>
#include "att.h"
#include "gap.h"
#include "ke_msg.h"                 // kernel message
#include "gpio.h"

/*
 * Code switches
 ****************************************************************************************
 */

#ifdef ALTERNATIVE_SCAN_TIMES_ON
    #define HAS_ALTERNATIVE_SCAN_TIMES              1
#else
    #define HAS_ALTERNATIVE_SCAN_TIMES              0
#endif

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define KBD_SET_COLUMN_ENTRY(x) ((COLUMN_INPUT_##x##_PORT << 4) | COLUMN_INPUT_##x##_PIN)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define KBD_SET_ROW_ENTRY(x)    ((ROW_OUTPUT_##x##_PORT << 4)   | ROW_OUTPUT_##x##_PIN)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define _KBD_GET_COLUMN_PORT(x)  ((GPIO_PORT)COLUMN_INPUT_##x##_PORT)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define _KBD_GET_COLUMN_PIN(x)   ((GPIO_PIN)COLUMN_INPUT_##x##_PIN)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define _KBD_GET_COLUMN_WKUP_MASK(x) (1 << COLUMN_INPUT_##x##_PIN)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define _KBD_GET_ROW_PORT(x)     ((GPIO_PORT)ROW_OUTPUT_##x##_PORT)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define _KBD_GET_ROW_PIN(x)      ((GPIO_PIN)ROW_OUTPUT_##x##_PIN)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define KBD_GET_COLUMN_PORT(x)       _KBD_GET_COLUMN_PORT(x)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define KBD_GET_COLUMN_PIN(x)        _KBD_GET_COLUMN_PIN(x)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define KBD_GET_COLUMN_WKUP_MASK(x)  _KBD_GET_COLUMN_WKUP_MASK(x)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define KBD_GET_ROW_PORT(x)    _KBD_GET_ROW_PORT(x)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define KBD_GET_ROW_PIN(x)     _KBD_GET_ROW_PIN(x)

#define PORT_UNUSED 0xFF
#define PIN_UNUSED  0xFF

#if (!defined(ROW_OUTPUT_0_PORT) || !defined(ROW_OUTPUT_0_PIN))
    #define ROW_OUTPUT_0_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_0_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_1_PORT) || !defined(ROW_OUTPUT_1_PIN))
    #define ROW_OUTPUT_1_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_1_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_2_PORT) || !defined(ROW_OUTPUT_2_PIN))
    #define ROW_OUTPUT_2_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_2_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_3_PORT) || !defined(ROW_OUTPUT_3_PIN))
    #define ROW_OUTPUT_3_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_3_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_4_PORT) || !defined(ROW_OUTPUT_4_PIN))
    #define ROW_OUTPUT_4_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_4_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_5_PORT) || !defined(ROW_OUTPUT_5_PIN))
    #define ROW_OUTPUT_5_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_5_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_6_PORT) || !defined(ROW_OUTPUT_6_PIN))
    #define ROW_OUTPUT_6_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_6_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_7_PORT) || !defined(ROW_OUTPUT_7_PIN))
    #define ROW_OUTPUT_7_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_7_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_8_PORT) || !defined(ROW_OUTPUT_8_PIN))
    #define ROW_OUTPUT_8_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_8_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_9_PORT) || !defined(ROW_OUTPUT_9_PIN))
    #define ROW_OUTPUT_9_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_9_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_10_PORT) || !defined(ROW_OUTPUT_10_PIN))
    #define ROW_OUTPUT_10_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_10_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_11_PORT) || !defined(ROW_OUTPUT_11_PIN))
    #define ROW_OUTPUT_11_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_11_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_12_PORT) || !defined(ROW_OUTPUT_12_PIN))
    #define ROW_OUTPUT_12_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_12_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_13_PORT) || !defined(ROW_OUTPUT_13_PIN))
    #define ROW_OUTPUT_13_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_13_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_14_PORT) || !defined(ROW_OUTPUT_14_PIN))
    #define ROW_OUTPUT_14_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_14_PIN  (PIN_UNUSED)
#endif
#if (!defined(ROW_OUTPUT_15_PORT) || !defined(ROW_OUTPUT_15_PIN))
    #define ROW_OUTPUT_15_PORT (PORT_UNUSED)
    #define ROW_OUTPUT_15_PIN  (PIN_UNUSED)
#endif

#if (!defined(COLUMN_INPUT_0_PORT) || !defined(COLUMN_INPUT_0_PIN))
    #define COLUMN_INPUT_0_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_0_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_1_PORT) || !defined(COLUMN_INPUT_1_PIN))
    #define COLUMN_INPUT_1_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_1_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_2_PORT) || !defined(COLUMN_INPUT_2_PIN))
    #define COLUMN_INPUT_2_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_2_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_3_PORT) || !defined(COLUMN_INPUT_3_PIN))
    #define COLUMN_INPUT_3_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_3_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_4_PORT) || !defined(COLUMN_INPUT_4_PIN))
    #define COLUMN_INPUT_4_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_4_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_5_PORT) || !defined(COLUMN_INPUT_5_PIN))
    #define COLUMN_INPUT_5_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_5_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_6_PORT) || !defined(COLUMN_INPUT_6_PIN))
    #define COLUMN_INPUT_6_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_6_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_7_PORT) || !defined(COLUMN_INPUT_7_PIN))
    #define COLUMN_INPUT_7_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_7_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_8_PORT) || !defined(COLUMN_INPUT_8_PIN))
    #define COLUMN_INPUT_8_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_8_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_9_PORT) || !defined(COLUMN_INPUT_9_PIN))
    #define COLUMN_INPUT_9_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_9_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_10_PORT) || !defined(COLUMN_INPUT_10_PIN))
    #define COLUMN_INPUT_10_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_10_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_11_PORT) || !defined(COLUMN_INPUT_11_PIN))
    #define COLUMN_INPUT_11_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_11_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_12_PORT) || !defined(COLUMN_INPUT_12_PIN))
    #define COLUMN_INPUT_12_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_12_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_13_PORT) || !defined(COLUMN_INPUT_13_PIN))
    #define COLUMN_INPUT_13_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_13_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_14_PORT) || !defined(COLUMN_INPUT_14_PIN))
    #define COLUMN_INPUT_14_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_14_PIN  (PIN_UNUSED)
#endif
#if (!defined(COLUMN_INPUT_15_PORT) || !defined(COLUMN_INPUT_15_PIN))
    #define COLUMN_INPUT_15_PORT (PORT_UNUSED)
    #define COLUMN_INPUT_15_PIN  (PIN_UNUSED)
#endif

// Masks for the initialization of the WKUP controller
#define WKUP_MASK_P0    (       SET_WKUP_MASK_FROM_COLUMN(0, 0)  | SET_WKUP_MASK_FROM_COLUMN(0, 1)  | SET_WKUP_MASK_FROM_COLUMN(0, 2)  | SET_WKUP_MASK_FROM_COLUMN(0, 3)    \
                              | SET_WKUP_MASK_FROM_COLUMN(0, 4)  | SET_WKUP_MASK_FROM_COLUMN(0, 5)  | SET_WKUP_MASK_FROM_COLUMN(0, 6)  | SET_WKUP_MASK_FROM_COLUMN(0, 7)    \
                              | SET_WKUP_MASK_FROM_COLUMN(0, 8)  | SET_WKUP_MASK_FROM_COLUMN(0, 9)  | SET_WKUP_MASK_FROM_COLUMN(0, 10) | SET_WKUP_MASK_FROM_COLUMN(0, 11)   \
                              | SET_WKUP_MASK_FROM_COLUMN(0, 12) | SET_WKUP_MASK_FROM_COLUMN(0, 13) | SET_WKUP_MASK_FROM_COLUMN(0, 14) | SET_WKUP_MASK_FROM_COLUMN(0, 15))  
                                                                                                                                                                            
#define WKUP_MASK_P1    (       SET_WKUP_MASK_FROM_COLUMN(1, 0)  | SET_WKUP_MASK_FROM_COLUMN(1, 1)  | SET_WKUP_MASK_FROM_COLUMN(1, 2)  | SET_WKUP_MASK_FROM_COLUMN(1, 3)    \
                              | SET_WKUP_MASK_FROM_COLUMN(1, 4)  | SET_WKUP_MASK_FROM_COLUMN(1, 5)  | SET_WKUP_MASK_FROM_COLUMN(1, 6)  | SET_WKUP_MASK_FROM_COLUMN(1, 7)    \
                              | SET_WKUP_MASK_FROM_COLUMN(1, 8)  | SET_WKUP_MASK_FROM_COLUMN(1, 9)  | SET_WKUP_MASK_FROM_COLUMN(1, 10) | SET_WKUP_MASK_FROM_COLUMN(1, 11)   \
                              | SET_WKUP_MASK_FROM_COLUMN(1, 12) | SET_WKUP_MASK_FROM_COLUMN(1, 13) | SET_WKUP_MASK_FROM_COLUMN(1, 14) | SET_WKUP_MASK_FROM_COLUMN(1, 15))  
                                                                                                                                                                            
#define WKUP_MASK_P2    (       SET_WKUP_MASK_FROM_COLUMN(2, 0)  | SET_WKUP_MASK_FROM_COLUMN(2, 1)  | SET_WKUP_MASK_FROM_COLUMN(2, 2)  | SET_WKUP_MASK_FROM_COLUMN(2, 3)    \
                              | SET_WKUP_MASK_FROM_COLUMN(2, 4)  | SET_WKUP_MASK_FROM_COLUMN(2, 5)  | SET_WKUP_MASK_FROM_COLUMN(2, 6)  | SET_WKUP_MASK_FROM_COLUMN(2, 7)    \
                              | SET_WKUP_MASK_FROM_COLUMN(2, 8)  | SET_WKUP_MASK_FROM_COLUMN(2, 9)  | SET_WKUP_MASK_FROM_COLUMN(2, 10) | SET_WKUP_MASK_FROM_COLUMN(2, 11)   \
                              | SET_WKUP_MASK_FROM_COLUMN(2, 12) | SET_WKUP_MASK_FROM_COLUMN(2, 13) | SET_WKUP_MASK_FROM_COLUMN(2, 14) | SET_WKUP_MASK_FROM_COLUMN(2, 15))  
                                                                                                                                                                            
#define WKUP_MASK_P3    (       SET_WKUP_MASK_FROM_COLUMN(3, 0)  | SET_WKUP_MASK_FROM_COLUMN(3, 1)  | SET_WKUP_MASK_FROM_COLUMN(3, 2)  | SET_WKUP_MASK_FROM_COLUMN(3, 3)    \
                              | SET_WKUP_MASK_FROM_COLUMN(3, 4)  | SET_WKUP_MASK_FROM_COLUMN(3, 5)  | SET_WKUP_MASK_FROM_COLUMN(3, 6)  | SET_WKUP_MASK_FROM_COLUMN(3, 7)    \
                              | SET_WKUP_MASK_FROM_COLUMN(3, 8)  | SET_WKUP_MASK_FROM_COLUMN(3, 9)  | SET_WKUP_MASK_FROM_COLUMN(3, 10) | SET_WKUP_MASK_FROM_COLUMN(3, 11)   \
                              | SET_WKUP_MASK_FROM_COLUMN(3, 12) | SET_WKUP_MASK_FROM_COLUMN(3, 13) | SET_WKUP_MASK_FROM_COLUMN(3, 14) | SET_WKUP_MASK_FROM_COLUMN(3, 15))

#define KBD_WKUP_MASK   WKUP_MASK( WKUP_MASK_P0, WKUP_MASK_P1, WKUP_MASK_P2, WKUP_MASK_P3)

#define CHECK_WKUP_CTRL_MASKS (  (WKUP_MASK_P0 & CHECK_WKUP_MASK0_FROM_COLUMN(KBD_NR_COLUMN_INPUTS-1))   \
                               | (WKUP_MASK_P1 & CHECK_WKUP_MASK1_FROM_COLUMN(KBD_NR_COLUMN_INPUTS-1))   \
                               | (WKUP_MASK_P2 & CHECK_WKUP_MASK2_FROM_COLUMN(KBD_NR_COLUMN_INPUTS-1))   \
                               | (WKUP_MASK_P3 & CHECK_WKUP_MASK3_FROM_COLUMN(KBD_NR_COLUMN_INPUTS-1)) )

#if (CHECK_WKUP_CTRL_MASKS == 0)
#error "WKUP controller masks not defined properly!"
#endif

#define _SET_WKUP_MASK_FROM_COLUMN(m,x) SET_WKUP_MASK_FROM_COLUMN(m, x)

#ifdef HAS_POWERUP_BUTTON
    #define KBD_POWER_BUTTON_PIN  KBD_GET_COLUMN_PIN(POWER_BUTTON_COLUMN)
#endif

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   port
 * \param[in]   pin
 ****************************************************************************************
 */
#define KBD_RESERVE_GPIO(port, pin) if (port != PORT_UNUSED) RESERVE_GPIO(KBD_PIN, port, pin, PID_GPIO)

static const pin_type_t app_kbd_row_pins[] = {
#if (KBD_NR_ROW_OUTPUTS>0)
    {.port = ROW_OUTPUT_0_PORT, .pin = ROW_OUTPUT_0_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>1)
    {.port = ROW_OUTPUT_1_PORT, .pin = ROW_OUTPUT_1_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>2)
    {.port = ROW_OUTPUT_2_PORT, .pin = ROW_OUTPUT_2_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>3)
    {.port = ROW_OUTPUT_3_PORT, .pin = ROW_OUTPUT_3_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>4)
    {.port = ROW_OUTPUT_4_PORT, .pin = ROW_OUTPUT_4_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>5)
    {.port = ROW_OUTPUT_5_PORT, .pin = ROW_OUTPUT_5_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>6)
    {.port = ROW_OUTPUT_6_PORT, .pin = ROW_OUTPUT_6_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>7)
    {.port = ROW_OUTPUT_7_PORT, .pin = ROW_OUTPUT_7_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>8)
    {.port = ROW_OUTPUT_8_PORT, .pin = ROW_OUTPUT_8_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>9)
    {.port = ROW_OUTPUT_9_PORT, .pin = ROW_OUTPUT_9_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>10)
    {.port = ROW_OUTPUT_0_PORT, .pin = ROW_OUTPUT_10_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>11)
    {.port = ROW_OUTPUT_11_PORT, .pin = ROW_OUTPUT_11_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>12)
    {.port = ROW_OUTPUT_12_PORT, .pin = ROW_OUTPUT_12_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>13)
    {.port = ROW_OUTPUT_13_PORT, .pin = ROW_OUTPUT_13_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>14)
    {.port = ROW_OUTPUT_14_PORT, .pin = ROW_OUTPUT_14_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
#if (KBD_NR_ROW_OUTPUTS>15)
    {.port = ROW_OUTPUT_15_PORT, .pin = ROW_OUTPUT_15_PIN, .high = 0, .mode_function = OUTPUT | PID_GPIO },
#endif
};

/**
 ****************************************************************************************
 * \brief
 ****************************************************************************************
 */
__INLINE void declare_keyboard_gpios()
{
#if (KBD_NR_ROW_OUTPUTS>0)
    KBD_RESERVE_GPIO(ROW_OUTPUT_0_PORT, ROW_OUTPUT_0_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>1)
    KBD_RESERVE_GPIO(ROW_OUTPUT_1_PORT, ROW_OUTPUT_1_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>2)
    KBD_RESERVE_GPIO(ROW_OUTPUT_2_PORT, ROW_OUTPUT_2_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>3)
    KBD_RESERVE_GPIO(ROW_OUTPUT_3_PORT, ROW_OUTPUT_3_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>4)
    KBD_RESERVE_GPIO(ROW_OUTPUT_4_PORT, ROW_OUTPUT_4_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>5)
    KBD_RESERVE_GPIO(ROW_OUTPUT_5_PORT, ROW_OUTPUT_5_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>6)
    KBD_RESERVE_GPIO(ROW_OUTPUT_6_PORT, ROW_OUTPUT_6_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>7)
    KBD_RESERVE_GPIO(ROW_OUTPUT_7_PORT, ROW_OUTPUT_7_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>8)
    KBD_RESERVE_GPIO(ROW_OUTPUT_8_PORT, ROW_OUTPUT_8_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>9)
    KBD_RESERVE_GPIO(ROW_OUTPUT_9_PORT, ROW_OUTPUT_9_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>10)
    KBD_RESERVE_GPIO(ROW_OUTPUT_10_PORT, ROW_OUTPUT_10_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>11)
    KBD_RESERVE_GPIO(ROW_OUTPUT_11_PORT, ROW_OUTPUT_11_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>12)
    KBD_RESERVE_GPIO(ROW_OUTPUT_12_PORT, ROW_OUTPUT_12_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>13)
    KBD_RESERVE_GPIO(ROW_OUTPUT_13_PORT, ROW_OUTPUT_13_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>14)
    KBD_RESERVE_GPIO(ROW_OUTPUT_14_PORT, ROW_OUTPUT_14_PIN);
#endif
#if (KBD_NR_ROW_OUTPUTS>15)
    KBD_RESERVE_GPIO(ROW_OUTPUT_15_PORT, ROW_OUTPUT_15_PIN);
#endif
}

//// DELAYED WAKEUP
extern bool ble_is_woken_up;

#ifdef HAS_POWERUP_BUTTON
extern uint8_t force_power_off;
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
 
/**
 ****************************************************************************************
 * \brief Handles the initialization of all non-retained variables used for key scanning
 ****************************************************************************************
 */
void port_kbd_scan_init(void);

/**
 ****************************************************************************************
 * \brief
 ****************************************************************************************
 */
void port_kbd_scan_stop(void);

/**
 ****************************************************************************************
 * \brief Enables the delayed scanning
 *
 * \param[in]   press
 ****************************************************************************************
 */
void port_kbd_scan_enable_delayed_scanning(bool press);

/**
 ****************************************************************************************
 * \brief Disables the hardware for key scanning
 ****************************************************************************************
 */
void port_kbd_scan_disable_scanning(void);
    
/**
 ****************************************************************************************
 * \brief Re-initializes the scan matrix after wakeup
 ****************************************************************************************
 */
void port_kbd_init_scan_gpios(void);

/**
 ****************************************************************************************
 * \brief       
 ****************************************************************************************
 */
void port_kbd_scan_delayed_start_proc(void);
                                                                              
/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   output
 *
 * \return
 ****************************************************************************************
 */
uint16_t port_kbd_scan_get_new_scan_data(uint8_t output);

/**
 ****************************************************************************************
 * \brief       
 *
 * \param[in]   output
 *
 * \return
 ****************************************************************************************
 */
uint16_t port_kbd_scan_get_scan_data(uint8_t output);

/**
 ****************************************************************************************
 * \brief       
 *
 * \return
 ****************************************************************************************
 */
bool port_kbd_scan_is_idle(void);                                   

/**
 ****************************************************************************************
 * \brief       
 *
 * \return
 ****************************************************************************************
 */
bool port_kbd_scan_is_active(void);

/**
 ****************************************************************************************
 * \brief       
 ****************************************************************************************
 */
void port_kbd_scan_fsm_update(void);

/**
 ****************************************************************************************
 * \brief       
 ****************************************************************************************
 */
void port_kbd_scan_before_sleep(void);  

/**
 ****************************************************************************************
 * \brief   Handler of the keyboard Timer - Action depends on the app state
 ****************************************************************************************
 */
void port_kbd_scan_timer_handler(void);   

/**
 ****************************************************************************************
 * \brief Handler for the WKUP_QUADEC_IRQn interrupt
 ****************************************************************************************
 */
void port_kbd_scan_wakeup_handler(void);
                                   
#endif // PORT_KBD_SCAN_H_

/**
 * \}
 * \}
 * \}
 */
