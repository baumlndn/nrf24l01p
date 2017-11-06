/*
 * spi.c
 *
 *  Created on: 05.11.2017
 *      Author: baumlndn
 */

#include "spi.h"
#include "usart.h"

void SPI_MasterInit(char * dataBuffer)
{
	/* Set MOSI and SCK output, all others input */
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_SS);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);

	SPI_dataBuffer = dataBuffer;

	PORT_SPI |= (1<<DD_SS);
}

char SPI_MasterTransmit(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	{
		// do nothing
	}

//	USART_Transmit(cData);

	return SPDR;
}

char* SPI_MasterTransmitMulti(uint8_t byte_cnt, char* data)
{
	PORT_SPI &= ~(1<<DD_SS);
	_delay_us(1);

	for (uint8_t cntr=0;cntr<byte_cnt;cntr++)
	{
		SPI_dataBuffer[cntr] = SPI_MasterTransmit(data[cntr]);
	}

	_delay_us(1);
	PORT_SPI |= (1<<DD_SS);

	return SPI_dataBuffer;
}
