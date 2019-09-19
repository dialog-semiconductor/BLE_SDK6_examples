/**
 ****************************************************************************************
 *
 * @file hw_otpc_531.c
 *
 * @brief DA14531 OTP Controller driver source file.
 *
 * Copyright (c) 2018 Dialog Semiconductor. All rights reserved.
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

#include "hw_otpc.h"
#include "syscntl.h"
#include "arch.h"

/*
 * Local variables
 */

/// DCDC converter reservation status
static bool dcdc_reserved;

/* Add specific TIM1 settings
 *  TIM1_CC_T_1US value  =  (1000ns * N Mhz / 1000) - 1
 *  TIM1_CC_T_20NS value =  (20ns   * N Mhz / 1000) - 1
 *  TIM1_CC_T_RD value   =  (60ns   * N Mhz / 1000) - 1
 *
 */
static const uint32_t tim1[] = {
     /*  2MHz*/
    ( 0x01U << 0 ) |    // write @ 2MHz is not supported
    ( 0x00U << 8 ) |
    ( 0x00U << 12 ) |
    ( 0x09U << 16 ) |
    ( 0x09U << 20 ) |
    ( 0x09U << 24 ),
    /*  4MHz*/
    ( 0x03U << 0 ) |
    ( 0x00U << 8 ) |
    ( 0x00U << 12 ) |
    ( 0x09U << 16 ) |
    ( 0x09U << 20 ) |
    ( 0x09U << 24 ),
    /*  8MHz*/
    ( 0x07U << 0 ) |
    ( 0x00U << 8 ) |
    ( 0x00U << 12 ) |
    ( 0x09U << 16 ) |
    ( 0x09U << 20 ) |
    ( 0x09U << 24 ),
    /*  16MHz*/
    ( 0x0FU << 0 ) |
    ( 0x00U << 8 ) |
    ( 0x00U << 12 ) |
    ( 0x09U << 16 ) |
    ( 0x09U << 20 ) |
    ( 0x09U << 24 ),
/* Over 16MHz not applicable in Barium */
    /*  32MHz*/
    ( 0x1FU << 0 ) |
    ( 0x00U << 8 ) |
    ( 0x01U << 12 ) |
    ( 0x09U << 16 ) |
    ( 0x09U << 20 ) |
    ( 0x09U << 24 ),
    /*  48MHz*/
    ( 0x2FU << 0 ) |
    ( 0x00U << 8 ) |
    ( 0x02U << 12 ) |
    ( 0x09U << 16 ) |
    ( 0x09U << 20 ) |
    ( 0x09U << 24 ),
    /*  96MHz*/
    ( 0x5FU << 0 ) |
    ( 0x01U << 8 ) |
    ( 0x05U << 12 ) |
    ( 0x09U << 16 ) |
    ( 0x09U << 20 ) |
    ( 0x09U << 24 ),
};

/* TIM2 settings */
static const uint32_t tim2 = {
    /* default*/
    ( 0x09U << 0 ) |
    ( 0x00U << 5 ) |
    ( 0x04U << 8 ) |
    ( 0x04U << 16 ) |
    ( 0x00U << 21 ) |
    ( 0x04U << 24 ) |
    ( 0x01U << 29 ) |
    ( 0x01U << 31 )
};

/*
 * Forward declarations
 */

/*
 * Inline helpers
 */

/*
 * Assertion macros
 */

/*
 * Make sure that the OTP clock is enabled
 */
#define ASSERT_WARNING_OTP_CLK_ENABLED ASSERT_WARNING(GetBits16(CLK_AMBA_REG, OTP_ENABLE))

/*
 * Make sure that the cell address is valid
 */
#define ASSERT_CELL_OFFSET_VALID(off) ASSERT_WARNING(off < HW_OTP_CELL_NUM)

/*
 * Function definitions
 */

bool hw_otpc_is_dcdc_reserved(void)
{
    return dcdc_reserved;
}

/**
 ****************************************************************************************
 * @brief Configures the DCDC converter for OTP actions (read/write). Called only by 
 * @p hw_otpc_enter_mode().
 * @param[in] level     The voltage level to be set.
 * @return void
 ****************************************************************************************
 */
