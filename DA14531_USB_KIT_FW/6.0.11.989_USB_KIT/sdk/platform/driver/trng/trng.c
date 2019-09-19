/**
 ****************************************************************************************
 *
 * @file trng.c
 *
 * @brief True Random Number Generator.
 *
 * Copyright (c) 2012-2019 Dialog Semiconductor. All rights reserved.
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
Revision History (ver. 2.0)

ver. 1.0  Initial version
ver. 1.1  Modified rfpt_init() , Added save/restore of the TEST_CTRL_REG in trng_acquire()
ver. 1.2  Added option for configurable buffer size to trade-off between code size and boot latency.
ver  2.0  Added true random number generator explicitly for DA14531
*/

#include "arch.h"

#if (USE_TRNG)
#include <stdlib.h>
#include <stdint.h>
#include "datasheet.h"
#include "trng.h"

#if (USE_CHACHA20_RAND)
#include "chacha20.h"
#else
#include "co_math.h"
#endif

#if defined (__DA14531__)
#include "adc.h"
#include "ke_mem.h"
#include "iid_return_codes.h"
#else
#include "gpio.h"
#include "arch_wdg.h"
#include "rf_585.h"
#include "system_library.h"
#endif

#if defined (__DA14531__)

// Configuration for AD converter
static const adc_config_t cfg =
{
    .input_mode       = ADC_INPUT_MODE_SINGLE_ENDED,
    .input            = ADC_INPUT_SE_TEMP_SENS,
    .smpl_time_mult   = 0,
    .continuous       = false,
    .interval_mult    = 0,
    .input_attenuator = ADC_INPUT_ATTN_4X,
    .chopping         = false,
    .oversampling     = 0
};

#define ADC_ENTRIES 0x3
// Buffer for ADC samples
static uint8_t adc_samples[ADC_ENTRIES];

 /**
 ****************************************************************************************
 * @brief  Helper function that acquires three 16-bit values from ADC
 * @return void
 ****************************************************************************************
 */
static void get_adc_values(void)
{
    // Save ADC settings
    uint16_t adc_ctrl_reg = GetWord16(GP_ADC_CTRL_REG);
    uint16_t adc_ctr2_reg = GetWord16(GP_ADC_CTRL2_REG);
    uint16_t adc_ctr3_reg = GetWord16(GP_ADC_CTRL3_REG);
    uint16_t adc_sel_reg = GetWord16(GP_ADC_SEL_REG);

    adc_init(&cfg);

    // Get sample from Temperature Sensor
    adc_samples[0] = (uint8_t) (adc_get_sample() >> 6);

    adc_set_se_input(ADC_INPUT_SE_VBAT_LOW);
    // Get sample from VBAT_LOW
    adc_samples[1] = (uint8_t) (adc_get_sample() >> 6);

    adc_set_se_input(ADC_INPUT_SE_VBAT_HIGH);
    // Get sample from VBAT_HIGH
    adc_samples[2] = (uint8_t) (adc_get_sample() >> 6);

    // Restore ADC settings
    SetWord16(GP_ADC_CTRL_REG, adc_ctrl_reg);
    SetWord16(GP_ADC_CTRL2_REG, adc_ctr2_reg);
    SetWord16(GP_ADC_CTRL3_REG, adc_ctr3_reg);
    SetWord16(GP_ADC_SEL_REG, adc_sel_reg);
}
#else
// RFPT acquisition definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TRNG_BUFFER_SIZE        (CFG_TRNG)
#define TRNG_DATA_LEN           (1024)                          // Number of points per acquisition
#define NUM_POINTS              (TRNG_DATA_LEN /2)              // Number of points per acquisition
#define NUM_POINTS_ITER         (TRNG_BUFFER_SIZE / 2)          // Number of points per iteration
#define NUM_ITER                (NUM_POINTS / NUM_POINTS_ITER)  // Number of iterations
#define NUM_POINTS_MUL_2_M_4    (NUM_POINTS * 2 - 4)            // Number of iters to run the mean_calc loop

