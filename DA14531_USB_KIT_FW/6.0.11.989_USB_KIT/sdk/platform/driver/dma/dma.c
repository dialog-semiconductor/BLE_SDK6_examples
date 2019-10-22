/**
 ****************************************************************************************
 *
 * @file dma.c
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if defined (CFG_I2C_DMA_SUPPORT) || defined (CFG_SPI_DMA_SUPPORT) || defined (CFG_UART_DMA_SUPPORT) || defined (CFG_ADC_DMA_SUPPORT)

#include <stdint.h>
#include <stddef.h>        // for NULL
#include <stdbool.h>
#include "compiler.h"
#include "dma.h"

/// Registered callback type
typedef struct
{
    /// Callback
    dma_cb_t      callback;
    /// User Data
    void        *user_data;
} dma_reg_cb_t;

/// @brief DMA channel number ID
typedef enum {
    DMA_CR_DMA_ON_POS = 0,
    DMA_CR_DMA_BW_POS = 1,
    DMA_CR_DMA_IRQ_EN_POS = 3,
    DMA_CR_DMA_DREQ_MODE_POS = 4,
    DMA_CR_DMA_BINC_POS = 5,
    DMA_CR_DMA_AINC_POS = 6,
    DMA_CR_DMA_CIRCULAR_POS = 7,
    DMA_CR_DMA_PRIO_POS = 8,
    DMA_CR_DMA_IDLE_POS = 11,
    DMA_CR_DMA_INIT_POS = 12,
    DMA_CR_DMA_REQ_SENSE_POS = 13,
} DMA_CR_POS;

/// Table with registered callbacks.
static dma_reg_cb_t dma_callbacks[DMA_CHANNEL_MAX]  __SECTION_ZERO("retention_mem_area0");

/**
 ****************************************************************************************
 * @brief DMA helper function
 *
 * @param[in] id            DMA channel ID
 * @param[in] len           Transfer length
 * @param[in] stop_dma      Stops DMA if true
 *
 * @return void
 ****************************************************************************************
 */
static void dma_helper(DMA_ID id, uint16_t len, bool stop_dma)
{
    NVIC_DisableIRQ(DMA_IRQn);
    // If not in Circular mode, stop DMA
    if (stop_dma)
    {
        dma_channel_stop(id);
    }
    // Fire callback, if registered
    if (dma_callbacks[DMA_CH_GET(id)].callback)
    {
        dma_callbacks[DMA_CH_GET(id)].callback(dma_callbacks[DMA_CH_GET(id)].user_data, len);
    }
    NVIC_EnableIRQ(DMA_IRQn);
}

/**
 ****************************************************************************************
 * @brief Capture DMA Interrupt Handler
 *
 * @details Calls the user interrupt handler
 *
 * @return void
 ****************************************************************************************
 */
void DMA_Handler(void)
{
    uint16_t risen;
    DMA_ID id;

    risen = dma_get_int_status();

    for (uint8_t i = 0; risen != 0 && i < DMA_CHANNEL_MAX; ++i, risen >>= 1)
    {
        id = DMA_ID_GET(i);
        if (risen & 1)
        {
            bool stop;
            // Stop DMA if it is not in circular mode and transfer is completed
            stop = (!(dma_get_ctrl_reg(id) & CIRCULAR)) &&
                    dma_get_int(id) == dma_get_len(id);

            // Reset the status bit of DMA_INT_STATUS_REG for the current channel
            dma_clear_int_reg(id);

            // Call DMA Helper
            dma_helper(id, dma_get_int(id), stop);
        }
    }
}

