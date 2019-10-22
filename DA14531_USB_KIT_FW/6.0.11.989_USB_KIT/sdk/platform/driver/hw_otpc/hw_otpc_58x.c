/**
 ****************************************************************************************
 *
 * @file hw_otpc_58x.c
 *
 * @brief DA14585/DA14586 OTP Controller driver source file.
 *
 * Copyright (c) 2016-2018 Dialog Semiconductor. All rights reserved.
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



#include <stdint.h>
#include "hw_otpc.h"


#define OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_CADX_Pos (0UL)
#define OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_CADX_Msk (0xffUL)
#define OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_PW_Pos (8UL)
#define OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_PW_Msk (0xff00UL)
#define OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_1US_Pos (16UL)
#define OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_1US_Msk (0x3f0000UL)
#define OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_500NS_Pos (22UL)
#define OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_500NS_Msk (0x7c00000UL)
#define OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_200NS_Pos (27UL)
#define OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_200NS_Msk (0x78000000UL)
#define OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_25NS_Pos (31UL)
#define OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_25NS_Msk (0x80000000UL)


/*
 * Local variables
 */

/*
 * 2MHz: 1 cycle = 500ns ==>
 *    25ns : 500ns, 200ns : 500ns, 500ns : 1us,   1us : 1.5us,   5us : 5us, 2us : 2.5us,   blanc : 500ns
 *
 * 4MHz: 1 cycle = 250ns ==>
 *    25ns : 250ns, 200ns : 250ns, 500ns : 750ns, 1us : 1.25ns,  5us : 5us, 2us : 2.25ns,  blanc : 250ns
 *
 * 8MHz: 1 cycle = 125ns ==>
 *    25ns : 125ns, 200ns : 250ns, 500ns : 625ns, 1us : 1.125ns, 5us : 5us, 2us : 2.125ns, blanc : 125ns
 *
 * 16MHz: 1 cycle = 62.5ns ==>
 *    25ns : 62ns, 200ns : 250ns,  500ns : 562ns, 1us : 1.062ns, 5us : 5us, 2us : 2.062ns, blanc : 125ns
 *
 */
const uint32_t tim1[4] = {
        /* 2MHz */
        (  0U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_25NS_Pos) |
        (  0U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_200NS_Pos) |
        (  1U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_500NS_Pos) |
        (  2U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_1US_Pos) |
        (  9U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_PW_Pos) |
        (  4U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_CADX_Pos),
        /* 4MHz */
        (  0U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_25NS_Pos) |
        (  0U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_200NS_Pos) |
        (  2U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_500NS_Pos) |
        (  4U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_1US_Pos) |
        ( 19U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_PW_Pos) |
        (  8U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_CADX_Pos),
        /* 8MHz */
        (  0U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_25NS_Pos) |
        (  1U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_200NS_Pos) |
        (  4U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_500NS_Pos) |
        (  8U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_1US_Pos) |
        ( 39U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_PW_Pos) |
        ( 16U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_CADX_Pos),
        /* 16MHz */
        (  0U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_25NS_Pos) |
        (  3U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_200NS_Pos) |
        (  8U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_500NS_Pos) |
        ( 16U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_1US_Pos) |
        ( 79U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_PW_Pos) |
        ( 32U << OTPC_OTPC_TIM1_REG_OTPC_TIM1_CC_T_CADX_Pos),
};

static const uint8_t tim2[4] =
{
    0, /* 2Mhz */
    0, /* 4Mhz */
    0, /* 8Mhz */
    1, /* 16Mhz */
};


/*
 * Forward declarations
 */



/*
 * Inline helpers
 */

/**
 * Wait for programming to finish.
 */
__STATIC_INLINE void wait_for_prog_done(void)
{
        while (!GetBits32(OTPC_STAT_REG, OTPC_STAT_PRDY))
                ;
}

/**
 * Wait for AREAD or APROG to finish.
 */
__STATIC_INLINE void wait_for_auto_done(void)
{
        while (!GetBits32(OTPC_STAT_REG, OTPC_STAT_ARDY))
                ;
}

