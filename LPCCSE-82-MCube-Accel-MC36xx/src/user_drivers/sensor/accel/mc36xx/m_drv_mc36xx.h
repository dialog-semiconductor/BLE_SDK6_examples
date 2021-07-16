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
 * @file    m_drv_mc36xx.h
 * @author  mCube
 * @date    12 June 2018
 * @brief   Driver interface header for accelerometer mc36xx series.
 * @see     http://www.mcubemems.com
 */

#ifndef _M_DRV_MC36XX_H_
    #define _M_DRV_MC36XX_H_

/******************************************************************************
 *** INCLUDE FILES
 *****************************************************************************/
/** mCube functions include */
#include "m_drv_mc_utility.h"
#include "m_drv_interface.h"

/******************************************************************************
 *** CONSTANT / DEFINE
 *****************************************************************************/
/** Ambiq SDK using uint32_t for communication. Nordic SDK using uint8_t. */
#define uint_dev    uint8_t

#define m_drv_buffer(A)                                                       \
    union                                                                     \
    {                                                                         \
        uint32_t words[(A + 3) >> 2];                                         \
        uint8_t  bytes[A];                                                    \
    }

#define M_DRV_MC36XX_RETCODE_SUCCESS                 (0)
#define M_DRV_MC36XX_RETCODE_ERROR_BUS               (-1)
#define M_DRV_MC36XX_RETCODE_ERROR_NULL_POINTER      (-2)
#define M_DRV_MC36XX_RETCODE_ERROR_STATUS            (-3)
#define M_DRV_MC36XX_RETCODE_ERROR_SETUP             (-4)
#define M_DRV_MC36XX_RETCODE_ERROR_GET_DATA          (-5)
#define M_DRV_MC36XX_RETCODE_ERROR_IDENTIFICATION    (-6)
#define M_DRV_MC36XX_RETCODE_ERROR_NO_DATA           (-7)
#define M_DRV_MC36XX_RETCODE_ERROR_WRONG_ARGUMENT    (-8)

#define M_DRV_MC36XX_AXIS_X      0
#define M_DRV_MC36XX_AXIS_Y      1
#define M_DRV_MC36XX_AXIS_Z      2
#define M_DRV_MC36XX_AXES_NUM    3

#define M_DRV_MC36XX_FIFO_DEPTH    32
#define M_DRV_MC36XX_REG_MAP_SIZE  64

#define M_DRV_MC36XX_INTR_C_IPP_MODE_OPEN_DRAIN     (0x00)
#define M_DRV_MC36XX_INTR_C_IPP_MODE_PUSH_PULL      (0x01)

#define M_DRV_MC36XX_INTR_C_IAH_ACTIVE_LOW          (0x00)
#define M_DRV_MC36XX_INTR_C_IAH_ACTIVE_HIGH         (0x02)

#define M_DRV_MC36XX_NULL_ADDR    (0)

/******************************************************************************
 *** DATA TYPE / STRUCTURE DEFINITION / ENUM
 *****************************************************************************/
/** Sensor mode configuration. */
typedef enum
{
    E_M_DRV_MC36XX_MODE_SLEEP = 0,
    E_M_DRV_MC36XX_MODE_STANDBY,
    E_M_DRV_MC36XX_MODE_SNIFF,
    E_M_DRV_MC36XX_MODE_FSNIFF,
    E_M_DRV_MC36XX_MODE_PWAKE,
    E_M_DRV_MC36XX_MODE_CWAKE,
    E_M_DRV_MC36XX_MODE_SWAKE,
    E_M_DRV_MC36XX_MODE_TRIG,
    E_M_DRV_MC36XX_MODE_END
}   e_m_drv_mc36xx_mode_t;

/** Sensor measurement range configuration. */
typedef enum
{
    E_M_DRV_MC36XX_RANGE_2G = 0,
    E_M_DRV_MC36XX_RANGE_4G,
    E_M_DRV_MC36XX_RANGE_8G,
    E_M_DRV_MC36XX_RANGE_16G,
    E_M_DRV_MC36XX_RANGE_12G,
    E_M_DRV_MC36XX_RANGE_24G,
    E_M_DRV_MC36XX_RANGE_END
}   e_m_drv_mc36xx_range_t;

