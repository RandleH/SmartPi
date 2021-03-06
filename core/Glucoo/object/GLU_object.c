
#include "../GLU_glucoo.h"

#include "BLK_graphic.h"

#define GUI_Y_WIDTH                 RH_CFG_SCREEN_HEIGHT
#define GUI_X_WIDTH                 RH_CFG_SCREEN_WIDTH

extern BLK_TYPE(Canvas) info_MainScreen; //...//

static void __gui_remove_object_text      ( const __GUI_Object_t* config ){
    struct{
        __Area_t area;
        bool     showFrame;
    }*pHistory = (void*)config->history;
    
#ifdef RH_DEBUG
    RH_ASSERT( config->widget == kGUI_ObjStyle_text || \
               config->widget == kGUI_ObjStyle_num  || \
               config->widget == kGUI_ObjStyle_fnum  );
#endif
    
    BLK_FUNC( Graph, backupCache )();
    
    if( !pHistory ){
        BLK_FUNC( Graph, set_penColor )( config->bk_color );
        BLK_FUNC( Graph, rect_fill )( config->area.xs+1, \
                           config->area.ys+1, \
                           config->area.xs+(int)config->area.width -1-1, \
                           config->area.ys+(int)config->area.height-1-1, \
                           &info_MainScreen, kApplyPixel_fill);
        if( config->showFrame ){
            BLK_FUNC( Graph, set_penColor )( config->bk_color );
            BLK_FUNC( Graph, rect_raw )( config->area.xs, \
                              config->area.ys, \
                              config->area.xs+(int)config->area.width -1, \
                              config->area.ys+(int)config->area.height-1, \
                              &info_MainScreen, kApplyPixel_fill);
        }
    }else{
    #ifdef RH_DEBUG
        RH_ASSERT( pHistory->area.xs >= config->area.xs );
        RH_ASSERT( pHistory->area.ys >= config->area.ys );
        RH_ASSERT( pHistory->area.xs+pHistory->area.width  <= config->area.xs+config->area.width  );
        RH_ASSERT( pHistory->area.ys+pHistory->area.height <= config->area.ys+config->area.height );
    #endif
        BLK_FUNC( Graph, set_penColor )( config->bk_color );
        BLK_FUNC( Graph, rect_fill )( pHistory->area.xs, \
                           pHistory->area.ys, \
                           pHistory->area.xs+(int)pHistory->area.width -1, \
                           pHistory->area.ys+(int)pHistory->area.height-1, \
                           &info_MainScreen, kApplyPixel_fill);
        if( pHistory->showFrame && !config->showFrame ){
            BLK_FUNC( Graph, set_penColor )( config->bk_color );
            BLK_FUNC( Graph, rect_raw )( config->area.xs, \
                              config->area.ys, \
                              config->area.xs+(int)config->area.width -1, \
                              config->area.ys+(int)config->area.height-1, \
                              &info_MainScreen, kApplyPixel_fill);
        }
    }
    
    BLK_FUNC( Graph, restoreCache )();
}
static void __gui_insert_object_text      ( const __GUI_Object_t* config ){
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( config->font < kGUI_NUM_FontStyle );
    RH_ASSERT( config->text );
    RH_ASSERT( config->widget == kGUI_ObjStyle_text );
#endif
    
    __gui_remove_object_text(config);

    struct{
        __Area_t area;
        bool     showFrame;
    }*pHistory = (void*)config->history;
    
    if( !pHistory ){
        pHistory = RH_MALLOC(sizeof(*pHistory));
        __SET_STRUCT_MB(__GUI_Object_t, void*, config, history, pHistory);
    }
    
    BLK_FUNC( Graph, backupCache )();
    __Font_backup_config();
    
    __Font_setSize(config->text_size);
    int cnt = __Font_getWordNum( config->area.width, config->text );

    char* p = NULL;
    if(cnt>0){
        p = alloca( cnt+sizeof('\0') );
        strncpy(p, config->text, cnt);
        p[cnt] = '\0';
        __GUI_Font_t* pF = __Font_exportStr(p);
    #ifdef RH_DEBUG
        RH_ASSERT( pF );
        RH_ASSERT( pF->output );
        RH_ASSERT( pF->width < config->area.width );
    #endif
        /* ????????????????????????????????????, ??????????????? , ????????????history, ???????????????????????? */
        int x_fs = 0;
        int y_fs = pHistory->area.ys = __limit( config->area.ys +(((int)(config->area.height - config->text_size))>>1) , 0, GUI_Y_WIDTH-1 );
        
        switch ( config->text_align ) {
            case kGUI_FontAlign_Left:
                x_fs = pHistory->area.xs = __limit( config->area.xs +(((int)(config->area.height - config->text_size))>>1) , 0, GUI_X_WIDTH-1 );
                break;
            case kGUI_FontAlign_Middle:
                x_fs = pHistory->area.xs = __limit( config->area.xs +(((int)(config->area.width - pF->width))>>1) , 0, GUI_X_WIDTH-1   );
                break;
            default:
                RH_ASSERT(0);
        }
        GLU_UION(Pixel) color_text = {.data = config->obj_color};
        
    #if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
        /* ????????????????????????pIter */
        uint8_t* pIter = pF->output;
        for( int y=0; y<pF->height&&y<config->area.height; y++ ){
            for( int x=0; x<pF->width; x++, pIter++ ){
                size_t index = ((y_fs+y)>>3)*(info_MainScreen.width)+(x_fs+x);
                if( (*pIter<128) ^ (color_text.data!=0) ){
                    info_MainScreen.pBuffer[ index ].data = __BIT_SET( info_MainScreen.pBuffer[ index ].data, (y_fs+y)%8 );
                }else{
                    info_MainScreen.pBuffer[ index ].data = __BIT_CLR( info_MainScreen.pBuffer[ index ].data, (y_fs+y)%8 );
                }
            
            }
        }
    #elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
        for( int y=0; y<pF->height&&y<config->area.height; y++ ){
            for( int x=0; x<pF->width; x++ ){
                size_t index = (y_fs+y)*(info_MainScreen.width)+(x_fs+x);
                uint8_t pixWeight = pF->output[y*pF->width+x];
                info_MainScreen.pBuffer[ index ].R = info_MainScreen.pBuffer[ index ].R + (( (color_text.R - info_MainScreen.pBuffer[ index ].R) * pixWeight )>>8);
                info_MainScreen.pBuffer[ index ].G = info_MainScreen.pBuffer[ index ].G + (( (color_text.G - info_MainScreen.pBuffer[ index ].G) * pixWeight )>>8);
                info_MainScreen.pBuffer[ index ].B = info_MainScreen.pBuffer[ index ].B + (( (color_text.B - info_MainScreen.pBuffer[ index ].B) * pixWeight )>>8);
            }
        }
    #elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
        RH_ASSERT(0);
    #else
         
    #endif
        
        /* ?????????????????????history, ???????????????????????? */
        pHistory->area.height = __min( pF->height, config->area.height );
        pHistory->area.width  = pF->width;
    }
    
    

    if( config->showFrame ){
        BLK_FUNC( Graph, set_penColor )( config->obj_color );
        BLK_FUNC( Graph, rect_raw )( config->area.xs, \
                          config->area.ys, \
                          config->area.xs+(int)(config->area.width )-1, \
                          config->area.ys+(int)(config->area.height)-1, \
                          &info_MainScreen, kApplyPixel_fill);

    }
    pHistory->showFrame = config->showFrame;
    
    BLK_FUNC( Graph, restoreCache )();
    __Font_restore_config();
    
}
static void __gui_adjust_object_text      ( const __GUI_Object_t* config ){
    struct __GUI_ObjDataScr_text* p = config->dataScr;
//    config->text = p->text;
    __SET_STRUCT_MB(__GUI_Object_t, char*, config, text, p->text);
    __gui_insert_object_text( config );
}

