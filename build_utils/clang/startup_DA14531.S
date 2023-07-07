/* File: startup_DA14531.S
 * Purpose: startup file for Cortex-M0+ devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 16 August 2013
 */

/**
 ****************************************************************************************
 * Copyright (C) 2011-2013 ARM LIMITED
 * Copyright (C) 2018-2020 Modified by Dialog Semiconductor
 *
 *  All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ARM nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************************
 */

    .syntax unified
    .arch   armv6-m

#if defined (CFG_STATEFUL_HIBERNATION)
/* DA14531 register definition needed below */
        .equ HIBERN_CTRL_REG, 0x50000310
#endif

    .section .stack
    .align  8
#ifdef __STACK_SIZE
        .equ    Stack_Size, __STACK_SIZE
#else
        .equ    Stack_Size, 0x700
#endif
    .globl  __StackTop
    .globl  __StackLimit
__StackLimit:
    .space  Stack_Size
    .size   __StackLimit, . - __StackLimit
    .align  8
__StackTop:
    .size   __StackTop, . - __StackTop

    .section .heap
    .align  8
#ifdef __HEAP_SIZE
        .equ    Heap_Size, __HEAP_SIZE
#else
        .equ    Heap_Size, 0x100
#endif
    .globl  __HeapBase
    .globl  __HeapLimit
__HeapBase:
    .if Heap_Size
        .space  Heap_Size
    .endif
    .size   __HeapBase, . - __HeapBase
    .align  8
__HeapLimit:
    .size   __HeapLimit, . - __HeapLimit

#if defined (CFG_STATEFUL_HIBERNATION)
/* Added reserve space for the ARM core register dump */
    .section .stateful_hibernation
    .align  4
/* Reserve space for stacking, in case an exception occurs while restoring the state */
safe_stack_state:   .space   32

stored_PRIMASK: /* reserve space for interrupt mask register (PRIMASK) */
                .space 4
stored_R4:      /* reserve space for R4, R5, R6, R7, R8, R9, R10, R11, R12, R14 */
                .space 40
stored_MSP:     /* reserve space for MSP and PSP and control */
                .space 12
stored_ISER:    /* reserve space for NVIC->ISER, NVIC->IPR and SCB->SCR */
                .space 68

hiber_magic_word:	.space 4
#endif /* CFG_STATEFUL_HIBERNATION */


/* ISR implementations common to DA14585_586 and DA14531 */
    .thumb
    .section .isr_impl
    .align  2

#if defined (CFG_STATEFUL_HIBERNATION)

    .thumb_func
    .weak   _store_primask
    .type   _store_primask, %function
    .globl  _store_primask
    .fnstart
/* Function to store PRIMASK */
_store_primask:
    /* Save PRIMASK to start of stored_PRIMASK and disable IRQ */
    ldr     r3, =stored_PRIMASK
    mrs     r0, PRIMASK
    cpsid   i
    stm     r3!, {r0}
    bx      lr
    .fnend
    .size   _store_primask, . - _store_primask

    .thumb_func
    .weak   _store_core_registers
    .type   _store_core_registers, %function
    .globl  _store_core_registers
    .fnstart
/* Function to store the ARM core state and let the system go to stateful
 * hibernation */
