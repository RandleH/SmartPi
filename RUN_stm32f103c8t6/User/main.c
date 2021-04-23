

#include "delay.h"
#include "ssd1306.h"
#include "joystick.h"
#include "led.h"
#include "beeper.h"
#include "RH_gui_api.h"
#include "RH_gui.h"
#include "RH_color.h"

#include "misc.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"



#include "RH_task.h"

static inline ID_t __config_UI_joystick(void){
    __GUI_Object_t cfg_obj = {0};

    GUI_object_quickSet(&cfg_obj);

    cfg_obj.style       = kGUI_ObjStyle_joystick;
    cfg_obj.area.xs     = 10;
    cfg_obj.area.ys     = 10;
    cfg_obj.area.height = 45;
    cfg_obj.area.width  = 45;
    cfg_obj.min[0]      = 0;
    cfg_obj.max[0]      = 4096;
    cfg_obj.min[1]      = 0;
    cfg_obj.max[1]      = 4096;
    cfg_obj.font        = kGUI_FontStyle_ArialRounded_Bold;
    cfg_obj.text_color  = M_COLOR_WHITE;
    cfg_obj.text_size   = 8;
    cfg_obj.text_align  = kGUI_FontAlign_Middle;
    cfg_obj.showFrame   = true;
    cfg_obj.bk_color    = M_COLOR_BLACK;
    cfg_obj.val[0]      = joystick_data[0];
    cfg_obj.val[1]      = joystick_data[1];
    
    return GUI_object_create( &cfg_obj );
}

// static inline ID_t __config_UI_num_X(void){
//     __GUI_Object_t cfg_obj = {0};
//     GUI_object_quickSet(&cfg_obj);
//     cfg_obj.style       = kGUI_ObjStyle_num;
//     cfg_obj.area.xs     = 90;
//     cfg_obj.area.ys     = 20;
//     cfg_obj.area.width  = 35;
//     cfg_obj.area.height = 12;
//     ID_t ID_NUM_1       = GUI_object_create( &cfg_obj );
// }

// static inline ID_t __config_UI_num_Y(void){
//     __GUI_Object_t cfg_obj = {0};
//     GUI_object_quickSet(&cfg_obj);
//     cfg_obj.style       = kGUI_ObjStyle_num;
//     cfg_obj.area.xs     = 90;
//     cfg_obj.area.ys     = 20+12;
//     cfg_obj.area.width  = 35;
//     cfg_obj.area.height = 12;
//     return GUI_object_create( &cfg_obj );
// }


TaskHandle_t       Handler_LedBlink = 0;
void Task_LedBlink(void* param){
    while(1){
        LED_Set(0);
        vTaskDelay (500);
        LED_Set(1);
        vTaskDelay (500);
    }
}

// OS_TCB          TCB_BeepBlink;
// CPU_STK         STK_BeepBlink[128];
TaskHandle_t       Handler_BeepBlink = 0;
void Task_BeepBlink(void* param){
    while(1){
        BEEP_Set(0);
        vTaskDelay (10);
        BEEP_Set(1);
        vTaskDelay (1000);
    }
}



/* 空闲任务控制块 */
/* 空闲任务任务栈 */
static StaticTask_t   Idle_Task_TCB;
static StackType_t    Idle_Task_Stack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer   ,
                                    StackType_t  ** ppxIdleTaskStackBuffer ,
                                    uint32_t      * pulIdleTaskStackSize   ){
    *ppxIdleTaskTCBBuffer    = &Idle_Task_TCB;             /* 任务控制块内存 */ 
    *ppxIdleTaskStackBuffer  = Idle_Task_Stack;            /* 任务堆栈内存 */ 
    *pulIdleTaskStackSize    = configMINIMAL_STACK_SIZE;   /* 任务堆栈大小 */
}

/* 定时器任务控制块 */
/* 定时器任务栈 */
static StaticTask_t   Timer_Task_TCB;
static StackType_t    Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer  ,
                                     StackType_t  ** ppxTimerTaskStackBuffer,
                                     uint32_t      * pulTimerTaskStackSize  ){
    *ppxTimerTaskTCBBuffer    = &Timer_Task_TCB;             /* 任务控制块内存 */ 
    *ppxTimerTaskStackBuffer  = Timer_Task_Stack;            /* 任务堆栈内存 */ 
    *pulTimerTaskStackSize    = configMINIMAL_STACK_SIZE;    /* 任务堆栈大小 */
}

// static StaticTask_t   GUI_Task_TCB;
// static StackType_t    GUI_Task_Stack[256];
void Task_UI(void* param){
    ID_t ID_JOY = __config_UI_joystick();
    while(1){
        taskENTER_CRITICAL();
        GUI_object_adjust(ID_JOY, joystick_data[0], joystick_data[1]);
        taskEXIT_CRITICAL();
        GUI_RefreashScreen();
        vTaskDelay(10);
    }
    
}

int main(void){

    delay_init(72);
    SSD1306_Init();
    JoyStick_Init();
    LED_Init();
    BEEP_Init();
    GUI_API_Init ();
    GUI_Init();

    __Task_init();

    vTaskStartScheduler();


    // xQueueCreate()
    // xEventGroupCreate()
    // xEventGroupSetBits()
    // xEventGroupSetBitsFromISR()
    // xEventGroupWaitBits()
    // xEventGroupClearBits()

}


// void main_old(void){

    

    


//     cfg_obj.style       = kGUI_ObjStyle_text;
    
    
//     cfg_obj.area.width  = 10;
//     cfg_obj.area.xs    -= cfg_obj.area.width-1;
//     cfg_obj.text        = "Y";
//     ID_t ID_TXT_Y       = GUI_object_create( &cfg_obj );
    
//     cfg_obj.area.ys    -= cfg_obj.area.height;
//     cfg_obj.text        = "X";
//     ID_t ID_TXT_X       = GUI_object_create( &cfg_obj );

//     GUI_object_insert(ID_JOY);
//     GUI_object_insert(ID_NUM_1);
//     GUI_object_insert(ID_NUM_2);
    
//     GUI_object_insert(ID_TXT_X);
//     GUI_object_insert(ID_TXT_Y);


// }


