
#include "RH_color.h"
#include "RH_gui.h"


#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"

#include "joystick.h"

#include "RH_task.h"

// $ROOT$
void __subtask_0x00000000_UI   ( void* param ){
#ifdef RH_DEBUG	
	RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == 0x00000000 );
#endif

/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
	ID_t ID_Menu = 0;
	{
		__GUI_Menu_t cfg = {0};
        cfg.area.xs = 0;
        cfg.area.ys = 0;
        cfg.area.height = RH_CFG_SCREEN_HEIGHT -1;
        cfg.area.width  = RH_CFG_SCREEN_WIDTH  -1;
        cfg.nItem = 2;
        cfg.title = "SmartPi";
        cfg.color_title = M_COLOR_WHITE;
        cfg.size  = 10;
        
        cfg.bk_color    = M_COLOR_BLACK;
        cfg.sl_color    = M_COLOR_WHITE;
        cfg.text_color  = M_COLOR_WHITE;

        __GUI_MenuParam_t m[2] = {0};
        m[0].text = "Hardware";
        m[1].text = "About";
        cfg.menuList = m;
    
        ID_Menu = GUI_menu_create(&cfg);
	}
    GUI_menu_frame  ( ID_Menu, 0 );
    GUI_menu_insert ( ID_Menu );
    GUI_RefreashEntireScreen();
    EventBits_t xResult;

/*====================================================================
 * Loop  子任务循环体
=====================================================================*/    
	while( EXIT == false ){
        xResult = xEventGroupWaitBits( EGHandle_Hardware, kHWEvent_JoySitck_Up|kHWEvent_JoySitck_Down|kHWEvent_JoySitck_Pressed,
                                       pdFALSE,         // 清除该位
                                       pdFALSE,         // 不等待所有指定的Bit, 即逻辑或
                                       portMAX_DELAY ); // 永久等待
        if( xResult&kHWEvent_JoySitck_Pressed ){
            EXIT = true;
        }


        int ans = 0;
        if     ( xResult&kHWEvent_JoySitck_Up   )  { ans = -1; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Up   ); }
        else if( xResult&kHWEvent_JoySitck_Down )  { ans =  1; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Down ); }
        else                                       ans =  0;
        
        taskENTER_CRITICAL();
        GUI_menu_scroll( ID_Menu, ans );
        GUI_RefreashEntireScreen();
        taskEXIT_CRITICAL();
	}

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/        
    GUI_menu_delete( ID_Menu );
    GUI_RefreashEntireScreen();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1); 
}
void __subtask_0x00000000_CTRL ( void* param ){
#ifdef RH_DEBUG 
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == 0x00000000 );
#endif

/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/    
    bool EXIT = false;

/*====================================================================
 * Loop  子任务循环体
=====================================================================*/       
    while( EXIT==false ){
        EventBits_t xResult = xEventGroupGetBitsFromISR( EGHandle_Hardware );
        if( xResult&kHWEvent_JoySitck_Pressed ){
            EXIT = true;
        }

        if( joystick_data[1] > 4000 ){
            if( SmartPi.serv_ID_tmp > 1 )
                SmartPi.serv_ID_tmp--;
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Down  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Up    );
        }else if( joystick_data[1] < 1000 ){
            if( SmartPi.serv_ID_tmp < SmartPi.numOfNextNodes )
            SmartPi.serv_ID_tmp++;
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Up    );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Down  );
        }

        if( joystick_data[0] > 4000 ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
        }else if( joystick_data[0] < 1000 ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
        }
        vTaskDelay(10);
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/     
    xEventGroupSetBits( EGHandle_Software, kSWEvent_CTRL_Finished );
    while(1);  
}

