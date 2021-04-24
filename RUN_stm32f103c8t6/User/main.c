
/*==============================
 * STM32 headfiles
===============================*/
#include "delay.h"
#include "misc.h"

/*==============================
 * BSP headfiles
===============================*/
#include "ssd1306.h"
#include "joystick.h"
#include "led.h"
#include "beeper.h"

/*==============================
 * GUI headfiles
===============================*/
#include "RH_gui_api.h"
#include "RH_gui.h"
#include "RH_color.h"

/*==============================
 * OS headfiles
===============================*/
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

/*==============================
 * SmartPi headfiles
===============================*/
#include "RH_task.h"



int main(void){

    delay_init(72);

    SSD1306_Init();
    JoyStick_Init();
    LED_Init();
    BEEP_Init();

    GUI_API_Init ();
    GUI_Init();
    __Event_init();
    __Task_init();

    vTaskStartScheduler();

    while(1);

    // xQueueCreate()
    // xEventGroupCreate()
    // xEventGroupSetBits()
    // xEventGroupSetBitsFromISR()
    // xEventGroupWaitBits()
    // xEventGroupClearBits()

}




