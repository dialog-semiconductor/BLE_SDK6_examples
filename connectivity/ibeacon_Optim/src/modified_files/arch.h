/**
 ****************************************************************************************
 * @addtogroup ARCH Architecture
 * @{
 * @addtogroup ARCH_SYSTEM
 * @brief System API
 * @{
 *
 * @file arch.h
 *
 * @brief This file contains the definitions of the macros and functions that are
 * architecture dependent.  The implementation of those is implemented in the
 * appropriate architecture directory.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
 */

#ifndef _ARCH_H_
#define _ARCH_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>        // standard integer definition
#include <stddef.h>        // for NULL
#include <stdbool.h>
#include "compiler.h"      // inline functions

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef enum
{
    mode_active = 0,
    mode_idle,
    mode_ext_sleep,
    mode_ext_sleep_otp_copy,
    mode_sleeping,
} sleep_mode_t;

typedef enum {
    NOT_MEASURED = 0,
    APPLY_OVERHEAD,
    NO_OVERHEAD,
} boost_overhead_st;

/// Drift values in ppm as expected by 'lld_evt.c'
typedef enum {
    DRIFT_20PPM  = (20),
    DRIFT_30PPM  = (30),
    DRIFT_50PPM  = (50),
    DRIFT_75PPM  = (75),
    DRIFT_100PPM = (100),
    DRIFT_150PPM = (150),
    DRIFT_250PPM = (250),
    DRIFT_500PPM = (500), // Default value
} drift_value_in_ppm;

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// Memory usage logging structure
struct mem_usage_log
{
    /// Maximum used size
    uint16_t max_used_sz;
    /// Heap used size
    uint16_t used_sz;
    /// Maximum size used in other heaps
    uint16_t max_used_other_sz;
    /// Size used in other heaps
    uint16_t used_other_sz;
};

extern uint32_t startup_sleep_delay;

extern uint32_t *rom_cfg_table;

/*
 * DEFINES
 ****************************************************************************************
 */

#if defined (__DA14531__)
#define PACKAGE_QFN_24      (0xAA)
#define PACKAGE_WLCSP_17    (0xFF)

#define NO_SWD              (0x00)
#define SWD_DATA_AT_P0_5    (0x01)
#define SWD_DATA_AT_P0_1    (0x02)
#define SWD_DATA_AT_P0_10   (0x03)
#endif

/** @brief OTP memory base address */
#define MEMORY_OTP_BASE     (0x07F80000)
#if defined (__DA14531__)
    #define MEMORY_OTP_END      (0x07F88000)
#else
    #define MEMORY_OTP_END      (0x07F90000)
#endif

/** @brief OTP memory size (64 KB in DA14585/586 - 32KB in DA14531) */
#define MEMORY_OTP_SIZE         (MEMORY_OTP_END - MEMORY_OTP_BASE)

/// ARM is a 32-bit CPU
#define CPU_WORD_SIZE               (4)

/// ARM is little endian
#define CPU_LE                      (1)

/******************************************
 * da1458x_config_basic/advanced.h SETTINGS
 ******************************************
 */

#if defined (CFG_LUT_PATCH)
    #define LUT_PATCH_ENABLED                           1
#else
    #define LUT_PATCH_ENABLED                           0
#endif /* CFG_LUT_PATCH */

#if !defined (__FPGA__)
#if defined (__DA14531__)
    #if defined (CFG_TRNG)
        #define USE_TRNG                                (1)
    #else
        #define USE_TRNG                                (0)
    #endif
#else
    #if ((CFG_TRNG == 32) || (CFG_TRNG == 64) || (CFG_TRNG == 128) || \
        (CFG_TRNG == 256) || (CFG_TRNG == 512)  || (CFG_TRNG == 1024))
        #define USE_TRNG                                (1)
    #elif (!defined (CFG_TRNG)) || (CFG_TRNG == 0)
        #define USE_TRNG                                (0)
    #else
        #warning "Config error: Not supported TRNG configuration"
    #endif
