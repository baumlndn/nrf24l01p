/*
 * bme280.c
 *
 *  Created on: 19.11.2017
 *      Author: baumlndn
 */


#include <avr/io.h>
#include <util/delay.h>
#include "bme280.h"
#include "i2c.h"
#include "usart.h"

void BME280_Init ( void )
{
	_delay_ms(500);

	/* DS1337 control register set to 0 -> 1 Hz output at INTB */
	uint8_t i2c_data[13];

	/* I2C status byte */
	char i2c_status;

	/* Initialize I2C with SCL = 100kHz */
	I2C_Init(32);
	_delay_ms(500);

	i2c_status = I2C_Receive(BME280_I2C_ADDR,0xD0,&i2c_data[0],2);
//	USART_Transmit(i2c_status);
//	USART_Transmit(i2c_data[0]);

	_delay_ms(2000);

	i2c_status = I2C_Receive(BME280_I2C_ADDR,0xF2,&i2c_data[0],13);
//	USART_Transmit(i2c_status);
	for (uint8_t i=0;i<13;i++)
	{
//		USART_Transmit(i2c_data[i]);
	}

	_delay_ms(500);
	BME280_readCompensation();
}

void BME280_readCompensation (void)
{
	uint8_t tmp_data[7];
	char i2c_status;

	/* Read temperatures compensation */
	i2c_status = I2C_Receive(BME280_I2C_ADDR,0x88,&tmp_data[0],6u);
	if (i2c_status == 0)
	{
		dig_T1 = (tmp_data[0]) | ((uint16_t) tmp_data[1] << 8 );
		dig_T2 = (tmp_data[2]) | ((int16_t) tmp_data[3] << 8 );
		dig_T3 = (tmp_data[4]) | ((int16_t) tmp_data[5] << 8 );
	}

	/* Read pressure compension */
	i2c_status = I2C_Receive(BME280_I2C_ADDR,0x8E,&tmp_data[0],6u);
	if (i2c_status == 0)
	{
		dig_P1 = (tmp_data[0]) | ((uint16_t) tmp_data[1] << 8 );
		dig_P2 = (tmp_data[2]) | ((int16_t) tmp_data[3] << 8 );
		dig_P3 = (tmp_data[4]) | ((int16_t) tmp_data[5] << 8 );
	}

	i2c_status = I2C_Receive(BME280_I2C_ADDR,0x94,&tmp_data[0],6u);
	if (i2c_status == 0)
	{
		dig_P4 = (tmp_data[0]) | ((int16_t) tmp_data[1] << 8 );
		dig_P5 = (tmp_data[2]) | ((int16_t) tmp_data[3] << 8 );
		dig_P6 = (tmp_data[4]) | ((int16_t) tmp_data[5] << 8 );
	}

	i2c_status = I2C_Receive(BME280_I2C_ADDR,0x9A,&tmp_data[0],6u);
	if (i2c_status == 0)
	{
		dig_P7 = (tmp_data[0]) | ((int16_t) tmp_data[1] << 8 );
		dig_P8 = (tmp_data[2]) | ((int16_t) tmp_data[3] << 8 );
		dig_P9 = (tmp_data[4]) | ((int16_t) tmp_data[5] << 8 );
	}

	/* Read humidity compensation */
	i2c_status = I2C_Receive(BME280_I2C_ADDR,0xA1,&tmp_data[0],1u);
	if (i2c_status == 0)
	{
		dig_H1 = (tmp_data[0]);
	}
	i2c_status = I2C_Receive(BME280_I2C_ADDR,0xE1,&tmp_data[0],7u);
	if (i2c_status == 0)
	{
		dig_H2 = (tmp_data[0]) | ((int16_t) tmp_data[1] << 8 );
		dig_H3 = (tmp_data[2]);
		dig_H4 = ((int16_t) tmp_data[3] << 4) | (tmp_data[4] & 0x0F);
		dig_H5 = ((int16_t) tmp_data[5] << 4) | ((tmp_data[4] & 0xF0) >> 4);
		dig_H6 = ((int8_t)tmp_data[6]);
	}
}