// $ROOT$ -> Hardware
void __subtask_0x00000001_UI   ( void* param ){
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    ID_t ID_Menu = 0;

    __GUI_MenuParam_t* m = alloca( sizeof(__GUI_MenuParam_t)*SmartPi.numOfNextNodes );
    {
        __GUI_Menu_t cfg = {0};
        cfg.area.xs = 0;
        cfg.area.ys = 0;
        cfg.area.height = RH_CFG_SCREEN_HEIGHT -1;
        cfg.area.width  = RH_CFG_SCREEN_WIDTH  -1;
        cfg.nItem = SmartPi.numOfNextNodes;
        cfg.title = "Hardware";
        cfg.color_title = M_COLOR_WHITE;
        cfg.size  = 10;
        
        cfg.bk_color    = M_COLOR_BLACK;
        cfg.sl_color    = M_COLOR_WHITE;
        cfg.text_color  = M_COLOR_WHITE;

        
        m[0].text = "NRF24L01";
        m[1].text = "JoyStick";
        m[2].text = "LED";
        m[3].text = "Bluetooth";
        m[4].text = "Beeper";

        cfg.menuList = m;
    
        ID_Menu = GUI_menu_create(&cfg);
    }
    GUI_menu_frame  ( ID_Menu, 0 );
    GUI_menu_insert ( ID_Menu );
    GUI_RefreashEntireScreen();
    EventBits_t xResult;

/*====================================================================
 * Loop  子任务循环体
=====================================================================*/    
    while( EXIT == false ){
        xResult = xEventGroupWaitBits( EGHandle_Hardware, kHWEvent_JoySitck_Up|kHWEvent_JoySitck_Down|kHWEvent_JoySitck_Pressed|kHWEvent_JoySitck_Left,
                                       pdFALSE,         // 清除该位
                                       pdFALSE,         // 不等待所有指定的Bit, 即逻辑或
                                       portMAX_DELAY ); // 永久等待
        if( (xResult&kHWEvent_JoySitck_Pressed) || (xResult&kHWEvent_JoySitck_Left) ){
            EXIT = true;
        }


        int ans = 0;
        if     ( xResult&kHWEvent_JoySitck_Up   )  { ans = -1; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Up   ); }
        else if( xResult&kHWEvent_JoySitck_Down )  { ans =  1; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Down ); }
        else                                       ans =  0;
        
        taskENTER_CRITICAL();
        GUI_menu_scroll( ID_Menu, ans );
        GUI_RefreashEntireScreen();
        taskEXIT_CRITICAL();
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/        
    GUI_menu_delete( ID_Menu );
    GUI_RefreashEntireScreen();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1); 
}
void __subtask_0x00000001_CTRL ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == 0x00000001 );
#endif

/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;

/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT==false ){
        EventBits_t xResult = xEventGroupGetBitsFromISR( EGHandle_Hardware );

        if( joystick_data[1] > 4000 ){
            if( SmartPi.serv_ID_tmp > 1 )
                SmartPi.serv_ID_tmp--;
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Down  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Up    );
        }else if( joystick_data[1] < 100 ){
            if( SmartPi.serv_ID_tmp < SmartPi.numOfNextNodes )
            SmartPi.serv_ID_tmp++;
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Up    );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Down  );
        }

        if( joystick_data[0] > 4000 ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
        }else if( joystick_data[0] < 100 ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
        }
        
        if( xResult&kHWEvent_JoySitck_Pressed ){
            EXIT = true;
        }
        
        if( xResult&kHWEvent_JoySitck_Left ){
            SmartPi.serv_ID_tmp = 0;
            EXIT = true;
        }
        vTaskDelay(10);
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    xEventGroupSetBits( EGHandle_Software, kSWEvent_CTRL_Finished );
    while(1);
}

