#ifndef _RH_TASK_H
#define _RH_TASK_H 


#include "FreeRTOS.h"
#include "event_groups.h"

#include "RH_common.h"

struct __SmartPiService_t{
    uint32_t          serv_ID;
    volatile uint32_t serv_ID_tmp;
    
    bool              enter;
    volatile bool     exit;
    
    int8_t            cache_task_num;
    TaskHandle_t*     cache_task_handle;
};
typedef struct __SmartPiService_t __SmartPiService_t;
extern __SmartPiService_t SmartPi;

typedef enum{
    kHWEvent_JoySitck_Pressed  = (1<<0),   // 0
    kHWEvent_JoySitck_Up       = (1<<1),   // 1
    kHWEvent_JoySitck_Down     = (1<<2),   // 2
    kHWEvent_JoySitck_Left     = (1<<3),   // 3
    kHWEvent_JoySitck_Right    = (1<<4),   // 4

    kHWEvent_NRF24L01_Offline  = (1<<5),   // 5
    kHWEvent_NRF24L01_Recved   = (1<<6),   // 6
    kHWEvent_NRF24L01_Sent     = (1<<7),   // 7

    kHWEvent_USART_Recved      = (1<<8),   // 8
    kHWEvent_USART_Sent        = (1<<9),   // 9

    kHWEvent_USB_PlugIn        = (1<<10),  // 10
    kHWEvent_USB_PlugOut       = (1<<11),  // 11
    
}E_HWEvent_t;

typedef enum{
    kSWEvent_StateChanged      = (1<<0),   // 0 
}E_SWEvent_t;


extern EventGroupHandle_t EGHandle_Hardware;
extern EventGroupHandle_t EGHandle_Software;


void __Event_init  ( void );
void __Task_init   ( void );

#endif



