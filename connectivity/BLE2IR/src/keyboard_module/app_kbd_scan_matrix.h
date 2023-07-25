/**
 ****************************************************************************************
 *
 * \file app_kbd_scan_matrix.h
 *
 * \brief HID Keyboard scan matrix.
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
 * OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */

#ifndef _APP_KBD_SCAN_MATRIX_H_
#define _APP_KBD_SCAN_MATRIX_H_

/**
 ****************************************************************************************
 * \addtogroup CONFIGURATION
 * \{
 * \addtogroup MODULE_CONFIG
 * \{
 * \addtogroup KEYBOARD_SCAN_MATRIX_CFG
 *
 * \brief Keyboard module scan matrix configuration
 * \{
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * \brief Number of input pins used for key matrix columns
 ****************************************************************************************
 */
#define KBD_NR_COLUMN_INPUTS     4

/**
 ****************************************************************************************
 * \brief Number of output pins used for key matrix rows
 ****************************************************************************************
 */
#define KBD_NR_ROW_OUTPUTS       4

/**
 ****************************************************************************************
 * \brief Define the pins used for columns. Maximum number of columns supported is 16.
 *        If a column is not used set the corresponding port and pin to PORT_UNUSED
 *        and PIN_UNUSED respectively.
 ****************************************************************************************
 */
#define COLUMN_INPUT_0_PORT		 0	
#define COLUMN_INPUT_0_PIN		 6
                                 
#define COLUMN_INPUT_1_PORT		 0	
#define COLUMN_INPUT_1_PIN		 7
                                 
#define COLUMN_INPUT_2_PORT		 0	
#define COLUMN_INPUT_2_PIN		 3

#define COLUMN_INPUT_3_PORT		 0	
#define COLUMN_INPUT_3_PIN		 0
/**
 ****************************************************************************************
 * \brief Define the pins used for rows. Maximum number of rows supported is 16.
 *        If a row is not used set the corresponding port and pin to PORT_UNUSED
 *        and PIN_UNUSED respectively.
 ****************************************************************************************
 */
#define ROW_OUTPUT_0_PORT	     1	
#define ROW_OUTPUT_0_PIN	     2
                                 
#define ROW_OUTPUT_1_PORT	     1	
#define ROW_OUTPUT_1_PIN	     1
                                 
#define ROW_OUTPUT_2_PORT	     0	
#define ROW_OUTPUT_2_PIN	     1
                                
#define ROW_OUTPUT_3_PORT	     0	
#define ROW_OUTPUT_3_PIN	     2

#ifdef HAS_POWERUP_BUTTON
    #define POWER_BUTTON_COLUMN  3
    #define POWER_BUTTON_ROW     1
#endif

/**
 * \}
 * \}
 * \}
 */

#endif // _APP_KBD_SCAN_MATRIX_H_
