/**
 ****************************************************************************************
 *
 * @file nau7802.c
 *
 * @brief Driver implementation for NAU7802 bridge sensor ADC.
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE. *
 ***************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "nau7802.h"
#include "i2cm.h"
#include "arch_console.h"

/*
 * DEFINES
 ****************************************************************************************
 */
 
/* NAU7802 Register Map */
#define NAU7802_REG_PU_CTRL											(0x00)
#define NAU7802_REG_CTRL1												(0x01)
#define NAU7802_REG_CTRL2												(0x02)
#define NAU7802_REG_OCAL1_B2										(0x03)
#define NAU7802_REG_OCAL1_B1										(0x04)
#define NAU7802_REG_OCAL1_B0										(0x05)
#define NAU7802_REG_GCAL1_B3										(0x06)
#define NAU7802_REG_GCAL1_B2										(0x07)
#define NAU7802_REG_GCAL1_B1										(0x08)
#define NAU7802_REG_GCAL1_B0										(0x09)
#define NAU7802_REG_OCAL2_B2										(0x0A)
#define NAU7802_REG_OCAL2_B1										(0x0B)
#define NAU7802_REG_OCAL2_B0										(0x0C)
#define NAU7802_REG_GCAL2_B3										(0x0D)
#define NAU7802_REG_GCAL2_B2										(0x0E)
#define NAU7802_REG_GCAL2_B1										(0x0F)
#define NAU7802_REG_GCAL2_B0										(0x10)
#define NAU7802_REG_I2C_CTRL										(0x11)
#define NAU7802_REG_ADCO_B2											(0x12)
#define NAU7802_REG_ADCO_B1											(0x13)
#define NAU7802_REG_ADCO_B0											(0x14)
#define NAU7802_REG_ADC													(0x15)
#define NAU7802_REG_OTP_B0											(0x16)
#define NAU7802_REG_REVCODE											(0x1F)

/* NAU7802 Register masks */
#define NAU7802_MSK_PU_CTRL_AVDDS								(0x80)
#define NAU7802_POS_PU_CTRL_AVDDS								(7UL)
#define NAU7802_MSK_PU_CTRL_OSCS								(0x40)
#define NAU7802_POS_PU_CTRL_OSCS								(6UL)
#define NAU7802_MSK_PU_CTRL_CR									(0x20)
#define NAU7802_POS_PU_CTRL_CR									(5UL)
#define NAU7802_MSK_PU_CTRL_CS									(0x10)
#define NAU7802_POS_PU_CTRL_CS									(4UL)
#define NAU7802_MSK_PU_CTRL_PUR									(0x08)
#define NAU7802_POS_PU_CTRL_PUR									(3UL)
#define NAU7802_MSK_PU_CTRL_PUA									(0x04)
#define NAU7802_POS_PU_CTRL_PUA									(2UL)
#define NAU7802_MSK_PU_CTRL_PUD									(0x02)
#define NAU7802_POS_PU_CTRL_PUD									(1UL)
#define NAU7802_MSK_PU_CTRL_RR									(0x01)
#define NAU7802_POS_PU_CTRL_RR									(0UL)

#define NAU7802_MSK_CTRL1_CRP										(0x80)
#define NAU7802_POS_CTRL1_CRP										(7UL)
#define NAU7802_MSK_CTRL1_DRDY_SEL							(0x40)
#define NAU7802_POS_CTRL1_DRDY_SEL							(6UL)
#define NAU7802_MSK_CTRL1_VLDO									(0x38)
#define NAU7802_POS_CTRL1_VLDO									(3UL)
#define NAU7802_MSK_CTRL1_GAINS									(0x07)
#define NAU7802_POS_CTRL1_GAINS									(0UL)

#define NAU7802_MSK_CTRL2_CHS										(0x80)
#define NAU7802_POS_CTRL2_CHS										(7UL)
#define NAU7802_MSK_CTRL2_CRS										(0x70)
#define NAU7802_POS_CTRL2_CRS										(4UL)
#define NAU7802_MSK_CTRL2_CAL_ERR								(0x08)
#define NAU7802_POS_CTRL2_CAL_ERR								(3UL)
#define NAU7802_MSK_CTRL2_CALS									(0x04)
#define NAU7802_POS_CTRL2_CALS									(2UL)
#define NAU7802_MSK_CTRL2_CALMOD								(0x03)
#define NAU7802_POS_CTRL2_CALMOD								(0UL)

