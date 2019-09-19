/**
 ****************************************************************************************
 *
 * @file pdm_mic.h
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

#ifndef PDM_MIC_H_
#define PDM_MIC_H_

#include "pdm.h"

/**
 \brief PDM interface sample rates. Possible values: PDM_8000, PDM_11025, PDM_16000, PDM_22050,
 PDM_32000, PDM_44100, PDM_48000, PDM_88200, PDM_96000, PDM_176400 or PDM_192000.
 */
typedef enum {
        PDM_8000 = 8000,
        PDM_11025 = 11025,
        PDM_16000 = 16000,
        PDM_22050 = 22050,
        PDM_32000 = 32000,
        PDM_44100 = 44100,
        PDM_48000 = 48000,
        PDM_88200 = 88200,
        PDM_96000 = 96000,
        PDM_176400 = 176400,
        PDM_192000 = 192000
} SRC_SampleRate_t;

typedef uint32_t * (*pdm_mic_data_available_cb)(uint16_t length);

typedef struct pdm_mic_setup_s {
    uint32_t *buffer;
    uint16_t buffer_length;
    bool buffer_circular;
    uint16_t int_thresold;
    pdm_gpio_t clk_gpio;
    pdm_gpio_t data_gpio;
    SRC_SampleRate_t sampling_rate;
    pdm_mic_data_available_cb   callback;
} pdm_mic_setup_t;

/**
 ****************************************************************************************
 * \brief       Start MIC recording
 *
 * \param [in]  config MIC configuration
 ****************************************************************************************
 */
void pdm_mic_start(pdm_mic_setup_t *config);

/**
 ****************************************************************************************
 * \brief       Stop MIC recording
 ****************************************************************************************
 */
void pdm_mic_stop(void);

#endif /* PDM_MIC_H_ */