/**
 * Check for correctable or uncorrectable programming error.
 */
__STATIC_INLINE bool have_prog_error(void)
{
        return GetWord32(OTPC_STAT_REG) & (OTPC_STAT_PERR_UNC | OTPC_STAT_PERR_COR );
}

/*
 * Assertion macros
 */


/*
 * Make sure that the OTP clock is enabled
 */
#define ASSERT_WARNING_OTP_CLK_ENABLED \
        ASSERT_WARNING(GetWord16(CLK_AMBA_REG) & OTP_ENABLE)

/*
 * Make sure that the OTPC is in given state
 */
#define ASSERT_WARNING_OTPC_MODE(s) \
        ASSERT_WARNING(GetBits16(OTPC_MODE_REG, OTPC_MODE_MODE) == s)

/*
 * Make sure that the cell address is valid
 */
#define ASSERT_CELL_OFFSET_VALID(off) \
        ASSERT_WARNING(off < 0x2000)

/*
 * Make sure val is non-zero and less than maximum
 */
#define ASSERT_WARNING_NONZERO_RANGE(val, maximum) \
        do { \
                ASSERT_WARNING(val); \
                ASSERT_WARNING((val) < (maximum)); \
        } while (0)


/*
 * Function definitions
 */

void hw_otpc_disable(void)
{
        /*
         * Enable OTPC clock
         */
        hw_otpc_init();

        /*
         * set OTPC to stand-by mode
         */
        SetBits(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_STBY);

        /*
         * Disable OTPC clock
         */
        hw_otpc_close();
}


void hw_otpc_set_speed(HW_OTPC_SYS_CLK_FREQ clk_speed)
{
        ASSERT_WARNING_OTP_CLK_ENABLED;

        /*
         * Set access speed
         */
        ASSERT_WARNING(clk_speed <= HW_OTPC_SYS_CLK_FREQ_16);

        SetWord32(OTPC_TIM1_REG, tim1[clk_speed]);
        SetBits32(OTPC_TIM2_REG, OTPC_TIM2_CC_T_BCHK, tim2[clk_speed]);
}


void hw_otpc_power_save(uint32_t inactivity_period)
{
        /* Only go to power save for an inactivity_period < 1024 */
        ASSERT_WARNING(inactivity_period < 1024);

        SetBits32(OTPC_TIM2_REG, OTPC_TIM2_CC_STBY_THR, inactivity_period);
}


uint32_t hw_otpc_num_of_rr(void)
{
        unsigned int i;

        volatile uint32_t *ptr = (volatile uint32_t *)MEMORY_OTP_BASE;

        ASSERT_WARNING_OTP_CLK_ENABLED;

        // STBY mode
        SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_STBY);

        // The access will be performed in the spare rows
        SetBits32(OTPC_MODE_REG, OTPC_MODE_USE_SP_ROWS, 1);

        // MREAD mode
        SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_MREAD);

        // Read Records
        i = 0;
        while ((i < MAX_RR_AVAIL) && (ptr[0x9e - 4 * i] & 0x1)) {
                i++;
        }

        // STBY mode
        SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_STBY);

        // Select the normal memory array
        SetBits32(OTPC_MODE_REG, OTPC_MODE_USE_SP_ROWS, 0);

        return i;
}

