
#include "RH_task.h"
#include "SMP_delay.h"

#include "GLU_glucoo.h"
#include "RH_color.h"

#include "joystick.h"
#include "nrf24l01.h"

/*====================================================================
 * SmartPi类内方法
=====================================================================*/
static inline void this_deleteSubtask(void){
    for( int i=0; i<SmartPi.cache_task_num; i++ ){
        vTaskDelete( SmartPi.cache_task_handle[i] );
    }
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
}

/*====================================================================
 * 静态事件句柄和事件组汇总
=====================================================================*/
StaticEventGroup_t EG_Hardware          = {0};
EventGroupHandle_t EGHandle_Hardware    = NULL;
StaticEventGroup_t EG_Software          = {0};
EventGroupHandle_t EGHandle_Software    = NULL;

/*====================================================================
 * 静态任务句柄汇总
=====================================================================*/
TaskHandle_t       THandle_main         = NULL;
TaskHandle_t       THandle_led_heart    = NULL;
TaskHandle_t       THandle_radio_check  = NULL;

/*====================================================================
 * SmartPi 业务结构体
=====================================================================*/
__SmartPiService_t SmartPi           = {.serv_ID = ROOT, .deleteSubtask = this_deleteSubtask};

/*====================================================================
 * FreeRTOS事件配置
=====================================================================*/
void __Event_init( void ){
    EGHandle_Hardware = xEventGroupCreateStatic(&EG_Hardware); // xEventGroupCreateStatic
    EGHandle_Software = xEventGroupCreateStatic(&EG_Software);
#ifdef RH_DEBUG
    RH_ASSERT( EGHandle_Hardware );
    RH_ASSERT( EGHandle_Software );
#endif
}

/*====================================================================
 * FreeRTOS空闲任务配置
=====================================================================*/
static StaticTask_t   Idle_Task_TCB;
static StackType_t    Idle_Task_Stack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer, StackType_t  ** ppxIdleTaskStackBuffer, uint32_t * pulIdleTaskStackSize   ){
    *ppxIdleTaskTCBBuffer    = &Idle_Task_TCB;             /* 任务控制块内存 */ 
    *ppxIdleTaskStackBuffer  = Idle_Task_Stack;            /* 任务堆栈内存 */ 
    *pulIdleTaskStackSize    = configMINIMAL_STACK_SIZE;   /* 任务堆栈大小 */
}

/*====================================================================
 * FreeRTOS定时器任务配置
=====================================================================*/
static StaticTask_t   Timer_Task_TCB;
static StackType_t    Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer, StackType_t  ** ppxTimerTaskStackBuffer, uint32_t * pulTimerTaskStackSize  ){
    *ppxTimerTaskTCBBuffer    = &Timer_Task_TCB;             /* 任务控制块内存 */ 
    *ppxTimerTaskStackBuffer  = Timer_Task_Stack;            /* 任务堆栈内存 */ 
    *pulTimerTaskStackSize    = configMINIMAL_STACK_SIZE;    /* 任务堆栈大小 */
}


/*====================================================================
 * SmartPi 子任务
=====================================================================*/



#ifdef MAKE_TASK
#undef MAKE_TASK
#endif
#define MAKE_TASK( CLASS, ID, TYPE ) __##CLASS##_##ID##_##TYPE

extern void MAKE_TASK( subtask, ROOT, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, ROOT, UI   ) ( void* param );

extern void MAKE_TASK( subtask, ROOT_Hardware, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, ROOT_Hardware, UI   ) ( void* param );

extern void MAKE_TASK( subtask, ROOT_Game, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, ROOT_Game, UI   ) ( void* param );

extern void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01, UI   ) ( void* param );

extern void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RecvCFG, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RecvCFG, UI   ) ( void* param );

extern void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RecvCFG_Addr, UI   ) ( void* param );
extern void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RecvCFG_Addr, CTRL ) ( void* param );

// extern void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RXAddress, CTRL ) ( void* param );
// extern void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RXAddress, UI   ) ( void* param );

extern void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RFCH, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RFCH, UI   ) ( void* param );

