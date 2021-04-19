

#include "delay.h"
#include "ssd1306.h"
#include "joystick.h"
#include "led.h"
#include "beeper.h"
#include "RH_gui_api.h"
#include "RH_gui.h"
#include "RH_color.h"

int main(void){

    delay_init(72);
    SSD1306_Init();
    JoyStick_Init();
    LED_Init();
    BEEP_Init();
    GUI_API_Init ();
    GUI_Init();

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
    
    ID_t ID_JOY         = GUI_object_create( &cfg_obj );
    
    cfg_obj.style       = kGUI_ObjStyle_num;
    cfg_obj.area.xs     = 90;
    cfg_obj.area.ys     = 20;
    cfg_obj.area.width  = 35;
    cfg_obj.area.height = 12;
    ID_t ID_NUM_1       = GUI_object_create( &cfg_obj );
    
    cfg_obj.area.ys    += cfg_obj.area.height;
    ID_t ID_NUM_2       = GUI_object_create( &cfg_obj );

    cfg_obj.style       = kGUI_ObjStyle_text;
    
    
    cfg_obj.area.width  = 10;
    cfg_obj.area.xs    -= cfg_obj.area.width-1;
    cfg_obj.text        = "Y";
    ID_t ID_TXT_Y       = GUI_object_create( &cfg_obj );
    
    cfg_obj.area.ys    -= cfg_obj.area.height;
    cfg_obj.text        = "X";
    ID_t ID_TXT_X       = GUI_object_create( &cfg_obj );

    GUI_object_insert(ID_JOY);
    GUI_object_insert(ID_NUM_1);
    GUI_object_insert(ID_NUM_2);
    
    GUI_object_insert(ID_TXT_X);
    GUI_object_insert(ID_TXT_Y);

    while(1){
        // GUI_object_adjust(ID_JOY, 2066, 0); 
        GUI_object_adjust(ID_JOY, joystick_data[0], joystick_data[1]);
        GUI_object_adjust(ID_NUM_1, joystick_data[0], 0);
        GUI_object_adjust(ID_NUM_2, joystick_data[1], 0);
        GUI_RefreashScreen();
    }

    
}





