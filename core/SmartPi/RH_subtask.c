
#include "RH_color.h"
#include "GLU_glucoo.h"


#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"

#include "joystick.h"

#include "RH_task.h"

#undef RH_DEBUG

#ifdef MAKE_TASK
#undef MAKE_TASK
#endif
#define MAKE_TASK( CLASS, ID, TYPE ) __##CLASS##_##ID##_##TYPE

void MAKE_TASK( subtask, ROOT, UI   ) ( void* param ){
#ifdef RH_DEBUG	
	RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == ROOT );
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
        cfg.nItem = SmartPi.numOfNextNodes;
        cfg.title = "SmartPi";
        cfg.color_title = M_COLOR_WHITE;
        cfg.size  = 10;
        
        cfg.bk_color    = M_COLOR_BLACK;
        cfg.sl_color    = M_COLOR_WHITE;
        cfg.text_color  = M_COLOR_WHITE;

        __GUI_MenuParam_t *m = alloca( cfg.nItem*sizeof(__GUI_MenuParam_t) );
        m[0].text = "Hardware";
        m[1].text = "Game";
        m[2].text = "About";
        cfg.menuList = m;
    
        ID_Menu = GLU_FUNC( Menu, create )(&cfg);
	}
    GLU_FUNC( Menu, frame  ) ( ID_Menu, 0 );
    GLU_FUNC( Menu, insert ) ( ID_Menu );

    GLU_FUNC( GUI, refreashEntireScreen )();
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
        GLU_FUNC( Menu, scroll )( ID_Menu, ans );
        GLU_FUNC( GUI, refreashEntireScreen )();
        taskEXIT_CRITICAL();
	}

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    GLU_FUNC( Menu, delete )( ID_Menu );
    GLU_FUNC( GUI, refreashEntireScreen )();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1); 
}
void MAKE_TASK( subtask, ROOT, CTRL ) ( void* param ){
#ifdef RH_DEBUG 
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == ROOT );
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

void MAKE_TASK( subtask, ROOT_Hardware, UI   ) ( void* param ){
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
    
        ID_Menu = GLU_FUNC( Menu, create )(&cfg);
    }
    GLU_FUNC( Menu, frame )  ( ID_Menu, 0 );
    GLU_FUNC( Menu, insert ) ( ID_Menu );
    GLU_FUNC( GUI, refreashEntireScreen )();
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
        GLU_FUNC( Menu, scroll )( ID_Menu, ans );
        GLU_FUNC( GUI, refreashEntireScreen )();
        taskEXIT_CRITICAL();
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    GLU_FUNC( Menu, delete )( ID_Menu );
    GLU_FUNC( GUI, refreashEntireScreen )();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1); 
}
void MAKE_TASK( subtask, ROOT_Hardware, CTRL ) ( void* param ){
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

void MAKE_TASK( subtask, ROOT_Game, UI   ) ( void* param ){
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    ID_t ID_Menu = 0;

    __GUI_MenuParam_t* m = alloca( sizeof(__GUI_MenuParam_t)*SmartPi.numOfNextNodes );
    m[0].text = "Manila";
    {
        __GUI_Menu_t cfg = {0};
        cfg.area.xs = 0;
        cfg.area.ys = 0;
        cfg.area.height = RH_CFG_SCREEN_HEIGHT -1;
        cfg.area.width  = RH_CFG_SCREEN_WIDTH  -1;
        cfg.nItem       = SmartPi.numOfNextNodes;
        cfg.title       = "Game";
        cfg.color_title = M_COLOR_WHITE;
        cfg.size  = 10;
        
        cfg.bk_color    = M_COLOR_BLACK;
        cfg.sl_color    = M_COLOR_WHITE;
        cfg.text_color  = M_COLOR_WHITE;

        cfg.menuList    = m;
    
        ID_Menu = GLU_FUNC( Menu, create )(&cfg);
    }
    GLU_FUNC( Menu, frame )  ( ID_Menu, 0 );
    GLU_FUNC( Menu, insert ) ( ID_Menu );
    GLU_FUNC( GUI, refreashEntireScreen )();
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
        GLU_FUNC( Menu, scroll )( ID_Menu, ans );
        GLU_FUNC( GUI, refreashEntireScreen )();
        taskEXIT_CRITICAL();
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    GLU_FUNC( Menu, delete )( ID_Menu );
    GLU_FUNC( GUI, refreashEntireScreen )();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1);
}
void MAKE_TASK( subtask, ROOT_Game, CTRL ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == ROOT_Game );
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

        if( joystick_data[0] > M_JOYSTICK_THREASHOLD_RIGHT ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
        }else if( joystick_data[0] < M_JOYSTICK_THREASHOLD_LEFT && joystick_data[1] < M_JOYSTICK_THREASHOLD_UP ){
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
    


void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01, UI   ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( SmartPi.serv_ID == ROOT_Hardware_NRF24L01 );
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
        cfg.nItem = 5;
        cfg.title = "NRF24L01";
        cfg.color_title = M_COLOR_WHITE;
        cfg.size  = 10;
        
        cfg.bk_color    = M_COLOR_BLACK;
        cfg.sl_color    = M_COLOR_WHITE;
        cfg.text_color  = M_COLOR_WHITE;

        __GUI_MenuParam_t m[5] = {0};
        m[0].text = "RX mode";
        m[1].text = "RX Address";
        m[2].text = "TX mode";
        m[3].text = "TX Address";
        m[4].text = "ACK";
        cfg.menuList = m;
    
        ID_Menu = GLU_FUNC( Menu, create )(&cfg);
    }
    GLU_FUNC( Menu, frame )  ( ID_Menu, 0 );
    GLU_FUNC( Menu, insert ) ( ID_Menu );
    GLU_FUNC( GUI, refreashEntireScreen )();
    EventBits_t xResult;