#define NAU7802_MSK_I2C_CTRL_CRSD								(0x80)
#define NAU7802_POS_I2C_CTRL_CRSD								(7UL)
#define NAU7802_MSK_I2C_CTRL_FRD								(0x40)
#define NAU7802_POS_I2C_CTRL_FRD								(6UL)
#define NAU7802_MSK_I2C_CTRL_SPE								(0X20)
#define NAU7802_POS_I2C_CTRL_SPE								(5UL)
#define NAU7802_MSK_I2C_CTRL_WPD								(0X10)
#define NAU7802_POS_I2C_CTRL_WPD								(4UL)
#define NAU7802_MSK_I2C_CTRL_SI									(0X08)
#define NAU7802_POS_I2C_CTRL_SI									(3UL)
#define NAU7802_MSK_I2C_CTRL_BOPGA							(0X04)
#define NAU7802_POS_I2C_CTRL_BOPGA							(2UL)
#define NAU7802_MSK_I2C_CTRL_TS									(0X02)
#define NAU7802_POS_I2C_CTRL_TS									(1UL)
#define NAU7802_MSK_I2C_CTRL_BGPCP							(0X01)
#define NAU7802_POS_I2C_CTRL_BGPCP							(0UL)

#define NAU7802_MSK_ADC_REG_CHP									(0x30)
#define NAU7802_POS_ADC_REG_CHP									(4UL)
#define NAU7802_MSK_ADC_REG_ADC_VCM							(0x0C)
#define NAU7802_POS_ADC_REG_ADC_VCM							(2UL)
#define NAU7802_MSK_ADC_REG_ADC_REG_CHP					(0x03)
#define NAU7802_POS_ADC_REG_ADC_REG_CHP					(0UL)

#define NAU7802_MSK_RD_OTP_SEL									(0x80)
#define NAU7802_POS_RD_OTP_SEL									(7UL)
#define NAU7802_MSK_RD_LDOMODE									(0x40)
#define NAU7802_POS_RD_LDOMODE									(6UL)
#define NAU7802_MSK_RD_PGA_OUT_BUF_EN						(0x20)
#define NAU7802_POS_RD_PGA_OUT_BUF_EN						(5UL)
#define NAU7802_MSK_RD_PGA_BYPASS_EN						(0x10)
#define NAU7802_POS_RD_PGA_BYPASS_EN						(4UL)
#define NAU7802_MSK_RD_PGAINV										(0x8)
#define NAU7802_POS_RD_PGAINV										(3UL)
#define NAU7802_MSK_RD_PGACHPDIS								(0x1)
#define NAU7802_POS_RD_PGACHPDIS								(0UL)

#define NAU7802_MSK_PGA_CAP_EN									(0x80)
#define NAU7802_POS_PGA_CAP_EN									(7UL)
#define NAU7802_MSK_MASTER_BIAS_CURR						(0x70)
#define NAU7802_POS_MASTER_BIAS_CURR						(4UL)
#define NAU7802_MSK_ADC_CURR										(0xC)
#define NAU7802_POS_ADC_CURR										(2UL)
#define NAU7802_MSK_PGA_CURR										(0x03)
#define NAU7802_POS_PGA_CURR										(0UL)

#define NAU7802_MSK_REVCODE_ID									(0x0F)
#define NAU7802_POS_REVCODE_ID									(0UL)


/* NAU7802 Register definitions */
#define NAU7802_DEF_CTRL1_VLDO_2V4							(0b111)
#define NAU7802_DEF_CTRL1_VLDO_2V7							(0b110)
#define NAU7802_DEF_CTRL1_VLDO_3V0							(0b101)
#define NAU7802_DEF_CTRL1_VLDO_3V3							(0b100)
#define NAU7802_DEF_CTRL1_VLDO_3V6							(0b011)
#define NAU7802_DEF_CTRL1_VLDO_3V9							(0b010)
#define NAU7802_DEF_CTRL1_VLDO_4V2							(0b001)
#define NAU7802_DEF_CTRL1_VLDO_4V5							(0b000)

