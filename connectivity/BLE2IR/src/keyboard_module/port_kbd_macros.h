/**
 ****************************************************************************************
 *
 * @file port_kbd_macros.h
 *
 * @brief Keyboard (HID) Application specific macros header file.
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
 * \addtogroup PORT_KEYBOARD
 * \brief HID (Keyboard) Platform macros.
 * \{
 ****************************************************************************************	 
 */ 
 
#ifndef APP_KBD_MACROS_H_
#define APP_KBD_MACROS_H_

#include "datasheet.h"


/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#if defined(CFG_PRINTF)
    #ifndef DISABLE_UART_RX    
        #define COL(x)  ( ( ((COLUMN_INPUT_##x##_PORT == UART2_RX_PORT) && (COLUMN_INPUT_##x##_PIN == UART2_RX_PIN)) || \
                            ((COLUMN_INPUT_##x##_PORT == UART2_TX_PORT) && (COLUMN_INPUT_##x##_PIN == UART2_TX_PIN)) ) ? \
                          (4 << 4) | (COLUMN_INPUT_##x##_PIN) : ( (COLUMN_INPUT_##x##_PORT << 4) | (COLUMN_INPUT_##x##_PIN) ) )
    #else               
        #define COL(x)  ( ( ((COLUMN_INPUT_##x##_PORT == UART2_TX_PORT) && (COLUMN_INPUT_##x##_PIN == UART2_TX_PIN)) ) ? \
                          (4 << 4) | (COLUMN_INPUT_##x##_PIN) : ( (COLUMN_INPUT_##x##_PORT << 4) | (COLUMN_INPUT_##x##_PIN) ) )
    #endif              
#else                   
    #define COL(x)      ( (COLUMN_INPUT_##x##_PORT << 4) | (COLUMN_INPUT_##x##_PIN) )
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
#if defined(CFG_PRINTF) 
    #define ROW(x)      ( ( ((ROW_OUTPUT_##x##_PORT == UART2_RX_PORT) && (ROW_OUTPUT_##x##_PIN == UART2_RX_PIN)) || \
                            ((ROW_OUTPUT_##x##_PORT == UART2_TX_PORT) && (ROW_OUTPUT_##x##_PIN == UART2_TX_PIN)) ) ? \
                          (4 << 4) | (ROW_OUTPUT_##x##_PIN) : ( (ROW_OUTPUT_##x##_PORT << 4) | (ROW_OUTPUT_##x##_PIN) ) )
#else                   
    #define ROW(x)      ( (ROW_OUTPUT_##x##_PORT << 4) | (ROW_OUTPUT_##x##_PIN) )
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
#define SET_OUTPUT_MODE_REG(x) ( ((P00_MODE_REG - P0_DATA_REG) + (ROW_OUTPUT_##x##_PORT == 3 ? \
                                 4 : ROW_OUTPUT_##x##_PORT) * 0x20) + (ROW_OUTPUT_##x##_PIN * 2) )    

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define SET_RESET_REG(x) ( ((P0_RESET_DATA_REG - P0_DATA_REG) + (ROW_OUTPUT_##x##_PORT == 3 ? 4 : ROW_OUTPUT_##x##_PORT) * 0x20) )    

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#if defined(CFG_PRINTF)
    #ifndef DISABLE_UART_RX  
        #define SET_BITMAP(x)   ( ( ((ROW_OUTPUT_##x##_PORT == UART2_RX_PORT) && (ROW_OUTPUT_##x##_PIN == UART2_RX_PIN)) || \
                                    ((ROW_OUTPUT_##x##_PORT == UART2_TX_PORT) && (ROW_OUTPUT_##x##_PIN == UART2_TX_PIN)) || \
                                    (ROW_OUTPUT_##x##_PORT >= 4) ) ? (0) : ( (1 << ROW_OUTPUT_##x##_PIN) ) )
    #else
        #define SET_BITMAP(x)   ( ( ((ROW_OUTPUT_##x##_PORT == UART2_TX_PORT) && (ROW_OUTPUT_##x##_PIN == UART2_TX_PIN)) || \
                                    (ROW_OUTPUT_##x##_PORT >= 4) ) ? (0) : ( (1 << ROW_OUTPUT_##x##_PIN) ) )
    #endif
#else
    #define SET_BITMAP(x)   ( (ROW_OUTPUT_##x##_PORT < 4) ? ( (1 << ROW_OUTPUT_##x##_PIN) ) : (0) )
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
#if defined(CFG_PRINTF)
    #ifndef DISABLE_UART_RX
        #define SET_INPUT_MODE_REG(x)   ( ( ((COLUMN_INPUT_##x##_PORT == UART2_RX_PORT) && (COLUMN_INPUT_##x##_PIN == UART2_RX_PIN)) || \
                                            ((COLUMN_INPUT_##x##_PORT == UART2_TX_PORT) && (COLUMN_INPUT_##x##_PIN == UART2_TX_PIN)) || \
                                            (COLUMN_INPUT_##x##_PORT >= 4) ) ? (0) : ( ((P00_MODE_REG - P0_DATA_REG) + (COLUMN_INPUT_##x##_PORT == 3 ? \
                                          4 : COLUMN_INPUT_##x##_PORT) * 0x20) + (COLUMN_INPUT_##x##_PIN * 2) ) )
    #else
        #define SET_INPUT_MODE_REG(x)   ( ( ((COLUMN_INPUT_##x##_PORT == UART2_TX_PORT) && (COLUMN_INPUT_##x##_PIN == UART2_TX_PIN)) || \
                                            (COLUMN_INPUT_##x##_PORT >= 4) ) ? (0) : ( ((P00_MODE_REG - P0_DATA_REG) + (COLUMN_INPUT_##x##_PORT == 3 ? \
                                            4 : COLUMN_INPUT_##x##_PORT) * 0x20) + (COLUMN_INPUT_##x##_PIN * 2) ) )
    #endif
#else
    #define SET_INPUT_MODE_REG(x)   ( (COLUMN_INPUT_##x##_PORT < 4) ? ( ((P00_MODE_REG - P0_DATA_REG) + (COLUMN_INPUT_##x##_PORT == 3 ? \
                                      4 : COLUMN_INPUT_##x##_PORT) * 0x20) + (COLUMN_INPUT_##x##_PIN * 2) ) : (0) )
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
#define SET_MASK0_FROM_COLUMN(x)    ( (COLUMN_INPUT_##x##_PORT == 0) ? (1 << COLUMN_INPUT_##x##_PIN) : 0 )

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define SET_MASK12_FROM_COLUMN(x)   ( (COLUMN_INPUT_##x##_PORT == 1) ? (0x0400 << COLUMN_INPUT_##x##_PIN) : ((COLUMN_INPUT_##x##_PORT == 2) ? \
                                      (1 << COLUMN_INPUT_##x##_PIN) : 0) )

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define SET_MASK3_FROM_COLUMN(x)    ( (COLUMN_INPUT_##x##_PORT == 3) ? (1 << COLUMN_INPUT_##x##_PIN) : 0 )

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 * \param[in]   m
 *
 * \return
 ****************************************************************************************
 */
#define CHECK_MASK_FROM_COLUMN(x, m)                            \
                                    ( (x == 0)  ? m(0)  :   \
                                      (x == 1)  ? m(1)  :   \
                                      (x == 2)  ? m(2)  :   \
                                      (x == 3)  ? m(3)  :   \
                                      (x == 4)  ? m(4)  :   \
                                      (x == 5)  ? m(5)  :   \
                                      (x == 6)  ? m(6)  :   \
                                      (x == 7)  ? m(7)  :   \
                                      (x == 8)  ? m(8)  :   \
                                      (x == 9)  ? m(9)  :   \
                                      (x == 10) ? m(10) :   \
                                      (x == 11) ? m(11) :   \
                                      (x == 12) ? m(12) :   \
                                      (x == 13) ? m(13) :   \
                                      (x == 14) ? m(14) :   \
                                      (x == 15) ? m(15) :   \
                                      (x == 16) ? m(16) :   \
                                      (x == 17) ? m(17) :   \
                                      (x == 18) ? m(18) :   \
                                      (x == 19) ? m(19) :   \
                                      (x == 20) ? m(20) :   \
                                      (x == 21) ? m(21) :   \
                                      (x == 22) ? m(22) :   \
                                      (x == 23) ? m(23) :   \
                                      (x == 24) ? m(24) :   \
                                      (x == 25) ? m(25) :   \
                                      (x == 26) ? m(26) :   \
                                      (x == 27) ? m(27) :   \
                                      (x == 28) ? m(28) :   \
                                      (x == 29) ? m(29) :   \
                                      (x == 30) ? m(30) :   \
                                      (x == 31) ? m(31) :   \
                                      (x == 32) ? m(32) : 0  )
                                       
/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define CHECK_MASK0_FROM_COLUMN(x)      CHECK_MASK_FROM_COLUMN(x, SET_MASK0_FROM_COLUMN)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define CHECK_MASK12_FROM_COLUMN(x)     CHECK_MASK_FROM_COLUMN(x, SET_MASK12_FROM_COLUMN)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define CHECK_MASK3_FROM_COLUMN(x)      CHECK_MASK_FROM_COLUMN(x, SET_MASK3_FROM_COLUMN)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   m
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define SET_WKUP_MASK_FROM_COLUMN(m, x) ( (COLUMN_INPUT_##x##_PORT == m) ? (1 << COLUMN_INPUT_##x##_PIN) : 0 )

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   p
 * \param[in]   x
 * \param[in]   m
 *
 * \return
 ****************************************************************************************
 */
#define CHECK_WKUP_MASK_FROM_COLUMN(p, x, m)                       \
                                    ( (x == 0)  ? m(p, 0)  :   \
                                      (x == 1)  ? m(p, 1)  :   \
                                      (x == 2)  ? m(p, 2)  :   \
                                      (x == 3)  ? m(p, 3)  :   \
                                      (x == 4)  ? m(p, 4)  :   \
                                      (x == 5)  ? m(p, 5)  :   \
                                      (x == 6)  ? m(p, 6)  :   \
                                      (x == 7)  ? m(p, 7)  :   \
                                      (x == 8)  ? m(p, 8)  :   \
                                      (x == 9)  ? m(p, 9)  :   \
                                      (x == 10) ? m(p, 10) :   \
                                      (x == 11) ? m(p, 11) :   \
                                      (x == 12) ? m(p, 12) :   \
                                      (x == 13) ? m(p, 13) :   \
                                      (x == 14) ? m(p, 14) :   \
                                      (x == 15) ? m(p, 15) :   \
                                      (x == 16) ? m(p, 16) :   \
                                      (x == 17) ? m(p, 17) :   \
                                      (x == 18) ? m(p, 18) :   \
                                      (x == 19) ? m(p, 19) :   \
                                      (x == 20) ? m(p, 20) :   \
                                      (x == 21) ? m(p, 21) :   \
                                      (x == 22) ? m(p, 22) :   \
                                      (x == 23) ? m(p, 23) :   \
                                      (x == 24) ? m(p, 24) :   \
                                      (x == 25) ? m(p, 25) :   \
                                      (x == 26) ? m(p, 26) :   \
                                      (x == 27) ? m(p, 27) :   \
                                      (x == 28) ? m(p, 28) :   \
                                      (x == 29) ? m(p, 29) :   \
                                      (x == 30) ? m(p, 30) :   \
                                      (x == 31) ? m(p, 31) :   \
                                      (x == 32) ? m(p, 32) : 0  )
                                       
/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define CHECK_WKUP_MASK0_FROM_COLUMN(x)  CHECK_WKUP_MASK_FROM_COLUMN(0, x, SET_WKUP_MASK_FROM_COLUMN)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define CHECK_WKUP_MASK1_FROM_COLUMN(x)  CHECK_WKUP_MASK_FROM_COLUMN(1, x, SET_WKUP_MASK_FROM_COLUMN)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define CHECK_WKUP_MASK2_FROM_COLUMN(x)  CHECK_WKUP_MASK_FROM_COLUMN(2, x, SET_WKUP_MASK_FROM_COLUMN)

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   x
 *
 * \return
 ****************************************************************************************
 */
#define CHECK_WKUP_MASK3_FROM_COLUMN(x)  CHECK_WKUP_MASK_FROM_COLUMN(3, x, SET_WKUP_MASK_FROM_COLUMN)

#endif // APP_KBD_MACROS_H_

/**
 * \}
 * \}
 * \}
 */