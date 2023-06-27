/******************************************************************************
 *
 * Copyright (c) 2018 mCube, Inc.  All rights reserved.
 *
 * This source is subject to the mCube Software License.
 * This software is protected by Copyright and the information and source code
 * contained herein is confidential. The software including the source code
 * may not be copied and the information contained herein may not be used or
 * disclosed except with the written permission of mCube Inc.
 *
 * All other rights reserved.
 *****************************************************************************/

/**
 * @file    m_drv_mc36xx.c
 * @author  mCube
 * @date    12 June 2018
 * @brief   Driver interface for accelerometer mc36xx series.
 * @see     http://www.mcubemems.com
 */

/******************************************************************************
 *** INFORMATION
 *****************************************************************************/
#define M_DRV_MC36XX_VERSION_AlGOTITHM_MAJOR                 0
#define M_DRV_MC36XX_VERSION_AlGOTITHM_MINOR                 0
#define M_DRV_MC36XX_VERSION_AlGOTITHM_BUILD                 0

#define M_DRV_MC36XX_VERSION_INTERFACE_MAJOR                 3
#define M_DRV_MC36XX_VERSION_INTERFACE_MINOR                 1
#define M_DRV_MC36XX_VERSION_INTERFACE_BUILD                 0

/******************************************************************************
 *** INCLUDE FILES
 *****************************************************************************/
/** mCube functions include */
#include "m_drv_mc36xx.h"

/******************************************************************************
 *** CONFIGURATION
 *****************************************************************************/
/** !!! DO NOT use both I2C and SPI at the same time !!! */
#define M_DRV_MC36XX_CFG_BUS_I2C
//#define M_DRV_MC36XX_CFG_BUS_SPI

#define M_DRV_MC36XX_CFG_I2C_ADDR    (0x4C)
//#define M_DRV_MC36XX_CFG_I2C_ADDR    (0x6C)

//#define M_DRV_MC36XX_OPERATE_MODE_WAKE_WHEN_READ
//#define M_DRV_MC36XX_SUPPORT_LPF

//#define DEBUG_MODE

#define M_DRV_MC36XX_CFG_SAMPLE_RATE_CWAKE_DEFAULT                          \
            E_M_DRV_MC36XX_CWAKE_SR_LP_54Hz
#define M_DRV_MC36XX_CFG_SAMPLE_RATE_SNIFF_DEFAULT                          \
            E_M_DRV_MC36XX_SNIFF_SR_6Hz
#define M_DRV_MC36XX_CFG_RANGE                                              \
            E_M_DRV_MC36XX_RANGE_4G
#define M_DRV_MC36XX_CFG_RESOLUTION                                         \
            E_M_DRV_MC36XX_RES_12BIT
#define M_DRV_MC36XX_CFG_ORIENTATION_MAP                                    \
            E_M_DRV_UTIL_ORIENTATION_TOP_RIGHT_UP
#define M_DRV_MC36XX_CFG_WAKE_GAIN_DEFAULT                                  \
            E_M_DRV_MC36XX_WAKE_GAIN_LOW
#define M_DRV_MC36XX_CFG_SNIFF_GAIN_DEFAULT                                 \
            E_M_DRV_MC36XX_SNIFF_GAIN_HIGH
#define M_DRV_MC36XX_CFG_INT_MODE_DEFAULT                                   \
            M_DRV_MC36XX_INTR_C_IAH_ACTIVE_LOW                              \
          | M_DRV_MC36XX_INTR_C_IPP_MODE_PUSH_PULL

#if (!defined (M_DRV_MC36XX_CFG_BUS_SPI) && !defined (M_DRV_MC36XX_CFG_BUS_I2C))
#error "MUST use one bus to access register!"
#endif

#if (defined (M_DRV_MC36XX_CFG_BUS_SPI) && defined (M_DRV_MC36XX_CFG_BUS_I2C))
#error "DO NOT use both SPI and I2C simultaneously!"
#endif

#ifdef M_DRV_MC36XX_SUPPORT_LPF
/** (Hz) Sampling frequency */
#define SAMPLE_RATE         50
/** Sampling interval */
#define SAMPLE_INTERVAL     1/SAMPLE_RATE
/** (Hz) Cut-off frequency */
#define CUT_OFF_FREQUENCY   4
#define PI                  3.14
/** LPF coefficient */
#define ALPHA \
        SAMPLE_INTERVAL/(1/((2*PI*CUT_OFF_FREQUENCY)+SAMPLE_INTERVAL))
#endif

/******************************************************************************
 *** MACRO
 *****************************************************************************/
#ifdef M_DRV_MC36XX_CFG_BUS_I2C
#define _M_DRV_MC36XX_REG_WRITE(bRegAddr, pbDataBuf, bLength)               \
        mcube_write_regs(1, M_DRV_MC36XX_CFG_I2C_ADDR,                      \
                         bRegAddr, pbDataBuf, bLength)
#define _M_DRV_MC36XX_REG_READ(bRegAddr, pbDataBuf, bLength)                \
        mcube_read_regs(1, M_DRV_MC36XX_CFG_I2C_ADDR,                       \
                        bRegAddr, pbDataBuf, bLength)
#else
#define _M_DRV_MC36XX_REG_WRITE(bRegAddr, pbDataBuf, bLength)               \
        mcube_write_regs(0, 0, bRegAddr, pbDataBuf, bLength)
#define _M_DRV_MC36XX_REG_READ(bRegAddr, pbDataBuf, bLength)                \
        mcube_read_regs(0, 0, bRegAddr, pbDataBuf, bLength)
#endif

#define _M_DRV_MC36XX_REG_STATUS_1_MODE(bRegStatus1)                        \
        (bRegStatus1 & 0x07)
#define _M_DRV_MC36XX_REG_STATUS_1_NEW_DATA(bRegStatus1)                    \
        (bRegStatus1 & 0x08)
#define _M_DRV_MC36XX_REG_STATUS_1_FIFO_EMPTY(bRegStatus1)                  \
        (bRegStatus1 & 0x10)
#define _M_DRV_MC36XX_REG_STATUS_1_FIFO_FULL(bRegStatus1)                   \
        (bRegStatus1 & 0x20)
#define _M_DRV_MC36XX_REG_STATUS_1_FIFO_THRESH(bRegStatus1)                 \
        (bRegStatus1 & 0x40)
#define _M_DRV_MC36XX_REG_STATUS_1_INT_PEND(bRegStatus1)                    \
        (bRegStatus1 & 0x80)

