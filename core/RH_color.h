#ifndef _RH_COLOR_H
#define _RH_COLOR_H

#include "RH_common.h"
#include "RH_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_BIN    )
  #define REVERSE_COLOR( M_COLOR )         (((M_COLOR)==0)?(0xff):(0x00))
  #define MAKE_COLOR(R_255,G_255,B_255)    (uint8_t)(((R_255+G_255+B_255)/3 > 128)?0xff:0x00)
  #define COLOR_MASK_RED                   0x01
  #define COLOR_MASK_GREEN                 0x01
  #define COLOR_MASK_BLUE                  0x01
  
  #define DARKEN_COLOR_1Bit(C)             (uint8_t)((C)&0)
  #define DARKEN_COLOR_2Bit(C)             (uint8_t)((C)&0)

#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB565    )
  
  #define MAKE_COLOR(R_255,G_255,B_255)    (uint16_t)(((R_255>>3)<<11)|((G_255>>2)<<5)|(B_255>>3))
  #define COLOR_MASK_RED                   0xF800
  #define COLOR_MASK_GREEN                 0x7E00
  #define COLOR_MASK_BLUE                  0x001F
  
  #define REVERSE_COLOR( M_COLOR )         (uint16_t)( (0xF800-(M_COLOR)&(0xF800)) | (0x7E00-(M_COLOR)&(0x7E00)) | (0x001F-(M_COLOR)&0x001F) )
  
  #define DARKEN_COLOR_1Bit(C)             (uint16_t)( ((((C)&COLOR_MASK_RED)>>1)&(COLOR_MASK_RED))|((((C)&COLOR_MASK_GREEN)>>1)&(COLOR_MASK_GREEN))|((((C)&COLOR_MASK_BLUE)>>1)&(COLOR_MASK_BLUE)) )
  #define DARKEN_COLOR_2Bit(C)             (uint16_t)( ((((C)&COLOR_MASK_RED)>>2)&(COLOR_MASK_RED))|((((C)&COLOR_MASK_GREEN)>>2)&(COLOR_MASK_GREEN))|((((C)&COLOR_MASK_BLUE)>>2)&(COLOR_MASK_BLUE)) )

#elif ( RH_CFG_GRAPHIC_COLOR_TYPE == RH_CFG_GRAPHIC_COLOR_RGB888    )

  #define MAKE_COLOR(R_255,G_255,B_255)    (uint32_t)((((R_255)&0xff)<<16)|(((G_255)&0xff)<<8)|((B_255)&0xff))
  #define COLOR_MASK_RED                   0x00FF0000
  #define COLOR_MASK_GREEN                 0x0000FF00
  #define COLOR_MASK_BLUE                  0x000000FF
  
  #define REVERSE_COLOR( M_COLOR )         (uint32_t)( (0x00FF0000-(M_COLOR)&(0x00FF0000)) | (0x0000FF00-(M_COLOR)&(0x0000FF00)) | (0x000000FF-(M_COLOR)&0x000000FF) )

  #define DARKEN_COLOR_1Bit(C)             (uint32_t)( ((((C)&COLOR_MASK_RED)>>1)&(COLOR_MASK_RED))|((((C)&COLOR_MASK_GREEN)>>1)&(COLOR_MASK_GREEN))|((((C)&COLOR_MASK_BLUE)>>1)&(COLOR_MASK_BLUE)) )
  #define DARKEN_COLOR_2Bit(C)             (uint32_t)( ((((C)&COLOR_MASK_RED)>>2)&(COLOR_MASK_RED))|((((C)&COLOR_MASK_GREEN)>>2)&(COLOR_MASK_GREEN))|((((C)&COLOR_MASK_BLUE)>>2)&(COLOR_MASK_BLUE)) )
#else
  #error "[RH_color]: Unknown color type."
#endif

// Standard
#define M_COLOR_WHITE                        (MAKE_COLOR(255,255,255))  // ??????
#define M_COLOR_BLACK                        (MAKE_COLOR(  0,  0,  0))  // ??????
           
#define M_COLOR_BLUE                         (MAKE_COLOR(  0,  0,255))  // ??????
#define M_COLOR_RED                          (MAKE_COLOR(255,  0,  0))  // ??????
#define M_COLOR_GREEN                        (MAKE_COLOR(  0,255,  0))  // ??????
#define M_COLOR_YELLOW                       (MAKE_COLOR(255,255,  0))  // ??????
#define M_COLOR_CYAN                         (MAKE_COLOR(  0,255,255))  // ??????
#define M_COLOR_MAGENTA                      (MAKE_COLOR(255,  0,255))  // ??????
           
