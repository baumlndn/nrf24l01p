/*
 * bme280.h
 *
 *  Created on: 19.11.2017
 *      Author: baumlndn
 */

#ifndef BME280_H_
#define BME280_H_

#include <avr/io.h>

#define BME280_I2C_ADDR		0x77

typedef uint32_t BME280_U32_t;
typedef int32_t BME280_S32_t;

uint16_t 	dig_T1;
int16_t		dig_T2, dig_T3;
uint16_t		dig_P1;
int16_t		dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
uint8_t		dig_H1;
int16_t		dig_H2;
uint8_t		dig_H3;
int16_t		dig_H4, dig_H5;
int8_t		dig_H6;
BME280_S32_t t_fine;


void BME280_Init( void );
void BME280_readCompensation (void);
void BME280_measure (int32_t * temperature, uint32_t *pressure, uint32_t *humidity);

BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T);
BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H);
BME280_U32_t BME280_compensate_P_int32(BME280_S32_t adc_P);

#endif /* BME280_H_ */
