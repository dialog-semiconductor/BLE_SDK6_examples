/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : lis2dh_driver.h
* Author             : MSH Application Team
* Author             : Abhishek Anand, Fabio Tota
* Version            : $Revision:$
* Date               : $Date:$
* Description        : Descriptor Header for lis2dh_driver.c driver file
*
* HISTORY:
* Date        | Modification                                | Author
* 24/06/2011  | Initial Revision                            | Fabio Tota
* 07/06/2012  | Support for multiple drivers in the same program | Abhishek Anand
*
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOFTWARE IS SPECIFICALLY DESIGNED FOR EXCLUSIVE USE WITH ST PARTS.
*
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LIS2DH_DRIVER__H
#define __LIS2DH_DRIVER__H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/


//these could change accordingly with the architecture

#ifndef __ARCHDEP__TYPES
#define __ARCHDEP__TYPES

typedef unsigned char u8_t;
typedef unsigned short int u16_t;
typedef short int i16_t;
typedef signed char i8_t;

#endif /*__ARCHDEP__TYPES*/

typedef u8_t LIS2DH_IntPinConf_t;
typedef u8_t LIS2DH_Axis_t;
typedef u8_t LIS2DH_Int1Conf_t;


//define structure
#ifndef __SHARED__TYPES
#define __SHARED__TYPES

typedef enum {
  MEMS_SUCCESS				=		0x01,
  MEMS_ERROR				=		0x00	
} status_t;

typedef enum {
  MEMS_ENABLE				=		0x01,
  MEMS_DISABLE				=		0x00	
} State_t;

typedef struct {
  i16_t AXIS_X;
  i16_t AXIS_Y;
  i16_t AXIS_Z;
} AxesRaw_t;




#endif /*__SHARED__TYPES*/

typedef enum {  
  LIS2DH_ODR_1Hz		        =		0x01,		
  LIS2DH_ODR_10Hz                      =		0x02,
  LIS2DH_ODR_25Hz		        =		0x03,
  LIS2DH_ODR_50Hz		        =		0x04,
  LIS2DH_ODR_100Hz		        =		0x05,	
  LIS2DH_ODR_200Hz		        =		0x06,
  LIS2DH_ODR_400Hz		        =		0x07,
  LIS2DH_ODR_1620Hz_LP		        =		0x08,
  LIS2DH_ODR_1344Hz_NP_5367HZ_LP       =		0x09	
} LIS2DH_ODR_t;

typedef enum {
  LIS2DH_POWER_DOWN                    =		0x00,
  LIS2DH_LOW_POWER 			=		0x01,
  LIS2DH_NORMAL			=		0x02
} LIS2DH_Mode_t;

typedef enum {
  LIS2DH_HPM_NORMAL_MODE_RES           =               0x00,
  LIS2DH_HPM_REF_SIGNAL                =               0x01,
  LIS2DH_HPM_NORMAL_MODE               =               0x02,
  LIS2DH_HPM_AUTORESET_INT             =               0x03
} LIS2DH_HPFMode_t;

typedef enum {
  LIS2DH_HPFCF_0                       =               0x00,
  LIS2DH_HPFCF_1                       =               0x01,
  LIS2DH_HPFCF_2                       = 		0x02,
  LIS2DH_HPFCF_3                       =               0x03
} LIS2DH_HPFCutOffFreq_t;

typedef struct {
  u16_t AUX_1;
  u16_t AUX_2;
  u16_t AUX_3;
} LIS2DH_Aux123Raw_t;

typedef enum {
  LIS2DH_FULLSCALE_2                   =               0x00,
  LIS2DH_FULLSCALE_4                   =               0x01,
  LIS2DH_FULLSCALE_8                   =               0x02,
  LIS2DH_FULLSCALE_16                  =               0x03
} LIS2DH_Fullscale_t;

typedef enum {
  LIS2DH_BLE_LSB			=		0x00,
  LIS2DH_BLE_MSB			=		0x01
} LIS2DH_Endianess_t;

typedef enum {
  LIS2DH_SELF_TEST_DISABLE             =               0x00,
  LIS2DH_SELF_TEST_0                   =               0x01,
  LIS2DH_SELF_TEST_1                   =               0x02
} LIS2DH_SelfTest_t;

