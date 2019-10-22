/*
 ****************************************************************************************
 *
 * @file pdm.c
 *
 * @brief PDM audio interface driver.
 *
 * Copyright (c) 2017-2018 Dialog Semiconductor. All rights reserved.
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

#include "pdm.h"
#include <assert.h>

static pdm_src_interrupt_cb pdm_src_callback;

/**
 ****************************************************************************************
 * \brief   Configures the GPIOs used for PDM
 *          
 * \param[in]  config The GPIOs assigned to PDM
 *
 * \return  None
 ****************************************************************************************
 */
static void pdm_config_port_pins(const pdm_config_t *config)
{
        if (config->mode == PDM_MODE_MASTER) {
                GPIO_ConfigurePin(config->clk_gpio.port, config->clk_gpio.pin,
                        OUTPUT, PID_PDM_CLK, false);
        } else {
                GPIO_ConfigurePin(config->clk_gpio.port, config->clk_gpio.pin,
                        INPUT, PID_PDM_CLK, false);
        }

        if (config->direction == PDM_DIRECTION_IN) {
                GPIO_ConfigurePin(config->data_gpio.port, config->data_gpio.pin,
                        INPUT, PID_PDM_DATA, false);
        } else {
                GPIO_ConfigurePin(config->data_gpio.port, config->data_gpio.pin,
                        OUTPUT, PID_PDM_DATA, false);
        }
}

/**
 ****************************************************************************************
 * \brief   Sets the clock of the PDM peripheral and enables the PDM
 *          
 * \param[in]  pdm_clock_divider The clock divider which determines the PDM clock
 *
 * \return  None
 ****************************************************************************************
 */
static void pdm_set_master_clock(unsigned int pdm_clock_divider)
{
        unsigned int div_field;
        if (pdm_clock_divider == 0) {
                div_field = 8;
        } else {
                div_field = pdm_clock_divider;
        }

        GLOBAL_INT_DISABLE();
        uint16_t reg_val = GetWord16(PDM_DIV_REG);
        SetBits16(&reg_val, PDM_DIV, div_field);
        SetBits16(&reg_val, CLK_PDM_EN, 1);
        SetWord16(PDM_DIV_REG, reg_val);
        GLOBAL_INT_RESTORE();
}

/**
 ****************************************************************************************
 * \brief   Sets the clock of the SRC peripheral
 *          
 * \param None
 *
 * \return  None
 ****************************************************************************************
 */
static void pdm_set_src_clock()
{
        GLOBAL_INT_DISABLE();
        uint16_t reg_val = GetWord16(SRC_DIV_REG);
        SetBits16(&reg_val, SRC_DIV, 1);
        SetBits16(&reg_val, CLK_SRC_EN, 1);
        SetWord16(SRC_DIV_REG, reg_val);
        GLOBAL_INT_RESTORE();
}

/**
 ****************************************************************************************
 * \brief       Enables the interrupt of the PDM 
 *
 * \param       
 *
 * \return      None
 *
 ****************************************************************************************
 */
static void pdm_enable_interrupt(bool enable_interrupt, unsigned int int_prio,
        pdm_direction_t direction, pdm_src_interrupt_cb callback)
{
        if (enable_interrupt) {
                pdm_src_callback = callback;

                if (direction == PDM_DIRECTION_IN) {
                        NVIC_ClearPendingIRQ(SRC_OUT_IRQn);
                        NVIC_SetPriority(SRC_OUT_IRQn, int_prio);
                        NVIC_EnableIRQ(SRC_OUT_IRQn);
                } else {
                        NVIC_ClearPendingIRQ(SRC_IN_IRQn);
                        NVIC_SetPriority(SRC_IN_IRQn, int_prio);
                        NVIC_EnableIRQ(SRC_IN_IRQn);
                }
        } else {
                pdm_src_callback = NULL;
        }
}

/**
 ****************************************************************************************
 * \brief       Sets the SRC peripheral sample rate
 *
 * \param       
 *
 * \return      None
 *
 ****************************************************************************************
 */
static void pdm_set_src_sample_rate(pdm_direction_t direction, unsigned int sample_rate)
{
        uint32_t fs_reg;
        unsigned int multiplier = 8192;

        /* sample_rate is up to 192000. */
        /* XXX: Maximum HW supported sample rate resolution is in fact 100 / 8192 Hz. */
        if (sample_rate > 96000) {
                multiplier = 8192 / 4;
        } else if (sample_rate > 48000) {
                multiplier = 8192 / 2;
        }
        fs_reg = (multiplier * sample_rate / 100);

        if (direction == PDM_DIRECTION_IN) {
                SetBits32(SRC1_OUT_FS_REG, SRC_OUT_FS, fs_reg);
        } else {
                SetBits32(SRC1_IN_FS_REG, SRC_IN_FS, fs_reg);
        }
}

/**
 ****************************************************************************************
 * \brief       Sets the SRC peripheral filters
 *
 * \param       
 *
 * \return      None
 *
 ****************************************************************************************
 */
static void pdm_set_src_filters(pdm_direction_t direction, unsigned int sample_rate)
{
        int setting = 0;

        if (sample_rate > 96000) {
                setting = 3;
        } else if (sample_rate > 48000) {
                setting = 1;
        }

        if (direction == PDM_DIRECTION_IN) {
                SetBits32(SRC1_CTRL_REG, SRC_OUT_US, setting);
        } else {
                SetBits32(SRC1_CTRL_REG, SRC_IN_DS, setting);
        }
}

