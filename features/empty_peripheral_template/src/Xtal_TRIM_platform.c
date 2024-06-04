/**
 ****************************************************************************************
 *
 * @file Xtal_TRIM_platform.c
 *
 * @brief Platform specifics for automated XTAL trim calculation algorithm.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "Xtal_TRIM_platform.h"
#include "Xtal_TRIM.h"

int measure_pulse(int datareg, int shift_bit);              // forward declaration


int xtal_pltfrm_get_port_pin_reg(uint8_t port_number, uint32_t *port_reg, uint16_t *pin_bit)
{
    uint8_t port_number_10, port_number_1;                  // 10th and 1th e.g. 2 and 3 => port P2_3
    uint16_t shift_bit;
    uint32_t datareg;

#if defined (__DA14531__)
    port_number_10 = (uint8_t)GPIO_PORT_0;
    port_number_1 = (uint8_t)port_number;
#else
    port_number_10 = (uint8_t)(port_number/10);
    port_number_1 = (uint8_t)(port_number%10);
#endif

    shift_bit = (1 << port_number_1);

    switch (port_number_10)
    {
        case 0: datareg = P0_DATA_REG; break;
#ifndef __DA14531__
        case 1: datareg = P1_DATA_REG; break;
        case 2: datareg = P2_DATA_REG; break;
        case 3: datareg = P3_DATA_REG; break;
#endif
        default:
            return INVALID_GPIO_ERROR;
    }
    
    *port_reg = datareg;
    *pin_bit = shift_bit;

    return XTAL_PLTFRM_NO_ERROR;
}

int xtal_pltfrm_set_port_in_pu(uint8_t port_number)
{
    switch (port_number)
    {
        case 0:  SetBits16(P00_MODE_REG, PUPD, 1);  break;
        case 1:  SetBits16(P01_MODE_REG, PUPD, 1);  break;
        case 2:  SetBits16(P02_MODE_REG, PUPD, 1);  break;
        case 3:  SetBits16(P03_MODE_REG, PUPD, 1);  break;
        case 4:  SetBits16(P04_MODE_REG, PUPD, 1);  break;
        case 5:  SetBits16(P05_MODE_REG, PUPD, 1);  break;
        case 6:  SetBits16(P06_MODE_REG, PUPD, 1);  break;
        case 7:  SetBits16(P07_MODE_REG, PUPD, 1);  break;
#ifdef __DA14531__
        case 8:  SetBits16(P08_MODE_REG, PUPD, 1);  break;
        case 9:  SetBits16(P09_MODE_REG, PUPD, 1);  break;
        case 10: SetBits16(P010_MODE_REG, PUPD, 1); break;
        case 11: SetBits16(P011_MODE_REG, PUPD, 1); break;
#else
        case 10: SetBits16(P10_MODE_REG, PUPD, 1);  break;
        case 11: SetBits16(P11_MODE_REG, PUPD, 1);  break;
        case 12: SetBits16(P12_MODE_REG, PUPD, 1);  break;
        case 13: SetBits16(P13_MODE_REG, PUPD, 1);  break;
        case 14: SetBits16(P14_MODE_REG, PUPD, 1);  break;
        case 15: SetBits16(P15_MODE_REG, PUPD, 1);  break;
        case 20: SetBits16(P20_MODE_REG, PUPD, 1);  break;
        case 21: SetBits16(P21_MODE_REG, PUPD, 1);  break;
        case 22: SetBits16(P22_MODE_REG, PUPD, 1);  break;
        case 23: SetBits16(P23_MODE_REG, PUPD, 1);  break;
        case 24: SetBits16(P24_MODE_REG, PUPD, 1);  break;
        case 25: SetBits16(P25_MODE_REG, PUPD, 1);  break;
        case 26: SetBits16(P26_MODE_REG, PUPD, 1);  break;
        case 27: SetBits16(P27_MODE_REG, PUPD, 1);  break;
        case 28: SetBits16(P28_MODE_REG, PUPD, 1);  break;
        case 29: SetBits16(P29_MODE_REG, PUPD, 1);  break;
        case 30: SetBits16(P30_MODE_REG, PUPD, 1);  break;
        case 31: SetBits16(P31_MODE_REG, PUPD, 1);  break;
        case 32: SetBits16(P32_MODE_REG, PUPD, 1);  break;
        case 33: SetBits16(P33_MODE_REG, PUPD, 1);  break;
        case 34: SetBits16(P34_MODE_REG, PUPD, 1);  break;
        case 35: SetBits16(P35_MODE_REG, PUPD, 1);  break;
        case 36: SetBits16(P36_MODE_REG, PUPD, 1);  break;
        case 37: SetBits16(P37_MODE_REG, PUPD, 1);  break;
#endif
        default: return INVALID_GPIO_ERROR;
    }

    return XTAL_PLTFRM_NO_ERROR;
}

int xtal_pltfrm_get_val_from_reg(volatile uint32_t *Trim_Value)
{
    *Trim_Value = (long)GetWord16(CLK_FREQ_TRIM_REG);   // read TRIM register

    return XTAL_PLTFRM_NO_ERROR;
}

int xtal_pltfrm_set_val_to_reg(uint32_t Trim_Value)
{
    SetWord16(CLK_FREQ_TRIM_REG, Trim_Value);

    return XTAL_PLTFRM_NO_ERROR;
}

uint32_t xtal_pltfrm_measure_pulse(uint32_t datareg, uint16_t shift_bit)
{
    return measure_pulse(datareg, shift_bit);
}