extern void MAKE_TASK( subtask, ROOT_Hardware_JoyStick, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, ROOT_Hardware_JoyStick, UI   ) ( void* param );

extern void MAKE_TASK( subtask, ROOT_Hardware_LED, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, ROOT_Hardware_LED, UI   ) ( void* param );

extern void MAKE_TASK( subtask, ROOT_Hardware_Beeper, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, ROOT_Hardware_Beeper, UI   ) ( void* param );

extern void MAKE_TASK( subtask, ROOT_Game_Manila, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, ROOT_Game_Manila, UI   ) ( void* param );

extern void MAKE_TASK( subtask, ROOT_Game_Manila_ShipDiagram, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, ROOT_Game_Manila_ShipDiagram, UI   ) ( void* param );

extern void MAKE_TASK( subtask, Default   , CTRL ) ( void* param );
extern void MAKE_TASK( subtask, Default   , UI   ) ( void* param );

/*====================================================================
 * SmartPi 主任务
=====================================================================*/
#define I_AM_CHILD      do{ \
                            xEventGroupSetBits  ( EGHandle_Software, kSWEvent_ChildTaskCall  );\
                            xEventGroupClearBits( EGHandle_Software, kSWEvent_ParentTaskCall );\
                        } while(0)

#define I_AM_PARENT     do{ \
                            xEventGroupSetBits  ( EGHandle_Software, kSWEvent_ParentTaskCall );\
                            xEventGroupClearBits( EGHandle_Software, kSWEvent_ChildTaskCall  );\
                        } while(0)

#define IS_CHILDCALL    ( kSWEvent_ChildTaskCall  == xEventGroupGetBits( EGHandle_Software ) )
#define IS_PARENTCALL   ( kSWEvent_ParentTaskCall == xEventGroupGetBits( EGHandle_Software ) )                        

static StaticTask_t   Task_TCB_main;
static StackType_t    Task_Stack_main[384];
void __TaskStatic_main( void* param ){
    SmartPi.enter = true;
    SmartPi.exit  = false;
    while(1){
        switch( SmartPi.serv_ID ){
case ROOT:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = ROOT;
    SmartPi.serv_ID_tmp    = 1;
    SmartPi.numOfNextNodes = 3;

    taskENTER_CRITICAL();
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = RH_MALLOC( SmartPi.cache_task_num*sizeof(TaskHandle_t) );

    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Left    |
                                             kHWEvent_JoySitck_Right   |
                                             kHWEvent_JoySitck_Down    |
                                             kHWEvent_JoySitck_Up      |
                                             kHWEvent_JoySitck_Pressed );

    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );

    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT, UI   ) , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT, CTRL ) , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();

/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                   pdTRUE,          // 清除该位
                                   pdTRUE,          // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待
    
/*====================================================================
 * Clear  清除任务
=====================================================================*/
    taskENTER_CRITICAL();
    for( int i=0; i<SmartPi.cache_task_num; i++ ){
        vTaskDelete( SmartPi.cache_task_handle[i] );
    }
    RH_FREE( SmartPi.cache_task_handle );
    taskEXIT_CRITICAL();
    
/*====================================================================
 * Next  进入下一业务
=====================================================================*/
    SmartPi.serv_ID         += SmartPi.serv_ID_tmp;
    SmartPi.cache_task_num   = 0;
    
    break;
}

case ROOT_Hardware:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = ROOT_Hardware;
    SmartPi.serv_ID_tmp    = 1;
    SmartPi.numOfNextNodes = 5;

    taskENTER_CRITICAL();
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = alloca( SmartPi.cache_task_num*sizeof(TaskHandle_t) );

    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Left    |
                                             kHWEvent_JoySitck_Right   |
                                             kHWEvent_JoySitck_Down    |
                                             kHWEvent_JoySitck_Up      |
                                             kHWEvent_JoySitck_Pressed );

    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );

    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware, UI   ) , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware, CTRL ) , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();

/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                   pdTRUE,          // 清除该位
                                   pdTRUE,          // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待

/*====================================================================
 * Clear  清除任务
=====================================================================*/
    taskENTER_CRITICAL();
    SmartPi.deleteSubtask();
    taskEXIT_CRITICAL();
    