static void __gui_remove_object_num       ( const __GUI_Object_t* config ){
    __gui_remove_object_text(config);
}
static void __gui_insert_object_num       ( const __GUI_Object_t* config ){
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( config->font < kGUI_NUM_FontStyle );
    RH_ASSERT( config->widget == kGUI_ObjStyle_num );
#endif
    __gui_remove_object_num(config);
    
    __Font_backup_config();
    BLK_FUNC( Graph, backupCache )();
    
    struct{
        __Area_t area;
        bool     showFrame;
    }*pHistory = (void*)config->history;
    
    if( !pHistory ){
        pHistory = RH_MALLOC(sizeof(*pHistory));
        __SET_STRUCT_MB(__GUI_Object_t, void*, config, history, pHistory);
    }
    
    // ????????????
    char __str[GUI_X_WIDTH>>2] = {0};
    __Font_setSize(config->text_size);
    
    // ???????????????????????????????????????
    int wordCnt = snprintf(__str, sizeof(__str), "%d",((struct __GUI_ObjDataScr_num*)config->dataScr)->value);
    
    // ??????????????????????????????(width)?????????????????????, ??????????????????????????????
    int maxWordCnt = __Font_getWordNum(config->area.width, __str);
    // ??????????????????????????????
    __str[ maxWordCnt ] = '\0';
    
    // ???????????????????????????????????????????????????????????????: [???] ??????????????????3?????????  123 | 90 | 1 ?????? ??? 1234 | 4253 ????????????????????????
    bool isEnough = wordCnt <= maxWordCnt;
    
    // ?????????????????????????????????, ???????????????#???, ?????????????????????
    if( !isEnough){
        memset(__str, '#', maxWordCnt);
    }
 
    
    
    if(__str[0]!='\0'){

        __GUI_Font_t* pF = __Font_exportStr(__str);
        
        /* ????????????????????????????????????, ??????????????? , ????????????history, ???????????????????????? */
        
        int x_fs = 0;
        int y_fs = pHistory->area.ys = __limit( config->area.ys +(((int)(config->area.height - config->text_size))>>1) , 0, GUI_Y_WIDTH-1 );
        switch ( config->text_align ) {
            case kGUI_FontAlign_Left:
                x_fs = pHistory->area.xs = __limit( config->area.xs +(((int)(config->area.height - config->text_size))>>1) , 0, GUI_X_WIDTH-1 );
                break;
            case kGUI_FontAlign_Middle:
                x_fs = pHistory->area.xs = __limit( config->area.xs +(((int)(config->area.width - pF->width))>>1) , 0, GUI_X_WIDTH-1   );
                break;
            default:
                RH_ASSERT(0);
        }
        GLU_UION(Pixel) color_text = {.data = config->obj_color};
    
    #if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
        uint8_t* pIter = pF->output;
        for( int y=0; y<pF->height && y<config->area.height; y++ ){
            for( int x=0; x<pF->width; x++, pIter++ ){
                size_t index = ((y_fs+y)>>3)*(info_MainScreen.width)+(x_fs+x);
                if( (*pIter<128) ^ (color_text.data!=0) ){
                    info_MainScreen.pBuffer[ index ].data = __BIT_SET( info_MainScreen.pBuffer[ index ].data, (y_fs+y)%8 );
                }else{
                    info_MainScreen.pBuffer[ index ].data = __BIT_CLR( info_MainScreen.pBuffer[ index ].data, (y_fs+y)%8 );
                }
            
            }
        }
    #elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
        for( int y=0; y<pF->height&&y<config->area.height; y++ ){
            for( int x=0; x<pF->width; x++ ){
                size_t index = (y_fs+y)*(info_MainScreen.width)+(x_fs+x);
                uint8_t pixWeight = pF->output[y*pF->width+x];
                info_MainScreen.pBuffer[ index ].R = info_MainScreen.pBuffer[ index ].R + (( (color_text.R - info_MainScreen.pBuffer[ index ].R) * pixWeight )>>8);
                info_MainScreen.pBuffer[ index ].G = info_MainScreen.pBuffer[ index ].G + (( (color_text.G - info_MainScreen.pBuffer[ index ].G) * pixWeight )>>8);
                info_MainScreen.pBuffer[ index ].B = info_MainScreen.pBuffer[ index ].B + (( (color_text.B - info_MainScreen.pBuffer[ index ].B) * pixWeight )>>8);
            }
        }
    #elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
        RH_ASSERT(0);
    #else
         
    #endif
        /* ?????????????????????history, ???????????????????????? */
        pHistory->area.height = __min( pF->height, config->area.height );
        pHistory->area.width  = pF->width;
    }

    if( config->showFrame ){
        BLK_FUNC( Graph, set_penColor )( M_COLOR_WHITE );
        BLK_FUNC( Graph, rect_raw )( config->area.xs, \
                          config->area.ys, \
                          config->area.xs+(int)(config->area.width )-1, \
                          config->area.ys+(int)(config->area.height)-1, \
                          &info_MainScreen, kApplyPixel_fill);

    }
    pHistory->showFrame = config->showFrame;

    __Font_restore_config();
    BLK_FUNC( Graph, restoreCache )();
}
static void __gui_adjust_object_num       ( const __GUI_Object_t* config ){
    __gui_insert_object_num( config );
}

static void __gui_remove_object_fnum      ( const __GUI_Object_t* config ){
    __gui_remove_object_text(config);
}
static void __gui_insert_object_fnum      ( const __GUI_Object_t* config ){
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( config->font < kGUI_NUM_FontStyle );
    RH_ASSERT( config->widget == kGUI_ObjStyle_fnum );
#endif
    __gui_remove_object_fnum(config);
    
    // ????????????????????????
    struct{
        __Area_t area;
        bool     showFrame;
    }*pHistory = (void*)config->history;
    
    if( !pHistory ){
        pHistory = RH_MALLOC(sizeof(*pHistory));
        __SET_STRUCT_MB(__GUI_Object_t, void*, config, history, pHistory);
    }
    
    
    __Font_backup_config();
    BLK_FUNC( Graph, backupCache )();
    
    char __str[GUI_X_WIDTH>>2] = {'\0'};
    __Font_setSize(config->text_size);
    snprintf(__str, sizeof(__str), "%.3f",((struct __GUI_ObjDataScr_fnum*)config->dataScr)->value);
    
    // ??????????????????????????????(width)?????????????????????, ??????????????????????????????
    int maxWordCnt = __Font_getWordNum(config->area.width, __str);
    // ??????????????????????????????
    __str[ maxWordCnt ] = '\0';
    
    // ?????????????????????????????????????????????????????????????????????: [???] ??????????????????3?????????  12.34 | 90.001 | 123.123 ?????? ??? 1234.567 | 4253.000 ????????????????????????
    
    bool isEnough = false;
    for ( int i=0; i<strlen(__str)&&i<maxWordCnt; i++) {
        if( __str[i] == '.' ){
            if( __str[i+1] == '\0' ) // ???????????????????????????
                __str[i] = '\0'; // ???????????????????????????
            isEnough = true;
            break;
        }
    }
    // ?????????????????????????????????, ???????????????#???, ?????????????????????
    if( !isEnough){
        memset(__str, '#', maxWordCnt);
    }
    
    __GUI_Font_t* pF = __Font_exportStr(__str);
    /* ????????????????????????????????????, ??????????????? , ????????????history, ???????????????????????? */
    int x_fs = 0;
    int y_fs = pHistory->area.ys = __limit( config->area.ys +(((int)(config->area.height - config->text_size))>>1) , 0, GUI_Y_WIDTH-1 );
    switch ( config->text_align ) {
        case kGUI_FontAlign_Left:
            x_fs = pHistory->area.xs = __limit( config->area.xs +(((int)(config->area.height - config->text_size))>>1) , 0, GUI_X_WIDTH-1 );
            break;
        case kGUI_FontAlign_Middle:
            x_fs = pHistory->area.xs = __limit( config->area.xs +(((int)(config->area.width - pF->width))>>1) , 0, GUI_X_WIDTH-1   );
            break;
        default:
            RH_ASSERT(0);
    }
    GLU_UION(Pixel) color_text = {.data = config->obj_color};
    
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
    uint8_t*       pIterFont = pF->output;
    
    GLU_UION(Pixel)* pIterScr  = info_MainScreen.pBuffer + (y_fs>>3)*(info_MainScreen.width)+x_fs;
    
    for( int y=0; y<pF->height && y<config->area.height; y++ ){
        for( int x=0; x<pF->width; x++, pIterFont++, pIterScr++ ){
            if( (*pIterFont<128) ^ (color_text.data!=0) ){
                pIterScr->data = __BIT_SET( pIterScr->data, (y_fs+y)%8 );
            }else{
                pIterScr->data = __BIT_CLR( pIterScr->data, (y_fs+y)%8 );
            }
        }
        
        pIterScr -= pF->width;
        if( ((y_fs+y+1)>>3) > ((y_fs+y)>>3) ){
            pIterScr += info_MainScreen.width;
        }
    }
    
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
    for( int y=0; y<pF->height&&y<config->area.height; y++ ){
        for( int x=0; x<pF->width; x++ ){
            size_t index = (y_fs+y)*(info_MainScreen.width)+(x_fs+x);
            uint8_t pixWeight = pF->output[y*pF->width+x];
            info_MainScreen.pBuffer[ index ].R = info_MainScreen.pBuffer[ index ].R + (( (color_text.R - info_MainScreen.pBuffer[ index ].R) * pixWeight )>>8);
            info_MainScreen.pBuffer[ index ].G = info_MainScreen.pBuffer[ index ].G + (( (color_text.G - info_MainScreen.pBuffer[ index ].G) * pixWeight )>>8);
            info_MainScreen.pBuffer[ index ].B = info_MainScreen.pBuffer[ index ].B + (( (color_text.B - info_MainScreen.pBuffer[ index ].B) * pixWeight )>>8);
        }
    }
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
    RH_ASSERT(0);
#else
     
#endif
    /* ?????????????????????history, ???????????????????????? */
    pHistory->area.height = __min( pF->height, config->area.height );
    pHistory->area.width  = pF->width;
        
    
    
    if( config->showFrame ){
        BLK_FUNC( Graph, set_penColor )( M_COLOR_WHITE );
        BLK_FUNC( Graph, rect_raw )( config->area.xs, \
                          config->area.ys, \
                          config->area.xs+(int)(config->area.width )-1, \
                          config->area.ys+(int)(config->area.height)-1, \
                          &info_MainScreen, kApplyPixel_fill);

    }
    pHistory->showFrame = config->showFrame;

    __Font_restore_config();
    BLK_FUNC( Graph, restoreCache )();
}
static void __gui_adjust_object_fnum      ( const __GUI_Object_t* config ){
    __gui_insert_object_fnum( config );
}

