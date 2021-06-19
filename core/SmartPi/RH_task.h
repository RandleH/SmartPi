#ifndef _RH_TASK_H
#define _RH_TASK_H 


#include "FreeRTOS.h"
#include "event_groups.h"

#include "RH_common.h"

#include "BLK_data.h"

#include "./project/game/manila.h"

typedef enum{
    ROOT                              , //  /
    ROOT_Hardware                     , //  //
    ROOT_Game                         , //  //
    ROOT_About                        , //  //
    
    ROOT_Hardware_                    , //  ---
    ROOT_Hardware_NRF24L01            , //  ///
    ROOT_Hardware_JoyStick            , //  ///      
    ROOT_Hardware_LED                 , //  ///
    ROOT_Hardware_Bluetooth           , //  ///
    ROOT_Hardware_Beeper              , //  ///
    
    ROOT_Hardware_NRF24L01_           , //  ----
    ROOT_Hardware_NRF24L01_RecvCFG    , //  ////
    ROOT_Hardware_NRF24L01_TranCFG    , //  ////
    ROOT_Hardware_NRF24L01_ACK        , //  ////
    ROOT_Hardware_NRF24L01_RFCH       , //  ////

    ROOT_Hardware_NRF24L01_RecvCFG_     , //  -----
    ROOT_Hardware_NRF24L01_RecvCFG_Pipe , //  /////
    ROOT_Hardware_NRF24L01_RecvCFG_Addr , //  /////
    
    ROOT_Game_                        , //  ---
    ROOT_Game_Manila                  , //  ///

    ROOT_Game_Manila_                 , //  ----
    ROOT_Game_Manila_ShipDiagram      , //  ////
    ROOT_Game_Manila_ArrivedA         , //  ////
    ROOT_Game_Manila_ArrivedB         , //  ////
    ROOT_Game_Manila_ArrivedC         , //  ////
    ROOT_Game_Manila_DrownA           , //  ////
    ROOT_Game_Manila_DrownB           , //  ////
    ROOT_Game_Manila_DrownC           , //  ////

}E_TaskID_t;

struct __SmartPiService_t{
    E_TaskID_t          serv_ID;         // 当前业务号
    BLK_SRCT(Stack)*    serv_ID_Stack;   // 业务栈
    volatile uint32_t   serv_ID_tmp;     // 临时预选业务号

    int8_t              numOfNextNodes;  

    uint8_t*            radio_data;     
    
    bool                enter;
    volatile bool       exit;
    
    int8_t            cache_task_num;    
    TaskHandle_t*     cache_task_handle;
    
    void              (*deleteSubtask)(void);
};
typedef struct __SmartPiService_t __SmartPiService_t;
extern __SmartPiService_t SmartPi;

typedef enum{
    kHWEvent_JoySitck_Pressed    = (1<<0),   // 0
    kHWEvent_JoySitck_Up         = (1<<1),   // 1
    kHWEvent_JoySitck_Down       = (1<<2),   // 2
    kHWEvent_JoySitck_Left       = (1<<3),   // 3
    kHWEvent_JoySitck_Right      = (1<<4),   // 4

    kHWEvent_NRF24L01_Offline    = (1<<5),   // 5
    kHWEvent_NRF24L01_RecvReady  = (1<<6),   // 6
    kHWEvent_NRF24L01_RecvFailed = (1<<7),   // 7
    kHWEvent_NRF24L01_RecvDone   = (1<<8),   // 8 
    kHWEvent_NRF24L01_SendReady  = (1<<9),   // 9
    kHWEvent_NRF24L01_SendFailed = (1<<10),  // 10
    kHWEvent_NRF24L01_SendDone   = (1<<11),  // 11

    kHWEvent_USART_Recved        = (1<<12),  // 12
    kHWEvent_USART_Sent          = (1<<13),  // 13

    kHWEvent_USB_PlugIn          = (1<<14),  // 14
    kHWEvent_USB_PlugOut         = (1<<15),  // 15
    
}E_HWEvent_t;

typedef enum{
    kSWEvent_StateChanged      = (1<<0),   // 0 
    kSWEvent_UI_Finished       = (1<<1),   // 1
    kSWEvent_CTRL_Finished     = (1<<2),   // 2
    kSWEvent_ParentTaskCall    = (1<<3),   // 3
    kSWEvent_ChildTaskCall     = (1<<4),   // 4
    kSWEvent_UI_Refreashed     = (1<<5),   // 5

}E_SWEvent_t;


extern EventGroupHandle_t EGHandle_Hardware;
extern EventGroupHandle_t EGHandle_Software;


void __Event_init  ( void );
void __Task_init   ( void );

#endif