/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT == false ){
        xResult = xEventGroupWaitBits( EGHandle_Hardware, kHWEvent_JoySitck_Up|kHWEvent_JoySitck_Down|kHWEvent_JoySitck_Pressed|kHWEvent_JoySitck_Left,
                                       pdFALSE,         // 清除该位
                                       pdFALSE,         // 不等待所有指定的Bit, 即逻辑或
                                       portMAX_DELAY ); // 永久等待
        if( (xResult&kHWEvent_JoySitck_Left) || (xResult&kHWEvent_JoySitck_Pressed) ){
            EXIT = true;
        }


        int ans = 0;
        if     ( xResult&kHWEvent_JoySitck_Up   )  { ans = -1; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Up   ); }
        else if( xResult&kHWEvent_JoySitck_Down )  { ans =  1; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Down ); }
        else                                       ans =  0;
        
        taskENTER_CRITICAL();
        GLU_FUNC( Menu, scroll )( ID_Menu, ans );
        GLU_FUNC( GUI, refreashEntireScreen )();
        taskEXIT_CRITICAL();
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    GLU_FUNC( Menu, delete )( ID_Menu );
    GLU_FUNC( GUI, refreashEntireScreen )();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1);
}
void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01, CTRL ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( SmartPi.serv_ID == ROOT_Hardware_NRF24L01 );
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

void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RXAddress, UI   ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( SmartPi.serv_ID == 0x00000112 || SmartPi.serv_ID == 0x00000114 );
#endif
    
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    struct{
        uint8_t Addr[5];
        size_t  size;
        int     bucket;
        const char* text;
    }*p = param;
    ID_t ID_Text       = 0;
    ID_t ID_Addr[5]    = {0};
    
    {
        __GUI_Object_t cfg = {0};
        
        cfg.widget      = kGUI_ObjStyle_num;
        cfg.area.width  = 23;
        cfg.area.height = 12;
        cfg.area.xs     = (int)((RH_CFG_SCREEN_WIDTH - 5*cfg.area.width)>>1);
        cfg.area.ys     = 32;
        
        cfg.bk_color    = M_COLOR_BLACK;
        cfg.font        = kGUI_FontStyle_ArialRounded_Bold;
        cfg.text_align  = kGUI_FontAlign_Middle;
        cfg.obj_color   = M_COLOR_WHITE;
        cfg.text_size   = 8;
        // cfg.max[0]      = 255;
        // cfg.min[0]      = 0;
        // cfg.val[0]      = p->Addr[0];
        cfg.showFrame   = true;
        ID_Addr[0] = GLU_FUNC( Object, create )( &cfg );
        
        cfg.showFrame  = false;
        cfg.area.xs   += cfg.area.width;
        // cfg.val[0]     = p->Addr[1];
        ID_Addr[1] = GLU_FUNC( Object, create )( &cfg );
        
        cfg.area.xs   += cfg.area.width;
        // cfg.val[0]     = p->Addr[2];
        ID_Addr[2] = GLU_FUNC( Object, create )( &cfg );
        
        cfg.area.xs   += cfg.area.width;
        // cfg.val[0]     = p->Addr[3];
        ID_Addr[3] = GLU_FUNC( Object, create )( &cfg );
        
        cfg.area.xs   += cfg.area.width;
        // cfg.val[0]     = p->Addr[4];
        ID_Addr[4] = GLU_FUNC( Object, create )( &cfg );
        
        cfg.widget      = kGUI_ObjStyle_text;
        cfg.area.ys   -= cfg.area.height;
        cfg.area.xs    = 0;
        cfg.text       = p->text;
        cfg.area.width = strlen(cfg.text)*8;
        cfg.text_align  = kGUI_FontAlign_Left;
        cfg.obj_color   = M_COLOR_WHITE;
        cfg.text_size   = 8;
        ID_Text         = GLU_FUNC( Object, create )( &cfg );
    }
    
    GLU_FUNC( Object, insert )( ID_Text    );
    GLU_FUNC( Object, insert )( ID_Addr[0] );
    GLU_FUNC( Object, insert )( ID_Addr[1] );
    GLU_FUNC( Object, insert )( ID_Addr[2] );
    GLU_FUNC( Object, insert )( ID_Addr[3] );
    GLU_FUNC( Object, insert )( ID_Addr[4] );

    __GUI_ObjDataScr_num data = {0};

    data.value = p->Addr[0];
    GLU_FUNC( Object, adjust )( ID_Addr[0], &data, sizeof(data) );
    data.value = p->Addr[1];
    GLU_FUNC( Object, adjust )( ID_Addr[1], &data, sizeof(data) );
    data.value = p->Addr[2];
    GLU_FUNC( Object, adjust )( ID_Addr[2], &data, sizeof(data) );
    data.value = p->Addr[3];
    GLU_FUNC( Object, adjust )( ID_Addr[3], &data, sizeof(data) );
    data.value = p->Addr[4];
    GLU_FUNC( Object, adjust )( ID_Addr[4], &data, sizeof(data) );

    GLU_FUNC( GUI, refreashEntireScreen )();
    EventBits_t xResult;