static void __gui_remove_object_switch    ( const __GUI_Object_t* config ){
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( config->widget == kGUI_ObjStyle_switch );
#endif
    
    // ????????????????????????
    struct{
        bool     switchState;
        bool     showFrame;
    }*pHistory = (void*)config->history;
    
    __Font_backup_config();
    BLK_FUNC( Graph, backupCache )();
    
    bool needRemove = !pHistory;
    if( pHistory ){
        needRemove |= (pHistory->switchState != ((int32_t)(config->val)!=0));
        needRemove |= (pHistory->showFrame)^(config->showFrame);
    }
    
    if( needRemove ){
        BLK_FUNC( Graph, set_penColor )(config->bk_color);
        __Area_t area = config->area;
        area.width  -=4;
        area.height -=4;
        area.xs     +=2;
        area.ys     +=2;
        BLK_FUNC( Graph, sausage_fill )( area.xs, \
                              area.ys, \
                              area.xs+(int)(area.width  -1), \
                              area.ys+(int)(area.height -1), \
                              &info_MainScreen, kApplyPixel_fill);
        BLK_FUNC( Graph, rect_raw )( config->area.xs, \
                          config->area.ys, \
                          config->area.xs+(int)(config->area.width )-1, \
                          config->area.ys+(int)(config->area.height)-1, \
                          &info_MainScreen, kApplyPixel_fill);
    }
    
    __Font_restore_config();
    BLK_FUNC( Graph, restoreCache )();
}
static void __gui_insert_object_switch    ( const __GUI_Object_t* config ){
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( config->widget == kGUI_ObjStyle_switch );
#endif
    // ????????????????????????
    struct{
        bool     switchState;
        bool     showFrame;
    }*pHistory = (void*)config->history;
    
    if( !pHistory ){
        pHistory = RH_MALLOC(sizeof(*pHistory));
    }
#ifdef RH_DEBUG
    RH_ASSERT( pHistory );
#endif
    
    __gui_remove_object_switch(config);
    
    __Font_backup_config();
    BLK_FUNC( Graph, backupCache )();
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
    GLU_UION(Pixel) color_switch_on  = {.data = (config->bk_color==0x00)?0xff:0x00};
//    GLU_UION(Pixel) color_switch_off = {.data = (config->bk_color==0x00)?0x00:0xff};
    GLU_UION(Pixel) color_switch     = {.data = 0x00};
    //...//
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
    GLU_UION(Pixel) color_switch_on  = {.data = config->obj_color };
    GLU_UION(Pixel) color_switch_off = {.data = M_COLOR_COAL      };
    GLU_UION(Pixel) color_switch     = {.data = M_COLOR_WHITESMOKE     };
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
#else
  #error "[RH_graphic]: Unknown color type."
#endif
    BLK_FUNC( Graph, set_penColor )( color_switch_on.data );
    
    __Area_t area = config->area;
    area.width  -=4;
    area.height -=4;
    area.xs     +=2;
    area.ys     +=2;
    
    if( ((struct __GUI_ObjDataScr_switch*)config->dataScr)->cmd ){
        BLK_FUNC( Graph, set_penColor )( color_switch_on.data );
        BLK_FUNC( Graph, sausage_fill )( area.xs, \
                              area.ys, \
                              area.xs+(int)(area.width  -1), \
                              area.ys+(int)(area.height -1), \
                              &info_MainScreen, kApplyPixel_fill);
        
        // ???????????????
        BLK_FUNC( Graph, set_penColor )( color_switch.data );
        BLK_FUNC( Graph, circle_fill ) ( area.xs+(int)(area.width)-(int)(area.height>>1)-1, \
                              area.ys+(int)(area.height>>1)-(area.height%2==0) , \
                              (int)area.height                   , \
                              &info_MainScreen, kApplyPixel_fill);
        
        BLK_FUNC( Graph, set_penColor )( color_switch_on.data );
        BLK_FUNC( Graph, circle_raw )  ( area.xs+(int)(area.width)-(int)(area.height>>1)-1, \
                              area.ys+(int)(area.height>>1)-(area.height%2==0) , \
                              (int)area.height                   , \
                              &info_MainScreen, kApplyPixel_fill);

        pHistory->switchState = true;
    }else{
        // ???????????????????????????????????????
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
        BLK_FUNC( Graph, set_penColor )( color_switch_on.data );
        BLK_FUNC( Graph, sausage_raw ) ( area.xs, \
                              area.ys, \
                              area.xs+(int)(area.width  -1), \
                              area.ys+(int)(area.height -1), \
                              &info_MainScreen, kApplyPixel_fill);
        
        BLK_FUNC( Graph, circle_raw )  ( area.xs+(int)(area.height>>1), \
                              area.ys+(int)(area.height>>1)-(area.height%2==0), \
                              (int)area.height                   , \
                              &info_MainScreen, kApplyPixel_fill);
#else
        BLK_FUNC( Graph, set_penColor )( color_switch_off.data );
        BLK_FUNC( Graph, sausage_fill )( area.xs, \
                              area.ys, \
                              area.xs+(int)(area.width  -1), \
                              area.ys+(int)(area.height -1), \
                              &info_MainScreen, kApplyPixel_fill);
        
        BLK_FUNC( Graph, set_penColor )( color_switch.data );
        BLK_FUNC( Graph, circle_fill ) ( area.xs+(int)(area.height>>1), \
                              area.ys+(int)(area.height>>1)-(area.height%2==0), \
                              (int)area.height                   , \
                              &info_MainScreen, kApplyPixel_fill);
        
        BLK_FUNC( Graph, set_penColor )( color_switch_off.data );
        BLK_FUNC( Graph, circle_raw )  ( area.xs+(int)(area.height>>1), \
                              area.ys+(int)(area.height>>1)-(area.height%2==0), \
                              (int)area.height                   , \
                              &info_MainScreen, kApplyPixel_fill);
        
#endif
        pHistory->switchState = false;
    }
    
    __SET_STRUCT_MB(__GUI_Object_t, void*, config, history, pHistory);
    
    __Font_restore_config();
    BLK_FUNC( Graph, restoreCache )();
}
static void __gui_adjust_object_switch    ( const __GUI_Object_t* config ){
    __gui_insert_object_switch(config);
}

static void __gui_remove_object_bar_h     ( const __GUI_Object_t* config ){
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( config->widget == kGUI_ObjStyle_barH );
#endif
    
    // ????????????????????????
    struct{
        int     bar_pos; /* ??????????????????????????????????????????(?????????) */
    }*pHistory = (void*)config->history;
    
    int32_t val = ((struct __GUI_ObjDataScr_barH*)config->dataScr)->value;
    int32_t min = ((struct __GUI_ObjDataScr_barH*)config->dataScr)->min;
    int32_t max = ((struct __GUI_ObjDataScr_barH*)config->dataScr)->max;
    val = __limit(val, min, max);
    int bar_pos = config->area.xs + val*(int)config->area.width/(max-min);
    __Font_backup_config();
    BLK_FUNC( Graph, backupCache )();
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
    GLU_UION(Pixel) color_bar_off = {.data = (config->bk_color==0x00)?0x00:0xff};
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
    GLU_UION(Pixel) color_bar_off = {.data = config->bk_color};
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
#else
  #error "[RH_graphic]: Unknown color type."
#endif
    if( !pHistory ){
        BLK_FUNC( Graph, set_penColor )( color_bar_off.data );
        BLK_FUNC( Graph, rect_fill )( config->area.xs+1, \
                           config->area.ys+1, \
                           config->area.xs+(int)(config->area.width )-1-1, \
                           config->area.ys+(int)(config->area.height)-1-1, \
                           &info_MainScreen, kApplyPixel_fill);
    }else if(pHistory->bar_pos > bar_pos){
        BLK_FUNC( Graph, set_penColor )( color_bar_off.data );
        BLK_FUNC( Graph, rect_fill )( bar_pos, \
                           config->area.ys+1, \
                           pHistory->bar_pos, \
                           config->area.ys+(int)(config->area.height)-1-1, \
                           &info_MainScreen, kApplyPixel_fill);
    }
    
    __Font_restore_config();
    BLK_FUNC( Graph, restoreCache )();
}
static void __gui_insert_object_bar_h     ( const __GUI_Object_t* config ){
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( config->widget == kGUI_ObjStyle_barH );
#endif
    // ????????????????????????
    struct{
        int     bar_pos; /* ??????????????????????????????????????????(?????????) */
        
    }*pHistory = (void*)config->history;

    __gui_remove_object_bar_h(config);
    
    if( !pHistory ){
        pHistory = RH_CALLOC(sizeof(*pHistory),1);
    #ifdef RH_DEBUG
        RH_ASSERT( pHistory );
    #endif
        pHistory->bar_pos = config->area.xs;
        __SET_STRUCT_MB(__GUI_Object_t, void*, config, history, pHistory );
    }

    __Font_backup_config();
    BLK_FUNC( Graph, backupCache )();
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
    GLU_UION(Pixel) color_bar_on  = {.data = (config->bk_color==0x00)?0xff:0x00};
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
    GLU_UION(Pixel) color_bar_on  = {.data = config->obj_color};
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
#else
  #error "[RH_graphic]: Unknown color type."
#endif
    int32_t val = ((struct __GUI_ObjDataScr_barH*)config->dataScr)->value;
    int32_t min = ((struct __GUI_ObjDataScr_barH*)config->dataScr)->min;
    int32_t max = ((struct __GUI_ObjDataScr_barH*)config->dataScr)->max;
    val = __limit(val, min, max);
    
    int bar_pos = config->area.xs + val*(int)config->area.width/(max-min);
    
    BLK_FUNC( Graph, set_penColor )( color_bar_on.data );
    BLK_FUNC( Graph, rect_raw )( config->area.xs, \
                      config->area.ys, \
                      config->area.xs+(int)(config->area.width )-1, \
                      config->area.ys+(int)(config->area.height)-1, \
                      &info_MainScreen, kApplyPixel_fill);
    
    if( pHistory->bar_pos < bar_pos ){
        BLK_FUNC( Graph, set_penColor )( color_bar_on.data );
        BLK_FUNC( Graph, rect_fill )( pHistory->bar_pos, \
                           config->area.ys+1, \
                           bar_pos, \
                           config->area.ys+(int)(config->area.height)-1-1, \
                           &info_MainScreen, kApplyPixel_fill);
    }

    pHistory->bar_pos = bar_pos;
    
    __Font_restore_config();
    BLK_FUNC( Graph, restoreCache )();
}
static void __gui_adjust_object_bar_h     ( const __GUI_Object_t* config ){
    __gui_insert_object_bar_h(config);
}

