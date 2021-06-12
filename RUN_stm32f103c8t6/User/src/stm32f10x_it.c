/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
// void SVC_Handler(void)
// {
// }

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
// void PendSV_Handler(void)
// {
// }

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void xPortSysTickHandler(void);
//systick中断服务函数
void SysTick_Handler(void)
{ 
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
      {
    #endif  /* INCLUDE_xTaskGetSchedulerState */  
        xPortSysTickHandler();
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      }
    #endif  /* INCLUDE_xTaskGetSchedulerState */
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

#include "stm32f10x_exti.h"
#include "delay.h"

#include "joystick.h"

#include "FreeRTOS.h"
#include "event_groups.h"

#include "RH_task.h"

#define EXTI_OK_LINE             EXTI_Line7
#define EXTI_OK_GPIO             GPIOA
#define EXTI_OK_PIN              GPIO_Pin_7

#define JOYSTICK_IRQHandler      EXTI9_5_IRQHandler

#define NRF_LINE                 EXTI_Line5
#define NRF_IRQn                 EXTI9_5_IRQn
#define NRF_PIN_IRQ              GPIO_Pin_5 
#define NRF_GPIO_IRQ             GPIOB


void EXTI9_5_IRQHandler(void){
    BaseType_t    xResult;
    BaseType_t    xHigherPriorityTaskWoken = pdFALSE;


    if(EXTI_GetITStatus(EXTI_OK_LINE)!=RESET && GPIO_ReadInputDataBit( EXTI_OK_GPIO, EXTI_OK_PIN ) == Bit_RESET ){
        xResult = xEventGroupSetBitsFromISR( EGHandle_Hardware, kHWEvent_JoySitck_Pressed, &xHigherPriorityTaskWoken );

        if( xResult != pdFAIL ){
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
        //...//
    }


    if(EXTI_GetITStatus(NRF_LINE)!=RESET && GPIO_ReadInputDataBit( NRF_GPIO_IRQ, NRF_PIN_IRQ ) == Bit_RESET ){
        extern bool message;
        // xResult = xEventGroupSetBitsFromISR( EGHandle_Hardware, kHWEvent_NRF24L01_RecvReady, &xHigherPriorityTaskWoken );

        // if( xResult != pdFAIL ){
        //     portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        // }
        message = true;
        //...// 
    }

    EXTI_ClearITPendingBit(EXTI_OK_LINE);
    EXTI_ClearITPendingBit(NRF_LINE);

}

#define NRF24L01_IRQHandler     

void assert_failed(uint8_t* file, uint32_t line){
    // const char* FILE = (char*)file;
    // uint32_t    LINE = line;
    while(1);
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