static bool manual_prog_verify(uint32_t cell_offset, uint32_t pword_l, uint32_t pword_h)
{
        uint32_t val_l, val_h, old_mode = GetWord32(OTPC_MODE_REG);
        uint32_t *addr = hw_otpc_cell_to_mem(cell_offset);

        /* we have to go through standby mode first */
        SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_STBY);
        __DMB();
        SetBits32(OTPC_MODE_REG, OTPC_MODE_ERR_RESP_DIS, 1);
        __DMB();
        SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_MREAD);

        if (GetWord32(OTPC_STAT_REG) & OTPC_STAT_RERROR) {
                SetBits32(OTPC_STAT_REG, OTPC_STAT_RERROR, 1); //This bit need to be cleared manually (can only happen if a previous read has not checked/cleared it)
        }
        /*
         * Read cell in manual mode, as 2 32-bit values (little-endian)
         */
        val_l = addr[0];
        val_h = addr[1];
        SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_STBY);
        __DMB();
        SetWord32(OTPC_MODE_REG, old_mode);
        if (GetWord32(OTPC_STAT_REG) & OTPC_STAT_RERROR) {
                SetBits32(OTPC_STAT_REG, OTPC_STAT_RERROR, 1); //This bit need to be cleared manually
                return false;
        }
        return (pword_h == val_h) && (pword_l == val_l);
}

/*
 * Set to 1 to manually verify the programmed cell value, if the auto-verification fails.
 */
#define MANUAL_PROG_VERIFICATION        1

bool hw_otpc_manual_word_prog(uint32_t cell_offset, uint32_t pword_l, uint32_t pword_h, bool use_rr)
{
        int i;
        bool ret = true;

        ASSERT_WARNING_OTP_CLK_ENABLED;

        ASSERT_WARNING_OTPC_MODE(HW_OTPC_MODE_STBY);

        ASSERT_CELL_OFFSET_VALID(cell_offset);

        /*
         * Program the data regs
         */
        SetWord32(OTPC_PWORDL_REG, pword_l);
        SetWord32(OTPC_PWORDH_REG, pword_h);

        /*
         * Start programming
         */
        SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_MPROG);
        SetWord32(OTPC_PCTRL_REG, OTPC_PCTRL_PSTART | cell_offset);
        wait_for_prog_done();

        /*
         * Check and retry up to 10 times
         */
        i = 0;
        while (have_prog_error()) {
                if (i == 10) {
                        break;
                }
                i = i + 1;

                SetWord32(OTPC_PCTRL_REG, OTPC_PCTRL_PSTART
                                            | OTPC_PCTRL_PRETRY
                                            | cell_offset);

                wait_for_prog_done();
        }

        if (i == 10) {
                ret = false;
                if (MANUAL_PROG_VERIFICATION)
                        ret = manual_prog_verify(cell_offset, pword_l, pword_h);

                if (use_rr && !ret) {
                        do {
                                // Reset state
                                SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_STBY);

                                // Abort if the writing was done in the spare area
                                if (GetWord32(OTPC_MODE_REG) & OTPC_MODE_USE_SP_ROWS) {
                                        break;
                                }

                                // Write the repair record to the spare area
                                if (!hw_otpc_write_rr(cell_offset, pword_l, pword_h)) {
                                        break;
                                }

                                SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_STBY);
                                // force reloading
                                SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_MREAD);

                                ret = true;
                                break;
                        } while (0);
                }
        }

        SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_STBY);

        return ret;
}

bool hw_otpc_manual_prog(const uint32_t *p_data, uint32_t cell_offset, HW_OTPC_WORD cell_word,
                uint32_t num_of_words, bool use_rr)
{
        uint32_t w, c, ncells, off, val;
        bool ret = true;
        const uint32_t* addr;

        ASSERT_WARNING_OTP_CLK_ENABLED;

        ASSERT_WARNING_OTPC_MODE(HW_OTPC_MODE_STBY);

        ASSERT_CELL_OFFSET_VALID(cell_offset);

        if (num_of_words == 0)
                return true;  // early exit

        /* index in p_data[] */
        w = 0;
        off = cell_offset;
        if (cell_word == HW_OTPC_WORD_HIGH) {
                /*
                 * read low 32-bit word so that we program the same value
                 */
                addr = hw_otpc_cell_to_mem(cell_offset);

                hw_otpc_manual_read_on(use_rr);
                /* little-endian */
                val = addr[0];
                hw_otpc_manual_read_off();

                if (!hw_otpc_manual_word_prog(off++, val, p_data[w++], use_rr))
                        return false;

                ncells = (num_of_words - 1) >> 1;
        } else {
                ncells = num_of_words >> 1;
        }

        for (c = 0; c < ncells; c++, off++, w += 2) {
                if (!hw_otpc_manual_word_prog(off, p_data[w], p_data[w + 1], use_rr))
                        return false;
        }

        if (w < num_of_words) {
                /*
                 * read high 32-bit word so that we program the same value
                 */
                addr = hw_otpc_cell_to_mem(off);

                hw_otpc_manual_read_on(use_rr);
                /* little-endian */
                val = addr[1];
                hw_otpc_manual_read_off();

                if (!hw_otpc_manual_word_prog(off, p_data[w++], val, use_rr))
                        return false;
        }

        ASSERT_WARNING(w == num_of_words);

        return ret;
}