typedef enum {
  LIS2DH_FIFO_BYPASS_MODE              =               0x00,
  LIS2DH_FIFO_MODE                     =               0x01,
  LIS2DH_FIFO_STREAM_MODE              =               0x02,
  LIS2DH_FIFO_TRIGGER_MODE             =               0x03,
  LIS2DH_FIFO_DISABLE                  =               0x04
} LIS2DH_FifoMode_t;

typedef enum {
  LIS2DH_TRIG_INT1                     =		0x00,
  LIS2DH_TRIG_INT2 			=		0x01
} LIS2DH_TrigInt_t;

typedef enum {
  LIS2DH_SPI_4_WIRE                    =               0x00,
  LIS2DH_SPI_3_WIRE                    =               0x01
} LIS2DH_SPIMode_t;

typedef enum {
  LIS2DH_X_ENABLE                      =               0x01,
  LIS2DH_X_DISABLE                     =               0x00,
  LIS2DH_Y_ENABLE                      =               0x02,
  LIS2DH_Y_DISABLE                     =               0x00,
  LIS2DH_Z_ENABLE                      =               0x04,
  LIS2DH_Z_DISABLE                     =               0x00    
} LIS2DH_AXISenable_t;

typedef enum {
  LIS2DH_INT1_6D_4D_DISABLE            =               0x00,
  LIS2DH_INT1_6D_ENABLE                =               0x01,
  LIS2DH_INT1_4D_ENABLE                =               0x02 
} LIS2DH_INT_6D_4D_t;

typedef enum {
  LIS2DH_UP_SX                         =               0x44,
  LIS2DH_UP_DX                         =               0x42,
  LIS2DH_DW_SX                         =               0x41,
  LIS2DH_DW_DX                         =               0x48,
  LIS2DH_TOP                           =               0x60,
  LIS2DH_BOTTOM                        =               0x50
} LIS2DH_POSITION_6D_t;

typedef enum {
  LIS2DH_INT_MODE_OR                   =               0x00,
  LIS2DH_INT_MODE_6D_MOVEMENT          =               0x01,
  LIS2DH_INT_MODE_AND                  =               0x02,
  LIS2DH_INT_MODE_6D_POSITION          =               0x03  
} LIS2DH_Int1Mode_t;


//interrupt click response
//  b7 = don't care   b6 = IA  b5 = DClick  b4 = Sclick  b3 = Sign  
//  b2 = z      b1 = y     b0 = x
typedef enum {
LIS2DH_DCLICK_Z_P                      =               0x24,
LIS2DH_DCLICK_Z_N                      =               0x2C,
LIS2DH_SCLICK_Z_P                      =               0x14,
LIS2DH_SCLICK_Z_N                      =               0x1C,
LIS2DH_DCLICK_Y_P                      =               0x22,
LIS2DH_DCLICK_Y_N                      =               0x2A,
LIS2DH_SCLICK_Y_P                      =               0x12,
LIS2DH_SCLICK_Y_N			=		0x1A,
LIS2DH_DCLICK_X_P                      =               0x21,
LIS2DH_DCLICK_X_N                      =               0x29,
LIS2DH_SCLICK_X_P                      =               0x11,
LIS2DH_SCLICK_X_N                      =               0x19,
LIS2DH_NO_CLICK                        =               0x00
} LIS2DH_Click_Response; 

//TODO: start from here and manage the shared macros etc before this

/* Exported constants --------------------------------------------------------*/

#ifndef __SHARED__CONSTANTS
#define __SHARED__CONSTANTS

#define MEMS_SET                                        0x01
#define MEMS_RESET                                      0x00

#endif /*__SHARED__CONSTANTS*/


//Register Definition
#define LIS2DH_WHO_AM_I				0x0F  // device identification register

// CONTROL REGISTER 1
#define LIS2DH_CTRL_REG1				0x20
#define LIS2DH_ODR_BIT				        BIT(4)
#define LIS2DH_LPEN					BIT(3)
#define LIS2DH_ZEN					BIT(2)
#define LIS2DH_YEN					BIT(1)
#define LIS2DH_XEN					BIT(0)

