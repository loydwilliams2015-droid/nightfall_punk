#include "nf_c2_h21_reconcile.h"
#include <stdio.h>
NfC2H21Status nf_c2_h21_reconcile(const char *file,
 uint32_t tick,uint32_t actor,uint32_t action,
 uint32_t a4,uint32_t a5,uint32_t b3){
 if(!file||!tick||!actor||!action||!a4||!a5||!b3)return NF_C2_H21_REJECT;
 FILE *f=fopen(file,"rb");
 if(!f)return NF_C2_H21_UNRESOLVED; /* A5 could have committed already */
 uint32_t fields[7];bool found=false;bool corrupted=false;
 while(fread(fields,sizeof fields,1,f)==1){
  uint32_t hash=fields[0]^fields[1]^fields[2]^fields[3]^
                 fields[4]^fields[5]^0xA5B318C2u;
  if(fields[6]!=hash){corrupted=true;break;}
  if(fields[0]==tick&&fields[1]==actor&&fields[2]==action){
   if(found||fields[3]!=a4||fields[4]!=a5||fields[5]!=b3){
    corrupted=true;break;
   }
   found=true;
  }
 }
 if(ferror(f))corrupted=true;
 long end=ftell(f);
 if(end<0||end%(long)sizeof fields!=0)corrupted=true;
 if(fclose(f)!=0)corrupted=true;
 if(corrupted)return NF_C2_H21_REJECT;
 return found?NF_C2_H21_MATCHED:NF_C2_H21_UNRESOLVED;
}