/** Sensor measurement resolution configuration. */
typedef enum
{
    E_M_DRV_MC36XX_RES_6BIT = 0,
    E_M_DRV_MC36XX_RES_7BIT,
    E_M_DRV_MC36XX_RES_8BIT,
    E_M_DRV_MC36XX_RES_10BIT,
    E_M_DRV_MC36XX_RES_12BIT,
    E_M_DRV_MC36XX_RES_14BIT,
    E_M_DRV_MC36XX_RES_END,
}   e_m_drv_mc36xx_res_t;

/** Sensor sniff power mode configuration. */
typedef enum
{
    E_M_DRV_MC36XX_SNIFF_SR_MODE_LOW_POWER = 0,
    E_M_DRV_MC36XX_SNIFF_SR_MODE_LOW_PRECISION,
    E_M_DRV_MC36XX_SNIFF_SR_MODE_PRECISION,
    E_M_DRV_MC36XX_SNIFF_SR_MODE_ULTRA_LOW_POWER,
    E_M_DRV_MC36XX_SNIFF_SR_MODE_HIGH_PRECISION,
    E_M_DRV_MC36XX_SNIFF_SR_MODE_END,
}   e_m_drv_mc36xx_sniff_sr_mode_t;

/** Sensor sniff mode sample rate configuration. */
typedef enum
{
    E_M_DRV_MC36XX_SNIFF_SR_DEFAULT_6Hz = 0,
    E_M_DRV_MC36XX_SNIFF_SR_0p4Hz,
    E_M_DRV_MC36XX_SNIFF_SR_0p8Hz,
    E_M_DRV_MC36XX_SNIFF_SR_2Hz,
    E_M_DRV_MC36XX_SNIFF_SR_6Hz,
    E_M_DRV_MC36XX_SNIFF_SR_13Hz,
    E_M_DRV_MC36XX_SNIFF_SR_26Hz,
    E_M_DRV_MC36XX_SNIFF_SR_50Hz,
    E_M_DRV_MC36XX_SNIFF_SR_100Hz,
    E_M_DRV_MC36XX_SNIFF_SR_200Hz,
    E_M_DRV_MC36XX_SNIFF_SR_400Hz,
    E_M_DRV_MC36XX_SNIFF_SR_END,
}   e_m_drv_mc36xx_sniff_sr_t;

/** Sensor wake power mode configuration. */
typedef enum
{
    E_M_DRV_MC36XX_WAKE_SR_MODE_LOW_POWER = 0,
    E_M_DRV_MC36XX_WAKE_SR_MODE_LOW_PRECISION,
    E_M_DRV_MC36XX_WAKE_SR_MODE_PRECISION,
    E_M_DRV_MC36XX_WAKE_SR_MODE_ULTRA_LOW_POWER,
    E_M_DRV_MC36XX_WAKE_SR_MODE_HIGH_PRECISION,
    E_M_DRV_MC36XX_WAKE_SR_MODE_END,
}   e_m_drv_mc36xx_wake_sr_mode_t;

