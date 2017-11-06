/*
 * main.c
 *
 *  Created on: 11.02.2015
 *      Author: baumlndn
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "config.h"
#include "usart.h"
#include "nrf24l01p.h"


int main()
{
	unsigned char tmp_spi;
	uint8_t usart_cnt = 0;

	/* Initialize USART */
	USART_Init(51u);

	/* Initialize NRF24L01+ */
	NRF24L01P_Init();

	/* activate global interrupt */
	sei();

	while (1)
	{
		/* Send byte via NRF */
#ifndef NRF24L01P_MODE_RX
		char test_payload[NRF24L01P_PW];

		for (uint8_t i=0;i<NRF24L01P_PW;i++)
		{
			test_payload[i] = usart_cnt;
		}

		NRF24L01P_Transmit(&test_payload[0]);
#endif

		tmp_spi = NRF24L01P_ReadReg(STATUS,0x00);
		USART_Transmit(tmp_spi);

		if (NRF24L01P_getState() == NRF_STATE_NEW_RX)
		{
			char * tmp_rx;
			tmp_rx = NRF24L01P_getData();
			for (uint8_t i=1;i<NRF24L01P_PW;i++)
			{
				USART_Transmit(tmp_rx[i]);
			}
		}

		/* Send a byte via USART per second */
		USART_Transmit( (unsigned char) usart_cnt );
		usart_cnt++;
		_delay_ms(1000);
	}
	
	return 0;
}