#define _M_DRV_MC36XX_REG_STATUS_2_INT_WAKE(bRegStatus2)                    \
        ((bRegStatus2 >> 2) & 0x01)
#define _M_DRV_MC36XX_REG_STATUS_2_INT_ACQ(bRegStatus2)                     \
        ((bRegStatus2 >> 3) & 0x01)
#define _M_DRV_MC36XX_REG_STATUS_2_INT_FIFO_EMPTY(bRegStatus2)              \
        ((bRegStatus2 >> 4) & 0x01)
#define _M_DRV_MC36XX_REG_STATUS_2_INT_FIFO_FULL(bRegStatus2)               \
        ((bRegStatus2 >> 5) & 0x01)
#define _M_DRV_MC36XX_REG_STATUS_2_INT_FIFO_THRESH(bRegStatus2)             \
        ((bRegStatus2 >> 6) & 0x01)
#define _M_DRV_MC36XX_REG_STATUS_2_INT_SWAKE_SNIFF(bRegStatus2)             \
        ((bRegStatus2 >> 7) & 0x01)

#define _M_DRV_MC36XX_REG_MODE_C_MODE(bRegMODE_C)                           \
        (bRegMODE_C & 0x07)
#define _M_DRV_MC36XX_REG_RANGE_C_RES(bRegRANGE_C)                          \
        (bRegRANGE_C & 0x07)
#define _M_DRV_MC36XX_REG_RANGE_C_RANGE(bRegRANGE_C)                        \
        ((bRegRANGE_C >> 4) & 0x07)

#define _M_DRV_MC36XX_REG_FIFO_C_FIFO_EN(bRegFIFO_C)                        \
        (bRegFIFO_C & 0x40)
#define _M_DRV_MC36XX_FIFO_VDD_EN(bRegPwrCtrl)                              \
        (bRegPwrCtrl | 0x42);

#ifdef M_DRV_MC36XX_SUPPORT_LPF
#define _M_DRV_MC36XX_SENSOR_FILTER(last_data, curr_data)                   \
        (( ALPHA * (last_data)) + (( 1 - ALPHA) * (curr_data)))
#endif

#ifdef DEBUG_MODE
extern void mcube_printf(const char *pui8Fmt, ...);
#define M_PRINTF(...)       do {                                            \
                            mcube_printf(__VA_ARGS__);                      \
                            } while (0);
#else
#define M_PRINTF(...)
#endif

/******************************************************************************
 *** STATIC VARIABLE
 *****************************************************************************/
/** unit: SI/LSB, SI: m/s^2 */
static float s_fMC36XX_Sensitivity = 0.0f;

static e_m_drv_mc36xx_range_t s_eRange                                      \
            = M_DRV_MC36XX_CFG_RANGE;
static e_m_drv_mc36xx_res_t s_eRes                                          \
            = M_DRV_MC36XX_CFG_RESOLUTION;
static e_m_drv_mc36xx_cwake_sr_t s_eSR_CWAKE                                \
            = M_DRV_MC36XX_CFG_SAMPLE_RATE_CWAKE_DEFAULT;
static e_m_drv_mc36xx_sniff_sr_t s_eSR_SNIFF                                \
            = M_DRV_MC36XX_CFG_SAMPLE_RATE_SNIFF_DEFAULT;
static e_m_drv_mc36xx_wake_gain_t s_eGAIN_WAKE                              \
            = M_DRV_MC36XX_CFG_WAKE_GAIN_DEFAULT;
static e_m_drv_mc36xx_sniff_gain_t s_eGAIN_SNIFF                            \
            =  M_DRV_MC36XX_CFG_SNIFF_GAIN_DEFAULT;

static uint_dev s_bCfgRngResol      = 0x00;
static uint_dev s_bCfgSniffThr      = 0x00;
static uint_dev s_bCfgSniffCfg      = 0x00;
static uint_dev s_bCfgFifo          = 0x00;
static uint_dev s_bCfgINT           = 0x00;
static uint_dev s_bCfgFifoVdd       = 0x42;
static uint_dev s_bCfgWakeSRMode    = 0xFF;
static uint_dev s_bCfgSniffSRMode   = 0xFF;
static uint_dev s_bCfgHSMode        = 0x00;

static int g_bIsMC3610 = 0;

#ifdef M_DRV_MC36XX_SUPPORT_LPF
static short _saLPFPrevData[M_DRV_MC36XX_AXES_NUM]={0};
#endif

/******************************************************************************
 *** STATIC FUNCTION
 *****************************************************************************/
/******************************************************************************
 *** _M_DRV_MC36XX_Delay
 *****************************************************************************/
void _M_DRV_MC36XX_Delay(uint32_t dwMs) {
    mcube_delay_ms(dwMs);
}

/******************************************************************************
 *** _M_DRV_MC36XX_SetBusIF
 *****************************************************************************/
static int _M_DRV_MC36XX_SetBusIF(void)
{
    M_PRINTF("[%s]\r\n", __func__);

    /*Orion I2C/SPI interface Setup */
    uint_dev _bRegIO_C = 0;
    uint_dev _bRegSNIFFTH_C = 0;

    if(g_bIsMC3610 == 0) {
         M_PRINTF("Sensor MC36XX.\r\n");
        _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_FEATURE_C_1, &_bRegIO_C, 1);
#ifdef M_DRV_MC36XX_CFG_BUS_I2C
        _bRegIO_C &= 0x3F;
        _bRegIO_C |= 0x40;
#else
        _bRegIO_C &= 0x3F;
        _bRegIO_C |= 0x80;
#endif
        _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_FEATURE_C_1, &_bRegIO_C, 1);
    } else {
        M_PRINTF("Sensor MC3610.\r\n");
        _M_DRV_MC36XX_REG_READ(0x13, &_bRegSNIFFTH_C, 1);
        _M_DRV_MC36XX_REG_READ(0x14, &_bRegIO_C     , 1);
#ifdef M_DRV_MC36XX_CFG_BUS_I2C
        _bRegSNIFFTH_C |= 0x80;
        _bRegIO_C      &= 0x7F;
#else
        _bRegSNIFFTH_C &= 0x7F;
        _bRegIO_C      |= 0x80;
#endif
        _M_DRV_MC36XX_REG_WRITE(0x13, &_bRegSNIFFTH_C, 1);
        _M_DRV_MC36XX_REG_WRITE(0x14, &_bRegIO_C     , 1);
    }

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** _M_DRV_MC36XX_SetSniffAGAIN
 *****************************************************************************/