static void __gui_remove_object_bar_v     ( const __GUI_Object_t* config ){
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( config->widget == kGUI_ObjStyle_barV );
#endif
    // ????????????????????????
    struct{
        int     bar_pos; /* ??????????????????????????????????????????(?????????) */
    }*pHistory = (void*)config->history;
    
    int32_t min = ((struct __GUI_ObjDataScr_barH*)config->dataScr)->min;
    int32_t max = ((struct __GUI_ObjDataScr_barH*)config->dataScr)->max;
    int32_t val = __limit(((struct __GUI_ObjDataScr_barH*)config->dataScr)->value, min, max);
    int bar_pos = config->area.ys + (int)config->area.height-1 - val*(int)config->area.height/(max-min);
    
    __Font_backup_config();
    BLK_FUNC( Graph, backupCache )();
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
    GLU_UION(Pixel) color_bar_off = {.data = (config->bk_color==0x00)?0x00:0xff};
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
    GLU_UION(Pixel) color_bar_off = {.data = config->bk_color};
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
#else
  #error "[RH_graphic]: Unknown color type."
#endif
    if( !pHistory ){
        BLK_FUNC( Graph, set_penColor )( color_bar_off.data );
        BLK_FUNC( Graph, rect_fill )( config->area.xs+1, \
                           config->area.ys+1, \
                           config->area.xs+(int)(config->area.width )-1-1, \
                           config->area.ys+(int)(config->area.height)-1-1, \
                           &info_MainScreen, kApplyPixel_fill);
    }else if(pHistory->bar_pos < bar_pos){
        BLK_FUNC( Graph, set_penColor )( color_bar_off.data );
        BLK_FUNC( Graph, rect_fill )( config->area.xs+1, \
                           pHistory->bar_pos, \
                           config->area.xs+(int)(config->area.width )-1-1, \
                           bar_pos, \
                           &info_MainScreen, kApplyPixel_fill);
    }
    
    __Font_restore_config();
    BLK_FUNC( Graph, restoreCache )();
}
static void __gui_insert_object_bar_v     ( const __GUI_Object_t* config ){
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( config->widget == kGUI_ObjStyle_barV );
#endif
    // ????????????????????????
    struct{
        int     bar_pos; /* ??????????????????????????????????????????(?????????) */
        
    }*pHistory = (void*)config->history;

    __gui_remove_object_bar_v(config);
    
    if( !pHistory ){
        pHistory = RH_CALLOC(sizeof(*pHistory),1);
    #ifdef RH_DEBUG
        RH_ASSERT( pHistory );
    #endif
        pHistory->bar_pos = config->area.ys + (int)config->area.height-1;
        __SET_STRUCT_MB(__GUI_Object_t, void*, config, history, pHistory );
    }
    
    int32_t val = ((struct __GUI_ObjDataScr_barH*)config->dataScr)->value;
    int32_t min = ((struct __GUI_ObjDataScr_barH*)config->dataScr)->min;
    int32_t max = ((struct __GUI_ObjDataScr_barH*)config->dataScr)->max;
    val = __limit(val, min, max);
    int bar_pos = config->area.ys + (int)config->area.height-1 - val*(int)config->area.height/(max-min);
    
    __Font_backup_config();
    BLK_FUNC( Graph, backupCache )();
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
    GLU_UION(Pixel) color_bar_on  = {.data = (config->bk_color==0x00)?0xff:0x00};
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
    GLU_UION(Pixel) color_bar_on  = {.data = config->obj_color};
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
#else
  #error "[RH_graphic]: Unknown color type."
#endif
    BLK_FUNC( Graph, set_penColor )( color_bar_on.data );
    BLK_FUNC( Graph, rect_raw )( config->area.xs, \
                      config->area.ys, \
                      config->area.xs+(int)(config->area.width )-1, \
                      config->area.ys+(int)(config->area.height)-1, \
                      &info_MainScreen, kApplyPixel_fill);
    
    if( pHistory->bar_pos > bar_pos ){
        BLK_FUNC( Graph, set_penColor )( color_bar_on.data );
        BLK_FUNC( Graph, rect_fill )( config->area.xs+1, \
                           bar_pos, \
                           config->area.xs+(int)(config->area.width)-1, \
                           pHistory->bar_pos, \
                           &info_MainScreen, kApplyPixel_fill);
    }

    pHistory->bar_pos = bar_pos;
    
    __Font_restore_config();
    BLK_FUNC( Graph, restoreCache )();
    
}
static void __gui_adjust_object_bar_v     ( const __GUI_Object_t* config ){
    __gui_insert_object_bar_v(config);
}

static void __gui_remove_object_joystick  ( const __GUI_Object_t* config ){
    struct{
        int      cord; // (x,y)????????????
        __Area_t area;
    }*pHistory = (void*)config->history;
    __Font_backup_config();
    BLK_FUNC( Graph, backupCache )();
    BLK_FUNC( Graph, set_penColor )( config->bk_color );
    if( !pHistory ){
        
        BLK_FUNC( Graph, rect_fill )( config->area.xs, \
                           config->area.ys, \
                           config->area.xs+(int)(config->area.width )-1, \
                           config->area.ys+(int)(config->area.height)-1, \
                           &info_MainScreen, kApplyPixel_fill);
    }else{
        
        BLK_FUNC( Graph, rect_fill )( pHistory->area.xs, \
                           pHistory->area.ys, \
                           pHistory->area.xs+(int)(pHistory->area.width )-1, \
                           pHistory->area.ys+(int)(pHistory->area.height)-1, \
                           &info_MainScreen, kApplyPixel_fill);
    }
    __Font_restore_config();
    BLK_FUNC( Graph, restoreCache )();
}
static void __gui_insert_object_joystick  ( const __GUI_Object_t* config ){
    struct{
        int      cord; // (x,y)????????????
        __Area_t area;
    }*pHistory = (void*)config->history;
    
    
    __gui_remove_object_joystick(config);
    
    __Font_backup_config();
    BLK_FUNC( Graph, backupCache )();
    
    BLK_FUNC( Graph, set_penColor )( config->obj_color );
    
    
    int D = (int)__min( config->area.height , config->area.width );
    bool eps = ((D&0x01)==0);
    int X = (int)( config->area.xs + (D>>1) - eps );
    int Y = (int)( config->area.ys + (D>>1) - eps  );
//    BLK_FUNC( Graph, circle_raw )( X, Y, D, &info_MainScreen, kApplyPixel_fill );
     if(  !pHistory  ){
         BLK_FUNC( Graph, circle_raw )( X, Y, D, &info_MainScreen, kApplyPixel_fill );
     }else{
         
         switch( pHistory->cord ){
             
             case 1:
                 BLK_FUNC( Graph, circle_qrt1_raw )( X    , Y+eps, (D>>1)+1, &info_MainScreen, kApplyPixel_fill );
                 break;
             case 2:
                 BLK_FUNC( Graph, circle_qrt2_raw )( X+eps, Y+eps, (D>>1)+1, &info_MainScreen, kApplyPixel_fill );
                 break;
             case 3:
                 BLK_FUNC( Graph, circle_qrt3_raw )( X+eps, Y    , (D>>1)+1, &info_MainScreen, kApplyPixel_fill );
                 break;
             case 4:
                 BLK_FUNC( Graph, circle_qrt4_raw )( X    , Y    , (D>>1)+1, &info_MainScreen, kApplyPixel_fill );
                 break;
             case 0:
             case 5:
             case 6:
                 BLK_FUNC( Graph, circle_raw )( X, Y, D, &info_MainScreen, kApplyPixel_fill );
                 break;
         }
     }
    
    struct __GUI_ObjDataScr_joystick* pDataSrc = ((struct __GUI_ObjDataScr_joystick*)config->dataScr);
    
    int32_t val_x = __limit( (int32_t)pDataSrc->value[0], (int32_t)pDataSrc->min[0], (int32_t)pDataSrc->max[0] );
    int32_t val_y = __limit( (int32_t)pDataSrc->value[1], (int32_t)pDataSrc->min[1], (int32_t)pDataSrc->max[1] );
    
    
    int dis_cir_max = ((3*D)>>3); // ?????????????????????

    
    int px = (val_x - (int32_t)pDataSrc->min[0])*(dis_cir_max<<1)/((int32_t)pDataSrc->max[0]-(int32_t)pDataSrc->min[0]) - dis_cir_max;
    int py = (val_y - (int32_t)pDataSrc->min[1])*(dis_cir_max<<1)/((int32_t)pDataSrc->max[1]-(int32_t)pDataSrc->min[1]) - dis_cir_max;
    int pd = __limit( (D>>3), 1, D );

    int cord   = __Point_toCord2D( px, py );// ?????????????????????????????????

    if( px*px +py*py >= dis_cir_max*dis_cir_max ){
        
        int pTmp_x = dis_cir_max*cosf(atan2f(py, px));
        int pTmp_y = dis_cir_max*sinf(atan2f(py, px));
        px = __abs(pTmp_x);
        py = __abs(pTmp_y);
        switch( cord ){
            case 5: // Axis +X
            case 7: // Axis +Y
            case 1: // Cord 1
                px = px;
                py = py;
                break;
            case 2: // Cord 2
                px = -px;
                break;
            case 6: // Axis -X
            case 8: // Axis -Y
            case 3: // Cord 3
                px = -px;
                py = -py;
                break;
            case 4: // Cord 4
                py = -py;
                break;
        }
    }
    BLK_FUNC( Graph, circle_fill )( X+px, Y-py, pd, &info_MainScreen, kApplyPixel_fill);
    
    if( !pHistory ){
        pHistory = RH_MALLOC(sizeof(*pHistory));
    #ifdef RH_DEBUG
        RH_ASSERT( pHistory );
    #endif
        __SET_STRUCT_MB(__GUI_Object_t, void*, config, history, pHistory);
    }
    pHistory->cord        = cord;
    pHistory->area.xs     = (X+px-(pd>>1)+eps);
    pHistory->area.ys     = (Y-py-(pd>>1)+eps);
    pHistory->area.width  = pd;
    pHistory->area.height = pd;
    
    __Font_restore_config();
    BLK_FUNC( Graph, restoreCache )();
    
}
static void __gui_adjust_object_joystick  ( const __GUI_Object_t* config ){
    __gui_insert_object_joystick(config);
}