// Bul def...
#define WAIT100() for (wait=0; wait < 0x91; wait++) {}          // Wait about 100 us

// TRNG I/Q sample buffer. This buffer must be located lower than the
// 64K boundary in SysRAM. I.e. it cannot be placed in SysRAM4.
static uint8_t rfadc_data_buffer[TRNG_BUFFER_SIZE] __SECTION_ZERO("trng_buffer_area_zi");


/**
 ****************************************************************************************
 * @brief Initialize the rfpt
 ****************************************************************************************
 */
extern void rf_regs(void);
static void rfpt_init(void)
{
    volatile uint16_t wait;

    SetWord16(CLK_RADIO_REG, 0x0089);   // En BLE clk, dis BLE timer, En RFCU, RFCU div by 2
    SetWord16(BLE_CNTL2_REG, 0x2000);
    SetBits16(CLK_AMBA_REG, OTP_ENABLE, 1);
    SetBits16(CLK_AMBA_REG, HCLK_DIV, 0);
    SetBits16(TEST_CTRL_REG, ENABLE_RFPT, 1);

    rf_regs();

#if (LUT_PATCH_ENABLED)
    IffCalibration_580();
    DCoffsetCalibration_580();
#else
    DCoffsetCalibration();
    PllLF_IFF_MIX_CapCalibration();
#endif
    WAIT100();                          // Wait 100 us to finish all DCF signals
}

/**
 ****************************************************************************************
 * @brief Override RF preferred settings for TRNG mode
 ****************************************************************************************
 */
static void trng_init(void)
{
    volatile uint16_t  wait;

    // Override preferred settings for TRNG mode
    SetWord16(RF_ENABLE_CONFIG1_REG,  0x0000);  // LNA off
    SetWord16(RF_ENABLE_CONFIG2_REG,  0x0000);  // Mixer off
    SetWord16(RF_DC_OFFSET_CTRL1_REG, 0x8080);  // Fixed DC offset compensation values for I and Q
    SetWord16(RF_DC_OFFSET_CTRL2_REG, 0x01D1);  // Use the manual DC offset compensation values
    SetWord16(RF_ENABLE_CONFIG4_REG,  0x0000);  // VCO_LDO_EN=0, MD_LDO_EN=0. You need this for more isolation from the RF input
//    SetWord16(RF_ENABLE_CONFIG14_REG, 0x0000);   // LOBUF_RXIQ_EN=0, DIV2_EN=0. This increases the noise floor for some reason. So you get more entropy. Need to understand it and then decide...
//    SetWord16(RF_SPARE1_REG,          0x0001);   // Set the IFF in REAL transfer function, to remove I-Q correlation. But it affects the DC offsets!
    SetWord16(RF_AGC_CTRL2_REG,       0x00C3);  // AGC=0 i.e. max RX gain

    // Overrule-start the RX
    SetWord16(RF_OVERRULE_REG,0x8);     // Start up receiver
    WAIT100();                          // Wait for the DCFs to finish
    WAIT100();                          // Wait for the DCFs to finish
    WAIT100();                          // Wait for the DCFs to finish
}

/**
 ****************************************************************************************
 * @brief  Acquires the raw RFADC IQ samples
 ****************************************************************************************
 */
