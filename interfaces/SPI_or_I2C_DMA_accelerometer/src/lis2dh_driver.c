 /**
 ***************************************************************************************
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
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
/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : LIS2DH_driver.c
* Author             : MSH Application Team
* Author             : Fabio Tota
* Version            : $Revision:$
* Date               : $Date:$
* Description        : LIS2DH driver file
*                      
* HISTORY:
* Date               |	Modification                    |	Author
* 24/06/2011         |	Initial Revision                |	Fabio Tota
* 11/06/2012         |	Support for multiple drivers in the same program |	Abhishek Anand

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

/* Includes ------------------------------------------------------------------*/
#include "lis2dh_driver.h"
#include "i2c.h"
#include "spi.h"
#include "i2c_impl.h"
#include "spi_impl.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name		: LIS2DH_ReadReg
* Description		: Generic Reading function. It must be fulfilled with either
*			: I2C or SPI reading functions					
* Input			: Register Address
* Output		: Data REad
* Return		: None
*******************************************************************************/

u8_t LIS2DH_ReadRegN(u8_t Reg_start, AxesRaw_t Data[33], u16_t burst_num)
{
	status_t err = MEMS_SUCCESS;
#ifdef __ACCEL_IF_I2C__	
	err = i2c_accel_read_fifo(Reg_start, Data, burst_num);
#elif defined(__ACCEL_IF_SPI__)
	spi_accel_read_fifo(Reg_start, Data, burst_num);
#else
	"Please select a SPI or I2C interface to the accel"
#endif
			
	return err;
}

/*******************************************************************************
* Function Name		: LIS2DH_ReadReg
* Description		: Generic Reading function. It must be fulfilled with either
*			: I2C or SPI reading functions					
* Input			: Register Address
* Output		: Data REad
* Return		: None
*******************************************************************************/
u8_t LIS2DH_ReadReg(u8_t Reg, u8_t* Data) {

	status_t err = MEMS_SUCCESS;
#ifdef __ACCEL_IF_I2C__		
	err = i2c_accel_read_reg(Reg, Data);
#elif defined(__ACCEL_IF_SPI__)
 err = spi_accel_read_reg(Reg, Data);
#else
	"Please select a SPI or I2C interface to the accel"
#endif 
	
		
	return err;
}


/*******************************************************************************
* Function Name		: LIS2DH_WriteReg
* Description		: Generic Writing function. It must be fulfilled with either
*			: I2C or SPI writing function
* Input			: Register Address, Data to be written
* Output		: None
* Return		: None
*******************************************************************************/
u8_t LIS2DH_WriteReg(u8_t WriteAddr, u8_t Data) {

	
	status_t err = MEMS_SUCCESS;
#ifdef __ACCEL_IF_I2C__	
	err = i2c_accel_write_reg(WriteAddr, Data);
#elif defined(__ACCEL_IF_SPI__)
  err = spi_accel_write_reg(WriteAddr, Data);
#else	
	#error "Please select a SPI or I2C interface to the accel"
#endif 
	return err;
}