void __subtask_0x00000012_UI   ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == 0x00000012 );
#endif
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    ID_t ID_JoyStick = 0;
    ID_t ID_Num_X, ID_Num_Y, ID_Text_X, ID_Text_Y = 0;
    {
        __GUI_Object_t cfg = {0};
        GUI_object_quickSet(&cfg);
        
        cfg.style       = kGUI_ObjStyle_joystick;
        cfg.area.xs     = 10;
        cfg.area.ys     = 10;
        cfg.area.height = 45;
        cfg.area.width  = 45;
        cfg.val[0]      = joystick_data[0];
        cfg.val[1]      = joystick_data[1];
        cfg.min[0]      = 0;
        cfg.max[0]      = 4096;
        cfg.min[1]      = 0;
        cfg.max[1]      = 4096;
        cfg.showFrame   = true;
        cfg.text_color  = M_COLOR_WHITE;
        cfg.bk_color    = M_COLOR_BLACK;
        ID_JoyStick     = GUI_object_create( &cfg );

        cfg.style       = kGUI_ObjStyle_num;
        cfg.area.xs     = 90;
        cfg.area.ys     = 20;
        cfg.area.width  = 35;
        cfg.area.height = 12;
        cfg.font        = kGUI_FontStyle_ArialRounded_Bold;
        cfg.text_color  = M_COLOR_WHITE;
        cfg.text_size   = 8;
        cfg.text_align  = kGUI_FontAlign_Middle;
        cfg.showFrame   = true;
        ID_Num_X        = GUI_object_create( &cfg );
        
        cfg.area.ys    += cfg.area.height;
        ID_Num_Y        = GUI_object_create( &cfg );
        
        cfg.style       = kGUI_ObjStyle_text;
            
        cfg.area.width  = 10;
        cfg.area.xs    -= cfg.area.width-1;
        cfg.text        = "Y";
        ID_Text_Y       = GUI_object_create( &cfg );
        
        cfg.area.ys    -= cfg.area.height;
        cfg.text        = "X";
        ID_Text_X       = GUI_object_create( &cfg );
    }
    
    GUI_object_insert(ID_JoyStick);
    GUI_object_insert(ID_Num_X);
    GUI_object_insert(ID_Num_Y);
    GUI_object_insert(ID_Text_X);
    GUI_object_insert(ID_Text_Y);
    
/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT == false ){
        EventBits_t xResult = xEventGroupGetBitsFromISR( EGHandle_Hardware );
        if( xResult&kHWEvent_JoySitck_Pressed ){
            EXIT = true;
        }
        
        GUI_object_adjust(ID_JoyStick, joystick_data[0], joystick_data[1]);
        GUI_object_adjust(ID_Num_X, joystick_data[0], 0);
        GUI_object_adjust(ID_Num_Y, joystick_data[1], 0);
        GUI_RefreashScreen();
        
        vTaskDelay(10);
    }
    GUI_object_delete( ID_JoyStick );
    GUI_object_delete( ID_Num_X    );
    GUI_object_delete( ID_Num_Y    );
    GUI_object_delete( ID_Text_X   );
    GUI_object_delete( ID_Text_Y   );
    GUI_RefreashScreen();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    while(1);
}
void __subtask_0x00000012_CTRL ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == 0x00000012 );
#endif
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    xEventGroupWaitBits( EGHandle_Hardware, kHWEvent_JoySitck_Pressed,
                           pdFALSE,         // 清除该位
                           pdFALSE,         // 不等待所有指定的Bit, 即逻辑或
                           portMAX_DELAY ); // 永久等待
    xEventGroupSetBits( EGHandle_Software, kSWEvent_CTRL_Finished );
    while(1);
}

// $ROOT$ -> Hardware -> LED
void __subtask_0x00000013_UI   ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == 0x00000013 );
#endif
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    
    ID_t ID_Switch = 0;
    ID_t ID_Text   = 0;
    
    // Config [ Switch ]
    {
        __GUI_Object_t cfg = {0};

        GUI_object_quickSet(&cfg);

        cfg.style       = kGUI_ObjStyle_switch;
        cfg.area.xs     = 72;
        cfg.area.ys     = 20;
        cfg.area.height = 12;
        cfg.area.width  = 30;
        cfg.min[0]      = 0;
        cfg.max[0]      = 256;
        cfg.showFrame   = false;

        cfg.bk_color    = M_COLOR_BLACK;

        cfg.val[0]      = 0;
    
        ID_Switch = GUI_object_create(&cfg);
    }
    
    // Config [ Text ]
    {
        __GUI_Object_t cfg = {0};

        GUI_object_quickSet(&cfg);

        cfg.style       = kGUI_ObjStyle_text;
        cfg.area.xs     = 20;
        cfg.area.ys     = 20;
        cfg.area.height = 12;
        cfg.area.width  = 40;
        cfg.font        = kGUI_FontStyle_ArialRounded_Bold;
        cfg.text_color  = M_COLOR_WHITE;
        cfg.text        = "PC13";
        cfg.text_size   = 8;
        cfg.text_align  = kGUI_FontAlign_Left;
        cfg.showFrame   = false;

        cfg.bk_color    = M_COLOR_BLACK;

        ID_Text = GUI_object_create( &cfg );
        
    }

    GUI_object_insert ( ID_Switch );
    GUI_object_insert ( ID_Text   );
    GUI_RefreashEntireScreen();
    EventBits_t xResult;
    
    int ans = 0;
