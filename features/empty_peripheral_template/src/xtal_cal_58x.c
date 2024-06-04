/**
 ****************************************************************************************
 *
 * @file xtal_cal_58x.c
 *
 * @brief External oscillator calculation functions source code file.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */


#include <stdbool.h>
#include <core_cm0.h>
#include "gpio.h"
#include "ll.h"


// *** defines
#define PULSE_OUT_OF_RANGE_ERROR    (-1)                    // Pulse found in the pulse pin assigned GPIO was out of acceptable range
#define NO_PULSE_ERROR              (-2)                    // No pulse found, or pulse > 740 ms (measure_pulse aborts)

#define TEMP_OFFSET         0                               // 40 = 5 ppm
#define IDEAL_CNT           8000000 - 4 + TEMP_OFFSET       // 500 msec  x = 1148 (8M)

#define ACCURACY_STEP       9                               // using the SYSTICK: accuracy is 7 clocks

#define DELAY_1MSEC         1777                            // delay x * 1 msec

#define TRIM_MIN            256                             // minimum TRIM value
#define TRIM_MAX            2047                            // maximum TRIM value

#define PPM_1               10                              // 1.25 ppm (8M)
#define PPM_2               20                              // 2.5 ppm (8M)

#define PPM_BOUNDARY        80                              // 80 = 10 ppm (8M)

#define border_1            256                             // for linearization
#define border_2            700
#define border_3            1150
#define border_4            1600
#define border_5            2047

// variables
bool PulseError = false;                                    // Error_Flag when no pulses arriving. 

int ccc1_temp;                                              // temp ccc1

long ccc;
char Q;                                                     // quadrant number
char Clock_Read_count;
volatile int Fr[12];                                        // input TRIM value output measured counter value (appr 8M counts)
volatile int Trim[12];
volatile int C[12];
volatile long actual_trimming_value;

// *** routines ***
void delay(unsigned long dd);
void Setting_Trim(unsigned int Trim_Value);                 // program new Trim_Value
long Clock_Read(uint8_t port_number);                       // testing block wave via input e.g. P0_6 ... port can be selected   
int Overall_calculation(uint8_t port_number);
int linearization(int C, int Cmin, int Cmax, int Tmin, int Tmax);
int auto_trim(uint8_t port_number);

// end of variables

// *** delay routine x * 1 msec
void delay(unsigned long dd)
{
    unsigned long j, jj;
    jj = dd * DELAY_1MSEC;
    for (j = 1; j<=jj; j++)
    {
        __NOP();
        __NOP();
    }
}

