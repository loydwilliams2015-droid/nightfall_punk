#include "nf_retc_h319_material_ledger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CK(x) do {if(!(x)){fprintf(stderr,"H319 LEDGER FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int64_t sum_mass(const NfRetcH319Cell a[2]) {return a[0].mass_units+a[1].mass_units;}
static int64_t sum_heat(const NfRetcH319Cell a[2]) {return a[0].thermal_units+a[1].thermal_units;}
static int unchanged(const NfRetcH319Cell a[2],const NfRetcH319Cell b[2]){
    return a[0].mass_units==b[0].mass_units &&
      a[1].mass_units==b[1].mass_units &&
      a[0].thermal_units==b[0].thermal_units &&
      a[1].thermal_units==b[1].thermal_units;
}
int main(void){
    Nf18a5Grid *g=malloc(sizeof *g);
    CK(g);
    nf18a5_grid_init(g,1u,NF18A5_SELECTIVE_FINE);
    uint8_t cells[NF18A5_CANON_VOXELS]={0};
    CK(nf18a5_load_canonical(g,0,0,0,cells,1u));
    NfRetcH319Cell ledger[2]={
      {10000,60000,{1.5f,.5f,1.5f}},
      {2000,10000,{2.5f,.5f,1.5f}}};
    const int64_t mass0=sum_mass(ledger),heat0=sum_heat(ledger);
    for(unsigned i=0;i<128u;++i){
        const unsigned source=(i&1u);
        const NfRetcH319Transfer status=nf_retc_h319_transfer(
             g,1u,&ledger[source],&ledger[1u-source],10,20);
        CK(status==NF_RETC_H319_TRANSFER_APPLIED);
        CK(sum_mass(ledger)==mass0&&sum_heat(ledger)==heat0);
    }
    NfRetcH319Cell before[2];
    memcpy(before,ledger,sizeof before);
    CK(nf_retc_h319_transfer(g,1u,&ledger[0],&ledger[1],INT64_MAX,1)
       ==NF_RETC_H319_TRANSFER_INVALID);
    CK(unchanged(ledger,before));
    CK(nf_retc_h319_transfer(g,1u,&ledger[0],&ledger[0],1,1)
       ==NF_RETC_H319_TRANSFER_INVALID);
    CK(unchanged(ledger,before));
    CK(nf_retc_h319_transfer(g,1u,&ledger[0],&ledger[1],0,0)
       ==NF_RETC_H319_TRANSFER_INVALID);
    CK(unchanged(ledger,before));
    memset(cells,NF18A5_MIXED,sizeof cells);
    CK(nf18a5_load_canonical(g,0,0,0,cells,2u));
    CK(nf_retc_h319_transfer(g,2u,&ledger[0],&ledger[1],1,1)
       ==NF_RETC_H319_TRANSFER_PENDING);
    CK(unchanged(ledger,before));
    memset(cells,NF18A5_SOLID,sizeof cells);
    CK(nf18a5_load_canonical(g,0,0,0,cells,3u));
    CK(nf_retc_h319_transfer(g,3u,&ledger[0],&ledger[1],1,1)
       ==NF_RETC_H319_TRANSFER_BLOCKED);
    CK(unchanged(ledger,before));
    CK(sum_mass(ledger)==mass0&&sum_heat(ledger)==heat0);
    printf("H319 ONI_LEDGER_COMPLETE transfers=128 mass=%lld thermal=%lld denied=5 rollback=1 source_A5=1 A6_owner=0 B2_grants=0 B3_effects=0\n",
       (long long)mass0,(long long)heat0);
    free(g);
    return 0;
}