_store_core_registers:
    /* store R4-R12, LR */
    ldr     r3, =stored_R4
    stm     r3!, {r4-r7}    /* save R4-R7 */
    mov     R4, R8
    mov     R5, R9
    mov     R6, R10
    mov     R7, R11
    stm     r3!, {r4-r7}    /* save R8-R11 */

    mov     R4, R12
    mov     R5, R14
    stm     r3!, {r4-r5}    /* save R12, LR (R14) */

    /* Store MSP, PSP, CONTROL */
    ldr     r3, =stored_MSP
    mrs     r0, msp
    mrs     r1, psp
    mrs     r2, control
    stm     r3!, {r0-r2}

    /* Store NVIC->ISER[0] */
    ldr     r3, =stored_ISER
    ldr     r0, =0xE000E100
    ldm     r0!, {r4}
    stm     r3!, {r4}
    /* Disable all interrupts in NVIC - NVIC->ICER[0U] = 0xFFFFFFFFUL */
    ldr     r0, =0xE000E180
    ldr     r1, =0xFFFFFFFF
    str     r1, [r0]
    /* Clear all pending interrupts in NVIC - NVIC->ICPR[0U] = 0xFFFFFFFFUL */
    ldr     r0, =0xE000E280
    ldr     r1, =0xFFFFFFFF
    str     r1, [r0]

    /* Save values of NVIC->IP[0..7] - for DA14531 we only have 8
     * (since we have 21 IRQs)
     * They fit in 8 32-bit registers */
    ldr     r0, =0xE000E400         /* NVIC->IPR[] base address */
    ldm     r0!, {r4-r7}
    stm     r3!, {r4-r7}
    ldm     r0!, {r4-r7}
    stm     r3!, {r4-r7}

    ldr     r0, =0xE000ED00         /* address of SCB */

    ldr     r4, [r0, #0x04]         /* value of SCB->ICSR */
    stm     r3!, {r4}

    ldr     r4, [r0, #0x10]         /* value of SCB->SCR */
    ldr     r5, [r0, #0x14]         /* value of SCB->CCR */
    ldr     r6, [r0, #0x1C]         /* value of SCB->SHPR2 */
    ldr     r7, [r0, #0x20]         /* value of SCB->SHPR3 */
    stm     r3!, {r4-r7}

    mov     r9, r0                  /* save address of SCB for later */
    mov     r10, r4                 /* save value of SCB->SCR for later */

    /* Set SCR.SLEEPDEEP */
    ldr     r1, =0xE000ED00         /* address of SCB */
    mov     r0, r4                  /* r4 = saved SCB->SCR value */
    movs    r2, #4
    orrs    r0, r0, r2
    str     r0, [r1, #0x10]         /* r9 = SCB start address */

    movs    r1, #0xA5
    ldr     r2, =hiber_magic_word
    str     r1, [r2]

    nop
    nop
    nop

    wfi

    /* we reach this point if device didn't actually go to stateful
     * hibernation */

    /* Restore SCR.SLEEPDEEP */
    str     r4, [r1, #0x10]

    /* Return 0 to the caller of _store_core_registers() */
    movs    r0, #0
    b       restore_system_state
    .fnend
    .size   _store_core_registers, . - _store_core_registers

    .thumb_func
    .weak   restore_system_state
    .type   restore_system_state, %function
    .globl  restore_system_state
    .fnstart
/* Function to restore the system state */
restore_system_state:
    /* disable interrupts, final state will be set when PRIMASK is restored */
    cpsid   i

    /* set SP to a safe value, we have reserved some space for this case */
    ldr     r0, =safe_stack_state
    mov     sp, r0


    /* restore values of NVIC->ISER, NVIC->PRIORITY{0,1,2},
     * SCB->SCR, SCB->SHR[0] (SHPR2) and SCB->SHR[1] (SHPR3) */

    ldr     r0, =stored_ISER

    /* Restore NVIC->ISER[0]  - for DA14531 we only have one */
    ldm     r0!, {r4}
    ldr     r1, =0xE000E100         /* address of NVIC->ISER[0] */
    stm     r1!, {r4}

    /* Restore values of NVIC->IPR[0..7] - we only have 8 IRQs for DA14531 which fit in 6 registers */
    ldm     r0!, {r4-r7}
    ldr     r1, =0xE000E400         /* NVIC->IPR[] base address */
    stm     r1!, {r4-r7}
    ldm     r0!, {r4-r7}
    ldr     r1, =0xE000E410         /* NVIC->IPR[] base address */
    stm     r1!, {r4-r7}

    /* Restore
     * SCB->SCR
     * SCB->CCR
     * SCB->SHPR2/3 */
    ldm     r0!, {r4}
    ldr     r1, =0xE000ED00

    ldr     r4, [r1, #0x04]
    ldm     r0!, {r4-r7}

    str     r4, [r1, #0x10]         /* SCB->SCR */
    str     r5, [r1, #0x14]         /* SCB->CCR */
    str     r6, [r1, #0x1C]         /* SCB->SHPR2 */
    str     r7, [r1, #0x20]         /* SCB->SHPR3 */

    /* restore MSP, PSP, CONTROL */
    ldr     r0, =stored_MSP
    ldm     r0!, {r1-r3}
    msr     msp, r1
    msr     psp, r2
    msr     control, r3

    /* Return 1 to the caller of restore_system_state() */
    movs    r0, #1
    LDR     R0, =arch_hibernation_restore
    BLX     R0

    /* Restore R4-R11, 8 32-bit registers in total */
    ldr     r1, =stored_R4
    movs    r2, #0x10
    add     r1, r1, r2
    ldm     r1!, {r4-r7} /* get R8-R11 */
    mov     r8, r4
    mov     r9, r5
    mov     r10, r6
    mov     r11, r7

    ldr     r1, =stored_R4
    ldm     r1!, {r4-r7} /* get R4-R7 */

    /* Restore PRIMASK */
    ldr     r1, =stored_PRIMASK
    ldr     r2, [r1]
    msr     PRIMASK, r2

    /* Restore R12 and LR */
    ldr     r1, =stored_R4
    movs    r2, #0x20
    add     r1, r1, r2
    ldm     r1!, {r4-r5} /* get R12 and LR */
    mov     r12, r4
    mov     r14, r5
    bx      lr
    .fnend
    .size   restore_system_state, . - restore_system_state

#endif /* CFG_STATEFUL_HIBERNATION */

    .thumb_func
    .weak   Reset_Handler
    .type   Reset_Handler, %function
    .fnstart

@ Reset Handler
Reset_Handler:
#if defined (CFG_STATEFUL_HIBERNATION)
    /* check HIBERN_CTRL_REG */
    movs    r3, #1
    ldr     r1, =HIBERN_CTRL_REG
    ldrh    r2, [r1]
    ands    r2, r2, r3
    cmp     r2, #1
    bne     cold_reset

    /* if hiber_magic_word is equal to 0xA5, it means system woke up
     * from stateful hibernation */
    ldr     r1, =hiber_magic_word
    ldr     r2, [r1]

    /* reset hiber_magic_word */
    movs    r3, #0
    str     r3, [r1]

    cmp     r2, #0xA5
    bne     cold_reset

    ldr     r3, =restore_system_state
    bx      r3
cold_reset:
    /* Initialize hiber_magic_word to 0 on cold reset */
    ldr     r1, =hiber_magic_word
    ldr     r2, [r1]

    /* reset hiber_magic_word */
    movs    r3, #0
    str     r3, [r1]
#endif /* CFG_STATEFUL_HIBERNATION */

/* normal reset behavior */

/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */
/* #define __STARTUP_COPY_MULTIPLE */

#ifdef __STARTUP_COPY_MULTIPLE
    /*  Multiple sections scheme.
     *
     *  Between symbol address __copy_table_start__ and __copy_table_end__,
     *  there are array of triplets, each of which specify:
     *    offset 0: LMA of start of a section to copy from
     *    offset 4: VMA of start of a section to copy to
     *    offset 8: size of the section to copy. Must be multiple of 4
     *
     *  All addresses must be aligned to 4 bytes boundary.
     */
    ldr     r4, =__copy_table_start__
    ldr     r0, =__copy_table_end__

.L_loop0:
    cmp     r4, r5
    bge     .L_loop0_done
    ldr     r1, [r4]
    ldr     r2, [r4, #4]
    ldr     r3, [r4, #8]

.L_loop0_0:
    subs    r3, #4
    blt     .L_loop0_0_done
    ldr     r0, [r1, r3]
    str     r0, [r2, r3]
    b       .L_loop0_0

.L_loop0_0_done:
    adds    r4, #12
    b       .L_loop0

.L_loop0_done:

#else
    /*  Single section scheme.
     *
     *  The ranges of copy from/to are specified by following symbols
     *    __etext: LMA of start of the section to copy from. Usually end of text
     *    __data_start__: VMA of start of the section to copy to
     *    __data_end__: VMA of end of the section to copy to
     *
     *  All addresses must be aligned to 4 bytes boundary.
     */
    ldr     r1, =__etext
    ldr     r2, =__data_start__
    ldr     r3, =__data_end__

    subs    r3, r2
    ble     .L_loop1_done

.L_loop1:
    subs    r3, #4
    ldr     r0, [r1, r3]
    str     r0, [r2, r3]
    bgt     .L_loop1

.L_loop1_done:
#endif /*__STARTUP_COPY_MULTIPLE */

/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise define macro __STARTUP_CLEAR_BSS to choose the later.
 */
#define __STARTUP_CLEAR_BSS_MULTIPLE

#ifdef __STARTUP_CLEAR_BSS_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of tuples specifying:
 *    offset 0: Start of a BSS section
 *    offset 4: Size of this BSS section. Must be multiple of 4
 */
    ldr     r3, =__zero_table_start__
    ldr     r4, =__zero_table_end__

.L_loop2:
    cmp     r3, r4
    bge     .L_loop2_done
    ldr     r1, [r3]
    ldr     r2, [r3, #4]
    movs    r0, 0

.L_loop2_0:
    subs    r2, #4
    blt     .L_loop2_0_done
    str     r0, [r1, r2]
    b       .L_loop2_0

.L_loop2_0_done:
    adds    r3, #8
    b       .L_loop2

.L_loop2_done:

#elif defined (__STARTUP_CLEAR_BSS)
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
    ldr     r1, =__bss_start__
    ldr     r2, =__bss_end__

    movs    r0, 0

    subs    r2, r1
    ble     .L_loop3_done

.L_loop3:
    subs    r2, #4
    str     r0, [r1, r2]
    bgt     .L_loop3

.L_loop3_done:
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

    /*bl      SystemInit*/
    LDR     R0, =SystemInit
    BLX     R0

    /*bl      _start*/
    LDR     R0, =_start
    BLX     R0 

    .fnend
    .size   Reset_Handler, . - Reset_Handler

    .thumb_func
    .type   NMI_Handler, %function
    .weak   NMI_Handler
    .fnstart

@ NMI handler
NMI_Handler:
    movs    r0, #4
    mov     r1, lr
    tst     r0, r1
    beq     NMI_stacking_used_MSP
    mrs     r0, psp
    ldr     r1, =NMI_HandlerC
    bx      r1
NMI_stacking_used_MSP:
    mrs     r0, msp
    ldr     r1, =NMI_HandlerC
    bx      r1
    .fnend
    .size   NMI_Handler, . - NMI_Handler

    .thumb_func
    .globl  HardFault_Handler
    .type   HardFault_Handler, %function
    .fnstart

@ HardFault handler
HardFault_Handler:
    movs    r0, #4
    mov     r1, lr
    tst     r0, r1
    beq     HardFault_stacking_used_MSP
    mrs     r0, psp
    ldr     r1, =HardFault_HandlerC
    bx      r1
HardFault_stacking_used_MSP:
    mrs     r0, msp
    ldr     r1, =HardFault_HandlerC
    bx      r1
    .fnend
    .size   HardFault_Handler, . - HardFault_Handler

    .thumb
    .thumb_func
    .weak   SVC_Handler
    .type   SVC_Handler, %function
    .fnstart

@ SVC handler
SVC_Handler:
    .weak   SVC_Handler
    b       .
    .fnend
    .size   SVC_Handler, . - SVC_Handler

.end
