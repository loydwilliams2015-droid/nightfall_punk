#include "nf_c2_h22_envelope.h"
#include "nf_embody18a6.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"H22 failed line %d\n",__LINE__);return 1;}}while(0)
int main(void){
 NfWorld *w=calloc(1,sizeof *w);Nf18a6Runtime *a=calloc(1,sizeof *a);
 Nf18a5Integrated *recovered=calloc(1,sizeof *recovered);
 CK(w&&a&&recovered);
 nf_world_init(w,42);
 CK(nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1})==10);
 Nf18a4Body crate={0};crate.id=2010;crate.center=(NfVec3){2,1,1};
 crate.inverse_mass=1.f/24.f;crate.box_half=(NfVec3){.2f,.5f,.35f};
 CK(nf18a6_init(a,w,10,crate,3,NF18A6_WORLD_BRIDGE));
 a->physical.world.tick=1;a->physical.committed_receipts=1;
 a->physical.world.history.raw_samples=1;
 NfC2H22Receipt source={.tick=1,.actor=10,.action=77,.a4_receipts=1,.a5_samples=1,.b3_receipts=1},rec={0};
 char path[160];snprintf(path,sizeof path,"/tmp/nightfall_h22_%ld.bin",(long)getpid());
 remove(path);
 CK(!nf_c2_h22_restore(path,recovered,&rec)); /* crash before publication */
 CK(nf_c2_h22_save(path,&a->physical,source));
 CK(nf_c2_h22_restore(path,recovered,&rec));
 CK(recovered->world.tick==1&&rec.action==77&&rec.b3_receipts==1);
 NfC2H22Receipt invalid=source;invalid.b3_receipts=2;
 CK(!nf_c2_h22_save(path,&a->physical,invalid));
 CK(nf_c2_h22_restore(path,recovered,&rec)); /* old committed state survives */
 FILE *f=fopen(path,"ab");CK(f!=NULL);
 CK(fputc(42,f)!=EOF&&fclose(f)==0);
 CK(!nf_c2_h22_restore(path,recovered,&rec)); /* interrupted/torn frame */
 CK(remove(path)==0);
 puts("H22 unified checkpoint: missing, complete, invalid and torn cases PASS");
 free(recovered);free(a);free(w);return 0;
}
