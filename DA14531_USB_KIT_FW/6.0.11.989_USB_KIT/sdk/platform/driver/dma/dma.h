/**
 ****************************************************************************************
 *
 * @file dma.h
 *
 * @brief DMA Low Level Driver for DA14585, DA14586 and DA14531 devices.
 *
 * Copyright (c) 2018-2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#ifndef DMA_H_
#define DMA_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if defined (CFG_I2C_DMA_SUPPORT) || defined (CFG_SPI_DMA_SUPPORT) || defined (CFG_UART_DMA_SUPPORT) || defined (CFG_ADC_DMA_SUPPORT)

#include <stdint.h>
#include <stddef.h>        // for NULL
#include <stdbool.h>

#include "datasheet.h"
#include "ll.h"

/*
 * DEFINITIONS
 *****************************************************************************************
 */

#define DMA_CHANNEL_MAX 4

/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */

/// @brief DMA channel number ID
typedef enum {
    /// DMA Channel 0 Base Address
    DMA_CHANNEL_0 = DMA0_A_STARTL_REG,
    /// DMA Channel 1 Base Address
    DMA_CHANNEL_1 = DMA1_A_STARTL_REG,
    /// DMA Channel 2 Base Address
    DMA_CHANNEL_2 = DMA2_A_STARTL_REG,
    /// DMA Channel 3 Base Address
    DMA_CHANNEL_3 = DMA3_A_STARTL_REG,
} DMA_ID;

/// @brief DMA channel enable/disable
typedef enum {
    /// DMA disabled
    DMA_STATE_DISABLED,
    /// DMA enabled
    DMA_STATE_ENABLED,
} DMA_STATE_CFG;

/// @brief DMA channel bus width configuration
typedef enum {
    /// 1B bus width
    DMA_BW_BYTE,
    /// 2B bus width
    DMA_BW_HALFWORD,
    /// 4B bus width
    DMA_BW_WORD,
} DMA_BW_CFG;

/// @brief DMA channel interrupt enable/disable
typedef enum {
    /// Disable interrupt on this channel
    DMA_IRQ_STATE_DISABLED,
    /// Enable interrupt on this channel
    DMA_IRQ_STATE_ENABLED,
} DMA_IRQ_CFG;

/// @brief DMA request input multiplexer controlled
typedef enum {
    /// DMA channel starts immediately
    DMA_DREQ_START,
    /// DMA channel must be triggered by peripheral DMA request
    DMA_DREQ_TRIGGERED,
} DMA_DREQ_CFG;

/// @brief Increment address mode
typedef enum {
    /// Do not increment
    DMA_INC_FALSE,
    /// Increment according value of BW
    DMA_INC_TRUE,
} DMA_INC_CFG;

/**
 ****************************************************************************************
 * @brief Channel mode
 *
 * In normal mode the DMA transfer stops the transfer after
 * length DMAx_LEN_REG.
 * In circular mode the DMA channel repeats the transfer
 * after length DMAx_LEN_REG with the initial register values
 * DMAx_A_START_REG, DMAx_B_START_REG, DMAx_LEN_REG, DMAx_INT_REG.
 *
 * @note only works if DREQ_MODE = 1
 ****************************************************************************************
 */
typedef enum {
    /// Normal mode
    DMA_MODE_NORMAL,
    /// Circular mode
    DMA_MODE_CIRCULAR,
} DMA_MODE_CFG;

/**
 ****************************************************************************************
 * @brief Channel priority
 *
 * Set priority level of DMA channel to determine which DMA
 * channel will be activated in case more than one DMA channel
 * requests DMA.
 ****************************************************************************************
 */
typedef enum {
    /// Lowest Priority
    DMA_PRIO_0,
    /// Priority level 1
    DMA_PRIO_1,
    /// Priority level 2
    DMA_PRIO_2,
    /// Priority level 3
    DMA_PRIO_3,
    /// Priority level 4
    DMA_PRIO_4,
    /// Priority level 5
    DMA_PRIO_5,
    /// Priority level 6
    DMA_PRIO_6,
    /// Highest Priority
    DMA_PRIO_7,
} DMA_PRIO_CFG;

/**
 ****************************************************************************************
 * @brief DMA idle mode
 *
 * In blocking mode the DMA performs a fast back-to-back
 * copy, disabling bus access for any bus master with lower priority.
 * In interrupting mode the DMA inserts a wait cycle after each
 * store allowing the CR16 to steal cycles or cache to perform a
 * burst read.
 *
 * @note if DREQ_MODE = 1, DMA_IDLE does not have any effect
 ****************************************************************************************
 */