static int _M_DRV_MC36XX_SetSniffAGAIN(uint8_t SniffGain)
{
    M_PRINTF("[%s] SniffGain= 0x%02X\r\n", __func__, SniffGain);

    uint_dev _bRegAGain = 0x00;

    if(SniffGain > E_M_DRV_MC36XX_SNIFF_GAIN_END) {
        M_PRINTF("[%s] SniffGain > %d\r\n", __func__, SniffGain);
        return (M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT);
    }
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_DMX, &_bRegAGain, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_DMX, _bRegAGain);

    _bRegAGain = 0x00;
    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_DMY, &_bRegAGain, 1);
    _bRegAGain &= 0x3F;
    _bRegAGain |= (SniffGain << 6 );

    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_DMY, &_bRegAGain, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_DMY, _bRegAGain);

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** _M_DRV_MC36XX_SetWakeAGAIN
 *****************************************************************************/
static int _M_DRV_MC36XX_SetWakeAGAIN(uint8_t WakeGain)
{
    M_PRINTF("[%s] WakeGain= 0x%02X\r\n", __func__, WakeGain);

    uint_dev _bRegAGain = 0x01;

    if(WakeGain > E_M_DRV_MC36XX_WAKE_GAIN_END) {
        M_PRINTF("[%s] WakeGain > %d\r\n", __func__, WakeGain);
        return (M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT);
    }

    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_DMX, &_bRegAGain, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_DMX, _bRegAGain);

    _bRegAGain = 0x00;
    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_DMY, &_bRegAGain, 1);

    _bRegAGain &= 0x3F;
    _bRegAGain |= (WakeGain << 6);
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_DMY, &_bRegAGain, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_DMY, _bRegAGain);

    _bRegAGain = 0x00;
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_DMX, &_bRegAGain, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_DMX, _bRegAGain);

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** _M_DRV_MC36XX_ResetChip
 *****************************************************************************/
static void _M_DRV_MC36XX_ResetChip(void)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bRegData = 0x01;

    _M_DRV_MC36XX_REG_WRITE(0x10, &_bRegData, 1);

    _M_DRV_MC36XX_Delay(10);

    _bRegData = 0x40;
    _M_DRV_MC36XX_REG_WRITE(0x24, &_bRegData, 1);

    _M_DRV_MC36XX_Delay(50);

    /** dummy write */
    _bRegData = 0x00;
    _M_DRV_MC36XX_REG_WRITE(0x09, &_bRegData, 1);
    _bRegData = 0x42;
    _M_DRV_MC36XX_REG_WRITE(0x0F, &_bRegData, 1);
    _bRegData = 0x01;
    _M_DRV_MC36XX_REG_WRITE(0x20, &_bRegData, 1);
    _bRegData = 0x80;
    _M_DRV_MC36XX_REG_WRITE(0x21, &_bRegData, 1);
    _bRegData = 0x00;
    _M_DRV_MC36XX_REG_WRITE(0x28, &_bRegData, 1);
    _bRegData = 0x00;
    _M_DRV_MC36XX_REG_WRITE(0x1a, &_bRegData, 1);

    _bRegData = 0x01;
    _M_DRV_MC36XX_REG_WRITE(0x10, &_bRegData, 1);

    _M_DRV_MC36XX_Delay(10);
}

/******************************************************************************
 *** _M_DRV_MC36XX_ValidateSensorIC
 *****************************************************************************/
static int _M_DRV_MC36XX_ValidateSensorIC(void)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bRegData = 0;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_PROD, &_bRegData, 1);
    M_PRINTF("%s: 0x18[0x%x]\r\n", (uint32_t)__func__, _bRegData);

    _bRegData&=0xff;

    if (0x71 == _bRegData) {
        _M_DRV_MC36XX_REG_READ(0x19, &_bRegData, 1);
        if (_bRegData < 3) {
            M_PRINTF("[%s] Orion. \r\n", __func__);
            return (M_DRV_MC36XX_RETCODE_SUCCESS);
        }
    }
    else if (0x70 == _bRegData) {
        _M_DRV_MC36XX_REG_READ(0x34, &_bRegData, 1);
        if (0x30 == _bRegData) {
            _M_DRV_MC36XX_REG_READ(0x3B, &_bRegData, 1);
            if (0x10 == (_bRegData & 0xF3)) {
                g_bIsMC3610 = 1;
                M_PRINTF("[%s] Orion mc3610. \r\n", __func__);
                return (M_DRV_MC36XX_RETCODE_SUCCESS);
            }
        }
    }

    return (M_DRV_MC36XX_RETCODE_ERROR_IDENTIFICATION);
}

#ifdef M_DRV_MC36XX_SUPPORT_LPF
/******************************************************************************
 *** _M_DRV_MC36XX_LowPassFilter
 *****************************************************************************/
static void _M_DRV_MC36XX_LowPassFilter                                 \
            (signed short _saData[M_DRV_MC36XX_AXES_NUM])
{
    M_PRINTF("[%s]\r\n", __func__);
    M_PRINTF("[CurrData]     %d    %d    %d",                           \
             _saData[M_DRV_MC36XX_AXIS_X],                              \
             _saData[M_DRV_MC36XX_AXIS_Y],                              \
             _saData[M_DRV_MC36XX_AXIS_Z]);

    M_PRINTF("[PreData]     %d    %d    %d",                            \
             _saLPFPrevData[M_DRV_MC36XX_AXIS_X],                       \
             _saLPFPrevData[M_DRV_MC36XX_AXIS_Y],                       \
             _saLPFPrevData[M_DRV_MC36XX_AXIS_Z]);

    _saData[M_DRV_MC36XX_AXIS_X] =                                      \
    _M_DRV_MC36XX_SENSOR_FILTER(_saLPFPrevData[M_DRV_MC36XX_AXIS_X],    \
                                _saData[M_DRV_MC36XX_AXIS_X]);
    _saData[M_DRV_MC36XX_AXIS_Y] =                                      \
    _M_DRV_MC36XX_SENSOR_FILTER(_saLPFPrevData[M_DRV_MC36XX_AXIS_Y],    \
                                _saData[M_DRV_MC36XX_AXIS_Y]);
    _saData[M_DRV_MC36XX_AXIS_Z] =                                      \
    _M_DRV_MC36XX_SENSOR_FILTER(_saLPFPrevData[M_DRV_MC36XX_AXIS_Z],    \
                                _saData[M_DRV_MC36XX_AXIS_Z]);

    _saLPFPrevData[M_DRV_MC36XX_AXIS_X] = _saData[M_DRV_MC36XX_AXIS_X];
    _saLPFPrevData[M_DRV_MC36XX_AXIS_Y] = _saData[M_DRV_MC36XX_AXIS_Y];
    _saLPFPrevData[M_DRV_MC36XX_AXIS_Z] = _saData[M_DRV_MC36XX_AXIS_Z];

    M_PRINTF("[M_DRV_MC36XX_SUPPORT_LPF]     %d    %d    %d",           \
             _saData[M_DRV_MC36XX_AXIS_X],                              \
             _saData[M_DRV_MC36XX_AXIS_Y],                              \
             _saData[M_DRV_MC36XX_AXIS_Z]);
}
#endif    // #ifdef M_DRV_MC36XX_SUPPORT_LPF