//CONTROL REGISTER 2
#define LIS2DH_CTRL_REG2				0x21
#define LIS2DH_HPM     				BIT(6)
#define LIS2DH_HPCF					BIT(4)
#define LIS2DH_FDS					BIT(3)
#define LIS2DH_HPCLICK					BIT(2)
#define LIS2DH_HPIS2					BIT(1)
#define LIS2DH_HPIS1					BIT(0)

//CONTROL REGISTER 3
#define LIS2DH_CTRL_REG3				0x22
#define LIS2DH_I1_CLICK				BIT(7)
#define LIS2DH_I1_AOI1					BIT(6)
#define LIS2DH_I1_AOI2				        BIT(5)
#define LIS2DH_I1_DRDY1				BIT(4)
#define LIS2DH_I1_DRDY2				BIT(3)
#define LIS2DH_I1_WTM					BIT(2)
#define LIS2DH_I1_ORUN					BIT(1)

//CONTROL REGISTER 6
#define LIS2DH_CTRL_REG6				0x25
#define LIS2DH_I2_CLICK				BIT(7)
#define LIS2DH_I2_INT1					BIT(6)
#define LIS2DH_I2_BOOT         			BIT(4)
#define LIS2DH_H_LACTIVE				BIT(1)

//TEMPERATURE CONFIG REGISTER
#define LIS2DH_TEMP_CFG_REG				0x1F
#define LIS2DH_ADC_PD				        BIT(7)
#define LIS2DH_TEMP_EN					BIT(6)

//CONTROL REGISTER 4
#define LIS2DH_CTRL_REG4				0x23
#define LIS2DH_BDU					BIT(7)
#define LIS2DH_BLE					BIT(6)
#define LIS2DH_FS					BIT(4)
#define LIS2DH_HR					BIT(3)
#define LIS2DH_ST       				BIT(1)
#define LIS2DH_SIM					BIT(0)

//CONTROL REGISTER 5
#define LIS2DH_CTRL_REG5				0x24
#define LIS2DH_BOOT                                    BIT(7)
#define LIS2DH_FIFO_EN                                 BIT(6)
#define LIS2DH_LIR_INT1                                BIT(3)
#define LIS2DH_D4D_INT1                                BIT(2)

//REFERENCE/DATA_CAPTURE
#define LIS2DH_REFERENCE_REG		                0x26
#define LIS2DH_REF		                	BIT(0)

//STATUS_REG_AXIES
#define LIS2DH_STATUS_REG				0x27
#define LIS2DH_ZYXOR                                   BIT(7)
#define LIS2DH_ZOR                                     BIT(6)
#define LIS2DH_YOR                                     BIT(5)
#define LIS2DH_XOR                                     BIT(4)
#define LIS2DH_ZYXDA                                   BIT(3)
#define LIS2DH_ZDA                                     BIT(2)
#define LIS2DH_YDA                                     BIT(1)
#define LIS2DH_XDA                                     BIT(0)

//STATUS_REG_AUX
#define LIS2DH_STATUS_AUX				0x07

//INTERRUPT 1 CONFIGURATION
#define LIS2DH_INT1_CFG				0x30
#define LIS2DH_ANDOR                                   BIT(7)
#define LIS2DH_INT_6D                                  BIT(6)
#define LIS2DH_ZHIE                                    BIT(5)
#define LIS2DH_ZLIE                                    BIT(4)
#define LIS2DH_YHIE                                    BIT(3)
#define LIS2DH_YLIE                                    BIT(2)
#define LIS2DH_XHIE                                    BIT(1)
#define LIS2DH_XLIE                                    BIT(0)

//FIFO CONTROL REGISTER
#define LIS2DH_FIFO_CTRL_REG                           0x2E
#define LIS2DH_FM                                      BIT(6)
#define LIS2DH_TR                                      BIT(5)
#define LIS2DH_FTH                                     BIT(0)

