/**
 ****************************************************************************************
 *
 * \file port_debug.h
 *
 * \brief Keyboard (HID) Debug header file.
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