static void trng_get_raw_data(uint32_t * address)
{
#if (TRNG_BUFFER_SIZE == 1024)
    SetWord16(RFPT_ADDR_REG, (uint32_t) address);
    SetWord16(RFPT_LEN_REG, TRNG_BUFFER_SIZE/4-1);
    SetWord16(RFPT_CTRL_REG, 0x1);
    // Wait for the transfer to complete. RFPT_CTRL_REG resets upon completion.
    while ((GetWord16(RFPT_STAT_REG) & RFPT_ACTIVE) == RFPT_ACTIVE);

#else
    // Depending on the configured buffer size, collect data and each time copy them
    // to the given address.
    for (uint8_t i = 0; i < NUM_ITER; i++)
    {
        // Reset address stored to RFPT_ADDR_REG. It points to next address, after capturing completion.
        SetWord16(RFPT_ADDR_REG, (uint32_t) rfadc_data_buffer);
        // Reset length stored to RFPT_LEN_REG. It is self-cleared to 0,  after capturing completion.
        SetWord16(RFPT_LEN_REG, TRNG_BUFFER_SIZE/4-1);
        // Start capturing data.
        SetWord16(RFPT_CTRL_REG, 0x1);
        // Wait for the transfer to complete. RFPT_CTRL_REG resets upon completion.
        while ((GetWord16(RFPT_STAT_REG) & RFPT_ACTIVE) == RFPT_ACTIVE);
        memcpy(address, rfadc_data_buffer, TRNG_BUFFER_SIZE);
        address += TRNG_BUFFER_SIZE / 4;
    }
#endif // TRNG_BUFFER_SIZE == 1024

}

 /**
 ****************************************************************************************
 * @brief  Acquires 128 bits, saves them in trng_bits[] and restores the modified regs
 ****************************************************************************************
 */