//CONTROL REG3 bit mask
#define LIS2DH_CLICK_ON_PIN_INT1_ENABLE                0x80
#define LIS2DH_CLICK_ON_PIN_INT1_DISABLE               0x00
#define LIS2DH_I1_INT1_ON_PIN_INT1_ENABLE              0x40
#define LIS2DH_I1_INT1_ON_PIN_INT1_DISABLE             0x00
#define LIS2DH_I1_INT2_ON_PIN_INT1_ENABLE              0x20
#define LIS2DH_I1_INT2_ON_PIN_INT1_DISABLE             0x00
#define LIS2DH_I1_DRDY1_ON_INT1_ENABLE                 0x10
#define LIS2DH_I1_DRDY1_ON_INT1_DISABLE                0x00
#define LIS2DH_I1_DRDY2_ON_INT1_ENABLE                 0x08
#define LIS2DH_I1_DRDY2_ON_INT1_DISABLE                0x00
#define LIS2DH_WTM_ON_INT1_ENABLE                      0x04
#define LIS2DH_WTM_ON_INT1_DISABLE                     0x00
#define LIS2DH_INT1_OVERRUN_ENABLE                     0x02
#define LIS2DH_INT1_OVERRUN_DISABLE                    0x00

//CONTROL REG6 bit mask
#define LIS2DH_CLICK_ON_PIN_INT2_ENABLE                0x80
#define LIS2DH_CLICK_ON_PIN_INT2_DISABLE               0x00
#define LIS2DH_I2_INT1_ON_PIN_INT2_ENABLE              0x40
#define LIS2DH_I2_INT1_ON_PIN_INT2_DISABLE             0x00
#define LIS2DH_I2_INT2_ON_PIN_INT2_ENABLE              0x20
#define LIS2DH_I2_INT2_ON_PIN_INT2_DISABLE             0x00
#define LIS2DH_I2_BOOT_ON_INT2_ENABLE                  0x10
#define LIS2DH_I2_BOOT_ON_INT2_DISABLE                 0x00
#define LIS2DH_INT_ACTIVE_HIGH                         0x00
#define LIS2DH_INT_ACTIVE_LOW                          0x02

//INT1_CFG bit mask
#define LIS2DH_INT1_AND                                0x80
#define LIS2DH_INT1_OR                                 0x00
#define LIS2DH_INT1_ZHIE_ENABLE                        0x20
#define LIS2DH_INT1_ZHIE_DISABLE                       0x00
#define LIS2DH_INT1_ZLIE_ENABLE                        0x10
#define LIS2DH_INT1_ZLIE_DISABLE                       0x00
#define LIS2DH_INT1_YHIE_ENABLE                        0x08
#define LIS2DH_INT1_YHIE_DISABLE                       0x00
#define LIS2DH_INT1_YLIE_ENABLE                        0x04
#define LIS2DH_INT1_YLIE_DISABLE                       0x00
#define LIS2DH_INT1_XHIE_ENABLE                        0x02
#define LIS2DH_INT1_XHIE_DISABLE                       0x00
#define LIS2DH_INT1_XLIE_ENABLE                        0x01
#define LIS2DH_INT1_XLIE_DISABLE                       0x00

//INT1_SRC bit mask
#define LIS2DH_INT1_SRC_IA                             0x40
#define LIS2DH_INT1_SRC_ZH                             0x20
#define LIS2DH_INT1_SRC_ZL                             0x10
#define LIS2DH_INT1_SRC_YH                             0x08
#define LIS2DH_INT1_SRC_YL                             0x04
#define LIS2DH_INT1_SRC_XH                             0x02
#define LIS2DH_INT1_SRC_XL                             0x01

//INT1 REGISTERS
#define LIS2DH_INT1_THS                                0x32
#define LIS2DH_INT1_DURATION                           0x33

//INTERRUPT 1 SOURCE REGISTER
#define LIS2DH_INT1_SRC				0x31

//FIFO Source Register bit Mask
#define LIS2DH_FIFO_SRC_WTM                            0x80
#define LIS2DH_FIFO_SRC_OVRUN                          0x40
#define LIS2DH_FIFO_SRC_EMPTY                          0x20
  
//INTERRUPT CLICK REGISTER
#define LIS2DH_CLICK_CFG				0x38
//INTERRUPT CLICK CONFIGURATION bit mask
#define LIS2DH_ZD_ENABLE                               0x20
#define LIS2DH_ZD_DISABLE                              0x00
#define LIS2DH_ZS_ENABLE                               0x10
#define LIS2DH_ZS_DISABLE                              0x00
#define LIS2DH_YD_ENABLE                               0x08
#define LIS2DH_YD_DISABLE                              0x00
#define LIS2DH_YS_ENABLE                               0x04
#define LIS2DH_YS_DISABLE                              0x00
#define LIS2DH_XD_ENABLE                               0x02
#define LIS2DH_XD_DISABLE                              0x00
#define LIS2DH_XS_ENABLE                               0x01
#define LIS2DH_XS_DISABLE                              0x00

