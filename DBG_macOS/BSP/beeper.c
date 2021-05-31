
//#include "stm32f10x.h"
//#include "stm32f10x_rcc.h"
//#include "stm32f10x_gpio.h"

#include "beeper.h"


#define BEEP_GPIO_RCC    RCC_APB2Periph_GPIOA
#define BEEP_GPIO        GPIOA
#define BEEP_GPIO_PIN    GPIO_Pin_2

static void __configGPIO(void){

}

void BEEP_Init(void){
    __configGPIO();
}


void BEEP_Set( int cmd ){

}



