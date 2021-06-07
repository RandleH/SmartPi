#ifndef _NRF24L01_H
#define _NRF24L01_H 

#include <stdbool.h>
#include <stdint.h>

#include "stm32f10x_conf.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"

#define SPI_PIN_MOSI             GPIO_Pin_15 
#define SPI_PIN_MISO             GPIO_Pin_14 
#define SPI_PIN_SCL              GPIO_Pin_13 
#define SPI_GPIO                 GPIOB

#define NRF_PIN_CS               GPIO_Pin_4
#define NRF_GPIO_CS              GPIOB  

#define NRF_PIN_CE               GPIO_Pin_3
#define NRF_GPIO_CE              GPIOB

#define NRF_PIN_IRQ              GPIO_Pin_5 
#define NRF_GPIO_IRQ             GPIOB
#define NRF_PORTSRC_IRQ          GPIO_PortSourceGPIOB, GPIO_PinSource5
#define NRF_IRQn                 EXTI9_5_IRQn
#define NRF_LINE                 EXTI_Line5

#define SPI_BASE                 SPI2


#define RCC_ENABLE               do{ RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | \
                                                             RCC_APB2Periph_AFIO  , ENABLE );\
                                     RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2  , ENABLE );\
                                 } while(0)

#include "delay.h"
#define   NRF24L01_DELAY_MS(ms)    delay_ms(ms)
#define   NRF24L01_ASSERT(expr)    RH_ASSERT(expr)
#define   NRF24L01_MALLOC(x)       RH_MALLOC(x)
#define   NRF24L01_CALLOC(x,s)     RH_CALLOC(x,s)
#define   NRF24L01_FREE(x)         RH_FREE(x)     

struct{
	
}                                 

void      NRF24L01_init   ( void );
bool      NRF24L01_check  ( void );
void      NRF24L01_tx     ( void );
void      NRF24L01_rx     ( void );
uint8_t   NRF24L01_send   ( uint8_t *data, uint8_t len );
uint8_t   NRF24L01_recv   ( uint8_t *buf , uint8_t len );

void      NRF24L01_autoACK    ( bool cmd );
void      NRF24L01_enCRC      ( bool cmd );


void      NRF24L01_setTXAddr  (                uint8_t *addr, uint8_t len );
void      NRF24L01_setRXAddr  ( uint8_t  pipe, uint8_t *addr, uint8_t len );

const uint8_t* NRF24L01_getTXAddr( uint8_t *byteCnt );
const uint8_t* NRF24L01_getRXAddr( uint8_t *byteCnt );


#endif




