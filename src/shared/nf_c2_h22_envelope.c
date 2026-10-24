#define _POSIX_C_SOURCE 200809L
#include "nf_c2_h22_envelope.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
typedef struct {uint32_t magic,version,length,checksum;NfC2H22Receipt receipt;} Header;
static uint32_t digest(const unsigned char *p,size_t n,NfC2H22Receipt r){
 uint32_t h=2166136261u;
 const uint32_t values[]={r.tick,r.actor,r.action,r.a4_receipts,r.a5_samples,r.b3_receipts};
 for(size_t i=0;i<6;i++){h^=values[i];h*=16777619u;}
 for(size_t i=0;i<n;i++){h^=p[i];h*=16777619u;}
 return h;
}
static bool valid(NfC2H22Receipt r,const Nf18a5Integrated *p){
 return p&&r.tick==p->world.tick&&r.actor==p->actor.id&&r.action&&
  r.a4_receipts==p->committed_receipts&&
  r.a5_samples==p->world.history.raw_samples&&r.b3_receipts&&
  r.b3_receipts<=r.a4_receipts;
}
bool nf_c2_h22_save(const char *path,const Nf18a5Integrated *p,NfC2H22Receipt r){
 if(!path||strlen(path)>900||!valid(r,p))return false;
 char phys[1024],temp[1024];
 if(snprintf(phys,sizeof phys,"%s.phystmp",path)>=1024||
    snprintf(temp,sizeof temp,"%s.staging",path)>=1024)return false;
 if(!nf18a5_checkpoint(phys,p)){remove(phys);return false;}
 FILE *in=fopen(phys,"rb");if(!in){remove(phys);return false;}
 if(fseek(in,0,SEEK_END)!=0){fclose(in);remove(phys);return false;}
 long sz=ftell(in);
 if(sz<=0||sz>16*1024*1024L||fseek(in,0,SEEK_SET)!=0){
  fclose(in);remove(phys);return false;
 }
 unsigned char *buf=malloc((size_t)sz);
 bool ok=buf&&fread(buf,1,(size_t)sz,in)==(size_t)sz;
 if(fclose(in)!=0)ok=false;
 remove(phys);
 if(!ok){free(buf);return false;}
 Header h={0};h.magic=0xC222A5B3u;h.version=1;h.length=(uint32_t)sz;
 h.receipt=r;h.checksum=digest(buf,(size_t)sz,r);
 FILE *out=fopen(temp,"wb");
 if(!out){free(buf);return false;}
 ok=fwrite(&h,sizeof h,1,out)==1&&fwrite(buf,1,(size_t)sz,out)==(size_t)sz;
 if(ok)ok=fflush(out)==0&&fsync(fileno(out))==0;
 if(fclose(out)!=0)ok=false;
 free(buf);
 if(ok)ok=rename(temp,path)==0;
 if(!ok)remove(temp);
 return ok;
}
bool nf_c2_h22_restore(const char *path,Nf18a5Integrated *p,NfC2H22Receipt *r){
 if(!path||!p||!r||strlen(path)>900)return false;
 FILE *in=fopen(path,"rb");if(!in)return false;
 Header h={0};bool ok=fread(&h,sizeof h,1,in)==1&&
  h.magic==0xC222A5B3u&&h.version==1&&h.length>0&&h.length<=16*1024*1024u;
 unsigned char *buf=ok?malloc(h.length):NULL;
 if(!buf)ok=false;
 if(ok)ok=fread(buf,1,h.length,in)==h.length&&fgetc(in)==EOF&&!ferror(in);
 if(fclose(in)!=0)ok=false;
 if(ok)ok=digest(buf,h.length,h.receipt)==h.checksum;
 if(!ok){free(buf);return false;}
 char phys[1024];snprintf(phys,sizeof phys,"%s.restoretmp",path);
 FILE *out=fopen(phys,"wb");ok=out!=NULL;
 if(ok)ok=fwrite(buf,1,h.length,out)==h.length&&fflush(out)==0;
 if(out&&fclose(out)!=0)ok=false;
 free(buf);
 Nf18a5Integrated restored={0};
 if(ok)ok=nf18a5_restore(phys,&restored)&&valid(h.receipt,&restored);
 remove(phys);
 if(!ok)return false;
 *p=restored;*r=h.receipt;return true;
}
