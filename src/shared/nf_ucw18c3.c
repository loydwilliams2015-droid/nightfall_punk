#define _POSIX_C_SOURCE 200809L
#include "nf_ucw18c3.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
/* Local ABI-only checkpoint payload in a versioned section envelope.
 * Does not claim canonical portable serialization or independent A6 hook.
 * File frame: magic,generation,length,hash + [kind,len,payload] x2. */
#define HEAD 16u
#define SEC 8u
static uint32_t hash(const unsigned char *p,size_t n){uint32_t h=2166136261u;for(size_t i=0;i<n;i++){h^=p[i];h*=16777619u;}return h;}
static void put(unsigned char *p,uint32_t v){for(int i=3;i>=0;i--){p[i]=(unsigned char)v;v>>=8;}}
static uint32_t get(const unsigned char *p){return (uint32_t)p[0]<<24|(uint32_t)p[1]<<16|(uint32_t)p[2]<<8|p[3];}
static size_t length(void){return HEAD+2u*SEC+sizeof(Nf18a5Integrated)+sizeof(UcwWorld);}
static bool valid(const Ucw3Snapshot *s){
 return s&&s->contracts.resolution==UCW_RESOLVED&&!s->contracts.pending_frontier&&
 s->contracts.count<=UCW_MAX_ACTIONS&&s->contracts.tick==s->physical.world.tick&&
 s->physical.world.revision!=0&&s->contracts.revision!=0;
}
static void pack(unsigned char *b,const Ucw3Snapshot *s,uint32_t generation){
 size_t n=length();memset(b,0,n);memcpy(b,"U3W1",4);put(b+4,generation);put(b+8,(uint32_t)n);
 size_t at=HEAD;put(b+at,UCW3_SECTION_A5);put(b+at+4,(uint32_t)sizeof s->physical);at+=SEC;
 memcpy(b+at,&s->physical,sizeof s->physical);at+=sizeof s->physical;
 put(b+at,UCW3_SECTION_CONTRACTS);put(b+at+4,(uint32_t)sizeof s->contracts);at+=SEC;
 memcpy(b+at,&s->contracts,sizeof s->contracts);
 put(b+12,hash(b+HEAD,n-HEAD));
}
static bool unpack(const unsigned char *b,size_t n,Ucw3Snapshot *out,uint32_t *generation){
 if(n!=length()||memcmp(b,"U3W1",4)||get(b+8)!=n||
    get(b+12)!=hash(b+HEAD,n-HEAD))return false;
 size_t at=HEAD;
 if(get(b+at)!=UCW3_SECTION_A5||get(b+at+4)!=sizeof out->physical)return false;
 at+=SEC;memcpy(&out->physical,b+at,sizeof out->physical);at+=sizeof out->physical;
 if(get(b+at)!=UCW3_SECTION_CONTRACTS||get(b+at+4)!=sizeof out->contracts)return false;
 at+=SEC;memcpy(&out->contracts,b+at,sizeof out->contracts);
 *generation=get(b+4);return valid(out);
}
void ucw3_close(Ucw3Journal *j){if(j&&j->fd>=0){close(j->fd);j->fd=-1;}}
UcwStatus ucw3_open(Ucw3Journal *j,const char *path,const Ucw3Snapshot *genesis){
 if(!j||!path||!valid(genesis)||length()>UCW3_MAX_PAYLOAD)return UCW_INVALID;
 memset(j,0,sizeof *j);j->fd=-1;
 int fd=open(path,O_RDWR|O_CREAT|O_CLOEXEC|O_NOFOLLOW,0600);
 if(fd<0)return UCW_IO;
 if(flock(fd,LOCK_EX|LOCK_NB)){close(fd);return UCW_IO;}
 j->fd=fd;j->physical=genesis->physical;j->contracts=genesis->contracts;
 struct stat st;if(fstat(fd,&st)||!S_ISREG(st.st_mode)||st.st_size<0){j->quarantine=true;return UCW_IO;}
 size_t n=length();
 if((uint64_t)st.st_size%n){j->quarantine=true;return UCW_UNCERTAIN;}
 unsigned char *b=malloc(n);if(!b){j->quarantine=true;return UCW_IO;}
 for(off_t pos=0;pos<st.st_size;pos+=(off_t)n){
  Ucw3Snapshot next;uint32_t generation=0;
  if(pread(fd,b,n,pos)!=(ssize_t)n||!unpack(b,n,&next,&generation)||
    generation!=j->generation+1||next.contracts.tick!=j->contracts.tick+1||
    next.contracts.revision!=j->contracts.revision+1||
    next.physical.world.revision<=j->physical.world.revision){
   free(b);j->quarantine=true;return UCW_UNCERTAIN;
  }
  j->physical=next.physical;j->contracts=next.contracts;j->generation=generation;
 }
 free(b);return UCW_OK;
}
UcwStatus ucw3_commit(Ucw3Journal *j,const Ucw3Snapshot *candidate){
 if(!j||j->fd<0||j->quarantine||!valid(candidate))return UCW_INVALID;
 if(j->generation==UINT32_MAX||candidate->contracts.tick!=j->contracts.tick+1||
    candidate->contracts.revision!=j->contracts.revision+1||
    candidate->physical.world.revision<=j->physical.world.revision)return UCW_STALE;
 size_t n=length();unsigned char *b=malloc(n);if(!b)return UCW_IO;
 pack(b,candidate,j->generation+1);
 off_t off=lseek(j->fd,0,SEEK_END);if(off<0){free(b);j->quarantine=true;return UCW_IO;}
 size_t written=0;while(written<n){
  ssize_t k=pwrite(j->fd,b+written,n-written,off+(off_t)written);
  if(k<=0){free(b);j->quarantine=true;return UCW_UNCERTAIN;}
  written+=(size_t)k;
 }
 free(b);
 if(fdatasync(j->fd)){j->quarantine=true;return UCW_UNCERTAIN;}
 j->physical=candidate->physical;j->contracts=candidate->contracts;j->generation++;
 return UCW_OK;
}