void BME280_measure (int32_t * temperature, uint32_t *pressure, uint32_t *humidity)
{
	char i2c_status;
	uint8_t i2c_data[9];

	int32_t raw_temp, raw_press, raw_hum;

	/* Trigger measurement */
	i2c_data[0] = 0x01;
	i2c_status = I2C_Transmit(BME280_I2C_ADDR,0xF2,&i2c_data[0],1u);

	i2c_data[0] = 0x00;
	i2c_status = I2C_Transmit(BME280_I2C_ADDR,0xF5,&i2c_data[0],1u);

	i2c_data[0] = 0x25;
	i2c_status = I2C_Transmit(BME280_I2C_ADDR,0xF4,&i2c_data[0],1u);

	/* Wait until measurement is finished */
	i2c_status = 0xFF;

	while ( (i2c_status != 0) || (i2c_data[0] != 0) )
	{
		_delay_ms(250);
		i2c_status = I2C_Receive(BME280_I2C_ADDR,0xF3,&i2c_data[0],1u);
	}

	i2c_status = I2C_Receive(BME280_I2C_ADDR,0xF7,&i2c_data[0],8u);
	if (i2c_status == 0)
	{
		raw_press = ((uint32_t)i2c_data[0] << 12) | ((uint32_t)i2c_data[1] << 4) | (i2c_data[2] & 0x0F);
		raw_temp =  ((uint32_t)i2c_data[3] << 12) | ((uint32_t)i2c_data[4] << 4) | ((i2c_data[5] & 0xF0) >> 4);
		raw_hum = ((uint32_t)i2c_data[6] <<  8) | (i2c_data[7]);

		*temperature = BME280_compensate_T_int32(raw_temp);
		*pressure = BME280_compensate_P_int32(raw_press);
		*humidity = BME280_compensate_H_int32(raw_hum);

	}
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T)
{
	BME280_S32_t var1, var2, T;
	var1  = ((((adc_T>>3) - ((BME280_S32_t)dig_T1<<1))) * ((BME280_S32_t)dig_T2)) >> 11;
	var2  = (((((adc_T>>4) - ((BME280_S32_t)dig_T1)) * ((adc_T>>4) - ((BME280_S32_t)dig_T1))) >> 12) *
			((BME280_S32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T  = (t_fine * 5 + 128) >> 8;
	return T;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H)
{
	BME280_S32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)dig_H4) << 20) - (((BME280_S32_t)dig_H5) * v_x1_u32r)) +
			((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)dig_H6)) >> 10) * (((v_x1_u32r *
			((BME280_S32_t)dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) *
			((BME280_S32_t)dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (BME280_U32_t)(v_x1_u32r>>12);
}

// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
BME280_U32_t BME280_compensate_P_int32(BME280_S32_t adc_P)
{
	BME280_S32_t var1, var2;
	BME280_U32_t p;
	var1 = (((BME280_S32_t)t_fine)>>1) - (BME280_S32_t)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((BME280_S32_t)dig_P6);
	var2 = var2 + ((var1*((BME280_S32_t)dig_P5))<<1);
	var2 = (var2>>2)+(((BME280_S32_t)dig_P4)<<16);
	var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((BME280_S32_t)dig_P2) * var1)>>1))>>18;
	var1 =((((32768+var1))*((BME280_S32_t)dig_P1))>>15);
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = (((BME280_U32_t)(((BME280_S32_t)1048576)-adc_P)-(var2>>12)))*3125;
	if (p < 0x80000000)
	{
		p = (p << 1) / ((BME280_U32_t)var1);
	}
	else
	{
		p = (p / (BME280_U32_t)var1) * 2;
	}
	var1 = (((BME280_S32_t)dig_P9) * ((BME280_S32_t)(((p>>3) * (p>>3))>>13)))>>12;
	var2 = (((BME280_S32_t)(p>>2)) * ((BME280_S32_t)dig_P8))>>13;
	p = (BME280_U32_t)((BME280_S32_t)p + ((var1 + var2 + dig_P7) >> 4));
	return p;
}