//INTERRUPT CLICK SOURCE REGISTER
#define LIS2DH_CLICK_SRC                               0x39
//INTERRUPT CLICK SOURCE REGISTER bit mask
#define LIS2DH_IA                                      0x40
#define LIS2DH_DCLICK                                  0x20
#define LIS2DH_SCLICK                                  0x10
#define LIS2DH_CLICK_SIGN                              0x08
#define LIS2DH_CLICK_Z                                 0x04
#define LIS2DH_CLICK_Y                                 0x02
#define LIS2DH_CLICK_X                                 0x01

//Click-click Register
#define LIS2DH_CLICK_THS                               0x3A
#define LIS2DH_TIME_LIMIT                              0x3B
#define LIS2DH_TIME_LATENCY                            0x3C
#define LIS2DH_TIME_WINDOW                             0x3D

//OUTPUT REGISTER
#define LIS2DH_OUT_X_L					0x28
#define LIS2DH_OUT_X_H					0x29
#define LIS2DH_OUT_Y_L					0x2A
#define LIS2DH_OUT_Y_H					0x2B
#define LIS2DH_OUT_Z_L					0x2C
#define LIS2DH_OUT_Z_H					0x2D

//AUX REGISTER
#define LIS2DH_OUT_1_L					0x08
#define LIS2DH_OUT_1_H					0x09
#define LIS2DH_OUT_2_L					0x0A
#define LIS2DH_OUT_2_H					0x0B
#define LIS2DH_OUT_3_L					0x0C
#define LIS2DH_OUT_3_H					0x0D

//STATUS REGISTER bit mask
#define LIS2DH_STATUS_REG_ZYXOR                        0x80    // 1	:	new data set has over written the previous one
							// 0	:	no overrun has occurred (default)	
#define LIS2DH_STATUS_REG_ZOR                          0x40    // 0	:	no overrun has occurred (default)
							// 1	:	new Z-axis data has over written the previous one
#define LIS2DH_STATUS_REG_YOR                          0x20    // 0	:	no overrun has occurred (default)
							// 1	:	new Y-axis data has over written the previous one
#define LIS2DH_STATUS_REG_XOR                          0x10    // 0	:	no overrun has occurred (default)
							// 1	:	new X-axis data has over written the previous one
#define LIS2DH_STATUS_REG_ZYXDA                        0x08    // 0	:	a new set of data is not yet avvious one
                                                        // 1	:	a new set of data is available 
#define LIS2DH_STATUS_REG_ZDA                          0x04    // 0	:	a new data for the Z-Axis is not availvious one
                                                        // 1	:	a new data for the Z-Axis is available
#define LIS2DH_STATUS_REG_YDA                          0x02    // 0	:	a new data for the Y-Axis is not available
                                                        // 1	:	a new data for the Y-Axis is available
#define LIS2DH_STATUS_REG_XDA                          0x01    // 0	:	a new data for the X-Axis is not available

#define LIS2DH_DATAREADY_BIT                           LIS2DH_STATUS_REG_ZYXDA


//STATUS AUX REGISTER bit mask
#define LIS2DH_STATUS_AUX_321OR                         0x80
#define LIS2DH_STATUS_AUX_3OR                           0x40
#define LIS2DH_STATUS_AUX_2OR                           0x20
#define LIS2DH_STATUS_AUX_1OR                           0x10
#define LIS2DH_STATUS_AUX_321DA                         0x08
#define LIS2DH_STATUS_AUX_3DA                           0x04
#define LIS2DH_STATUS_AUX_2DA                           0x02
#define LIS2DH_STATUS_AUX_1DA                           0x01

#define LIS2DH_MEMS_I2C_ADDRESS			        0x33

//FIFO REGISTERS
#define LIS2DH_FIFO_CTRL_REG			        0x2E
#define LIS2DH_FIFO_SRC_REG			        0x2F


/* Exported macro ------------------------------------------------------------*/

#ifndef __SHARED__MACROS

#define __SHARED__MACROS
#define ValBit(VAR,Place)         (VAR & (1<<Place))
#define BIT(x) ( (x) )

