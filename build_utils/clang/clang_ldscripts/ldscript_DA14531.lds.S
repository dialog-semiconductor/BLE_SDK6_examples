/*
 ****************************************************************************************
 *
 * @file ldscript_DA14531.lds.S
 *
 * @brief Common GNU LD linker script file.
 *
 * Copyright (C) 2018-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "da1458x_config_basic.h"
#include "da1458x_config_advanced.h"
#include "da1458x_scatter_config.h"

#include "mem_DA14531.lds"

__ER_IROM3_BASE__   = ORIGIN(LR_IROM3);
__ER_IROM3_LENGTH__ = LENGTH(LR_IROM3);

/* Set the entry instruction */
ENTRY(Reset_Handler)

/* Sections layout */
SECTIONS
{
    /* Reset vector at the beginning of RAM */
    ER_IROM1 :
    {
        KEEP(*(.vectors))
        KEEP(*(.otp_cs_booter))

    } > LR_IROM1

    ER_IROM2 :
    {
        KEEP(*(.patch_table))
    } > LR_IROM2

    ER_IROM3 :
    {
        /* Move ISR implementations after patch_table */
        KEEP(*(.isr_impl))
        KEEP(*(.text.disp_heaplog))
        *(.text*)

        . = ALIGN(4);

        KEEP(*(.init))
        KEEP(*(.fini))
        *(.rodata*)
    } > LR_IROM3

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > LR_IROM3

    __exidx_start = .;
    .ARM.exidx :
    {
       *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > LR_IROM3
    __exidx_end = .;

    /* To copy multiple ROM to RAM sections,
     * define __STARTUP_COPY_MULTIPLE in startup_DA14531.S
     */
    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__etext)
        LONG (__data_start__)
        LONG (__data_end__ - __data_start__)
        __copy_table_end__ = .;
    } > LR_IROM3

    /* To clear multiple BSS sections,
     * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_DA14531.S
     */
    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG (__bss_end__ - __bss_start__)

        LONG (__ret_data_start__)
        LONG (__ret_data_end__ - __ret_data_start__)

        __zero_table_end__ = .;
    } > LR_IROM3


    /* Location counter can end up 2byte aligned with narrow Thumb code but
     * __etext is assumed by startup code to be the LMA of a section in RAM
     * which must be 4byte aligned
     */
    __etext = ALIGN(4);

    /* The initialised data section is stored immediately
     * at the end of the text section.
     */
    .data : AT (__etext)
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
    } > LR_IROM3

    __code_area_end__ = .;
    
    .constdata :
    {

    } > LR_IROM3

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > LR_IROM3
    

    ER_PRODTEST (NOLOAD):
    {
        . = ALIGN(4);
        *(prodtest_uninit)
    } > LR_IROM3

    ER_NZI (NOLOAD) :
    {
        . = ALIGN(4);
        __heap_mem_area_not_ret_start__ = .;       
        *(heap_mem_area_not_ret)
        __heap_mem_area_not_ret_end__ = .;
    } > LR_IROM3

    /* After the zero-init sections goes the non retained heap and stack */
    .heap (NOLOAD):
    {
        __end__ = .;
        PROVIDE(end = .);
        *(.heap*)
        __HeapLimit = .;
    } > LR_IROM3

  
    /* Execution region needed for stateful hibernation. */
    ER_STATEFUL_HIBERNATION (NOLOAD) :
    {
        *(.stateful_hibernation)
    } > LR_IROM3
    
    /* .stack_dummy section does not contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later.
     * COPY type does not allocate memory.
     */
    .stack_dummy (COPY):
    {
       KEEP(*(.stack*))
    } > LR_IROM3
    
     /* Set stack top to end of LR_IROM3 before the retained section,
     * and stack limit moves down by size of stack_dummy section.
     */
    __StackTop = ORIGIN(LR_IROM3) + LENGTH(LR_IROM3);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);
    


    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")

    /* Exact address placement to force first section in LR_RETAINED_RAM0 alignment to single word boundary.
     * By default linker aligns it in two-word boundary even when LR_RETAINED_RAM0 starts at a word boundary.
     */
    RET_DATA_UNINIT RET_MEM_BASE_ADDR (NOLOAD) :
    {
        . = ALIGN(4);
        __retention_mem_area_uninit_start__ = .;
        *(retention_mem_area_uninit)            /* uninitialized application data */
        . = ALIGN(4);
        __retention_mem_area_uninit_end__ = .;
    } > LR_RETAINED_RAM0

    RET_DATA (NOLOAD) :
    {
        __ret_data_start__ = .;
        . = ALIGN(4);
        *(retention_mem_area0)                  /* zero initialized SDK + application data */
        . = ALIGN(4);
        __ret_data_end__ = .;
    } > LR_RETAINED_RAM0
    
    
    /*********************************************************************************************
     * Check if the user selected retained data (the zero initialized) size fits in the RET_DATA
     * executon region.
     * If the check fails, then the CFG_RET_DATA_SIZE value must be increased accordingly.
     * Note: If the selected size is equal to the value calculated by the linker, then the check
     *       can be omitted.
     *********************************************************************************************/
    ASSERT(CFG_RET_DATA_SIZE > SIZEOF(RET_DATA), "CFG_RET_DATA_SIZE value must be increased.")

    RET_HEAP (NOLOAD):
    {
        . = ALIGN(4);
        *(heap_env_area)
        __db_heap_start__ = .;
        *(heap_db_area)     
        __db_heap_end__ = .;
        *(heap_msg_area) 
    } > LR_RETAINED_RAM0

#if defined (CFG_TRNG)
    RET_DATA_UNINIT_TRNG_STATE (NOLOAD) :
    {
        *(trng_state)         /* flag to decide if TRNG must run or not in order to generate the seed number */
    } > LR_RETAINED_TRNG_STATE
#endif /* defined (CFG_TRNG) */

#if defined (CFG_USE_CHACHA20_RAND)
    RET_DATA_UNINIT_CHACHA_STATE (NOLOAD) :
    {
        *(chacha20_state) /* random state in case chacha20 is used */
    } > LR_RETAINED_CHACHA_STATE
#endif /* defined (CFG_USE_CHACHA20_RAND) */
}

/* Include ROM symbol definitions */
INCLUDE da14531_symbols.lds
