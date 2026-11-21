/* SPDX-License-Identifier: GPL-3.0-or-later */
#define _POSIX_C_SOURCE 200809L
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
#define PACKET_BYTES (16u+NF_H323_BYTES)
#define PACKET_MAGIC 0x4c415748u
#define PHASE_PREPARE 1u
#define PHASE_COMMIT 2u
static void le32(uint8_t *p,uint32_t v) {
 p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);p[2]=(uint8_t)(v>>16);p[3]=(uint8_t)(v>>24);
}
static uint32_t read32(const uint8_t *p) {
 return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
static uint32_t hash(const uint8_t *p,size_t n){
 uint32_t h=2166136261u;for(size_t i=0;i<n;i++){h^=p[i];h*=16777619u;}return h;
}
static void packet(uint8_t out[PACKET_BYTES],uint32_t phase,const uint8_t data[NF_H323_BYTES]){
 memset(out,0,PACKET_BYTES);
 le32(out,PACKET_MAGIC);le32(out+4,phase);le32(out+8,NF_H323_BYTES);
 memcpy(out+16,data,NF_H323_BYTES);
 /* Digest includes header and payload, excludes digest word itself. */
 uint32_t h=hash(out,12);
 h^=hash(out+16,NF_H323_BYTES);h*=16777619u;
 le32(out+12,h);
}
static int exact_write(int fd,const uint8_t *p,size_t n){
 while(n){ssize_t k=write(fd,p,n);if(k<0&&errno==EINTR)continue;
   if(k<=0)return 0;
   p+=(size_t)k;
   n-=(size_t)k;}return 1;
}
static int exact_read(int fd,uint8_t *p,size_t n){
 while(n){ssize_t k=read(fd,p,n);if(k<0&&errno==EINTR)continue;
   if(k<=0)return 0;
   p+=(size_t)k;
   n-=(size_t)k;}return 1;
}
static int parent_sync(const char *path){
 size_t n=strlen(path);if(n==0||n>900)return 0;
 char dir[1024];memcpy(dir,path,n+1);
 char *slash=strrchr(dir,'/');
 if(!slash)strcpy(dir,".");
 else if(slash==dir)slash[1]='\0';
 else *slash='\0';
 int fd=open(dir,O_RDONLY);if(fd<0)return 0;
 int ok=fsync(fd)==0;if(close(fd)!=0)ok=0;return ok;
}
static int injected_error(const char *stage){
#ifdef NF_H323_TEST_FAULTS
 const char *requested=getenv("NF_H323_IO_FAIL");
 return requested && strcmp(requested,stage)==0;
#else
 (void)stage;
 return 0;
#endif
}
static void fault(const char *stage){
#ifdef NF_H323_TEST_FAULTS
 const char *cut=getenv("NF_H323_KILL");
 if(cut&&strcmp(cut,stage)==0)_exit(93);
#else
 (void)stage;
#endif
}
int nf_h323_journal_write(const char *path,const NfH323Record *r){
 uint8_t payload[NF_H323_BYTES],rec[PACKET_BYTES];
 if(!path||strlen(path)>850||!nf_h323_encode(r,payload))return 0;
 /* Atomic destination creation below rejects all competing writers. */
 char tmp[1024];int x=snprintf(tmp,sizeof tmp,"%s.h323tmp.%ld",path,(long)getpid());
 if(x<0||(size_t)x>=sizeof tmp)return 0;
 fault("before_open");
 int fd=open(tmp,O_WRONLY|O_CREAT|O_EXCL,0600);
 if(fd<0)return 0;
 int ok=1,published=0;
 packet(rec,PHASE_PREPARE,payload);
 if(ok)ok=exact_write(fd,rec,sizeof rec);
 if(ok)fault("after_prepare_write");
 if(ok)ok=!injected_error("prepare_sync") && fsync(fd)==0;
 if(ok)fault("after_prepare_sync");
 packet(rec,PHASE_COMMIT,payload);
 if(ok)ok=exact_write(fd,rec,sizeof rec);
 if(ok)fault("after_commit_write");
 if(ok)ok=!injected_error("commit_sync") && fsync(fd)==0;
 if(ok)fault("after_commit_sync");
 if(close(fd)!=0)ok=0;
 if(ok)fault("before_link");
 /* Hard-link creation is atomic NO-REPLACE on the same filesystem; rename()
  * would silently overwrite a competing writer's accepted transaction. */
 if(ok)ok=!injected_error("publish") && link(tmp,path)==0;
 if(ok){published=1;fault("after_link");}
 /* Always remove the private staging name in a non-crashing writer. */
 if(unlink(tmp)!=0)ok=0;
 if(ok)ok=!injected_error("dirsync") && parent_sync(path);
 if(ok)fault("after_dirsync");
 (void)published;
 /* If dir-sync failed, the file may be visible but NOT durably acknowledged. */
 return ok;
}
int nf_h323_journal_read(const char *path,NfH323Record *out){
 if(!path||!out)return 0;
 int fd=open(path,O_RDONLY);if(fd<0)return 0;
 struct stat st;int ok=fstat(fd,&st)==0&&S_ISREG(st.st_mode)&&
   st.st_size==(off_t)(2u*PACKET_BYTES);
 uint8_t a[PACKET_BYTES],b[PACKET_BYTES];
 if(ok)ok=exact_read(fd,a,sizeof a)&&exact_read(fd,b,sizeof b);
 if(close(fd)!=0)ok=0;
 if(!ok||memcmp(a+16,b+16,NF_H323_BYTES)!=0)return 0;
 const uint8_t *p[2]={a,b};
 for(uint32_t i=0;i<2;i++){
  if(read32(p[i])!=PACKET_MAGIC||read32(p[i]+4)!=i+1u||
     read32(p[i]+8)!=NF_H323_BYTES)return 0;
  uint32_t h=hash(p[i],12);
  h^=hash(p[i]+16,NF_H323_BYTES);h*=16777619u;
  if(read32(p[i]+12)!=h)return 0;
 }
 NfH323Record candidate={0};
 if(!nf_h323_decode(a+16,NF_H323_BYTES,&candidate))return 0;
 *out=candidate;
 return 1;
}
