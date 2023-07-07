/**
 ****************************************************************************************
 *
 * @file mem_DA14531.lds
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
 * This file needs to be run against C preprocessor before it is used by the linker program.
 *
 * ==============================================================================================
 * |                                          System RAM                                        |
 * ----------------------------------------------------------------------------------------------
 * |+ 1st RAM block (16KB)        + 2rd RAM block (12KB)         + 3th RAM block (20KB)         |
 * ----------------------------------------------------------------------------------------------
 * |                              ^                 ^            ^                   ^          |
 * |                              |                 |            |                   |          |
 * |                              |        RET_MEM_BASE_ADDR     |                   |          |
 * |                              |                       RAM_3_BASE_ADDR            |          |
 * |                       RAM_2_BASE_ADDR                                      __SCT_BLE_BASE  |
 * ==============================================================================================
 */

/* Macro to align val on the multiple of 4 equal or nearest higher */
#define ALIGN4_HI(val) (((val)+3) & (~(3)))

#if !defined(CFG_RET_DATA_SIZE)
    #error "CFG_RET_DATA_SIZE is not defined!"
#endif

#if !defined(CFG_RET_DATA_UNINIT_SIZE)
    #error "CFG_RET_DATA_UNINIT_SIZE is not defined!"
#endif

#if !defined(RET_DATA_UNINIT_CHACHA_STATE_SIZE)
    #error "RET_DATA_UNINIT_CHACHA_STATE_SIZE is not defined!"
#endif

#if !defined(RET_DATA_UNINIT_TRNG_STATE_SIZE)
    #error "RET_DATA_UNINIT_TRNG_STATE_SIZE is not defined!"
#endif

/********************************************************************************************
 * Memory area where retained data will be stored.
 ********************************************************************************************/
#define RET_MEM_SIZE        (CFG_RET_DATA_UNINIT_SIZE + CFG_RET_DATA_SIZE + RET_HEAP_SIZE)

/* Retained data base address */
#define RET_MEM_BASE_ADDR    ALIGN4_HI(__SCT_BLE_BASE - RET_MEM_SIZE)

/* chacha20_state base address */
#define CHACHA_STATE_BASE_ADDR   ALIGN4_HI(ROM_DATA_BASE_ADDR - RET_DATA_UNINIT_CHACHA_STATE_SIZE)

/* trng_state base address */
#define TRNG_STATE_BASE_ADDR     ALIGN4_HI(ROM_DATA_BASE_ADDR - RET_DATA_UNINIT_CHACHA_STATE_SIZE - RET_DATA_UNINIT_TRNG_STATE_SIZE)

/********************************************************************************************
 * Free area resides between the Exchange memory and ROM data.
 ********************************************************************************************/
/* Free area base address */
#define FREE_AREA_BASE_ADDR     ALIGN4_HI(__SCT_BLE_BASE + __SCT_EM_BLE_END)

/* Free area size */
#define FREE_AREA_SIZE          (ROM_DATA_BASE_ADDR - FREE_AREA_BASE_ADDR) - (RET_DATA_UNINIT_CHACHA_STATE_SIZE + RET_DATA_UNINIT_TRNG_STATE_SIZE)

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

/* These defines are specific to DA14531, do not alter. */
#define SRAM_BASE_ADDR      0x07fc0000

#define BOOT_VECTOR_AREA_SZ 0xC0
#define PATCH_TABLE_AREA_SZ 0x50

/* OTP memory size = 32K*/
#define OTP_MEM_SIZE            (32 * 1024)

/* OTP header section size = 64 bytes*/
#define OTP_HEADER_SIZE         (64)

/* OTP CS section size = 240 bytes*/
#define OTP_CS_SIZE             (240)

/* Useful OTP memory size*/
#define OTP_MEM_USEFUL_SIZE     (OTP_MEM_SIZE - OTP_HEADER_SIZE - OTP_CS_SIZE)

/* Base address of code (RAM base address + interrupt vector table size + patch table size)*/
#define CODE_AREA_BASE          (SRAM_BASE_ADDR + BOOT_VECTOR_AREA_SZ + PATCH_TABLE_AREA_SZ)

/* Max needs in RAM per application - excluding the retained data, the interrupt vector table and the patch table*/
#define CODE_AREA_MAX_SIZE      (RET_MEM_BASE_ADDR - CODE_AREA_BASE)

#if CODE_LOCATION_OTP
    #define CODE_AREA_SIZE      (OTP_MEM_USEFUL_SIZE - (BOOT_VECTOR_AREA_SZ + PATCH_TABLE_AREA_SZ))
#elif CODE_LOCATION_EXT
    #define CODE_AREA_SIZE      CODE_AREA_MAX_SIZE
#endif


MEMORY
{
    LR_IROM1          (rwx) : ORIGIN = SRAM_BASE_ADDR,                                              LENGTH = BOOT_VECTOR_AREA_SZ
    LR_IROM2          (rwx) : ORIGIN = SRAM_BASE_ADDR + BOOT_VECTOR_AREA_SZ,                        LENGTH = PATCH_TABLE_AREA_SZ
    LR_IROM3          (rwx) : ORIGIN = CODE_AREA_BASE,                                              LENGTH = CODE_AREA_SIZE
    LR_RETAINED_RAM0  (rw)  : ORIGIN = RET_MEM_BASE_ADDR,                                           LENGTH = RET_MEM_SIZE
    /* After this there's only BLE Exchange Memory, externally defined by the __SCT_BLE_BASE address and with custom zeroing code in arch_rom.c */

    /* Free area to be used by the application (change attribute to rw if used) */
    LR_FREE           (r)          : ORIGIN = FREE_AREA_BASE_ADDR,                                  LENGTH = FREE_AREA_SIZE
    /* Fixed area used by TRNG */
    LR_RETAINED_TRNG_STATE   (rw)  : ORIGIN = TRNG_STATE_BASE_ADDR,                                 LENGTH = RET_DATA_UNINIT_TRNG_STATE_SIZE
    /* Fixed area used by CHACHA20 */
    LR_RETAINED_CHACHA_STATE (rw)  : ORIGIN = CHACHA_STATE_BASE_ADDR,                               LENGTH = RET_DATA_UNINIT_CHACHA_STATE_SIZE
}