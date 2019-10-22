/**
 ****************************************************************************************
 * Copyright (C) 2018 Dialog Semiconductor.
 * Copyright (c) 2011 - 2013 ARM LIMITED
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
 
    .syntax	unified
    .arch	armv6-m

    .section .vectors
    .align 2
        .globl	__Vectors
        .globl	__Vectors_End
        .globl	__Vectors_Size
__Vectors:
    .long	__StackTop            /*     Top of Stack */
    .long	Reset_Handler         /*     Reset Handler */
    .long	NMI_Handler           /* -14 NMI Handler */
    .long	HardFault_Handler     /* -13 Hard Fault Handler */
    .long	0                     /*     Reserved */
    .long	0                     /*     Reserved */
    .long	0                     /*     Reserved */
    .long	0                     /*     Reserved */
    .long	0                     /*     Reserved */
    .long	0                     /*     Reserved */
    .long	0                     /*     Reserved */
    .long	SVC_Handler           /* -5  SVCall Handler */
    .long	0                     /*     Reserved */
    .long	0                     /*     Reserved */
    .long	PendSV_Handler        /* -2  PendSV Handler */
    .long	SysTick_Handler       /* -1  SysTick Handler */

    /* Interrupts */
    .long   BLE_WAKEUP_LP_Handler   /* 0 */
    .long   rwble_isr               /* 1 */
    .long   UART_Handler            /* 2 */
    .long   UART2_Handler           /* 3 */
    .long   I2C_Handler             /* 4 */
    .long   SPI_Handler             /* 5 */
    .long   ADC_Handler             /* 6 */
    .long   KEYBRD_Handler          /* 7 */
    .long   BLE_RF_DIAG_Handler     /* 8 */
    .long   RFCAL_Handler           /* 9 */
    .long   GPIO0_Handler           /* 10 */
    .long   GPIO1_Handler           /* 11 */
    .long   GPIO2_Handler           /* 12 */
    .long   GPIO3_Handler           /* 13 */
    .long   GPIO4_Handler           /* 14 */
    .long   SWTIM_Handler           /* 15 */
    .long   WKUP_QUADEC_Handler     /* 16 */
    .long   SWTIM1_Handler          /* 17 */
    .long   RTC_Handler             /* 18 */
    .long   DMA_Handler             /* 19 */
    .long   XTAL32M_RDY_Handler     /* 20 */
    .long   RESERVED21_Handler      /* 21 */
    .long   RESERVED22_Handler      /* 22 */
    .long   RESERVED23_Handler      /* 23 */

__Vectors_End:

    .equ   __Vectors_Size, __Vectors_End - __Vectors
    .size	__Vectors, . - __Vectors


/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers
 */
.macro	def_irq_handler	handler_name
    .weak	\handler_name
    .set	\handler_name, Default_Handler
.endm

/* Default ISR handlers implementation */
    .thumb
    .section .isr_impl
    .align	2

/* Default handler */
	.thumb_func
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler:
    .weak   Default_Handler
    b       .

/* Other handlers set to Default handler */
def_irq_handler PendSV_Handler
def_irq_handler SysTick_Handler
def_irq_handler BLE_WAKEUP_LP_Handler
def_irq_handler rwble_isr
def_irq_handler UART_Handler
def_irq_handler UART2_Handler
def_irq_handler I2C_Handler
def_irq_handler SPI_Handler
def_irq_handler ADC_Handler
def_irq_handler KEYBRD_Handler
def_irq_handler BLE_RF_DIAG_Handler
def_irq_handler RFCAL_Handler
def_irq_handler GPIO0_Handler
def_irq_handler GPIO1_Handler
def_irq_handler GPIO2_Handler
def_irq_handler GPIO3_Handler
def_irq_handler GPIO4_Handler
def_irq_handler SWTIM_Handler
def_irq_handler WKUP_QUADEC_Handler
def_irq_handler SWTIM1_Handler
def_irq_handler RTC_Handler
def_irq_handler DMA_Handler
def_irq_handler XTAL32M_RDY_Handler
def_irq_handler RESERVED21_Handler
def_irq_handler RESERVED22_Handler
def_irq_handler RESERVED23_Handler
