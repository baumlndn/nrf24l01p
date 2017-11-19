/*
 * usart.c
 *
 *  Created on: 11.02.2015
 *      Author: baumlndn
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "i2c.h"
#include <util/twi.h>
#include "usart.h"

void I2C_Init( unsigned int twbr)
{
	TWBR = twbr;
}

uint8_t I2C_Transmit( char device_id, char address, unsigned char * data, uint8_t length )
{
	uint8_t tmpError = 0;

	/* Send START condition */
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	/* START condition transmitted */
	while (!(TWCR & (1<<TWINT)))
	{
		/* Do nothing */
	}

	/* Check TWI status */
	if ((TWSR & 0xF8) != TW_START)
	{
		tmpError = 0x01;
	}

	if (!tmpError)
	{
		/* Send Slave write address */
		TWDR = (device_id << 1);
		TWCR = (1<<TWINT) | (1<<TWEN);

		/* Wait for transmission to finish */
		while (!(TWCR & (1<<TWINT)))
		{
			/* do nothing */
		}

		/* Check TWI status */
		if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
		{
			tmpError = 0x02;
		}
	}

	/* Load address */
	if (!tmpError)
	{
		TWDR = address;
		TWCR = (1<<TWINT) | (1<<TWEN);

		/* Wait for transmission to finish */
		while (!(TWCR & (1<<TWINT)))
		{
			/* Do nothing */
		}

		/* Check TWI status */
		if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
		{
			tmpError = 0x04;
		}
	}

	/* Load data */
	for (uint8_t idx=0;idx<length;idx++)
	{
		if (!tmpError)
		{
			TWDR = data[idx];
			TWCR = (1<<TWINT) | (1<<TWEN);

			/* Wait for transmission to finish */
			while (!(TWCR & (1<<TWINT)))
			{
				/* Do nothing */
			}

			/* Check TWI status */
			if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
			{
				tmpError = 0x08;
			}
		}
	}

	/* Transmit STOP condition */
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

	return tmpError;
}

unsigned char I2C_Receive( char device_id, char address, unsigned char * data, uint8_t length )
{
	uint8_t tmpError = 0;

	/* Send START condition */
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	/* START condition transmitted */
	while (!(TWCR & (1<<TWINT)))
	{
		/* Do nothing */
	}

	/* Check TWI status */
	if ((TWSR & 0xF8) != TW_START)
	{
		tmpError = 0x01;
	}

	if (!tmpError)
	{
		/* Send Slave write address */
		TWDR = (device_id << 1);
		TWCR = (1<<TWINT) | (1<<TWEN);

		/* Wait for transmission to finish */
		while (!(TWCR & (1<<TWINT)))
		{
			/* do nothing */
		}

		/* Check TWI status */
		if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
		{
			tmpError = 0x02;
		}
	}

	/* Load address */
	if (!tmpError)
	{
		TWDR = address;
		TWCR = (1<<TWINT) | (1<<TWEN);

		/* Wait for transmission to finish */
		while (!(TWCR & (1<<TWINT)))
		{
			/* Do nothing */
		}

		/* Check TWI status */
		if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
		{
			tmpError = 0x04;
		}
	}

	/* Send START condition */
	if (!tmpError)
	{
		TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

		/* START condition transmitted */
		while (!(TWCR & (1<<TWINT)))
		{
			/* Do nothing */
		}

		/* Check TWI status */
		if ((TWSR & 0xF8) != TW_REP_START)
		{
			tmpError = 0x08;
		}
	}

	/* Send Slave read address */
	if (!tmpError)
	{
		TWDR = (device_id << 1) | 0x01;
		TWCR = (1<<TWINT) | (1<<TWEN);

		/* Wait for transmission to finish */
		while (!(TWCR & (1<<TWINT)))
		{
			/* do nothing */
		}

		/* Check TWI status */
		if ((TWSR & 0xF8) != TW_MR_SLA_ACK)
		{
			tmpError = 0x10;
		}
	}

	/* Load data */
	for (uint8_t idx=0;idx<length;idx++)
	{
		if (!tmpError)
		{
			TWDR = 0x00;

			if ( (idx+1) != length)
			{
				TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
			}
			else
			{
				TWCR = (1<<TWINT) | (1<<TWEN);
			}

			/* Wait for transmission to finish */
			while (!(TWCR & (1<<TWINT)))
			{
				/* Do nothing */
			}

			/* Check TWI status */
			if ( (idx+1) != length)
			{
				if ((TWSR & 0xF8) != TW_MR_DATA_ACK)
				{
					tmpError = 0x20;
				}
				else
				{
					data[idx] = TWDR;
				}
			}
			else
				if ((TWSR & 0xF8) != TW_MR_DATA_NACK)
				{
					tmpError = 0x40;
				}
				else
				{
					data[idx] = TWDR;
					TWCR = (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);
				}
		}
	}

	/* Transmit STOP condition */
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

	return tmpError;
}
