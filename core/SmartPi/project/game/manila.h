// jade, ginseng, silk and nutmeg

#include "RH_config.h"

#ifndef _SMP_PROJ_MANILA_H
#define _SMP_PROJ_MANILA_H

#ifdef  RH_DEBUG
#define MANILA_DEBUG
#endif

#define PROJ_MALLOC(x)              RH_MALLOC(x)
#define PROJ_CALLOC(size,x)         RH_CALLOC(size,x)
#define PROJ_FREE(x)                RH_FREE(x)
#define PROJ_ASSERT(expr)           RH_ASSERT(expr)

#define M_MANILA_SHIPMENT_JADE      0 
#define M_MANILA_SHIPMENT_SILK      1 
#define M_MANILA_SHIPMENT_NUTMEG    2 
#define M_MANILA_SHIPMENT_GINSENG   3 
#define M_MANILA_SHIPMENT_CNT       4

#define M_MANILA_DOCK_A            0
#define M_MANILA_DOCK_B            1
#define M_MANILA_DOCK_C            2

#define M_MANILA_BOATPOS_MAX        (13+1) // 上岸

#define M_MANILA_ROUND_MAX          3
#define M_MANILA_ROUND_MIN          1


/*=======================================================================================================
 
 [ Shipyard C ]                       [ Wharf A   ]
 [ Shipyard B ]                       [ Wharf B   ]
 [ Shipyard A ]                       [ Wharf C   ]
                                      [ Insurance ]
 
 [ Pirate ]       -----------------   [13]
                  -----------------   [12]
                  -----------------   [11]
                  -----------------   [10]
                  -----------------   [ 9]
                  -----------------   [ 8]
                  -----------------   [ 7]
                  -----------------   [ 6]
                  -----------------   [ 5]
                  -----------------   [ 4]
                  -----------------   [ 3]
                  -----------------   [ 2]
                  -----------------   [ 1]
                  -----------------   [ 0]
 
                  [ A ] [ B ] [ C ]
 
 ======================================================================================================*/
typedef float Prob_t;

struct __ProjManilaInfo_t{
    int8_t money_profit;                       
    int8_t money_loss;                         
    Prob_t prob_profit;       // in precentage
    Prob_t prob_loss;         // in precentage
};
typedef struct __ProjManilaInfo_t __ProjManilaInfo_t;

struct __ProjManilaBoat_t{
    int8_t pos;
    int8_t shipment;
};
typedef struct __ProjManilaBoat_t __ProjManilaBoat_t;

struct __ProjManila_t{
    int8_t shipment_depricated;

    __ProjManilaBoat_t boat[3];
    
    int8_t dice_round;

    __ProjManilaInfo_t  pirate[2];
    
    __ProjManilaInfo_t  accomplice_A;
    __ProjManilaInfo_t  accomplice_B;
    __ProjManilaInfo_t  accomplice_C;
    __ProjManilaInfo_t  wharf_A;
    __ProjManilaInfo_t  wharf_B;
    __ProjManilaInfo_t  wharf_C;
    __ProjManilaInfo_t  shipyard_A;
    __ProjManilaInfo_t  shipyard_B;
    __ProjManilaInfo_t  shipyard_C;
    __ProjManilaInfo_t  insurance;
};
typedef struct __ProjManila_t __ProjManila_t;

extern __ProjManila_t* Manila;


void SMP_Proj_Manila_init    ( void );
void SMP_Proj_Manila_deinit  ( void );
void SMP_Proj_Manila_analyze ( void );
void SMP_Proj_Manila_round   ( int8_t round );



void   SMP_Proj_Manila_setboat    ( int8_t M_MANILA_ROUTE_xxxx, int8_t M_MANILA_SHIPMENT_xxxx, int8_t pos );
int8_t SMP_Proj_Manila_getboatpos ( int M_MANILA_SHIPMENT_xxxx );


#endif
