#ifndef _NRF24L01_H
#define _NRF24L01_H 

#include <stdbool.h>
#include <stdint.h>

extern uint8_t NRF24L01_TX_Addr[5];
extern uint8_t NRF24L01_RX_Addr[5];

#include "delay.h"
#define NRF24L01_delay_ms(ms)    delay_ms(ms)

void      NRF24L01_init   ( void );
bool      NRF24L01_check  ( void );
void      NRF24L01_tx     ( void );
void      NRF24L01_rx     ( void );
uint8_t   NRF24L01_send   ( uint8_t *data, uint8_t len );
uint8_t   NRF24L01_recv   ( uint8_t *buf , uint8_t len );


void      NRF24L01_setTXAddr  (                uint8_t *addr, uint8_t len );
void      NRF24L01_setRXAddr  ( uint8_t  pipe, uint8_t *addr, uint8_t len );



#endif