bool hw_otpc_write_rr(uint32_t cell_addr, uint32_t pword_l, uint32_t pword_h)
{
        uint32_t repair_cnt;
        bool ret = false;

        ASSERT_WARNING_OTP_CLK_ENABLED;

        ASSERT_WARNING_OTPC_MODE(HW_OTPC_MODE_STBY);

        // Get the number of used Repair Records
        repair_cnt = hw_otpc_num_of_rr();

        // Abort if all repair records are being used
        if (repair_cnt < MAX_RR_AVAIL) {
                do {
                        // The access will be performed in the spare rows
                        SetBits32(OTPC_MODE_REG, OTPC_MODE_USE_SP_ROWS, 1);

                        // Write the data to the spare area
                        if (!hw_otpc_manual_word_prog(0x4F - repair_cnt - 1,
                                                pword_l, pword_h, false)) {
                                break;
                        }

                        // Write the header of the repair record to the spare area
                        if (!hw_otpc_manual_word_prog(0x4F - repair_cnt,
                                                1 | (cell_addr << 1), 0x00000000, false)) {
                                break;
                        }

                        ret = true;
                } while (0);

                // Return to the normal memory array
                SetBits32(OTPC_MODE_REG, OTPC_MODE_USE_SP_ROWS, 0);

                if (ret) {
                        /*
                         * Request the reloading of the repair records at the next
                         * enabling of the OTP cell
                         */
                        SetBits32(OTPC_MODE_REG, OTPC_MODE_RLD_RR_REQ, 1);
                }
        }

        return ret;
}


void hw_otpc_manual_read_on(bool spare_rows)
{
        ASSERT_WARNING_OTP_CLK_ENABLED;

        ASSERT_WARNING_OTPC_MODE(HW_OTPC_MODE_STBY);

        /*
         * Place the OTPC in manual read mode
         */
        if (spare_rows) {
                SetWord32(OTPC_MODE_REG, OTPC_MODE_USE_SP_ROWS | HW_OTPC_MODE_MREAD);
        }
        else {
                SetWord32(OTPC_MODE_REG, HW_OTPC_MODE_MREAD);
        }
}


void hw_otpc_manual_read_off(void)
{
        ASSERT_WARNING_OTP_CLK_ENABLED;

        ASSERT_WARNING_OTPC_MODE(HW_OTPC_MODE_MREAD);

        /*
         * Place the OTPC in STBY mode
         */
        SetWord32(OTPC_MODE_REG, HW_OTPC_MODE_STBY);
}