/******************************************************************************
 *** _M_DRV_MC36XX_SetSniffOverSR
 *****************************************************************************/
static void _M_DRV_MC36XX_SetSniffOverSR(uint8_t bDesiredSRMode)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bRegData = 0x00;

    if (s_bCfgSniffSRMode == bDesiredSRMode)
        return;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_PMCR, &_bRegData, 1);
    _bRegData &= 0x8F;
    _bRegData |= (s_bCfgHSMode | (bDesiredSRMode<< 4));
    s_bCfgHSMode = _bRegData;

    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_PMCR, &_bRegData, 1);
    M_PRINTF("[%s] REG[0x1C] 0x%02X\r\n", __func__, _bRegData );
    s_bCfgSniffSRMode = bDesiredSRMode;
}

/******************************************************************************
 *** _M_DRV_MC36XX_SetWakeOverSR
 *****************************************************************************/
static void _M_DRV_MC36XX_SetWakeOverSR(uint8_t bDesiredSRMode)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bRegData = 0x00;

    if (s_bCfgWakeSRMode == bDesiredSRMode)
        return;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_PMCR, &_bRegData, 1);
    _bRegData &= 0xF8;
    _bRegData |= (s_bCfgHSMode | bDesiredSRMode);
    s_bCfgHSMode = _bRegData;

    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_PMCR, &_bRegData, 1);
    M_PRINTF("[%s] REG[0x1C] 0x%02X\r\n", __func__, _bRegData );
    s_bCfgWakeSRMode = bDesiredSRMode;
}

/******************************************************************************
 *** _M_DRV_MC36XX_SetCwakeSR
 *****************************************************************************/
static int _M_DRV_MC36XX_SetCwakeSR(e_m_drv_mc36xx_cwake_sr_t eSR)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bRegMODE = ((eSR & 0x70) >> 4);
    uint_dev _bRegWAKE = (eSR & 0x0f);

    _M_DRV_MC36XX_SetWakeOverSR(_bRegMODE);
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_WAKE_C, &_bRegWAKE, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_WAKE_C, _bRegWAKE );

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** _M_DRV_MC36XX_CheckCwakeSR
 *****************************************************************************/
static int _M_DRV_MC36XX_CheckCwakeSR(e_m_drv_mc36xx_cwake_sr_t eSR)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bRegMODE = ((eSR & 0x70) >> 4);

    if (_bRegMODE > E_M_DRV_MC36XX_WAKE_SR_MODE_HIGH_PRECISION)
        return (M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT);

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** _M_DRV_MC36XX_SetSniffSR
 *****************************************************************************/
static int _M_DRV_MC36XX_SetSniffSR(e_m_drv_mc36xx_sniff_sr_t eSR)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bRegWAKE = 0x00;
    uint_dev _bRegSNIFF = 0xC0;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_WAKE_C, &_bRegWAKE, 1);
    _M_DRV_MC36XX_SetSniffOverSR(E_M_DRV_MC36XX_SNIFF_SR_MODE_LOW_POWER);

    _bRegSNIFF |= eSR;
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_SNIFF_C, &_bRegSNIFF, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_SNIFF_C, _bRegSNIFF );

    _bRegWAKE  &= 0x7F;
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_WAKE_C, &_bRegWAKE, 1);
     M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                      \
              E_M_DRV_MC36XX_REG_WAKE_C, _bRegWAKE );

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** _M_DRV_MC36XX_SetMode
 *****************************************************************************/
static int _M_DRV_MC36XX_SetMode(e_m_drv_mc36xx_mode_t eNextMode)
{
    uint_dev _bCurrMode = 0;
    uint_dev _bRegMODE_C = 0;
    uint_dev _bGuard = 0;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_STATUS_1, &_bCurrMode, 1);

    if (eNextMode == _M_DRV_MC36XX_REG_STATUS_1_MODE(_bCurrMode))
        return (M_DRV_MC36XX_RETCODE_ERROR_STATUS);

    switch((int)eNextMode)
    {
    case E_M_DRV_MC36XX_MODE_SNIFF:
        _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_PWR_CONTROL,             \
                                &s_bCfgFifoVdd, 1);
        M_PRINTF("[%s] MODE_SNIFF\r\n", __func__);
        break;
    case E_M_DRV_MC36XX_MODE_SLEEP:
        _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_PWR_CONTROL,             \
                                &s_bCfgFifoVdd, 1);
        M_PRINTF("[%s] MODE_SLEEP\r\n", __func__);
        break;
    case E_M_DRV_MC36XX_MODE_STANDBY:
        M_PRINTF("[%s] MODE_STANDBY\r\n", __func__);
        break;
    default:
        M_PRINTF("[%s] MODE WAKE\r\n", __func__);
        if (E_M_DRV_MC36XX_MODE_CWAKE == eNextMode) {
            _M_DRV_MC36XX_SetCwakeSR(s_eSR_CWAKE);
        }
        break;
    }

    _bRegMODE_C |= eNextMode;

    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_MODE_C, &_bRegMODE_C, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_MODE_C, _bRegMODE_C);

    while (1)
    {
        _bGuard++;
        _M_DRV_MC36XX_Delay(1);
        _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_STATUS_1, &_bCurrMode, 1);
        if (eNextMode == _M_DRV_MC36XX_REG_STATUS_1_MODE(_bCurrMode)) {
            M_PRINTF("SET MODE SUCCESS!!\r\n");
            M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,               \
                     E_M_DRV_MC36XX_REG_MODE_C, _bCurrMode&0xFF);
            break;
        }
        if (_bGuard > 100) {
            M_PRINTF("M_DRV_MC36XX_RETCODE_ERROR_SETUP\r\n");
            return (M_DRV_MC36XX_RETCODE_ERROR_SETUP);
        }
    }

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** _M_DRV_MC36XX_ReadRawData
 *****************************************************************************/
