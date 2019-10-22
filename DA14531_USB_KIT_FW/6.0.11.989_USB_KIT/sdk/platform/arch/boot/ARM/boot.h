/**
 ****************************************************************************************
 *
 * @file boot.h
 *
 * @brief This file contains the declarations of the boot related variables.
 *
 * Copyright (C) RivieraWaves 2009-2014
 * Copyright (C) 2018 Modified by Dialog Semiconductor
 *
 *
 ****************************************************************************************
 */

#ifndef _BOOT_H_
#define _BOOT_H_

extern const uint32_t Image$$ER_IROM3$$Base[];
#define CODE_AREA_BASE      ((uint32_t)Image$$ER_IROM3$$Base)

extern const uint32_t Image$$ER_IROM3$$Length[];
#define CODE_AREA_LENGTH    ((uint32_t)Image$$ER_IROM3$$Length)

extern const uint32_t Image$$RET_DATA_UNINIT$$Base[];
#define RET_MEM_BASE        ((uint32_t)Image$$RET_DATA_UNINIT$$Base)

extern const uint32_t heap_mem_area_not_ret$$Base[];
#define NON_RET_HEAP_BASE   ((uint32_t)heap_mem_area_not_ret$$Base)

extern const uint32_t heap_mem_area_not_ret$$Length[];
#define NON_RET_HEAP_LENGTH ((uint32_t)heap_mem_area_not_ret$$Length)

#endif // _BOOT_H_
