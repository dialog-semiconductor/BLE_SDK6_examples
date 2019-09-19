/*
 ****************************************************************************************
 *
 * @file pdm_mic.c
 *
 * @brief PDM MIC audio interface driver.
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

#include "dma.h"
#include "pdm_mic.h"
#include "app_audio_config.h"

pdm_mic_data_available_cb mic_callback;
bool mic_circular;
uint16_t mic_int_threshold;
DMA_setup DMA_Setup_for_PDM_to_buffer;

/**
 ****************************************************************************************
 * \brief       Function to perform a quick initialization of the DMA instead of a complete one.
 *
 * \param       new_buffer The buffer to store the data to.
 *
 * \return      None
 *
 ****************************************************************************************
 */ 
static void DMA_reinit(uint32_t *new_buffer) {
        DMA_Setup_for_PDM_to_buffer.dest_address = (uint32_t)new_buffer;
        dma_channel_initialization_minimal(&DMA_Setup_for_PDM_to_buffer);
}

/* Note: App should not modify the sleep mode until all messages have been printed out */
#ifdef CFG_AUDIO_DEBUG_PDM_TO_UART
#include "uart.h"

#define PDM_INPUT_BUFFER_LENGTH (1000)

uint8_t uart_finished;
uint32_t input_buffer[PDM_INPUT_BUFFER_LENGTH] __SECTION_ZERO("user_pdm_buffer_area");                         

static void my_uart_callback(uint8_t res)
{
    #ifdef CFG_PRINTF_UART2
        uart2_finish_transfers();
    #else
        uart_finish_transfers();
    #endif
        uart_finished = true;
}

/**
 ****************************************************************************************
 * \brief       DMA Callback that dumps the data through UART
 *
 * \param       user_data & length provided from DMA handler
 *
 * \return      None
 *
 ****************************************************************************************
*/ 
void DMA_callback(void *user_data, uint16_t len)
{
    #ifdef USE_AUDIO_MARK
        GPIO_ConfigurePin(AUDIO_MARK_PORT, AUDIO_MARK_PIN, OUTPUT, PID_GPIO, true );
    #endif
       uint8_t *my_buf = (uint8_t*) &input_buffer;

        for (uint32_t i = 0; i < PDM_INPUT_BUFFER_LENGTH * 4; i++) {
					uart_finished = false;
          uart2_write(my_buf, 1, my_uart_callback);
          while(uart_finished == false);
          my_buf++;
        }

       while(1);

    #ifdef USE_AUDIO_MARK                    
        GPIO_ConfigurePin(AUDIO_MARK_PORT, AUDIO_MARK_PIN, OUTPUT, PID_GPIO, false );
    #endif
}

#else
/**
 ****************************************************************************************
 * \brief       DMA Callback that reads the pdm data and restarts the DMA
 *
 * \param       user_data & length provided from DMA handler
 *
 * \return      None
 *
 ****************************************************************************************
*/
void DMA_callback(void *user_data, uint16_t len)
{
    #ifdef USE_AUDIO_MARK                    
        GPIO_ConfigurePin(AUDIO_MARK_PORT, AUDIO_MARK_PIN, OUTPUT, PID_GPIO, true );
    #endif                
        ASSERT_ERROR(mic_callback != NULL); // mic_callback should be provided
            
        if(mic_circular == true) {
            uint32_t *new_buffer_pos = mic_callback(dma_channel_transfered_bytes(DMA_CHANNEL_2));
            dma_channel_update_int_ix(DMA_CHANNEL_2, (uint32_t)new_buffer_pos + mic_int_threshold-1);
        }
        else {            
            if (GetBits16(DMA2_CTRL_REG, DMA_ON) == true) {
                ASSERT_ERROR(0);
            }
            uint32_t * buffer = mic_callback(DMA_Setup_for_PDM_to_buffer.length);
            DMA_reinit(buffer);
            dma_channel_enable(DMA_CHANNEL_2, DMA_STATE_ENABLED);
        }

    #ifdef USE_AUDIO_MARK                    
        GPIO_ConfigurePin(AUDIO_MARK_PORT, AUDIO_MARK_PIN, OUTPUT, PID_GPIO, false );
    #endif   
}

#endif