static int _M_DRV_MC36XX_ReadRawData(signed short Output[M_DRV_MC36XX_AXES_NUM])
{
    M_PRINTF("[%s]\r\n", __func__);

    signed short _waRaw[M_DRV_MC36XX_AXES_NUM] = { 0 };
    m_drv_buffer(180) _baData = {0};
    const S_M_DRV_MC_UTIL_OrientationReMap *_ptOrienMap                     \
            = &g_MDrvUtilOrientationReMap[M_DRV_MC36XX_CFG_ORIENTATION_MAP];

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_XOUT_LSB,                     \
                           (uint_dev *)_baData.words ,6);

    _waRaw[M_DRV_MC36XX_AXIS_X] = ((signed short) ((_baData.bytes[0])       \
                                                 | (_baData.bytes[1] << 8)));
    _waRaw[M_DRV_MC36XX_AXIS_Y] = ((signed short) ((_baData.bytes[2])       \
                                                 | (_baData.bytes[3] << 8)));
    _waRaw[M_DRV_MC36XX_AXIS_Z] = ((signed short) ((_baData.bytes[4])       \
                                                 | (_baData.bytes[5] << 8)));

#ifdef M_DRV_MC36XX_SUPPORT_LPF
    _M_DRV_MC36XX_LowPassFilter(_waRaw);
#endif

    Output[M_DRV_MC36XX_AXIS_X] =
            ((_ptOrienMap->bSign[M_DRV_MC36XX_AXIS_X]                       \
            * _waRaw[_ptOrienMap->bMap[M_DRV_MC36XX_AXIS_X]]));
    Output[M_DRV_MC36XX_AXIS_Y] =
            ((_ptOrienMap->bSign[M_DRV_MC36XX_AXIS_Y]                       \
            * _waRaw[_ptOrienMap->bMap[M_DRV_MC36XX_AXIS_Y]]));
    Output[M_DRV_MC36XX_AXIS_Z] =
            ((_ptOrienMap->bSign[M_DRV_MC36XX_AXIS_Z]                       \
            * _waRaw[_ptOrienMap->bMap[M_DRV_MC36XX_AXIS_Z]]));

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** _M_DRV_MC36XX_ReadData
 *****************************************************************************/
static int _M_DRV_MC36XX_ReadData(float faOutput[M_DRV_MC36XX_AXES_NUM])
{
    M_PRINTF("[%s]\r\n", __func__);

    signed short _waRaw[M_DRV_MC36XX_AXES_NUM] = { 0 };

    _M_DRV_MC36XX_ReadRawData(_waRaw);

    faOutput[M_DRV_MC36XX_AXIS_X] = (float) ((_waRaw[M_DRV_MC36XX_AXIS_X]   \
                                   * s_fMC36XX_Sensitivity));
    faOutput[M_DRV_MC36XX_AXIS_Y] = (float) ((_waRaw[M_DRV_MC36XX_AXIS_Y]   \
                                   * s_fMC36XX_Sensitivity));
    faOutput[M_DRV_MC36XX_AXIS_Z] = (float) ((_waRaw[M_DRV_MC36XX_AXIS_Z]   \
                                   * s_fMC36XX_Sensitivity));
    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** FUNCTION
 *****************************************************************************/
/******************************************************************************
 *** M_DRV_MC36XX_Init
 *****************************************************************************/
int M_DRV_MC36XX_Init(void)
{
    M_PRINTF("[%s]\r\n", __func__);

    if (M_DRV_MC36XX_RETCODE_SUCCESS != _M_DRV_MC36XX_ValidateSensorIC())
        return (M_DRV_MC36XX_RETCODE_ERROR_IDENTIFICATION);

    _M_DRV_MC36XX_ResetChip();

    /** Config Bus Type either SPI or I2C */
    _M_DRV_MC36XX_SetBusIF();

    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_PWR_CONTROL, &s_bCfgFifoVdd, 1);

    /** Config Range and Resolution */
    M_DRV_MC36XX_ConfigRegRngResCtrl(s_eRange, s_eRes);

    /** Config Sniff and CWake Sample Rate */
    _M_DRV_MC36XX_SetSniffSR(s_eSR_SNIFF);
    _M_DRV_MC36XX_SetCwakeSR(s_eSR_CWAKE);

    /* Config Sniff and CWake Analog Gain */
    _M_DRV_MC36XX_SetWakeAGAIN(s_eGAIN_WAKE);
    _M_DRV_MC36XX_SetSniffAGAIN(s_eGAIN_SNIFF);

#ifdef M_DRV_MC36XX_OPERATE_MODE_WAKE_WHEN_READ
    _M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_SLEEP);
#endif

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_FIFO_C, &s_bCfgFifo, 1);
    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_INTR_C, &s_bCfgINT, 1);

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC36XX_SetMode
 *****************************************************************************/
int M_DRV_MC36XX_SetMode(e_m_drv_mc36xx_mode_t eNextMode)
{
    M_PRINTF("[%s]\r\n", __func__);

    if (!(E_M_DRV_MC36XX_MODE_STANDBY ^ eNextMode))
        _M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_STANDBY);
    _M_DRV_MC36XX_SetMode(eNextMode);

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

#ifdef M_DRV_MC36XX_CFG_BUS_SPI
/******************************************************************************
 *** M_DRV_MC36XX_SPIHSMode
 *****************************************************************************/
int M_DRV_MC36XX_SPIHSMode(void)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bPreMode = 0;
    uint_dev _bRegHS = 0;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_MODE_C, &_bPreMode, 1);
    _M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_STANDBY);

    _bRegHS = 0;
    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_PMCR, &_bRegHS, 1);
    _bRegHS |= 0x80;
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_PMCR, &_bRegHS, 1);
    s_bCfgHSMode = _bRegHS;

    _M_DRV_MC36XX_SetMode((e_m_drv_mc36xx_mode_t)                             \
                          _M_DRV_MC36XX_REG_MODE_C_MODE(_bPreMode));

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}
#endif

/******************************************************************************
 *** M_DRV_MC36XX_ConfigRegRngResCtrl
 *****************************************************************************/
