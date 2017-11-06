/*
 * nrf24l01p.h
 *
 *  Created on: 05.11.2017
 *      Author: baumlndn
 */

#ifndef NRF24L01P_H_
#define NRF24L01P_H_

#include <util/delay.h>
#include <avr/interrupt.h>
#include "config.h"
#include "spi.h"

enum nrf24l01p_reg_t
{
	CONFIG			= 0x00,
	EN_AA			= 0x01,
	EN_RXADDR		= 0x02,
	SETUP_AW		= 0x03,
	SETUP_RETR		= 0x04,
	RF_CH			= 0x05,
	RF_SETUP		= 0x06,
	STATUS			= 0x07,
	OBSERVE_TX		= 0x08,
	RPD				= 0x09,
	RX_ADDR_P0		= 0x0A,
	RX_ADDR_P1		= 0x0B,
	RX_ADDR_P2		= 0x0C,
	RX_ADDR_P3		= 0x0D,
	RX_ADDR_P4		= 0x0E,
	RX_ADDR_P5		= 0x0F,
	TX_ADDR			= 0x10,
	RX_PW_P0		= 0x11,
	RX_PW_P1		= 0x12,
	RX_PW_P2		= 0x13,
	RX_PW_P3		= 0x14,
	RX_PW_P4		= 0x15,
	RX_PW_P5		= 0x16,
	FIFO_STATUS		= 0x17,
	DYNPD			= 0x1C
};

typedef char nrf24l01p_reg_e;

enum nrf24l01p_config_t
{
	PRIM_RX,
	PWR_UP,
	CRCO,
	EN_CRC,
	MASK_MAX_RT,
	MASK_TX_DS,
	MASK_RX_DR
};

enum nrf24l01p_sm_t
{
	NRF_STATE_IDLE,
	NRF_STATE_PENDING,
	NRF_STATE_DONE,
	NRF_STATE_FAILED,
	NRF_STATE_NEW_RX
};

char NRF24L01P_PL[NRF24L01P_PW+1];
char NRF24L01P_RX_BUF[NRF24L01P_PW];

void NRF24L01P_Init(void);
void NRF24L01P_WriteReg(nrf24l01p_reg_e reg, char data);
void NRF24L01P_WriteRegAddr(nrf24l01p_reg_e reg, char* data);
char NRF24L01P_ReadReg(nrf24l01p_reg_e reg, char data);
void NRF24L01P_WritePayload(char * arg_pl);
char* NRF24L01P_ReadPayload(void);
void NRF24L01P_Transmit(char * arg_pl);
char* NRF24L01P_getData(void);
uint8_t NRF24L01P_getState(void);

#endif /* NRF24L01P_H_ */