static void __gui_remove_object_trunk     ( const __GUI_Object_t* config ){
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( config->widget == kGUI_ObjStyle_trunk );
#endif
    // ????????????????????????
    struct{
        int     bar_s;   /* ???????????????????????? */
        int     bar_e;   /* ???????????????????????? */
        int     bar_pos; /* ??????????????????????????????????????????(?????????) */
        int     margin;  /* ?????? */
    }*cache = (void*)config->history;
    
    
    __Font_backup_config();
    BLK_FUNC( Graph, backupCache )();
    
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
    GLU_UION(Pixel) color  = {.data = (config->bk_color==0x00)?0x00:0xff};
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
    GLU_UION(Pixel) color  = {.data = config->bk_color};
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
#else
  #error "[RH_graphic]: Unknown color type."
#endif
    BLK_FUNC( Graph, set_penColor )( color.data );
    
    if( !cache ){
        BLK_FUNC( Graph, rect_fill )( config->area.xs+1, \
                           config->area.ys+1, \
                           config->area.xs+(int)(config->area.width )-1-1, \
                           config->area.ys+(int)(config->area.height)-1-1, \
                           &info_MainScreen, kApplyPixel_fill);
    }else{
        int32_t min = ((__GUI_ObjDataScr_trunk*)config->dataScr)->min;
        int32_t max = ((__GUI_ObjDataScr_trunk*)config->dataScr)->max;
        int32_t val = __limit(((__GUI_ObjDataScr_trunk*)config->dataScr)->value, min, max);
        int bar_pos = cache->bar_e - val*(int)(cache->bar_e-cache->bar_s+1)/(max-min);
        
        if( cache->bar_pos - config->text_size < bar_pos ){
            BLK_FUNC( Graph, rect_fill )( config->area.xs+1, \
                               cache->bar_pos - config->text_size, \
                               config->area.xs+(int)(config->area.width )-1-1, \
                               bar_pos, \
                               &info_MainScreen, kApplyPixel_fill);
        }
        //...//
    }
    
    __Font_restore_config();
    BLK_FUNC( Graph, restoreCache )();
}
static void __gui_insert_object_trunk     ( const __GUI_Object_t* config ){
    // ????????????????????????
    struct{
        int     bar_s;   /* ???????????????????????? */
        int     bar_e;   /* ???????????????????????? */
        int     bar_pos; /* ??????????????????????????????????????????(?????????) */
        int     margin;  /* ?????? */
    }*cache = (void*)config->history;
    
    __gui_remove_object_trunk(config);
    
    if( !cache ){
        cache = RH_CALLOC(sizeof(*cache),1);
    #ifdef RH_DEBUG
        RH_ASSERT( cache );
    #endif
        cache->margin = 2;
        cache->bar_s = config->area.ys + config->text_size + cache->margin;
        cache->bar_e = config->area.ys + (int)config->area.height-1-1;
        cache->bar_pos = cache->bar_e;
        __SET_STRUCT_MB(__GUI_Object_t, void*, config, history, cache );
    }
    
    int32_t min = ((__GUI_ObjDataScr_trunk*)config->dataScr)->min;
    int32_t max = ((__GUI_ObjDataScr_trunk*)config->dataScr)->max;
    int32_t val = __limit(((__GUI_ObjDataScr_trunk*)config->dataScr)->value, min, max);
    int bar_pos = cache->bar_e - val*(int)(cache->bar_e-cache->bar_s+1)/(max-min);
    
    __Font_backup_config();
    BLK_FUNC( Graph, backupCache )();
    
    // ??????????????????
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
    GLU_UION(Pixel) color  = {.data = (config->obj_color==0x00)?0x00:0xff};
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
    GLU_UION(Pixel) color  = {.data = config->obj_color};
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
#else
  #error "[RH_graphic]: Unknown color type."
#endif
    // ?????????????????????????????????????????????????????????(???????????????)
    if( cache->bar_pos >= bar_pos ){
        BLK_FUNC( Graph, set_penColor )( color.data );
        BLK_FUNC( Graph, rect_fill )( config->area.xs+cache->margin, \
                           bar_pos, \
                           config->area.xs+(int)(config->area.width)-1-cache->margin, \
                           cache->bar_pos, \
                           &info_MainScreen, kApplyPixel_fill);
    }
    cache->bar_pos = bar_pos;
    // ????????????
    __GUI_Font_t* pF = NULL;
    {
        char __str[GUI_X_WIDTH>>2] = {0};
        __Font_setSize(config->text_size);
        snprintf(__str, sizeof(__str), "%d",val);
        __str[ __Font_getWordNum(config->area.width, __str) ] = '\0';
        
    #ifdef RH_DEBUG
        RH_ASSERT( __str[0] != '\0' );
    #endif
        
        // ???????????????????????????????????????
        int wordCnt = snprintf(__str, sizeof(__str), "%d",val);
        
        // ??????????????????????????????(width)?????????????????????, ??????????????????????????????
        int maxWordCnt = __Font_getWordNum(config->area.width, __str);
        // ??????????????????????????????
        __str[ maxWordCnt ] = '\0';
        
        // ???????????????????????????????????????????????????????????????: [???] ??????????????????3?????????  123 | 90 | 1 ?????? ??? 1234 | 4253 ????????????????????????
        bool isEnough = wordCnt <= maxWordCnt;
        
        // ?????????????????????????????????, ???????????????#???, ?????????????????????
        if( !isEnough){
            memset(__str, '#', maxWordCnt);
        }
        pF = __Font_exportStr(__str);
    }
#ifdef RH_DEBUG
    RH_ASSERT( pF );
#endif
    int x_fs = 0;
    int y_fs = bar_pos - config->text_size;
    switch ( config->text_align ) {
        case kGUI_FontAlign_Left:
            x_fs = config->area.xs;
            break;
        case kGUI_FontAlign_Middle:
            x_fs = __limit( config->area.xs +(((int)(config->area.width - pF->width))>>1) , 0, GUI_X_WIDTH-1   );
            break;
        default:
            RH_ASSERT(0);
    }
    GLU_UION(Pixel) color_text = {.data = config->obj_color};

#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
    uint8_t* pIter = pF->output;
    for( int y=0; y<pF->height && y<config->area.height; y++ ){
        for( int x=0; x<pF->width; x++, pIter++ ){
            size_t index = ((y_fs+y)>>3)*(info_MainScreen.width)+(x_fs+x);
            if( (*pIter<128) ^ (color_text.data!=0) ){
                info_MainScreen.pBuffer[ index ].data = __BIT_SET( info_MainScreen.pBuffer[ index ].data, (y_fs+y)%8 );
            }else{
                info_MainScreen.pBuffer[ index ].data = __BIT_CLR( info_MainScreen.pBuffer[ index ].data, (y_fs+y)%8 );
            }
        
        }
    }
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
    for( int y=0; y<pF->height&&y<config->area.height; y++ ){
        for( int x=0; x<pF->width; x++ ){
            size_t index = (y_fs+y)*(info_MainScreen.width)+(x_fs+x);
            uint8_t pixWeight = pF->output[y*pF->width+x];
            info_MainScreen.pBuffer[ index ].R = info_MainScreen.pBuffer[ index ].R + (( (color_text.R - info_MainScreen.pBuffer[ index ].R) * pixWeight )>>8);
            info_MainScreen.pBuffer[ index ].G = info_MainScreen.pBuffer[ index ].G + (( (color_text.G - info_MainScreen.pBuffer[ index ].G) * pixWeight )>>8);
            info_MainScreen.pBuffer[ index ].B = info_MainScreen.pBuffer[ index ].B + (( (color_text.B - info_MainScreen.pBuffer[ index ].B) * pixWeight )>>8);
        }
    }
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
    RH_ASSERT(0);
#else
     
#endif

    __Font_restore_config();
    BLK_FUNC( Graph, restoreCache )();
    
}
static void __gui_adjust_object_trunk     ( const __GUI_Object_t* config ){
    __gui_insert_object_trunk(config);
}

