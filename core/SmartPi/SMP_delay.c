
#include "FreeRTOS.h"
#include "task.h"
#include "projdefs.h"

void SMP_delay_ms( uint32_t ms ){
    // static TickType_t prevTick = xTaskGetTickCount();
    // const  TickType_t incrTick = pdMS_TO_TICKS(ms);
    
    // vTaskDelayUntil( &prevTick, incrTick);
}

void SMP_delay_us( uint32_t us ){
    // static TickType_t prevTick = xTaskGetTickCount();
    // const  TickType_t incrTick = pdMS_TO_TICKS(ms);
    
    // vTaskDelayUntil( &prevTick, incrTick);
}
