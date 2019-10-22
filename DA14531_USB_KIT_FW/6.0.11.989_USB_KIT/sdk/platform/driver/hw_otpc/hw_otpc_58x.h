/**
 ****************************************************************************************
 *
 * @file hw_otpc_58x.h
 *
 * @brief DA14585/DA14586 OTP Controller driver header file.
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

#ifndef _HW_OTPC_58X_H_
#define _HW_OTPC_58X_H_


#include <stdbool.h>
#include <stdint.h>
#include "datasheet.h"
#include "arch.h"


/**
 * @brief OTP Controller mode
 */
typedef enum {
        HW_OTPC_MODE_STBY       = 0,
        HW_OTPC_MODE_MREAD      = 1,
        HW_OTPC_MODE_MPROG      = 2,
        HW_OTPC_MODE_AREAD      = 3,
        HW_OTPC_MODE_APROG      = 4,
        HW_OTPC_MODE_TBLANK     = 5,
        HW_OTPC_MODE_TDEC       = 6,
        HW_OTPC_MODE_TWR        = 7
} HW_OTPC_MODE;

#define HW_OTP_CELL_SIZE        (0x08)                          /*each cell is 8 bytes*/

#define MAX_RR_AVAIL            (8)


/**
 * @brief Word inside cell to program/read
 *
 * Cell contents in memory starts with low word (i.e. to program/read both words in cell at once,
 * HW_OTPC_WORD_LOW should be used for addressing).
 *
 */
typedef enum {
        HW_OTPC_WORD_LOW = 0,
        HW_OTPC_WORD_HIGH = 1
} HW_OTPC_WORD;


/**
 * @brief System clock frequency in MHz
 *
 */
typedef enum {
        HW_OTPC_SYS_CLK_FREQ_2 = 0,
        HW_OTPC_SYS_CLK_FREQ_4 = 1,
        HW_OTPC_SYS_CLK_FREQ_8 = 2,
        HW_OTPC_SYS_CLK_FREQ_16 = 3,
} HW_OTPC_SYS_CLK_FREQ;


/**
 * @brief Initialize the OTP Controller.
 *
 * @warning The AHB clock must be up to 16MHz! It is a responsibility of the caller to check this!
 *
 */
static inline void hw_otpc_init(void)
{
        /*
         * Reset OTP controller
         */
        SetWord32(OTPC_MODE_REG, HW_OTPC_MODE_STBY);    // Set OTPC to standby
        SetBits16(SYS_CTRL_REG, OTPC_RESET_REQ, 1);     // Reset the OTPC
        SetBits16(SYS_CTRL_REG, OTPC_RESET_REQ, 0);     // Get the OTPC out of Reset

        /*
         * Initialize the POR registers
         */
        SetWord32(OTPC_NWORDS_REG, OTPC_NWORDS_REG_RESET);
        SetWord32(OTPC_TIM1_REG, OTPC_TIM1_REG_RESET);
        SetWord32(OTPC_TIM2_REG, OTPC_TIM2_REG_RESET);

        /*
         * Enable OTPC clock
         */
        SetBits16(CLK_AMBA_REG, OTP_ENABLE, 1);

        /*
         * Wait for a while for the OTP settling time.
         */
        for(volatile int i = 0; i < 100; i++);
}

/**
 * @brief Close the OTP Controller.
 *
 */
static inline void hw_otpc_close(void)
{
        /*
         * Disable OTPC clock
         */
        SetBits16(CLK_AMBA_REG, OTP_ENABLE, 0);
}

/**
 * @brief Check if the OTP Cotnroller is active.
 *
 * @return 1 if it is active, else 0.
 *
 */
static inline uint32_t hw_otpc_is_active(void) __attribute__((always_inline));

static inline uint32_t hw_otpc_is_active(void)
{
        /*
         * Check if the OTPC clock is enabled
         */
        return !!(GetBits16(CLK_AMBA_REG,OTP_ENABLE));
}

/**
 * @brief Put the OTP Controller in STBY mode and turn-off clock.
 *
 */
void hw_otpc_disable(void);

/**
 * @brief Set the access speed of the OTP Controller based on the system clock.
 *
 * @details Switch from PLL to XTAL : call function after clock switch with high_clk_speed == false
 *          Switch from XTAL to PLL : call function before clock switch with high_clk_speed == true
 *
 * @param[in] clk_speed The frequency of the system clock: 2, 4, 8, 16.
 *
 * @warning The OTP clock must have been enabled (OTP_ENABLE == 1).
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 */
void hw_otpc_set_speed(HW_OTPC_SYS_CLK_FREQ clk_speed);