void trng_acquire(uint8_t *trng_bits_ptr)
{
    uint16_t i,i_acq;
    uint16_t save_TEST_CTRL_REG, save_RF_ENABLE_CONFIG1_REG,save_RF_ENABLE_CONFIG2_REG,save_RF_DC_OFFSET_CTRL1_REG,save_RF_DC_OFFSET_CTRL2_REG;
    uint16_t save_RF_ENABLE_CONFIG4_REG,save_RF_ENABLE_CONFIG14_REG,save_RF_SPARE1_REG,save_RF_AGC_CTRL2_REG;
    uint8_t bit_cnt, rnd_byte, single_rnd_bit;
    uint16_t byte_idx;
#if (TRNG_BUFFER_SIZE == 1024)
    // No intermediate buffer will be used. Assign address of rfadc_data to rfadc_data_buffer.
    uint8_t * rfadc_data = rfadc_data_buffer;
#else
    // rfadc_data_buffer will be used as an intermediate buffer. Data will be copied in rfadc_data.
    uint8_t rfadc_data[TRNG_DATA_LEN];
#endif
    uint8_t * vq_uint8=(uint8_t *)&rfadc_data[0];   // The Q RFADC channel
    uint8_t * vi_uint8=(uint8_t *)&rfadc_data[2];   // The I RFADC channel

    save_TEST_CTRL_REG =GetWord16(TEST_CTRL_REG);

    rfpt_init(); // Init system and radio, set pref settings, and perform radio calibrations. Do not Radio Overrule yet...

    // Implement Save-Modify-Restore for the prefered settings that will be changed for the TRNG mode
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    save_RF_ENABLE_CONFIG1_REG  = GetWord16(RF_ENABLE_CONFIG1_REG);  // LNA off
    save_RF_ENABLE_CONFIG2_REG  = GetWord16(RF_ENABLE_CONFIG2_REG);  // Mixer off
    save_RF_DC_OFFSET_CTRL1_REG = GetWord16(RF_DC_OFFSET_CTRL1_REG); // Fixed DC offset compensation values for I and Q
    save_RF_DC_OFFSET_CTRL2_REG = GetWord16(RF_DC_OFFSET_CTRL2_REG); // Use the manual DC offset compensation values
    save_RF_ENABLE_CONFIG4_REG  = GetWord16(RF_ENABLE_CONFIG4_REG);  // VCO_LDO_EN=0, MD_LDO_EN=0. You need this for more isolation from the RF input
    save_RF_ENABLE_CONFIG14_REG = GetWord16(RF_ENABLE_CONFIG14_REG); // LOBUF_RXIQ_EN=0, DIV2_EN=0. This increases the noise floor for some reason. So you get more entropy. Need to understand it and then decide...
    save_RF_SPARE1_REG          = GetWord16(RF_SPARE1_REG);          // Set the IFF in REAL transfer function, to remove I-Q correlation. But it affects the DC offsets!
    save_RF_AGC_CTRL2_REG       = GetWord16(RF_AGC_CTRL2_REG);       // AGC=0 i.e. max RX gain

    // Configure radio for TRNG mode (modifies some pref settings, starts RX in overrule)
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    trng_init();
    // Start acquiring raw IQ RFADC data and then extract the random bits
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    byte_idx=0;
    for (i_acq=0; i_acq < 128/(NUM_POINTS*2/16); i_acq++)
    {
        trng_get_raw_data((uint32_t *)&rfadc_data[0]); // acquires the raw RFADC IQ samples
        bit_cnt=0;
        rnd_byte=0;

        for (i=0;i<=NUM_POINTS_MUL_2_M_4;i=i+16)
        {
            single_rnd_bit = (vq_uint8[i] & 0x01)  ^  (vi_uint8[i] & 0x01) ; // This way it can pass ALL the NIST tests! This solves a small bias in 1s or 0s which appears due to the actual value of the DC offset...

            rnd_byte= rnd_byte | (single_rnd_bit<<bit_cnt++);
            if(bit_cnt==8)
            {
                trng_bits_ptr[byte_idx++] = rnd_byte;
                bit_cnt=0;
                rnd_byte=0;
            }
        if (USE_WDOG)
            wdg_reload(WATCHDOG_DEFAULT_PERIOD);          // Reset WDOG! 200 * 10.24ms active time for normal mode!
        }
    }

    // Restore the modified regs
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    SetWord16(TEST_CTRL_REG,  save_TEST_CTRL_REG);

    SetWord16(RF_OVERRULE_REG,0x0);
    SetWord16(RF_ENABLE_CONFIG1_REG,  save_RF_ENABLE_CONFIG1_REG);  // LNA off
    SetWord16(RF_ENABLE_CONFIG2_REG,  save_RF_ENABLE_CONFIG2_REG);  // Mixer off
    SetWord16(RF_DC_OFFSET_CTRL1_REG, save_RF_DC_OFFSET_CTRL1_REG); // Fixed DC offset compensation values for I and Q
    SetWord16(RF_DC_OFFSET_CTRL2_REG, save_RF_DC_OFFSET_CTRL2_REG); // Use the manual DC offset compensation values
    SetWord16(RF_ENABLE_CONFIG4_REG,  save_RF_ENABLE_CONFIG4_REG);  // VCO_LDO_EN=0, MD_LDO_EN=0. You need this for more isolation from the RF input
    SetWord16(RF_ENABLE_CONFIG14_REG, save_RF_ENABLE_CONFIG14_REG); // LOBUF_RXIQ_EN=0, DIV2_EN=0. This increases the noise floor for some reason. So you get more entropy. Need to understand it and then decide...
    SetWord16(RF_SPARE1_REG,          save_RF_SPARE1_REG);          // Set the IFF in REAL transfer function, to remove I-Q correlation. But it affects the DC offsets!
    SetWord16(RF_AGC_CTRL2_REG,       save_RF_AGC_CTRL2_REG);       // AGC=0 i.e. max RX gain

    SetBits16 (CLK_AMBA_REG, OTP_ENABLE, 0);

}
#endif

uint8_t trng_bits[TRNG_SEED_SIZE] __ALIGNED(4);

void init_rand_seed_from_trng(void)
{
#if defined (__DA14531__)
    uint8_t status = IID_SUCCESS;
    uint8_t *buffer = ke_malloc(TRNG_RAM_SIZE_USED, KE_MEM_ATT_DB);

    get_adc_values();

    for (uint32_t idx = 0; idx < TRNG_RAM_SIZE_USED; idx++)
    {
        buffer[idx] ^= buffer[(TRNG_RAM_SIZE_USED - 1) - idx] ^ adc_samples[buffer[idx] % ADC_ENTRIES];
    }

    status = trng_acquire(trng_bits, buffer, TRNG_NUM_BLOCKS);

    ASSERT_ERROR(status == IID_SUCCESS);

    ke_free(buffer);
#else
    trng_acquire(trng_bits);
#endif

#if (USE_CHACHA20_RAND)
    csprng_seed(trng_bits);
#else
    uint32_t seed;
    memcpy(&seed, trng_bits, sizeof(seed));
    co_random_init(seed);
#endif
}

#endif  //USE_TRNG