// Red-Blue Series           
#define M_COLOR_PINK                         (MAKE_COLOR(255,192,203))  // ??????
#define M_COLOR_CRIMSON                      (MAKE_COLOR(220, 20, 60))  // ??????
#define M_COLOR_LAVENDERBLUSH                (MAKE_COLOR(255,240,245))  // ??????????????????
#define M_COLOR_PALEVIOLATRED                (MAKE_COLOR(219,112,147))  // ???????????????
#define M_COLOR_HOTPINK                      (MAKE_COLOR(255,105,180))  // ????????????
#define M_COLOR_MEDIUMVIOLATRED              (MAKE_COLOR(199, 21,133))  // ???????????????
#define M_COLOR_ORCHID                       (MAKE_COLOR(218,112,214))  // ?????????
#define M_COLOR_THISTLE                      (MAKE_COLOR(216,191,216))  // ??????
#define M_COLOR_PLUM                         (MAKE_COLOR(221,160,221))  // ??????
#define M_COLOR_VOILET                       (MAKE_COLOR(218,112,214))  // ?????????
#define M_COLOR_DARKVOILET                   (MAKE_COLOR(255,  0,255))  // ??????
#define M_COLOR_PURPLE                       (MAKE_COLOR(128,  0,128))  // ???
#define M_COLOR_MEDIUMORCHID                 (MAKE_COLOR(255,  0,255))  // ???????????????
#define M_COLOR_DARKVIOLET                   (MAKE_COLOR(148,  0,211))  // ????????????
#define M_COLOR_INDIGO                       (MAKE_COLOR( 75,  0,130))  // ??????
#define M_COLOR_BLUEVIOLET                   (MAKE_COLOR(138, 43,226))  // ????????????
#define M_COLOR_MEDIUMPURPLE                 (MAKE_COLOR(147,112,219))  // ?????????
#define M_COLOR_MEDIUMSLATEBLUE              (MAKE_COLOR(123,104,238))  // ???????????????
#define M_COLOR_SLATEBLUE                    (MAKE_COLOR(106, 90,205))  // ?????????
#define M_COLOR_DARKSLATEBLUE                (MAKE_COLOR( 72, 61,139))  // ????????????
#define M_COLOR_LAVENDER                     (MAKE_COLOR(230,230,250))  // ????????????
#define M_COLOR_GHOSTWHITE                   (MAKE_COLOR(248,248,255))  // ?????????
           
