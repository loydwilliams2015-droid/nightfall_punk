#include "nf_retc_h319_material_transport.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CK(x) do {if(!(x)){fprintf(stderr,"H319 O1 FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)

static void reset_cells(NfRetcH319Cell *c){
    for(size_t i=0;i<4u;++i){
        c[i]=(NfRetcH319Cell){1000,6000,
            {(float)i+.5f,.5f,.5f}};
    }
}
static int equal_cells(const NfRetcH319Cell *a,const NfRetcH319Cell *b){
    for(size_t i=0;i<4u;++i)
       if(a[i].mass_units!=b[i].mass_units||
          a[i].thermal_units!=b[i].thermal_units)return 0;
    return 1;
}
static int conserved(const NfRetcH319Cell *c){
    int64_t mass=0,thermal=0;
    for(size_t i=0;i<4u;++i) {
        mass+=c[i].mass_units;thermal+=c[i].thermal_units;
    }
    return mass==4000&&thermal==24000;
}
static int receipt_equal(NfRetcH319TransportReceipt a,
                         NfRetcH319TransportReceipt b) {
    return a.tick==b.tick&&a.material_epoch==b.material_epoch&&
        a.cells==b.cells&&a.edges==b.edges&&
        a.total_mass==b.total_mass&&a.total_thermal==b.total_thermal;
}
int main(void){
    Nf18a5Grid *g=malloc(sizeof *g);CK(g);
    nf18a5_grid_init(g,1u,NF18A5_SELECTIVE_FINE);
    uint8_t voxels[NF18A5_CANON_VOXELS]={0};
    CK(nf18a5_load_canonical(g,0,0,0,voxels,1u));
    NfRetcH319Cell a[4],b[4],saved[4];
    reset_cells(a);reset_cells(b);
    const NfRetcH319Edge edges[4]={
        {0u,1u,10,20},{1u,2u,20,40},
        {2u,3u,30,60},{3u,0u,40,80}
    };
    const NfRetcH319Edge reverse[4]={
        {3u,0u,40,80},{2u,3u,30,60},
        {1u,2u,20,40},{0u,1u,10,20}
    };
    unsigned applied=0,denied=0;
    const uint32_t epoch=g->global_revision;
    for(uint32_t tick=1u;tick<=64u;++tick){
        NfRetcH319TransportReceipt ar={0},br={0};
        CK(nf_retc_h319_transport(g,tick,epoch,a,4u,edges,4u,&ar)
           ==NF_RETC_H319_TRANSFER_APPLIED);
        CK(nf_retc_h319_transport(g,tick,epoch,b,4u,reverse,4u,&br)
           ==NF_RETC_H319_TRANSFER_APPLIED);
        CK(equal_cells(a,b)&&conserved(a)&&conserved(b));
        CK(receipt_equal(ar,br));
        CK(ar.total_mass==4000&&ar.total_thermal==24000);
        CK(ar.material_epoch==epoch&&ar.tick==tick);
        applied+=2u;
    }
    memcpy(saved,a,sizeof saved);
    NfRetcH319TransportReceipt receipt={
         0xaabbccddU,0x12345678U,17u,19u,33,44};
    const NfRetcH319TransportReceipt sentinel=receipt;
    #define DENY(ee,nn,tk,ep,expect) do { \
       CK(nf_retc_h319_transport(g,tk,ep,a,4u,ee,nn,&receipt)==(expect)); \
       CK(equal_cells(a,saved)&&receipt_equal(receipt,sentinel));++denied; \
    } while(0)
    NfRetcH319Edge duplicated[2]={edges[0],edges[0]};
    DENY(duplicated,2u,65u,epoch,NF_RETC_H319_TRANSFER_INVALID);
    NfRetcH319Edge overdraw[2]={
        {0u,1u,2000,1},{0u,2u,2000,1}};
    /* Duplicate-direction independent destinations cannot borrow incoming. */
    DENY(overdraw,2u,65u,epoch,NF_RETC_H319_TRANSFER_INVALID);
    NfRetcH319Edge self={0u,0u,1,1};
    DENY(&self,1u,65u,epoch,NF_RETC_H319_TRANSFER_INVALID);
    NfRetcH319Edge out_of_range={0u,4u,1,1};
    DENY(&out_of_range,1u,65u,epoch,NF_RETC_H319_TRANSFER_INVALID);
    NfRetcH319Edge negative={0u,1u,-1,1};
    DENY(&negative,1u,65u,epoch,NF_RETC_H319_TRANSFER_INVALID);
    DENY(edges,4u,65u,epoch+1u,NF_RETC_H319_TRANSFER_INVALID);
    const NfVec3 old_sample=a[0].sample;
    a[0].sample.x=NAN;
    DENY(edges,4u,65u,epoch,NF_RETC_H319_TRANSFER_INVALID);
    a[0].sample=old_sample;
    memset(voxels,NF18A5_MIXED,sizeof voxels);
    CK(nf18a5_load_canonical(g,0,0,0,voxels,66u));
    const uint32_t mixed_epoch=g->global_revision;
    DENY(edges,4u,66u,mixed_epoch,NF_RETC_H319_TRANSFER_PENDING);
    memset(voxels,NF18A5_SOLID,sizeof voxels);
    CK(nf18a5_load_canonical(g,0,0,0,voxels,67u));
    DENY(edges,4u,67u,g->global_revision,NF_RETC_H319_TRANSFER_BLOCKED);
    CK(conserved(a)&&denied==9u);
    printf("H319 O1_TRANSPORT_COMPLETE steps=64 applied=%u refused=%u mass=4000 thermal=24000 order_invariant=1 rollback=1 source_A5=1 A6_owner=0 B2_grants=0 B3_effects=0\n",applied,denied);
    free(g);
    return 0;
}
