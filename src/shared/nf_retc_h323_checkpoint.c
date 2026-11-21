/* SPDX-License-Identifier: GPL-3.0-or-later */
#define _POSIX_C_SOURCE 200809L
#include "nf_retc_h323_checkpoint.h"
#include "nf_retc_h323_journal.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define NF_H323_MANIFEST_BYTES 40u
#define NF_H323_MANIFEST_MAGIC 0x334b5043u
static void put32(uint8_t *p,uint32_t v){
 p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);p[2]=(uint8_t)(v>>16);p[3]=(uint8_t)(v>>24);
}
static uint32_t get32(const uint8_t *p){
 return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
static uint32_t hash(const uint8_t *p,size_t n){
 uint32_t h=2166136261u;
 for(size_t i=0;i<n;i++){h^=p[i];h*=16777619u;}return h;
}
static int paths(const char *base,uint32_t generation,char *snap,char *man){
 if(!base||strlen(base)>750||generation==0||generation>1000000u)return 0;
 int a=snprintf(snap,1024,"%s.snap.%u",base,generation);
 int b=snprintf(man,1024,"%s.manifest.%u",base,generation);
 return a>0&&a<1024&&b>0&&b<1024;
}
static int write_all(int fd,const uint8_t *p,size_t n){
 while(n){
  ssize_t k=write(fd,p,n);
  if(k<0&&errno==EINTR)continue;
  if(k<=0)return 0;
  p+=(size_t)k;n-=(size_t)k;
 }
 return 1;
}
static int read_all(int fd,uint8_t *p,size_t n){
 while(n){
  ssize_t k=read(fd,p,n);
  if(k<0&&errno==EINTR)continue;
  if(k<=0)return 0;
  p+=(size_t)k;n-=(size_t)k;
 }
 return 1;
}
static int directory_sync(const char *path){
 size_t n=strlen(path);if(n==0||n>900)return 0;
 char dir[1024];memcpy(dir,path,n+1u);
 char *slash=strrchr(dir,'/');
 if(!slash)strcpy(dir,".");
 else if(slash==dir)slash[1]='\0';
 else *slash='\0';
 int fd=open(dir,O_RDONLY);
 if(fd<0)return 0;
 int ok=fsync(fd)==0;
 if(close(fd)!=0)ok=0;
 return ok;
}
static void cut(const char *name){
#ifdef NF_H323_TEST_FAULTS
 const char *at=getenv("NF_H323_CHECKPOINT_KILL");
 if(at&&strcmp(at,name)==0)_exit(93);
#else
 (void)name;
#endif
}
static void encode_manifest(uint8_t m[NF_H323_MANIFEST_BYTES],
                            uint32_t gen,const NfH323Record *r,
                            const uint8_t canon[NF_H323_BYTES]){
 memset(m,0,NF_H323_MANIFEST_BYTES);
 put32(m,NF_H323_MANIFEST_MAGIC);put32(m+4,1u);put32(m+8,gen);
 put32(m+12,r->txn_low);put32(m+16,r->txn_high);
 put32(m+20,r->prior_tick);put32(m+24,r->next_tick);
 put32(m+28,hash(canon,NF_H323_BYTES));
 put32(m+32,r->material_epoch);
 put32(m+36,hash(m,36));
}
int nf_h323_checkpoint_create(const char *base,uint32_t gen,
                             const NfH323Record *r){
 char snap[1024],manifest[1024],tmp[1024];
 uint8_t canon[NF_H323_BYTES],m[NF_H323_MANIFEST_BYTES];
 if(!paths(base,gen,snap,manifest)||!nf_h323_encode(r,canon))return 0;
 /* A preexisting manifest is immutable. An orphan snapshot from a crash is
  * not automatically adopted: a separate audited cleanup/reconciliation
  * protocol is required before retrying that generation. */
 if(access(manifest,F_OK)==0)return 0;
 if(!nf_h323_journal_write(snap,r))return 0;
 cut("after_snapshot");
 encode_manifest(m,gen,r,canon);
 int n=snprintf(tmp,sizeof tmp,"%s.tmp.%ld",manifest,(long)getpid());
 if(n<=0||(size_t)n>=sizeof tmp)return 0;
 int fd=open(tmp,O_WRONLY|O_CREAT|O_EXCL,0600);
 if(fd<0)return 0;
 int ok=write_all(fd,m,sizeof m);
 if(ok)cut("after_manifest_write");
 if(ok)ok=fsync(fd)==0;
 if(ok)cut("after_manifest_sync");
 if(close(fd)!=0)ok=0;
 if(ok)ok=link(tmp,manifest)==0; /* atomic no-replace */
 if(ok)cut("after_manifest_link");
 if(unlink(tmp)!=0)ok=0;
 if(ok)ok=directory_sync(manifest);
 if(ok)cut("after_dirsync");
 /* Failure after link can leave visible-but-unacknowledged manifest. */
 return ok;
}
int nf_h323_checkpoint_read(const char *base,uint32_t gen,NfH323Record *out){
 char snap[1024],manifest[1024];
 uint8_t m[NF_H323_MANIFEST_BYTES],canon[NF_H323_BYTES];
 if(!out||!paths(base,gen,snap,manifest))return 0;
 int fd=open(manifest,O_RDONLY);
 if(fd<0)return 0;
 struct stat st;
 int ok=fstat(fd,&st)==0&&S_ISREG(st.st_mode)&&st.st_size==NF_H323_MANIFEST_BYTES;
 if(ok)ok=read_all(fd,m,sizeof m);
 if(close(fd)!=0)ok=0;
 if(!ok||get32(m)!=NF_H323_MANIFEST_MAGIC||get32(m+4)!=1u||
    get32(m+8)!=gen||get32(m+36)!=hash(m,36))return 0;
 NfH323Record candidate={0};
 if(!nf_h323_journal_read(snap,&candidate)||
    !nf_h323_encode(&candidate,canon))return 0;
 if(get32(m+12)!=candidate.txn_low||get32(m+16)!=candidate.txn_high||
    get32(m+20)!=candidate.prior_tick||get32(m+24)!=candidate.next_tick||
    get32(m+28)!=hash(canon,NF_H323_BYTES)||
    get32(m+32)!=candidate.material_epoch)return 0;
 *out=candidate;return 1;
}
