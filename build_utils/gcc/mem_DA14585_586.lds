/**
 ****************************************************************************************
 *
 * @file mem_DA14585_586.lds
 *
 * @brief Common GNU LD linker script file - memory configuration.
 *
 * Copyright (C) 2018-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*
 * This file needs to be run nagainst C preprocessor before it is used by the linker program.
 *
 * ==============================================================================================================================
 * |                                                         System RAM                                                         |
 * ------------------------------------------------------------------------------------------------------------------------------
 * |+ 1st RAM block (32KB)         + 2nd RAM block (16KB)         + 3rd RAM block (16KB)         + 4th RAM block (32KB)         |
 * ------------------------------------------------------------------------------------------------------------------------------
 * |                                                              ^                 ^            ^                   ^          |
 * |                                                              |                 |            |                   |          |
 * |                                                              |        RET_MEM_BASE_ADDR     |                   |          |
 * |                                                              |                       RAM_4_BASE_ADDR            |          |
 * |                                                       RAM_3_BASE_ADDR                                      __SCT_BLE_BASE  |
 * ==============================================================================================================================
 */

/* Macro to align val on the multiple of 4 equal or nearest higher */
#define ALIGN4_HI(val) (((val)+3) & (~(3)))

#if !defined(CFG_RET_DATA_SIZE)
    #error "CFG_RET_DATA_SIZE is not defined!"
#endif

#if !defined(CFG_RET_DATA_UNINIT_SIZE)
    #error "CFG_RET_DATA_UNINIT_SIZE is not defined!"
#endif

/********************************************************************************************
 * Memory area where retained data will be stored.
 *******************************************************************************************/
#define RET_MEM_SIZE        (CFG_RET_DATA_UNINIT_SIZE + CFG_RET_DATA_SIZE + RET_HEAP_SIZE)
/* Retained data base address */
#define RET_MEM_BASE_ADDR    ALIGN4_HI(__SCT_BLE_BASE - RET_MEM_SIZE)

/********************************************************************************************
 * Free area resides between the Exchange memory and ROM data.
 *******************************************************************************************/
/* Free area base address */
#define FREE_AREA_BASE_ADDR     ALIGN4_HI(__SCT_BLE_BASE + __SCT_EM_BLE_END)
/* Free area size */
#define FREE_AREA_SIZE          (ROM_DATA_BASE_ADDR - FREE_AREA_BASE_ADDR)

#if defined(CFG_CODE_LOCATION_OTP) && defined(CFG_CODE_LOCATION_EXT)
    #error "Only one of CFG_CODE_LOCATION_OTP and CFG_CODE_LOCATION_EXT must be defined!"
#elif defined(CFG_CODE_LOCATION_OTP)
    #define CODE_LOCATION_OTP   1
    #define CODE_LOCATION_EXT   0
#elif defined(CFG_CODE_LOCATION_EXT)
    #define CODE_LOCATION_OTP   0
    #define CODE_LOCATION_EXT   1
#else
    #error "One of CFG_CODE_LOCATION_OTP and CFG_CODE_LOCATION_EXT must be defined!"
#endif

#if defined (CFG_TRNG)
    #define TRNG_BUFFER_AREA_SZ CFG_TRNG
#else
    #define TRNG_BUFFER_AREA_SZ 0
#endif

/* OTP memory size = 64K */
#define OTP_MEM_SIZE            (64 * 1024)

/* OTP header section size = 64 * 64-bit words = 512bytes */
#define OTP_HEADER_SIZE         ((64 * 64) / 8)

/* These defines are specific to DA14585_586, do not alter. */
#define SRAM_BASE_ADDR      0x07fc0000

#define BOOT_VECTOR_AREA_SZ 0xC0
#define PATCH_TABLE_AREA_SZ 0x50

/* Useful OTP memory size:
 * 1. The 512 bytes of the OTP header start at 63.5K offset in OTP
 *    memory.
 * 2. There is a limitation in the location of the TRNG buffer (max size is 1K).
 *    The TRNG buffer must be always placed before the 64K limit in RAM.
 */
#define OTP_MEM_USEFUL_SIZE     (OTP_MEM_SIZE - OTP_HEADER_SIZE - TRNG_BUFFER_AREA_SZ)

#if CODE_LOCATION_OTP
    /* Base address of code (RAM base address + interrupt vector table size + patch table size) */
    #define CODE_AREA_BASE      (SRAM_BASE_ADDR + BOOT_VECTOR_AREA_SZ + PATCH_TABLE_AREA_SZ)

    /* Max needs in RAM per application - excluding the retained data, the interrupt vector table and the patch table */
    #define CODE_AREA_MAX_SIZE  (RET_MEM_BASE_ADDR - CODE_AREA_BASE)

    /* Useful memory area for OTP code */
    #define CODE_AREA_SIZE      (OTP_MEM_USEFUL_SIZE - (BOOT_VECTOR_AREA_SZ + PATCH_TABLE_AREA_SZ))
#elif CODE_LOCATION_EXT
    /* Base address of code (RAM base address + interrupt vector table size + patch table size + TRNG buffer) */
    #define CODE_AREA_BASE      ALIGN4_HI(SRAM_BASE_ADDR + BOOT_VECTOR_AREA_SZ + PATCH_TABLE_AREA_SZ + TRNG_BUFFER_AREA_SZ)

    /* Max needs in RAM per application - excluding the retained data, the interrupt vector table, the patch table and the TRNG buffer */
    #define CODE_AREA_MAX_SIZE  (RET_MEM_BASE_ADDR - CODE_AREA_BASE)

    /* Same as max size */
    #define CODE_AREA_SIZE      CODE_AREA_MAX_SIZE
#endif


MEMORY
{
    LR_IROM1          (rwx) : ORIGIN = SRAM_BASE_ADDR,                                              LENGTH = BOOT_VECTOR_AREA_SZ
    LR_IROM2          (rwx) : ORIGIN = SRAM_BASE_ADDR + BOOT_VECTOR_AREA_SZ,                        LENGTH = PATCH_TABLE_AREA_SZ
#if CODE_LOCATION_EXT
    LR_TRNG_ZI        (rwx) : ORIGIN = SRAM_BASE_ADDR + BOOT_VECTOR_AREA_SZ + PATCH_TABLE_AREA_SZ,  LENGTH = TRNG_BUFFER_AREA_SZ
#endif
    LR_IROM3          (rwx) : ORIGIN = CODE_AREA_BASE,                                              LENGTH = CODE_AREA_SIZE
    LR_RETAINED_RAM0  (rw)  : ORIGIN = RET_MEM_BASE_ADDR,                                           LENGTH = RET_MEM_SIZE
    /* After this there's only BLE Exchange Memory, externally defined by the __SCT_BLE_BASE address and with custom zeroing code in arch_rom.c*/

    /* Free area to be used by the application (change attribute to rw if used) */
    LR_FREE           (r)   : ORIGIN = FREE_AREA_BASE_ADDR                                          LENGTH = FREE_AREA_SIZE
}