// Blue-Green Series           
#define M_COLOR_MEDIUMBLUE                   (MAKE_COLOR(  0,  0,205))  // ?????????
#define M_COLOR_MIDNIGHTBLUE                 (MAKE_COLOR( 25, 25,112))  // ?????????
#define M_COLOR_DARKBLUE                     (MAKE_COLOR(  0,  0,139))  // ??????
#define M_COLOR_NAVY                         (MAKE_COLOR(  0,  0,128))  // ?????????
#define M_COLOR_ROYALBLUE                    (MAKE_COLOR( 65,105,225))  // ?????????
#define M_COLOR_CORNFLOWERBLUE               (MAKE_COLOR(100,149,237))  // ????????????
#define M_COLOR_LIGHTSTEELBLUE               (MAKE_COLOR(176,196,222))  // ?????????
#define M_COLOR_LIGHTSLATEGRAY               (MAKE_COLOR(119,136,153))  // ????????????
#define M_COLOR_SLATEGRAY                    (MAKE_COLOR(112,128,144))  // ?????????
#define M_COLOR_DODGERBLUE                   (MAKE_COLOR( 30,114,255))  // ?????????
#define M_COLOR_ALICEBLUE                    (MAKE_COLOR(240,248,255))  // ????????????
#define M_COLOR_STEELBLUE                    (MAKE_COLOR( 70,130,180))  // ??????
#define M_COLOR_LIGHTSKYBLUE                 (MAKE_COLOR(135,206,250))  // ?????????
#define M_COLOR_SKYBLUE                      (MAKE_COLOR(135,206,235))  // ??????
#define M_COLOR_DEEPSKYBLUE                  (MAKE_COLOR(  0,191,255))  // ?????????
#define M_COLOR_LIGHTBLUE                    (MAKE_COLOR(173,216,230))  // ??????
#define M_COLOR_POWDERBLUE                   (MAKE_COLOR(176,224,230))  // ?????????
#define M_COLOR_CADETBLUE                    (MAKE_COLOR( 95,158,160))  // ?????????
#define M_COLOR_AZURE                        (MAKE_COLOR(245,255,255))  // ??????
#define M_COLOR_LIGHTCYAN                    (MAKE_COLOR(240,255,255))  // ??????
#define M_COLOR_PALETURQUOISE                (MAKE_COLOR(175,238,238))  // ???????????????
#define M_COLOR_AQUA                         (MAKE_COLOR(  0,255,255))  // ??????
#define M_COLOR_DARKTURQUOISE                (MAKE_COLOR(  0,206,209))  // ????????????
#define M_COLOR_DARKSLATEGRAY                (MAKE_COLOR( 47, 79, 79))  // ????????????
#define M_COLOR_DARKCYAN                     (MAKE_COLOR(  0,139,139))  // ?????????
#define M_COLOR_TEAL                         (MAKE_COLOR(  0,128,128))  // ?????????
#define M_COLOR_MEDIUMTURQUOISE              (MAKE_COLOR( 72,209,204))  // ???????????????
#define M_COLOR_LIGHTSEEGREEN                (MAKE_COLOR( 32,178,170))  // ????????????
#define M_COLOR_TURQUOISE                    (MAKE_COLOR( 64,224,208))  // ?????????
#define M_COLOR_AQUAMARINE                   (MAKE_COLOR(127,255,212))  // ??????
#define M_COLOR_MEDIUMAQUAMARINE             (MAKE_COLOR(102,205,170))  // ????????????
#define M_COLOR_MEDIUMSPRINGGREEN            (MAKE_COLOR(  0,250,154))  // ???????????????
#define M_COLOR_SPRINGGREEN                  (MAKE_COLOR(  0,255,127))  // ?????????
#define M_COLOR_MEDIUMSEEGREEN               (MAKE_COLOR( 60,179,113))  // ???????????????
#define M_COLOR_SEEGREEN                     (MAKE_COLOR( 46,139, 87))  // ?????????
#define M_COLOR_LIGHTGREEN                   (MAKE_COLOR(144,238,144))  // ??????
#define M_COLOR_PALEGREEN                    (MAKE_COLOR(152,251,152))  // ?????????
#define M_COLOR_DARKSEEGREEN                 (MAKE_COLOR(143,188,143))  // ????????????
#define M_COLOR_LIME                         (MAKE_COLOR( 50,205, 50))  // ?????????
#define M_COLOR_CHARTREUSE                   (MAKE_COLOR(127,255,  0))  // ????????????
           