#define NAU7802_DEF_CTRL1_GAIN_128							(0b111)
#define NAU7802_DEF_CTRL1_GAIN_64								(0b110)
#define NAU7802_DEF_CTRL1_GAIN_32								(0b101)
#define NAU7802_DEF_CTRL1_GAIN_16								(0b100)
#define NAU7802_DEF_CTRL1_GAIN_8								(0b011)
#define NAU7802_DEF_CTRL1_GAIN_4								(0b010)
#define NAU7802_DEF_CTRL1_GAIN_2								(0b001)
#define NAU7802_DEF_CTRL1_GAIN_1								(0b000)

#define NAU7802_DEF_CTRL2_CRS_320SPS						(0b111)
#define NAU7802_DEF_CTRL2_CRS_80SPS							(0b011)
#define NAU7802_DEF_CTRL2_CRS_40SPS							(0b010)
#define NAU7802_DEF_CTRL2_CRS_20SPS							(0b001)
#define NAU7802_DEF_CTRL2_CRS_10SPS							(0b000)

#define NAU7802_DEF_CTRL2_CALMOD_GAIN						(0b11)
#define NAU7802_DEF_CTRL2_CALMOD_OFFS						(0b10)
#define NAU7802_DEF_CTRL2_CALMOD_OFFSINT				(0b00)

#define NAU7802_DEF_ADC_REG_CHPS_OFF						(0b11)

#define NAU7802_DEF_ADC_REG_ADC_VCM_DIS					(0b00)
#define NAU7802_DEF_ADC_REG_ADC_VCM_EN_REFN			(0b10)
#define NAU7802_DEF_ADC_REG_ADC_VCM_EN_REFP			(0b11)

#define NAU7802_DEF_PWR_CTRL_BIAS_100PCT				(0b000)
#define NAU7802_DEF_PWR_CTRL_BIAS_90PCT					(0b001)
#define NAU7802_DEF_PWR_CTRL_BIAS_80PCT					(0b010)
#define NAU7802_DEF_PWR_CTRL_BIAS_73PCT					(0b011)
#define NAU7802_DEF_PWR_CTRL_BIAS_67PCT					(0b100)
#define NAU7802_DEF_PWR_CTRL_BIAS_62PCT					(0b101)
#define NAU7802_DEF_PWR_CTRL_BIAS_58PCT					(0b110)
#define NAU7802_DEF_PWR_CTRL_BIAS_54PCT					(0b111)

#define NAU7802_DEF_PWR_CTRL_ADC_100PCT					(0b00)
#define NAU7802_DEF_PWR_CTRL_ADC_75PCT					(0b01)
#define NAU7802_DEF_PWR_CTRL_ADC_50PCT					(0b10)
#define NAU7802_DEF_PWR_CTRL_ADC_25PCT					(0b11)

#define NAU7802_DEF_PWR_CTRL_PGA_100PCT					(0b00)
#define NAU7802_DEF_PWR_CTRL_PGA_75PCT					(0b01)
#define NAU7802_DEF_PWR_CTRL_PGA_50PCT					(0b10)
#define NAU7802_DEF_PWR_CTRL_PGA_25PCT					(0b11)

/* NAU7802 definitions */
#define NAU7802_DEF_I2C_ADDR										(0x2A)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
 
 /*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */
static int nau7802_weight_offset __SECTION_ZERO("retention_mem_area0");
static uint32_t nau7802_ug_per_bit __SECTION_ZERO("retention_mem_area0");

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

static int write(uint8_t addr, uint8_t const * const data, uint8_t len);
static int read(uint8_t addr, uint8_t * const data, uint8_t len);
static int write_reg(uint8_t addr, uint8_t data);
static int read_reg(uint8_t addr, uint8_t * const data);
static int set_reg_field(uint8_t addr, uint8_t pos, uint8_t msk, uint8_t val);
static int get_reg_field(uint8_t addr, uint8_t pos, uint8_t msk, uint8_t * const val);


/**
 ****************************************************************************************
 * @brief   Initialize the NAU7802 driver and the device.
 *
 * @return  0 on success, and non-zero error code on failure.
 ****************************************************************************************
*/
int nau7802_init(void)
{
  int ret;
  uint8_t id;
	
	/* set default calibration values */
	nau7802_weight_offset = 120000; 
	nau7802_ug_per_bit = 10000;

  /* First get device into a known state */
  if ((ret = nau7802_sw_reset())==0)
	{
		/* Check if communication with the device is possible */
		if ((ret = nau7802_get_chip_rev(&id)) == 0) {
			/* Print revision code */
			#ifdef CFG_PRINTF
			{
				arch_printf("NAU7802 REV: %X \n\r",id);
			}
			#else
			ret = 0;
			#endif
		}
		else {
			arch_printf("NAU7802 communication error: %i \n",ret);
		}
	}
	else {
		arch_printf("NAU7802 communication errorr: %i \n",ret);
	}
	
  return ret;
}


