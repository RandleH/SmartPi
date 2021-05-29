

#include "./manila.h"



#define DICE_NUM_MAX      6
#define DICE_NUM_MIN      1

#define PROJ_MIN(a,b)                           (((a)<(b))?(a):(b))
#define PROJ_MAX(a,b)                           (((a)>(b))?(a):(b))

__ProjManila_t* Manila = NULL;

static int8_t __dice_cnt( int8_t target, int8_t try_num ){
    if( target > try_num*DICE_NUM_MAX || target<try_num || try_num==0 )
        return 0;
    
    // dp的下标表示 点数之和，值为方法数量
    uint32_t* dp = alloca( (try_num*DICE_NUM_MAX+1) * sizeof(uint32_t) );

    memset(dp, 0, (try_num*DICE_NUM_MAX+1) * sizeof(uint32_t));
    for( int j=1; j<=DICE_NUM_MAX; j++ )
        dp[j]=1;
    for(int i=2;i<=try_num;i++){
        for(int j=target;j>=1;j--){
            //注意j从1开始，因为需要初始化每一行DP[j]，下一行会用到上一行的DP[j]，而上一行某些部分应当为0，即必须全部初始化，与上一个DP方法比较下就知道了。
            dp[j]=0; //每一行初始化
            if( j<i )
                continue;
            for(int k=1; k<=DICE_NUM_MAX && k<=j; k++){
                dp[j]=(dp[j]+dp[j-k]);
            }
        }
    }
    return dp[target];
}


void SMP_Proj_Manila_init(void){
    Manila = PROJ_CALLOC( 1, sizeof(*Manila) );

#ifdef MANILA_DEBUG
    RH_ASSERT( Manila );
#endif
    Manila->boat[0].shipment = M_MANILA_SHIPMENT_JADE;
    Manila->boat[1].shipment = M_MANILA_SHIPMENT_SILK;
    Manila->boat[2].shipment = M_MANILA_SHIPMENT_NUTMEG;
    
    Manila->boat[0].pos      = 0;
    Manila->boat[1].pos      = 0;
    Manila->boat[2].pos      = 0;


    Manila->shipment_depricated = M_MANILA_SHIPMENT_GINSENG;
    
    Manila->dice_round       = 1;

}

void SMP_Proj_Manila_setboat   ( int8_t M_MANILA_DOCK_xxxx, int8_t M_MANILA_SHIPMENT_xxxx, int8_t pos ){
#ifdef MANILA_DEBUG
    RH_ASSERT( M_MANILA_DOCK_xxxx < 3 );
    RH_ASSERT( M_MANILA_SHIPMENT_xxxx < M_MANILA_SHIPMENT_CNT );
#endif
    
    if( pos > 14 )
        pos = 14;

    if( pos < 0 )
        pos = 0;
    
    Manila->boat[ M_MANILA_DOCK_xxxx ].pos      = pos;
    Manila->boat[ M_MANILA_DOCK_xxxx ].shipment = M_MANILA_SHIPMENT_xxxx;
    
    //...//

}

void SMP_Proj_Manila_round( int8_t round ){
    Manila->dice_round = round;
}

void SMP_Proj_Manila_deinit(void){
    PROJ_FREE( Manila );
    Manila = NULL;
}