#endif // __DA14531__
#endif // __FPGA__

#if defined (CFG_ENABLE_SMP_SECURE)
    #define ENABLE_SMP_SECURE                           1
#else
    #define ENABLE_SMP_SECURE                           0
#endif /* CFG_ENABLE_SMP_SECURE */

#if defined (CFG_POWER_OPTIMIZATIONS)
    #define USE_POWER_OPTIMIZATIONS                     1
#else
    #define USE_POWER_OPTIMIZATIONS                     0
#endif

#if defined (CFG_HIGH_TEMPERATURE)
    #define USE_HIGH_TEMPERATURE                        1
#else
    #define USE_HIGH_TEMPERATURE                        0
#endif /* CFG_HIGH_TEMPERATURE */

#if defined (CFG_AMB_TEMPERATURE)
    #define USE_AMB_TEMPERATURE                         1
#else
    #define USE_AMB_TEMPERATURE                         0
#endif /* CFG_AMB_TEMPERATURE */

#if defined (CFG_MID_TEMPERATURE)
    #define USE_MID_TEMPERATURE                         1
#else
    #define USE_MID_TEMPERATURE                         0
#endif /* CFG_MID_TEMPERATURE */

#if defined (CFG_EXT_TEMPERATURE)
    #define USE_EXT_TEMPERATURE                         1
#else
    #define USE_EXT_TEMPERATURE                         0
#endif /* CFG_EXT_TEMPERATURE */

#if defined (CFG_DEVELOPMENT_DEBUG)
    #define DEVELOPMENT_DEBUG                           1
#else
    #define DEVELOPMENT_DEBUG                           0
#endif

#if defined (CFG_USE_DEFAULT_XTAL16M_TRIM_VALUE_IF_NOT_CALIBRATED)
    #define DEFAULT_XTAL16M_TRIM_VALUE_USED             1
#else
    #define DEFAULT_XTAL16M_TRIM_VALUE_USED             0
#endif

/// RCX MEAS
#if defined(CFG_RCX_MEASURE)
#define RCX_MEASURE_ENABLED                             1
#else
#define RCX_MEASURE_ENABLED                             0
#endif

#if defined(CFG_LOG_HEAP_USAGE)
#define LOG_HEAP_USAGE                                  1
#else
#define LOG_HEAP_USAGE                                  0
#endif

#if defined(CFG_GTL_SPI)
#define GTL_SPI_SUPPORTED                               1
#else
#define GTL_SPI_SUPPORTED                               0
#endif

#if defined(CFG_EXTERNAL_WAKEUP)
#define EXTERNAL_WAKEUP                                 1
#else
#define EXTERNAL_WAKEUP                                 0
#endif

#if defined(CFG_WAKEUP_EXT_PROCESSOR)
#define WKUP_EXT_PROCESSOR                              1
#else
#define WKUP_EXT_PROCESSOR                              0
#endif

#if defined(CFG_BLE_METRICS)
#define BLE_METRICS                                     1
#else
#define BLE_METRICS                                     0
#endif

#if defined(CFG_PRODUCTION_DEBUG_OUTPUT)
#define PRODUCTION_DEBUG_OUTPUT                         1
#else
#define PRODUCTION_DEBUG_OUTPUT                         0
#endif

#if defined(CFG_PRODUCTION_TEST)
#define PRODUCTION_TEST                                 1
#else
#define PRODUCTION_TEST                                 0
#endif

#if defined (CFG_USE_AES)
#define USE_AES                                         1
    #if defined (CFG_AES_DECRYPT)
    #define USE_AES_DECRYPT                             1
    #else
    #define USE_AES_DECRYPT                             0
    #endif
#else
#define USE_AES                                         0
#define USE_AES_DECRYPT                                 0
#endif

