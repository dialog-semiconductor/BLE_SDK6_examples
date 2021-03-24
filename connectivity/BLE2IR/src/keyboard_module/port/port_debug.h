/**
 ****************************************************************************************
 *
 * \file port_debug.h
 *
 * \brief Keyboard (HID) Debug header file.
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
 * \addtogroup DEBUG
 * \{
 * \addtogroup PORT_DEBUG
 * \{
 ****************************************************************************************	 
 */
 
#ifndef PORT_DEBUG_H_
#define PORT_DEBUG_H_

#include "arch_console.h"

#ifndef ASSERT_ERROR
#if (DEVELOPMENT_DEBUG == 0)
	#warning "KBD: ASSERT_ERROR() will cause a Soft-Reset!"
#endif

/**
 ****************************************************************************************
 * \brief Asserts an error if the input expression is false
 * \param[in] x the input expression passed as argument
 ****************************************************************************************
 */
#define ASSERT_ERROR(x)                                         \
    {                                                           \
        do {                                                    \
            if (DEVELOPMENT_DEBUG) {                            \
                if (!(x)) {                                     \
                        __asm("BKPT #0\n");                     \
                }                                               \
            }                                                   \
            else {                                              \
                if (!(x)) {                                     \
                        SetWord16(WATCHDOG_REG, 0x1);           \
                        SetWord16(RESET_FREEZE_REG, FRZ_WDOG);  \
                        while(1);                               \
                }                                               \
            }                                                   \
        } while (0);                                            \
    }

/**
 ****************************************************************************************
 * \brief Asserts a warning if the input expression is false
 * \param[in] x the input expression passed as argument
 ****************************************************************************************
 */
#define ASSERT_WARNING(x)                                       \
    {                                                           \
        do {                                                    \
            if (DEVELOPMENT_DEBUG) {                            \
                if (!(x)) {                                     \
                        __asm("BKPT #0\n");                     \
                }                                               \
            }                                                   \
        } while (0);                                            \
    }
    #endif

#define DBG_NONE            0x00        
#define DBG_APP_LVL         0x01
#define DBG_CONN_LVL        0x02
#define DBG_ADV_LVL         0x04
#define DBG_KBD_LVL         0x08
#define DBG_HID_LVL         0x10
#define DBG_TOUCH_LVL       0x20
#define DBG_PWR_MGR_LVL     0x40
#define DBG_MOT_LVL         0x80
#define DBG_ALL             0xFF        

#if defined (CFG_PRINTF)
    #if (DEBUG_LEVEL > DBG_NONE)
        #define dbg_printf(lvl, fmt, args...)                                   \
            {                                                                   \
                if ((lvl & DEBUG_LEVEL) == lvl)                                 \
                    arch_printf(fmt, args);                                     \
            }
            
        #define dbg_puts(lvl, s)                                                \
            {                                                                   \
                if ((lvl & DEBUG_LEVEL) == lvl)                                 \
                    arch_puts(s);                                               \
            }
        #define dbg_putc(ch)                                                    \
            {                                                                   \
                while((GetWord16(UART2_LSR_REG)&0x20)==0);                      \
                SetWord16(UART2_RBR_THR_DLL_REG,(ch));                          \
            }                                                                   \

    #else
        #define dbg_printf(lvl, fmt, args...) {}
        #define dbg_puts(lvl, s) {}
        #define dbg_putc(ch) {}
    #endif
#else
    #define dbg_printf(lvl, fmt, args...) {}
    #define dbg_puts(lvl, s) {}
    #define dbg_putc(ch) {}
#endif    


#endif // PORT_DEBUG_H_

/**
 * \}
 * \}
 * \}
 */