void dma_initialize(DMA_ID id, dma_cfg_t *dma_cfg)
{
    // Set DMA Control Register
    dma_set_ctrl_reg(id,    (dma_cfg->dma_sense << DMA_CR_DMA_REQ_SENSE_POS) |
                            (dma_cfg->dma_init << DMA_CR_DMA_INIT_POS) |
                            (dma_cfg->dma_idle << DMA_CR_DMA_IDLE_POS) |
                            (dma_cfg->dma_prio << DMA_CR_DMA_PRIO_POS) |
                            (dma_cfg->circular << DMA_CR_DMA_CIRCULAR_POS) |
                            (dma_cfg->src_inc << DMA_CR_DMA_AINC_POS) |
                            (dma_cfg->dst_inc << DMA_CR_DMA_BINC_POS) |
                            (dma_cfg->dreq_mode << DMA_CR_DMA_DREQ_MODE_POS) |
                            (dma_cfg->irq_enable << DMA_CR_DMA_IRQ_EN_POS) |
                            (dma_cfg->bus_width << DMA_CR_DMA_BW_POS));

    // Set DMA_REQ_MUX_REG for the requested channel / trigger combination
    if (dma_cfg->dma_req_mux != DMA_TRIG_NONE)
    {
        switch (id)
        {
            case DMA_CHANNEL_0:
            case DMA_CHANNEL_1:
                GLOBAL_INT_DISABLE();
                // Clear if trigger for the same peripheral has been previously assigned
                // to different channel.
                if (dma_get_req_mux_ch23() == dma_cfg->dma_req_mux)
                    dma_set_req_mux_ch23(DMA_TRIG_NONE);
                dma_set_req_mux_ch01(dma_cfg->dma_req_mux);
                GLOBAL_INT_RESTORE();
            break;
            case DMA_CHANNEL_2:
            case DMA_CHANNEL_3:
                GLOBAL_INT_DISABLE();
                // Clear if trigger for the same peripheral has been previously assigned
                // to different channel.
                if (dma_get_req_mux_ch01() == dma_cfg->dma_req_mux)
                    dma_set_req_mux_ch01(DMA_TRIG_NONE);
                dma_set_req_mux_ch23(dma_cfg->dma_req_mux);
                GLOBAL_INT_RESTORE();
                break;
            default:
            break;
        }
    }
    // Set source address
    dma_set_src(id, dma_cfg->src_address);
    // Set destination address
    dma_set_dst(id, dma_cfg->dst_address);
    // Set transfer length
    dma_set_len(id, dma_cfg->length);
    // Set IRQ number of transfers
    dma_set_int(id, (dma_cfg->irq_nr_of_trans) ? dma_cfg->irq_nr_of_trans : dma_cfg->length);
    // Register callback
    dma_callbacks[DMA_CH_GET(id)].callback = dma_cfg->cb;
    dma_callbacks[DMA_CH_GET(id)].user_data = dma_cfg->user_data;

    // Set the priority of DMA interrupt to level 2
    NVIC_SetPriority(DMA_IRQn, 2);
    // Enable DMA interrupt
    NVIC_EnableIRQ(DMA_IRQn);
}

void dma_register_callback(DMA_ID id, dma_cb_t cb, void *user_data)
{
    dma_callbacks[DMA_CH_GET(id)].callback = cb;
    dma_callbacks[DMA_CH_GET(id)].user_data = user_data;
}

void dma_channel_start(DMA_ID id, DMA_IRQ_CFG irq_en)
{
    dma_set_irq(id, irq_en);
    SetBits16(DMA(id)->DMA_CTRL_REG, DMA_ON, DMA_STATE_ENABLED);
}

void dma_channel_stop(DMA_ID id)
{
    SetBits16(DMA(id)->DMA_CTRL_REG, DMA_ON, DMA_STATE_DISABLED);
    dma_set_irq(id, DMA_IRQ_STATE_DISABLED);
}

void dma_channel_cancel(DMA_ID id)
{
    dma_helper(id, dma_get_idx(id), true);
}

bool dma_channel_active(void)
{
    bool dma_on;
    dma_on = dma_get_channel_state(DMA_CHANNEL_0) | dma_get_channel_state(DMA_CHANNEL_1) |
             dma_get_channel_state(DMA_CHANNEL_2) | dma_get_channel_state(DMA_CHANNEL_3);

    return dma_on;
}

#endif