#if defined(CFG_USE_CHACHA20_RAND)
#define USE_CHACHA20_RAND                               1
#else
#define USE_CHACHA20_RAND                               0
#endif

#if defined(CFG_RANGE_EXT)
#define USE_RANGE_EXT                                   1
#else
#define USE_RANGE_EXT                                   0
#endif

#if defined(CFG_XTAL16M_ADAPTIVE_SETTLING)
#define USE_XTAL16M_ADAPTIVE_SETTLING                   1
#else
#define USE_XTAL16M_ADAPTIVE_SETTLING                   0
#endif

#if defined(CFG_DISABLE_QUADEC_ON_START_UP)
#define DISABLE_QUADEC_ON_START_UP                      1
#else
#define DISABLE_QUADEC_ON_START_UP                      0
#endif

#if defined(CFG_POWER_MODE_BYPASS)
#define USE_POWER_MODE_BYPASS                           1
#else
#define USE_POWER_MODE_BYPASS                           0
#endif

#if defined (CFG_COEX)
#define WLAN_COEX_ENABLED                               1
#else
#define WLAN_COEX_ENABLED                               0
#endif

/*
 * Deep sleep threshold. Application specific. Control if during deep sleep the system RAM will be powered off and if OTP copy will be required.
 ****************************************************************************************
 */

#define MS_TO_SLOTS_CONVERT(x)  ((int)((1000 * x) / 625))

/// Sleep Duration Value in periodic wake-up mode
#define MAX_SLEEP_DURATION_PERIODIC_WAKEUP      MS_TO_SLOTS_CONVERT(CFG_MAX_SLEEP_DURATION_PERIODIC_WAKEUP_MS)
/// Sleep Duration Value in non external wake-up mode
#define MAX_SLEEP_DURATION_EXTERNAL_WAKEUP      MS_TO_SLOTS_CONVERT(CFG_MAX_SLEEP_DURATION_EXTERNAL_WAKEUP_MS)

#define DEEP_SLEEP_THRESHOLD    (800000)

/*
 * Duration from XTAL16M active to XTAL16M settle.
 ****************************************************************************************
 */
#define XTAL16M_TRIM_DELAY_SETTING   (0xA2)  // TRIM_CTRL_REG value ==> 14 cycles of 250usec each = 3.5ms (note: RC16M is used)

/*
 * XTAL16M settling time measured in low power clock cycles.
 * Cannot be less than 1.9ms due to the OTP copy in deep sleep, which may take quite long
 * if RC16M is too low (i.e. 10MHz).
 ****************************************************************************************
 */
#define XTAL16M_SETTLING_IN_XTAL32_CYCLES       (95)    // count for 95 LP cycles, (x 30.5usec = ~2.9ms)
#define XTAL16M_SETTLING_IN_USEC                (2900)  // when RCX is used

/*
 * Wakeup timing parameters.
 ****************************************************************************************
 */

#if (USE_POWER_OPTIMIZATIONS)
#define MINIMUM_SLEEP_DURATION      (1250)  // 2 slots (3 or more slots will be allowed (note: not for timer wakeups))
#define XTAL32_OTP_COPY_OVERHEAD    (50)
#define RCX_OTP_COPY_OVERHEAD       (17)
#if !defined (__DA14531__)
#define LP_ISR_TIME_USEC            (3357)  // 110 * (1000000/32768); must be greater than XTAL16M_SETTLING_IN_USEC
#define LP_ISR_TIME_XTAL32_CYCLES   (110)
#define XTAL32_POWER_UP_TIME        (21)
#define RCX_POWER_UP_TIME           (18)
#define BOOST_POWER_UP_OVERHEAD     (33)    // 1msec
#define CLK_TRIM_WAIT_CYCLES        (1000)  // How many 16MHz cycles we will wait before applying the correct FREQ TRIM value
#endif // !defined (__DA14531__)
#endif // (USE_POWER_OPTIMIZATIONS)