/**
 * @brief Set or reset the power saving mode of the OTP Controller.
 *
 * @param[in] inactivity_period The number of HCLK cycles to remain powered while no access is made.
 *                               If set to 0, this feature is disabled.
 *
 * @warning The hw_otpc_init() and hw_otpc_set_speed() must have been called.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 */
void hw_otpc_power_save(uint32_t inactivity_period);

/**
 * @brief Get the number of the Repair Records.
 *
 * @return The number of used Repair Records.
 *
 * @warning The hw_otpc_init() and hw_otpc_set_speed() must have been called. The OTPC mode will be reset.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 */
uint32_t hw_otpc_num_of_rr(void);

/**
 * @brief Program manually an OTP entry.
 *
 * @param[in] cell_offset Cell offset to be programmed.
 *
 * @param[in] pword_l   The low 32-bits of the 64-bit word to be written.
 *
 * @param[in] pword_h   The high 32-bits of the 64-bit word to be written.
 *
 * @param[in] use_rr    Use a Repair Record if writing fails when true.
 *
 * @return true for success, false for failure
 *
 * @warning The hw_otpc_init() and the hw_otpc_set_speed() must have been called and the mode must
 *          be STBY.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 */
bool hw_otpc_manual_word_prog(uint32_t cell_offset, uint32_t pword_l, uint32_t pword_h, bool use_rr);

/**
 * @brief Program manually an OTP block.
 *
 * @param[in] p_data        The start address of the data to copy from.
 *
 * @param[in] cell_offset   Cell offset to start programming from.
 *
 * @param[in] cell_word     Word inside cell to start programming from.
 *
 * @param[in] num_of_words  The actual number of the words to be written.
 *
 * @param[in] use_rr       true to use Repair Records if writing fails
 *
 * @return true for success, false for failure
 *
 * @warning The hw_otpc_init() and the hw_otpc_set_speed() must have been called and the mode must
 *          be STBY.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 *
 */
bool hw_otpc_manual_prog(const uint32_t *p_data, uint32_t cell_offset, HW_OTPC_WORD cell_word,
                uint32_t num_of_words, bool use_rr);

/**
 * @brief Put the OTP in manual read mode.
 *
 * @details In manual read mode the OTP is memory mapped to the address MEMORY_OTP_BASE. So, any
 *          access to this memory space results into reading from the OTP. The caller must make sure
 *          that the function hw_otpc_manual_read_off() is called when reading from the OTP is
 *          finished.
 *
 * @param[in] spare_rows true to access the spare rows area, false to access normal memory cell
 *
 * @warning The hw_otpc_init() and the hw_otpc_set_speed() must have been called and the mode must
 *          be STBY.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 *
 */
void hw_otpc_manual_read_on(bool spare_rows);

/**
 * @brief Take the OTP out of manual read mode and put it in STBY.
 *
 * @warning The hw_otpc_init() and the hw_otpc_set_speed() must have been called and the mode must
 *          be STBY.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 *
 */
void hw_otpc_manual_read_off(void);

/**
 * @brief Program manually a Repair Record in the OTP's spare area.
 *
 * @param[in] cell_offset   The address programmed with errors.
 * @param[in] pword_l       The low 32-bits of the 64-bit word to be written.
 * @param[in] pword_h       The high 32-bits of the 64-bit word to be written.
 * @return true for success, false for failure
 *
 * @warning The hw_otpc_init() and the hw_otpc_set_speed() must have been called
 *          and the mode must be STBY.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 *
 */
bool hw_otpc_write_rr(uint32_t cell_offset, uint32_t pword_l, uint32_t pword_h);

/**
 * @brief Program OTP via DMA.
 *
 * @param[in] p_data        The start address of the data in RAM.
 *
 * @param[in] cell_offset   Cell offset to be programmed.
 *
 * @param[in] cell_word     Word inside cell to be programmed.
 *
 * @param[in] num_of_words  The actual number of the words to be written.
 *                          The driver will write this value minus 1 to the specific register.
 *
 * @param[in] spare_rows true to access the spare rows area, false to access normal memory cell
 *
 * @return true for success, false for failure (the programming should be retried or fixed!)
 *
 * @warning The hw_otpc_init() and the hw_otpc_set_speed() must have been called
 *          and the mode must be STBY.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 *
 */
bool hw_otpc_dma_prog(const uint32_t *p_data, uint32_t cell_offset, HW_OTPC_WORD cell_word,
                uint32_t num_of_words, bool spare_rows);

