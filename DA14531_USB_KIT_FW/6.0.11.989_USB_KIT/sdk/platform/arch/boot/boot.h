/**
 ****************************************************************************************
 *
 * @file boot.h
 *
 * @brief Declarations of the boot related variables.
 *
 * Copyright (C) 2018 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*
 * Arm Compiler 4/5
 */
#if   defined ( __CC_ARM )
    #include "ARM/boot.h"

/*
 * GNU Compiler
 */
#elif defined ( __GNUC__ )
    #include "GCC/boot.h"

#endif
