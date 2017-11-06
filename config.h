/*
 * config.h
 *
 *  Created on: 12.02.2015
 *      Author: baumlndn
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define DDR_SPI				DDRB
#define PORT_SPI			PORTB
#define DD_MOSI				3
#define DD_SCK				5
#define DD_SS				2

#define NRF24L01P_PW		4
#define NRF24L01P_DDR_CE	DDRB
#define NRF24L01P_PORT_CE	PORTB
#define NRF24L01P_DD_CE		1
#define NRF24L01P_ADDR		{ 'T' , 'e' , 'm' , 'p' , '1' }
#define NRF24L01P_MODE_RX	1
#define NRF24L01P_INT		INT0

#define DELAY_MS_DEFAULT	500
#define DEBUG				1

#define DS1820_DDR			DDRD
#define DS1820_PORT			PORTD
#define DS1820_PIN			PIND
#define DS1820_BIT			PD7

enum ds1820_t
{
	DS18S20,
	DS18B20
};

//#define DS18S20

#endif /* CONFIG_H_ */