/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT == false ){
        xResult = xEventGroupWaitBits( EGHandle_Hardware, kHWEvent_JoySitck_Right|kHWEvent_JoySitck_Pressed|kHWEvent_JoySitck_Left,
                                       pdFALSE,         // 清除该位
                                       pdFALSE,         // 不等待所有指定的Bit, 即逻辑或
                                       portMAX_DELAY ); // 永久等待
        if( xResult&kHWEvent_JoySitck_Pressed ){
            EXIT = true;
        }


        
        if     ( xResult&kHWEvent_JoySitck_Right )  { ans = 1; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Right ); }
        else if( xResult&kHWEvent_JoySitck_Left  )  { ans = 0; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Left  ); }
        
        taskENTER_CRITICAL();
        GUI_object_adjust( ID_Switch, ans, 0 );
        GUI_RefreashScreen();
        taskEXIT_CRITICAL();
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    GUI_object_delete( ID_Switch );
    GUI_RefreashEntireScreen();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1);
}
void __subtask_0x00000013_CTRL ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == 0x00000013 );
#endif

/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    extern void LED_Set( int );
/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT==false ){
        EventBits_t xResult = xEventGroupGetBitsFromISR( EGHandle_Hardware );

        if( joystick_data[0] > 4000 ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
            LED_Set(1);
        }else if( joystick_data[0] < 100 ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            LED_Set(0);
        }
        
        if( xResult&kHWEvent_JoySitck_Pressed ){
            SmartPi.serv_ID_tmp = 0;
            LED_Set(0);
            EXIT = true;
        }

        vTaskDelay(10);
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    xEventGroupSetBits( EGHandle_Software, kSWEvent_CTRL_Finished );
    while(1);
}

// $ROOT$ -> Hardware -> BEEPER
void __subtask_0x00000015_UI   ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == 0x00000015 );
#endif
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    
    ID_t ID_Switch = 0;
    ID_t ID_Text   = 0;
    
    // Config [ Switch ]
    {
        __GUI_Object_t cfg = {0};

        GUI_object_quickSet(&cfg);

        cfg.style       = kGUI_ObjStyle_switch;
        cfg.area.xs     = 72;
        cfg.area.ys     = 20;
        cfg.area.height = 12;
        cfg.area.width  = 30;
        cfg.min[0]      = 0;
        cfg.max[0]      = 256;
        cfg.showFrame   = false;

        cfg.bk_color    = M_COLOR_BLACK;

        cfg.val[0]      = 0;
    
        ID_Switch = GUI_object_create(&cfg);
    }
    
    // Config [ Text ]
    {
        __GUI_Object_t cfg = {0};

        GUI_object_quickSet(&cfg);

        cfg.style       = kGUI_ObjStyle_text;
        cfg.area.xs     = 20;
        cfg.area.ys     = 20;
        cfg.area.height = 12;
        cfg.area.width  = 40;
        cfg.font        = kGUI_FontStyle_ArialRounded_Bold;
        cfg.text_color  = M_COLOR_WHITE;
        cfg.text        = "BEEPER";
        cfg.text_size   = 8;
        cfg.text_align  = kGUI_FontAlign_Left;
        cfg.showFrame   = false;

        cfg.bk_color    = M_COLOR_BLACK;

        ID_Text = GUI_object_create( &cfg );
        
    }

    GUI_object_insert ( ID_Switch );
    GUI_object_insert ( ID_Text   );
    GUI_RefreashEntireScreen();
    EventBits_t xResult;
    
    int ans = 0;
