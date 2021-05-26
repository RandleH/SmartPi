// jade, ginseng, silk and nutmeg


#define M_MANILA_SHIPMENT_JADE      0 
#define M_MANILA_SHIPMENT_SILK      1 
#define M_MANILA_SHIPMENT_NUTMEG    2 
#define M_MANILA_SHIPMENT_GINSENG   3 
#define M_MANILA_SHIPMENT_CNT       4

#define M_MANILA_BOATPOS_MAX        (13+1) // 上岸

#define M_MANILA_ROUND_MAX          3
#define M_MANILA_ROUND_MIN          1

#include "RH_config.h"
#define PROJ_MALLOC(x)              RH_MALLOC(x)
#define PROJ_CALLOC(size,x)         RH_CALLOC(size,x)
#define PROJ_FREE(x)                RH_FREE(x)


struct __ProjManilaInfo_t{
    int8_t money_profit;                       
    int8_t money_loss;                         
    int8_t prob_profit;       // in precentage
    int8_t prob_loss;         // in precentage
};
typedef struct __ProjManilaInfo_t __ProjManilaInfo_t;

struct __ProjManilaBoat_t{
    int8_t pos;
    int8_t shipment;
};
typedef struct __ProjManilaBoat_t __ProjManilaBoat_t;

struct __ProjManila_t{
    int8_t shipment_depricated;

    // int8_t shipment_ABC[3];
    __ProjManilaBoat_t boat[3];
    // int8_t boatpos_Jade;
    // int8_t boatpos_Silk;
    // int8_t boatpos_Nutmeg;
    // int8_t boatpos_Ginseng;
    
    int8_t dice_round;

    __ProjManilaInfo_t  pirate         [2];
    __ProjManilaInfo_t  boatman_jade   [4];
    __ProjManilaInfo_t  boatman_silk   [4];
    __ProjManilaInfo_t  boatman_nutmeg [4];
    __ProjManilaInfo_t  boatman_ginseng[4];
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
void SMP_Proj_Manila_setboatpos ( int M_MANILA_SHIPMENT_xxxx, int8_t pos );
int8_t SMP_Proj_Manila_getboatpos ( int M_MANILA_SHIPMENT_xxxx );



