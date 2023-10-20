/**
 ****************************************************************************************
 *
 * @file Xtal_TRIM.c
 *
 * @brief Automated XTAL trim calculation core.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*
inputs:
        auto_trim(uint8_t XTAL_select, uint8_t port_number)
        Xtal_select: 0 = 16MHz / 1 = 32MHz
        TRIM-limits are dependent of the XTAL
        port_number: input of the 300 or 500 msec signal for XTAL-calibration
outputs:
        return (-1)     // -1 = no square pulse detected
        return (-2)     // -2 = square pulse outside boundaries
        return (-3)     // -3 = failed to write otp value
        return (-4)     // -4 = wrong input of port_number
        return (-5)     // -5 = wrong input of XTAL_select
        return (TRIM)   // TRIM-value is returned
*/

// connection for testing
// Tx = P0.1 -> terminal emulator
// Rx = P0.0 -> input for 500 msec pulse

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "math.h"
#include "Xtal_TRIM.h"
#include "Xtal_TRIM_platform.h"
#if AUTO_XTAL_TEST_DBG_EN
#include "arch_console.h"
#endif

/*
 *  Macro definitions
 */

#define MAX_VAL(val1, val2)             (val1 > val2) ? val1 : val2
#define MIN_VAL(val1, val2)             (val1 < val2) ? val1 : val2

// General parameters
#define MAX_LOOPS                       (10)
#define DELAY_1MSEC                     (1777)              // delay x * 1 msec

typedef enum __xtal_sections{
    XTAL_SEC_MIN = 0,
    XTAL_SEC_MAX,
    XTAL_SEC_ENUM_MAX_VAL,                                  // NOT TO BE USED INSIDE THE CODE, DO NOT REMOVE, This shows the size of the enumeration.....
}_xtal_sections;


/*
 * Variables
 */
volatile uint32_t TRIM_MIN;
volatile uint32_t TRIM_MAX;
volatile uint32_t border[XTAL_SEC_ENUM_MAX_VAL] = {0};
bool flag_XTAL32M = false;                                  // = 0 => 16M / = 1 => 32M
bool PulseError = false;                                    // __585__ // Error_Flag when no pulses arriving.
volatile uint32_t actual_trimming_value;

/*
 * Function Declaration
 */
void delay(uint32_t dd);
void Setting_Trim(uint32_t Trim_Value);
long Clock_Read(uint8_t port_number);
int linearization(int C, int Cmin, int Cmax, int Tmin, int Tmax);

// *** delay routine x * 1 msec / is controlled by 16M or 32M XTAL
void delay(uint32_t dd)
{
    uint32_t j, jj;

    if (flag_XTAL32M == 1)
        dd = 2 * dd;       // true := 32M / false := 16M

    jj = dd * DELAY_1MSEC;

    for (j = 1; j<=jj; j++)
    {
        __NOP();
        __NOP();
    }
}

// *** boundary testing new TRIM-value and storing in CLK_FREQ_TRIM_REG
void Setting_Trim(uint32_t Trim_Value)                  // program new Trim_Value
{
    if (Trim_Value < TRIM_MIN && Trim_Value != 0)   Trim_Value = TRIM_MIN;
    if (Trim_Value > TRIM_MAX)                      Trim_Value = TRIM_MAX;

    xtal_pltfrm_set_val_to_reg(Trim_Value);

    delay(2);       // delay = 2msec
}

/**
******************************************************************************************
* @brief Measures the high duration of an externally applied square pulse in system ticks.
*
* @param[in] port_number    GPIO where the square pulse is applied on.
* @return   Zero if no external pulse is detected on the given GPIO.
*           Otherwise the pulse high duration in system ticks is returned.
******************************************************************************************
*/
long Clock_Read(uint8_t port_number)
{
    int32_t cnt_output = 0;
    uint16_t shift_bit;
    uint32_t datareg;
    volatile uint32_t tick_counter = 0;

    if (xtal_pltfrm_get_port_pin_reg(port_number, &datareg, &shift_bit) != XTAL_PLTFRM_NO_ERROR)
        return 0;

    // configure systick timer
    SysTick->LOAD = 0xFFFFFF;
    SysTick->VAL = 0;
    SysTick->CTRL |= 0x04;  // use processor-clock

    // during counting, no interrupts should appear
    GLOBAL_INT_DISABLE();                                   // disable interrupts

    tick_counter =  xtal_pltfrm_measure_pulse(datareg, shift_bit);
    SysTick->CTRL &= ~(0x01);                               // stop systick timer ... bit 0: ENABLE

    GLOBAL_INT_RESTORE();                                   // enable interrupts

    cnt_output = 0xFFFFFF - tick_counter;

    PulseError = false;                                     // {xx}
    if (cnt_output == 0xFFFFFF)
    {
        PulseError = true;
    }

    return cnt_output;
}