/* Note
 * --------------------------
 * After XTAL16M settling, the code needs ~10usec + 1/2 LP cycle to power-up BLE and pop BLE registers.
 * This translates to ~26usec for XTAL32K and ~57usec for RCX.
 *
 * Thus, LP_ISR_TIME_USEC - XTAL16M settling time = 3357 - 2900 = 457usec and 457 - 57 = 400usec
 * are left as a safety margin in the worst case (RCX) (best case is 3357 - 2900 - 26 = 431 for XTAL32K).
 *
 * In principle, this time can be reduced to be at least 2 low power cycles.
 *
 * If the remaining time is not enough for periph_init() to complete then an ASSERTION will hit in
 * BLE_WAKEUP_LP_Handler (when DEVELOPMENT_DEBUG is 1). In this case, since the settling time of the
 * XTAL16M is fixed, the user should increase the LP_ISR_TIME_USEC (and, consequently, the
 * LP_ISR_TIME_XTAL32_CYCLES).
 */

#if !defined (__DA14531__) && (!USE_POWER_OPTIMIZATIONS)
#define XTAL_TRIMMING_TIME_USEC     (5798)  // 190 * (1000000/32768)
#define XTAL_TRIMMING_TIME          (190)
#endif

/*
 * Minimum and maximum LP clock periods. Measured in usec.
 ****************************************************************************************
 */
#define XTAL32_PERIOD_MIN           (30)    // 30.5usec
#define XTAL32_PERIOD_MAX           (31)
#define RCX_PERIOD_MIN              (85)    // normal range: 91.5 - 93usec
#define RCX_PERIOD_MAX              (200)

/*
 * DEEP SLEEP: Power down configuration
 ****************************************************************************************
 */
#define TWIRQ_RESET_VALUE           (1)
#define TWEXT_VALUE_RCX             (TWIRQ_RESET_VALUE + 2) // ~190usec (LP ISR processing time) / 85usec (LP clk period) = 2.23 => 3 LP cycles
#define TWEXT_VALUE_XTAL32          (TWIRQ_RESET_VALUE + 6) // ~190usec (LP ISR processing time) / 30usec (LP clk period) = 6.3 => 7 LP cycles


// The times below have been measured for RCX. For XTAL32K will be less. But this won't make much difference...
#define SLP_PROC_TIME               (60)    // 60 usec for SLP ISR to request Clock compensation
#define SLEEP_PROC_TIME             (30)    // 30 usec for rwip_sleep() to program Sleep time and drive DEEP_SLEEP_ON

// Change this if the application needs to increase the sleep delay limit from 9375usec that is now.
#define APP_SLEEP_DELAY_OFFSET      (0)

/// Possible errors detected by FW
#define    RESET_NO_ERROR           (0x00000000)
#define    RESET_MEM_ALLOC_FAIL     (0xF2F2F2F2)

/// Reset platform and stay in ROM
#define    RESET_TO_ROM             (0xA5A5A5A5)
/// Reset platform and reload FW
#define    RESET_AND_LOAD_FW        (0xC3C3C3C3)

#define    RESET_AFTER_SUOTA_UPDATE (0x11111111)

#if defined (__DA14531__)
#define STARTUP_SLEEP_DELAY_DEFAULT (480)    // Sleep delay 300 msec for XTAL32K to settle (480 * 625usec)
#else
#define STARTUP_SLEEP_DELAY_DEFAULT (3200)   // Sleep delay 2 sec for XTAL32K to settle (3200 * 625usec)
#endif

#if defined (__DA14531__)
#define HW_STARTUP_TIME_IN_XTAL32K_CYCLES    (11)   // 11 LP clocks for startup state machine handling
#define HW_STARTUP_TIME_IN_RCX_CYCLES        (7)    // 7 LP clocks for startup state machine handling
#define RCX_BLE_PWR_UP_TO_SLP_IRQ_USEC       (60)
#define XTAL32K_BLE_PWR_UP_TO_SLP_IRQ_USEC   (45)
#endif

