
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
#include "nrf24l01.h" 

/*==============================
 * GUI headfiles
===============================*/
#include "GLU_api.h"
#include "GLU_glucoo.h"
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

/*==============================
 * temp headfiles
===============================*/
#include "stm32f10x_exti.h"

bool message = false;
int main(void){

    delay_init(72);

#if 1    
    portDISABLE_INTERRUPTS();
#else
    __disable_irq();
#endif

    SSD1306_Init();
    JoyStick_Init();
    LED_Init();
    BEEP_Init();

    NRF24L01_init();
    NRF24L01_pd(NULL);

    GLU_FUNC( API, init )();
    GLU_FUNC( GUI, init )();


#if 1
    portENABLE_INTERRUPTS();
#else
    __enable_irq();  
#endif 


#if 1
    __Event_init();
    __Task_init();

    vTaskStartScheduler();
#endif

    while(1);


}