/** Sensor wake mode sample rate configuration. */
typedef enum
{
    E_M_DRV_MC36XX_CWAKE_SR_LP_DUMMY_BASE = 0x00,
    E_M_DRV_MC36XX_CWAKE_SR_LP_0p4Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LP_0p8Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LP_1p7Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LP_7Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LP_14Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LP_27Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LP_54Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LP_106Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LP_210Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LP_411Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LP_606Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LP_DUMMY_END,

    E_M_DRV_MC36XX_CWAKE_SR_LOW_PR_DUMMY_BASE = 0x10,
    E_M_DRV_MC36XX_CWAKE_SR_LOW_PR_0p2Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LOW_PR_0p9Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LOW_PR_3p6Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LOW_PR_7p3Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LOW_PR_14Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LOW_PR_28Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LOW_PR_55Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LOW_PR_109Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LOW_PR_213Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LOW_PR_315Hz,
    E_M_DRV_MC36XX_CWAKE_SR_LOW_PR_DUMMY_END,

    E_M_DRV_MC36XX_CWAKE_SR_PR_DUMMY_BASE = 0x20,
    E_M_DRV_MC36XX_CWAKE_SR_PR_0p2Hz,
    E_M_DRV_MC36XX_CWAKE_SR_PR_0p4Hz,
    E_M_DRV_MC36XX_CWAKE_SR_PR_1p8Hz,
    E_M_DRV_MC36XX_CWAKE_SR_PR_3p7Hz,
    E_M_DRV_MC36XX_CWAKE_SR_PR_14Hz,
    E_M_DRV_MC36XX_CWAKE_SR_PR_28Hz,
    E_M_DRV_MC36XX_CWAKE_SR_PR_55Hz,
    E_M_DRV_MC36XX_CWAKE_SR_PR_109Hz,
    E_M_DRV_MC36XX_CWAKE_SR_PR_161Hz,
    E_M_DRV_MC36XX_CWAKE_SR_PR_DUMMY_END,

    E_M_DRV_MC36XX_CWAKE_SR_UL_DUMMY_BASE = 0x30,
    E_M_DRV_MC36XX_CWAKE_SR_UL_0p4Hz,
    E_M_DRV_MC36XX_CWAKE_SR_UL_0p8Hz,
    E_M_DRV_MC36XX_CWAKE_SR_UL_1p6Hz,
    E_M_DRV_MC36XX_CWAKE_SR_UL_6p5Hz,
    E_M_DRV_MC36XX_CWAKE_SR_UL_13Hz,
    E_M_DRV_MC36XX_CWAKE_SR_UL_26Hz,
    E_M_DRV_MC36XX_CWAKE_SR_UL_51Hz,
    E_M_DRV_MC36XX_CWAKE_SR_UL_100Hz,
    E_M_DRV_MC36XX_CWAKE_SR_UL_197Hz,
    E_M_DRV_MC36XX_CWAKE_SR_UL_389Hz,
    E_M_DRV_MC36XX_CWAKE_SR_UL_761Hz,
    E_M_DRV_MC36XX_CWAKE_SR_UL_1122Hz,
    E_M_DRV_MC36XX_CWAKE_SR_UL_DUMMY_END,

    E_M_DRV_MC36XX_CWAKE_SR_HIGH_PR_DUMMY_BASE = 0x40,
    E_M_DRV_MC36XX_CWAKE_SR_HIGH_PR_0p2Hz,
    E_M_DRV_MC36XX_CWAKE_SR_HIGH_PR_0p4Hz,
    E_M_DRV_MC36XX_CWAKE_SR_HIGH_PR_0p9Hz,
    E_M_DRV_MC36XX_CWAKE_SR_HIGH_PR_7p2Hz,
    E_M_DRV_MC36XX_CWAKE_SR_HIGH_PR_14Hz,
    E_M_DRV_MC36XX_CWAKE_SR_HIGH_PR_28Hz,
    E_M_DRV_MC36XX_CWAKE_SR_HIGH_PR_55Hz,
    E_M_DRV_MC36XX_CWAKE_SR_HIGH_PR_81Hz,
    E_M_DRV_MC36XX_CWAKE_SR_HIGH_PR_DUMMY_END,
}   e_m_drv_mc36xx_cwake_sr_t;

/** Sensor FIFO control configuration. */
typedef enum
{
    E_M_DRV_MC36XX_FIFO_CTL_DISABLE = 0,
    E_M_DRV_MC36XX_FIFO_CTL_ENABLE,
    E_M_DRV_MC36XX_FIFO_CTL_END,
}   e_m_drv_mc36xx_fifo_ctl_t;

/** Sensor FIFO mode configuration. */
typedef enum
{
    E_M_DRV_MC36XX_FIFO_MODE_NORMAL = 0,
    E_M_DRV_MC36XX_FIFO_MODE_WATERMARK,
    E_M_DRV_MC36XX_FIFO_MODE_END,
}   e_m_drv_mc36xx_fifo_mode_t;

/** Sensor wake mode gain configuration. */
typedef enum
{
    E_M_DRV_MC36XX_WAKE_GAIN_HIGH= 0,
    E_M_DRV_MC36XX_WAKE_GAIN_MED,
    E_M_DRV_MC36XX_WAKE_GAIN_LOW,
    E_M_DRV_MC36XX_WAKE_GAIN_END,
}   e_m_drv_mc36xx_wake_gain_t;

