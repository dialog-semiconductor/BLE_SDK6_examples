/**
 ****************************************************************************************
 *
 * @file rdtest_support.c
 *
 * @brief RD test support functions source code file.
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rdtest_support.h" 

void delay_systick_us(unsigned long int delay) //delay in units of 1 usec
{
    delay *= 16;            // see explanation above this function

    if (delay>0xFFFFFF)
    {
        delay = 0xFFFFFF;   // limit, max 24 bits
    }
    SysTick->CTRL = 0;      // Disable SysTick
    SysTick->LOAD = delay;  // Count down from delay eg. 999 to 0
    SysTick->VAL  = 0;      // Clear current value to 0
    SysTick->CTRL = 0x5;    // Enable SysTick, disable SysTick exception and use processor clock
    while ((SysTick->CTRL & 0x10000) == 0);
}
