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

static void __subtask_0x00000000_UI( void* param){
#ifdef RH_DEBUG	
	RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == 0x00000000 );
#endif

	ID_t ID_Menu = 0;
	{
		__GUI_Menu_t cfg = {0};
    
        cfg.area.xs = 10;
        cfg.area.ys = 10;
        cfg.area.height = 50;
        cfg.area.width  = 90;
        cfg.nItem = 2;
        cfg.title = "Title";
        cfg.color_title = M_COLOR_WHITE;
        cfg.size  = 10;
        
        cfg.bk_color = M_COLOR_BLACK;
        cfg.sl_color = M_COLOR_WHITE;
        cfg.text_color = M_COLOR_WHITE;

        __GUI_MenuParam_t m[2] = {0};
        m[0].text = "Hardware";
        m[1].text = "About";

        cfg.menuList = m;
    
        ID_Menu = GUI_menu_create(&cfg);
    
	}

    GUI_menu_frame  ( ID_Menu, 1 );
    GUI_menu_insert ( ID_Menu );
    GUI_RefreashScreen();
	while(1){
        taskENTER_CRITICAL();

        int ans = 0;
        if( joystick_data[1] > 4000 ){
            ans = -1;
        }else if( joystick_data[1] < 1000 ){
            ans = 1;
        }else{
            ans = 0;
        }
        GUI_menu_scroll( ID_Menu, ans );
        GUI_RefreashEntireScreen();

        taskEXIT_CRITICAL();

        vTaskDelay(1);
	}
}



static StaticTask_t   Task_TCB_main;
static StackType_t    Task_Stack_main[256];
void __TaskStatic_main( void* param ){
    SmartPi.enter = true;
    SmartPi.exit  = false;
    while(1){
        switch( SmartPi.serv_ID ){

case 0x00000000:{
    if( SmartPi.enter ){
        // Launch sub_task 1
        // Launch sub task 2
        // ...
        taskENTER_CRITICAL();
        SmartPi.cache_task_num    = 1;
        SmartPi.cache_task_handle = RH_MALLOC( SmartPi.cache_task_num*sizeof(TaskHandle_t) );

        RH_ASSERT( pdPASS == xTaskCreate(  __subtask_0x00000000_UI , NULL, 256, &SmartPi.serv_ID, 3, &SmartPi.cache_task_handle[0] ));

        taskEXIT_CRITICAL();

        SmartPi.enter = false;
    }else if( SmartPi.exit ){
        // Delete sub_task 1
        // Delete sub_task 2
        // ...
        taskENTER_CRITICAL();
        for( int i=0; i<SmartPi.cache_task_num; i++ ){
            vTaskDelete( SmartPi.cache_task_handle[i] );
        }
        RH_FREE( SmartPi.cache_task_handle );
        taskEXIT_CRITICAL();

        SmartPi.serv_ID = SmartPi.serv_ID_tmp;
        SmartPi.enter = true;
        SmartPi.exit  = false;
    }
    break;
}
    
        
        }
        // GUI_RefreashScreen();
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





