/** Sensor sniff mode gain configuration. */
typedef enum
{
    E_M_DRV_MC36XX_SNIFF_GAIN_HIGH = 0,
    E_M_DRV_MC36XX_SNIFF_GAIN_MED,
    E_M_DRV_MC36XX_SNIFF_GAIN_LOW,
    E_M_DRV_MC36XX_SNIFF_GAIN_END,
}   e_m_drv_mc36xx_sniff_gain_t;

/** Sensor sniff and/or mode configuration. */
typedef enum
{
    E_M_DRV_MC36XX_ANDORN_OR = 0,
    E_M_DRV_MC36XX_ANDORN_AND,
    E_M_DRV_MC36XX_ANDORN_END,
}   e_m_drv_mc36xx_andorn_t;

/** Sensor sniff data delta mode configuration. */
typedef enum
{
    //Compare to previous
    E_M_DRV_MC36XX_DELTA_MODE_C2P = 0,
    //Compare to baseline
    E_M_DRV_MC36XX_DELTA_MODE_C2B,
    E_M_DRV_MC36XX_DELTA_MODE_END,
}   e_m_drv_mc36xx_delta_mode_t;

/** Available registers in MC36XX sensor device */
typedef enum
{
    E_M_DRV_MC36XX_REG_EXT_STAT_1      = (0x00),
    E_M_DRV_MC36XX_REG_EXT_STAT_2      = (0x01),
    E_M_DRV_MC36XX_REG_XOUT_LSB        = (0x02),
    E_M_DRV_MC36XX_REG_XOUT_MSB        = (0x03),
    E_M_DRV_MC36XX_REG_YOUT_LSB        = (0x04),
    E_M_DRV_MC36XX_REG_YOUT_MSB        = (0x05),
    E_M_DRV_MC36XX_REG_ZOUT_LSB        = (0x06),
    E_M_DRV_MC36XX_REG_ZOUT_MSB        = (0x07),
    E_M_DRV_MC36XX_REG_STATUS_1        = (0x08),
    E_M_DRV_MC36XX_REG_STATUS_2        = (0x09),
    E_M_DRV_MC36XX_REG_FEATURE_C_1     = (0X0D),
    E_M_DRV_MC36XX_REG_FEATURE_C_2     = (0X0E),
    E_M_DRV_MC36XX_REG_PWR_CONTROL     = (0X0F),
    E_M_DRV_MC36XX_REG_MODE_C          = (0x10),
    E_M_DRV_MC36XX_REG_WAKE_C          = (0x11),
    E_M_DRV_MC36XX_REG_SNIFF_C         = (0x12),
    E_M_DRV_MC36XX_REG_SNIFFTH_C       = (0x13),
    E_M_DRV_MC36XX_REG_SNIFF_CFG       = (0x14),
    E_M_DRV_MC36XX_REG_RANGE_C         = (0x15),
    E_M_DRV_MC36XX_REG_FIFO_C          = (0x16),
    E_M_DRV_MC36XX_REG_INTR_C          = (0x17),
    E_M_DRV_MC36XX_REG_PROD            = (0x18),
    E_M_DRV_MC36XX_REG_PMCR            = (0x1C),
    E_M_DRV_MC36XX_REG_DMX             = (0x20),
    E_M_DRV_MC36XX_REG_DMY             = (0x21),
    E_M_DRV_MC36XX_REG_GAIN            = (0x21),
    E_M_DRV_MC36XX_REG_DMZ             = (0x22),

    E_M_DRV_MC36XX_REG_SEC             = (0x25),

    E_M_DRV_MC36XX_REG_XOFFL           = (0x2A),
    E_M_DRV_MC36XX_REG_XOFFH           = (0x2B),
    E_M_DRV_MC36XX_REG_YOFFL           = (0x2C),
    E_M_DRV_MC36XX_REG_YOFFH           = (0x2D),
    E_M_DRV_MC36XX_REG_ZOFFL           = (0x2E),
    E_M_DRV_MC36XX_REG_ZOFFH           = (0x2F),
    E_M_DRV_MC36XX_REG_XGAIN           = (0x30),
    E_M_DRV_MC36XX_REG_YGAIN           = (0x31),
    E_M_DRV_MC36XX_REG_ZGAIN           = (0x32),

    E_M_DRV_MC36XX_REG_XAOFSP          = (0x35),
    E_M_DRV_MC36XX_REG_XAOFSN          = (0x36),
    E_M_DRV_MC36XX_REG_YAOFSP          = (0x37),
    E_M_DRV_MC36XX_REG_YAOFSN          = (0x38),
    E_M_DRV_MC36XX_REG_ZAOFSP          = (0x39),
    E_M_DRV_MC36XX_REG_ZAOFSN          = (0x3A),

    E_M_DRV_MC36XX_REG_OPT             = (0x3B),
    E_M_DRV_MC36XX_REG_LOC_X           = (0x3C),
    E_M_DRV_MC36XX_REG_LOC_Y           = (0x3D),
    E_M_DRV_MC36XX_REG_LOT_dAOFSZ      = (0x3E),
    E_M_DRV_MC36XX_REG_WAF_LOT         = (0x3F),
    E_M_DRV_MC36XX_REG_END             = (0x40),
}   e_m_drv_mc36xx_reg_t;

