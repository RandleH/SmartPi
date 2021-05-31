

    
#include <stdio.h>

#include "GLU_glucoo.h"
#include "GLU_api.h"
#include "ssd1306.h"
#ifdef __cplusplcus
extern "C"{
#endif


void GLU_FUNC( API, init ) (void){
    GUI_API_DrawArea     = SSD1306_API_DrawArea;
    
}


#ifdef __cplusplcus
}
#endif