#endif /*__SHARED__MACROS*/

/* Exported functions --------------------------------------------------------*/

u8_t LIS2DH_WriteReg(u8_t WriteAddr, u8_t Data);
//Sensor Configuration Functions
status_t LIS2DH_SetODR(LIS2DH_ODR_t ov);
status_t LIS2DH_SetMode(LIS2DH_Mode_t md);
status_t LIS2DH_SetAxis(LIS2DH_Axis_t axis);
status_t LIS2DH_SetFullScale(LIS2DH_Fullscale_t fs);
status_t LIS2DH_SetBDU(State_t bdu);
status_t LIS2DH_SetBLE(LIS2DH_Endianess_t ble);
status_t LIS2DH_SetSelfTest(LIS2DH_SelfTest_t st);
status_t LIS2DH_SetTemperature(State_t state);
status_t LIS2DH_SetADCAux(State_t state);

//Filtering Functions
status_t LIS2DH_HPFClickEnable(State_t hpfe);
status_t LIS2DH_HPFAOI1Enable(State_t hpfe);
status_t LIS2DH_HPFAOI2Enable(State_t hpfe);
status_t LIS2DH_SetHPFMode(LIS2DH_HPFMode_t hpf);
status_t LIS2DH_SetHPFCutOFF(LIS2DH_HPFCutOffFreq_t hpf);
status_t LIS2DH_SetFilterDataSel(State_t state);

//Interrupt Functions
status_t LIS2DH_SetInt1Pin(LIS2DH_IntPinConf_t pinConf);
status_t LIS2DH_SetInt2Pin(LIS2DH_IntPinConf_t pinConf);
status_t LIS2DH_Int1LatchEnable(State_t latch);
status_t LIS2DH_ResetInt1Latch(void);
status_t LIS2DH_Reset(void);
status_t LIS2DH_SetIntConfiguration(LIS2DH_Int1Conf_t ic);
status_t LIS2DH_SetInt1Threshold(u8_t threshold);
status_t LIS2DH_SetInt1Duration(LIS2DH_Int1Conf_t id);
status_t LIS2DH_SetIntMode(LIS2DH_Int1Mode_t ic);
status_t LIS2DH_SetClickCFG(u8_t status);
status_t LIS2DH_SetInt6D4DConfiguration(LIS2DH_INT_6D_4D_t ic);
status_t LIS2DH_GetInt1Src(u8_t* val);
status_t LIS2DH_GetInt1SrcBit(u8_t statusBIT, u8_t* val);

//FIFO Functions
status_t LIS2DH_FIFOModeEnable(LIS2DH_FifoMode_t fm);
status_t LIS2DH_SetWaterMark(u8_t wtm);
status_t LIS2DH_SetTriggerInt(LIS2DH_TrigInt_t tr);
status_t LIS2DH_GetFifoSourceReg(u8_t* val);
status_t LIS2DH_GetFifoSourceBit(u8_t statusBIT, u8_t* val);
status_t LIS2DH_GetFifoSourceFSS(u8_t* val);

//Other Reading Functions
status_t LIS2DH_GetStatusReg(u8_t* val);
status_t LIS2DH_GetStatusBit(u8_t statusBIT, u8_t* val);
status_t LIS2DH_GetStatusAUXBit(u8_t statusBIT, u8_t* val);
status_t LIS2DH_GetStatusAUX(u8_t* val);
status_t LIS2DH_GetAccAxesRawBurst(AxesRaw_t buff[32], u16_t num_reads);
status_t LIS2DH_GetAccAxesRaw(AxesRaw_t* buff);
status_t LIS2DH_GetAuxRaw(LIS2DH_Aux123Raw_t* buff);
status_t LIS2DH_GetClickResponse(u8_t* val);
status_t LIS2DH_GetTempRaw(i8_t* val);
status_t LIS2DH_GetWHO_AM_I(u8_t* val);
status_t LIS2DH_Get6DPosition(u8_t* val);

//Generic
// i.e. u8_t LIS2DH_ReadReg(u8_t Reg, u8_t* Data);
// i.e. u8_t LIS2DH_WriteReg(u8_t Reg, u8_t Data);


#endif /* __LIS2DH_H */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/



