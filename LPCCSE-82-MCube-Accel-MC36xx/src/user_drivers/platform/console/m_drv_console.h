/******************************************************************************
 *
 * Copyright (c) 2018 mCube, Inc.  All rights reserved.
 *
 * This source is subject to the mCube Software License.
 * This software is protected by Copyright and the information and source code
 * contained herein is confidential. The software including the source code
 * may not be copied and the information contained herein may not be used or
 * disclosed except with the written permission of mCube Inc.
 *
 * All other rights reserved.
 *****************************************************************************/

/**
 * @file    m_drv_console.h
 * @author  mCube
 * @date    19 June 2018
 * @brief   Console interface header for mCube sensor driver.
 * @see     http://www.mcubemems.com
 */

#ifndef _M_DRV_CONSOLE_H_
#define _M_DRV_CONSOLE_H_

/******************************************************************************
 *** INCLUDE FILES
 *****************************************************************************/
/** Please include interface driver from platform SDK */

/******************************************************************************
 *** MACRO
 *****************************************************************************/
#define M_INT_TO_FLOAT(val)                                                 \
            (uint32_t)(((val) < 0 && (val) > -1.0) ? "-" : ""),             \
            (int32_t)(val),                                                 \
            (int32_t)((((val) > 0) ? (val) - (int32_t)(val)                 \
            : (int32_t)(val) - (val))*100)

#define M_FLOAT_PRINTF(val)                                                 \
do {                                                                        \
    M_PRINTF("%s%d.%02d", M_INT_TO_FLOAT(val));                             \
    NRF_LOG_FLUSH();                                                        \
} while (0);

#define M_3FLOAT_PRINTF(val1, val2, val3)                                   \
do {                                                                        \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val1));                           \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val2));                           \
    M_PRINTF("%s%d.%02d", M_INT_TO_FLOAT(val3));                            \
    NRF_LOG_FLUSH();                                                        \
} while (0);

#define M_6FLOAT_PRINTF(val1, val2, val3, val4, val5, val6)                 \
do {                                                                        \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val1));                           \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val2));                           \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val3));                           \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val4));                           \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val5));                           \
    M_PRINTF("%s%d.%02d", M_INT_TO_FLOAT(val6));                            \
    NRF_LOG_FLUSH();                                                        \
} while (0);

#define M_9FLOAT_PRINTF(val1, val2, val3, val4, val5, val6, val7, val8, val9) \
do {                                                                        \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val1));                           \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val2));                           \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val3));                           \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val4));                           \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val5));                           \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val6));                           \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val7));                           \
    M_PRINTF("%s%d.%02d,", M_INT_TO_FLOAT(val8));                           \
    M_PRINTF("%s%d.%02d", M_INT_TO_FLOAT(val9));                            \
    NRF_LOG_FLUSH();                                                        \
} while (0);

extern void mcube_printf(const char *format, ...);
extern void printf_dec(short a);

#endif /* _M_DRV_CONSOLE_H_ */
