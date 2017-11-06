/*
 * spi.h
 *
 *  Created on: 05.11.2017
 *      Author: baumlndn
 */

#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include <util/delay.h>
#include "config.h"

char * SPI_dataBuffer;

void SPI_MasterInit(char * dataBuffer);
char SPI_MasterTransmit(char cData);
char* SPI_MasterTransmitMulti(uint8_t byte_cnt, char* data);

#endif /* SPI_H_ */