int M_DRV_MC36XX_ConfigRegRngResCtrl                                          \
                            (e_m_drv_mc36xx_range_t eCfgRange,                \
                             e_m_drv_mc36xx_res_t eCfgResolution)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bPreMode = 0;
    float _faRange[E_M_DRV_MC36XX_RANGE_END]                                \
            = { 19.614f, 39.228f, 78.456f, 156.912f, 117.684f, 235.368f };
    float _faResolution[E_M_DRV_MC36XX_RES_END]                             \
            = { 32.0f, 64.0f, 128.0f, 512.0f, 2048.0f, 8192.0f };

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_MODE_C, &_bPreMode, 1);
    _M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_STANDBY);

    if (E_M_DRV_MC36XX_RES_12BIT < eCfgResolution) {
        s_bCfgFifo = 0x80;
        _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_FIFO_C, &s_bCfgFifo, 1);
        s_bCfgFifo = 0x00;
        _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_FIFO_C, &s_bCfgFifo, 1);
    }

    s_bCfgRngResol = (((eCfgRange << 4) & 0x70) | eCfgResolution);
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_RANGE_C, &s_bCfgRngResol, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_RANGE_C, s_bCfgRngResol );

    s_fMC36XX_Sensitivity = (_faRange[eCfgRange]/_faResolution[eCfgResolution]);
    M_PRINTF("[%s] s_fMC36XX_Sensitivity=%f\r\n", __func__,                 \
             s_fMC36XX_Sensitivity);
    _M_DRV_MC36XX_SetMode((e_m_drv_mc36xx_mode_t)                           \
                          _M_DRV_MC36XX_REG_MODE_C_MODE(_bPreMode));
    s_eRange = eCfgRange;
    s_eRes = eCfgResolution;

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/*****************************************
 *** M_DRV_MC36XX_SetSampleRate
 *****************************************/
int M_DRV_MC36XX_SetSampleRate(e_m_drv_mc36xx_cwake_sr_t eCwakeSR,          \
                               e_m_drv_mc36xx_sniff_sr_t eSniffSR)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bPreMode = 0;

    if (M_DRV_MC36XX_RETCODE_SUCCESS != _M_DRV_MC36XX_CheckCwakeSR(eCwakeSR))
        return (M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT);

    s_eSR_CWAKE = eCwakeSR;
    s_eSR_SNIFF = eSniffSR;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_MODE_C, &_bPreMode, 1);
    _M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_STANDBY);

    _M_DRV_MC36XX_SetSniffSR(s_eSR_SNIFF);
    _M_DRV_MC36XX_SetCwakeSR(s_eSR_CWAKE);

    _M_DRV_MC36XX_SetMode((e_m_drv_mc36xx_mode_t)                           \
                          _M_DRV_MC36XX_REG_MODE_C_MODE(_bPreMode));

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC36XX_SetGain
 *****************************************************************************/
int M_DRV_MC36XX_SetGain(e_m_drv_mc36xx_wake_gain_t eWakeGain,              \
                         e_m_drv_mc36xx_sniff_gain_t eSniffGain)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bPreMode = 0x00;

    s_eGAIN_WAKE = eWakeGain;
    s_eGAIN_SNIFF = eSniffGain;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_MODE_C, &_bPreMode, 1);
    _M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_STANDBY);

    _M_DRV_MC36XX_SetWakeAGAIN(s_eGAIN_WAKE);
    _M_DRV_MC36XX_SetSniffAGAIN(s_eGAIN_SNIFF);

    _M_DRV_MC36XX_SetMode((e_m_drv_mc36xx_mode_t)                           \
                           _M_DRV_MC36XX_REG_MODE_C_MODE(_bPreMode));

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC36XX_SetSniffThreshold
 *****************************************************************************/
int M_DRV_MC36XX_SetSniffThreshold(uint8_t axis, uint8_t sniff_thr)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bRegSniff_addr = 0;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_SNIFFTH_C, &s_bCfgSniffThr, 1);

    if (axis < M_DRV_MC36XX_AXES_NUM)
        _bRegSniff_addr = axis + 1;
    else
        return (M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT);

    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_SNIFF_CFG, &_bRegSniff_addr, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_SNIFF_CFG, _bRegSniff_addr);

    s_bCfgSniffThr |= sniff_thr;
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_SNIFFTH_C, &s_bCfgSniffThr, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_SNIFFTH_C, s_bCfgSniffThr);

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC36XX_SetSniffDetectCount
 *****************************************************************************/
int M_DRV_MC36XX_SetSniffDetectCount(uint8_t axis, uint8_t SniffCount)
{
    M_PRINTF("[%s]\r\n", __func__);

    /** unsigned SNIFF event count, 1 to 62 events,
        independent from other channels */
    uint_dev _bRegSniff_Count = SniffCount;
    uint_dev _bRegSniff_Axis = 0;
    uint_dev _bRegSniff_Count_En = 0x08;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_SNIFFTH_C, &s_bCfgSniffThr, 1);
    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_SNIFF_CFG, &s_bCfgSniffCfg, 1);

    if (axis < M_DRV_MC36XX_AXES_NUM)
        _bRegSniff_Axis = axis + 5;
    else
        return (M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT);

    s_bCfgSniffCfg |= _bRegSniff_Axis;
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_SNIFF_CFG, &s_bCfgSniffCfg, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_SNIFF_CFG, s_bCfgSniffCfg);

    /** Set detection count as (count +1) */
    s_bCfgSniffThr |= _bRegSniff_Count;
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_SNIFFTH_C, &s_bCfgSniffThr, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__,                       \
             E_M_DRV_MC36XX_REG_SNIFFTH_C, s_bCfgSniffThr);

    /** Enable SNIFF detection counts, required for valid SNIFF wake-up */
    s_bCfgSniffCfg |= _bRegSniff_Count_En;
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_SNIFF_CFG, &s_bCfgSniffCfg, 1);
    M_PRINTF("0x%x : s_bCfgSniffCfg:%x\r\n",                                \
             E_M_DRV_MC36XX_REG_SNIFF_CFG, s_bCfgSniffCfg);
    M_PRINTF("0x%x : s_bCfgSniffThr:%x\r\n",                                \
             E_M_DRV_MC36XX_REG_SNIFFTH_C, s_bCfgSniffThr);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X", __func__,                           \
             E_M_DRV_MC36XX_REG_SNIFF_CFG, s_bCfgSniffCfg);

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC36XX_SetSniffAndOrN
 *****************************************************************************/
