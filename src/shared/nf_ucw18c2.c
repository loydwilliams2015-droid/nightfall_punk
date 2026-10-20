#define _POSIX_C_SOURCE 200809L
#include "nf_ucw18c2.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <limits.h>
static uint64_t mix2(uint64_t h,uint64_t v){return (h^v)*UINT64_C(1099511628211);}
static void put32(unsigned char *p,uint32_t n){for(int k=3;k>=0;--k){p[k]=(unsigned char)n;n>>=8;}}
static uint32_t get32(const unsigned char *p){return (uint32_t)p[0]<<24|(uint32_t)p[1]<<16|(uint32_t)p[2]<<8|p[3];}
static void put64(unsigned char *p,uint64_t n){for(int k=7;k>=0;--k){p[k]=(unsigned char)n;n>>=8;}}
static uint64_t get64(const unsigned char *p){uint64_t v=0;for(int k=0;k<8;++k)v=(v<<8)|p[k];return v;}
static uint64_t hash_bytes(const unsigned char *p,size_t n){uint64_t h=UINT64_C(14695981039346656037);for(size_t k=0;k<n;++k)h=mix2(h,p[k]);return h;}
static void pack(unsigned char b[UCW2_RECORD_SIZE],const UcwWorld *w,uint64_t previous){
 memset(b,0,UCW2_RECORD_SIZE);
 memcpy(b,"UCW2",4);put32(b+4,2);put32(b+8,w->tick);put32(b+12,w->revision);
 put32(b+16,w->count);put32(b+20,w->pending_frontier);
 put32(b+24,(uint32_t)w->resolution);
 put64(b+28,(uint64_t)w->material);put64(b+36,(uint64_t)w->contracts);
 put64(b+44,w->history_hash);put64(b+52,previous);
 for(uint32_t i=0;i<w->count;++i){
  const UcwProposal *p=&w->receipts[i];unsigned char *v=b+64u+36u*i;
  put32(v,p->actor);put32(v+4,p->action);put32(v+8,p->object);
  put32(v+12,p->contract);put32(v+16,p->authority_epoch);
  put32(v+20,(uint32_t)p->material_delta);put32(v+24,(uint32_t)p->contract_delta);
  put32(v+28,p->read_revision);put32(v+32,p->priority);
 }
 put32(b+60,(uint32_t)hash_bytes(b,60));
 /* Store full-record checksum in the last 8 bytes of the fixed envelope.
    All proposal slots are included, including zeros after count. */
 put64(b+UCW2_RECORD_SIZE-8,hash_bytes(b,UCW2_RECORD_SIZE-8));
}
static bool unpack(const unsigned char b[UCW2_RECORD_SIZE],UcwWorld *w,uint64_t *previous){
 if(memcmp(b,"UCW2",4)||get32(b+4)!=2||
    get32(b+16)>UCW_MAX_ACTIONS||get32(b+20)||
    get32(b+24)!=UCW_RESOLVED||get32(b+60)!=(uint32_t)hash_bytes(b,60)||
    get64(b+UCW2_RECORD_SIZE-8)!=hash_bytes(b,UCW2_RECORD_SIZE-8))
    return false;
 memset(w,0,sizeof *w);w->tick=get32(b+8);w->revision=get32(b+12);
 w->count=get32(b+16);w->material=(int64_t)get64(b+28);
 w->contracts=(int64_t)get64(b+36);w->history_hash=get64(b+44);
 w->resolution=UCW_RESOLVED;*previous=get64(b+52);
 for(uint32_t i=0;i<w->count;++i){
  const unsigned char *v=b+64u+36u*i;UcwProposal *p=&w->receipts[i];
  p->actor=get32(v);p->action=get32(v+4);p->object=get32(v+8);
  p->contract=get32(v+12);p->authority_epoch=get32(v+16);
  p->material_delta=(int32_t)get32(v+20);p->contract_delta=(int32_t)get32(v+24);
  p->read_revision=get32(v+28);p->priority=get32(v+32);
 }
 return true;
}
static bool equal_world(const UcwWorld *a,const UcwWorld *b){
 if(a->tick!=b->tick||a->revision!=b->revision||a->count!=b->count||
    a->material!=b->material||a->contracts!=b->contracts||
    a->history_hash!=b->history_hash||a->pending_frontier!=b->pending_frontier||
    a->resolution!=b->resolution)return false;
 for(uint32_t i=0;i<a->count;++i){
  const UcwProposal *x=&a->receipts[i],*y=&b->receipts[i];
  if(x->actor!=y->actor||x->action!=y->action||x->object!=y->object||
     x->contract!=y->contract||x->authority_epoch!=y->authority_epoch||
     x->material_delta!=y->material_delta||x->contract_delta!=y->contract_delta||
     x->read_revision!=y->read_revision||x->priority!=y->priority)return false;
 }
 return true;
}
static bool validate_transition(const UcwWorld *previous,const UcwWorld *next){
 if(next->count<previous->count||next->count>UCW_MAX_ACTIONS)return false;
 UcwCandidate candidate={0};
 size_t added=next->count-previous->count;
 UcwStatus status=ucw_stage(previous,next->receipts+previous->count,added,&candidate);
 return status==UCW_OK&&candidate.ready&&equal_world(&candidate.staged,next);
}
void ucw2_close(Ucw2Journal *j){if(j&&j->fd>=0){close(j->fd);j->fd=-1;}}
UcwStatus ucw2_open(Ucw2Journal *j,const char *path,int64_t material,int64_t contracts){
 if(!j||!path||material<0||contracts<0)return UCW_INVALID;
 memset(j,0,sizeof *j);j->fd=-1;ucw_init(&j->state,material,contracts);
 int fd=open(path,O_RDWR|O_CREAT|O_CLOEXEC|O_NOFOLLOW,0600);
 if(fd<0)return UCW_IO;
 if(flock(fd,LOCK_EX|LOCK_NB)){close(fd);return UCW_IO;}
 j->fd=fd;struct stat st;
 if(fstat(fd,&st)||!S_ISREG(st.st_mode)||st.st_size<0){j->quarantine=true;return UCW_IO;}
 if(st.st_size%UCW2_RECORD_SIZE){j->quarantine=true;return UCW_UNCERTAIN;}
 uint64_t last= j->state.history_hash;
 for(off_t at=0;at<st.st_size;at+=UCW2_RECORD_SIZE){
  unsigned char b[UCW2_RECORD_SIZE];UcwWorld decoded;uint64_t prev=0;
  if(pread(fd,b,sizeof b,at)!=(ssize_t)sizeof b||!unpack(b,&decoded,&prev)||
     prev!=last||!validate_transition(&j->state,&decoded)){
   j->quarantine=true;return UCW_UNCERTAIN;
  }
  j->state=decoded;last=decoded.history_hash;
 }
 return UCW_OK;
}
UcwStatus ucw2_commit(Ucw2Journal *j,const UcwCandidate *candidate){
 if(!j||j->fd<0||j->quarantine||!candidate||!candidate->ready)return UCW_INVALID;
 const UcwWorld *w=&candidate->staged;
 if(!validate_transition(&j->state,w))return UCW_STALE;
 unsigned char b[UCW2_RECORD_SIZE];pack(b,w,j->state.history_hash);
 off_t off=lseek(j->fd,0,SEEK_END);if(off<0){j->quarantine=true;return UCW_IO;}
 size_t n=0;while(n<sizeof b){
  ssize_t wrote=pwrite(j->fd,b+n,sizeof b-n,off+(off_t)n);
  if(wrote<=0){j->quarantine=true;return UCW_UNCERTAIN;}
  n+=(size_t)wrote;
 }
 if(fdatasync(j->fd)){j->quarantine=true;return UCW_UNCERTAIN;}
 j->state=*w;return UCW_OK;
}