static void __gui_remove_object_spinbox   ( const __GUI_Object_t* config ){
    struct{
        int32_t    value;
        int8_t     margin;
        bool       triUP;
        bool       triDN;
        int        lineUP;
        int        lineDN;
        int        textXS;
        int        textYS;
        __Area_t   num;
        bool       active;
    }*cache = (void*)config->history;
    
    __GUI_ObjDataScr_spinbox* dataScr = (__GUI_ObjDataScr_spinbox*)(config->dataScr);
    BLK_FUNC( Graph, backupCache )();
    
    // ????????????
    if( dataScr->value != cache->value ){
        BLK_FUNC( Graph, set_penColor )( config->bk_color );
        BLK_FUNC( Graph, rect_fill )( cache->num.xs       , \
                                      cache->num.ys       , \
                                      (int)(cache->num.xs+cache->num.width -1), \
                                      (int)(cache->num.ys+cache->num.height-1), \
                                      &info_MainScreen, kApplyPixel_fill );
    }
    
    // ???????????????
    {
        // ??????????????????????????????
        if( dataScr->value >= dataScr->max || (cache->active&& !dataScr->active) ){
            // ???????????????
            int len = config->text_size;                                                 // ?????????(???) ?????????(pix)
            int xs  = (config->area.xs + cache->textXS - len)>>1;                        // ?????????(???) ???????????????
            int ys  = cache->lineDN + cache->margin ; // ?????????(??????) ???????????????
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
            for( int y=ys; len>0&&y<config->area.ys+config->area.height; len-=2, y++, xs++ ){
                GLU_UION(Pixel)* pIter = &info_MainScreen.pBuffer[(y>>3)*info_MainScreen.width + xs];
                for( int x=0; x<len; x++,pIter++ ){
                    if( config->bk_color!=0 ){
                        pIter->data = __BIT_SET( pIter->data, y%8 );
                    }else{
                        pIter->data = __BIT_CLR( pIter->data, y%8 );
                    }
                }
            }
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
            for( int y=ys; len>0&&y<config->area.ys+config->area.height; len-=2, y++, xs++ ){
                GLU_UION( Pixel )* pIter = &info_MainScreen.pBuffer[ y*info_MainScreen.width+xs ];
                for( int x=0; x<len; x++,pIter++ ){
                    pIter->data = config->bk_color;
                }
            }
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
            RH_ASSERT(0);
#endif
        }
        
        if( dataScr->value <= dataScr->min || (cache->active&& !dataScr->active) ){
            // ???????????????
            int len = config->text_size;                                    // ?????????(???) ?????????(pix)
            int xs  = (config->area.xs + cache->textXS - len)>>1;           // ?????????(???) ???????????????
            int ys  = cache->lineUP - cache->margin;                        // ?????????(??????) ???????????????
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
            for( int y=ys; len>0&&y>=config->area.ys; len-=2, y--, xs++ ){
                GLU_UION(Pixel)* pIter = &info_MainScreen.pBuffer[(y>>3)*info_MainScreen.width + xs];
                for( int x=0; x<len; x++, pIter++ ){
                    if( config->bk_color!=0 ){
                        pIter->data = __BIT_SET( pIter->data, y%8 );
                    }else{
                        pIter->data = __BIT_CLR( pIter->data, y%8 );
                    }
                }
                
            }
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
            for( int y=ys; len>0&&y>=config->area.ys; len-=2, y--, xs++ ){
                GLU_UION( Pixel )* pIter = &info_MainScreen.pBuffer[ y*info_MainScreen.width+xs ];
                for( int x=0; x<len; x++,pIter++ ){
                    pIter->data = config->bk_color;
                }
            }
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
            RH_ASSERT(0);
#endif
        }
        
    }
    
    BLK_FUNC( Graph, restoreCache )();
}
static void __gui_insert_object_spinbox   ( const __GUI_Object_t* config ){
    BLK_FUNC( Graph, backupCache )();
    __Font_backup_config();
    struct{
        int32_t    value;
        int8_t     margin;
        bool       triUP;
        bool       triDN;
        int        lineUP;
        int        lineDN;
        int        textXS;
        int        textYS;
        __Area_t   num;
        bool       active;
    }*cache = (void*)config->history;
    
    __GUI_ObjDataScr_spinbox* dataScr = (__GUI_ObjDataScr_spinbox*)(config->dataScr);
    
    if( !cache ){
        cache = RH_MALLOC( sizeof(*cache) );
        cache->margin = 4;
        cache->value  = dataScr->min;
        cache->triUP  = false;
        cache->lineUP = (int)(config->area.ys + ((config->area.height-(config->text_size+cache->margin))>>1));
        cache->lineDN = (int)(config->area.ys + ((config->area.height+(config->text_size+cache->margin))>>1));
        cache->textXS = config->area.xs+dataScr->text_offset-1;
        cache->textYS = cache->lineUP + (cache->margin>>1) + 1;
        cache->active = false;
        { // ?????????????????????
            int line_y1 = cache->lineUP = (int)(config->area.ys + ((config->area.height-(config->text_size+cache->margin))>>1));
            int line_y2 = cache->lineDN = (int)(config->area.ys + ((config->area.height+(config->text_size+cache->margin))>>1));
            BLK_FUNC( Graph, set_penColor )( M_COLOR_WHITE );
            BLK_FUNC( Graph, line_raw )( config->area.xs, line_y1, (int)(config->area.xs+config->area.width-1), line_y1, &info_MainScreen, kApplyPixel_fill );
            BLK_FUNC( Graph, line_raw )( config->area.xs, line_y2, (int)(config->area.xs+config->area.width-1), line_y2, &info_MainScreen, kApplyPixel_fill );
        }
        
        { // ????????????
            if( config->text!=NULL ){
                char* ptrUnit = alloca( strlen(config->text) );
                strcpy( ptrUnit, config->text );
                __Font_setSize( config->text_size );
                ptrUnit[ __Font_getWordNum( config->area.width - dataScr->text_offset , config->text) ] = '\0';
                __GUI_Font_t* pF = __Font_exportStr( ptrUnit );
                
                size_t width;
                __Font_getStrSize( &width, NULL, ptrUnit );
                int x_fs = cache->textXS;
                int y_fs = cache->textYS = cache->lineUP + (cache->margin>>1);
                
                GLU_UION(Pixel) color_text = {.data = config->obj_color};
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
                uint8_t* pIter = pF->output;
                for( int y=0; y<pF->height && y<config->area.height; y++ ){
                    for( int x=0; x<pF->width; x++, pIter++ ){
                        size_t index = ((y_fs+y)>>3)*(info_MainScreen.width)+(x_fs+x);
                        if( (*pIter<128) ^ (color_text.data!=0) ){
                            info_MainScreen.pBuffer[ index ].data = __BIT_SET( info_MainScreen.pBuffer[ index ].data, (y_fs+y)%8 );
                        }else{
                            info_MainScreen.pBuffer[ index ].data = __BIT_CLR( info_MainScreen.pBuffer[ index ].data, (y_fs+y)%8 );
                        }
                    }
                }
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
                for( int y=0; y<pF->height&&y<config->area.height; y++ ){
                    for( int x=0; x<pF->width; x++ ){
                        size_t index = (y_fs+y)*(info_MainScreen.width)+(x_fs+x);
                        uint8_t pixWeight = pF->output[y*pF->width+x];
                        info_MainScreen.pBuffer[ index ].R = info_MainScreen.pBuffer[ index ].R + (( (color_text.R - info_MainScreen.pBuffer[ index ].R) * pixWeight )>>8);
                        info_MainScreen.pBuffer[ index ].G = info_MainScreen.pBuffer[ index ].G + (( (color_text.G - info_MainScreen.pBuffer[ index ].G) * pixWeight )>>8);
                        info_MainScreen.pBuffer[ index ].B = info_MainScreen.pBuffer[ index ].B + (( (color_text.B - info_MainScreen.pBuffer[ index ].B) * pixWeight )>>8);
                    }
                }
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
    RH_ASSERT(0);
#else
     
#endif
            }
        }
        __SET_STRUCT_MB(__GUI_Object_t, void*, config, history, cache);
    }else{
        __gui_remove_object_spinbox( config );
    }
    
    if( dataScr->value < dataScr->max )
        cache->triDN = true;
    else
        cache->triDN = false;
    
    if( dataScr->value > dataScr->min )
        cache->triUP = true;
    else
        cache->triUP = false;
    
    { // ????????????
        cache->value = __limit( dataScr->value, dataScr->min, dataScr->max );
        __Font_setSize( config->text_size );
        size_t size   = 1 + BLK_FUNC( Bit, DECs )( cache->value );
        char* ptrNum  = alloca( size );
        snprintf( ptrNum, size, "%d", cache->value );
        ptrNum[size-1] = '\0';
        
        __Font_getStrSize( &cache->num.width, &cache->num.height, ptrNum);
        cache->num.xs     = (int)(cache->textXS - cache->num.width - dataScr->margin);
        cache->num.ys     = cache->textYS;
        
        __GUI_Font_t* pF = __Font_exportStr( ptrNum );
        
        GLU_UION(Pixel) color_text = {.data = config->obj_color};
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
        uint8_t* pIter = pF->output;
        for( int y=0; y<pF->height && y<config->area.height; y++ ){
            for( int x=0; x<pF->width; x++, pIter++ ){
                size_t index = ((cache->num.ys+y)>>3)*(info_MainScreen.width)+(cache->num.xs+x);
                if( (*pIter<128) ^ (color_text.data!=0) ){
                    info_MainScreen.pBuffer[ index ].data = __BIT_SET( info_MainScreen.pBuffer[ index ].data, (cache->num.ys+y)%8 );
                }else{
                    info_MainScreen.pBuffer[ index ].data = __BIT_CLR( info_MainScreen.pBuffer[ index ].data, (cache->num.ys+y)%8 );
                }
            }
        }
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
        for( int y=0; y<pF->height&&y<config->area.height; y++ ){
            for( int x=0; x<pF->width; x++ ){
                size_t index = (cache->num.ys+y)*(info_MainScreen.width)+(cache->num.xs+x);
                uint8_t pixWeight = pF->output[y*pF->width+x];
                info_MainScreen.pBuffer[ index ].R = info_MainScreen.pBuffer[ index ].R + (( (color_text.R - info_MainScreen.pBuffer[ index ].R) * pixWeight )>>8);
                info_MainScreen.pBuffer[ index ].G = info_MainScreen.pBuffer[ index ].G + (( (color_text.G - info_MainScreen.pBuffer[ index ].G) * pixWeight )>>8);
                info_MainScreen.pBuffer[ index ].B = info_MainScreen.pBuffer[ index ].B + (( (color_text.B - info_MainScreen.pBuffer[ index ].B) * pixWeight )>>8);
            }
        }
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
    RH_ASSERT(0);
#else
     
#endif
    }
    
    if( dataScr->active ){ // ????????????????????????
        int len   = config->text_size;                                 // ?????????(???) ?????????(pix)
        int xs    = (config->area.xs + cache->textXS - len)>>1;        // ?????????(???) ???????????????
        int ys[2] = { cache->lineUP - cache->margin , cache->lineDN + cache->margin }; // ?????????(??????) ???????????????
        
#if   ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
        if( cache->triUP ){
            for( int y=ys[0]; len>0&&y>=config->area.ys; len-=2, y--, xs++ ){
                GLU_UION(Pixel)* pIter = &info_MainScreen.pBuffer[(y>>3)*info_MainScreen.width + xs];
                for( int x=0; x<len; x++, pIter++ ){
                    if( config->obj_color!=0 ){
                        pIter->data = __BIT_SET( pIter->data, y%8 );
                    }else{
                        pIter->data = __BIT_CLR( pIter->data, y%8 );
                    }
                }
                
            }
        }
        
        len = config->text_size;
        xs  = (config->area.xs + cache->textXS - len)>>1;
        if( cache->triDN ){
            for( int y=ys[1]; len>0&&y<config->area.ys+config->area.height; len-=2, y++, xs++ ){
                GLU_UION(Pixel)* pIter = &info_MainScreen.pBuffer[(y>>3)*info_MainScreen.width + xs];
                for( int x=0; x<len; x++,pIter++ ){
                    if( config->obj_color!=0 ){
                        pIter->data = __BIT_SET( pIter->data, y%8 );
                    }else{
                        pIter->data = __BIT_CLR( pIter->data, y%8 );
                    }
                }
            }
        }
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565 )
        
        if( cache->triUP ){
            for( int y=ys[0]; len>0&&y>=config->area.ys; len-=2, y--, xs++ ){
                GLU_UION( Pixel )* pIter = &info_MainScreen.pBuffer[ y*info_MainScreen.width+xs ];
                for( int x=0; x<len; x++,pIter++ ){
                    pIter->data = config->obj_color;
                }
            }
        }
        
        len = config->text_size;
        xs  = cache->num.xs + (int)((cache->num.width - len)>>1);
        if( cache->triDN ){
            for( int y=ys[1]; len>0&&y<config->area.ys+config->area.height; len-=2, y++, xs++ ){
                GLU_UION( Pixel )* pIter = &info_MainScreen.pBuffer[ y*info_MainScreen.width+xs ];
                for( int x=0; x<len; x++,pIter++ ){
                    pIter->data = config->obj_color;
                }
            }
        }
        
#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888 )
        RH_ASSERT(0);
#else
     
#endif
        
    }
    
    cache->active = dataScr->active;
    
    BLK_FUNC( Graph, restoreCache )();
    __Font_restore_config();
    
}
static void __gui_adjust_object_spinbox   ( const __GUI_Object_t* config ){
    __gui_insert_object_spinbox( config );
}


