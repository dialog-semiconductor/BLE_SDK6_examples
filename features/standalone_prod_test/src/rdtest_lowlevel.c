/**
 ****************************************************************************************
 *
 * @file rdtest_lowlevel.c
 *
 * @brief RD test low level functions source code file.
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
 
/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rdtest_lowlevel.h"
#include "gpio.h"

#if !defined (__DA14531__)
void rdtest_init_ports(uint8_t version)
{
    // data
    SetWord16(P3_DATA_REG, 0);
    SetWord16(P30_MODE_REG, 0x300);
    SetWord16(P31_MODE_REG, 0x300);
    SetWord16(P32_MODE_REG, 0x300);
    SetWord16(P33_MODE_REG, 0x300);
    SetWord16(P34_MODE_REG, 0x300);
    SetWord16(P35_MODE_REG, 0x300);
    SetWord16(P36_MODE_REG, 0x300);
    SetWord16(P37_MODE_REG, 0x300);

    SetWord16(P2_DATA_REG, 0); 
    // address
    SetWord16(P20_MODE_REG, 0x300);
    SetWord16(P21_MODE_REG, 0x300);
    SetWord16(P22_MODE_REG, 0x300);
    SetWord16(P23_MODE_REG, 0x300);
    // latch
    SetWord16(P24_MODE_REG, 0x300);
    // enable
    SetWord16(P25_MODE_REG, 0x300);
}

void rdtest_ll_write_data(uint8_t data)
{
    SetWord16(P3_DATA_REG, ((data<<DATASHIFT)&DATAMASK));
}

void rdtest_ll_write_address(uint8_t address)
{
    uint16_t now_p2;
    now_p2 = GetWord16(P2_DATA_REG);
    SetWord16(P2_DATA_REG, (now_p2&~ADDRESSMASK) | ((address<<ADDRESSSHIFT)&ADDRESSMASK));
}

void rdtest_ll_write_latch(uint8_t latch)
{
    uint16_t now_p2;
    now_p2 = GetWord16(P2_DATA_REG);
    SetWord16(P2_DATA_REG, (now_p2&~LATCHMASK) | ((latch<<LATCHSHIFT)&LATCHMASK));
}

void rdtest_ll_write_enable(uint8_t enable)
{
    uint16_t now_p2;
    now_p2 = GetWord16(P2_DATA_REG);
    SetWord16(P2_DATA_REG, (now_p2&~ENABLEMASK) | ((enable<<ENABLESHIFT)&ENABLEMASK));
}

void rdtest_ll_reset(uint8_t reset)
{
    rdtest_updateregister(RESET_CTRL_REG, (uint8_t) reset);
    rdtest_enable();
}

void rdtest_updateregister(uint8_t address, uint8_t data)
{
    rdtest_ll_write_latch(0);
    rdtest_ll_write_data(data);
    rdtest_ll_write_address(address);
    rdtest_ll_write_latch(1);
    rdtest_ll_write_latch(0);
}

void rdtest_enable (void)
{
    rdtest_ll_write_enable(0);
    rdtest_ll_write_enable(1);
    rdtest_ll_write_enable(0);
}
#else
//DO NOTHING IN 531
#endif //!defined (__DA14531__)

