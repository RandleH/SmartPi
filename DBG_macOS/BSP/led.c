
//#include "stm32f10x.h"
//#include "stm32f10x_rcc.h"
//#include "stm32f10x_gpio.h"

#include "led.h"


#define LED_GPIO_RCC    RCC_APB2Periph_GPIOC
#define LED_GPIO        GPIOC
#define LED_GPIO_PIN    GPIO_Pin_13

static void __configGPIO(void){

}


void LED_Init(void){
    __configGPIO();
}


void LED_Set( int cmd ){

}