typedef enum {
    /// Blocking mode
    DMA_IDLE_BLOCKING_MODE,
    /// Interrupting mode
    DMA_IDLE_INTERRUPTING_MODE,
} DMA_IDLE_CFG;

/// @brief DMA init mode
typedef enum {
    /// DMA performs copy A1 to B1, A2 to B2
    DMA_INIT_AX_BX_AY_BY,
    /// DMA performs copy A1 to B1, B2
    DMA_INIT_AX_BX_BY,
} DMA_INIT_CFG;

/// @brief Channel request trigger
typedef enum {
    /// DMA triggered by SPI
    DMA_TRIG_SPI_RXTX = 0x0,
    /// DMA triggered by UART
    DMA_TRIG_UART_RXTX = 0x2,
    /// DMA triggered by UART2
    DMA_TRIG_UART2_RXTX = 0x3,
    /// DMA triggered by I2C
    DMA_TRIG_I2C_RXTX = 0x4,
#if defined (__DA14531__)
    /// DMA triggered by ADC (RX only)
    DMA_TRIG_ADC_RX = 0x5,
#endif
    DMA_TRIG_NONE = 0xF
} DMA_TRIG_CFG;

/// @brief Channel Request Sense
typedef enum {
    /// Level triggered
    DMA_SENSE_LEVEL_SENSITIVE,
    /// Positive edge triggered
    DMA_SENSE_POSITIVE_EDGE_SENSITIVE,
} DMA_SENSE_CFG;


/*
 * TYPE DEFINITIONS
 *****************************************************************************************
 */

/// DMA Channel registers (@ 0x50003600)
typedef struct {
    /// DMA Source start address, lower 16 bits  (@ 0x00000000)
    volatile uint16_t DMA_A_STARTL_REG;
    /// DMA Source start address, upper 16 bits  (@ 0x00000002)
    volatile uint16_t DMA_A_STARTH_REG;
    /// DMA Destination start address, lower 16 bits  (@ 0x00000004)
    volatile uint16_t DMA_B_STARTL_REG;
    /// DMA Destination start address, upper 16 bits  (@ 0x00000006)
    volatile uint16_t DMA_B_STARTH_REG;
    /// DMA Interrupt Register (@ 0x00000008)
    volatile uint16_t DMA_INT_REG;
    /// DMA Length Register (@ 0x0000000A)
    volatile uint16_t DMA_LEN_REG;
    /// DMA Control Register (@ 0x0000000C)
    volatile uint16_t DMA_CTRL_REG;
    /// DMA Index Register (@ 0x0000000E)
    volatile uint16_t DMA_IDX_REG;
} dma_ch_t;

/**
 ****************************************************************************************
 * @brief DMA channel transfer callback
 *
 * @details This function is called by the DMA driver when the
 *          interrupt is fired.
 *
 * @param[in] user_data     Data passed from the user
 * @param[in] len           Length of transferred data
 *
 * @return void
 ****************************************************************************************
 */
typedef void (*dma_cb_t)(void *user_data, uint16_t len);

/// @brief DMA Configuration structure
typedef struct
{
    /// Transfer Bus width: 8, 16 or 32 bits
    DMA_BW_CFG          bus_width:2;
    /// Enable or disable IRQ generation
    DMA_IRQ_CFG         irq_enable:1;
    /// Number of transfers before IRQ generation. Set to 0 to fire IRQ after transfer ends
    uint16_t            irq_nr_of_trans;
    /// Start DMA immediately or triggered by peripheral
    DMA_DREQ_CFG        dreq_mode:1;
    /// Increment of source address
    DMA_INC_CFG         src_inc:1;
    /// Increment of destination address
    DMA_INC_CFG         dst_inc:1;
    ///Select normal or circular operation
    DMA_MODE_CFG        circular:1;
    /// Channel priority from 0 to 7
    DMA_PRIO_CFG        dma_prio:4;
    /// Idle mode: blocking or interrupting
    DMA_IDLE_CFG        dma_idle:1;
    /// Copy mode: block copy or mem init
    DMA_INIT_CFG        dma_init:1;
    /// DMA trigger request source
    DMA_TRIG_CFG        dma_req_mux;
    /// DMA sensitivity level or edge
    DMA_SENSE_CFG       dma_sense:1;
    /// Source address
    uint32_t            src_address;
    /// Destination address
    uint32_t            dst_address;
    /// DMA transfer length.
    uint16_t            length;
    /// Function to call after completion
    dma_cb_t            cb;
    /// Data to pass to Callback
    void                *user_data;
} dma_cfg_t;


/*
 * MACRO DEFINITIONS
 *****************************************************************************************
 */