#if (!USE_POWER_MODE_BYPASS)
static void dcdc_cfg(syscntl_dcdc_level_t level)
{
    if (level > syscntl_dcdc_get_level())
    {
        syscntl_dcdc_set_level(level);
    }
    // Check DCDC converter status
    if (!GetBits16(DCDC_CTRL_REG, DCDC_ENABLE))
    {
        // Enable the DCDC converter
        SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 1);

        // Wait for the indication that VBAT_HIGH (boost mode) is OK
        while (!GetBits(ANA_STATUS_REG, DCDC_OK));
    }
}
#endif

void hw_otpc_init(void)
{
    GLOBAL_INT_DISABLE();

#if (!USE_POWER_MODE_BYPASS)
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
        extern syscntl_dcdc_state_t syscntl_dcdc_state;

        // OTPC has higher priority than GPIOs when it requests the DCDC converter
        dcdc_reserved = true;

        syscntl_dcdc_state.level = syscntl_dcdc_get_level();
        syscntl_dcdc_state.status = GetBits16(DCDC_CTRL_REG, DCDC_ENABLE);
    }
#endif

    /*
     * Enable OTPC clock
     */
    SetBits16(CLK_AMBA_REG, OTP_ENABLE, 0x01);

    /*
     * Set OTPC to deep stand-by mode
     */
    SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_DSTBY);
    hw_otpc_wait_mode_change();

    /*
     * Configure OTPC timings (by defualt 16MHz clock)
     */
    hw_otpc_set_speed(HW_OTPC_CLK_FREQ_16MHz);

    GLOBAL_INT_RESTORE();
}