/*====================================================================
 * Next  进入下一业务
=====================================================================*/
    if( SmartPi.serv_ID_tmp == 0 )
        SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(uint32_t)BLK_FUNC( Stack, pop )( SmartPi.serv_ID_Stack );
    else{
        BLK_FUNC( Stack, push )( SmartPi.serv_ID_Stack, (void*)(SmartPi.serv_ID) );
        SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(ROOT_Hardware_ + SmartPi.serv_ID_tmp);
    }
    break;
}
                
case ROOT_Game:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID           = ROOT_Game;
    SmartPi.serv_ID_tmp       = 1;
    SmartPi.numOfNextNodes    = 1;
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = alloca( SmartPi.cache_task_num*sizeof(TaskHandle_t) );
    
    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Left    |
                                             kHWEvent_JoySitck_Down    |
                                             kHWEvent_JoySitck_Up      |
                                             kHWEvent_JoySitck_Pressed );
    
    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );
    taskENTER_CRITICAL();
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Game, UI   ) , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Game, CTRL ) , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();
    
/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                            pdTRUE,          // 清除该位
                                            pdTRUE,          // 等待所有指定的Bit
                                            portMAX_DELAY ); // 永久等待
    
/*====================================================================
 * Clear  清除任务
=====================================================================*/
    taskENTER_CRITICAL();
    SmartPi.deleteSubtask();
    taskEXIT_CRITICAL();
    
/*====================================================================
 * Next  进入下一业务
=====================================================================*/
    if( SmartPi.serv_ID_tmp == 0 )
        SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(uint32_t)BLK_FUNC( Stack, pop )( SmartPi.serv_ID_Stack );
    else{
        BLK_FUNC( Stack, push )( SmartPi.serv_ID_Stack, (void*)(SmartPi.serv_ID) );
        SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(ROOT_Game_ + SmartPi.serv_ID_tmp);

        // 进入下一业务, 这是一个父任务, 应该告知为 父任务Call
        xEventGroupSetBits   ( EGHandle_Software, kSWEvent_ParentTaskCall );
        xEventGroupClearBits ( EGHandle_Software, kSWEvent_ChildTaskCall  );
    }
    
    break;
}
               
case ROOT_Hardware_NRF24L01:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = ROOT_Hardware_NRF24L01;
    SmartPi.serv_ID_tmp    = 1;
    SmartPi.numOfNextNodes = 4;

    taskENTER_CRITICAL();
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = alloca( SmartPi.cache_task_num*sizeof(TaskHandle_t) );

    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Left    |
                                             kHWEvent_JoySitck_Right   |
                                             kHWEvent_JoySitck_Down    |
                                             kHWEvent_JoySitck_Up      |
                                             kHWEvent_JoySitck_Pressed );

    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );

    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_NRF24L01, UI   ) , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_NRF24L01, CTRL ) , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();

/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                   pdTRUE,          // 清除该位
                                   pdTRUE,          // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待

/*====================================================================
 * Clear  清除任务
=====================================================================*/
    taskENTER_CRITICAL();
    SmartPi.deleteSubtask();
    taskEXIT_CRITICAL();

/*====================================================================
 * Next  进入下一业务
=====================================================================*/
    if( SmartPi.serv_ID_tmp == 0 )
        SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(uint32_t)BLK_FUNC( Stack, pop )( SmartPi.serv_ID_Stack );
    else{
        BLK_FUNC( Stack, push )( SmartPi.serv_ID_Stack, (void*)(SmartPi.serv_ID) );
        SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(ROOT_Hardware_NRF24L01_ + SmartPi.serv_ID_tmp);
    }
    
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
    break;
}

case ROOT_Hardware_NRF24L01_RecvCFG:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = ROOT_Hardware_NRF24L01_RecvCFG;
    SmartPi.serv_ID_tmp    = 1;
    SmartPi.numOfNextNodes = 2;

    taskENTER_CRITICAL();
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = alloca( SmartPi.cache_task_num*sizeof(TaskHandle_t) );

    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Left    |
                                             kHWEvent_JoySitck_Right   |
                                             kHWEvent_JoySitck_Down    |
                                             kHWEvent_JoySitck_Up      |
                                             kHWEvent_JoySitck_Pressed );

    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );

    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RecvCFG, UI   ) , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RecvCFG, CTRL ) , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();