/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT == false ){
        xResult = xEventGroupWaitBits( EGHandle_Hardware, kHWEvent_JoySitck_Right|kHWEvent_JoySitck_Pressed|kHWEvent_JoySitck_Left,
                                       pdFALSE,         // 清除该位
                                       pdFALSE,         // 不等待所有指定的Bit, 即逻辑或
                                       portMAX_DELAY ); // 永久等待
        if( xResult&kHWEvent_JoySitck_Pressed ){
            EXIT = true;
        }


        
        if     ( xResult&kHWEvent_JoySitck_Right )  { ans = 1; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Right ); }
        else if( xResult&kHWEvent_JoySitck_Left  )  { ans = 0; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Left  ); }
        
        taskENTER_CRITICAL();
        GUI_object_adjust( ID_Switch, ans, 0 );
        GUI_RefreashScreen();
        taskEXIT_CRITICAL();
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    GUI_object_delete( ID_Switch );
    GUI_RefreashEntireScreen();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1);
}
void __subtask_0x00000015_CTRL ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == 0x00000015 );
#endif

/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    extern void BEEP_Set( int );
/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT==false ){
        EventBits_t xResult = xEventGroupGetBitsFromISR( EGHandle_Hardware );

        if( joystick_data[0] > 4000 ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
            BEEP_Set(1);
        }else if( joystick_data[0] < 100 ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            BEEP_Set(0);
        }
        
        if( xResult&kHWEvent_JoySitck_Pressed ){
            SmartPi.serv_ID_tmp = 0;
            BEEP_Set(0);
            EXIT = true;
        }

        vTaskDelay(10);
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    xEventGroupSetBits( EGHandle_Software, kSWEvent_CTRL_Finished );
    while(1);
}
// $DEFAULT$
void __subtask_default_UI      ( void* param ){
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    ID_t ID_Object = 0;

    {
        __GUI_Object_t cfg = {0};

        GUI_object_quickSet(&cfg);

        cfg.style       = kGUI_ObjStyle_text;
        cfg.area.xs     = 30;
        cfg.area.ys     = 30;
        cfg.area.height = 12;
        cfg.area.width  = 70;
        cfg.font        = kGUI_FontStyle_ArialRounded_Bold;
        cfg.text_color  = M_COLOR_WHITE;
        cfg.text        = "No preview.";
        cfg.text_size   = 8;
        cfg.text_align  = kGUI_FontAlign_Left;
        cfg.showFrame   = false;

        cfg.bk_color    = M_COLOR_BLACK;

        ID_Object = GUI_object_create( &cfg );
        
    }
    
    GUI_object_insert( ID_Object );
    GUI_RefreashScreen();
    
    EventBits_t xResult;

/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT == false ){
        xResult = xEventGroupWaitBits( EGHandle_Hardware, kHWEvent_JoySitck_Pressed,
                                       pdFALSE,         // 清除该位
                                       pdFALSE,         // 不等待所有指定的Bit, 即逻辑或
                                       portMAX_DELAY ); // 永久等待
        if( xResult&kHWEvent_JoySitck_Pressed ){
            EXIT = true;
        }

    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    GUI_object_delete( ID_Object );
    GUI_RefreashScreen();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1);
}
void __subtask_default_CTRL    ( void* param ){
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;

/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT==false ){
        EventBits_t xResult = xEventGroupGetBitsFromISR( EGHandle_Hardware );
        
        if( xResult&kHWEvent_JoySitck_Pressed ){
            SmartPi.serv_ID_tmp = 0;
            EXIT = true;
        }
        
        vTaskDelay(10);
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    xEventGroupSetBits( EGHandle_Software, kSWEvent_CTRL_Finished );
    while(1);
}