/// Macro to convert the DMA ID to a pointer to the DMA register structure
#define DMA(id)                 &((dma_ch_t *) id)
/// Macro to get the actual channel number from ID (Channel base Address)
#define DMA_CH_GET(id)          ((id & 0x70) >> 4)
/// Macro to get the ID (Channel Base Address) from the channel number
#define DMA_ID_GET(ch)          (DMA_ID) (DMA_CHANNEL_0 + (ch << 4))


/*
 * FUNCTION DEFINITIONS
 *****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Set DMA source address
 *
 * @param[in] id            DMA channel ID
 * @param[in] src_address   DMA source address
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_src(DMA_ID id, uint32_t src_address){

    SetWord16(DMA(id)->DMA_A_STARTL_REG, src_address & 0xFFFF);
    SetWord16(DMA(id)->DMA_A_STARTH_REG, src_address >> 16);
}

/**
 ****************************************************************************************
 * @brief Set DMA destination address
 *
 * @param[in] id            DMA channel ID
 * @param[in] dst_address   DMA destination address
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_dst(DMA_ID id, uint32_t dst_address){
    SetWord16(DMA(id)->DMA_B_STARTL_REG, dst_address & 0xFFFF);
    SetWord16(DMA(id)->DMA_B_STARTH_REG, dst_address >> 16);
}

/**
 ****************************************************************************************
 * @brief Set DMA interrupt trigger value.
 *
 * @details DMA channel can trigger an interrupt after arbitrary transfer has finished.
 *          Usually interrupt is triggered after transmission finishes, but for circular mode,
 *          where DMA never stops, it is convenient to trigger an interrupt at other times.
 *          This function allows to specify the number of transfers after which the interrupt
 *          is triggered.
 *          The interrupt is generated after a transfer, if DMAx_INT_REG is equal to
 *          DMAx_IDX_REG and before DMAx_IDX_REG is incremented. The bit-field IRQ_ENABLE
 *          of DMAx_CTRL_REG must be set to '1' to let the controller generate the
 *          interrupt.
 *
 * @param[in] id            DMA channel ID
 * @param[in] int_ix        Number of transfers until an interrupt is generated
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_int(DMA_ID id, uint16_t int_ix){
    SetWord16(DMA(id)->DMA_INT_REG, int_ix - 1);
}

/**
 ****************************************************************************************
 * @brief Get DMA Channel interrupt trigger value.
 *
 * @param[in] id            DMA channel ID
 *
 * @return  Number of transfers until an interrupt is generated
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t dma_get_int(DMA_ID id){
    return GetWord16(DMA(id)->DMA_INT_REG) + 1;
}

/**
 ****************************************************************************************
 * @brief Set DMA channel's transfer length.
 *
 * @param[in] id            DMA channel ID
 * @param[in] length        DMA transfer length
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_len(DMA_ID id, uint16_t length){
    SetWord16(DMA(id)->DMA_LEN_REG, length - 1);
}

/**
 ****************************************************************************************
 * @brief Get DMA configured channel's transfer length from DMA_LEN register.
 *
 * @param[in] id            DMA channel ID
 *
 * @return  DMA configured transfer length
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t dma_get_len(DMA_ID id){
    return GetWord16(DMA(id)->DMA_LEN_REG) + 1;
}

/**
 ****************************************************************************************
 * @brief Clear DMA Control register.
 *
 * @param[in] id            DMA channel ID
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_clear_ctrl_reg(DMA_ID id){
    SetWord16(DMA(id)->DMA_CTRL_REG, 0);
}

/**
 ****************************************************************************************
 * @brief Set DMA Control register.
 *
 * @param[in] id            DMA channel ID
 * @param[in] control_val   Control Value
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_ctrl_reg(DMA_ID id, uint16_t control_val){
    SetWord16(DMA(id)->DMA_CTRL_REG, control_val);
}

/**
 ****************************************************************************************
 * @brief Get DMA Configuration from Control register.
 *
 * @param[in] id            DMA channel ID
 *
 * @return  Control Value
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t dma_get_ctrl_reg(DMA_ID id){
    return GetWord16(DMA(id)->DMA_CTRL_REG);
}

/**
 ****************************************************************************************
 * @brief Set DMA request sense in Control Register.
 *
 * @param[in] id            DMA channel ID
 * @param[in] req_sense     DMA_SENSE_CFG
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_req_sense(DMA_ID id, DMA_SENSE_CFG req_sense){
    SetBits16(DMA(id)->DMA_CTRL_REG, REQ_SENSE, req_sense);
}

/**
 ****************************************************************************************
 * @brief Set DMA memory initialization option in Control Register.
 *
 * @param[in] id            DMA channel ID
 * @param[in] init          DMA_INIT_CFG
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_init(DMA_ID id, DMA_INIT_CFG init){
    SetBits16(DMA(id)->DMA_CTRL_REG, DMA_INIT, init);
}

/**
 ****************************************************************************************
 * @brief Set DMA IDLE in Control Register.
 *
 * @param[in] id            DMA channel ID
 * @param[in] idle          DMA_IDLE_CFG
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_idle(DMA_ID id, DMA_IDLE_CFG idle){
    SetBits16(DMA(id)->DMA_CTRL_REG, DMA_IDLE, idle);
}

/**
 ****************************************************************************************
 * @brief Set DMA Priority in Control Register.
 *
 * @param[in] id            DMA channel ID
 * @param[in] prio          DMA_PRIO_CFG
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_prio(DMA_ID id, DMA_PRIO_CFG prio){
    SetBits16(DMA(id)->DMA_CTRL_REG, DMA_PRIO, prio);
}

/**
 ****************************************************************************************
 * @brief Set DMA Circular mode in Control Register.
 *
 * @param[in] id            DMA channel ID
 * @param[in] circular      DMA_MODE_CFG
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_circular(DMA_ID id, DMA_MODE_CFG circular){
    SetBits16(DMA(id)->DMA_CTRL_REG, CIRCULAR, circular);
}

/**
 ****************************************************************************************
 * @brief Set DMA AINC in Control Register.
 *
 * @param[in] id            DMA channel ID
 * @param[in] ainc          DMA_INC_CFG
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_ainc(DMA_ID id, DMA_INC_CFG ainc){
    SetBits16(DMA(id)->DMA_CTRL_REG, AINC, ainc);
}

/**
 ****************************************************************************************
 * @brief Set DMA BINC in Control Register.
 *
 * @param[in] id            DMA channel ID
 * @param[in] binc          DMA_INC_CFG
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_binc(DMA_ID id, DMA_INC_CFG binc){
    SetBits16(DMA(id)->DMA_CTRL_REG, BINC, binc);
}

/**
 ****************************************************************************************
 * @brief Set DMA DREQ in Control Register.
 *
 * @param[in] id            DMA channel ID
 * @param[in] dreq          DMA_DREQ_CFG
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_dreq(DMA_ID id, DMA_DREQ_CFG dreq){
    SetBits16(DMA(id)->DMA_CTRL_REG, DREQ_MODE, dreq);
}

/**
 ****************************************************************************************
 * @brief Set DMA IRQ_ENABLE in Control Register.
 *
 * @param[in] id            DMA channel ID
 * @param[in] irq_en        DMA_IRQ_CFG
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_irq(DMA_ID id, DMA_IRQ_CFG irq_en){
    SetBits16(DMA(id)->DMA_CTRL_REG, IRQ_ENABLE, irq_en);
}

/**
 ****************************************************************************************
 * @brief Set DMA Bus width in Control Register.
 *
 * @param[in] id            DMA channel ID
 * @param[in] bw            DMA_BW_CFG
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_bw(DMA_ID id, DMA_BW_CFG bw){
    SetBits16(DMA(id)->DMA_CTRL_REG, BW, bw);
}

/**
 ****************************************************************************************
 * @brief Read number of so far transmitted bytes.
 *
 * @details Use this function to see how many bytes were transferred
 *          via DMA channel so far. This number can changed very soon.
 *          Number of bytes already transferred when transfer is in progress,
 *          0 when transfer has already finished,
 *          undefined if called or not started channel.
 * @param[in] id            DMA channel ID
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t dma_get_idx(DMA_ID id){
    return GetWord16(DMA(id)->DMA_IDX_REG);
}

/**
 ****************************************************************************************
 * @brief Select which peripherals are mapped on the DMA channels 0 and 1.
 *          The peripherals are mapped as pairs on two channels.
 *
 * @param[in] periph_sel        DMA_TRIG_CFG
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_req_mux_ch01(DMA_TRIG_CFG periph_sel){
    SetBits16(DMA_REQ_MUX_REG, DMA01_SEL, periph_sel);
}

/**
 ****************************************************************************************
 * @brief Get which peripherals are mapped on the DMA channels 0 and 1.
 *
 * @return  DMA_TRIG_CFG
 ****************************************************************************************
 */