/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                   pdTRUE,          // 清除该位
                                   pdTRUE,          // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待

/*====================================================================
 * Clear  清除任务
=====================================================================*/
    taskENTER_CRITICAL();
    SmartPi.deleteSubtask();
    taskEXIT_CRITICAL();

/*====================================================================
 * Next  进入下一业务
=====================================================================*/
    if( SmartPi.serv_ID_tmp == 0 )
        SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(uint32_t)BLK_FUNC( Stack, pop )( SmartPi.serv_ID_Stack );
    else{
        BLK_FUNC( Stack, push )( SmartPi.serv_ID_Stack, (void*)(SmartPi.serv_ID) );
        SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(ROOT_Hardware_NRF24L01_RecvCFG_ + SmartPi.serv_ID_tmp);
    }
    
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
    break;
}

case ROOT_Hardware_NRF24L01_RecvCFG_Addr:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID_tmp    = 0;
    SmartPi.numOfNextNodes = 0;
    
    taskENTER_CRITICAL();
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = alloca( SmartPi.cache_task_num*sizeof(TaskHandle_t) );

    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Pressed );

    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );
    struct{
        uint8_t     Addr[5];
        uint8_t     size;
        int         bucket;
        const char* text;
    }param;

    param.size = sizeof( param.Addr );
    if( SmartPi.serv_ID==ROOT_Hardware_NRF24L01_RecvCFG_Addr ){
        param.text = "RX Address:";
        
        RH_ASSERT( param.size == NRF24L01_getRXAddr( NRF24L01_PIPE_P0, param.Addr ) );

    }else{
        param.text = "TX Address:";
       
        RH_ASSERT( param.size == NRF24L01_getTXAddr( param.Addr) );
    }
    
    
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RecvCFG_Addr, UI   ) , NULL, 256, &param, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RecvCFG_Addr, CTRL ) , NULL, 128, &param, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();
    
/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                   pdTRUE,          // 清除该位
                                   pdTRUE,          // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待

/*====================================================================
 * Clear  清除任务
=====================================================================*/
    taskENTER_CRITICAL();
    for( int i=0; i<SmartPi.cache_task_num; i++ ){
        vTaskDelete( SmartPi.cache_task_handle[i] );
    }

    taskEXIT_CRITICAL();

/*====================================================================
 * Process 数据处理
=====================================================================*/
    if( SmartPi.serv_ID==ROOT_Hardware_NRF24L01_RecvCFG_Addr ){
        NRF24L01_setRXAddr( NRF24L01_PIPE_P0, param.Addr );
    }else{
        NRF24L01_setTXAddr( param.Addr );
    }
/*====================================================================
 * Next  进入下一业务
=====================================================================*/
    SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(uint32_t)BLK_FUNC( Stack, pop )( SmartPi.serv_ID_Stack );
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
    break;
}

case ROOT_Hardware_NRF24L01_RFCH:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID_tmp       = 0;
    SmartPi.numOfNextNodes    = 0;
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = alloca( SmartPi.cache_task_num*sizeof(TaskHandle_t) );
    struct{
        E_NRF24L01_Freq_t freq;
        E_NRF24L01_Freq_t max;
        E_NRF24L01_Freq_t min;
        int16_t           offset;
    }param;
    param.min    = NRF24L01_FREQ_2400MHz;
    param.max    = NRF24L01_FREQ_2525MHz;
    param.freq   = NRF24L01_getFreq();
    param.offset = 2400 + (uint8_t)NRF24L01_FREQ_2400MHz; 

    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Up | kHWEvent_JoySitck_Down | kHWEvent_JoySitck_Pressed );
    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );

    taskENTER_CRITICAL();
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RFCH, UI   ) , NULL, 256, &param, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RFCH, CTRL ) , NULL, 128, &param, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();

