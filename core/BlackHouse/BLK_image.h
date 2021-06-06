#ifndef _BLK_IMAGE_H
#define _BLK_IMAGE_H

#include "RH_common.h"
#include "BLK_lib.h"
#include "BLK_math.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================================================================
 > Image Processing Reference
============================================================================================================================*/
typedef uint8_t BLK_TYPE(PixelBin);
union BLK_UION(PixelBin){
    struct{
        uint8_t BIT0 : 1;
        uint8_t BIT1 : 1;
        uint8_t BIT2 : 1;
        uint8_t BIT3 : 1;
        uint8_t BIT4 : 1;
        uint8_t BIT5 : 1;
        uint8_t BIT6 : 1;
        uint8_t BIT7 : 1;
    };
    uint8_t data;
};
typedef union BLK_UION(PixelBin) BLK_UION(PixelBin);

typedef uint16_t BLK_TYPE(Pixel565);
struct BLK_SRCT(Pixel565){
    uint16_t B : 5;
    uint16_t G : 6;
    uint16_t R : 5;
};
union BLK_UION(Pixel565){
    struct{
        uint16_t B : 5;
        uint16_t G : 6;
        uint16_t R : 5;
    };
    uint16_t data;
};
typedef struct  BLK_SRCT(Pixel565)   BLK_SRCT(Pixel565);
typedef union   BLK_UION(Pixel565)   BLK_UION(Pixel565);

typedef uint32_t BLK_TYPE(Pixel888);
 struct BLK_SRCT(Pixel888){
     uint8_t B ;
     uint8_t G ;
     uint8_t R ;
 };
union BLK_UION(Pixel888){
    struct{
        uint8_t B : 8;
        uint8_t G : 8;
        uint8_t R : 8;
    };
    uint32_t data;
};
typedef struct  BLK_SRCT(Pixel888)   BLK_SRCT(Pixel888);
typedef union   BLK_UION(Pixel888)   BLK_UION(Pixel888);

struct BLK_SRCT(ImgBin){
    BLK_UION(PixelBin)*    pBuffer;
    size_t      width;
    size_t      height;
};
typedef struct BLK_SRCT(ImgBin)  BLK_SRCT(ImgBin);

struct BLK_SRCT(Img565){
    BLK_UION(Pixel565)*    pBuffer;
    size_t      width;
    size_t      height;
};
typedef struct BLK_SRCT(Img565)  BLK_SRCT(Img565);
 

struct BLK_SRCT(Img888){
    BLK_UION(Pixel888)* pBuffer;
    size_t      width;
    size_t      height;
};
typedef struct BLK_SRCT(Img888)  BLK_SRCT(Img888);

BLK_SRCT(ImgBin)* BLK_FUNC( ImgBin, create       ) (size_t width,size_t height);
BLK_SRCT(Img565)* BLK_FUNC( Img565, create       ) (size_t width,size_t height);//
BLK_SRCT(Img888)* BLK_FUNC( Img888, create       ) (size_t width,size_t height);


BLK_SRCT(ImgBin)* BLK_FUNC( ImgBin, load_bmp     ) (const char* __restrict__ path);
BLK_SRCT(Img565)* BLK_FUNC( Img565, load_bmp     ) (const char* __restrict__ path);//
BLK_SRCT(Img888)* BLK_FUNC( Img888, load_bmp     ) (const char* __restrict__ path);
BLK_SRCT(Img888)* BLK_FUNC( Img888, load_png     ) (const char* __restrict__ path);//

BLK_SRCT(ImgBin)* BLK_FUNC( ImgBin, out_bmp      ) (const char* __restrict__ path,BLK_SRCT(ImgBin)* p);
BLK_SRCT(Img565)* BLK_FUNC( Img565, out_bmp      ) (const char* __restrict__ path,BLK_SRCT(Img565)* p);//
BLK_SRCT(Img888)* BLK_FUNC( Img888, out_bmp      ) (const char* __restrict__ path,BLK_SRCT(Img888)* p);

BLK_SRCT(ImgBin)* BLK_FUNC( ImgBin, copy         ) (const BLK_SRCT(ImgBin)* src,BLK_SRCT(ImgBin)* dst);
BLK_SRCT(Img565)* BLK_FUNC( Img565, copy         ) (const BLK_SRCT(Img565)* src,BLK_SRCT(Img565)* dst);//
BLK_SRCT(Img888)* BLK_FUNC( Img888, copy         ) (const BLK_SRCT(Img888)* src,BLK_SRCT(Img888)* dst);

BLK_SRCT(Img565)* BLK_FUNC( Img565, conv2D       ) (const BLK_SRCT(Img565)* src,BLK_SRCT(Img565)* dst,const __Kernel_t* k,uint16_t br_100);
BLK_SRCT(Img888)* BLK_FUNC( Img888, conv2D       ) (const BLK_SRCT(Img888)* src,BLK_SRCT(Img888)* dst,const __Kernel_t* k,uint16_t br_100);

void              BLK_FUNC( Img888, free         ) (      BLK_SRCT(Img888)* ptr);
BLK_SRCT(Img888)* BLK_FUNC( Img888, free_buffer  ) (      BLK_SRCT(Img888)* ptr);
 
BLK_SRCT(Img888)* BLK_FUNC( Img888, filter_gray  ) (const BLK_SRCT(Img888)* src,BLK_SRCT(Img888)* dst,uint32_t br_100);
BLK_SRCT(Img888)* BLK_FUNC( Img888, filter_cold  ) (const BLK_SRCT(Img888)* src,BLK_SRCT(Img888)* dst,uint32_t br_100);//
BLK_SRCT(Img888)* BLK_FUNC( Img888, filter_warm  ) (const BLK_SRCT(Img888)* src,BLK_SRCT(Img888)* dst,uint32_t br_100);//
BLK_SRCT(Img888)* BLK_FUNC( Img888, filter_OTUS  ) (const BLK_SRCT(Img888)* src,BLK_SRCT(Img888)* dst,uint32_t br_100);

BLK_SRCT(Img888)* BLK_FUNC( Img888, trans_mirror ) (const BLK_SRCT(Img888)* src,BLK_SRCT(Img888)* dst,uint8_t HV);

BLK_SRCT(Img888)* BLK_FUNC( Img888, blur_gussian ) (const BLK_SRCT(Img888)* src,BLK_SRCT(Img888)* dst,__Area_t* area,uint32_t radSize, uint16_t br_100);
BLK_SRCT(Img888)* BLK_FUNC( Img888, blur_average ) (const BLK_SRCT(Img888)* src,BLK_SRCT(Img888)* dst,__Area_t* area,uint32_t radSize, uint16_t br_100);
BLK_SRCT(Img888)* BLK_FUNC( Img888, blur_fast    ) (const BLK_SRCT(Img888)* src,BLK_SRCT(Img888)* dst,__Area_t* area,uint32_t radSize, uint16_t br_100);//


BLK_SRCT(Img888)* BLK_FUNC( Img888, insert_NstNeighbor ) (const BLK_SRCT(Img888)* src,BLK_SRCT(Img888)* dst,size_t height,size_t width);



void              BLK_FUNC( Img888, data_OTUS    ) (const BLK_SRCT(Img888)* src,uint32_t* threshold);


#ifdef __cplusplus
}
#endif

#endif