/*
 * Values for the XTAL16M adaptive settling algorithm.
 ****************************************************************************************
 */
#if (USE_XTAL16M_ADAPTIVE_SETTLING && USE_POWER_OPTIMIZATIONS)

    #define XTAL16M_CAL_MAX_TRIES                   (5)
    #define CAL_LP_TICKS                            (4)
    #define EXPECTED_XTAL32K_CYCLES                 ((488 * CAL_LP_TICKS) + (CAL_LP_TICKS / 3))

    #define TWIRQSET_MIN_XTAL32_TICKS               (19)
    #define TWIRQSET_MAX_XTAL32_TICKS               (400)
    #define XTAL16_CALIBRATION_SUCCESS_COUNT        (20)
    #define XTAL16_LP_CYCLES_OFFSET                 (6)

    #define XTAL16_CALIBRATION_IN_XTAL32_TICKS      (6)
    #define XTAL16_SAFETY_OFFSET_IN_XTAL32_TICKS    (12)
    #define TWIRQSET_INIT_XTAL32_TICKS              (TWIRQSET_MIN_XTAL32_TICKS + 60)
    #define TWIRQRESET_XTAL32_TICKS                 (XTAL16_CALIBRATION_IN_XTAL32_TICKS + XTAL16_SAFETY_OFFSET_IN_XTAL32_TICKS)

    # if ((TWIRQRESET_XTAL32_TICKS + 1) > TWIRQSET_MIN_XTAL32_TICKS)
    # error "Minimum wakeup time (TWIRQSET_MIN_XTAL32_TICKS) is too small ..."
    # endif
#endif

/*
 * EXPORTED FUNCTION DECLARATION
 ****************************************************************************************
 */

/*
 * External wake up declarations and includes
 ****************************************************************************************
 */

#if ((EXTERNAL_WAKEUP) && (!BLE_APP_PRESENT)) // external wake up, only in full embedded designs
/**
 ****************************************************************************************
 * @brief Enable external wake up GPIO Interrupt.
 * @param[in] port The GPIO port of the external wake up signal
 * @param[in] pin The GPIO pin of the external wake up signal
 * @param[in] polarity The polarity of the external wake up interrupt. 0=active low. 1=active high
 ****************************************************************************************
 */
void ext_wakeup_enable(uint32_t port, uint32_t pin, uint8_t polarity);

/**
 ****************************************************************************************
 * @brief Disable external wake up GPIO Interrupt.
 ****************************************************************************************
 */
void ext_wakeup_disable(void);

/**
 ****************************************************************************************
 * @brief Notifies that system has been woken up.
 ****************************************************************************************
 */
void ext_wakeup_resume_from_sleep(void);

/**
 ****************************************************************************************
 * @brief Callback function, called when external wakeup function is triggered.
 ****************************************************************************************
 */
void ext_wakeup_cb(void);
#endif

/**
 ****************************************************************************************
 * @brief Re-boot FW. This function is used to re-boot the FW when error has been
 *        detected, it is the end of the current FW execution. After waiting transfers
 *        on UART to be finished, and storing the information that FW has re-booted by
 *        itself in a non-loaded area, the FW restart by branching at FW entry point.
 * @note When calling this function, the code after it will not be executed.
 * @param[in] error Error detected by FW
 ****************************************************************************************
 */
void platform_reset(uint32_t error);

/**
 ****************************************************************************************
 * @brief Checks if system clocks (AHB & APB) can be lowered (weak definition).
 * @return true if system clocks can be lowered, otherwise false
 ****************************************************************************************
 */
bool app_use_lower_clocks_check(void);

/**
 ****************************************************************************************
 * @brief Sets the BLE wake-up delay.
 ****************************************************************************************
 */
void set_sleep_delay(void);

/**
 ****************************************************************************************
 * @brief Starts RCX20 calibration.
 * @param[in] cal_time Calibration time in RCX20 cycles
 ****************************************************************************************
 */
