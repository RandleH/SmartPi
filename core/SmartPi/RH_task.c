#include "RH_task.h"



EventGroupHandle_t EGHandle_Hardware = NULL;


void __Event_hw_init( void ){
	EGHandle_Hardware = xEventGroupCreate();
}