/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT == false ){
        xResult = xEventGroupWaitBits( EGHandle_Hardware, kHWEvent_JoySitck_Up|kHWEvent_JoySitck_Down|kHWEvent_JoySitck_Pressed|kHWEvent_JoySitck_Left|kHWEvent_JoySitck_Right,
                                       pdFALSE,         // 清除该位
                                       pdFALSE,         // 不等待所有指定的Bit, 即逻辑或
                                       portMAX_DELAY ); // 永久等待
        if( xResult&kHWEvent_JoySitck_Pressed ){
            EXIT = true;
        }

        if( xResult&kHWEvent_JoySitck_Left ){
            GLU_FUNC( Object, frame )( ID_Addr[p->bucket+1], false );
            GLU_FUNC( Object, frame )( ID_Addr[p->bucket  ], true  );
            xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Left   );
        }else if( xResult&kHWEvent_JoySitck_Right ){
            GLU_FUNC( Object, frame )( ID_Addr[p->bucket  ], true  );
            GLU_FUNC( Object, frame )( ID_Addr[p->bucket-1], false );
            xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Right );
        }
        
        if( (xResult&kHWEvent_JoySitck_Up) || (xResult&kHWEvent_JoySitck_Down) ){
            for( int i=0; i<p->size; i++ ){
                __GUI_ObjDataScr_num data = { .value = p->Addr[i] };
                GLU_FUNC( Object, adjust )( ID_Addr[i], &data, sizeof(data) );
            }

            xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Up   );
            xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Down );
        }

        
        taskENTER_CRITICAL();
        GLU_FUNC( GUI, refreashScreen )();
        taskEXIT_CRITICAL();
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    GLU_FUNC( Object, delete )( ID_Addr[0] );
    GLU_FUNC( Object, delete )( ID_Addr[1] );
    GLU_FUNC( Object, delete )( ID_Addr[2] );
    GLU_FUNC( Object, delete )( ID_Addr[3] );
    GLU_FUNC( Object, delete )( ID_Addr[4] );
    GLU_FUNC( GUI, refreashEntireScreen )();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1);
}
void MAKE_TASK( subtask, ROOT_Hardware_NRF24L01_RXAddress, CTRL ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( SmartPi.serv_ID == ROOT_Hardware_NRF24L01_RXAddress || SmartPi.serv_ID == ROOT_Hardware_NRF24L01_TXAddress );
#endif

/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    struct{
        uint8_t Addr[5];
        size_t  size;
        int     bucket;
        const char* text;
    }*p = param;
    
    // 调节数据的速度, 用于本任务的阻塞时间
    const TickType_t short_delay = 10, normal_delay=100;
    TickType_t              time = normal_delay;
    // 使用 16Bit 分别记录上下的操作次数 [7:0]|[7:0]
    uint16_t         operation_record = 0x0101;
    
    RH_ASSERT( p->size == sizeof(p->Addr) );
    p->bucket = 0;
/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT==false ){
        taskENTER_CRITICAL();
        EventBits_t xResult = xEventGroupGetBitsFromISR( EGHandle_Hardware );
        if( joystick_data[1] > 4000 /* && p->RX_Addr[ p->bucket ]<0xff */ ){
            p->Addr[ p->bucket ]++;
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Down  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Up    );
            
            operation_record &= 0xff00;
            if( operation_record < (1<<15) ){
                operation_record <<= 1;
                time = normal_delay;
            }else{
                time = short_delay;
            }
            operation_record |= (0x0001);
        }else if( joystick_data[1] < 100 /* && p->RX_Addr[ p->bucket ]>0x00*/  ){
            p->Addr[ p->bucket ]--;
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Up    );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Down  );
            
            operation_record &= 0x00ff;
            if( operation_record < (1<<7) ){
                operation_record <<= 1;
                time = normal_delay;
            }else{
                time = short_delay;
            }
            operation_record |= (0x0100);
        }else{
            operation_record = 0x0101;
            time            = normal_delay;
        }

        if( joystick_data[0] > 4000 && p->bucket<p->size-1 ){
            p->bucket++;
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
            operation_record = 0;
            time            = normal_delay;
            operation_record = 0x0101;
            vTaskDelay(100);
        }else if( joystick_data[0] < 100 && p->bucket>0 ){
            p->bucket--;
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            operation_record = 0x0101;
            time            = normal_delay;
            vTaskDelay(100);
        }

        if( xResult&kHWEvent_JoySitck_Pressed ){
            SmartPi.serv_ID_tmp = 0;
            EXIT = true;
        }
        taskEXIT_CRITICAL();
        vTaskDelay( time );
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    xEventGroupSetBits( EGHandle_Software, kSWEvent_CTRL_Finished );
    while(1);
}