void SMP_Proj_Manila_analyze ( void ){
    
    
    int8_t try_num = (M_MANILA_ROUND_MAX - Manila->dice_round+1);
    int8_t target  = try_num*DICE_NUM_MAX;      // 从可投掷的最大点数开始计算
//#ifdef MANILA_DEBUG
//    RH_ASSERT( target > M_MANILA_BOATPOS_MAX );
//#endif

    

    // 投掷try_num次骰子所有可组合数量 为 DICE_NUM_MAX^try_num
    uint32_t total = 1;
    for( int8_t i=0; i<try_num; i++ ){
        total *= DICE_NUM_MAX;
    }
    
    Prob_t prob_A_Success = 0;
    Prob_t prob_B_Success = 0;
    Prob_t prob_C_Success = 0;

    // 分析 shipyard_A
    {
        uint32_t num   = 0;
        Manila->shipyard_A.money_loss   = 4;
        Manila->shipyard_A.money_profit = 2;
        // 能够使得船上岸的组合数量
        while( Manila->boat[0].pos + target > M_MANILA_BOATPOS_MAX ){
            num += __dice_cnt( target, try_num );
            target--;
        }
        prob_A_Success = (num*100.0)/(total);

        num        = 0;
        target     = try_num*DICE_NUM_MAX;
        // 能够使得船上岸的组合数量
        while( Manila->boat[1].pos + target >= M_MANILA_BOATPOS_MAX ){
            num += __dice_cnt( target, try_num );
            target--;
        }
        prob_B_Success= num*100.0/total;

        num        = 0;
        target     = try_num*DICE_NUM_MAX;
        // 能够使得船上岸的组合数量
        while( Manila->boat[2].pos + target >= M_MANILA_BOATPOS_MAX ){
            num += __dice_cnt( target, try_num );
            target--;
        }
        prob_C_Success= num*100.0/(total);


        Manila->shipyard_A.prob_profit = 100 - (100-prob_A_Success)*(100-prob_B_Success)*(100-prob_C_Success)/10000;
        Manila->shipyard_A.prob_loss   = 100 - Manila->shipyard_A.prob_profit;
    }
    
    // 分析 shipyard_B
    {
        Manila->shipyard_B.money_loss   = 3;
        Manila->shipyard_B.money_profit = 5;
        Manila->shipyard_B.prob_profit  =   (   prob_A_Success*prob_B_Success*(100-prob_C_Success)\
                                              + prob_A_Success*prob_C_Success*(100-prob_B_Success)\
                                              + prob_B_Success*prob_C_Success*(100-prob_A_Success))/10000;
        Manila->shipyard_B.prob_loss    = 100 - Manila->shipyard_B.prob_profit;
    }

    // 分析 shipyard_C
    {
        Manila->shipyard_C.money_loss   = 2;
        Manila->shipyard_C.money_profit = 13;
        Manila->shipyard_C.prob_profit  = (prob_A_Success*prob_B_Success*prob_C_Success)/10000;
        Manila->shipyard_C.prob_loss    = 100 - Manila->shipyard_C.prob_profit;
    }

    // 分析 wharf_A
    {
        Manila->wharf_A.money_loss   = 4;
        Manila->wharf_A.money_profit = 2;
        Manila->wharf_A.prob_profit  = 100 - Manila->shipyard_C.prob_profit;
        Manila->wharf_A.prob_loss    = Manila->shipyard_C.prob_profit;
    }

    // 分析 wharf_B
    {
        Manila->wharf_B.money_loss   = 3;
        Manila->wharf_B.money_profit = 5;
        Manila->wharf_B.prob_profit  = (  prob_A_Success*(100-prob_B_Success)*(100-prob_C_Success)\
                                        + prob_B_Success*(100-prob_A_Success)*(100-prob_C_Success)\
                                        + prob_C_Success*(100-prob_A_Success)*(100-prob_B_Success))/10000;
        Manila->wharf_B.prob_loss    = 100 - Manila->wharf_B.prob_profit;
    }

    // 分析 wharf_C
    {
        Manila->wharf_C.money_loss   = 2;
        Manila->wharf_C.money_profit = 13;
        Manila->wharf_C.prob_profit  = (100-prob_A_Success)*(100-prob_B_Success)*(100-prob_C_Success)/10000;
        Manila->wharf_C.prob_loss    = 100 - Manila->wharf_C.prob_profit;
    }
    
    // 分析 accomplice A/B/C
    {
        const int8_t profit[ M_MANILA_SHIPMENT_CNT ] = { [ M_MANILA_SHIPMENT_JADE    ] = 36,
                                                         [ M_MANILA_SHIPMENT_SILK    ] = 30,
                                                         [ M_MANILA_SHIPMENT_NUTMEG  ] = 24,
                                                         [ M_MANILA_SHIPMENT_GINSENG ] = 18 };
        
        const int8_t loss  [ M_MANILA_SHIPMENT_CNT ] = { [ M_MANILA_SHIPMENT_JADE    ] = 3,
                                                         [ M_MANILA_SHIPMENT_SILK    ] = 3,
                                                         [ M_MANILA_SHIPMENT_NUTMEG  ] = 2,
                                                         [ M_MANILA_SHIPMENT_GINSENG ] = 1 };
        
        Manila->accomplice_A.money_profit = profit [ Manila->boat[ M_MANILA_DOCK_A ].shipment ];
        Manila->accomplice_A.money_loss   = loss   [ Manila->boat[ M_MANILA_DOCK_A ].shipment ];
        Manila->accomplice_A.prob_profit  = prob_A_Success;
        Manila->accomplice_A.prob_loss    = 100-prob_A_Success;
        
        Manila->accomplice_B.money_profit = profit [ Manila->boat[ M_MANILA_DOCK_B ].shipment ];
        Manila->accomplice_B.money_loss   = loss   [ Manila->boat[ M_MANILA_DOCK_B ].shipment ];
        Manila->accomplice_B.prob_profit  = prob_B_Success;
        Manila->accomplice_B.prob_loss    = 100-prob_B_Success;
        
        Manila->accomplice_C.money_profit = profit [ Manila->boat[ M_MANILA_DOCK_C ].shipment ];
        Manila->accomplice_C.money_loss   = loss   [ Manila->boat[ M_MANILA_DOCK_C ].shipment ];
        Manila->accomplice_C.prob_profit  = prob_C_Success;
        Manila->accomplice_C.prob_loss    = 100-prob_C_Success;
    }
    
    
    
    //...//
}