/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                   pdTRUE,          // 清除该位
                                   pdTRUE,          // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待

/*====================================================================
 * Clear  清除任务
=====================================================================*/
    SmartPi.deleteSubtask();

/*====================================================================
 * Process 数据处理
=====================================================================*/
    NRF24L01_setFreq( param.freq );
    //...//

/*====================================================================
 * Back  返回
=====================================================================*/
    SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(uint32_t)BLK_FUNC( Stack, pop )( SmartPi.serv_ID_Stack );
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;

    I_AM_CHILD; break;
}

                
case ROOT_Hardware_JoyStick:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = ROOT_Hardware_JoyStick;
    SmartPi.serv_ID_tmp    = 0;
    SmartPi.numOfNextNodes = 0;
    taskENTER_CRITICAL();
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = alloca( SmartPi.cache_task_num*sizeof(TaskHandle_t) );

    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Pressed );

    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_JoyStick, UI   ) , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_JoyStick, CTRL ) , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();
/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                   pdTRUE,          // 清除该位
                                   pdTRUE,          // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待

/*====================================================================
 * Clear  清除任务
=====================================================================*/
    taskENTER_CRITICAL();
    for( int i=0; i<SmartPi.cache_task_num; i++ ){
        vTaskDelete( SmartPi.cache_task_handle[i] );
    }
    vTaskResume( THandle_led_heart );
    taskEXIT_CRITICAL();
    
/*====================================================================
 * Back  返回
=====================================================================*/
    SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(uint32_t)BLK_FUNC( Stack, pop )( SmartPi.serv_ID_Stack );
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
    break;
}

case ROOT_Hardware_LED:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = ROOT_Hardware_LED;
    SmartPi.serv_ID_tmp    = 0;
    SmartPi.numOfNextNodes = 0;

    taskENTER_CRITICAL();
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = alloca( SmartPi.cache_task_num*sizeof(TaskHandle_t) );

    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Left    |
                                             kHWEvent_JoySitck_Right   |
                                             kHWEvent_JoySitck_Pressed );

    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );

    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_LED, UI   ) , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_LED, CTRL ) , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
    vTaskSuspend( THandle_led_heart );
    taskEXIT_CRITICAL();

/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                   pdTRUE,          // 清除该位
                                   pdTRUE,          // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待

/*====================================================================
 * Clear  清除任务
=====================================================================*/
    taskENTER_CRITICAL();
    for( int i=0; i<SmartPi.cache_task_num; i++ ){
        vTaskDelete( SmartPi.cache_task_handle[i] );
    }
    vTaskResume( THandle_led_heart );
    taskEXIT_CRITICAL();
    
/*====================================================================
 * Next  进入下一业务
=====================================================================*/
    SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(uint32_t)BLK_FUNC( Stack, pop )( SmartPi.serv_ID_Stack );
    
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
    break;
}
                
case ROOT_Hardware_Beeper:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = ROOT_Hardware_Beeper;
    SmartPi.serv_ID_tmp    = 0;
    SmartPi.numOfNextNodes = 0;

    taskENTER_CRITICAL();
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = alloca( SmartPi.cache_task_num*sizeof(TaskHandle_t) );

    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Left    |
                                             kHWEvent_JoySitck_Right   |
                                             kHWEvent_JoySitck_Pressed );

    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );

    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_Beeper, UI   ) , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Hardware_Beeper, CTRL ) , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();

/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                   pdTRUE,          // 清除该位
                                   pdTRUE,          // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待

/*====================================================================
 * Clear  清除任务
=====================================================================*/
    taskENTER_CRITICAL();
    for( int i=0; i<SmartPi.cache_task_num; i++ ){
        vTaskDelete( SmartPi.cache_task_handle[i] );
    }
    taskEXIT_CRITICAL();
    
/*====================================================================
 * Next  进入下一业务
=====================================================================*/
    SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(uint32_t)BLK_FUNC( Stack, pop )( SmartPi.serv_ID_Stack );
    
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
    break;
}
                
