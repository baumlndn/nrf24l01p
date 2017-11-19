/*
 * usart.h
 *
 *  Created on: 11.02.2015
 *      Author: baumlndn
 */

#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>

void I2C_Init( unsigned int twbr);
uint8_t I2C_Transmit( char device_id, char address, unsigned char * data, uint8_t length );
unsigned char I2C_Receive( char device_id, char address, unsigned char * data, uint8_t length );

#endif /* I2C_H_ */