bool hw_otpc_dma_prog(const uint32_t *p_data, uint32_t cell_offset, HW_OTPC_WORD cell_word,
                uint32_t num_of_words, bool spare_rows)
{
        ASSERT_WARNING_OTP_CLK_ENABLED;

        ASSERT_WARNING_OTPC_MODE(HW_OTPC_MODE_STBY);

        ASSERT_CELL_OFFSET_VALID(cell_offset);

        /*
         * Make sure that the size is valid
         */
        ASSERT_WARNING(num_of_words);
        ASSERT_WARNING(num_of_words < 16384);

        /*
         * Set up DMA
         */
        SetWord32(OTPC_AHBADR_REG, (uint32_t)p_data);

        SetWord32(OTPC_CELADR_REG, (cell_offset << 1) | (cell_word == HW_OTPC_WORD_HIGH));
        SetWord32(OTPC_NWORDS_REG, num_of_words-1);

        /*
         * Start DMA programming
         */
        if (spare_rows) {
                SetWord32(OTPC_MODE_REG, OTPC_MODE_USE_DMA
                                         | OTPC_MODE_USE_SP_ROWS
                                         | HW_OTPC_MODE_APROG);
        }
        else {
                SetWord32(OTPC_MODE_REG, OTPC_MODE_USE_DMA | HW_OTPC_MODE_APROG);
        }

        wait_for_auto_done();

        /*
         * Check result
         */
        if (GetWord32(OTPC_STAT_REG) & OTPC_STAT_PERR_UNC) {
                return false;
        }

        return true;
}


void hw_otpc_dma_read(uint32_t *p_data, uint32_t cell_offset, HW_OTPC_WORD cell_word,
                uint32_t num_of_words, bool spare_rows)
{
        ASSERT_WARNING_OTP_CLK_ENABLED;

        ASSERT_WARNING_OTPC_MODE(HW_OTPC_MODE_STBY);

        ASSERT_CELL_OFFSET_VALID(cell_offset);

        /*
         * Make sure that the size is valid
         */
        ASSERT_WARNING(num_of_words);
        ASSERT_WARNING(num_of_words < 16384);

        /*
         * Set up DMA
         */
        SetWord32(OTPC_AHBADR_REG, (uint32_t)p_data);

        SetWord32(OTPC_CELADR_REG, (cell_offset << 1) | (cell_word == HW_OTPC_WORD_HIGH));
        SetWord32(OTPC_NWORDS_REG, num_of_words-1);

        /*
         * Start DMA programming
         */
        if (spare_rows) {
                SetWord32(OTPC_MODE_REG, OTPC_MODE_USE_DMA
                                         | OTPC_MODE_USE_SP_ROWS
                                         | HW_OTPC_MODE_AREAD);
        }
        else {
                SetWord32(OTPC_MODE_REG, OTPC_MODE_USE_DMA
                                         | HW_OTPC_MODE_AREAD);
        }

        wait_for_auto_done();
}


bool hw_otpc_fifo_prog(const uint32_t *p_data, uint32_t cell_offset, HW_OTPC_WORD cell_word,
                uint32_t num_of_words, bool spare_rows)
{
        unsigned int i;

        ASSERT_WARNING_OTP_CLK_ENABLED;

        ASSERT_WARNING_OTPC_MODE(HW_OTPC_MODE_STBY);

        ASSERT_CELL_OFFSET_VALID(cell_offset);

        ASSERT_WARNING_NONZERO_RANGE(num_of_words, 16384);

        /*
         * Set up FIFO
         */
        SetWord32(OTPC_CELADR_REG, (cell_offset << 1) | (cell_word == HW_OTPC_WORD_HIGH));
        SetWord32(OTPC_NWORDS_REG, num_of_words-1);

        /*
         * Perform programming via FIFO
         */
        if (spare_rows) {
                SetWord32(OTPC_MODE_REG, OTPC_MODE_USE_SP_ROWS | HW_OTPC_MODE_APROG);
        }
        else {
                SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, HW_OTPC_MODE_APROG);
        }

        for (i = 0; i < num_of_words; i++) {
                while (GetBits32(OTPC_STAT_REG, OTPC_STAT_FWORDS) == 8)
                        ;
                SetWord32(OTPC_FFPRT_REG, p_data[i]); // Write FIFO data
        }

        /*
         * Wait for completion
         */
        wait_for_auto_done();

        /*
         * Check result
         */
        if (GetWord32(OTPC_STAT_REG) & OTPC_STAT_PERR_UNC) {
                return false;
        }

        return true;
}