// *** calculate new TRIM via linear line algorithm
int linearization(int C, int Cmin, int Cmax, int Tmin, int Tmax)
{
    int T;

    T = Tmin + (Cmax - C) * (Tmax - Tmin) / (Cmax - Cmin);

    return T;
}

// *** main function is start of auto-calibration
int auto_trim(uint8_t XTAL_select, uint8_t port_number)
{
    int response = WRONG_XTAL_SOURCE_ERROR;
    uint8_t loop = 1;                               // loop 1 ... max 10

    uint32_t Trim_low = 0;
    uint32_t Trim_hi = 0;
    static volatile uint32_t Trim_curr = 0;
    volatile uint32_t ticks_hi = 0, ticks_low = 0;
    volatile uint32_t ticks_curr = 0;
    static volatile uint32_t IDEAL_XTAL_count = 0;
    static volatile uint32_t PPM_1 = 0, PPM_2 = 0;
    static volatile uint32_t START_TRIM_VALUE = 0;

#if AUTO_XTAL_TEST_DBG_EN
    int debug_array_trim[MAX_LOOPS+2] = {-4};       // -4 means that the cell is not accessed yet...
    int debug_array_diff_ticks[MAX_LOOPS+2] = {-4}; // -4 means that the cell is not accessed yet...

    memset(debug_array_trim, -4, (MAX_LOOPS+2)*sizeof(int));
    memset(debug_array_diff_ticks, -4, (MAX_LOOPS+2)*sizeof(int));

    arch_printf("Debugging of Auto XTAL\r\n");
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////
    // Initiliazation Part
    ////////////////////////////////////////////////////////////////////////////////////////////

    {
        // Initialize values regarding the type of pulse.
        // 0 = 16MHz and 1 = 32MHz
        switch (XTAL_select)
        {
            case 0:
                flag_XTAL32M = false;                   // XTAL = 16M
                TRIM_MIN = TRIM_MIN_16M;                // MIN and MAX values of 16M Xtal
                TRIM_MAX = TRIM_MAX_16M;
                IDEAL_XTAL_count = XTAL16M;
                PPM_1 = PPM_1_16M;
                PPM_2 = PPM_2_16M;
                START_TRIM_VALUE = START_TRIM_VALUE_16M;
                break;                                  // 500 msec  x_ideal = 1148 (16M*0.5=8M)
            case 1:
                flag_XTAL32M = true;                    // XTAL = 32M
                TRIM_MIN = TRIM_MIN_32M;                // MIN and MAX values of 32M Xtal
                TRIM_MAX = TRIM_MAX_32M;
                IDEAL_XTAL_count = XTAL32M;
                PPM_1 = PPM_1_32M;
                PPM_2 = PPM_2_32M;
                START_TRIM_VALUE = START_TRIM_VALUE_32M;
                break;                                  // 300 msec  x_ideal = 218 (32M*0.3=9.6M)
            default:
            {
                response = WRONG_XTAL_SOURCE_ERROR;     // wrong input of XTAL_CNT
                goto XTAL_TRIM_END;
            }
        }

        // Set borders....
        border[XTAL_SEC_MIN] = TRIM_MIN;
        border[XTAL_SEC_MAX] = TRIM_MAX;

        // Start value for Trim_next
        Trim_curr = START_TRIM_VALUE;

        // Set port_number as an input with pull up resistor.
        if (xtal_pltfrm_set_port_in_pu(port_number) == INVALID_GPIO_ERROR) {
            response = INVALID_GPIO_ERROR;
            goto XTAL_TRIM_END;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    // End of Initiliazation Part
    ////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////
    // Run with initial Value
    ////////////////////////////////////////////////////////////////////////////////////////////

    // Set initial trim value and read back the counts.
    // If the measurement is within acceptable range, finish here...
    {
        // ** 1e Setting_Trim & temp = Clock_Read()
        Setting_Trim(Trim_curr);    // start e.g. Trim at 350

        // ** 2e Clock_Read
        ticks_curr = Clock_Read(port_number);
#if AUTO_XTAL_TEST_DBG_EN
        debug_array_trim[0] = Trim_curr;
        debug_array_diff_ticks[0] = ticks_curr;// - IDEAL_XTAL_count;
#endif

        // ** 3e if abs(temp - C_ideal) <= in spec => break
        if ((ticks_curr >= IDEAL_XTAL_count) && ((ticks_curr - IDEAL_XTAL_count) <= PPM_1)) { // XTAL32M = C_ideal (9.6M at 300ms)
            response = Trim_curr;
            goto XTAL_TRIM_END;
        }
        if ((ticks_curr < IDEAL_XTAL_count) && ((IDEAL_XTAL_count - ticks_curr) <= PPM_1)) {  // XTAL32M = C_ideal (9.6M at 300ms)
            response = Trim_curr;
            goto XTAL_TRIM_END;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    // End of Run with initial Value
    ////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////
    // Run for the edge point that will be used
    ////////////////////////////////////////////////////////////////////////////////////////////

    // At this point we only have one measurement. We need a second to complete the triangles.
    // This is going to be the opposite of the point that will change later on.
    // Prepare the next point based on the calculation of the ideal value.
    // Calculate the edge point to be used for the next measurement.
    {
        // ** 4e set Trim at Tmax or Tmin
        if (ticks_curr > IDEAL_XTAL_count) //left part
        {
            Trim_hi = border[XTAL_SEC_MAX];
            Setting_Trim(border[XTAL_SEC_MAX]); // start e.g. Trim at 250
            // ** 5e Clock Read at Trim
            ticks_hi = Clock_Read(port_number); // at Tmin (at C_max) or Tmax (at C_min)
#if AUTO_XTAL_TEST_DBG_EN
            debug_array_trim[1] = Trim_hi;
            debug_array_diff_ticks[1] = ticks_hi;// - IDEAL_XTAL_count;
#endif
        }
        else
        {
            Trim_low = border[XTAL_SEC_MIN];
            Setting_Trim(border[XTAL_SEC_MIN]);
            // ** 5e Clock Read at Trim
            ticks_low = Clock_Read(port_number); // at Tmin (at C_max) or Tmax (at C_min)
#if AUTO_XTAL_TEST_DBG_EN
            debug_array_trim[1] = Trim_low;
            debug_array_diff_ticks[1] = ticks_low;// - IDEAL_XTAL_count;
#endif
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    // End Run for the edge point that will be used
    ////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////
    // Minimize the calculation error
    ////////////////////////////////////////////////////////////////////////////////////////////

    /*  The following parameters are needed to enter the main loop
        Trim_low
        Trim_hi
        Trim_curr
        ticks_low       // Coupled to Trim_low
        ticks_hi        // Coupled to Trim_hi
        ticks_curr
    */

    // Try to minimilize the calculation error by continiously closing to the ideal count.
    {
        // ** 6e loop
        loop = 0;
        do
        {
            loop++; // max amount of loops

            // Depending on the tick offset (positive or negative regarding the ideal count),
            // determine the direction of the next measurement.
            // Based on that decision, take a measurement on the edge of that direction.

            // We have three types of points
            // LOW, CURR, HIGH.
            // In each iteration we should update our triangles and narrow them to the optimal value.
            if (ticks_curr > IDEAL_XTAL_count)
            {
                // Assign the current value as the low value
                // High value remains the same.
                Trim_low = Trim_curr;
                ticks_low = ticks_curr;
            }
            else
            {
                // Assign the current value as the high value
                // Low value remains the same.
                Trim_hi = Trim_curr;
                ticks_hi = ticks_curr;
            }

            // **  8e  Trim_next = sub linearization
            Trim_curr = linearization(IDEAL_XTAL_count, MIN_VAL(ticks_low, ticks_hi), MAX_VAL(ticks_low, ticks_hi), Trim_low, Trim_hi);

            // **  9e Trim = Trim_next
            Setting_Trim(Trim_curr);

            // ** 10e temp = Clock_Read()
            ticks_curr = Clock_Read(port_number);

#if AUTO_XTAL_TEST_DBG_EN
            debug_array_trim[loop+1] = Trim_curr;
            debug_array_diff_ticks[loop+1] = ticks_curr;// - IDEAL_XTAL_count;
#endif
            // ** 11e if abs(temp - C_ideal) <= in spec => break
            if ((ticks_curr >= IDEAL_XTAL_count) && ((ticks_curr - IDEAL_XTAL_count) <= PPM_1))   // XTAL32M = C_ideal (9.6M at 300ms)
                break;  // out of while
            if ((ticks_curr < IDEAL_XTAL_count) && ((IDEAL_XTAL_count - ticks_curr) <= PPM_1))    // XTAL32M = C_ideal (9.6M at 300ms)
                break;  // out of while
        }
        while(loop < MAX_LOOPS/*=10*/);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    // End of Minimize the calculation error
    ////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////
    // FAILED TO CALCULATE. RESPOND WITH THE REASON.
    ////////////////////////////////////////////////////////////////////////////////////////////

    // Check the outcome of the auto trim calibration procedure.
    // Respond depending on the result.
    {
        // ** 12e check square pulse
        if (PulseError == true)                             // no square pulse detected
        {
            Setting_Trim(0);                                // if no square pulse was detected, then TRIM = 0
            response = NO_PULSE_ERROR;                      // no square pulse detected
            goto XTAL_TRIM_END;
        }

        // ** 13e Check if the clock count is within 1 PPM
        if ((ticks_curr < (IDEAL_XTAL_count - PPM_1)) ||
            (ticks_curr > (IDEAL_XTAL_count + PPM_1)))
        {
            Setting_Trim(0);                                // if no square pulse was detected, then TRIM = 0
            response = PULSE_OUT_OF_RANGE_ERROR;            // no square pulse detected
            goto XTAL_TRIM_END;
        }

        xtal_pltfrm_get_val_from_reg(&actual_trimming_value);

        // ** 14e Check if the trim value is within boundaries
        if (actual_trimming_value <= TRIM_MIN && actual_trimming_value > 0) {   // TRIM-value = 512  out of spec!
            response = PULSE_OUT_OF_RANGE_ERROR;
            goto XTAL_TRIM_END;
        }
        if (actual_trimming_value >= TRIM_MAX) {                                // {32M} TRIM-value = 2047 out of spec!
            response = PULSE_OUT_OF_RANGE_ERROR;
            goto XTAL_TRIM_END;
        }

        // ** 15e return actual TRIM-value
        response = actual_trimming_value;                   // actual TRIM-value is send back
        goto XTAL_TRIM_END;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    // End of FAILED TO CALCULATE. RESPOND WITH THE REASON.
    ////////////////////////////////////////////////////////////////////////////////////////////


XTAL_TRIM_END:

#if AUTO_XTAL_TEST_DBG_EN
    // Print debug array in UART2

    arch_printf("\r\n");
    arch_printf("XTAL TRIM DEBUG ARRAY:");
    arch_printf("_____________________\r\n");
    arch_printf("|##|  Trim|  Ticks  |\r\n");
    for (int dbg_cnt=0; dbg_cnt < MAX_LOOPS; dbg_cnt++)
    {
        arch_printf("|%02d|", dbg_cnt+1);
        if (debug_array_trim[dbg_cnt] == -4) {
            arch_printf("|      |");
        }else {
            arch_printf("|%06d|", debug_array_trim[dbg_cnt]);
        }
        if (debug_array_diff_ticks[dbg_cnt] == -4) {
            arch_printf("|        |");
        }else {
            arch_printf("|%08d|", debug_array_diff_ticks[dbg_cnt]);
        }
        arch_printf("\r\n");
    }
    arch_printf("|__|______|_________|\r\n");
    arch_printf("\r\n");
#endif

    return response;
}

/*
 * Debug function
 */
#if AUTO_XTAL_TEST_DBG_EN
void TRIM_test (int S1, int S2) // measuring Arrays in TRIM-values
{
#if 0
        volatile signed int i, j;
        static int ff[2050];

        for (i=S1; i<=S2; i++)
        {
#if (dg_configDEVICE == DEVICE_DA14680)
                CRG_TOP->CLK_FREQ_TRIM_REG = i;
#elif (dg_configDEVICE == DEVICE_D2522)
                CRG_XTAL->CLK_FREQ_TRIM_REG = i;
#endif
                ff[i] = Clock_Read(23);
                j = 9600000 - ff[i];
                __NOP();
        }
#endif
}
#endif