/**
 * @brief Read OTP via DMA.
 *
 * @param[in] p_data        The address in RAM to write the data to.
 *
 * @param[in] cell_offset   Cell offset to be programmed.
 *
 * @param[in] cell_word     Word inside cell to be programmed.
 *
 * @param[in] num_of_words  The actual number of the words to be read.
 *                          The driver will write this value minus 1 to the specific register.
 *
 * @param[in] spare_rows true to access the spare rows area, false to access normal memory cell
 *
 * @warning The hw_otpc_init() and the hw_otpc_set_speed() must have been called
 *          and the mode must be STBY.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 *
 */
void hw_otpc_dma_read(uint32_t *p_data, uint32_t cell_offset, HW_OTPC_WORD cell_word,
                uint32_t num_of_words, bool spare_rows);

/**
 * @brief Program OTP via FIFO (Auto mode).
 *
 * @param[in] p_data        The start address of the data in RAM.
 *
 * @param[in] cell_offset   Cell offset to be programmed.
 *
 * @param[in] cell_word     Word inside cell to be programmed.
 *
 * @param[in] num_of_words  The actual number of the words to be written.
 *                          The driver will write this value minus 1 to the specific register.
 *
 * @param[in] spare_rows true to access the spare rows area, false to access normal memory cell
 *
 * @return true for success, false for failure
 *
 * @warning The hw_otpc_init() and the hw_otpc_set_speed() must have been called
 *          and the mode must be STBY.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 *
 */
bool hw_otpc_fifo_prog(const uint32_t *p_data, uint32_t cell_offset, HW_OTPC_WORD cell_word,
                uint32_t num_of_words, bool spare_rows);

/**
 * @brief Read OTP via FIFO.
 *
 * @param[in] p_data        The address in RAM to write the data to.
 *
 * @param[in] cell_offset   Cell offset to be programmed.
 *
 * @param[in] cell_word     Word inside cell to be programmed.
 *
 * @param[in] num_of_words  The actual number of the words to be read.
 *                          The driver will write this value minus 1 to the specific register.
 *
 * @param[in] spare_rows true to access the spare rows area, false to access normal memory cell
 *
 * @return true for success, false for failure
 *
 * @warning The hw_otpc_init() and the hw_otpc_set_speed() must have been called
 *          and the mode must be STBY.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 */
bool hw_otpc_fifo_read(uint32_t *p_data, uint32_t cell_offset, HW_OTPC_WORD cell_word,
                uint32_t num_of_words, bool spare_rows);

/**
 * @brief Prepare OTPC to execute an OTP copy after wakeup.
 *
 * @param[in] num_of_bytes The actual number of the bytes to be copied.
 *                         The driver will write the proper value to the specific register.
 *
 * @warning The hw_otpc_init() and the hw_otpc_set_speed() must have been called
 *          and the mode must be STBY.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 */
void hw_otpc_prepare(uint32_t num_of_bytes);

/**
 * @brief Cancel any previous preparations of the OTPC for executing an OTP copy after wakeup.
 *
 * @warning The hw_otpc_init() and the hw_otpc_set_speed() must have been called
 *          and the mode must be STBY.
 *          (Note: the hw_otpc_set_speed() must be called at startup and after
 *          each HCLK speed change, since the register bits it modifies are retained.)
 *
 */
void hw_otpc_cancel_prepare(void);

/**
 * @brief Get cell memory address
 *
 * Returns mapped memory address for given cell, can be used for e.g. manual reading.
 *
 * @param[in] cell_offset cell offset
 *
 * @return cell memory address
 *
 * @sa hw_otpc_manual_read_on
 *
 */
static inline void *hw_otpc_cell_to_mem(uint32_t cell_offset)
{
        return (void *) (MEMORY_OTP_BASE + (cell_offset << 3)); // cell size is 8 bytes
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// TEST FUNCTIONALITY
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Execution of the BLANK test.
 *
 * @return The test result.
 *         <ul>
 *             <li> 0, if the test succeeded
 *             <li> 1, if the test 1 failed
 *         </ul>
 *
 */
int hw_otpc_blank(void);

/**
 * @brief Execution of the TDEC test.
 *
 * @return The test result.
 *         <ul>
 *             <li> 0, if the test succeeded
 *             <li> 1, if the test 1 failed
 *         </ul>
 *
 */
int hw_otpc_tdec(void);

/**
 * @brief Execution of the TWR test.
 *
 * @return The test result.
 *         <ul>
 *             <li> 0, if the test succeeded
 *             <li> 1, if the test 1 failed
 *         </ul>
 *
 */
int hw_otpc_twr(void);


#endif /* _HW_OTPC_58X_H_ */