__STATIC_FORCEINLINE DMA_TRIG_CFG dma_get_req_mux_ch01(void){
    return (DMA_TRIG_CFG) GetBits16(DMA_REQ_MUX_REG, DMA01_SEL);
}

/**
 ****************************************************************************************
 * @brief Select which peripherals are mapped on the DMA channels 2 and 3.
 *          The peripherals are mapped as pairs on two channels.
 *
 * @param[in] periph_sel        DMA_TRIG_CFG
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_set_req_mux_ch23(DMA_TRIG_CFG periph_sel){
    SetBits16(DMA_REQ_MUX_REG, DMA23_SEL, periph_sel);
}

/**
 ****************************************************************************************
 * @brief Get which peripherals are mapped on the DMA channels 2 and 3.
 *
 * @return  DMA_TRIG_CFG
 ****************************************************************************************
 */
__STATIC_FORCEINLINE DMA_TRIG_CFG dma_get_req_mux_ch23(void){
    return (DMA_TRIG_CFG) GetBits16(DMA_REQ_MUX_REG, DMA23_SEL);
}

/**
 ****************************************************************************************
 * @brief Get the interrupt status of the DMA engine
 *
 * @details <br> bit0. 0: IRQ on channel 0 is not set 1: IRQ on channel 0 is set.
 *          <br> bit1. 0: IRQ on channel 1 is not set 1: IRQ on channel 1 is set.
 *          <br> bit2. 0: IRQ on channel 2 is not set 1: IRQ on channel 2 is set.
 *          <br> bit3. 0: IRQ on channel 3 is not set 1: IRQ on channel 3 is set.
 * @return  DMA interrupt status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t dma_get_int_status(void){
    return GetWord16(DMA_INT_STATUS_REG);
}

/**
 ****************************************************************************************
 * @brief Clear DMA interrupts
 *
 * @param[in] id            DMA channel ID
 * @return  DMA interrupt status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_clear_int_reg(DMA_ID id){
    SetWord16(DMA_CLEAR_INT_REG, 1 << DMA_CH_GET(id));
}

/**
 ****************************************************************************************
 * @brief Freeze DMA
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_freeze(void){
    SetBits16(SET_FREEZE_REG, FRZ_DMA, 1);
}

/**
 ****************************************************************************************
 * @brief Unfreeze DMA
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void dma_unfreeze(void){
    SetBits16(RESET_FREEZE_REG, FRZ_DMA, 1);
}

/**
 ****************************************************************************************
 * @brief Register callback for a specific DMA channel
 *
 * @param[in] id            DMA channel ID
 * @param[in] cb            DMA callback
 * @param[in] user_data     data to be passed in callback function
 *
 * @return void
 ****************************************************************************************
 */
