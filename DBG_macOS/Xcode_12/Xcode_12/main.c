//
//  main.c
//  Xcode_12
//
//  Created by Randle.Helmslay on 2021/5/27.
//

#include <stdio.h>

#include "manila.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    SMP_Proj_Manila_init();
    SMP_Proj_Manila_round( 2 );
    SMP_Proj_Manila_setboat( M_MANILA_DOCK_A, M_MANILA_SHIPMENT_JADE   , 8 );
    SMP_Proj_Manila_setboat( M_MANILA_DOCK_B, M_MANILA_SHIPMENT_GINSENG, 5 );
    SMP_Proj_Manila_setboat( M_MANILA_DOCK_C, M_MANILA_SHIPMENT_SILK   , 11 );
    
    SMP_Proj_Manila_analyze();
    
    printf( "%f\n", Manila->accomplice_A.prob_profit );
    printf( "%f\n", Manila->accomplice_B.prob_profit );
    printf( "%f\n", Manila->accomplice_C.prob_profit );

    
    //...//
    
    SMP_Proj_Manila_deinit();
    return 0;
}