#ifdef RH_DEBUG
static inline void __gui_check_object  ( const __GUI_Object_t* config ){
    RH_ASSERT( config );
    RH_ASSERT( config->min   <= config->max       );
    RH_ASSERT( config->widget <  NUM_kGUI_ObjWidgets );
    RH_ASSERT( config->area.xs + config->area.width  -1  < GUI_X_WIDTH   ); // Can be compromised, no need to abort the program.
    RH_ASSERT( config->area.ys + config->area.height -1  < GUI_Y_WIDTH   ); // Can be compromised, no need to abort the program.
}
#endif

ID_t RH_RESULT    GLU_FUNC( Object, create   )  ( const __GUI_Object_t* config, const void* RH_NULLABLE dataScr ){
    __GUI_Object_t* m_config = (__GUI_Object_t*)RH_MALLOC( sizeof(__GUI_Object_t) );
#ifdef RH_DEBUG
    RH_ASSERT( m_config );
    RH_ASSERT( config );
    __gui_check_object(config);
#endif
    memmove(m_config, config, sizeof(__GUI_Object_t));
    __SET_STRUCT_MB(__GUI_Object_t, void*, m_config, history, NULL);
    
    switch( m_config->widget ){
        case kGUI_ObjStyle_text:
            m_config->insert_func = __gui_insert_object_text;
            m_config->remove_func = __gui_remove_object_text;
            m_config->adjust_func = __gui_adjust_object_text;
            m_config->dataScr     = RH_CALLOC( 1U, sizeof(struct __GUI_ObjDataScr_text) );
            break;
        case kGUI_ObjStyle_num:
            m_config->insert_func = __gui_insert_object_num;
            m_config->remove_func = __gui_remove_object_num;
            m_config->adjust_func = __gui_adjust_object_num;
            m_config->dataScr     = RH_CALLOC( 1U, sizeof(struct __GUI_ObjDataScr_num) );
            break;
        case kGUI_ObjStyle_fnum:
            m_config->insert_func = __gui_insert_object_fnum;
            m_config->remove_func = __gui_remove_object_fnum;
            m_config->adjust_func = __gui_adjust_object_fnum;
            m_config->dataScr     = RH_CALLOC( 1U, sizeof(struct __GUI_ObjDataScr_fnum) );
//            if( !dataScr ){}
//            else{}
            break;
        case kGUI_ObjStyle_switch:
            m_config->insert_func = __gui_insert_object_switch;
            m_config->remove_func = __gui_remove_object_switch;
            m_config->adjust_func = __gui_adjust_object_switch;
            m_config->dataScr     = RH_CALLOC( 1U, sizeof(struct __GUI_ObjDataScr_switch) );
//            if( !dataScr ){}
//            else{}
            break;
        case kGUI_ObjStyle_barH:
            m_config->insert_func = __gui_insert_object_bar_h;
            m_config->remove_func = __gui_remove_object_bar_h;
            m_config->adjust_func = __gui_adjust_object_bar_h;
            m_config->dataScr     = RH_CALLOC( 1U, sizeof(struct __GUI_ObjDataScr_barH) );
//            if( !dataScr ){}
//            else          {}
            break;
        case kGUI_ObjStyle_barV:
            m_config->insert_func = __gui_insert_object_bar_v;
            m_config->remove_func = __gui_remove_object_bar_v;
            m_config->adjust_func = __gui_adjust_object_bar_v;
            m_config->dataScr     = RH_CALLOC( 1U, sizeof(       __GUI_ObjDataScr_barV) );
            if( !dataScr ){
                ((__GUI_ObjDataScr_barV*)m_config->dataScr)->max   = 100;
                ((__GUI_ObjDataScr_barV*)m_config->dataScr)->min   = 0;
                ((__GUI_ObjDataScr_barV*)m_config->dataScr)->value = 0;
            }else{
                memcpy(m_config->dataScr, dataScr, sizeof( __GUI_ObjDataScr_barV ));
            }
            break;
        case kGUI_ObjStyle_trunk:
            m_config->insert_func = __gui_insert_object_trunk;
            m_config->remove_func = __gui_remove_object_trunk;
            m_config->adjust_func = __gui_adjust_object_trunk;
            m_config->dataScr     = RH_CALLOC( 1U, sizeof( __GUI_ObjDataScr_trunk) );
            if( !dataScr ){
                ((__GUI_ObjDataScr_barV*)m_config->dataScr)->max   = 100;
                ((__GUI_ObjDataScr_barV*)m_config->dataScr)->min   = 0;
                ((__GUI_ObjDataScr_barV*)m_config->dataScr)->value = 0;
            }else{
                memcpy(m_config->dataScr, dataScr, sizeof( __GUI_ObjDataScr_trunk));
            }
            break;
        case kGUI_ObjStyle_joystick:
            m_config->insert_func = __gui_insert_object_joystick;
            m_config->remove_func = __gui_remove_object_joystick;
            m_config->adjust_func = __gui_adjust_object_joystick;
            m_config->dataScr     = RH_CALLOC( 1U, sizeof(struct __GUI_ObjDataScr_joystick) );
            if( !dataScr ){
                ((struct __GUI_ObjDataScr_joystick*)m_config->dataScr)->max[0]   = 100;
                ((struct __GUI_ObjDataScr_joystick*)m_config->dataScr)->max[1]   = 100;
                ((struct __GUI_ObjDataScr_joystick*)m_config->dataScr)->min[0]   = 0;
                ((struct __GUI_ObjDataScr_joystick*)m_config->dataScr)->min[1]   = 0;
                ((struct __GUI_ObjDataScr_joystick*)m_config->dataScr)->value[0] = 50;
                ((struct __GUI_ObjDataScr_joystick*)m_config->dataScr)->value[1] = 50;
            }else{
                memcpy(m_config->dataScr, dataScr, sizeof(struct __GUI_ObjDataScr_joystick));
            }
            break;
        case kGUI_ObjStyle_spinbox:
            m_config->insert_func = __gui_insert_object_spinbox;
            m_config->remove_func = __gui_remove_object_spinbox;
            m_config->adjust_func = __gui_adjust_object_spinbox;
            m_config->dataScr     = RH_CALLOC( 1U, sizeof(struct __GUI_ObjDataScr_spinbox) );
            if( !dataScr ){
                ((struct __GUI_ObjDataScr_spinbox*)m_config->dataScr)->min  = 0;
                ((struct __GUI_ObjDataScr_spinbox*)m_config->dataScr)->max  = 100;
            }else{
                memcpy(m_config->dataScr, dataScr, sizeof(struct __GUI_ObjDataScr_spinbox));
            }
            //...//
            break;
        default:
            RH_ASSERT(0);
    }
    
    return (ID_t)m_config;
}

