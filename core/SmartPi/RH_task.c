#include "misc.h"
#include "RH_task.h"

#include "RH_gui.h"
#include "RH_color.h"

#include "joystick.h"

StaticEventGroup_t EG_Hardware       = {0};
EventGroupHandle_t EGHandle_Hardware = NULL;
StaticEventGroup_t EG_Software       = {0};
EventGroupHandle_t EGHandle_Software = NULL;

__SmartPiService_t SmartPi           = {0};

void __Event_init( void ){
    EGHandle_Hardware = xEventGroupCreateStatic(&EG_Hardware); // xEventGroupCreateStatic
    EGHandle_Software = xEventGroupCreateStatic(&EG_Software);
#ifdef RH_DEBUG
    RH_ASSERT( EGHandle_Hardware );
    RH_ASSERT( EGHandle_Software );
#endif
}

/* 空闲任务控制块 */
/* 空闲任务任务栈 */
static StaticTask_t   Idle_Task_TCB;
static StackType_t    Idle_Task_Stack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer, StackType_t  ** ppxIdleTaskStackBuffer, uint32_t * pulIdleTaskStackSize   ){
    *ppxIdleTaskTCBBuffer    = &Idle_Task_TCB;             /* 任务控制块内存 */ 
    *ppxIdleTaskStackBuffer  = Idle_Task_Stack;            /* 任务堆栈内存 */ 
    *pulIdleTaskStackSize    = configMINIMAL_STACK_SIZE;   /* 任务堆栈大小 */
}

/* 定时器任务控制块 */
/* 定时器任务栈 */
static StaticTask_t   Timer_Task_TCB;
static StackType_t    Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer, StackType_t  ** ppxTimerTaskStackBuffer, uint32_t * pulTimerTaskStackSize  ){
    *ppxTimerTaskTCBBuffer    = &Timer_Task_TCB;             /* 任务控制块内存 */ 
    *ppxTimerTaskStackBuffer  = Timer_Task_Stack;            /* 任务堆栈内存 */ 
    *pulTimerTaskStackSize    = configMINIMAL_STACK_SIZE;    /* 任务堆栈大小 */
}

extern void __subtask_0x00000000_CTRL ( void* param );
extern void __subtask_0x00000000_UI   ( void* param );
extern void __subtask_0x00000001_CTRL ( void* param );
extern void __subtask_0x00000001_UI   ( void* param );
extern void __subtask_default_UI      ( void* param );
extern void __subtask_default_CTRL    ( void* param );

static StaticTask_t   Task_TCB_main;
static StackType_t    Task_Stack_main[256];
void __TaskStatic_main( void* param ){
    SmartPi.enter = true;
    SmartPi.exit  = false;
    while(1){
        switch( SmartPi.serv_ID ){

case 0x00000000:{
    memset( &SmartPi, 0, sizeof(SmartPi) );
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

    SmartPi.serv_ID         |= SmartPi.serv_ID_tmp;
    SmartPi.cache_task_num  = 0;
    
    break;
}

// $ROOT$ -> Hardware
case 0x00000001:{
    memset( &SmartPi, 0, sizeof(SmartPi) );
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

    // 进入阻塞, 直到子任务完成为止
    xEventGroupWaitBits( EGHandle_Software, kSWEvent_UI_Finished | kSWEvent_CTRL_Finished,
                                   pdTRUE,          // 清除该位
                                   pdTRUE,          // 等待所有指定的Bit
                                   portMAX_DELAY ); // 永久等待

    taskENTER_CRITICAL();
    for( int i=0; i<SmartPi.cache_task_num; i++ ){
        vTaskDelete( SmartPi.cache_task_handle[i] );
    }

    taskEXIT_CRITICAL();
    if( SmartPi.serv_ID_tmp == 0 )
        SmartPi.serv_ID  = 0x00000000;
    else{
        SmartPi.serv_ID <<= 4;
        SmartPi.serv_ID |= SmartPi.serv_ID_tmp;
    }
    
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

    SmartPi.serv_ID         = 0x00000000;
    SmartPi.cache_task_num  = 0;
    
    break;
}
                
        }
        vTaskDelay(10);
    }
}

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

#include "led.h"
static StaticTask_t   Task_TCB_led_heart;
static StackType_t    Task_Stack_led_heart[128];
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
    xTaskCreateStatic( __TaskStatic_main         , \
                      "__TaskStatic_main"        , \
                      sizeof( Task_Stack_main      ), NULL, 4, Task_Stack_main     , &Task_TCB_main      );

    xTaskCreateStatic( __TaskStatic_radio_check  , \
                      "__TaskStatic_radio_check" , \
                      sizeof( Task_Stack_radiock   ), NULL, 4, Task_Stack_radiock  , &Task_TCB_radiock   );

    xTaskCreateStatic( __TaskStatic_led_heart    , \
                      "__TaskStatic_led_heart"   , \
                      sizeof( Task_Stack_led_heart ), NULL, 4, Task_Stack_led_heart, &Task_TCB_led_heart );


}





























