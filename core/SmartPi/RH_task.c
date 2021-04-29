#include "misc.h"
#include "RH_task.h"

#include "RH_gui.h"
#include "RH_color.h"

#include "joystick.h"
#include "nrf24l01.h"

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
__SmartPiService_t SmartPi           = {0};

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

extern void MAKE_TASK( subtask, 0x00000000, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, 0x00000000, UI   ) ( void* param );

extern void MAKE_TASK( subtask, 0x00000001, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, 0x00000001, UI   ) ( void* param );

extern void MAKE_TASK( subtask, 0x00000011, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, 0x00000011, UI   ) ( void* param );

extern void MAKE_TASK( subtask, 0x00000112, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, 0x00000112, UI   ) ( void* param );

extern void MAKE_TASK( subtask, 0x00000012, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, 0x00000012, UI   ) ( void* param );

extern void MAKE_TASK( subtask, 0x00000013, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, 0x00000013, UI   ) ( void* param );

extern void MAKE_TASK( subtask, 0x00000015, CTRL ) ( void* param );
extern void MAKE_TASK( subtask, 0x00000015, UI   ) ( void* param );

extern void MAKE_TASK( subtask, default   , CTRL ) ( void* param );
extern void MAKE_TASK( subtask, default   , UI   ) ( void* param );

/*====================================================================
 * SmartPi 主任务
=====================================================================*/
static StaticTask_t   Task_TCB_main;
static StackType_t    Task_Stack_main[384];
void __TaskStatic_main( void* param ){
    SmartPi.enter = true;
    SmartPi.exit  = false;
    while(1){
        switch( SmartPi.serv_ID ){
// $ROOT$
case 0x00000000:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = 0x00000000;
    SmartPi.serv_ID_tmp    = 1;
    SmartPi.numOfNextNodes = 2;

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

    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000000_UI   , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000000_CTRL , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
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
    SmartPi.serv_ID         |= SmartPi.serv_ID_tmp;
    SmartPi.cache_task_num  = 0;
    
    break;
}

// $ROOT$ -> Hardware
case 0x00000001:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = 0x00000001;
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

    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000001_UI   , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000001_CTRL , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
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
    if( SmartPi.serv_ID_tmp == 0 )
        SmartPi.serv_ID = (typeof(SmartPi.serv_ID))__Stack_pop( SmartPi.serv_ID_Stack );
    else{
        __Stack_push( SmartPi.serv_ID_Stack, (void*)(SmartPi.serv_ID) );
        SmartPi.serv_ID <<= 4;
        SmartPi.serv_ID |= SmartPi.serv_ID_tmp;
    }
    
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
    break;
}
               
// $ROOT$ -> Hardware -> NRF24L01
case 0x00000011:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = 0x00000011;
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
    
//    // 用于信息交互的临时数据
//    struct{
//
//    }param;

    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000011_UI   , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000011_CTRL , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
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
    if( SmartPi.serv_ID_tmp == 0 )
        SmartPi.serv_ID = (typeof(SmartPi.serv_ID))__Stack_pop( SmartPi.serv_ID_Stack );
    else{
        __Stack_push( SmartPi.serv_ID_Stack, (void*)(SmartPi.serv_ID) );
        SmartPi.serv_ID <<= 4;
        SmartPi.serv_ID |= SmartPi.serv_ID_tmp;
    }
    
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
    break;
}
                
// $ROOT$ -> Hardware -> NRF24L01 -> RX Address
// $ROOT$ -> Hardware -> NRF24L01 -> TX Address
case 0x00000112:
case 0x00000114:{
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
        size_t      size;
        int         bucket;
        const char* text;
    }param;
    param.size = 5;
    if( SmartPi.serv_ID==0x00000112 ){
        param.text = "RX Address:";
        memcpy( param.Addr, NRF24L01_RX_Addr, 5 );
    }else{
        param.text = "TX Address:";
        memcpy( param.Addr, NRF24L01_TX_Addr, 5 );
    }
    
    
    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000112_UI   , NULL, 256, &param, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000112_CTRL , NULL, 128, &param, 3, &SmartPi.cache_task_handle[1] ));
    taskEXIT_CRITICAL();
    
