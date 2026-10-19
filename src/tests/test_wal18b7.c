#define _POSIX_C_SOURCE 200809L
#include "nf_wal18b7.h"
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
static unsigned long long checks=0;
#define CK(c) do{if(!(c)){fprintf(stderr,"FAIL:%d: %s\n",__LINE__,#c);return false;}++checks;}while(0)
static int pathnew(char *path,size_t size){
 const char *base="/tmp/nf4wal_XXXXXX";
 if(strlen(base)+1>size)return -1;
 strcpy(path,base);
 int fd=mkstemp(path);
 if(fd<0)return -1;
 close(fd);return 0;
}
static Nf4Event event(uint32_t epoch,uint32_t actor,uint32_t action,int32_t debit){
 Nf4Event e={actor,action,epoch,debit};return e;
}
static bool basics(void){
 char path[64];CK(pathnew(path,sizeof path)==0);
 Nf4Wal w;CK(nf4_open(&w,path,100)==NF4_APPLIED);
 CK(nf4_apply(&w,event(1,7,1,30))==NF4_APPLIED);
 CK(w.ledger.balance==70&&w.ledger.epoch==2&&w.ledger.count==1);
 CK(nf4_apply(&w,event(1,7,1,30))==NF4_DUPLICATE);
 CK(nf4_apply(&w,event(1,7,1,31))==NF4_INVALID);
 CK(nf4_apply(&w,event(1,7,2,3))==NF4_STALE);
 CK(nf4_apply(&w,event(2,7,2,0))==NF4_APPLIED);
 CK(w.ledger.balance==70&&w.ledger.count==2);
 nf4_close(&w);
 CK(nf4_open(&w,path,100)==NF4_APPLIED);
 CK(w.ledger.balance==70&&w.ledger.epoch==3&&w.ledger.count==2);
 CK(nf4_apply(&w,event(1,7,1,30))==NF4_DUPLICATE);
 CK(nf4_apply(&w,event(3,8,1,71))==NF4_INVALID);
 nf4_close(&w);unlink(path);return true;
}
static bool uncertain(void){
 char path[64];CK(pathnew(path,sizeof path)==0);
 Nf4Wal w;CK(nf4_open(&w,path,100)==NF4_APPLIED);
#ifdef NF4_TESTING
 w.fault_after_write=true;
 CK(nf4_apply(&w,event(1,9,3,10))==NF4_UNCERTAIN);
 CK(w.ledger.count==0&&w.ledger.balance==100);
 CK(nf4_apply(&w,event(1,9,3,10))==NF4_QUARANTINED);
 nf4_close(&w);
 /* A complete record can survive lost ACK/close; replay must prevent debit twice.
    This simulated process-level error is not a power-loss durability proof. */
 CK(nf4_open(&w,path,100)==NF4_APPLIED);
 CK(w.ledger.count==1&&w.ledger.balance==90);
 CK(nf4_apply(&w,event(1,9,3,10))==NF4_DUPLICATE);
#endif
 nf4_close(&w);unlink(path);return true;
}
static bool corrupt_and_torn(void){
 char path[64];CK(pathnew(path,sizeof path)==0);
 Nf4Wal w;CK(nf4_open(&w,path,40)==NF4_APPLIED);
 CK(nf4_apply(&w,event(1,1,1,5))==NF4_APPLIED);nf4_close(&w);
 int fd=open(path,O_WRONLY|O_APPEND);CK(fd>=0);
 CK(write(fd,"torn!",5)==5);close(fd);
 CK(nf4_open(&w,path,40)==NF4_QUARANTINED);
 CK(w.ledger.count==1&&w.ledger.balance==35);
 CK(nf4_apply(&w,event(2,2,2,1))==NF4_QUARANTINED);
 nf4_close(&w);unlink(path);
 CK(pathnew(path,sizeof path)==0);
 CK(nf4_open(&w,path,40)==NF4_APPLIED);
 CK(nf4_apply(&w,event(1,1,1,5))==NF4_APPLIED);nf4_close(&w);
 fd=open(path,O_RDWR);CK(fd>=0);unsigned char b=0x77;
 CK(pwrite(fd,&b,1,18)==1);close(fd);
 CK(nf4_open(&w,path,40)==NF4_QUARANTINED);
 CK(w.ledger.count==0&&w.ledger.balance==40);
 CK(nf4_apply(&w,event(1,1,1,5))==NF4_QUARANTINED);
 nf4_close(&w);unlink(path);
 return true;
}
static bool full(void){
 char path[64];CK(pathnew(path,sizeof path)==0);
 Nf4Wal w;CK(nf4_open(&w,path,0)==NF4_APPLIED);
 for(uint32_t i=0;i<NF4_MAX_RECORDS;++i){
  CK(nf4_apply(&w,event(i+1,1,i+1,0))==NF4_APPLIED);
 }
 CK(w.ledger.count==NF4_MAX_RECORDS);
 CK(nf4_apply(&w,event(65,2,65,0))==NF4_FULL);
 CK(nf4_apply(&w,event(1,1,1,0))==NF4_DUPLICATE);
 nf4_close(&w);
 CK(nf4_open(&w,path,0)==NF4_APPLIED);
 CK(w.ledger.count==NF4_MAX_RECORDS&&w.ledger.epoch==65);
 nf4_close(&w);unlink(path);return true;
}
static uint32_t next(uint32_t *s){
 *s^=*s<<13;*s^=*s>>17;*s^=*s<<5;return *s;
}
static bool corpus(uint32_t seed,unsigned count,unsigned mode,unsigned *applied){
 for(unsigned i=0;i<count;++i){
  char path[64];CK(pathnew(path,sizeof path)==0);
  Nf4Wal w;CK(nf4_open(&w,path,100)==NF4_APPLIED);
  uint32_t u=next(&seed);int32_t debit=(int32_t)(u%110u);
  Nf4Event e=event(1,1+(u%32),i+1,debit);
  Nf4Status expected=debit>100?NF4_INVALID:NF4_APPLIED;
  if(mode==1&&i%3==0)e.epoch=2,expected=NF4_STALE;
  if(mode==2&&i%3==0)debit=INT32_MAX,e.debit=debit,expected=NF4_INVALID;
  if(mode==3&&i%5==0){e.actor=0;expected=NF4_INVALID;}
  Nf4Status got=nf4_apply(&w,e);CK(got==expected);
  if(got==NF4_APPLIED){++*applied;
   CK(w.ledger.balance==100-debit);
   CK(w.ledger.count==1&&w.ledger.epoch==2);
   CK(nf4_apply(&w,e)==NF4_DUPLICATE);
  }else CK(w.ledger.balance==100&&w.ledger.count==0);
  nf4_close(&w);
  CK(nf4_open(&w,path,100)==NF4_APPLIED);
  CK(w.ledger.count==(unsigned)(got==NF4_APPLIED));
  CK(w.ledger.balance==(got==NF4_APPLIED?100-debit:100));
  nf4_close(&w);unlink(path);
 }
 return true;
}
int main(void){
 unsigned applied=0;
 if(!basics()||!uncertain()||!corrupt_and_torn()||!full())return 1;
 const struct {const char*name;uint32_t seed,count,mode;} strata[]={
  {"canonical",0x41000001u,64,0},
  {"randomized",0x41000002u,256,0},
  {"pathological",0x41000003u,64,1},
  {"boundary",0x41000004u,64,2},
  {"heldout",0x41DEC0DEu,256,3}};
 for(unsigned i=0;i<sizeof strata/sizeof strata[0];++i){
  if(!corpus(strata[i].seed,strata[i].count,strata[i].mode,&applied))return 1;
  printf("%s=%u PASS\n",strata[i].name,strata[i].count);
 }
 printf("file_recovery_cases=704 applied=%u assertions=%llu\n",
  applied,checks);return 0;
}
