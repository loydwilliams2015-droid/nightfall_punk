#define _POSIX_C_SOURCE 200809L
#include "nf_ucw18c1.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <errno.h>
static uint64_t mix(uint64_t h,uint64_t v){return (h^v)*UINT64_C(1099511628211);}
static uint64_t digest(const UcwWorld *w){
 uint64_t h=UINT64_C(14695981039346656037);
 h=mix(h,w->tick);h=mix(h,w->revision);h=mix(h,w->count);
 h=mix(h,(uint64_t)w->material);h=mix(h,(uint64_t)w->contracts);
 for(uint32_t i=0;i<w->count;++i){
  const UcwProposal *a=&w->receipts[i];
  h=mix(h,a->actor);h=mix(h,a->action);h=mix(h,a->object);
  h=mix(h,a->contract);h=mix(h,a->authority_epoch);
  h=mix(h,(uint32_t)a->material_delta);h=mix(h,(uint32_t)a->contract_delta);
 }
 return h;
}
void ucw_init(UcwWorld *w,int64_t m,int64_t c){
 memset(w,0,sizeof *w);w->revision=1;w->material=m;w->contracts=c;
 w->resolution=UCW_RESOLVED;w->history_hash=digest(w);
}
static int ordering(const void *l,const void *r){
 const UcwProposal *a=l,*b=r;
 if(a->priority!=b->priority)return a->priority>b->priority?-1:1;
 if(a->actor!=b->actor)return a->actor<b->actor?-1:1;
 if(a->action!=b->action)return a->action<b->action?-1:1;
 return a->object<b->object?-1:a->object>b->object?1:0;
}
UcwStatus ucw_stage(const UcwWorld *old,const UcwProposal *p,size_t n,UcwCandidate *out){
 if(!old||!out||(n&&!p)||n>UCW_MAX_ACTIONS||old->count>UCW_MAX_ACTIONS||
   old->resolution!=UCW_RESOLVED||old->pending_frontier||old->tick==UINT32_MAX||
   old->revision==UINT32_MAX)return UCW_INVALID;
 out->ready=false;
 UcwProposal sorted[UCW_MAX_ACTIONS];
 if(n)memcpy(sorted,p,n*sizeof *p);
 qsort(sorted,n,sizeof *sorted,ordering);
 UcwWorld next=*old;
 for(size_t i=0;i<n;++i){
  const UcwProposal *a=&sorted[i];
  if(!a->actor||!a->action||!a->object||!a->contract||
     a->read_revision!=old->revision||a->authority_epoch!=old->revision)
    return UCW_STALE;
  for(size_t j=0;j<i;++j)
   if(sorted[j].actor==a->actor&&sorted[j].action==a->action)return UCW_DUPLICATE;
  for(uint32_t j=0;j<old->count;++j)
   if(old->receipts[j].actor==a->actor&&old->receipts[j].action==a->action)
    return UCW_DUPLICATE;
  if(next.count==UCW_MAX_ACTIONS)return UCW_FULL;
  /* A single material object or contractual right cannot be granted twice
   * in the same tick. Disjoint proposals are independent, sorted once. */
  for(size_t j=0;j<i;++j)
   if(sorted[j].object==a->object||sorted[j].contract==a->contract)
    return UCW_CONFLICT;
  if((a->material_delta<0&&next.material<-(int64_t)a->material_delta)||
     (a->contract_delta<0&&next.contracts<-(int64_t)a->contract_delta)||
     (a->material_delta>0&&next.material>INT64_MAX-a->material_delta)||
     (a->contract_delta>0&&next.contracts>INT64_MAX-a->contract_delta))
    return UCW_INVALID;
  next.material+=a->material_delta;next.contracts+=a->contract_delta;
  next.receipts[next.count++]=*a;
 }
 next.tick++;next.revision++;next.history_hash=digest(&next);
 out->staged=next;out->ready=true;return UCW_OK;
}
UcwStatus ucw_frontier_step(UcwWorld *w,unsigned budget){
 if(!w||w->resolution==UCW_QUARANTINED)return UCW_INVALID;
 if(!w->pending_frontier){w->resolution=UCW_RESOLVED;return UCW_OK;}
 unsigned done=budget<w->pending_frontier?budget:w->pending_frontier;
 w->pending_frontier-=done;
 w->resolution=w->pending_frontier?UCW_PENDING:UCW_RESOLVED;
 return w->pending_frontier?UCW_UNCERTAIN:UCW_OK;
}
/* File format: immutable 64-byte logical header followed by one record per
 * complete world. Only valid committed records can be published on replay.
 * This H0 implementation is a portable on-disk *logical* envelope, not a
 * crash proof and not an A5 full-body serialization. */