case ROOT_Game_Manila:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    
    SmartPi.serv_ID           = ROOT_Game_Manila;
    SmartPi.serv_ID_tmp       = 1;
    SmartPi.numOfNextNodes    = 8;
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = alloca( SmartPi.cache_task_num*sizeof(TaskHandle_t) );
    
    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );

    taskENTER_CRITICAL();

    
    if( kSWEvent_ParentTaskCall == xEventGroupGetBits( EGHandle_Software ) ){
        // 初始化Manila项目
        SMP_Proj_Manila_init();
    }
    
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Game_Manila, UI   ) , NULL, 256, NULL, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Game_Manila, CTRL ) , NULL, 128, NULL, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();

/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                            pdTRUE,          // 清除该位
                                            pdTRUE,          // 等待所有指定的Bit
                                            portMAX_DELAY ); // 永久等待

/*====================================================================
 * Clear  清除任务
=====================================================================*/
    taskENTER_CRITICAL();
    SmartPi.deleteSubtask();
    taskEXIT_CRITICAL();

/*====================================================================
 * Shift  跳转业务
=====================================================================*/
    if( SmartPi.serv_ID_tmp == 0 ){
        SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(uint32_t)BLK_FUNC( Stack, pop )( SmartPi.serv_ID_Stack );
        SMP_Proj_Manila_deinit();
        I_AM_CHILD;
    }else{
        BLK_FUNC( Stack, push )( SmartPi.serv_ID_Stack, (void*)(SmartPi.serv_ID) );
        SmartPi.serv_ID = (typeof(SmartPi.serv_ID))(ROOT_Game_Manila_ + SmartPi.serv_ID_tmp);
        I_AM_PARENT;
    }

    break;
}

case ROOT_Game_Manila_ShipDiagram:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    
    SmartPi.serv_ID           = ROOT_Game_Manila;
    SmartPi.serv_ID_tmp       = 1;
    SmartPi.numOfNextNodes    = 0;
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = alloca( SmartPi.cache_task_num*sizeof(TaskHandle_t) );
    
    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );


    // 缓存的数据结构, 需与子任务同步
    struct{
        int8_t idx;
        struct {
            int8_t pos;
            int8_t shipment;
        }boat_info[3];
        int8_t round;
    }cache;
    
    // 初始化缓存
    cache.idx = 0;
    for( int8_t i=0; i<3; i++ ){
            cache.boat_info[i].shipment = Manila->boat[i].shipment;
            cache.boat_info[i].pos      = Manila->boat[i].pos;
    }
    cache.round = Manila->dice_round;
    
    // 分配任务, 并传入缓存数据
    taskENTER_CRITICAL();
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Game_Manila_ShipDiagram, UI   ) , NULL, 256, &cache, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, ROOT_Game_Manila_ShipDiagram, CTRL ) , NULL, 128, &cache, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();
/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                            pdTRUE,          // 清除该位
                                            pdTRUE,          // 等待所有指定的Bit
                                            portMAX_DELAY ); // 永久等待
    // 收集完数据后开始分析
    SMP_Proj_Manila_analyze();
/*====================================================================
 * Clear  清除任务
=====================================================================*/
    taskENTER_CRITICAL();
    SmartPi.deleteSubtask();
    taskEXIT_CRITICAL();

/*====================================================================
 * Work  计算任务
=====================================================================*/    
    SMP_Proj_Manila_analyze();

/*====================================================================
 * Back  返回上一业务
=====================================================================*/
    SmartPi.serv_ID         = (typeof(SmartPi.serv_ID))(uint32_t)BLK_FUNC( Stack, pop )( SmartPi.serv_ID_Stack );
    
    // 这是一个子节点任务, 返回时应告知为 子任务Call
    I_AM_CHILD;
    break;
}
                