void MAKE_TASK( subtask, ROOT_Hardware_JoyStick, UI   ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == ROOT_Hardware_JoyStick );
#endif
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    ID_t ID_JoyStick = 0;
    ID_t ID_Num_X, ID_Num_Y, ID_Text_X, ID_Text_Y = 0;
    {
        __GUI_Object_t cfg = {0};
        GLU_FUNC( Object, template )( &cfg, kGUI_ObjStyle_joystick );
        cfg.area.xs     = 10;
        cfg.area.ys     = 10;
        cfg.area.height = 45;
        cfg.area.width  = 45;
        // cfg.val[0]      = joystick_data[0];
        // cfg.val[1]      = joystick_data[1];
        // cfg.min[0]      = 0;
        // cfg.max[0]      = 4096;
        // cfg.min[1]      = 0;
        // cfg.max[1]      = 4096;
        cfg.showFrame   = true;
        cfg.obj_color   = M_COLOR_WHITE;
        cfg.bk_color    = M_COLOR_BLACK;
        ID_JoyStick     = GLU_FUNC( Object, create )( &cfg );

        cfg.widget       = kGUI_ObjStyle_num;
        cfg.area.xs     = 90;
        cfg.area.ys     = 20;
        cfg.area.width  = 35;
        cfg.area.height = 12;
        cfg.font        = kGUI_FontStyle_ArialRounded_Bold;
        cfg.obj_color   = M_COLOR_WHITE;
        cfg.text_size   = 8;
        cfg.text_align  = kGUI_FontAlign_Middle;
        cfg.showFrame   = true;
        ID_Num_X        = GLU_FUNC( Object, create )( &cfg );
        
        cfg.area.ys    += cfg.area.height;
        ID_Num_Y        = GLU_FUNC( Object, create )( &cfg );
        
        cfg.widget       = kGUI_ObjStyle_text;
            
        cfg.area.width  = 10;
        cfg.area.xs    -= cfg.area.width-1;
        cfg.text        = "Y";
        ID_Text_Y       = GLU_FUNC( Object, create )( &cfg );
        
        cfg.area.ys    -= cfg.area.height;
        cfg.text        = "X";
        ID_Text_X       = GLU_FUNC( Object, create )( &cfg );
    }
    
    GLU_FUNC( Object, insert )(ID_JoyStick);
    GLU_FUNC( Object, insert )(ID_Num_X);
    GLU_FUNC( Object, insert )(ID_Num_Y);
    GLU_FUNC( Object, insert )(ID_Text_X);
    GLU_FUNC( Object, insert )(ID_Text_Y);
    
/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT == false ){
        EventBits_t xResult = xEventGroupGetBitsFromISR( EGHandle_Hardware );
        if( xResult&kHWEvent_JoySitck_Pressed ){
            EXIT = true;
        }

        // UI显示摇杆数据
        __GUI_ObjDataScr_joystick objdata_joystick = {
            .value    = { joystick_data[0], joystick_data[1] },
            .max      = { 4096, 4096 },
            .min      = {    0,    0 }
        };
        GLU_FUNC( Object, adjust )(ID_JoyStick, &objdata_joystick, sizeof(objdata_joystick));
        
        // UI显示X方向的ADC数据
        __GUI_ObjDataScr_num   objdata_adc = {
            .value = objdata_joystick.value[0]
        };
        GLU_FUNC( Object, adjust )(ID_Num_X, &objdata_adc, sizeof(objdata_adc));

        // UI显示Y方向的ADC数据
        objdata_adc.value = objdata_joystick.value[1];
        GLU_FUNC( Object, adjust )(ID_Num_Y, &objdata_adc, sizeof(objdata_adc));

        GLU_FUNC( GUI, refreashScreen )();
        
        vTaskDelay(10);
    }
    GLU_FUNC( Object, delete )( ID_JoyStick );
    GLU_FUNC( Object, delete )( ID_Num_X    );
    GLU_FUNC( Object, delete )( ID_Num_Y    );
    GLU_FUNC( Object, delete )( ID_Text_X   );
    GLU_FUNC( Object, delete )( ID_Text_Y   );
    GLU_FUNC( GUI, refreashScreen )();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    while(1);
}
void MAKE_TASK( subtask, ROOT_Hardware_JoyStick, CTRL ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == ROOT_Hardware_JoyStick );
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

void MAKE_TASK( subtask, ROOT_Hardware_LED, UI   ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == ROOT_Hardware_LED );
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

        GLU_FUNC( Object, template )( &cfg, kGUI_ObjStyle_switch );

        cfg.area.xs     = 72;
        cfg.area.ys     = 20;
        cfg.area.height = 12;
        cfg.area.width  = 30;
        // cfg.min[0]      = 0;
        // cfg.max[0]      = 256;
        // cfg.val[0]      = 0;
        cfg.showFrame   = false;
        cfg.obj_color   = M_COLOR_WHITE;
        cfg.bk_color    = M_COLOR_BLACK;

        ID_Switch = GLU_FUNC( Object, create )(&cfg);
    }
    
    // Config [ Text ]
    {
        __GUI_Object_t cfg = {0};

        GLU_FUNC( Object, template )( &cfg, kGUI_ObjStyle_text );

        cfg.area.xs     = 20;
        cfg.area.ys     = 20;
        cfg.area.height = 12;
        cfg.area.width  = 40;
        cfg.font        = kGUI_FontStyle_ArialRounded_Bold;
        cfg.obj_color   = M_COLOR_WHITE;
        cfg.text        = "PC13";
        cfg.text_size   = 8;
        cfg.text_align  = kGUI_FontAlign_Left;
        cfg.showFrame   = false;

        cfg.bk_color    = M_COLOR_BLACK;

        ID_Text = GLU_FUNC( Object, create )( &cfg );
        
    }

    GLU_FUNC( Object, insert ) ( ID_Switch );
    GLU_FUNC( Object, insert ) ( ID_Text   );
    GLU_FUNC( GUI, refreashEntireScreen )();
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

        __GUI_ObjDataScr_switch objdata_switch = {
            .cmd = (bool)ans
        };
        GLU_FUNC( Object, adjust )( ID_Switch, &objdata_switch, sizeof(objdata_switch) );
        GLU_FUNC( GUI, refreashScreen )();
        taskEXIT_CRITICAL();
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    GLU_FUNC( Object, delete )( ID_Switch );
    GLU_FUNC( GUI, refreashEntireScreen )();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1);
}
void MAKE_TASK( subtask, ROOT_Hardware_LED, CTRL ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == ROOT_Hardware_LED );
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

