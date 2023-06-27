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
 * @file    m_drv_mc_utility.c
 * @author  mCube
 * @date    12 June 2018
 * @brief   Header of orientation map for sensor driver.
 * @see     http://www.mcubemems.com
 */

#ifndef _M_DRV_MC_UTILITY_H_
    #define _M_DRV_MC_UTILITY_H_

/*******************************************************************************
 *** INCLUDE FILES
 *******************************************************************************/

/*******************************************************************************
 *** CONSTANT / DEFINE
 *******************************************************************************/
#define M_DRV_MC_UTL_AXIS_X      0
#define M_DRV_MC_UTL_AXIS_Y      1
#define M_DRV_MC_UTL_AXIS_Z      2
#define M_DRV_MC_UTL_AXES_NUM    3

/*****************************************************************************
 *** DATA TYPE / STRUCTURE DEFINITION / ENUM
 *****************************************************************************/
typedef struct
{
    signed char      bSign[M_DRV_MC_UTL_AXES_NUM];
    unsigned char    bMap[M_DRV_MC_UTL_AXES_NUM];
}   S_M_DRV_MC_UTIL_OrientationReMap;

typedef enum
{
    E_M_DRV_UTIL_ORIENTATION_TOP_LEFT_DOWN = 0,
    E_M_DRV_UTIL_ORIENTATION_TOP_RIGHT_DOWN,
    E_M_DRV_UTIL_ORIENTATION_TOP_RIGHT_UP,
    E_M_DRV_UTIL_ORIENTATION_TOP_LEFT_UP,
    E_M_DRV_UTIL_ORIENTATION_BOTTOM_LEFT_DOWN,
    E_M_DRV_UTIL_ORIENTATION_BOTTOM_RIGHT_DOWN,
    E_M_DRV_UTIL_ORIENTATION_BOTTOM_RIGHT_UP,
    E_M_DRV_UTIL_ORIENTATION_BOTTOM_LEFT_UP,
    E_M_DRV_UTIL_ORIENTATION_TOTAL_CONFIG
}   E_M_DRV_UTIL_OrientationConfig;

/*******************************************************************************
 *** GLOBAL VARIABLE
 *******************************************************************************/
extern const S_M_DRV_MC_UTIL_OrientationReMap    g_MDrvUtilOrientationReMap[E_M_DRV_UTIL_ORIENTATION_TOTAL_CONFIG];

#endif    // END of _M_DRV_MC_UTILITY_H_
