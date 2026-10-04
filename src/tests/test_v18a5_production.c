#include "nf_contact18a5.h"
#include <stdio.h>
int main(void){
    Nf18a5Grid g;uint8_t coarse[64]={0};coarse[0]=NF18A5_MIXED;
    nf18a5_grid_init(&g,1,NF18A5_UNSAFE_FINE_CONTROL);
    if(g.policy==NF18A5_UNSAFE_FINE_CONTROL)return 1;
    if(!nf18a5_load_canonical(&g,0,0,0,coarse,1))return 2;
    if(nf18a5_query(&g,0.1f,0.1f,0.1f,1,true)!=NF18A5_Q_PENDING)return 3;
    puts("production unsafe-grid policy: DISABLED (PASS)");
    return 0;
}