/**
 ****************************************************************************************
 * @brief   Software reset puts the device into default state.
 *
 * @return  0 on success, 1 on failure
 ****************************************************************************************
*/
int nau7802_sw_reset(void)
{
	uint8_t PUR;
	
	/* perform reset procedure */
	set_reg_field(NAU7802_REG_PU_CTRL,NAU7802_POS_PU_CTRL_RR,NAU7802_MSK_PU_CTRL_RR,1);
	set_reg_field(NAU7802_REG_PU_CTRL,NAU7802_POS_PU_CTRL_RR,NAU7802_MSK_PU_CTRL_RR,0);
	
	/* check if reset was succesful */
	get_reg_field(NAU7802_REG_PU_CTRL, NAU7802_POS_PU_CTRL_PUR, NAU7802_MSK_PU_CTRL_PUR,&PUR);
	
	return PUR;
}

/**
 ****************************************************************************************
 * @brief   Enables the ADC of the NAU7802
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int nau7802_enable_adc(void)
{
	int ret;
	
	/* set bit fields for: reference voltage source, oscillator source, power up digital, power up analog, and cycle start */
	if ((ret = write_reg(NAU7802_REG_PU_CTRL,0xBE))==0)
	{		
		/* set bit fields for: adc reference voltage */
		if ((ret = write_reg(NAU7802_REG_ADC,0x30))==0){
			/* set bit fields for: adc gain (x128) */
			//ret = set_reg_field(NAU7802_REG_CTRL1,NAU7802_POS_CTRL1_GAINS,NAU7802_MSK_CTRL1_GAINS,0b111); 
		}
	}
	
	return ret;
}

/**
 ****************************************************************************************
 * @brief   Get the chip revision
 *
 * @param   Pointer to variable used to return the chip revision
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int nau7802_get_chip_rev(uint8_t * const chip_id)
{
  int ret;
  uint8_t tmp;

  ret = read_reg(NAU7802_REG_REVCODE, &tmp);

  if (!ret) {
    *chip_id = tmp;
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Get offset load cell adc value.
 *
 * @param   load - pointer to return offset adc value
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int nau7802_get_ofset_load(int *offsload)
{
  int ret;
	int adcval;
	int offsetload;
	
	if ((ret = nau7802_get_adc(&adcval))==0)
	{
		offsetload = (adcval - nau7802_weight_offset);
		if (offsetload < 0)
		{
			*offsload = 0;
		}else
		{
		*offsload = (adcval - nau7802_weight_offset);
		}
		arch_printf("  offsload: %i", *offsload);
	}
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Get load cell micrograms value.
 *
 * @param   load - pointer to return micrograms read from load cell
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int nau7802_get_ug_load(uint32_t *load)
{
  int ret;
	int offsetload;
	
	if ((ret = nau7802_get_ofset_load(&offsetload))==0)
	{
		*load = offsetload * nau7802_ug_per_bit;
		arch_printf("  load: %u  ug_per_bit: %i  ", *load, nau7802_ug_per_bit);
	}
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Get load cell adc value.
 *
 * @param   adcval - pointer to return adc value.
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int nau7802_get_adc(int *adcval)
{
  int ret;
  uint8_t raw_data[3];
	uint8_t ready;
	uint32_t raw_load;
	
	if ((ret = get_reg_field(NAU7802_REG_PU_CTRL, NAU7802_POS_PU_CTRL_CR, NAU7802_MSK_PU_CTRL_CR, &ready)) == 0){
		if (((ret = read(NAU7802_REG_ADCO_B2, raw_data, 3)) == 0) && ready) {
				raw_load = (uint32_t)((((uint32_t)(raw_data[0])) << 16) |
																					 (((uint32_t)(raw_data[1])) << 8)  |
																						((uint32_t)(raw_data[2])));

			*adcval = 0xFFFFFF - raw_load;
			arch_printf("  adcval: %i", *adcval);
		}
		#ifdef CFG_PRINTF
		else {
			arch_puts("not ready!\n\r");
		}
		#endif
	}
		
  return ret;
}

/**
 ****************************************************************************************
 * @brief   set the 0 point of the scale.
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int nau7802_zero_scale(void)
{
	#ifdef CFG_PRINTF
	arch_puts("zero scale\n\r");
	#endif
	int ret;
	int currload;
	if ((ret = nau7802_get_adc(&currload)) == 0){
		nau7802_weight_offset = currload;
	}
	return ret;
}

/**
 ****************************************************************************************
 * @brief   Calibrate the scale with a known weight
 *
 * @param   weight - calibration weight value in kg
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
int nau7802_calib_scale(uint32_t weight)
{
	#ifdef CFG_PRINTF
	arch_printf("Calib with value: %i\n\r", weight);
	#endif
	int ret;
	int offsetload;
	if ((ret = nau7802_get_ofset_load(&offsetload))==0)
	{
		nau7802_ug_per_bit = weight/offsetload;
	}
	
	return ret;
}

/**
 ****************************************************************************************
 * @brief   Microsecond delay, interrupts not disabled so could be longer than requested.
 *
 * @param   Number of microseconds to delay
 *
 * @return  None
 ****************************************************************************************
 */