/**
 ****************************************************************************************
 * \brief       DMA_init 
 *
 * \param       user_data & length provided from DMA handler
 *
 * \return      None
 *
 ****************************************************************************************
*/
static void DMA_init(uint32_t *buffer, uint16_t length)
{
    DMA_Setup_for_PDM_to_buffer.channel_number = DMA_CHANNEL_2;
    DMA_Setup_for_PDM_to_buffer.src_address = (uint32_t)SRC1_OUT1_REG;

    DMA_Setup_for_PDM_to_buffer.a_inc = DMA_AINC_FALSE;
    DMA_Setup_for_PDM_to_buffer.b_inc = DMA_BINC_TRUE;
    DMA_Setup_for_PDM_to_buffer.bus_width = DMA_BW_WORD;
    DMA_Setup_for_PDM_to_buffer.callback = DMA_callback;
    DMA_Setup_for_PDM_to_buffer.dma_idle = DMA_IDLE_INTERRUPTING_MODE; 
    DMA_Setup_for_PDM_to_buffer.dma_init = DMA_INIT_AX_BX_AY_BY;
    DMA_Setup_for_PDM_to_buffer.dma_prio = DMA_PRIO_3; 
    DMA_Setup_for_PDM_to_buffer.dma_req_mux = DMA_TRIG_PDM_LEFTRIGHT;
    DMA_Setup_for_PDM_to_buffer.dreq_mode = DMA_DREQ_TRIGGERED;
    DMA_Setup_for_PDM_to_buffer.irq_enable = DMA_IRQ_STATE_ENABLED;
    DMA_Setup_for_PDM_to_buffer.user_data = NULL;
		DMA_Setup_for_PDM_to_buffer.dma_sense = DMA_SENSE_LEVEL_SENSITIVE;

#ifdef CFG_AUDIO_DEBUG_PDM_TO_UART 
    DMA_Setup_for_PDM_to_buffer.circular = DMA_MODE_NORMAL;
    DMA_Setup_for_PDM_to_buffer.dest_address = (uint32_t)input_buffer;
    DMA_Setup_for_PDM_to_buffer.length = PDM_INPUT_BUFFER_LENGTH;
    DMA_Setup_for_PDM_to_buffer.irq_nr_of_trans = 0;
#else
    DMA_Setup_for_PDM_to_buffer.circular = (mic_circular == true) ? DMA_MODE_CIRCULAR : DMA_MODE_NORMAL;
    DMA_Setup_for_PDM_to_buffer.length = length;
    DMA_Setup_for_PDM_to_buffer.dest_address = (uint32_t)buffer;
    DMA_Setup_for_PDM_to_buffer.irq_nr_of_trans = mic_int_threshold;
#endif
    dma_channel_initialization(&DMA_Setup_for_PDM_to_buffer);
}

void pdm_mic_start(pdm_mic_setup_t *config)
{
    mic_callback = config->callback;
    mic_circular = config->buffer_circular;
    mic_int_threshold = config->int_thresold;
    
	pdm_config_t pdm_config;
	pdm_config.clk_gpio = config->clk_gpio;
	pdm_config.data_gpio = config->data_gpio;
	pdm_config.mode = PDM_MODE_MASTER;
	pdm_config.direction = PDM_DIRECTION_IN;
	pdm_config.src_direction = PDM_SRC_DIRECTION_REG;
    pdm_config.src_sample_rate = config->sampling_rate;
	pdm_config.bypass_out_filter = false;
	pdm_config.bypass_in_filter = false;
	pdm_config.enable_dithering = true;
	pdm_config.pdm_div = 5;
	pdm_config.enable_interrupt = false;
	pdm_config.interrupt_priority = 3;
	pdm_config.callback = NULL;
	pdm_enable(&pdm_config);
	DMA_init(config->buffer, config->buffer_length);
	dma_channel_enable(DMA_Setup_for_PDM_to_buffer.channel_number, DMA_STATE_ENABLED);
}

void pdm_mic_stop(void)
{
		//assumption is that pdm_mic_stop is called after DMA_init thus
		//DMA_Setup_for_PDM_to_buffer.channel_number is valid
    dma_channel_stop(DMA_Setup_for_PDM_to_buffer.channel_number);
    pdm_disable();
}