// *** boundary testing new TRIM-value and storing in CLK_FREQ_TRIM_REG
void Setting_Trim(unsigned int Trim_Value)                  // program new Trim_Value
{
    if (Trim_Value < TRIM_MIN && Trim_Value != 0)   Trim_Value = TRIM_MIN;
    if (Trim_Value > TRIM_MAX)                      Trim_Value = TRIM_MAX;

    SetWord16(CLK_FREQ_TRIM_REG,Trim_Value);
    delay(2);                                               // delay = 2msec
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
long Clock_Read(uint8_t port_number)                        // testing block wave via input e.g. P0_6 ... port can be selected
{
    long ccc1 = 0;
    uint8_t port_number_10, port_number_1;                  // 10th and 1th e.g. 2 and 3 => port P2_3
    char shift_bit;

    uint32_t datareg;
    volatile uint32_t tick_counter = 0;
    int measure_pulse(int datareg, int shift_bit);          // forward declaration

    Clock_Read_count++;                                     // counting the loops

    port_number_10 = port_number / 10;
    port_number_1 = port_number % 10;

    shift_bit = (1 << port_number_1);

    switch (port_number_10)
    {
        case 0: datareg = P0_DATA_REG; break;
        case 1: datareg = P1_DATA_REG; break;
        case 2: datareg = P2_DATA_REG; break;
        case 3: datareg = P3_DATA_REG; break;

        default:
            return 0;
    }

    // configure systick timer
    SysTick->LOAD = 0xFFFFFF;
    SysTick->VAL = 0;
    SetBits32(&SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk, 1);

    // during counting, no interrupts should appear
    GLOBAL_INT_DISABLE();                                   // disable interrupts

    tick_counter = measure_pulse(datareg, shift_bit);
    SetBits32(&SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, 0);  // stop systick timer

    GLOBAL_INT_RESTORE();                                   // enable interrupts

    ccc1 = 0xFFFFFF - tick_counter;

    PulseError = false;                                     // {xx}
    if (ccc1 == 0xFFFFFF)
    {
        PulseError = true;
    }

    ccc1_temp = ccc1;                                       // store ccc1

    return ccc1;
}

int Overall_calculation(uint8_t port_number)
{
    int temp;
    char c1;

    c1 = 2;
    do
    {
        c1++;                                               // loop counter

        //int linearization (int C, int Cmin, int Cmax, int Tmin, int Tmax)
        temp = linearization (IDEAL_CNT, Fr[c1-1], Fr[1], Trim[c1-1], Trim[1]);

        if (temp < 256)   temp = 256;
        if (temp > 2047)  temp = 2047;

        Trim[c1] = temp;

        Setting_Trim(Trim[c1]);
        Fr[c1] = Clock_Read(port_number);                   // C1
        ccc = Fr[c1];
    }
    while((c1 < 10) && (Fr[c1] < (IDEAL_CNT - PPM_1)));

    Clock_Read_count = 0;                                   // counting the loops

    return temp;
}

// *** calculate new TRIM via linear line agorithm
int linearization(int C, int Cmin, int Cmax, int Tmin, int Tmax)
{
    int T;
    volatile float temp;

    // offset = zero C / C1 = T / T1
    // offset = non zero (C - C0) / (C1 - C0) = (T - T0) / (T1 - T0)
    temp =  (float)(C - Cmin)/(Cmax - Cmin);
    temp = temp * (Tmax - Tmin);
    T = (int) temp + Tmin;

    return T; 
}

// *** main function
int auto_trim(uint8_t port_number)
{
    char jj;

    // system init

    // Set port_number as an input with pull up resistor.
    switch (port_number)
    {
        case 00: SetBits16(P00_MODE_REG, PUPD, 1); break;
        case 01: SetBits16(P01_MODE_REG, PUPD, 1); break;
        case 02: SetBits16(P02_MODE_REG, PUPD, 1); break;
        case 03: SetBits16(P03_MODE_REG, PUPD, 1); break;
        case 04: SetBits16(P04_MODE_REG, PUPD, 1); break;
        case 05: SetBits16(P05_MODE_REG, PUPD, 1); break;
        case 06: SetBits16(P06_MODE_REG, PUPD, 1); break;
        case 07: SetBits16(P07_MODE_REG, PUPD, 1); break;
        case 10: SetBits16(P10_MODE_REG, PUPD, 1); break;
        case 11: SetBits16(P11_MODE_REG, PUPD, 1); break;
        case 12: SetBits16(P12_MODE_REG, PUPD, 1); break;
        case 13: SetBits16(P13_MODE_REG, PUPD, 1); break;
        case 14: SetBits16(P14_MODE_REG, PUPD, 1); break;
        case 15: SetBits16(P15_MODE_REG, PUPD, 1); break;
        case 20: SetBits16(P20_MODE_REG, PUPD, 1); break;
        case 21: SetBits16(P21_MODE_REG, PUPD, 1); break;
        case 22: SetBits16(P22_MODE_REG, PUPD, 1); break;
        case 23: SetBits16(P23_MODE_REG, PUPD, 1); break;
        case 24: SetBits16(P24_MODE_REG, PUPD, 1); break;
        case 25: SetBits16(P25_MODE_REG, PUPD, 1); break;
        case 26: SetBits16(P26_MODE_REG, PUPD, 1); break;
        case 27: SetBits16(P27_MODE_REG, PUPD, 1); break;
        case 28: SetBits16(P28_MODE_REG, PUPD, 1); break;
        case 29: SetBits16(P29_MODE_REG, PUPD, 1); break;
        case 30: SetBits16(P30_MODE_REG, PUPD, 1); break;
        case 31: SetBits16(P31_MODE_REG, PUPD, 1); break;
        case 32: SetBits16(P32_MODE_REG, PUPD, 1); break;
        case 33: SetBits16(P33_MODE_REG, PUPD, 1); break;
        case 34: SetBits16(P34_MODE_REG, PUPD, 1); break;
        case 35: SetBits16(P35_MODE_REG, PUPD, 1); break;
        case 36: SetBits16(P36_MODE_REG, PUPD, 1); break;
        case 37: SetBits16(P37_MODE_REG, PUPD, 1); break;
    }

    // init arrays
    for (jj = 0; jj <= 10; jj++)
    {
        Trim[jj] = 0;
        C[jj] = 0;
        Fr[jj] = 0;
    }

    // Linearisation:

    // 1e Clock_Read in the middle = 1150
    Trim[2] = border_3;                             // TRIM_min 1150
    Setting_Trim(Trim[2]);
    C[3] = Clock_Read(port_number);                 // C_min (1st measurement)

    if (PulseError == false)
    {
        if (C[3] > IDEAL_CNT)                       // e.g. > 1150
        {
            Trim[2] = border_2;                     // TRIM_min 700
            Trim[1] = border_3;                     // TRIM_max 1150
            Setting_Trim(Trim[2]);
            C[2] = Clock_Read(port_number);         // C_min (2nd measurement)
            Fr[1] = C[3];                           // C_max
            Fr[2] = C[2];                           // C_min
            Q = 2;

            if (C[2] > IDEAL_CNT)
            {
                Trim[2] = border_1;                 // TRIM_min 256
                Trim[1] = border_2;                 // TRIM_max 700
                Setting_Trim(Trim[2]);
                C[1] = Clock_Read(port_number);     // C_min (3rd measurement)
                Fr[1] = C[2];                       // C_max
                Fr[2] = C[1];                       // C_min
                Q = 1;
            }
        }
        else
        {
            Trim[2] = border_3;                     // TRIM_min 1150
            Trim[1] = border_4;                     // TRIM_max 1600
            Setting_Trim(Trim[1]);
            C[4] = Clock_Read(port_number);         // C_min (2nd measurement)
            Fr[1] = C[4];                           // C_max
            Fr[2] = C[3];                           // C_min
            Q = 3;

            if (C[4] < IDEAL_CNT)
            {
                Trim[2] = border_4;                 // TRIM_min 1600
                Trim[1] = border_5;                 // TRIM_max 2047
                Setting_Trim(Trim[1]);
                C[5] = Clock_Read(port_number);     // C_min (3rd measurement)
                Fr[1] = C[5];                       // C_max
                Fr[2] = C[4];                       // C_min
                Q = 4;
            }
        }
    }

    // check square pulse
    if (PulseError == true)                         // no square pulse detected
    {
        Setting_Trim(0);                            // if no square pulse was detected, then TRIM = 0
        return NO_PULSE_ERROR;                      // no square pulse detected
    }

    Overall_calculation(port_number);               // new calculation 20151201

    // check boundaries
    if (((ccc1_temp - IDEAL_CNT) > PPM_BOUNDARY) || ((IDEAL_CNT - ccc1_temp) > PPM_BOUNDARY))
    {
        Setting_Trim(0);                            // if square pulse was detected, but out of boundaries, then TRIM = 0
        return PULSE_OUT_OF_RANGE_ERROR;            // square pulse out of boundaries
    }

    actual_trimming_value = GetWord16(CLK_FREQ_TRIM_REG);   // read TRIM register

    return (actual_trimming_value);
}