void MAKE_TASK( subtask, ROOT_Hardware_Beeper, UI   ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == ROOT_Hardware_Beeper );
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

        GLU_FUNC( Object, template )( &cfg, kGUI_ObjStyle_switch );
        cfg.area.xs     = 72;
        cfg.area.ys     = 20;
        cfg.area.height = 12;
        cfg.area.width  = 30;
        cfg.showFrame   = false;
        cfg.obj_color   = M_COLOR_WHITE;
        cfg.bk_color    = M_COLOR_BLACK;

    
        ID_Switch = GLU_FUNC( Object, create )(&cfg);
    }
    
    // Config [ Text ]
    {
        __GUI_Object_t cfg = {0};

        GLU_FUNC( Object, template )( &cfg, kGUI_ObjStyle_text );

        cfg.area.xs     = 20;
        cfg.area.ys     = 20;
        cfg.area.height = 12;
        cfg.area.width  = 40;
        cfg.font        = kGUI_FontStyle_ArialRounded_Bold;
        cfg.obj_color   = M_COLOR_WHITE;
        cfg.text        = "BEEPER";
        cfg.text_size   = 8;
        cfg.text_align  = kGUI_FontAlign_Left;
        cfg.showFrame   = false;

        cfg.bk_color    = M_COLOR_BLACK;

        ID_Text = GLU_FUNC( Object, create )( &cfg );
        
    }

    GLU_FUNC( Object, insert ) ( ID_Switch );
    GLU_FUNC( Object, insert ) ( ID_Text   );
    GLU_FUNC( GUI, refreashEntireScreen )();
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


        __GUI_ObjDataScr_switch objdata_switch = {
            .cmd = (bool)ans
        };
        GLU_FUNC( Object, adjust )( ID_Switch, &objdata_switch, sizeof(objdata_switch) );
        GLU_FUNC( GUI, refreashScreen )();
        taskEXIT_CRITICAL();
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    GLU_FUNC( Object, delete )( ID_Switch );
    GLU_FUNC( GUI, refreashEntireScreen )();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1);
}
void MAKE_TASK( subtask, ROOT_Hardware_Beeper, CTRL ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( *(typeof(SmartPi.serv_ID)*)param == ROOT_Hardware_Beeper );
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

void MAKE_TASK( subtask, ROOT_Game_Manila, UI   ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( SmartPi.serv_ID == ROOT_Game_Manila );
#endif
    
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    const char* menu_name[] = {
        "ShipDiagram" ,
        "Arrived A"   ,
        "Arrived B"   ,
        "Arrived C"   ,
        "Drown A"     ,
        "Drown B"     ,
        "Drown C"     ,
        "Pirate"
    };
    bool EXIT = false;
    ID_t ID_Menu = 0;
    {
        __GUI_Menu_t cfg = {0};
        cfg.area.xs = 0;
        cfg.area.ys = 0;
        cfg.area.height = RH_CFG_SCREEN_HEIGHT -1;
        cfg.area.width  = RH_CFG_SCREEN_WIDTH  -1;
        cfg.nItem = sizeof(menu_name)/sizeof(const char*);
        cfg.title = "Manila";
        cfg.color_title = M_COLOR_WHITE;
        cfg.size  = 10;
        
        cfg.bk_color    = M_COLOR_BLACK;
        cfg.sl_color    = M_COLOR_WHITE;
        cfg.text_color  = M_COLOR_WHITE;

        __GUI_MenuParam_t *m = alloca(sizeof(__GUI_MenuParam_t)*cfg.nItem);
        for( int8_t i=0; i<cfg.nItem; i++){
            m[i].text = menu_name[i];
        }
        cfg.menuList = m;
    
        ID_Menu = GLU_FUNC( Menu, create )(&cfg);
    }
    GLU_FUNC( Menu, frame )  ( ID_Menu, 0 );
    GLU_FUNC( Menu, insert ) ( ID_Menu );
    GLU_FUNC( GUI, refreashEntireScreen )();
    EventBits_t xResult;

/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT == false ){
        xResult = xEventGroupWaitBits( EGHandle_Hardware, kHWEvent_JoySitck_Up|kHWEvent_JoySitck_Down|kHWEvent_JoySitck_Pressed|kHWEvent_JoySitck_Left,
                                       pdFALSE,         // 清除该位
                                       pdFALSE,         // 不等待所有指定的Bit, 即逻辑或
                                       portMAX_DELAY ); // 永久等待
        if( (xResult&kHWEvent_JoySitck_Left) || (xResult&kHWEvent_JoySitck_Pressed) ){
            EXIT = true;
        }


        int ans = 0;
        if     ( xResult&kHWEvent_JoySitck_Up   )  { ans = -1; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Up   ); }
        else if( xResult&kHWEvent_JoySitck_Down )  { ans =  1; xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Down ); }
        else                                       ans =  0;
        
        taskENTER_CRITICAL();
        GLU_FUNC( Menu, scroll )( ID_Menu, ans );
        GLU_FUNC( GUI, refreashEntireScreen )();
        taskEXIT_CRITICAL();
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    GLU_FUNC( Menu, delete )( ID_Menu );
    GLU_FUNC( GUI, refreashEntireScreen )();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1);

}
void MAKE_TASK( subtask, ROOT_Game_Manila, CTRL ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( SmartPi.serv_ID == ROOT_Game_Manila );
#endif

/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Left    |
                                             kHWEvent_JoySitck_Right   |
                                             kHWEvent_JoySitck_Down    |
                                             kHWEvent_JoySitck_Up      |
                                             kHWEvent_JoySitck_Pressed );
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

        if( joystick_data[0] > M_JOYSTICK_THREASHOLD_RIGHT ){
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
        }else if( joystick_data[0] < M_JOYSTICK_THREASHOLD_LEFT  && joystick_data[1] < M_JOYSTICK_THREASHOLD_UP ){
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

void MAKE_TASK( subtask, ROOT_Game_Manila_ShipDiagram, UI   ) ( void* param ){
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/  
    struct{
        int8_t idx;
        struct {
            int8_t pos;
            int8_t shipment;
        }boat_info[3];
        int8_t round;
    }*cache = param;
    bool EXIT = false;  
    const char* pStrs[] = {
        [ M_MANILA_SHIPMENT_JADE    ] = "Jd" , // Jade
        [ M_MANILA_SHIPMENT_NUTMEG  ] = "Gs" , // Ginseng
        [ M_MANILA_SHIPMENT_SILK    ] = "Sl" , // Silk
        [ M_MANILA_SHIPMENT_GINSENG ] = "Ng"   // Nutmeg
    };

    ID_t ID_Objects[ ] = {
        [0] = 0, // BoatStatus A
        [1] = 0, // BoatStatus B
        [2] = 0, // BoatStatus C
        [3] = 0, // DiceRound 
        [4] = 0, // ShipmentName A
        [5] = 0, // ShipmentName B
        [6] = 0, // ShipmentName C
        [7] = 0, // DiceRound_text
    };
    
    // ID_t ID_BoatStatus[3] = {0};
    // ID_t ID_CargoName[3]  = {0};
    // ID_t ID_DiceRound_txt = 0;
    // ID_t ID_DiceRound_num = 0;
    __GUI_Object_t cfg = {0};

    GLU_FUNC( Object, template )( &cfg, kGUI_ObjStyle_trunk );
    cfg.obj_color  = M_COLOR_WHITE;
    cfg.area.xs    = 10;
    cfg.area.ys    = 3;
    cfg.area.width = 14;
    
    ID_Objects[0] = GLU_FUNC( Object, create )( &cfg );
    cfg.area.xs  += cfg.area.width+2;
    ID_Objects[1] = GLU_FUNC( Object, create )( &cfg );
    cfg.area.xs  += cfg.area.width+2;
    ID_Objects[2] = GLU_FUNC( Object, create )( &cfg );
    
    GLU_FUNC( Object, insert )( ID_Objects[0] );
    GLU_FUNC( Object, insert )( ID_Objects[1] );
    GLU_FUNC( Object, insert )( ID_Objects[2] );
    {
        __GUI_ObjDataScr_trunk data = {
            .min   = 0,
            .max   = 14,
            .value = cache->boat_info[0].pos 
        };
        GLU_FUNC( Object, adjust )( ID_Objects[0], &data, sizeof(data));
        
        data.value = cache->boat_info[1].pos; 
        GLU_FUNC( Object, adjust )( ID_Objects[1], &data, sizeof(data));

        data.value = cache->boat_info[2].pos;
        GLU_FUNC( Object, adjust )( ID_Objects[2], &data, sizeof(data));
    }

    GLU_FUNC( GUI, setPenColor )(M_COLOR_WHITE);
    GLU_FUNC( GUI, line_raw )(67, 7, 67, 57);

    GLU_FUNC( Object, template )(&cfg, kGUI_ObjStyle_text);
    cfg.area.xs     = 10;
    cfg.area.ys     = 52;
    cfg.area.height = 12;
    cfg.area.width  = 14;
    cfg.text        = pStrs[ cache->boat_info[ 0 ].shipment ];
    cfg.showFrame   = false;
    cfg.obj_color   = M_COLOR_WHITE;
    ID_Objects[4] = GLU_FUNC( Object, create )( &cfg );

    cfg.text = pStrs[ cache->boat_info[ 1 ].shipment ];
    cfg.area.xs += cfg.area.width +2;
    ID_Objects[5] = GLU_FUNC( Object, create )( &cfg );

    cfg.text = pStrs[ cache->boat_info[ 2 ].shipment ];
    cfg.area.xs += cfg.area.width +2;
    ID_Objects[6] = GLU_FUNC( Object, create )( &cfg );
    GLU_FUNC( Object, insert )( ID_Objects[4] );
    GLU_FUNC( Object, insert )( ID_Objects[5] );
    GLU_FUNC( Object, insert )( ID_Objects[6] );

    GLU_FUNC( Object, frame )(  ID_Objects[0], true);
    
    cfg.text         = "Round:";
    cfg.area.xs      = 73;
    cfg.area.ys      = 20;
    cfg.area.height  = 11;
    cfg.area.width   = strlen(cfg.text)*6+2;
    cfg.showFrame    = false;
    ID_Objects[7]    = GLU_FUNC( Object, create )( &cfg );
    
    cfg.widget       = kGUI_ObjStyle_num;
    cfg.area.xs     += cfg.area.width;
    cfg.area.width   = 7;
    ID_Objects[3]    = GLU_FUNC( Object, create )( &cfg );
    
    
    GLU_FUNC( Object, insert )( ID_Objects[7] );
    GLU_FUNC( Object, insert )( ID_Objects[3] );
    {
        __GUI_ObjDataScr_num data = {
            .value = cache->round
        };
        GLU_FUNC( Object, adjust )( ID_Objects[3], &data, sizeof(data) );
    }
    
    GLU_FUNC( GUI, refreashEntireScreen )();


    EventBits_t xResult;

/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT == false ){
        typeof( cache->idx ) idx = cache->idx;
        xResult = xEventGroupWaitBits( EGHandle_Hardware, kHWEvent_JoySitck_Right  |
                                                          kHWEvent_JoySitck_Pressed|
                                                          kHWEvent_JoySitck_Left   |
                                                          kHWEvent_JoySitck_Up     |
                                                          kHWEvent_JoySitck_Down   ,
                                       pdFALSE,         // 清除该位
                                       pdFALSE,         // 不等待所有指定的Bit, 即逻辑或
                                       portMAX_DELAY ); // 永久等待
        if( xResult&kHWEvent_JoySitck_Pressed ){
            EXIT = true;
        }

        if( idx != cache->idx ){
            GLU_FUNC( Object, frame )( ID_Objects[ idx        ], false );
            GLU_FUNC( Object, frame )( ID_Objects[ cache->idx ], true  );            
            idx = cache->idx;
        }
 
        switch( cache->idx ){
        case 0:  // Boat A
        case 1:  // Boat B
        case 2:{ // Boat C
            __GUI_ObjDataScr_trunk data = {
                .value = cache->boat_info[ cache->idx ].pos,  
                .max   = M_MANILA_BOATPOS_MAX,
                .min   = 0,
            };
            GLU_FUNC( Object, adjust )( ID_Objects[ cache->idx ], &data, sizeof(data) );
            break; 
        }
        case 3:{ // Round
            __GUI_ObjDataScr_num data = {
                .value = cache->round
            };
            GLU_FUNC( Object, adjust )( ID_Objects[ cache->idx ], &data, sizeof(data) );
            break;
        }
        case 4: // Name A
        case 5: // Name B
        case 6:{// Name C
            __GUI_ObjDataScr_text data = {
                .text = pStrs[ cache->boat_info[ cache->idx-4 ].shipment ]
            };
            GLU_FUNC( Object, adjust )( ID_Objects[ cache->idx ], &data, sizeof(data) );
            break;  
        }          
        
        }
        xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Right  |
                                                  kHWEvent_JoySitck_Left   |
                                                  kHWEvent_JoySitck_Up     |
                                                  kHWEvent_JoySitck_Down    );
        taskENTER_CRITICAL();

        GLU_FUNC( GUI, refreashScreen )();
        taskEXIT_CRITICAL();
    }