default:{
    taskENTER_CRITICAL();
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = RH_MALLOC( SmartPi.cache_task_num*sizeof(TaskHandle_t) );

    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Pressed );

    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );

    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, Default   , UI   ) , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, Default   , CTRL ) , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();
    
    // 进入阻塞, 直到子任务完成为止
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                   pdTRUE,          // 清除该位
                                   pdTRUE,          // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待
    taskENTER_CRITICAL();
    for( int i=0; i<SmartPi.cache_task_num; i++ ){
        vTaskDelete( SmartPi.cache_task_handle[i] );
    }
    RH_FREE( SmartPi.cache_task_handle );
    taskEXIT_CRITICAL();

    SmartPi.serv_ID         = (typeof(SmartPi.serv_ID))(uint32_t)BLK_FUNC( Stack, pop )( SmartPi.serv_ID_Stack );
    SmartPi.cache_task_num  = 0;
    
    break;
}
                
        }
    }
}

/*====================================================================
 * SmartPi 主任务(无线电检查)
=====================================================================*/
static StaticTask_t   Task_TCB_radio;
static StackType_t    Task_Stack_radio[64];

extern void MAKE_TASK( subtask, radio, recv  ) ( void* param );
extern void MAKE_TASK( subtask, radio, send  ) ( void* param );
extern void MAKE_TASK( subtask, radio, check ) ( void* param );
void __TaskStatic_radio( void* param ){

    while(1){

        xEventGroupWaitBits( EGHandle_Hardware, kHWEvent_NRF24L01_RecvReady  | \
                                                kHWEvent_NRF24L01_SendReady  | \
                                                kHWEvent_NRF24L01_SendFailed | \
                                                kHWEvent_NRF24L01_RecvFailed | \
                                                kHWEvent_NRF24L01_Offline,
                                   pdFALSE,         // 清除该位
                                   pdFALSE,         // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待

        EventBits_t xResult = xEventGroupGetBits( EGHandle_Hardware );
        
        // 准备接收消息
        if( xResult&kHWEvent_NRF24L01_RecvReady ){
            //...// 还需要检查radio模式
            TaskHandle_t task_handle_radioRecv;
            RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, radio, recv ) , NULL, 256, NULL, 3, &task_handle_radioRecv ));
            xEventGroupClearBits( EGHandle_Hardware, kHWEvent_NRF24L01_RecvReady );
        }
        
        // 准备发送消息
        if( xResult&kHWEvent_NRF24L01_SendReady ){
            //...// 还需要检查radio模式
            TaskHandle_t task_handle_radioSend;
            RH_ASSERT( pdPASS == xTaskCreate(  MAKE_TASK( subtask, radio, send ) , NULL, 256, NULL, 3, &task_handle_radioSend ));
            xEventGroupClearBits( EGHandle_Hardware, kHWEvent_NRF24L01_SendReady );
        }

    }
}



/*====================================================================
 * SmartPi 主任务(心跳灯)
=====================================================================*/
#include "led.h"
static StaticTask_t   Task_TCB_led_heart;
static StackType_t    Task_Stack_led_heart[64];
void __TaskStatic_led_heart( void* param ){
    TickType_t        prevTick = xTaskGetTickCount();
    const  TickType_t onTick   = pdMS_TO_TICKS(50);
    const  TickType_t offTick  = pdMS_TO_TICKS(2000);
    while(1){
        LED_Set(1);
        vTaskDelayUntil( &prevTick, onTick  );
        LED_Set(0);
        vTaskDelayUntil( &prevTick, offTick );
    }
}

void __Task_init( void ){
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
    THandle_main        = xTaskCreateStatic( __TaskStatic_main         , \
                                            "__T_main"        , \
                                            sizeof( Task_Stack_main      ), NULL, 4, Task_Stack_main     , &Task_TCB_main      );

    THandle_radio_check = xTaskCreateStatic( __TaskStatic_radio  , \
                                            "__T_radio" , \
                                            sizeof( Task_Stack_radio   ), NULL, 4, Task_Stack_radio  , &Task_TCB_radio   );

    THandle_led_heart   = xTaskCreateStatic( __TaskStatic_led_heart    , \
                                            "__T_led_heart"   , \
                                            sizeof( Task_Stack_led_heart ), NULL, 4, Task_Stack_led_heart, &Task_TCB_led_heart );
    

    SmartPi.serv_ID_Stack = BLK_FUNC( Stack, createBase )( NULL );
#ifdef RH_DEBUG    
    RH_ASSERT( SmartPi.serv_ID_Stack );
#endif

}





























