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

static void __subtask_0x00000000_UI( void* param ){
#ifdef RH_DEBUG	
	RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == 0x00000000 );
#endif

/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
	ID_t ID_Menu = 0;
	{
		__GUI_Menu_t cfg = {0};
        cfg.area.xs = 0;
        cfg.area.ys = 0;
        cfg.area.height = RH_CFG_SCREEN_HEIGHT -1;
        cfg.area.width  = RH_CFG_SCREEN_WIDTH  -1;
        cfg.nItem = 2;
        cfg.title = "SmartPi";
        cfg.color_title = M_COLOR_WHITE;
        cfg.size  = 10;
        
        cfg.bk_color    = M_COLOR_BLACK;
        cfg.sl_color    = M_COLOR_WHITE;
        cfg.text_color  = M_COLOR_WHITE;

        __GUI_MenuParam_t m[2] = {0};
        m[0].text = "Hardware";
        m[1].text = "About";
        cfg.menuList = m;
    
        ID_Menu = GUI_menu_create(&cfg);
	}
    GUI_menu_frame  ( ID_Menu, 0 );
    GUI_menu_insert ( ID_Menu );
    GUI_RefreashEntireScreen();
    EventBits_t xResult;

/*====================================================================
 * Loop  子任务循环体
=====================================================================*/    
	while( EXIT == false ){
        xResult = xEventGroupWaitBits( EGHandle_Hardware, kHWEvent_JoySitck_Up|kHWEvent_JoySitck_Down|kHWEvent_JoySitck_Pressed,
                                       pdFALSE,         // 清除该位
                                       pdFALSE,         // 不等待所有指定的Bit, 即逻辑或
                                       portMAX_DELAY ); // 永久等待
        if( xResult&kHWEvent_JoySitck_Pressed ){
            EXIT = true;
        }


        int ans = 0;
        if     ( xResult&kHWEvent_JoySitck_Up   )  { ans = -1; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Up   ); }
        else if( xResult&kHWEvent_JoySitck_Down )  { ans =  1; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Down ); }
        else                                       ans =  0;
        
        taskENTER_CRITICAL();
        GUI_menu_scroll( ID_Menu, ans );
        GUI_RefreashEntireScreen();
        taskEXIT_CRITICAL();
	}

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/        
    // Delete cache or object.

    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1); 
}

static void __subtask_0x00000000_CTRL( void* param ){
#ifdef RH_DEBUG 
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == 0x00000000 );
#endif

/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/    
    bool EXIT = false;

/*====================================================================
 * Loop  子任务循环体
=====================================================================*/       
    while( EXIT==false ){
        EventBits_t xResult = xEventGroupGetBitsFromISR( EGHandle_Hardware );
        if( xResult&kHWEvent_JoySitck_Pressed ){
            EXIT = true;
        }

        if( joystick_data[1] > 4000 ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Down  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Up    );
        }else if( joystick_data[1] < 1000 ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Up    );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Down  );
        }

        if( joystick_data[0] > 4000 ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
        }else if( joystick_data[0] < 1000 ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
        }
        vTaskDelay(10);
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/     
    xEventGroupSetBits( EGHandle_Software, kSWEvent_CTRL_Finished );
    while(1);  
}



static StaticTask_t   Task_TCB_main;
static StackType_t    Task_Stack_main[256];
void __TaskStatic_main( void* param ){
    SmartPi.enter = true;
    SmartPi.exit  = false;
    while(1){
        switch( SmartPi.serv_ID ){

case 0x00000000:{

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

    SmartPi.serv_ID         = 0x00000001;// SmartPi.serv_ID_tmp;
    SmartPi.cache_task_num  = 0;
    
    break;
}

case 0x00000001:{
    if( SmartPi.enter ){
        GUI_rect_raw(30,30,40,40);
        GUI_RefreashScreen();
    }
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





