/*====================================================================
 * Exit  子任务退出工作
=====================================================================*/
    {
        int8_t cnt = sizeof(ID_Objects)/sizeof(*ID_Objects);
        while(cnt--)
            GLU_FUNC( Object, delete )( ID_Objects[cnt] );
    }
    
    
    GLU_FUNC( GUI, setPenColor )( M_COLOR_BLACK);
    GLU_FUNC( GUI, line_raw )(67, 7, 67, 57);
    GLU_FUNC( GUI, refreashEntireScreen )();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1);
}
void MAKE_TASK( subtask, ROOT_Game_Manila_ShipDiagram, CTRL ) ( void* param ){
#ifdef RH_DEBUG
    RH_ASSERT( SmartPi.serv_ID == ROOT_Game_Manila );
#endif

/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    const int8_t obj_num = 7;
    bool EXIT = false;
    xEventGroupClearBits( EGHandle_Hardware, kHWEvent_JoySitck_Left    |
                                             kHWEvent_JoySitck_Right   |
                                             kHWEvent_JoySitck_Down    |
                                             kHWEvent_JoySitck_Up      |
                                             kHWEvent_JoySitck_Pressed );
    // 导入缓存数据, 需与父任务同步
    struct{
        int8_t idx;
        struct {
            int8_t pos;
            int8_t shipment;
        }boat_info[3];
        int8_t round;
    }*cache = param;
/*====================================================================
 * Loop  子任务循环体
=====================================================================*/
    while( EXIT==false ){
        EventBits_t xResult = xEventGroupGetBitsFromISR( EGHandle_Hardware );

        // X方向 改变写入参数的类型种类
        // Y方向 改变写入参数的数据
        // 因此必须先判断X方向数据, 再判断Y方向, 先确定好选择何种类型, 再确定该种参数调节至多少

        if( joystick_data[0] > M_JOYSTICK_THREASHOLD_RIGHT ){
            if( cache->idx < obj_num-1 )
                cache->idx++;
            else
                cache->idx = 0;
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
            vTaskDelay(100);
        }else if( joystick_data[0] < M_JOYSTICK_THREASHOLD_LEFT ){
            if( cache->idx > 0 )
                cache->idx--;
            else{
                cache->idx = obj_num-1;
            }
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Right );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Left  );
            vTaskDelay(100);
        }

        if( joystick_data[1] > M_JOYSTICK_THREASHOLD_UP ){
            switch( cache->idx ){
                case 0: // Boat A
                case 1: // Boat B
                case 2: // Boat C
                    if( cache->boat_info[ cache->idx ].pos < M_MANILA_BOATPOS_MAX )
                        cache->boat_info[ cache->idx ].pos++;
                    // SMP_Proj_Manila_setboatpos( cache->boat_info[ cache->idx ].shipment, cache->boat_info[ cache->idx ].pos );
                    vTaskDelay(20);
                    break;

                case 3: // Round
                    if( cache->round < M_MANILA_ROUND_MAX )
                        cache->round++;
                    SMP_Proj_Manila_round( cache->round );
                    vTaskDelay(120);
                    break;
                case 4: // Name A
                case 5: // Name B
                case 6: // Name C
                    // 将弃用的运输货品与当前调换
                    __swap( cache->boat_info[ cache->idx-4 ].shipment, Manila->shipment_depricated );
                    // 更新到 Manila
                    Manila->boat[ cache->idx-4 ].shipment = cache->boat_info[ cache->idx-4 ].shipment;
                    vTaskDelay(120);
                    break;            
            }
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Down  );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Up    );
            
        }else if( joystick_data[1] < M_JOYSTICK_THREASHOLD_DOWN ){
            switch( cache->idx ){
                case 0: // Boat A
                case 1: // Boat B
                case 2: // Boat C
                    if( cache->boat_info[ cache->idx ].pos > 0 )
                        cache->boat_info[ cache->idx ].pos--;
                    // SMP_Proj_Manila_setboatpos( cache->boat_info[ cache->idx ].shipment, cache->boat_info[ cache->idx ].pos );
                    vTaskDelay(20);
                    break; 
                case 3: // Round
                    if( cache->round > M_MANILA_ROUND_MIN )
                        cache->round--;
                    SMP_Proj_Manila_round( cache->round );
                    vTaskDelay(120);
                    break;
                case 4: // Name A 
                case 5: // Name B  
                case 6: // Name C
                    // 将弃用的运输货品与当前调换
                    __swap( cache->boat_info[ cache->idx-4 ].shipment, Manila->shipment_depricated );
                    // 更新到 Manila
                    Manila->boat[ cache->idx-4 ].shipment = cache->boat_info[ cache->idx-4 ].shipment;
                    vTaskDelay(120);
                    break;            
            }
            xEventGroupClearBits ( EGHandle_Hardware, kHWEvent_JoySitck_Up    );
            xEventGroupSetBits   ( EGHandle_Hardware, kHWEvent_JoySitck_Down  );
            vTaskDelay(20);
        }

        
        
        if( xResult&kHWEvent_JoySitck_Pressed ){
            for( int8_t i=0; i<3; i++ ){
                // switch( cache->boat_info[ i ].shipment ){
                //     case M_MANILA_SHIPMENT_JADE:
                //         Manila->boatpos_Jade = cache->boat_info[ i ].pos;
                //         break;
                //     case M_MANILA_SHIPMENT_SILK:
                //         Manila->boatpos_Silk = cache->boat_info[ i ].pos;
                //         break;
                //     case M_MANILA_SHIPMENT_NUTMEG:
                //         Manila->boatpos_Nutmeg = cache->boat_info[ i ].pos;
                //         break;
                //     case M_MANILA_SHIPMENT_GINSENG:
                //         Manila->boatpos_Ginseng = cache->boat_info[ i ].pos;
                //         break;
                //     default:
                //         break;
                // }

                Manila->boat[i].pos = cache->boat_info[i].pos;
            }
            Manila->dice_round = cache->round;
            //...//
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

void MAKE_TASK( subtask, default   , UI   )    ( void* param ){
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    bool EXIT = false;
    ID_t ID_Object = 0;

    {
        __GUI_Object_t cfg = {0};

        GLU_FUNC( Object, template )( &cfg, kGUI_ObjStyle_text );
        cfg.area.xs     = 30;
        cfg.area.ys     = 30;
        cfg.area.height = 12;
        cfg.area.width  = 70;
        cfg.font        = kGUI_FontStyle_ArialRounded_Bold;
        cfg.obj_color   = M_COLOR_WHITE;
        cfg.text        = "No preview.";
        cfg.text_size   = 8;
        cfg.text_align  = kGUI_FontAlign_Left;
        cfg.showFrame   = false;

        cfg.bk_color    = M_COLOR_BLACK;

        ID_Object = GLU_FUNC( Object, create )( &cfg );
        
    }
    
    GLU_FUNC( Object, insert )( ID_Object );
    GLU_FUNC( GUI, refreashScreen )();
    
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
    GLU_FUNC( Object, delete )( ID_Object );
    GLU_FUNC( GUI, refreashScreen )();
    xEventGroupSetBits( EGHandle_Software, kSWEvent_UI_Finished );
    //...//

    while(1);
}
void MAKE_TASK( subtask, default   , CTRL )    ( void* param ){
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

#include "nrf24l01.h"
void MAKE_TASK( subtask, radio, recv ) ( void* param ){
/*====================================================================
 * Init  子任务参数初始化
=====================================================================*/
    while(1){

        uint8_t cnt  = 10;  // 最大重收发次数
        uint8_t size = 32;  // 数据包大小 32字节
        if( SmartPi.radio_data ){
            RH_FREE( SmartPi.radio_data );
        }
        SmartPi.radio_data = RH_CALLOC( size, 1 );
    #ifdef RH_DEBUG
        RH_ASSERT( SmartPi.radio_data );
    #endif
        while( --cnt &&  0!=NRF24L01_recv( SmartPi.radio_data, size ));

        if( !cnt ){
            xEventGroupSetBits( EGHandle_Hardware , kHWEvent_NRF24L01_RecvFailed );
        }else{
            xEventGroupSetBits( EGHandle_Hardware , kHWEvent_NRF24L01_RecvDone );
        }

        vTaskDelete(NULL);
        while(1);
    }
}

void MAKE_TASK( subtask, radio, send ) ( void* param ){
    while(1){
        vTaskDelete(NULL);
    }
}

void MAKE_TASK( subtask, radio, check ) ( void* param ){
    while(1){
        vTaskDelay(100);
    }
}