void dma_register_callback(DMA_ID id, dma_cb_t cb, void *user_data);

/**
 ****************************************************************************************
 * @brief Initialize DMA Channel
 *
 * @param[in] id            DMA channel ID
 * @param[in] dma_cfg       pointer to dma_cfg_t type
 *
 * @return void
 ****************************************************************************************
 */
void dma_initialize(DMA_ID id, dma_cfg_t *dma_cfg);

/**
 ****************************************************************************************
 * @brief Start DMA channel
 *
 * @param[in] id            DMA channel ID
 * @param[in] irq_en        DMA_IRQ_CFG
 *
 * @return void
 ****************************************************************************************
 */
void dma_channel_start(DMA_ID id, DMA_IRQ_CFG irq_en);

/**
 ****************************************************************************************
 * @brief Stop DMA channel
 *
 * @param[in] id            DMA channel ID
 *
 * @return void
 ****************************************************************************************
 */
void dma_channel_stop(DMA_ID id);

/**
 ****************************************************************************************
 * @brief Cancel DMA channel if operation is in progress
 *
 * @details If no transfer is in progress nothing happens.
 *          If there is outstanding DMA transfer it will be stopped and
 *          callback will be called with count of data already transferred
 *
 * @param[in] id            DMA channel ID
 *
 * @return void
 ****************************************************************************************
 */
void dma_channel_cancel(DMA_ID id);

/**
 ****************************************************************************************
 * @brief Get DMA State from Control Register.
 *
 * @param[in] id            DMA channel ID
 *
 * @return bool     True if channel is active, else false
 ****************************************************************************************
 */
__STATIC_FORCEINLINE bool dma_get_channel_state(DMA_ID id){
    return GetBits16(DMA(id)->DMA_CTRL_REG, DMA_ON);
}

/**
 ****************************************************************************************
 * @brief Checks whether a DMA channel is enabled (active) or not
 *
 * @return  bool
 *
 * @retval  Channel enabled/disabled
 *          <ul>
 *              <li> true if the DMA channel is enabled
 *              <li> false if the DMA channel is disabled
 *          </ul>
 ****************************************************************************************
 */
bool dma_channel_active(void);

#endif

#endif /* HW_DMA_H_ */

