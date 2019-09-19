/**
 ****************************************************************************************
 *
 * @file gcc/compiler.h
 *
 * @brief Definitions of compiler specific directives.
 *
 * Copyright (C) RivieraWaves 2009-2014
 * Copyright (c) 2018 Dialog Semiconductor. All rights reserved.
 *
 *
 ****************************************************************************************
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

#ifndef __GNUC__
#error "File only included with GCC!"
#endif // __GNUC__

#include "cmsis_compiler.h"

/// define the IRQ handler attribute for this compiler
#define __IRQ           __attribute__ ((interrupt("IRQ")))

/// define the BLE IRQ handler attribute for this compiler
#define __BTIRQ

/// define the BLE IRQ handler attribute for this compiler
#define __BLEIRQ

/// define the FIQ handler attribute for this compiler
#define __FIQ           __attribute__ ((interrupt("FIQ")))

/// define size of an empty array (used to declare structure with an array size not defined)
#define __ARRAY_EMPTY   0

/// Put a variable in a memory maintained during deep sleep
#define __LOWPOWER_SAVED

/// Put a variable in a zero-initialised memory section
#define __SECTION_ZERO(sec_name) __attribute__((section(sec_name))) // No zero_init attribute support in GCC. The initialisatio is being done by the low level initialisation code. Make sure your section is added in '.zero.table' inside the linker script.

/// Put a variable in a given memory section
#define __SECTION(sec_name) __attribute__((section(sec_name)))

#endif // _COMPILER_H_