/** Interrupt struct. */
typedef struct
{
    uint8_t    bWAKE;
    uint8_t    bACQ;
    uint8_t    bFIFO_EMPTY;
    uint8_t    bFIFO_FULL;
    uint8_t    bFIFO_THRESHOLD;
    uint8_t    bRESV;
    uint8_t    bSWAKE_SNIFF;
    uint8_t    baPadding[2];
}   s_m_drv_mc36xx_int_t;

/******************************************************************************
 *** EXTERNAL FUNCTION
 *****************************************************************************/
extern int  M_DRV_MC36XX_Init(void);
extern int  M_DRV_MC36XX_SetMode(e_m_drv_mc36xx_mode_t eNextMode);
extern int  M_DRV_MC36XX_SPIHSMode(void);
extern int  M_DRV_MC36XX_ConfigRegRngResCtrl(                               \
                                e_m_drv_mc36xx_range_t eCfgRange,           \
                                e_m_drv_mc36xx_res_t eCfgResolution);
extern int  M_DRV_MC36XX_SetSampleRate(e_m_drv_mc36xx_cwake_sr_t eCwakeSR,  \
                                       e_m_drv_mc36xx_sniff_sr_t eSniffSR);
extern int  M_DRV_MC36XX_SetGain(e_m_drv_mc36xx_wake_gain_t eWakeGain,      \
                                 e_m_drv_mc36xx_sniff_gain_t eSniffGain);
extern int  M_DRV_MC36XX_SetSniffThreshold(uint8_t axis, uint8_t sniff_thr);
extern int  M_DRV_MC36XX_SetSniffDetectCount(uint8_t axis, uint8_t SniffCount);
extern int  M_DRV_MC36XX_SetSniffAndOrN(e_m_drv_mc36xx_andorn_t LogicAndOr);
extern int  M_DRV_MC36XX_SetSniffDeltaMode(e_m_drv_mc36xx_delta_mode_t Mode);
extern int  M_DRV_MC36XX_EnableFIFO(e_m_drv_mc36xx_fifo_ctl_t eCtrl,        \
                                    e_m_drv_mc36xx_fifo_mode_t eMode,       \
                                    uint8_t bThreshold);

extern int  M_DRV_MC36XX_ReadData(                                          \
            float faOutput[M_DRV_MC36XX_FIFO_DEPTH][M_DRV_MC36XX_AXES_NUM], \
            int nNumOfSample);
extern int  M_DRV_MC36XX_ReadRawData(
            short Output[M_DRV_MC36XX_FIFO_DEPTH][M_DRV_MC36XX_AXES_NUM],   \
            int nNumOfSample);

extern uint8_t  M_DRV_MC36XX_ReadReg(uint8_t bRegAddr);
extern int  M_DRV_MC36XX_ReadRegMap(                                        \
            uint8_t baRegMap[M_DRV_MC36XX_REG_MAP_SIZE]);

extern int  M_DRV_MC36XX_ConfigINT(uint8_t bFifoThreshEnable,               \
                                   uint8_t bFifoFullEnable,                 \
                                   uint8_t bFifoEmptyEnable,                \
                                   uint8_t bACQEnable,                      \
                                   uint8_t bWakeEnable);
extern int  M_DRV_MC36XX_HandleINT(s_m_drv_mc36xx_int_t *ptINT_Event);
unsigned long  M_DRV_MC36XX_GetVersion(void);
#endif    // END of _M_DRV_MC36XX_H_