void hw_otpc_enter_mode(hw_otpc_mode_t mode)
{
    volatile hw_otpc_mode_t current_mode;

    /*change mode only if new mode is different than the old one*/
    current_mode = (hw_otpc_mode_t) GetBits32(OTPC_MODE_REG, OTPC_MODE_MODE);
    if (mode != current_mode)
    {
#if (!USE_POWER_MODE_BYPASS)
        if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
        {
            switch(mode)
            {
                case HW_OTPC_MODE_READ:
                case HW_OTPC_MODE_AREAD:
                case HW_OTPC_MODE_PVFY:
                case HW_OTPC_MODE_RINI:
                {
                    // Set DCDC converter voltage for read (must be > 1.62V)
                    dcdc_cfg(SYSCNTL_DCDC_LEVEL_1V8);
                    break;
                }
                case HW_OTPC_MODE_PROG:
                {
                    // Set DCDC converter voltage for read (must be > 2.25V)
                    dcdc_cfg(SYSCNTL_DCDC_LEVEL_2V5);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
#endif
        // Change the operation mode of the OTP controller
        SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, mode);
        hw_otpc_wait_mode_change();
    }
}

void hw_otpc_set_speed(hw_otpc_clk_freq_t clk_speed)
{
    ASSERT_WARNING_OTP_CLK_ENABLED;

    /*
     * Set access speed
     */
    SetWord32(OTPC_TIM1_REG, tim1[(uint8_t) clk_speed]);
    SetWord32(OTPC_TIM2_REG, tim2);
}

bool hw_otpc_word_prog_and_verify(uint32_t wdata, uint32_t cell_offset)
{

    ASSERT_CELL_OFFSET_VALID(cell_offset);

    ASSERT_WARNING_OTP_CLK_ENABLED;

    hw_otpc_word_prog(wdata, cell_offset);

    hw_otpc_enter_mode(HW_OTPC_MODE_PVFY);
    if (wdata != *(uint32_t *)(MEMORY_OTP_BASE + HW_OTP_CELL_SIZE * cell_offset))
    {
        return false;
    }

    hw_otpc_enter_mode(HW_OTPC_MODE_RINI);
    if (wdata != *(uint32_t *)(MEMORY_OTP_BASE + HW_OTP_CELL_SIZE * cell_offset))
    {
        return false;
    }

    return true;
}

uint32_t hw_otpc_word_read(uint32_t cell_offset)
{
    ASSERT_CELL_OFFSET_VALID(cell_offset);

    ASSERT_WARNING_OTP_CLK_ENABLED;

    hw_otpc_enter_mode(HW_OTPC_MODE_READ);
    return *(uint32_t *)(MEMORY_OTP_BASE + HW_OTP_CELL_SIZE * cell_offset);
}

void hw_otpc_prog(uint32_t *p_data, uint32_t cell_offset, uint32_t num_of_words)
{
    uint32_t i;

    ASSERT_WARNING_OTP_CLK_ENABLED;
    ASSERT_CELL_OFFSET_VALID(cell_offset + num_of_words - 1);

    hw_otpc_enter_mode(HW_OTPC_MODE_PROG);

    for (i = 0; i < num_of_words; i++)
    {
        SetWord32(OTPC_PWORD_REG, *p_data++);
        SetWord32(OTPC_PADDR_REG, cell_offset++);
        hw_otpc_wait_while_programming_buffer_is_full();
    }
    hw_otpc_wait_while_busy_programming();
}

static bool hw_otpc_read_verif(uint32_t *w_data, uint32_t cell_offset, uint32_t num_of_words, hw_otpc_mode_t mode)
{
    uint32_t i;

    ASSERT_WARNING_OTP_CLK_ENABLED;

    hw_otpc_enter_mode(mode);

    for (i = 0; i < num_of_words; i++)
    {
        if (*w_data != *(uint32_t *)(MEMORY_OTP_BASE + HW_OTP_CELL_SIZE * cell_offset))
        {
            return false;
        }
        cell_offset++;
        w_data++;
    }
    return true;
}

bool hw_otpc_prog_and_verify(uint32_t *p_data, uint32_t cell_offset, uint32_t num_of_words)
{
    ASSERT_WARNING_OTP_CLK_ENABLED;

    hw_otpc_prog(p_data, cell_offset, num_of_words);

    if (false == hw_otpc_read_verif(p_data, cell_offset, num_of_words, HW_OTPC_MODE_PVFY))
    {
        return false;
    }
    if (false == hw_otpc_read_verif(p_data, cell_offset, num_of_words, HW_OTPC_MODE_RINI))
    {
        return false;
    }
    hw_otpc_enter_mode(HW_OTPC_MODE_PROG);
    return true;
}

void hw_otpc_read(uint32_t *p_data, uint32_t cell_offset, uint32_t num_of_words)
{
    uint32_t i;

    ASSERT_WARNING_OTP_CLK_ENABLED;

    ASSERT_CELL_OFFSET_VALID(cell_offset + num_of_words - 1);

    hw_otpc_enter_mode(HW_OTPC_MODE_READ);

    for (i = 0; i < num_of_words; i++)
    {
        *p_data = *(uint32_t *)(MEMORY_OTP_BASE + HW_OTP_CELL_SIZE * cell_offset);
        p_data++;
        cell_offset++;
    }
}

void hw_otpc_disable(void)
{
    GLOBAL_INT_DISABLE();

    /*
     * Enable OTPC clock
     */
    SetBits16(CLK_AMBA_REG, OTP_ENABLE, 0x01);

     /*
     * set OTPC to stand-by mode
     */
    SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_DSTBY);
    hw_otpc_wait_mode_change();

    SetWord32(OTPC_TIM1_REG, OTPC_TIM1_REG_RESET);
    SetWord32(OTPC_TIM2_REG, OTPC_TIM2_REG_RESET);

    /*
     * Disable OTPC clock
     */
    SetBits16(CLK_AMBA_REG, OTP_ENABLE, 0x00);

#if (!USE_POWER_MODE_BYPASS)
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
    {
        extern syscntl_dcdc_state_t syscntl_dcdc_state;

        dcdc_reserved = false;

        // Reload DCDC converter voltage level value
        syscntl_dcdc_set_level(syscntl_dcdc_state.level);

        if (!syscntl_dcdc_state.status)
        {
            // Disable the DCDC
            SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 0);
        }
    }
#endif

    GLOBAL_INT_RESTORE();
}