void calibrate_rcx20(uint16_t cal_time);

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Calculates RCX20 frequency.
 * @param[in] wait_for_calibration  If true, function waits for calibration to complete,
 *                                  else it returns immediately if calibration is in
 *                                  progress.
 ****************************************************************************************
 */
void read_rcx_freq(bool wait_for_calibration);

#else
/**
 ****************************************************************************************
 * @brief Calculates RCX20 frequency.
 * @param[in] cal_time Calibration time in RCX20 cycles
 ****************************************************************************************
 */
void read_rcx_freq(uint16_t cal_time);
#endif


/**
 ****************************************************************************************
 * @brief Selects conversion function (XTAL32K or RCX20) for low power cycles to us.
 * @param[in] lpcycles Low power cycles
 * @return uint32_t microseconds
 ****************************************************************************************
 */
uint32_t lld_sleep_lpcycles_2_us_sel_func(uint32_t lpcycles);

/**
 ****************************************************************************************
 * @brief Selects conversion function (XTAL32K or RCX20) for us to low power cycles.
 * @param[in] us microseconds
 * @return uint32_t Low power cycles
 ****************************************************************************************
 */
uint32_t lld_sleep_us_2_lpcycles_sel_func(uint32_t us);

/**
 ****************************************************************************************
 * @brief Runs conditionally (time + temperature) RF and coarse calibration.
 ****************************************************************************************
 */
void conditionally_run_radio_cals(void);

/**
 ****************************************************************************************
 * @brief Check gtl state.
 * @return If gtl is idle returns true. Otherwise returns false.
 ****************************************************************************************
 */
bool check_gtl_state(void);

/**
 ****************************************************************************************
 * @brief Checks if system is in startup phase.
 * @return 1 if system is in startup phase, otherwise 0.
 ****************************************************************************************
 */
uint8_t check_sys_startup_period(void);

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Set the XTAL32M trim value.
 * @param[in] trim_value    Trim value
 ****************************************************************************************
 */
void set_xtal32m_trim_value(uint16_t trim_value);
#else
/**
 ****************************************************************************************
 * @brief Set the XTAL16M trim value.
 * @param[in] trim_value    Trim value
 ****************************************************************************************
 */
void set_xtal16m_trim_value(uint16_t trim_value);
#endif

void dbg_prod_output(int mode, unsigned long *hardfault_args);

/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR(cond)

/// Assertions showing a critical error that could require a full system reset
#define ASSERT_INFO(cond, param0, param1)

/// Assertions showing a non-critical problem that has to be fixed by the SW
#define ASSERT_WARN(cond)

// required to define GLOBAL_INT_** macros as inline assembly. This file is included after
// definition of ASSERT macros as they are used inside ll.h
#include "ll.h"     // ll definitions

/**
 ****************************************************************************************
 * ASSERTION CHECK
 ****************************************************************************************
 */
#ifndef ASSERT_ERROR
#define ASSERT_ERROR(x)                                     \
{                                                           \
    do {                                                    \
        if (DEVELOPMENT_DEBUG)                              \
        {                                                   \
            if (!(x))                                       \
            {                                               \
                __BKPT(0);                                  \
            }                                               \
        }                                                   \
        else                                                \
        {                                                   \
            if (!(x))                                       \
            {                                               \
                wdg_reload(1);                              \
                wdg_resume();                               \
                while(1);                                   \
            }                                               \
        }                                                   \
    } while (0);                                            \
}

#define ASSERT_WARNING(x)                                   \
{                                                           \
    do {                                                    \
        if (DEVELOPMENT_DEBUG)                              \
        {                                                   \
            if (!(x))                                       \
            {                                               \
                __BKPT(0);                                  \
            }                                               \
        }                                                   \
    } while (0);                                            \
}
#endif

#endif // _ARCH_H_

/// @}
/// @}