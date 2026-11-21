/* SPDX-License-Identifier: GPL-3.0-or-later */
#define _POSIX_C_SOURCE 200809L
#include "nf_retc_h323_checkpoint.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <threads.h>
#include <unistd.h>
static int checks=0,failures=0;
#define CHECK(x) do{checks++;if(!(x)){failures++;fprintf(stderr,"H323.C fail line %d: %s\n",__LINE__,#x);}}while(0)
static NfH323Record fixture(uint32_t txn){
 NfH323Record r={0};
 r.type=NF_H323_TYPE_COMMIT;r.txn_low=txn;r.txn_high=7u;
 r.prior_tick=txn;r.next_tick=txn+1u;r.owner_epoch=15u;
 r.actor[0]=10;r.actor[1]=20;r.object=2010;
 r.action[0]=91;r.action[1]=92;r.contact[0]=101;r.contact[1]=102;
 r.material_epoch=22;r.grant[0]=301;r.grant[1]=302;
 r.receipt[0]=401;r.receipt[1]=402;r.prior_digest=111;r.new_digest=222;
 return r;
}
static void paths(const char *base,uint32_t gen,char *s,char *m){
 snprintf(s,1024,"%s.snap.%u",base,gen);
 snprintf(m,1024,"%s.manifest.%u",base,gen);
}
static void clean(const char *base,uint32_t gen){
 char s[1024],m[1024];paths(base,gen,s,m);unlink(s);unlink(m);
}
typedef struct {char base[256];unsigned errors;} Reader;
static int reader(void *arg){
 Reader *w=arg;
 for(int i=0;i<256;i++){
  NfH323Record x={0};
  if(!nf_h323_checkpoint_read(w->base,1,&x)||
     x.actor[0]!=10||x.actor[1]!=20||x.object!=2010||
     x.receipt[0]!=401||x.receipt[1]!=402)w->errors++;
 }
 return 0;
}
int main(void){
 char base[256],s[1024],m[1024];
 snprintf(base,sizeof base,"/tmp/nf_h323_ckpt_%ld",(long)getpid());
 NfH323Record r=fixture(100),out={0};
 clean(base,1);
 CHECK(!nf_h323_checkpoint_read(base,1,&out));
 CHECK(nf_h323_checkpoint_create(base,1,&r));
 CHECK(nf_h323_checkpoint_read(base,1,&out));
 CHECK(out.txn_low==100&&out.actor[1]==20&&out.receipt[1]==402);
 CHECK(!nf_h323_checkpoint_create(base,1,&r)); /* immutable */
 for(int i=0;i<1024;i++)CHECK(nf_h323_checkpoint_read(base,1,&out)&&out.txn_low==100);
 thrd_t threads[4];Reader w[4];int started=0;
 for(int j=0;j<4;j++){
  memset(&w[j],0,sizeof w[j]);snprintf(w[j].base,sizeof w[j].base,"%s",base);
  if(thrd_create(&threads[j],reader,&w[j])!=thrd_success)break;
  started++;
 }
 CHECK(started==4);
 for(int j=0;j<started;j++){
  int status=-1;CHECK(thrd_join(threads[j],&status)==thrd_success&&status==0&&w[j].errors==0);
 }
 clean(base,1);
 const char *cuts[]={"after_snapshot","after_manifest_write",
  "after_manifest_sync","after_manifest_link","after_dirsync"};
 for(int j=0;j<5;j++){
  uint32_t gen=(uint32_t)j+10u;
  clean(base,gen);
  pid_t pid=fork();CHECK(pid>=0);if(pid<0)continue;
  if(pid==0){
   setenv("NF_H323_CHECKPOINT_KILL",cuts[j],1);
   _exit(nf_h323_checkpoint_create(base,gen,&r)?0:91);
  }
  int status=0;
  CHECK(waitpid(pid,&status,0)==pid&&WIFEXITED(status)&&WEXITSTATUS(status)==93);
  out=fixture(999);
  int accepted=nf_h323_checkpoint_read(base,gen,&out);
  CHECK(accepted==(j>=3));
  if(j>=3)CHECK(out.txn_low==100);
  else CHECK(out.txn_low==999);
  /* Crash can leave orphan temp. A full recovery must reconcile these. */
  char tmp[1200];paths(base,gen,s,m);
  int length=snprintf(tmp,sizeof tmp,"%s.tmp.%ld",m,(long)pid);
  CHECK(length>0&&(size_t)length<sizeof tmp);
  if(length>0&&(size_t)length<sizeof tmp)unlink(tmp);
  clean(base,gen);
 }
 /* Every manifest byte must be covered by the manifest integrity checksum. */
 for(int i=0;i<40;i++){
  uint32_t gen=100u+(uint32_t)i;clean(base,gen);
  CHECK(nf_h323_checkpoint_create(base,gen,&r));
  paths(base,gen,s,m);
  int fd=open(m,O_RDWR);CHECK(fd>=0);
  if(fd>=0){
   unsigned char ch=0;
   CHECK(pread(fd,&ch,1,i)==1);
   ch^=0x80;
   CHECK(pwrite(fd,&ch,1,i)==1);
   CHECK(close(fd)==0);
  }
  out=fixture(999);
  CHECK(!nf_h323_checkpoint_read(base,gen,&out)&&out.txn_low==999);
  clean(base,gen);
 }
 /* A valid manifest must never permit a corrupted/orphan snapshot. */
 clean(base,200);
 CHECK(nf_h323_checkpoint_create(base,200,&r));
 paths(base,200,s,m);
 CHECK(truncate(s,77)==0);
 CHECK(!nf_h323_checkpoint_read(base,200,&out));
 clean(base,200);
 /* Two generations must be independently immutable, not an overwrite. */
 clean(base,201);clean(base,202);
 r=fixture(100);CHECK(nf_h323_checkpoint_create(base,201,&r));
 r=fixture(101);CHECK(nf_h323_checkpoint_create(base,202,&r));
 CHECK(nf_h323_checkpoint_read(base,201,&out)&&out.txn_low==100);
 CHECK(nf_h323_checkpoint_read(base,202,&out)&&out.txn_low==101);
 clean(base,201);clean(base,202);
 printf("H323.C checkpoint checks=%d failures=%d serial=1024 parallel=1024 cuts=5 corrupt_manifest=40\n",checks,failures);
 return failures?1:0;
}
