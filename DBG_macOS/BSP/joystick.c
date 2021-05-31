//#include "stm32f10x_conf.h"
//#include "stm32f10x_dma.h"
//#include "stm32f10x_exti.h"

#include "delay.h"
#include "joystick.h"

#define ADC_BASE                 ADC1

#define DMAx_CHx                 DMA1_Channel1
#define DMA_RCC                  RCC_AHBPeriph_DMA1

#define ADC_X_CH                 ADC_Channel_8
#define ADC_X_RCC                RCC_APB2Periph_ADC1
#define ADC_X_GPIO_RCC           RCC_APB2Periph_GPIOB
#define ADC_X_GPIO               GPIOB
#define ADC_X_PIN                GPIO_Pin_0
       
#define ADC_Y_CH                 ADC_Channel_9
#define ADC_Y_RCC                RCC_APB2Periph_ADC1
#define ADC_Y_GPIO_RCC           RCC_APB2Periph_GPIOB
#define ADC_Y_GPIO               GPIOB
#define ADC_Y_PIN                GPIO_Pin_1
       
#define EXTI_OK_GPIO_RCC         RCC_APB2Periph_GPIOA
#define EXTI_OK_GPIO             GPIOA
#define EXTI_OK_PIN              GPIO_Pin_7
#define EXTI_OK_LINE             EXTI_Line7
#define EXTI_OK_PORTSRC          GPIO_PortSourceGPIOA, GPIO_PinSource7
#define EXTI_OK_IRQ              EXTI9_5_IRQn

#define JOYSTICK_IRQHandler      EXTI9_5_IRQHandler


uint16_t joystick_data[2] = {0};

static void __configGPIO(void){

}

static void __configEXTI(void){

}

static void __configNVIC(void){

}

static void __configDMA(void){

}

static void __configADC(void){

}

void JoyStick_Init(void){
    __configGPIO();
    __configEXTI();
    __configNVIC();
    __configDMA();
    __configADC();
}








