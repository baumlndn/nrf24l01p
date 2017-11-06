/*
 * nrf24l01p.c
 *
 *  Created on: 05.11.2017
 *      Author: baumlndn
 */


#include "nrf24l01p.h"
#include "usart.h"

uint8_t nrf24l01p_state = NRF_STATE_IDLE;
char * rx_buf_ptr;

void NRF24L01P_Init(void)
{
	char tmp_addr[5] = NRF24L01P_ADDR;
	SPI_MasterInit(NRF24L01P_PL);
	NRF24L01P_DDR_CE |= (1<<NRF24L01P_DD_CE);

#ifdef NRF24L01P_MODE_RX
	NRF24L01P_WriteReg(CONFIG,(1<<EN_CRC)|(1<<PWR_UP)|(1<<PRIM_RX));
#else
	NRF24L01P_WriteReg(CONFIG,(1<<EN_CRC)|(1<<PWR_UP));
#endif
	NRF24L01P_WriteReg(RX_PW_P0,NRF24L01P_PW);
	NRF24L01P_WriteRegAddr(TX_ADDR,&tmp_addr[0]);
	NRF24L01P_WriteRegAddr(RX_ADDR_P0,&tmp_addr[0]);

#if NRF24L01P_INT == INT0
	EICRA &= ~(0b11 << ISC00);
	EIMSK |= (1<<INT0);
#endif

#ifdef NRF24L01P_MODE_RX
	/* activate NRF RX */
	NRF24L01P_PORT_CE |= (1<<NRF24L01P_DD_CE);
#endif
}

void NRF24L01P_WriteReg(nrf24l01p_reg_e reg, char data)
{
	char tmp_data[2];
	tmp_data[0] = (0x20 | reg);
	tmp_data[1] = data;
	(void) SPI_MasterTransmitMulti(2,&tmp_data[0]);
}

void NRF24L01P_WriteRegAddr(nrf24l01p_reg_e reg, char* data)
{
	char tmp_data[6];
	tmp_data[0] = (0x20 | reg);

	for (uint8_t addr_cnt=0;addr_cnt<5;addr_cnt++)
	{
		tmp_data[addr_cnt+1] = data[addr_cnt];
	}
	(void) SPI_MasterTransmitMulti(6,&tmp_data[0]);
}

char NRF24L01P_ReadReg(nrf24l01p_reg_e reg, char data)
{
	char tmp_data[2];
	char *ret_data;
	tmp_data[0] = reg;
	tmp_data[1] = data;
	ret_data = SPI_MasterTransmitMulti(2,&tmp_data[0]);
	return ret_data[1];
}

void NRF24L01P_WritePayload(char * arg_pl)
{
	char tmp_data[NRF24L01P_PW+1];
	tmp_data[0] = 0b10100000;

	for (uint8_t pl_cnt=0;pl_cnt<NRF24L01P_PW;pl_cnt++)
	{
		tmp_data[pl_cnt+1] = arg_pl[pl_cnt];
	}

	(void) SPI_MasterTransmitMulti(NRF24L01P_PW+1,&tmp_data[0]);
}

char* NRF24L01P_ReadPayload(void)
{
	char tmp_data[NRF24L01P_PW+1];
//	char *ret_data;
	tmp_data[0] = 0b01100001;

	for (uint8_t pl_cnt=0;pl_cnt<NRF24L01P_PW;pl_cnt++)
	{
		tmp_data[pl_cnt+1] = 0;
	}

	return SPI_MasterTransmitMulti(NRF24L01P_PW+1,&tmp_data[0]);
}

void NRF24L01P_Transmit(char * arg_pl)
{
	while (nrf24l01p_state == NRF_STATE_PENDING)
	{
		/* Do nothing */
	}

	nrf24l01p_state = NRF_STATE_PENDING;
	NRF24L01P_WritePayload(arg_pl);

	NRF24L01P_PORT_CE |= (1<<NRF24L01P_DD_CE);
	_delay_us(100);
	NRF24L01P_PORT_CE &= ~(1<<NRF24L01P_DD_CE);
}

char* NRF24L01P_getData(void)
{
	for (uint8_t i=0;i<NRF24L01P_PW;i++)
	{
		NRF24L01P_RX_BUF[i] = rx_buf_ptr[i+1];
	}
	nrf24l01p_state = NRF_STATE_IDLE;
	return &NRF24L01P_RX_BUF[0];
}

uint8_t NRF24L01P_getState(void)
{
	return nrf24l01p_state;
}

#if NRF24L01P_INT == INT0
ISR(INT0_vect)
{
	char tmp_spi = NRF24L01P_ReadReg(STATUS,0x00);

	switch (tmp_spi>>4)
	{
	case 0x01:
		nrf24l01p_state = NRF_STATE_FAILED;
		break;
	case 0x02:
		nrf24l01p_state = NRF_STATE_DONE;
		break;
	case 0x04:
		nrf24l01p_state = NRF_STATE_NEW_RX;
		rx_buf_ptr = NRF24L01P_ReadPayload();
		break;
	default:
		break;
	}

	NRF24L01P_WriteReg(STATUS,0x70);
}
#endif
