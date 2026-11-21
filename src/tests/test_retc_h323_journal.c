/* SPDX-License-Identifier: GPL-3.0-or-later */
#define _POSIX_C_SOURCE 200809L
#include "nf_retc_h323_journal.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <threads.h>
#include <unistd.h>
static int checks=0,failures=0;
#define CHECK(v) do {checks++;if(!(v)){failures++;fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#v);}}while(0)
static NfH323Record fixture(void) {
 NfH323Record r={0};r.type=NF_H323_TYPE_COMMIT;r.txn_low=91;r.txn_high=42;
 r.prior_tick=3;r.next_tick=4;r.owner_epoch=7;r.actor[0]=10;r.actor[1]=20;
 r.object=2010;r.action[0]=91;r.action[1]=92;r.contact[0]=111;r.contact[1]=112;
 r.material_epoch=8;r.grant[0]=301;r.grant[1]=302;r.receipt[0]=401;r.receipt[1]=402;
 r.prior_digest=0x11223344;r.new_digest=0xaabbccdd;return r;
}
typedef struct {char path[220];uint32_t fail;} Worker;
static int reader(void *p) {
 Worker *w=p;for(int i=0;i<256;i++){NfH323Record r={0};
  if(!nf_h323_journal_read(w->path,&r)||r.actor[0]!=10||r.actor[1]!=20||
     r.receipt[1]!=402)w->fail++;
 }return 0;
}
static void pathfor(char *out,size_t n,int i){
 snprintf(out,n,"/tmp/nf_h323_journal_%ld_%d.bin",(long)getpid(),i);
}
int main(void){
 char path[220];NfH323Record r=fixture(),out={0};
 pathfor(path,sizeof path,0);unlink(path);
 CHECK(nf_h323_journal_write(path,&r));
 CHECK(nf_h323_journal_read(path,&out));
 CHECK(out.actor[0]==10&&out.actor[1]==20&&out.receipt[0]==401);
 CHECK(!nf_h323_journal_write(path,&r)); /* not a replace API */
 for(int i=0;i<1024;i++) {
  CHECK(nf_h323_journal_read(path,&out)&&out.txn_low==91);
 }
 thrd_t t[4];Worker w[4];int started=0;
 for(int i=0;i<4;i++){
  memset(&w[i],0,sizeof w[i]);snprintf(w[i].path,sizeof w[i].path,"%s",path);
  if(thrd_create(&t[i],reader,&w[i])!=thrd_success)break;
  started++;
 }
 CHECK(started==4);
 for(int i=0;i<started;i++){int status=-1;
  CHECK(thrd_join(t[i],&status)==thrd_success&&status==0&&w[i].fail==0);
 }
 CHECK(unlink(path)==0);
 const char *cuts[]={"before_open","after_prepare_write","after_prepare_sync",
    "after_commit_write","after_commit_sync","before_rename",
    "after_rename","after_dirsync"};
 for(int i=0;i<8;i++){
  pathfor(path,sizeof path,10+i);unlink(path);
  pid_t pid=fork();CHECK(pid>=0);if(pid<0)continue;
  if(pid==0){
   setenv("NF_H323_KILL",cuts[i],1);
   (void)nf_h323_journal_write(path,&r);_exit(91);
  }
  int status=0;CHECK(waitpid(pid,&status,0)==pid&&WIFEXITED(status)&&WEXITSTATUS(status)==93);
  out=(NfH323Record){0};
  CHECK(nf_h323_journal_read(path,&out)==(i>=6));
  if(i>=6)CHECK(out.txn_low==91&&out.receipt[1]==402);
  unlink(path);
 }
 /* Mutate either packet, including phase bytes, header and payload.
  * Every tampering mutation must fail without altering caller's output. */
 for(int i=0;i<224;i++){
  pathfor(path,sizeof path,40+i);unlink(path);
  CHECK(nf_h323_journal_write(path,&r));
  int fd=open(path,O_RDWR);CHECK(fd>=0);
  if(fd>=0){
   unsigned char ch=0;CHECK(pread(fd,&ch,1,i)==1);
   ch^=0x80;CHECK(pwrite(fd,&ch,1,i)==1);
   CHECK(close(fd)==0);
  }
  out=fixture();out.txn_low=999;
  CHECK(!nf_h323_journal_read(path,&out)&&out.txn_low==999);
  CHECK(unlink(path)==0);
 }
 pathfor(path,sizeof path,300);unlink(path);
 CHECK(nf_h323_journal_write(path,&r));
 CHECK(truncate(path,120)==0);
 CHECK(!nf_h323_journal_read(path,&out));
 CHECK(unlink(path)==0);
 printf("H323.B journal checks=%d failures=%d serial_reads=1024 parallel_reads=1024 cuts=8 corrupt_bytes=224\n",checks,failures);
 return failures?1:0;
}
