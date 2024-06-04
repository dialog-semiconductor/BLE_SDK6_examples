/**
 ****************************************************************************************
 *
 * @file rdtest_api.c
 *
 * @brief RD test API source code file.
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

#include <stdint.h>
#include "rdtest_API.h"

#if !defined (__DA14531__)
void rdtest_initialize(uint8_t version)
{
    rdtest_init_ports(version);
    rdtest_vppcontrol(0);
    rdtest_uart_connect(0);
    rdtest_make_loopback(0);
    rdtest_vbatcontrol(0);
    rdtest_pulse_to_uart(0);
    rdtest_select_pulsewidth(0);
}

void rdtest_vppcontrol(uint8_t state)
{
    rdtest_updateregister(VPP_CTRL_ADDRESS, state&0x01);
    rdtest_enable();
}

void rdtest_select_pulsewidth(uint8_t length)
{
    rdtest_updateregister(PW_CTRL_ADDRESS, length);
    rdtest_enable();
}

void rdtest_uart_connect(uint16_t connectmap16)
{
    rdtest_updateregister(UART_CONNECT_ADDRESS+0, (uint8_t) ((connectmap16>>0)&0xFF));
    rdtest_updateregister(UART_CONNECT_ADDRESS+1, (uint8_t) ((connectmap16>>8)&0xFF));
    rdtest_enable();
}

void rdtest_make_loopback(uint8_t port)
{
    uint16_t map = 0x0000;
    if (port!=0) 
    {
        map = (0x01 << (port-1));
    }
    rdtest_updateregister(LOOPBACK_CTRL_ADDRESS+0, (uint8_t) ((map>>0)&0xFF));
    rdtest_updateregister(LOOPBACK_CTRL_ADDRESS+1, (uint8_t) ((map>>8)&0xFF));
    rdtest_enable();
}

void rdtest_vbatcontrol(uint16_t connectmap16)
{
    rdtest_updateregister(VBAT_CTRL_ADDRESS+0, (uint8_t) ((connectmap16>>0)&0xFF));
    rdtest_updateregister(VBAT_CTRL_ADDRESS+1, (uint8_t) ((connectmap16>>8)&0xFF));
    rdtest_enable();
}

void rdtest_resetpulse(uint16_t delayms)
{
    rdtest_ll_reset(0);
    rdtest_ll_reset(1);
    delay_systick_us(1000*delayms);
    rdtest_ll_reset(0);
}

void rdtest_pulse_to_uart(uint16_t connectmap16)
{
    rdtest_updateregister(GATE_TO_UART_ADDRESS+0, (uint8_t) ((connectmap16>>0)&0xFF));
    rdtest_updateregister(GATE_TO_UART_ADDRESS+1, (uint8_t) ((connectmap16>>8)&0xFF));
    rdtest_enable();
}

void rdtest_generate_pulse(void)
{
    rdtest_updateregister(STARTGATE_ADDRESS, 0);
    rdtest_enable();
    rdtest_updateregister(STARTGATE_ADDRESS, 1);
    rdtest_enable();
    rdtest_updateregister(STARTGATE_ADDRESS, 0);
    rdtest_enable();
}
#else
//DO NOTHING IN 531
#endif //!defined (__DA14531__)