int M_DRV_MC36XX_SetSniffAndOrN(e_m_drv_mc36xx_andorn_t LogicAndOr)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bRegAndOrN = 0x00;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_SNIFFTH_C, &_bRegAndOrN, 1);

    if(LogicAndOr == 1)
        _bRegAndOrN |= (LogicAndOr << 6);
    else if (LogicAndOr == 0)
        _bRegAndOrN &= 0xBF;
    else
        return (M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT);

    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_SNIFFTH_C, &_bRegAndOrN, 1);
    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC36XX_SetSniffAndOrN
 *****************************************************************************/
int M_DRV_MC36XX_SetSniffDeltaMode(e_m_drv_mc36xx_delta_mode_t Mode)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bRegDeltaMode = 0x00;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_SNIFFTH_C, &_bRegDeltaMode, 1);

    /** mode(0) is C2P, mode(1) is C2B*/
    if(Mode == 1)
        _bRegDeltaMode |= 0x80;
    else if (Mode == 0)
        _bRegDeltaMode &= 0x7F;
    else
        return (M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT);

    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_SNIFFTH_C, &_bRegDeltaMode, 1);
    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC36XX_EnableFIFO
 *****************************************************************************/
int M_DRV_MC36XX_EnableFIFO(e_m_drv_mc36xx_fifo_ctl_t eCtrl,                \
                            e_m_drv_mc36xx_fifo_mode_t eMode,               \
                            uint8_t bThreshold)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bPreMode = 0;

    if (eCtrl >= E_M_DRV_MC36XX_FIFO_CTL_END)
        return (M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT);

    if (eMode >= E_M_DRV_MC36XX_FIFO_MODE_END)
        return (M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT);

    if (bThreshold > (M_DRV_MC36XX_FIFO_DEPTH-1))
        bThreshold = (M_DRV_MC36XX_FIFO_DEPTH-1);

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_MODE_C, &_bPreMode, 1);
    _M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_STANDBY);

    uint_dev _bRegRANGE_C = 0;
    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_RANGE_C, &_bRegRANGE_C, 1);

    if (E_M_DRV_MC36XX_RES_12BIT < _M_DRV_MC36XX_REG_RANGE_C_RES(_bRegRANGE_C))
        M_DRV_MC36XX_ConfigRegRngResCtrl((e_m_drv_mc36xx_range_t)           \
                            _M_DRV_MC36XX_REG_RANGE_C_RANGE(_bRegRANGE_C),  \
                            E_M_DRV_MC36XX_RES_12BIT);

    s_bCfgFifo = ((eCtrl << 6) | (eMode << 5) | (bThreshold));
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_FIFO_C, &s_bCfgFifo, 1);
    _M_DRV_MC36XX_SetMode((e_m_drv_mc36xx_mode_t)                           \
                           _M_DRV_MC36XX_REG_MODE_C_MODE(_bPreMode));

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC36XX_ReadData
 *****************************************************************************/
int M_DRV_MC36XX_ReadData(                                                  \
    float faOutput[M_DRV_MC36XX_FIFO_DEPTH][M_DRV_MC36XX_AXES_NUM],         \
    int nNumOfSample)
{
    int _nDataCount = 0;
    uint_dev _bRegStatus1 = 0;
    uint_dev _bRegFIFO_C = 0;

    if ((M_DRV_MC36XX_NULL_ADDR == faOutput) || (0 == nNumOfSample))
        return (M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT);

#ifdef M_DRV_MC36XX_OPERATE_MODE_WAKE_WHEN_READ
    _M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_STANDBY);
    _M_DRV_MC36XX_SetCwakeSR(s_eSR_CWAKE);
    _M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_CWAKE);
#endif

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_STATUS_1, &_bRegStatus1, 1);
    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_FIFO_C, &_bRegFIFO_C, 1);

    /** FIFO Mode */
    if (_M_DRV_MC36XX_REG_FIFO_C_FIFO_EN(_bRegFIFO_C)) {
        if (M_DRV_MC36XX_FIFO_DEPTH < nNumOfSample)
            nNumOfSample = M_DRV_MC36XX_FIFO_DEPTH;

        M_PRINTF("[%s] FIFO mode read data\r\n", __func__);

        if (_M_DRV_MC36XX_REG_STATUS_1_FIFO_EMPTY(_bRegStatus1))
            return (M_DRV_MC36XX_RETCODE_ERROR_NO_DATA);

        for (_nDataCount = 0; _nDataCount < nNumOfSample; _nDataCount++) {
            _M_DRV_MC36XX_ReadData(faOutput[_nDataCount]);
            _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_STATUS_1,             \
                                   &_bRegStatus1, 1);

            if (_M_DRV_MC36XX_REG_STATUS_1_FIFO_EMPTY(_bRegStatus1)) {
                _nDataCount++;
                break;
            }
        }
    }
    /** Normal Mode */
    else
    {
        M_PRINTF("[%s] Normal mode read data", __func__);
        /**
         * 0: No new sample data has arrived since last read.
         * 1: New sample data has arrived and has been written to FIFO/registers
         */
        if (!_M_DRV_MC36XX_REG_STATUS_1_NEW_DATA(_bRegStatus1))
            return (M_DRV_MC36XX_RETCODE_ERROR_NO_DATA);

        _M_DRV_MC36XX_ReadData(faOutput[0]);
        _nDataCount = 1;
    }

    return (_nDataCount);
}

/******************************************************************************
 *** M_DRV_MC36XX_ReadRawData
 *****************************************************************************/
int M_DRV_MC36XX_ReadRawData(                                               \
    short Output[M_DRV_MC36XX_FIFO_DEPTH][M_DRV_MC36XX_AXES_NUM],           \
    int nNumOfSample)
{

    int _nDataCount = 0;
    uint_dev _bRegStatus1 = 0;
    uint_dev _bRegFIFO_C = 0;

    if ((M_DRV_MC36XX_NULL_ADDR == Output) || (0 == nNumOfSample))
        return (M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT);

#ifdef M_DRV_MC36XX_OPERATE_MODE_WAKE_WHEN_READ
    _M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_STANDBY);
    _M_DRV_MC36XX_SetCwakeSR(s_eSR_CWAKE);
    _M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_CWAKE);