static void us_delay(uint32_t delay_us)
{
  while( delay_us-- ){
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
  }
}

/**
 ****************************************************************************************
 * @brief   Perform read-modify-write on a bit field within a NAU7802 register.
 *
 * @param   addr - register address
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int set_reg_field(uint8_t addr, uint8_t pos, uint8_t msk, uint8_t val)
{
  int ret = 0;
  uint8_t tmp;

  ret = read_reg(addr, &tmp);

  if (!ret) {
    tmp = (tmp & ~msk);
    tmp = tmp | ((val << pos) & msk);
    ret = write_reg(addr, tmp);
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Get field from a NAU7802 register
 *
 * @param   addr - register address
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int get_reg_field(uint8_t addr, uint8_t pos, uint8_t msk, uint8_t * const val)
{
  int ret;
  uint8_t tmp;

  if ((ret = read_reg(addr, &tmp)) == 0) {
    *val = (tmp & msk) >> pos;
  }
  return ret;
}

/*
 ****************************************************************************************
 * @brief   Read data from a NAU7802 register.
 *
 * @param   Memory address
 * @param   Buffer in which data is placed
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int read_reg(uint8_t addr, uint8_t * const data)
{
  return read(addr, data, sizeof(uint8_t));
}

/**
 ****************************************************************************************
 * @brief   Write data to a NAU7802 register.
 *
 * @param   Memory address
 * @param   Data to be written
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int write_reg(uint8_t addr, uint8_t data)
{
  return write(addr, &data, sizeof(uint8_t));
}

/*
 ****************************************************************************************
 * @brief   Read data from the NAU7802 using the I2C interface.
 *
 * @param   Memory address
 * @param   Buffer in which data is placed
 * @param   Number of bytes of data to read
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int read(uint8_t addr, uint8_t * const data, uint8_t len)
{
  int ret;

  /* Set register address to read from */
  if ((ret = i2cm_write(NAU7802_DEF_I2C_ADDR, &addr, sizeof(addr), false)) == 0) {
    /* Read data at the address */
	us_delay(2);
  ret = i2cm_read(NAU7802_DEF_I2C_ADDR, data, len, true);
	us_delay(2);
  }
  return ret;
}

/**
 ****************************************************************************************
 * @brief   Write data to the NAU7802 using the I2C interface.
 *
 * @param   Memory address
 * @param   Variable length payload
 * @param   Number of bytes within payload to write
 *
 * @return  0 on success, and non-zero error code on failure
 ****************************************************************************************
*/
static int write(uint8_t addr, uint8_t const * const data, uint8_t len)
{
  int ret = -1;
  uint8_t payload[16];

  if (len <= (sizeof(payload) - 1)) {
    /* Start address to write must be first byte of payload */
    memset(payload, 0, sizeof(payload));
    memcpy(&payload[1], data, len);
    payload[0] = addr;

    /* Write data (if any) to address */
    ret = i2cm_write(NAU7802_DEF_I2C_ADDR, payload, len + 1, true);
		us_delay(2);
  }
  return ret;
}