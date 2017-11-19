/*
 * main.c
 *
 *  Created on: 11.02.2015
 *      Author: baumlndn
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "config.h"
#include "usart.h"
#include "nrf24l01p.h"
#include "bme280.h"


int main()
{
//	unsigned char tmp_spi;
	uint8_t usart_cnt = 0;

	/* Initialize USART */
	USART_Init(51u);

	/* Initialize NRF24L01+ */
//	NRF24L01P_Init();

	/* Initialize BME280 */
	BME280_Init();

	/* activate global interrupt */
	sei();

	char tmp_welcome[] = "Hallo";
	for (uint8_t i=0;i<strlen(tmp_welcome);i++)
	{
		USART_Transmit(tmp_welcome[i]);
	}

	int32_t tmp_temp;
	uint32_t tmp_press, tmp_hum;
	BME280_measure(&tmp_temp,&tmp_press,&tmp_hum);

	USART_Transmit(tmp_temp >> 24);
	USART_Transmit(tmp_temp >> 16);
	USART_Transmit(tmp_temp >>  8);
	USART_Transmit(tmp_temp >>  0);

	USART_Transmit(tmp_press >> 24);
	USART_Transmit(tmp_press >> 16);
	USART_Transmit(tmp_press >>  8);
	USART_Transmit(tmp_press >>  0);

	USART_Transmit(tmp_hum >> 24);
	USART_Transmit(tmp_hum >> 16);
	USART_Transmit(tmp_hum >>  8);
	USART_Transmit(tmp_hum >>  0);



	while (1)
	{
		/* Send byte via NRF */
#ifndef NRF24L01P_MODE_RX
//		char test_payload[NRF24L01P_PW];

		for (uint8_t i=0;i<NRF24L01P_PW;i++)
		{
//			test_payload[i] = usart_cnt;
		}

//		NRF24L01P_Transmit(&test_payload[0]);
#else
		if (NRF24L01P_getState() == NRF_STATE_NEW_RX)
		{
			char * tmp_rx;
			tmp_rx = NRF24L01P_getData();
			for (uint8_t i=1;i<NRF24L01P_PW;i++)
			{
				USART_Transmit(tmp_rx[i]);
			}
		}
#endif
		/* Send a byte via USART per second */
//		USART_Transmit( (unsigned char) NRF24L01P_getState() );
		usart_cnt++;
		_delay_ms(1000);

//		NRF24L01P_Proc();
	}
	
	return 0;
}