bool hw_otpc_fifo_read(uint32_t *p_data, uint32_t cell_offset, HW_OTPC_WORD cell_word,
                uint32_t num_of_words, bool spare_rows)
{
        unsigned int i;

        ASSERT_WARNING_OTP_CLK_ENABLED;

        ASSERT_WARNING_OTPC_MODE(HW_OTPC_MODE_STBY);

        ASSERT_CELL_OFFSET_VALID(cell_offset);

        ASSERT_WARNING_NONZERO_RANGE(num_of_words, 16384);

        if (GetWord32(OTPC_STAT_REG) & OTPC_STAT_RERROR) {
                SetBits32(OTPC_STAT_REG, OTPC_STAT_RERROR, 1); //This bit need to be cleared manually (can only happen if a previous read has not checked/cleared it)
        }

        /*
         * Set up FIFO
         */
        SetWord32(OTPC_CELADR_REG, (cell_offset << 1) | (cell_word == HW_OTPC_WORD_HIGH));
        SetWord32(OTPC_NWORDS_REG, num_of_words-1);

        /*
         * Perform reading via FIFO
         */
        if (spare_rows) {
                SetWord32(OTPC_MODE_REG, OTPC_MODE_USE_SP_ROWS | HW_OTPC_MODE_AREAD);
        }
        else {
                SetWord32(OTPC_MODE_REG, HW_OTPC_MODE_AREAD);
        }

        for (i = 0; i < num_of_words; i++) {
                while (GetBits32(OTPC_STAT_REG, OTPC_STAT_FWORDS) == 0)
                        ;
                p_data[i] = GetWord32(OTPC_FFPRT_REG);
        }

        /*
         * Wait for completion
         */
        wait_for_auto_done();

        /*
         * Check result
         */
        if (GetWord32(OTPC_STAT_REG) & OTPC_STAT_RERROR) {
                SetBits32(OTPC_STAT_REG, OTPC_STAT_RERROR, 1); //This bit need to be cleared manually
                return false;
        }

        return true;
}


void hw_otpc_prepare(uint32_t num_of_bytes)
{
        ASSERT_WARNING_OTP_CLK_ENABLED;

        ASSERT_WARNING_OTPC_MODE(HW_OTPC_MODE_STBY);

        ASSERT_WARNING_NONZERO_RANGE(num_of_bytes, 65536);

        /*
         * Set up image size
         */
        SetWord32(OTPC_NWORDS_REG, ((num_of_bytes + 3) >> 2) - 1);

        /*
         * Enable OTP_COPY
         */
        SetBits32(SYS_CTRL_REG, OTP_COPY, 1);
}


void hw_otpc_cancel_prepare(void)
{
        ASSERT_WARNING_OTP_CLK_ENABLED;

        ASSERT_WARNING_OTPC_MODE(HW_OTPC_MODE_STBY);

        /*
         * Disable OTP_COPY
         */
        SetBits32(SYS_CTRL_REG, OTP_COPY, 0);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// TEST FUNCTIONALITY
////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * @brief Execution of the test.
 *
 * @return The test result.
 *         <ul>
 *             <li> 0, if the test succeeded
 *             <li> 1, if the test 1 failed
 *         </ul>
 *
 */
static int hw_otpc_core_test(int mode)
{
        ASSERT_WARNING_OTPC_MODE(HW_OTPC_MODE_STBY);

        /*
         * Put OTPC in the proper test mode
         */
        SetBits32(OTPC_MODE_REG, OTPC_MODE_MODE, mode);

        /*
         * Wait end of test
         */
        while (!GetBits32(OTPC_STAT_REG, OTPC_STAT_TRDY))
                ;


        /*
         * Check result
         */
        if (GetWord32(OTPC_STAT_REG) & OTPC_STAT_TERROR) {
                return 1;
        }

        return 0;
}


int hw_otpc_blank(void)
{
        return hw_otpc_core_test(HW_OTPC_MODE_TBLANK);
}


int hw_otpc_tdec(void)
{
        return hw_otpc_core_test(HW_OTPC_MODE_TDEC);
}


int hw_otpc_twr(void)
{
        return hw_otpc_core_test(HW_OTPC_MODE_TWR);
}