static void be32(unsigned char *p,uint32_t n){for(int i=3;i>=0;--i){p[i]=n&255u;n>>=8;}}
static uint32_t rd32(const unsigned char *p){return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];}
static void be64(unsigned char *p,uint64_t n){for(int i=7;i>=0;--i){p[i]=n&255u;n>>=8;}}
static uint64_t rd64(const unsigned char *p){uint64_t n=0;for(int i=0;i<8;++i)n=(n<<8)|p[i];return n;}
static void encode(unsigned char b[UCW_DISK_BYTES],const UcwWorld *w){
 memset(b,0,UCW_DISK_BYTES);memcpy(b,"UCW1",4);
 be32(b+4,w->tick);be32(b+8,w->revision);be32(b+12,w->count);
 be64(b+16,(uint64_t)w->material);be64(b+24,(uint64_t)w->contracts);
 be64(b+32,w->history_hash);be32(b+40,w->pending_frontier);
 be32(b+44,(uint32_t)w->resolution);
 uint64_t chk=UINT64_C(14695981039346656037);
 for(int i=0;i<48;++i)chk=mix(chk,b[i]);
 be64(b+48,chk);
}
static bool decode(const unsigned char b[UCW_DISK_BYTES],UcwWorld *w){
 if(memcmp(b,"UCW1",4))return false;
 uint64_t chk=UINT64_C(14695981039346656037);
 for(int i=0;i<48;++i)chk=mix(chk,b[i]);
 if(chk!=rd64(b+48)||rd32(b+12)>UCW_MAX_ACTIONS||
    rd32(b+40)!=0||rd32(b+44)!=UCW_RESOLVED)return false;
 memset(w,0,sizeof *w);
 w->tick=rd32(b+4);w->revision=rd32(b+8);w->count=rd32(b+12);
 w->material=(int64_t)rd64(b+16);w->contracts=(int64_t)rd64(b+24);
 w->history_hash=rd64(b+32);w->resolution=UCW_RESOLVED;
 return true;
}
void ucw_close(UcwJournal *j){if(j&&j->fd>=0){close(j->fd);j->fd=-1;}}
UcwStatus ucw_open(UcwJournal *j,const char *path,int64_t m,int64_t c){
 if(!j||!path)return UCW_INVALID;
 memset(j,0,sizeof *j);j->fd=-1;ucw_init(&j->state,m,c);
 int fd=open(path,O_RDWR|O_CREAT|O_CLOEXEC|O_NOFOLLOW,0600);
 if(fd<0)return UCW_IO;
 if(flock(fd,LOCK_EX|LOCK_NB)){close(fd);return UCW_IO;}
 j->fd=fd;
 struct stat st;if(fstat(fd,&st)||!S_ISREG(st.st_mode)){j->quarantine=true;return UCW_IO;}
 if(st.st_size%UCW_DISK_BYTES){j->quarantine=true;return UCW_UNCERTAIN;}
 for(off_t off=0;off<st.st_size;off+=UCW_DISK_BYTES){
  unsigned char b[UCW_DISK_BYTES];UcwWorld tmp;
  if(pread(fd,b,sizeof b,off)!=(ssize_t)sizeof b||!decode(b,&tmp)||
     tmp.tick!=j->state.tick+1||tmp.revision!=j->state.revision+1){
   j->quarantine=true;return UCW_UNCERTAIN;
  }
  /* H0 logical envelope cannot reconstruct B3 receipt arrays. Therefore
   * keep replay quarantined once receipts are present, not falsely complete. */
  if(tmp.count){j->quarantine=true;return UCW_UNCERTAIN;}
  j->state=tmp;
 }
 return UCW_OK;
}
UcwStatus ucw_commit(UcwJournal *j,const UcwCandidate *c){
 if(!j||j->fd<0||j->quarantine||!c||!c->ready)return UCW_INVALID;
 const UcwWorld *w=&c->staged;
 if(w->tick!=j->state.tick+1||w->revision!=j->state.revision+1||
    w->resolution!=UCW_RESOLVED||w->pending_frontier)return UCW_STALE;
 /* H0 fail closed: receipt-bearing world cannot be durably reconstructed
  * using this incomplete prototype schema. */
 if(w->count)return UCW_INVALID;
 unsigned char b[UCW_DISK_BYTES];encode(b,w);
 off_t off=lseek(j->fd,0,SEEK_END);if(off<0)return UCW_IO;
 if(write(j->fd,b,sizeof b)!=(ssize_t)sizeof b||fdatasync(j->fd)){
  j->quarantine=true;return UCW_UNCERTAIN;
 }
 j->state=*w;return UCW_OK;
}