// Green-RED Series           
#define M_COLOR_FORESTGREEN                  (MAKE_COLOR( 34,139, 34))  // ?????????
#define M_COLOR_LAWNGREEN                    (MAKE_COLOR(124,252,  0))  // ?????????
#define M_COLOR_GREENYELLOW                  (MAKE_COLOR(173,255, 47))  // ??????
#define M_COLOR_DARKOLIVEGREEN               (MAKE_COLOR( 85,107, 47))  // ????????????
#define M_COLOR_YELLOWGREEN                  (MAKE_COLOR(154,205, 50))  // ??????
#define M_COLOR_OLIVEDRAB                    (MAKE_COLOR( 34,139, 34))  // ?????????
#define M_COLOR_BEIGE                        (MAKE_COLOR(245,245,220))  // ??????
#define M_COLOR_LIGHTRODYELLOW               (MAKE_COLOR( 34,139, 34))  // ?????????
#define M_COLOR_IVORY                        (MAKE_COLOR(255,255,240))  // ?????????
#define M_COLOR_OLIVE                        (MAKE_COLOR(128,128,  0))  // ??????
#define M_COLOR_DARKKHAKI                    (MAKE_COLOR(189,183,107))  // ????????????
#define M_COLOR_LEMONCHIFFON                 (MAKE_COLOR(255,250,205))  // ?????????
#define M_COLOR_PALEGOLDROD                  (MAKE_COLOR(238,232,170))  // ??????
#define M_COLOR_KHAKI                        (MAKE_COLOR(240,230,140))  // ?????????
#define M_COLOR_GOLDEN                       (MAKE_COLOR(255,215,  0))  // ??????
#define M_COLOR_CORNMILK                     (MAKE_COLOR(255,248,220))  // ??????
#define M_COLOR_GOLDROD                      (MAKE_COLOR(218,165, 32))  // ??????
#define M_COLOR_DARKGOLDROD                  (MAKE_COLOR(184,134, 11))  // ??????
#define M_COLOR_FLORALWHITE                  (MAKE_COLOR(255,250,240))  // ??????
#define M_COLOR_OLDLACE                      (MAKE_COLOR(253,245,230))  // ?????????
#define M_COLOR_WHEAT                        (MAKE_COLOR(245,222,179))  // ??????
#define M_COLOR_MOCCASIN                     (MAKE_COLOR(255,228,181))  // ??????
#define M_COLOR_ORANGE                       (MAKE_COLOR(255,165,  0))  // ??????
#define M_COLOR_PAPAYAWHIP                   (MAKE_COLOR(255,239,213))  // ??????
#define M_COLOR_BLANCHEDALMOND               (MAKE_COLOR(255,235,205))  // ???????????????
#define M_COLOR_NAVAJOWHITE                  (MAKE_COLOR(255,222,173))  // ????????????
#define M_COLOR_ANTIQUEWHITE                 (MAKE_COLOR(250,235,215))  // ??????
#define M_COLOR_TAN                          (MAKE_COLOR(210,180,140))  // ???
#define M_COLOR_BURLYWOOD                    (MAKE_COLOR(222,184,135))  // ??????
#define M_COLOR_BISQUE                       (MAKE_COLOR(255,228,196))  // ??????
#define M_COLOR_DARKORANGE                   (MAKE_COLOR(255,140,  0))  // ?????????
#define M_COLOR_LINEN                        (MAKE_COLOR(255,240,230))  // ??????
#define M_COLOR_PERU                         (MAKE_COLOR(205,133, 63))  // ??????
#define M_COLOR_SANDYBROWN                   (MAKE_COLOR(244,164, 96))  // ??????
#define M_COLOR_CHOCOLATE                    (MAKE_COLOR(210,105, 30))  // ?????????
#define M_COLOR_SEASHELL                     (MAKE_COLOR(255,245,238))  // ??????
#define M_COLOR_SIENNA                       (MAKE_COLOR(160, 82, 45))  // ?????????
#define M_COLOR_SALMON                       (MAKE_COLOR(255,160,122))  // ?????????
#define M_COLOR_CORAL                        (MAKE_COLOR(255,127, 80))  // ?????????
#define M_COLOR_ORANGERED                    (MAKE_COLOR(255, 69,  0))  // ??????
#define M_COLOR_TOMATO                       (MAKE_COLOR(255, 99, 71))  // ??????
#define M_COLOR_MISTYROSE                    (MAKE_COLOR(255,228,225))  // ????????????
#define M_COLOR_BLOODYMEAT                   (MAKE_COLOR(250,128,114))  // ??????
#define M_COLOR_LIGHTCORAL                   (MAKE_COLOR(240,128,128))  // ????????????
#define M_COLOR_ROSEBROWN                    (MAKE_COLOR(188,143,143))  // ?????????
#define M_COLOR_INDIANRED                    (MAKE_COLOR(205, 92, 92))  // ?????????
#define M_COLOR_BROWN                        (MAKE_COLOR(165, 42, 42))  // ??????
#define M_COLOR_FIREBRICK                    (MAKE_COLOR(178, 34, 34))  // ??????
#define M_COLOR_DARKRED                      (MAKE_COLOR(139,  0,  0))  // ??????
#define M_COLOR_MAROON                       (MAKE_COLOR(128,  0,  0))  // ??????
           
// Neutral Series           
#define M_COLOR_WHITESMOKE                   (MAKE_COLOR(245,245,245))  // ??????
#define M_COLOR_GAINSBORO                    (MAKE_COLOR(220,220,220))  // ????????????
#define M_COLOR_LIGHTGRAY                    (MAKE_COLOR(211,211,211))  // ??????
#define M_COLOR_SILVER                       (MAKE_COLOR(192,192,192))  // ??????
#define M_COLOR_DARKGRAY                     (MAKE_COLOR( 73, 73, 73))  // ??????
#define M_COLOR_DIMGRAY                      (MAKE_COLOR( 54, 54, 54))  // ??????

#define M_COLOR_COAL                         (MAKE_COLOR( 34, 35, 34))  // ?????????


#ifdef __cplusplus
}
#endif

#endif