/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : LIS2DH_GetWHO_AM_I
* Description    : Read identification code by WHO_AM_I register
* Input          : Char to empty by Device identification Value
* Output         : None
* Return         : Status [value of FSS]
*******************************************************************************/
status_t LIS2DH_GetWHO_AM_I(u8_t* val){
  
  if( !LIS2DH_ReadReg(LIS2DH_WHO_AM_I, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_GetStatusAUX
* Description    : Read the AUX status register
* Input          : Char to empty by status register buffer
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_GetStatusAUX(u8_t* val) {
  
  if( !LIS2DH_ReadReg(LIS2DH_STATUS_AUX, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;  
}



/*******************************************************************************
* Function Name  : LIS2DH_GetStatusAUXBIT
* Description    : Read the AUX status register BIT
* Input          : LIS2DH_STATUS_AUX_321OR, LIS2DH_STATUS_AUX_3OR, LIS2DH_STATUS_AUX_2OR, LIS2DH_STATUS_AUX_1OR,
                   LIS2DH_STATUS_AUX_321DA, LIS2DH_STATUS_AUX_3DA, LIS2DH_STATUS_AUX_2DA, LIS2DH_STATUS_AUX_1DA
* Output         : None
* Return         : Status of BIT [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_GetStatusAUXBit(u8_t statusBIT, u8_t* val) {
  u8_t value;  
  
  if( !LIS2DH_ReadReg(LIS2DH_STATUS_AUX, &value) )
    return MEMS_ERROR;
  
  if(statusBIT == LIS2DH_STATUS_AUX_321OR){
    if(value &= LIS2DH_STATUS_AUX_321OR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  }
  
  if(statusBIT == LIS2DH_STATUS_AUX_3OR){
    if(value &= LIS2DH_STATUS_AUX_3OR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }     
  }
  
  if(statusBIT == LIS2DH_STATUS_AUX_2OR){
    if(value &= LIS2DH_STATUS_AUX_2OR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }    
  }
  
  if(statusBIT == LIS2DH_STATUS_AUX_1OR){
    if(value &= LIS2DH_STATUS_AUX_1OR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  }
  
  if(statusBIT == LIS2DH_STATUS_AUX_321DA){
    if(value &= LIS2DH_STATUS_AUX_321DA) {     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  }
  
  if(statusBIT == LIS2DH_STATUS_AUX_3DA){
    if(value &= LIS2DH_STATUS_AUX_3DA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  }
  
  if(statusBIT == LIS2DH_STATUS_AUX_2DA){
    if(value &= LIS2DH_STATUS_AUX_2DA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS2DH_STATUS_AUX_1DA){
    if(value &= LIS2DH_STATUS_AUX_1DA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }  
  return MEMS_ERROR;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetODR
* Description    : Sets LIS2DH Output Data Rate
* Input          : Output Data Rate
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetODR(LIS2DH_ODR_t ov){
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG1, &value) )
    return MEMS_ERROR;
  
  value &= 0x0f;
  value |= ov<<LIS2DH_ODR_BIT;
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG1, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetTemperature
* Description    : Sets LIS2DH Output Temperature
* Input          : MEMS_ENABLE, MEMS_DISABLE
* Output         : None
* Note           : For Read Temperature by LIS2DH_OUT_AUX_3, LIS2DH_SetADCAux and LIS2DH_SetBDU 
				   functions must be ENABLE
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetTemperature(State_t state){
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_TEMP_CFG_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0xBF;
  value |= state<<LIS2DH_TEMP_EN;
  
  if( !LIS2DH_WriteReg(LIS2DH_TEMP_CFG_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetADCAux
* Description    : Sets LIS2DH Output ADC
* Input          : MEMS_ENABLE, MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetADCAux(State_t state){
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_TEMP_CFG_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0x7F;
  value |= state<<LIS2DH_ADC_PD;
  
  if( !LIS2DH_WriteReg(LIS2DH_TEMP_CFG_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_GetAuxRaw
* Description    : Read the Aux Values Output Registers
* Input          : Buffer to empty
* Output         : Aux Values Registers buffer
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_GetAuxRaw(LIS2DH_Aux123Raw_t* buff) {
  u8_t valueL;
  u8_t valueH;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_1_L, &valueL) )
    return MEMS_ERROR;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_1_H, &valueH) )
    return MEMS_ERROR;
  
  buff->AUX_1 = (u16_t)( (valueH << 8) | valueL )/16;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_2_L, &valueL) )
    return MEMS_ERROR;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_2_H, &valueH) )
    return MEMS_ERROR;
  
  buff->AUX_2 = (u16_t)( (valueH << 8) | valueL )/16;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_3_L, &valueL) )
    return MEMS_ERROR;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_3_H, &valueH) )
    return MEMS_ERROR;
  
  buff->AUX_3 = (u16_t)( (valueH << 8) | valueL )/16;
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : LIS2DH_GetTempRaw
* Description    : Read the Temperature Values by AUX Output Registers OUT_3_H
* Input          : Buffer to empty
* Output         : Temperature Values Registers buffer
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_GetTempRaw(i8_t* buff) {
  u8_t valueL;
  u8_t valueH;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_3_L, &valueL) )
    return MEMS_ERROR;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_3_H, &valueH) )
    return MEMS_ERROR;
  
  *buff = (i8_t)( valueH );
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : LIS2DH_SetMode
* Description    : Sets LIS2DH Operating Mode
* Input          : Modality (LIS2DH_NORMAL, LIS2DH_LOW_POWER, LIS2DH_POWER_DOWN)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetMode(LIS2DH_Mode_t md) {
  u8_t value;
  u8_t value2;
  static   u8_t ODR_old_value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG1, &value) )
    return MEMS_ERROR;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG4, &value2) )
    return MEMS_ERROR;
  
  if((value & 0xF0)==0) 
    value = value | (ODR_old_value & 0xF0); //if it comes from POWERDOWN  
  
  switch(md) {
    
  case LIS2DH_POWER_DOWN:
    ODR_old_value = value;
    value &= 0x0F;
    break;
    
  case LIS2DH_NORMAL:
    value &= 0xF7;
    value |= (MEMS_RESET<<LIS2DH_LPEN);
    value2 &= 0xF7;
    value2 |= (MEMS_SET<<LIS2DH_HR);   //set HighResolution_BIT
    break;
    
  case LIS2DH_LOW_POWER:		
    value &= 0xF7;
    value |=  (MEMS_SET<<LIS2DH_LPEN);
    value2 &= 0xF7;
    value2 |= (MEMS_RESET<<LIS2DH_HR); //reset HighResolution_BIT
    break;
    
  default:
    return MEMS_ERROR;
  }
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG1, value) )
    return MEMS_ERROR;
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG4, value2) )
    return MEMS_ERROR;  
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetAxis
* Description    : Enable/Disable LIS2DH Axis
* Input          : LIS2DH_X_ENABLE/DISABLE | LIS2DH_Y_ENABLE/DISABLE | LIS2DH_Z_ENABLE/DISABLE
* Output         : None
* Note           : You MUST use all input variable in the argument, as example
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetAxis(LIS2DH_Axis_t axis) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG1, &value) )
    return MEMS_ERROR;
  value &= 0xF8;
  value |= (0x07 & axis);
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG1, value) )
    return MEMS_ERROR;   
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetFullScale
* Description    : Sets the LIS2DH FullScale
* Input          : LIS2DH_FULLSCALE_2/LIS2DH_FULLSCALE_4/LIS2DH_FULLSCALE_8/LIS2DH_FULLSCALE_16
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetFullScale(LIS2DH_Fullscale_t fs) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0xCF;	
  value |= (fs<<LIS2DH_FS);
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetBDU
* Description    : Enable/Disable Block Data Update Functionality
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetBDU(State_t bdu) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0x7F;
  value |= (bdu<<LIS2DH_BDU);
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetBLE
* Description    : Set Endianness (MSB/LSB)
* Input          : BLE_LSB / BLE_MSB
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetBLE(LIS2DH_Endianess_t ble) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0xBF;	
  value |= (ble<<LIS2DH_BLE);
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetSelfTest
* Description    : Set Self Test Modality
* Input          : LIS2DH_SELF_TEST_DISABLE/ST_0/ST_1
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetSelfTest(LIS2DH_SelfTest_t st) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0xF9;
  value |= (st<<LIS2DH_ST);
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_HPFClick
* Description    : Enable/Disable High Pass Filter for click
* Input          : MEMS_ENABLE/MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_HPFClickEnable(State_t hpfe) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xFB;
  value |= (hpfe<<LIS2DH_HPCLICK);
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_HPFAOI1
* Description    : Enable/Disable High Pass Filter for AOI on INT_1
* Input          : MEMS_ENABLE/MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_HPFAOI1Enable(State_t hpfe) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xFE;
  value |= (hpfe<<LIS2DH_HPIS1);
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_HPFAOI2
* Description    : Enable/Disable High Pass Filter for AOI on INT_2
* Input          : MEMS_ENABLE/MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_HPFAOI2Enable(State_t hpfe) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xFD;
  value |= (hpfe<<LIS2DH_HPIS2);
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetHPFMode
* Description    : Set High Pass Filter Modality
* Input          : LIS2DH_HPM_NORMAL_MODE_RES/LIS2DH_HPM_REF_SIGNAL/
				   LIS2DH_HPM_NORMAL_MODE/LIS2DH_HPM_AUTORESET_INT
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetHPFMode(LIS2DH_HPFMode_t hpm) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0x3F;
  value |= (hpm<<LIS2DH_HPM);
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetHPFCutOFF
* Description    : Set High Pass CUT OFF Freq
* Input          : HPFCF [0,3]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetHPFCutOFF(LIS2DH_HPFCutOffFreq_t hpf) {
  u8_t value;
  
  if (hpf > 3)
    return MEMS_ERROR;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xCF;
  value |= (hpf<<LIS2DH_HPCF);
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : LIS2DH_SetFilterDataSel
* Description    : Set Filter Data Selection bypassed or sent to FIFO OUT register
* Input          : MEMS_SET, MEMS_RESET
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetFilterDataSel(State_t state) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xF7;
  value |= (state<<LIS2DH_FDS);
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : LIS2DH_SetInt1Pin
* Description    : Set Interrupt1 pin Function
* Input          :  LIS2DH_CLICK_ON_PIN_INT1_ENABLE/DISABLE    | LIS2DH_I1_INT1_ON_PIN_INT1_ENABLE/DISABLE |              
                    LIS2DH_I1_INT2_ON_PIN_INT1_ENABLE/DISABLE  | LIS2DH_I1_DRDY1_ON_INT1_ENABLE/DISABLE    |              
                    LIS2DH_I1_DRDY2_ON_INT1_ENABLE/DISABLE     | LIS2DH_WTM_ON_INT1_ENABLE/DISABLE         |           
                    LIS2DH_INT1_OVERRUN_ENABLE/DISABLE  
* example        : SetInt1Pin(LIS2DH_CLICK_ON_PIN_INT1_ENABLE | LIS2DH_I1_INT1_ON_PIN_INT1_ENABLE |              
                    LIS2DH_I1_INT2_ON_PIN_INT1_DISABLE | LIS2DH_I1_DRDY1_ON_INT1_ENABLE | LIS2DH_I1_DRDY2_ON_INT1_ENABLE |
                    LIS2DH_WTM_ON_INT1_DISABLE | LIS2DH_INT1_OVERRUN_DISABLE   ) 
* Note           : To enable Interrupt signals on INT1 Pad (You MUST use all input variable in the argument, as example)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetInt1Pin(LIS2DH_IntPinConf_t pinConf) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG3, &value) )
    return MEMS_ERROR;
  
  value &= 0x00;
  value |= pinConf;
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG3, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetInt2Pin
* Description    : Set Interrupt2 pin Function
* Input          : LIS2DH_CLICK_ON_PIN_INT2_ENABLE/DISABLE   | LIS2DH_I2_INT1_ON_PIN_INT2_ENABLE/DISABLE |               
                   LIS2DH_I2_INT2_ON_PIN_INT2_ENABLE/DISABLE | LIS2DH_I2_BOOT_ON_INT2_ENABLE/DISABLE |                   
                   LIS2DH_INT_ACTIVE_HIGH/LOW
* example        : LIS2DH_SetInt2Pin(LIS2DH_CLICK_ON_PIN_INT2_ENABLE/DISABLE | LIS2DH_I2_INT1_ON_PIN_INT2_ENABLE/DISABLE |               
                   LIS2DH_I2_INT2_ON_PIN_INT2_ENABLE/DISABLE | LIS2DH_I2_BOOT_ON_INT2_ENABLE/DISABLE |                   
                   LIS2DH_INT_ACTIVE_HIGH/LOW)
* Note           : To enable Interrupt signals on INT2 Pad (You MUST use all input variable in the argument, as example)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetInt2Pin(LIS2DH_IntPinConf_t pinConf) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG6, &value) )
    return MEMS_ERROR;
  
  value &= 0x00;
  value |= pinConf;
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG6, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}                       


/*******************************************************************************
* Function Name  : LIS2DH_SetClickCFG
* Description    : Set Click Interrupt config Function
* Input          : LIS2DH_ZD_ENABLE/DISABLE | LIS2DH_ZS_ENABLE/DISABLE  | LIS2DH_YD_ENABLE/DISABLE  | 
                   LIS2DH_YS_ENABLE/DISABLE | LIS2DH_XD_ENABLE/DISABLE  | LIS2DH_XS_ENABLE/DISABLE 
* example        : LIS2DH_SetClickCFG( LIS2DH_ZD_ENABLE | LIS2DH_ZS_DISABLE | LIS2DH_YD_ENABLE | 
                               LIS2DH_YS_DISABLE | LIS2DH_XD_ENABLE | LIS2DH_XS_ENABLE)
* Note           : You MUST use all input variable in the argument, as example
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetClickCFG(u8_t status) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CLICK_CFG, &value) )
    return MEMS_ERROR;
  
  value &= 0xC0;
  value |= status;
  
  if( !LIS2DH_WriteReg(LIS2DH_CLICK_CFG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}  


/*******************************************************************************
* Function Name  : LIS2DH_SetClickTHS
* Description    : Set Click Interrupt threshold
* Input          : Click-click Threshold value [0-127]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetClickTHS(u8_t threshold) {
  
  if(threshold>127)     
    return MEMS_ERROR;
  
  if( !LIS2DH_WriteReg(LIS2DH_CLICK_THS, threshold) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
} 


/*******************************************************************************
* Function Name  : LIS2DH_SetClickLIMIT
* Description    : Set Click Interrupt Time Limit
* Input          : Click-click Time Limit value [0-127]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetClickLIMIT(u8_t t_limit) {
  
  if(t_limit>127)     
    return MEMS_ERROR;
  
  if( !LIS2DH_WriteReg(LIS2DH_TIME_LIMIT, t_limit) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
} 


/*******************************************************************************
* Function Name  : LIS2DH_SetClickLATENCY
* Description    : Set Click Interrupt Time Latency
* Input          : Click-click Time Latency value [0-255]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetClickLATENCY(u8_t t_latency) {
  
  if( !LIS2DH_WriteReg(LIS2DH_TIME_LATENCY, t_latency) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
} 


/*******************************************************************************
* Function Name  : LIS2DH_SetClickWINDOW
* Description    : Set Click Interrupt Time Window
* Input          : Click-click Time Window value [0-255]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetClickWINDOW(u8_t t_window) {
  
  if( !LIS2DH_WriteReg(LIS2DH_TIME_WINDOW, t_window) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_GetClickResponse
* Description    : Get Click Interrupt Response by CLICK_SRC REGISTER
* Input          : char to empty by Click Response Typedef
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_GetClickResponse(u8_t* res) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CLICK_SRC, &value) ) 
    return MEMS_ERROR;
  
  value &= 0x7F;
  
  if((value & LIS2DH_IA)==0) {        
    *res = LIS2DH_NO_CLICK;     
    return MEMS_SUCCESS;
  }
  else {
    if (value & LIS2DH_DCLICK){
      if (value & LIS2DH_CLICK_SIGN){
        if (value & LIS2DH_CLICK_Z) {
          *res = LIS2DH_DCLICK_Z_N;   
          return MEMS_SUCCESS;
        }
        if (value & LIS2DH_CLICK_Y) {
          *res = LIS2DH_DCLICK_Y_N;   
          return MEMS_SUCCESS;
        }
        if (value & LIS2DH_CLICK_X) {
          *res = LIS2DH_DCLICK_X_N;   
          return MEMS_SUCCESS;
        }
      }
      else{
        if (value & LIS2DH_CLICK_Z) {
          *res = LIS2DH_DCLICK_Z_P;   
          return MEMS_SUCCESS;
        }
        if (value & LIS2DH_CLICK_Y) {
          *res = LIS2DH_DCLICK_Y_P;   
          return MEMS_SUCCESS;
        }
        if (value & LIS2DH_CLICK_X) {
          *res = LIS2DH_DCLICK_X_P;   
          return MEMS_SUCCESS;
        }
      }       
    }
    else{
      if (value & LIS2DH_CLICK_SIGN){
        if (value & LIS2DH_CLICK_Z) {
          *res = LIS2DH_SCLICK_Z_N;   
          return MEMS_SUCCESS;
        }
        if (value & LIS2DH_CLICK_Y) {
          *res = LIS2DH_SCLICK_Y_N;   
          return MEMS_SUCCESS;
        }
        if (value & LIS2DH_CLICK_X) {
          *res = LIS2DH_SCLICK_X_N;   
          return MEMS_SUCCESS;
        }
      }
      else{
        if (value & LIS2DH_CLICK_Z) {
          *res = LIS2DH_SCLICK_Z_P;   
          return MEMS_SUCCESS;
        }
        if (value & LIS2DH_CLICK_Y) {
          *res = LIS2DH_SCLICK_Y_P;   
          return MEMS_SUCCESS;
        }
        if (value & LIS2DH_CLICK_X) {
          *res = LIS2DH_SCLICK_X_P;   
          return MEMS_SUCCESS;
        }
      }
    }
  }
  return MEMS_ERROR;
} 


/*******************************************************************************
* Function Name  : LIS2DH_Int1LatchEnable
* Description    : Enable Interrupt 1 Latching function
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_Int1LatchEnable(State_t latch) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG5, &value) )
    return MEMS_ERROR;
  
  value &= 0xF7;
  value |= latch<<LIS2DH_LIR_INT1;
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG5, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}

status_t LIS2DH_Reset(void)
{
	u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG5, &value) )
    return MEMS_ERROR;
  
  value &= 0x7F;
  value |= 1<<LIS2DH_BOOT;
	
	if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG5, value) )
		return MEMS_ERROR;
  
	
	if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG5, &value) )
			return MEMS_ERROR;
  
  value &= 0x7F;
	
	if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG5, value) )
		return MEMS_ERROR;
	
  

	
  return MEMS_SUCCESS;
}



/*******************************************************************************
* Function Name  : LIS2DH_ResetInt1Latch
* Description    : Reset Interrupt 1 Latching function
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_ResetInt1Latch(void) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_INT1_SRC, &value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetIntConfiguration
* Description    : Interrupt 1 Configuration (without LIS2DH_6D_INT)
* Input          : LIS2DH_INT1_AND/OR | LIS2DH_INT1_ZHIE_ENABLE/DISABLE | LIS2DH_INT1_ZLIE_ENABLE/DISABLE...
* Output         : None
* Note           : You MUST use all input variable in the argument, as example
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetIntConfiguration(LIS2DH_Int1Conf_t ic) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_INT1_CFG, &value) )
    return MEMS_ERROR;
  
  value &= 0x40; 
  value |= ic;
  
  if( !LIS2DH_WriteReg(LIS2DH_INT1_CFG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
} 

     
/*******************************************************************************
* Function Name  : LIS2DH_SetIntMode
* Description    : Interrupt 1 Configuration mode (OR, 6D Movement, AND, 6D Position)
* Input          : LIS2DH_INT_MODE_OR, LIS2DH_INT_MODE_6D_MOVEMENT, LIS2DH_INT_MODE_AND, 
				   LIS2DH_INT_MODE_6D_POSITION
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetIntMode(LIS2DH_Int1Mode_t int_mode) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_INT1_CFG, &value) )
    return MEMS_ERROR;
  
  value &= 0x3F; 
  value |= (int_mode<<LIS2DH_INT_6D);
  
  if( !LIS2DH_WriteReg(LIS2DH_INT1_CFG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}

    
/*******************************************************************************
* Function Name  : LIS2DH_SetInt6D4DConfiguration
* Description    : 6D, 4D Interrupt Configuration
* Input          : LIS2DH_INT1_6D_ENABLE, LIS2DH_INT1_4D_ENABLE, LIS2DH_INT1_6D_4D_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetInt6D4DConfiguration(LIS2DH_INT_6D_4D_t ic) {
  u8_t value;
  u8_t value2;
  
  if( !LIS2DH_ReadReg(LIS2DH_INT1_CFG, &value) )
    return MEMS_ERROR;
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG5, &value2) )
    return MEMS_ERROR;
  
  if(ic == LIS2DH_INT1_6D_ENABLE){
    value &= 0xBF; 
    value |= (MEMS_ENABLE<<LIS2DH_INT_6D);
    value2 &= 0xFB; 
    value2 |= (MEMS_DISABLE<<LIS2DH_D4D_INT1);
  }
  
  if(ic == LIS2DH_INT1_4D_ENABLE){
    value &= 0xBF; 
    value |= (MEMS_ENABLE<<LIS2DH_INT_6D);
    value2 &= 0xFB; 
    value2 |= (MEMS_ENABLE<<LIS2DH_D4D_INT1);
  }
  
  if(ic == LIS2DH_INT1_6D_4D_DISABLE){
    value &= 0xBF; 
    value |= (MEMS_DISABLE<<LIS2DH_INT_6D);
    value2 &= 0xFB; 
    value2 |= (MEMS_DISABLE<<LIS2DH_D4D_INT1);
  }
  
  if( !LIS2DH_WriteReg(LIS2DH_INT1_CFG, value) )
    return MEMS_ERROR;
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG5, value2) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_Get6DPosition
* Description    : 6D, 4D Interrupt Position Detect
* Input          : Byte to empty by POSITION_6D_t Typedef
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_Get6DPosition(u8_t* val){
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_INT1_SRC, &value) )
    return MEMS_ERROR;
  
  value &= 0x7F;
  
  switch (value){
  case LIS2DH_UP_SX:   
    *val = LIS2DH_UP_SX;    
    break;
  case LIS2DH_UP_DX:   
    *val = LIS2DH_UP_DX;    
    break;
  case LIS2DH_DW_SX:   
    *val = LIS2DH_DW_SX;    
    break;
  case LIS2DH_DW_DX:   
    *val = LIS2DH_DW_DX;    
    break;
  case LIS2DH_TOP:     
    *val = LIS2DH_TOP;      
    break;
  case LIS2DH_BOTTOM:  
    *val = LIS2DH_BOTTOM;   
    break;
  }
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : LIS2DH_SetInt1Threshold
* Description    : Sets Interrupt 1 Threshold
* Input          : Threshold = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetInt1Threshold(u8_t threshold) {
  if (threshold > 127)
    return MEMS_ERROR;
  
  if( !LIS2DH_WriteReg(LIS2DH_INT1_THS, threshold) )
    return MEMS_ERROR;    
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetInt1Duration
* Description    : Sets Interrupt 1 Duration
* Input          : Duration value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetInt1Duration(LIS2DH_Int1Conf_t id) {
  
  if (id > 127)
    return MEMS_ERROR;
  
  if( !LIS2DH_WriteReg(LIS2DH_INT1_DURATION, id) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_FIFOModeEnable
* Description    : Sets Fifo Modality
* Input          : LIS2DH_FIFO_DISABLE, LIS2DH_FIFO_BYPASS_MODE, LIS2DH_FIFO_MODE, 
				   LIS2DH_FIFO_STREAM_MODE, LIS2DH_FIFO_TRIGGER_MODE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_FIFOModeEnable(LIS2DH_FifoMode_t fm) {
  u8_t value;  
  
  if(fm == LIS2DH_FIFO_DISABLE) { 
    if( !LIS2DH_ReadReg(LIS2DH_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1F;
    value |= (LIS2DH_FIFO_BYPASS_MODE<<LIS2DH_FM);                     
    
    if( !LIS2DH_WriteReg(LIS2DH_FIFO_CTRL_REG, value) )           //fifo mode bypass
      return MEMS_ERROR;   
    if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;    
    
    if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG5, value) )               //fifo disable
      return MEMS_ERROR;   
  }
  
  if(fm == LIS2DH_FIFO_BYPASS_MODE)   {  
    if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;
    value |= MEMS_SET<<LIS2DH_FIFO_EN;
    
    if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG5, value) )               //fifo enable
      return MEMS_ERROR;  
    if( !LIS2DH_ReadReg(LIS2DH_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<LIS2DH_FM);                     //fifo mode configuration
    
    if( !LIS2DH_WriteReg(LIS2DH_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }
  
  if(fm == LIS2DH_FIFO_MODE)   {
    if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;
    value |= MEMS_SET<<LIS2DH_FIFO_EN;
    
    if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG5, value) )               //fifo enable
      return MEMS_ERROR;  
    if( !LIS2DH_ReadReg(LIS2DH_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<LIS2DH_FM);                      //fifo mode configuration
    
    if( !LIS2DH_WriteReg(LIS2DH_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }
  
  if(fm == LIS2DH_FIFO_STREAM_MODE)   {  
    if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;
    value |= MEMS_SET<<LIS2DH_FIFO_EN;
    
    if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG5, value) )               //fifo enable
      return MEMS_ERROR;   
    if( !LIS2DH_ReadReg(LIS2DH_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<LIS2DH_FM);                      //fifo mode configuration
    
    if( !LIS2DH_WriteReg(LIS2DH_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }
  
  if(fm == LIS2DH_FIFO_TRIGGER_MODE)   {  
    if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;
    value |= MEMS_SET<<LIS2DH_FIFO_EN;
    
    if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG5, value) )               //fifo enable
      return MEMS_ERROR;    
    if( !LIS2DH_ReadReg(LIS2DH_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<LIS2DH_FM);                      //fifo mode configuration
    
    if( !LIS2DH_WriteReg(LIS2DH_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetTriggerInt
* Description    : Trigger event liked to trigger signal INT1/INT2
* Input          : LIS2DH_TRIG_INT1/LIS2DH_TRIG_INT2
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetTriggerInt(LIS2DH_TrigInt_t tr) {
  u8_t value;  
  
  if( !LIS2DH_ReadReg(LIS2DH_FIFO_CTRL_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0xDF;
  value |= (tr<<LIS2DH_TR); 
  
  if( !LIS2DH_WriteReg(LIS2DH_FIFO_CTRL_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_SetWaterMark
* Description    : Sets Watermark Value
* Input          : Watermark = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetWaterMark(u8_t wtm) {
  u8_t value;
  
  if(wtm > 31)
    return MEMS_ERROR;  
  
  if( !LIS2DH_ReadReg(LIS2DH_FIFO_CTRL_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0xE0;
  value |= wtm; 
  
  if( !LIS2DH_WriteReg(LIS2DH_FIFO_CTRL_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}

  
/*******************************************************************************
* Function Name  : LIS2DH_GetStatusReg
* Description    : Read the status register
* Input          : char to empty by Status Reg Value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_GetStatusReg(u8_t* val) {
  if( !LIS2DH_ReadReg(LIS2DH_STATUS_REG, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : LIS2DH_GetStatusBIT
* Description    : Read the status register BIT
* Input          : LIS2DH_STATUS_REG_ZYXOR, LIS2DH_STATUS_REG_ZOR, LIS2DH_STATUS_REG_YOR, LIS2DH_STATUS_REG_XOR,
                   LIS2DH_STATUS_REG_ZYXDA, LIS2DH_STATUS_REG_ZDA, LIS2DH_STATUS_REG_YDA, LIS2DH_STATUS_REG_XDA, 
				   LIS2DH_DATAREADY_BIT
				   val: Byte to be filled with the status bit	
* Output         : status register BIT
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_GetStatusBit(u8_t statusBIT, u8_t* val) {
  u8_t value;  
  
  if( !LIS2DH_ReadReg(LIS2DH_STATUS_REG, &value) )
    return MEMS_ERROR;
  
  switch (statusBIT){
  case LIS2DH_STATUS_REG_ZYXOR:     
    if(value &= LIS2DH_STATUS_REG_ZYXOR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  case LIS2DH_STATUS_REG_ZOR:       
    if(value &= LIS2DH_STATUS_REG_ZOR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  case LIS2DH_STATUS_REG_YOR:       
    if(value &= LIS2DH_STATUS_REG_YOR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }                                 
  case LIS2DH_STATUS_REG_XOR:       
    if(value &= LIS2DH_STATUS_REG_XOR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }     
  case LIS2DH_STATUS_REG_ZYXDA:     
    if(value &= LIS2DH_STATUS_REG_ZYXDA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  case LIS2DH_STATUS_REG_ZDA:       
    if(value &= LIS2DH_STATUS_REG_ZDA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  case LIS2DH_STATUS_REG_YDA:       
    if(value &= LIS2DH_STATUS_REG_YDA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  case LIS2DH_STATUS_REG_XDA:       
    if(value &= LIS2DH_STATUS_REG_XDA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }                                  
    
  }
  return MEMS_ERROR;
}
status_t LIS2DH_GetAccAxesRawBurst(AxesRaw_t buff[32], u16_t num_reads){

		if( !LIS2DH_ReadRegN(LIS2DH_OUT_X_L, buff, num_reads*6) )
			return MEMS_ERROR;
		
		return MEMS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LIS2DH_GetAccAxesRaw
* Description    : Read the Acceleration Values Output Registers
* Input          : buffer to empity by AxesRaw_t Typedef
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_GetAccAxesRaw(AxesRaw_t* buff) {
  i16_t value;
  u8_t *valueL = (u8_t *)(&value);
  u8_t *valueH = ((u8_t *)(&value)+1);
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_X_L, valueL) )
    return MEMS_ERROR;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_X_H, valueH) )
    return MEMS_ERROR;
  
  buff->AXIS_X = value;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_Y_L, valueL) )
    return MEMS_ERROR;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_Y_H, valueH) )
    return MEMS_ERROR;
  
  buff->AXIS_Y = value;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_Z_L, valueL) )
    return MEMS_ERROR;
  
  if( !LIS2DH_ReadReg(LIS2DH_OUT_Z_H, valueH) )
    return MEMS_ERROR;
  
  buff->AXIS_Z = value;
  
  return MEMS_SUCCESS; 
}


/*******************************************************************************
* Function Name  : LIS2DH_GetInt1Src
* Description    : Reset Interrupt 1 Latching function
* Input          : Char to empty by Int1 source value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_GetInt1Src(u8_t* val) {
  
  if( !LIS2DH_ReadReg(LIS2DH_INT1_SRC, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_GetInt1SrcBit
* Description    : Reset Interrupt 1 Latching function
* Input          : statusBIT: LIS2DH_INT_SRC_IA, LIS2DH_INT_SRC_ZH, LIS2DH_INT_SRC_ZL.....
*                  val: Byte to be filled with the status bit
* Output         : None
* Return         : Status of BIT [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_GetInt1SrcBit(u8_t statusBIT, u8_t* val) {
  u8_t value;  
   
  if( !LIS2DH_ReadReg(LIS2DH_INT1_SRC, &value) )
      return MEMS_ERROR;
   
  if(statusBIT == LIS2DH_INT1_SRC_IA){
    if(value &= LIS2DH_INT1_SRC_IA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS2DH_INT1_SRC_ZH){
    if(value &= LIS2DH_INT1_SRC_ZH){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS2DH_INT1_SRC_ZL){
    if(value &= LIS2DH_INT1_SRC_ZL){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS2DH_INT1_SRC_YH){
    if(value &= LIS2DH_INT1_SRC_YH){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS2DH_INT1_SRC_YL){
    if(value &= LIS2DH_INT1_SRC_YL){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  if(statusBIT == LIS2DH_INT1_SRC_XH){
    if(value &= LIS2DH_INT1_SRC_XH){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS2DH_INT1_SRC_XL){
    if(value &= LIS2DH_INT1_SRC_XL){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  return MEMS_ERROR;
}


/*******************************************************************************
* Function Name  : LIS2DH_GetFifoSourceReg
* Description    : Read Fifo source Register
* Input          : Byte to empty by FIFO source register value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_GetFifoSourceReg(u8_t* val) {
  
  if( !LIS2DH_ReadReg(LIS2DH_FIFO_SRC_REG, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS2DH_GetFifoSourceBit
* Description    : Read Fifo WaterMark source bit
* Input          : statusBIT: LIS2DH_FIFO_SRC_WTM, LIS2DH_FIFO_SRC_OVRUN, LIS2DH_FIFO_SRC_EMPTY
*				   val: Byte to fill  with the bit value
* Output         : None
* Return         : Status of BIT [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_GetFifoSourceBit(u8_t statusBIT,  u8_t* val){
  u8_t value;  
  
  if( !LIS2DH_ReadReg(LIS2DH_FIFO_SRC_REG, &value) )
    return MEMS_ERROR;
  
  
  if(statusBIT == LIS2DH_FIFO_SRC_WTM){
    if(value &= LIS2DH_FIFO_SRC_WTM){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS2DH_FIFO_SRC_OVRUN){
    if(value &= LIS2DH_FIFO_SRC_OVRUN){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  if(statusBIT == LIS2DH_FIFO_SRC_EMPTY){
    if(value &= statusBIT == LIS2DH_FIFO_SRC_EMPTY){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  return MEMS_ERROR;
}


/*******************************************************************************
* Function Name  : LIS2DH_GetFifoSourceFSS
* Description    : Read current number of unread samples stored in FIFO
* Input          : Byte to empty by FIFO unread sample value
* Output         : None
* Return         : Status [value of FSS]
*******************************************************************************/
status_t LIS2DH_GetFifoSourceFSS(u8_t* val){
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_FIFO_SRC_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0x1F;
  
  *val = value;
  
  return MEMS_SUCCESS;
}

      
/*******************************************************************************
* Function Name  : LIS2DH_SetSPIInterface
* Description    : Set SPI mode: 3 Wire Interface OR 4 Wire Interface
* Input          : LIS2DH_SPI_3_WIRE, LIS2DH_SPI_4_WIRE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS2DH_SetSPIInterface(LIS2DH_SPIMode_t spi) {
  u8_t value;
  
  if( !LIS2DH_ReadReg(LIS2DH_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0xFE;
  value |= spi<<LIS2DH_SIM;
  
  if( !LIS2DH_WriteReg(LIS2DH_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/