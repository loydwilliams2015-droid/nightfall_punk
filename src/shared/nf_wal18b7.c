#define _POSIX_C_SOURCE 200809L
#include "nf_wal18b7.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define NF4_MAGIC UINT32_C(0x3446574e)
#define NF4_VERSION UINT32_C(1)
static void put32(uint8_t *p,uint32_t x){
 for(unsigned j=0;j<4;++j)p[j]=(uint8_t)(x>>(8u*j));
}
static uint32_t get32(const uint8_t *p){
 uint32_t x=0;for(unsigned j=0;j<4;++j)x|=(uint32_t)p[j]<<(8u*j);return x;
}
static void put64(uint8_t *p,uint64_t x){
 for(unsigned j=0;j<8;++j)p[j]=(uint8_t)(x>>(8u*j));
}
static uint64_t get64(const uint8_t *p){
 uint64_t x=0;for(unsigned j=0;j<8;++j)x|=(uint64_t)p[j]<<(8u*j);return x;
}
static uint64_t checksum(const uint8_t *p){
 uint64_t h=UINT64_C(14695981039346656037);
 for(unsigned i=0;i<56;++i){h^=p[i];h*=UINT64_C(1099511628211);}
 return h;
}
static bool read_exact(int fd,uint8_t *p,size_t size,off_t offset){
 size_t done=0;
 while(done<size){
  ssize_t r=pread(fd,p+done,size-done,offset+(off_t)done);
  if(r<0&&errno==EINTR)continue;
  if(r<=0)return false;
  done+=(size_t)r;
 }
 return true;
}
static bool write_exact(int fd,const uint8_t *p,size_t size,off_t offset){
 size_t done=0;
 while(done<size){
  ssize_t r=pwrite(fd,p+done,size-done,offset+(off_t)done);
  if(r<0&&errno==EINTR)continue;
  if(r<=0)return false;
  done+=(size_t)r;
 }
 return true;
}
static bool parent_sync(const char *path){
 char parent[4096];
 size_t len=strlen(path),slash=len;
 while(slash&&path[slash-1]!='/')slash--;
 if(!slash){parent[0]='.';parent[1]=0;}
 else if(slash==1){parent[0]='/';parent[1]=0;}
 else{
  if(slash>=sizeof parent)return false;
  memcpy(parent,path,slash-1);parent[slash-1]=0;
 }
 int d=open(parent,O_RDONLY|O_DIRECTORY);
 if(d<0)return false;
 int rc=fsync(d),prior_errno=errno;close(d);errno=prior_errno;
 return rc==0;
}
static void encode(uint8_t out[NF4_RECORD_BYTES],Nf4Ledger state,Nf4Event e){
 memset(out,0,NF4_RECORD_BYTES);
 put32(out,NF4_MAGIC);put32(out+4,NF4_VERSION);
 put32(out+8,state.epoch);put32(out+12,state.epoch+1);
 put32(out+16,e.actor);put32(out+20,e.action);
 put32(out+24,(uint32_t)e.debit);
 put32(out+28,0);
 put64(out+32,(uint64_t)(state.balance-e.debit));
 put64(out+40,(uint64_t)state.balance);
 put32(out+48,state.count);put32(out+52,~state.count);
 put64(out+56,checksum(out));
}
static bool replay(Nf4Ledger *s,const uint8_t data[NF4_RECORD_BYTES]){
 if(get32(data)!=NF4_MAGIC||get32(data+4)!=NF4_VERSION||
    checksum(data)!=get64(data+56)||get32(data+28)!=0||
    get32(data+8)!=s->epoch||s->epoch==UINT32_MAX||
    get32(data+12)!=s->epoch+1||get32(data+48)!=s->count||
    get32(data+52)!=~s->count||s->count>=NF4_MAX_RECORDS)return false;
 uint32_t actor=get32(data+16),action=get32(data+20),debit=get32(data+24);
 uint64_t prior=get64(data+40),after=get64(data+32);
 if(!actor||!action||debit>INT32_MAX||prior>INT64_MAX||after>INT64_MAX||
    prior!=(uint64_t)s->balance||debit>prior||
    after!=prior-debit)return false;
 for(uint32_t j=0;j<s->count;++j)if(s->receipts[j].actor==actor&&
    s->receipts[j].action==action)return false;
 s->receipts[s->count++]=(Nf4Event){actor,action,s->epoch,(int32_t)debit};
 s->epoch++;s->balance=(int64_t)after;
 return true;
}
void nf4_close(Nf4Wal *w){if(!w)return;if(w->fd>=0)close(w->fd);w->fd=-1;w->quarantine=true;}
Nf4Status nf4_open(Nf4Wal *w,const char *path,int64_t initial_balance){
 if(!w)return NF4_INVALID;
 memset(w,0,sizeof *w);w->fd=-1;w->quarantine=true;
 if(!path||!*path||initial_balance<0)return NF4_INVALID;
 int fd=open(path,O_RDWR|O_CREAT|O_EXCL,0600);
 bool created=fd>=0;
 if(fd<0&&errno==EEXIST)fd=open(path,O_RDWR);
 if(fd<0)return NF4_IO_ERROR;
 w->fd=fd;
 if(flock(fd,LOCK_EX|LOCK_NB)!=0)return NF4_IO_ERROR;
 if(created&&!parent_sync(path))return NF4_IO_ERROR;
 struct stat st;if(fstat(fd,&st)!=0||!S_ISREG(st.st_mode)||st.st_size<0)
  return NF4_IO_ERROR;
 w->ledger.epoch=1;w->ledger.balance=initial_balance;
 if(st.st_size%(off_t)NF4_RECORD_BYTES!=0||
    st.st_size>(off_t)(NF4_MAX_RECORDS*NF4_RECORD_BYTES)){
  /* Keep the known good prefix for diagnostics; do not truncate automatically. */
  off_t n=st.st_size/(off_t)NF4_RECORD_BYTES;
  if(n>NF4_MAX_RECORDS)n=NF4_MAX_RECORDS;
  for(off_t i=0;i<n;++i){
   uint8_t rec[NF4_RECORD_BYTES];
   if(!read_exact(fd,rec,sizeof rec,i*(off_t)NF4_RECORD_BYTES)||
      !replay(&w->ledger,rec))break;
  }
  return NF4_QUARANTINED;
 }
 for(off_t i=0;i<st.st_size/(off_t)NF4_RECORD_BYTES;++i){
  uint8_t rec[NF4_RECORD_BYTES];
  if(!read_exact(fd,rec,sizeof rec,i*(off_t)NF4_RECORD_BYTES)||
     !replay(&w->ledger,rec))return NF4_QUARANTINED;
 }
 w->quarantine=false;
 return NF4_APPLIED;
}
Nf4Status nf4_apply(Nf4Wal *w,Nf4Event e){
 if(!w||w->fd<0)return NF4_INVALID;
 if(w->quarantine)return NF4_QUARANTINED;
 if(!e.actor||!e.action||e.debit<0)return NF4_INVALID;
 for(uint32_t i=0;i<w->ledger.count;++i){
  Nf4Event old=w->ledger.receipts[i];
  if(old.actor==e.actor&&old.action==e.action)
   return old.epoch==e.epoch&&old.debit==e.debit?NF4_DUPLICATE:NF4_INVALID;
 }
 if(e.epoch!=w->ledger.epoch)return NF4_STALE;
 if(w->ledger.epoch==UINT32_MAX||w->ledger.count==NF4_MAX_RECORDS)return NF4_FULL;
 if(w->ledger.balance<e.debit)return NF4_INVALID;
 struct stat st;
 if(fstat(w->fd,&st)!=0){w->quarantine=true;return NF4_UNCERTAIN;}
 if(st.st_size!=(off_t)(w->ledger.count*NF4_RECORD_BYTES)){
  w->quarantine=true;return NF4_QUARANTINED;
 }
 uint8_t rec[NF4_RECORD_BYTES];encode(rec,w->ledger,e);
 if(!write_exact(w->fd,rec,sizeof rec,st.st_size)){
  w->quarantine=true;return NF4_UNCERTAIN;
 }
#ifdef NF4_TESTING
 if(w->fault_after_write){w->quarantine=true;return NF4_UNCERTAIN;}
#endif
 if(fdatasync(w->fd)!=0){w->quarantine=true;return NF4_UNCERTAIN;}
 if(!replay(&w->ledger,rec)){w->quarantine=true;return NF4_QUARANTINED;}
 return NF4_APPLIED;
}