E_Status_t        GLU_FUNC( Object, template )  ( __GUI_Object_t* config, E_GUI_ObjWidget_t widget ){
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( widget < NUM_kGUI_ObjWidgets );
#endif
    const char* pText = "DEMO";
    // Common Settings
    config->widget    = widget;
    config->showFrame = true;
    
    // Speacial Settings
    switch ( widget ) {
        case kGUI_ObjStyle_text:
        case kGUI_ObjStyle_num:
        case kGUI_ObjStyle_fnum:
            config->bk_color    = M_COLOR_BLACK;
            config->obj_color   = M_COLOR_WHITE;
            config->font        = kGUI_FontStyle_ArialRounded_Bold;
            config->text_align  = kGUI_FontAlign_Middle;
            config->text_size   = __limit((GUI_Y_WIDTH*GUI_X_WIDTH)>>10, 8, 64);
            config->text        = pText;
            config->area.width  = __limit((config->text_size*strlen(pText)),0,GUI_X_WIDTH);
            config->area.height = __limit((config->text_size + (__limit((signed)(config->text_size>>3), 1, config->text_size )<<2)),0,GUI_Y_WIDTH);
            config->area.xs     = (int)( GUI_X_WIDTH - config->area.width  )>>1;
            config->area.ys     = (int)( GUI_Y_WIDTH - config->area.height )>>1;
            break;
            
        case kGUI_ObjStyle_switch:
            config->bk_color    = M_COLOR_BLACK;
            config->obj_color   = M_COLOR_GREEN;
            config->area.width  = (int)((hypotf(GUI_X_WIDTH, GUI_Y_WIDTH)+646)/26.3);
            config->area.height = __limit( (signed)(config->area.width>>1), 4, GUI_Y_WIDTH);
            config->area.xs     = (int)( GUI_X_WIDTH - config->area.width  )>>1;
            config->area.ys     = (int)( GUI_Y_WIDTH - config->area.height )>>1;
            break;
            
        case kGUI_ObjStyle_barH:
            config->bk_color    = M_COLOR_BLACK;
            config->obj_color   = M_COLOR_WHITE;
            config->area.width  = GUI_X_WIDTH>>2;
            config->area.height = config->area.width>>3;
            config->area.xs     = (int)( GUI_X_WIDTH - config->area.width  )>>1;
            config->area.ys     = (int)( GUI_Y_WIDTH - config->area.height )>>1;
            break;
        case kGUI_ObjStyle_barV:
            config->bk_color    = M_COLOR_BLACK;
            config->obj_color   = M_COLOR_WHITE;
            config->area.height = (GUI_Y_WIDTH*3)>>3;
            config->area.width  = __limit( (signed)(config->area.height>>3), 4, GUI_X_WIDTH );
            config->area.xs     = (int)( GUI_X_WIDTH - config->area.width  )>>1;
            config->area.ys     = (int)( GUI_Y_WIDTH - config->area.height )>>1;
            break;
        case kGUI_ObjStyle_joystick:
            config->bk_color    = M_COLOR_BLACK;
            config->obj_color   = M_COLOR_WHITE;
            config->area.width  = config->area.height = __min(GUI_X_WIDTH, GUI_Y_WIDTH)>>1;
            config->area.xs     = (int)( GUI_X_WIDTH - config->area.width  )>>1;
            config->area.ys     = (int)( GUI_Y_WIDTH - config->area.height )>>1;
            break;
            
        case kGUI_ObjStyle_trunk:
            config->bk_color    = M_COLOR_BLACK;
            config->obj_color   = M_COLOR_WHITE;
            config->area.height = (GUI_Y_WIDTH*3)>>2;
            config->area.width  = __limit( (signed)(config->area.height>>1), 4, GUI_X_WIDTH );
            config->area.xs     = (int)( GUI_X_WIDTH - config->area.width  )>>1;
            config->area.ys     = (int)( GUI_Y_WIDTH - config->area.height )>>1;
            config->text_size   = 8;
            config->text_align  = kGUI_FontAlign_Middle;
            break;
            
        default:
            break;
    }
    
    
    return MAKE_ENUM( kStatus_Success );
}

E_Status_t        GLU_FUNC( Object, frame    )  ( ID_t ID  , bool  cmd   ){
#ifdef RH_DEBUG
    RH_ASSERT( ID );
#endif
    __GUI_Object_t* p = (__GUI_Object_t*)(ID);
    
    BLK_FUNC( Graph, backupCache )();
    if( cmd ){
        BLK_FUNC( Graph, set_penColor )( p->obj_color );
        BLK_FUNC( Graph, rect_raw )(p->area.xs, p->area.ys, p->area.xs+(int)(p->area.width)-1, p->area.ys+(int)(p->area.height)-1, &info_MainScreen, kApplyPixel_fill);
    }else{
        BLK_FUNC( Graph, set_penColor )( p->bk_color );
        BLK_FUNC( Graph, rect_raw )(p->area.xs, p->area.ys, p->area.xs+(int)(p->area.width)-1, p->area.ys+(int)(p->area.height)-1, &info_MainScreen, kApplyPixel_fill);
    }
    p->showFrame = cmd;
    BLK_FUNC( Graph, restoreCache )();
    GLU_FUNC( GUI, isAutoDisplay )() ? GLU_FUNC( GUI, refreashScreenArea )( p->area.xs, \
                                                 p->area.ys, \
                                                 p->area.xs+(int)(p->area.width )-1, \
                                                 p->area.ys+(int)(p->area.height)-1) \
                       :
                         GLU_FUNC( GUI, addScreenArea )     ( p->area.xs, \
                                                 p->area.ys, \
                                                 p->area.xs+(int)(p->area.width )-1, \
                                                 p->area.ys+(int)(p->area.height)-1);
    return MAKE_ENUM( kStatus_Success );
}

E_Status_t        GLU_FUNC( Object, insert   )  ( ID_t ID ){
    __GUI_Object_t* config = (__GUI_Object_t*)ID;
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( config->insert_func );
#endif
    
    (*config->insert_func)( config );
    GLU_FUNC( GUI, isAutoDisplay )() ? GLU_FUNC( GUI, refreashScreenArea )( config->area.xs, \
                                                 config->area.ys, \
                                                 config->area.xs+(int)(config->area.width )-1, \
                                                 config->area.ys+(int)(config->area.height)-1) \
                       :
                         GLU_FUNC( GUI, addScreenArea )     ( config->area.xs, \
                                                 config->area.ys, \
                                                 config->area.xs+(int)(config->area.width )-1, \
                                                 config->area.ys+(int)(config->area.height)-1);
    return MAKE_ENUM( kStatus_Success );
}

E_Status_t        GLU_FUNC( Object, adjust   )  ( ID_t ID  , void*  dataScr, size_t dataSize ){
    __GUI_Object_t* config = (__GUI_Object_t*)ID;
#ifdef RH_DEBUG
    RH_ASSERT( config );
    RH_ASSERT( config->insert_func );
    //...//
#endif
    memcpy(config->dataScr, dataScr, dataSize);
    (*config->adjust_func)(config);
    GLU_FUNC( GUI, isAutoDisplay )() ? GLU_FUNC( GUI, refreashScreenArea )( config->area.xs, \
                                                 config->area.ys, \
                                                 config->area.xs+(int)(config->area.width )-1, \
                                                 config->area.ys+(int)(config->area.height)-1) \
                       :
                         GLU_FUNC( GUI, addScreenArea )     ( config->area.xs, \
                                                 config->area.ys, \
                                                 config->area.xs+(int)(config->area.width )-1, \
                                                 config->area.ys+(int)(config->area.height)-1);
    
    return MAKE_ENUM( kStatus_Success );
}

E_Status_t        GLU_FUNC( Object, delete   )  ( ID_t ID ){
    __GUI_Object_t* config = (__GUI_Object_t*)( ID );
    RH_FREE( (void*)config->history );
    RH_FREE( (void*)config->dataScr );
    __SET_STRUCT_MB(__GUI_Object_t, void*, config, history, NULL);
    
    BLK_FUNC( Graph, backupCache )();
    __Font_backup_config();
    BLK_FUNC( Graph, set_penColor )(config->bk_color);
    
    BLK_FUNC( Graph, rect_fill )( config->area.xs, \
                       config->area.ys, \
                       config->area.xs+(int)(config->area.width )-1, \
                       config->area.ys+(int)(config->area.height)-1, &info_MainScreen, kApplyPixel_fill);
    
    GLU_FUNC( GUI, isAutoDisplay )() ? GLU_FUNC( GUI, refreashScreenArea )( config->area.xs, \
                                                 config->area.ys, \
                                                 config->area.xs+(int)(config->area.width )-1, \
                                                 config->area.ys+(int)(config->area.height)-1) \
                       :
                         GLU_FUNC( GUI, addScreenArea )     ( config->area.xs, \
                                                 config->area.ys, \
                                                 config->area.xs+(int)(config->area.width )-1, \
                                                 config->area.ys+(int)(config->area.height)-1);
    
    RH_FREE( config );
    BLK_FUNC( Graph, restoreCache )();
    __Font_restore_config();
    
    return MAKE_ENUM( kStatus_Success );
}