#endif

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_STATUS_1, &_bRegStatus1, 1);
    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_FIFO_C, &_bRegFIFO_C, 1);

    /** FIFO Mode */
    if (_M_DRV_MC36XX_REG_FIFO_C_FIFO_EN(_bRegFIFO_C)) {
        if (M_DRV_MC36XX_FIFO_DEPTH < nNumOfSample)
            nNumOfSample = M_DRV_MC36XX_FIFO_DEPTH;

        M_PRINTF("[%s] FIFO mode read data\r\n", __func__);

        if (_M_DRV_MC36XX_REG_STATUS_1_FIFO_EMPTY(_bRegStatus1))
            return (M_DRV_MC36XX_RETCODE_ERROR_NO_DATA);

        for (_nDataCount = 0; _nDataCount < nNumOfSample; _nDataCount++) {
            _M_DRV_MC36XX_ReadRawData(Output[_nDataCount]);
            _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_STATUS_1,             \
                                   &_bRegStatus1, 1);

            if (_M_DRV_MC36XX_REG_STATUS_1_FIFO_EMPTY(_bRegStatus1)) {
                _nDataCount++;
                break;
            }
        }
    }
    /** Normal Mode */
    else
    {
        M_PRINTF("[%s] Normal mode read data", __func__);
        /**
         * 0: No new sample data has arrived since last read.
         * 1: New sample data has arrived and has been written to FIFO/registers
         */
        if (!_M_DRV_MC36XX_REG_STATUS_1_NEW_DATA(_bRegStatus1))
            return (M_DRV_MC36XX_RETCODE_ERROR_NO_DATA);

        _M_DRV_MC36XX_ReadRawData(Output[0]);
        _nDataCount = 1;
    }

    return (_nDataCount);
}

/******************************************************************************
 *** M_DRV_MC36XX_ReadReg
 *****************************************************************************/
uint8_t M_DRV_MC36XX_ReadReg(uint8_t bRegAddr)
{
    uint_dev _bData = 0;

    _M_DRV_MC36XX_REG_READ(bRegAddr, &_bData, 1);

    M_PRINTF("[%s] REG[0x%02X] 0x%02X\r\n", __func__, bRegAddr, _bData );
    return (_bData);
}

/******************************************************************************
 *** M_DRV_MC36XX_ReadRegMap
 *****************************************************************************/
int M_DRV_MC36XX_ReadRegMap(uint8_t baRegMap[M_DRV_MC36XX_REG_MAP_SIZE])
{
    uint8_t _bIndex = 0;
    uint_dev _bRegData = 0;

    for (_bIndex = 0; _bIndex < M_DRV_MC36XX_REG_MAP_SIZE; _bIndex++) {
        _M_DRV_MC36XX_REG_READ(_bIndex, &_bRegData, 1);
        M_PRINTF("REG[0x%02X] 0x%02X\r\n", _bIndex, _bRegData);

        if (0 != baRegMap)
            baRegMap[_bIndex] = _bRegData;
    }
    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC36XX_ConfigINT
 *****************************************************************************/
int M_DRV_MC36XX_ConfigINT(uint8_t bFifoThreshEnable,                       \
                           uint8_t bFifoFullEnable,                         \
                           uint8_t bFifoEmptyEnable,                        \
                           uint8_t bACQEnable,                              \
                           uint8_t bWakeEnable)
{
    M_PRINTF("[%s]\r\n", __func__);

    uint_dev _bPreMode = 0;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_MODE_C, &_bPreMode, 1);
    _M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_STANDBY);

    s_bCfgINT = (((bFifoThreshEnable & 0x01) << 6)
              | ((bFifoFullEnable & 0x01) << 5)
              | ((bFifoEmptyEnable & 0x01) << 4)
              | ((bACQEnable & 0x01) << 3)
              | ((bWakeEnable & 0x01) << 2)
              | M_DRV_MC36XX_CFG_INT_MODE_DEFAULT);

    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_INTR_C, &s_bCfgINT, 1);
    _M_DRV_MC36XX_SetMode((e_m_drv_mc36xx_mode_t)                             \
                           _M_DRV_MC36XX_REG_MODE_C_MODE(_bPreMode));

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC36XX_HandleINT
 *****************************************************************************/
int M_DRV_MC36XX_HandleINT(s_m_drv_mc36xx_int_t *ptINT_Event)
{
    uint_dev _bRegStatus2 = 0;

    _M_DRV_MC36XX_REG_READ(E_M_DRV_MC36XX_REG_STATUS_2, &_bRegStatus2, 1);

    ptINT_Event->bWAKE =                                                    \
        _M_DRV_MC36XX_REG_STATUS_2_INT_WAKE(_bRegStatus2);
    ptINT_Event->bACQ =                                                     \
        _M_DRV_MC36XX_REG_STATUS_2_INT_ACQ(_bRegStatus2);
    ptINT_Event->bFIFO_EMPTY =                                              \
        _M_DRV_MC36XX_REG_STATUS_2_INT_FIFO_EMPTY(_bRegStatus2);
    ptINT_Event->bFIFO_FULL =                                               \
        _M_DRV_MC36XX_REG_STATUS_2_INT_FIFO_FULL(_bRegStatus2);
    ptINT_Event->bFIFO_THRESHOLD =                                          \
        _M_DRV_MC36XX_REG_STATUS_2_INT_FIFO_THRESH(_bRegStatus2);
    ptINT_Event->bSWAKE_SNIFF =                                             \
        _M_DRV_MC36XX_REG_STATUS_2_INT_SWAKE_SNIFF(_bRegStatus2);

/** clear interrupt flag */
#ifdef    M_DRV_MC36XX_CFG_BUS_SPI
    _M_DRV_MC36XX_REG_WRITE(E_M_DRV_MC36XX_REG_STATUS_2, &_bRegStatus2, 1);
#endif

    return (M_DRV_MC36XX_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC36XX_GetVersion
 *****************************************************************************/
unsigned long M_DRV_MC36XX_GetVersion(void)
{
    return((((unsigned long)M_DRV_MC36XX_VERSION_AlGOTITHM_MAJOR) << 28) |
           (((unsigned long)M_DRV_MC36XX_VERSION_AlGOTITHM_MINOR) << 24) |
           (((unsigned long)M_DRV_MC36XX_VERSION_AlGOTITHM_BUILD) << 20) |
           (((unsigned long)M_DRV_MC36XX_VERSION_INTERFACE_MAJOR) << 12) |
           (((unsigned long)M_DRV_MC36XX_VERSION_INTERFACE_MINOR) << 8) |
           (((unsigned long)M_DRV_MC36XX_VERSION_INTERFACE_BUILD) << 4));
}
