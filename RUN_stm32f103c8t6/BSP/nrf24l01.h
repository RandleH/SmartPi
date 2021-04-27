#ifndef _NRF24L01_H
#define _NRF24L01_H 

#include <stdbool.h>
#include <stdint.h>

extern uint8_t NRF24L01_TX_Addr[5];
extern uint8_t NRF24L01_RX_Addr[5];

void NRF24L01_Init   ( void );
bool NRF24L01_Check  ( void );
#endif




