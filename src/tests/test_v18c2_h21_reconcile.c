#include "nf_c2_h21_reconcile.h"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
int main(void){
 char path[160];snprintf(path,sizeof path,"/tmp/nightfall_h21_interrupt_%ld.bin",(long)getpid());
 remove(path);
 if(nf_c2_h21_reconcile(path,1,10,77,4,5,1)!=NF_C2_H21_UNRESOLVED)return 1;
 FILE *f=fopen(path,"wb");if(!f)return 2;
 uint32_t v[7]={1,10,77,4,5,1,0};
 v[6]=v[0]^v[1]^v[2]^v[3]^v[4]^v[5]^0xA5B318C2u;
 if(fwrite(v,sizeof v,1,f)!=1||fclose(f)!=0)return 3;
 if(nf_c2_h21_reconcile(path,1,10,77,4,5,1)!=NF_C2_H21_MATCHED)return 4;
 f=fopen(path,"ab");if(!f)return 5;
 if(fwrite(v,1,3,f)!=3||fclose(f)!=0)return 6;
 if(nf_c2_h21_reconcile(path,1,10,77,4,5,1)!=NF_C2_H21_REJECT)return 7;
 remove(path);
 f=fopen(path,"wb");if(!f)return 8;
 if(fwrite(v,sizeof v,1,f)!=1||fwrite(v,sizeof v,1,f)!=1||fclose(f)!=0)return 9;
 if(nf_c2_h21_reconcile(path,1,10,77,4,5,1)!=NF_C2_H21_REJECT)return 10;
 if(remove(path)!=0)return 11;
 puts("H21 missing, complete, torn, and duplicated receipt recovery guards PASS");
 return 0;
}