/*====================================================================
 * Blocked  阻塞区
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                   pdTRUE,          // 清除该位
                                   pdTRUE,          // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待
   
    if( SmartPi.serv_ID==0x00000112 ){
        memcpy( NRF24L01_RX_Addr, param.Addr, 5 );
    }else{
        memcpy( NRF24L01_TX_Addr, param.Addr, 5 );
    }
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
    SmartPi.serv_ID = (typeof(SmartPi.serv_ID))__Stack_pop( SmartPi.serv_ID_Stack );
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
    break;
}
                
                
// $ROOT$ -> Hardware -> JoyStick
case 0x00000012:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = 0x00000012;
    SmartPi.serv_ID_tmp    = 0;
    SmartPi.numOfNextNodes = 0;
    taskENTER_CRITICAL();
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = alloca( SmartPi.cache_task_num*sizeof(TaskHandle_t) );

    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Pressed );

    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );
    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000012_UI   , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000012_CTRL , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
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
    SmartPi.serv_ID = (typeof(SmartPi.serv_ID))__Stack_pop( SmartPi.serv_ID_Stack );
    
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
    break;
}

// $ROOT$ -> Hardware -> LED
case 0x00000013:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = 0x00000013;
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

    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000013_UI   , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000013_CTRL , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
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
    SmartPi.serv_ID = (typeof(SmartPi.serv_ID))__Stack_pop( SmartPi.serv_ID_Stack );
    
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
    break;
}
                
// $ROOT$ -> Hardware -> BEEPER
case 0x00000015:{
/*====================================================================
 * Init  初始化
=====================================================================*/
    SmartPi.serv_ID        = 0x00000015;
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

    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000015_UI   , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000015_CTRL , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
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
    SmartPi.serv_ID = (typeof(SmartPi.serv_ID))__Stack_pop( SmartPi.serv_ID_Stack );
    
    SmartPi.cache_task_num    = 0;
    SmartPi.cache_task_handle = NULL;
    break;
}
                
default:{
    taskENTER_CRITICAL();
    SmartPi.cache_task_num    = 2;
    SmartPi.cache_task_handle = RH_MALLOC( SmartPi.cache_task_num*sizeof(TaskHandle_t) );

    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Pressed );

    xEventGroupClearBits( EGHandle_Software, kSWEvent_UI_Finished      |
                                             kSWEvent_CTRL_Finished    );

    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_default_UI   , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));
    RH_ASSERT( pdPASS == xTaskCreate(  __subtask_default_CTRL , NULL, 128, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[1] ));
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

    SmartPi.serv_ID         = (typeof(SmartPi.serv_ID))__Stack_pop( SmartPi.serv_ID_Stack );
    SmartPi.cache_task_num  = 0;
    
    break;
}
                
        }
    }
}

/*====================================================================
 * SmartPi 主任务(无线电检查)
=====================================================================*/
static StaticTask_t   Task_TCB_radiock;
static StackType_t    Task_Stack_radiock[64];
void __TaskStatic_radio_check( void* param ){
    while(1){
        vTaskDelay(10);
    }
}

void __Task_radio_recv( void* param ){
    while(1){

        vTaskDelete( NULL );
    }
}

void __Task_radio_sent( void* param ){
    while(1){

        vTaskDelay(10);
        vTaskDelete( NULL );
    }
}


/*====================================================================
 * SmartPi 主任务(心跳灯)
=====================================================================*/
#include "led.h"
static StaticTask_t   Task_TCB_led_heart;
static StackType_t    Task_Stack_led_heart[64];
void __TaskStatic_led_heart( void* param ){
    while(1){
        LED_Set(1);
        vTaskDelay(10);
        LED_Set(0);
        vTaskDelay(500);
    }
}


void __Task_init( void ){
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
    THandle_main        = xTaskCreateStatic( __TaskStatic_main         , \
                                            "__TaskStatic_main"        , \
                                            sizeof( Task_Stack_main      ), NULL, 4, Task_Stack_main     , &Task_TCB_main      );

    THandle_radio_check = xTaskCreateStatic( __TaskStatic_radio_check  , \
                                            "__TaskStatic_radio_check" , \
                                            sizeof( Task_Stack_radiock   ), NULL, 4, Task_Stack_radiock  , &Task_TCB_radiock   );

    THandle_led_heart   = xTaskCreateStatic( __TaskStatic_led_heart    , \
                                            "__TaskStatic_led_heart"   , \
                                            sizeof( Task_Stack_led_heart ), NULL, 4, Task_Stack_led_heart, &Task_TCB_led_heart );
    
    memset( &SmartPi, 0, sizeof(SmartPi) );

    SmartPi.serv_ID_Stack = __Stack_createBase( NULL );
#ifdef RH_DEBUG    
    RH_ASSERT( SmartPi.serv_ID_Stack );
#endif

}





