/**
 ****************************************************************************************
 * \brief       Configures the APU based on the PDM and SRC directions
 *
 * \param       
 *
 * \return      None
 *
 ****************************************************************************************
 */
static void pdm_set_apu_reg(pdm_direction_t direction, pdm_src_direction_t src_direction)
{
        if (src_direction == PDM_SRC_DIRECTION_REG) {
                if (direction == PDM_DIRECTION_IN) {
                        SetBits16(APU_MUX_REG, PDM1_MUX_IN, 1);
                } else {
                        SetBits16(APU_MUX_REG, SRC1_MUX_IN, 2);
                        SetBits16(APU_MUX_REG, PDM1_MUX_IN, 0);
                }
        } else {
                if (direction == PDM_DIRECTION_IN) {
                        SetBits16(APU_MUX_REG, PDM1_MUX_IN, 1);
                        SetBits16(APU_MUX_REG, PCM1_MUX_IN, 1);
                } else {
                        SetBits16(APU_MUX_REG, SRC1_MUX_IN, 1);
                        SetBits16(APU_MUX_REG, PDM1_MUX_IN, 0);
                }
        }
 }

/**
 ****************************************************************************************
 * \brief       Enables the PDM
 *
 * \param       config The configuration of the PDM
 *
 * \return      None
 *
 ****************************************************************************************
 */ 
void pdm_enable(const pdm_config_t *config)
{
        pdm_disable();

        pdm_config_port_pins(config);
        GLOBAL_INT_DISABLE();
        SetBits16(PDM_DIV_REG, PDM_MASTER_MODE, config->mode & 0x1);
        GLOBAL_INT_RESTORE();
        if (config->mode == PDM_MODE_MASTER) {
                pdm_set_master_clock(config->pdm_div);
        }

        pdm_enable_interrupt(config->enable_interrupt, config->interrupt_priority,
                config->direction, config->callback);

        pdm_set_apu_reg(config->direction, config->src_direction);
        pdm_set_src_clock();

        if (config->src_direction == PDM_SRC_DIRECTION_REG) {
                pdm_set_src_sample_rate(config->direction, config->src_sample_rate);
        }

        pdm_set_src_filters(config->direction, config->src_sample_rate);
        SetBits32(SRC1_CTRL_REG, SRC_OUT_AMODE,
                (config->direction == PDM_DIRECTION_IN &&
                        config->src_direction == PDM_SRC_DIRECTION_PCM) ? 1 : 0);
        SetBits32(SRC1_CTRL_REG, SRC_IN_AMODE,
                (config->direction == PDM_DIRECTION_IN ||
                        config->src_direction == PDM_SRC_DIRECTION_PCM) ? 1 : 0);
        SetBits32(SRC1_CTRL_REG, SRC_PDM_MODE, config->direction & 0x3);
        SetBits32(SRC1_CTRL_REG, SRC_OUT_CAL_BYPASS, config->bypass_out_filter & 0x1);
        SetBits32(SRC1_CTRL_REG, SRC_IN_CAL_BYPASS, config->bypass_in_filter & 0x1);
        SetBits32(SRC1_CTRL_REG, SRC_DITHER_DISABLE, !config->enable_dithering & 0x1);

        SetBits32(SRC1_CTRL_REG, SRC_EN, 1);
}

/**
 ****************************************************************************************
 * \brief       Disables the PDM
 *
 * \param       None
 *
 * \return      None
 *
 ****************************************************************************************
 */ 
void pdm_disable(void)
{
        NVIC_DisableIRQ(SRC_IN_IRQn);
        NVIC_DisableIRQ(SRC_OUT_IRQn);
        GLOBAL_INT_DISABLE();
        SetBits16(PDM_DIV_REG, CLK_PDM_EN, 0);
        SetBits16(SRC_DIV_REG, CLK_SRC_EN, 0);
        GLOBAL_INT_RESTORE();
}

#if !defined (__EXCLUDE_SRCIN_HANDLER__)
/**
 ****************************************************************************************
 * \brief       The handler for the Sample Rate Converter (SRC) input interrupt
 *
 * \param       None
 *
 * \return      None
 *
 ****************************************************************************************
 */ 
void SRCIN_Handler(void)
{
        if (pdm_src_callback) {
                pdm_src_isr_data_t data;
                data.src_out1_value = 0;
                data.src_out2_value = 0;

                data.src_in1_reg = (uint32_t *)SRC1_IN1_REG;
                data.src_in2_reg = (uint32_t *)SRC1_IN2_REG;

                pdm_src_callback(&data);
        }
}
#endif

#if !defined (__EXCLUDE_SRCOUT_HANDLER__)
/**
 ****************************************************************************************
 * \brief       The handler for the Sample Rate Converter (SRC) output interrupt
 *
 * \param       None
 *
 * \return      None
 *
 ****************************************************************************************
 */ 
void SRCOUT_Handler(void)
{
        if (pdm_src_callback) {
                pdm_src_isr_data_t data;

                data.src_out1_value = *(uint32_t*)SRC1_OUT1_REG;
                data.src_out2_value = *(uint32_t*)SRC1_OUT2_REG;

                data.src_in1_reg = (uint32_t *) NULL;
                data.src_in2_reg = (uint32_t *) NULL;

                pdm_src_callback(&data);
        }
}
#endif
