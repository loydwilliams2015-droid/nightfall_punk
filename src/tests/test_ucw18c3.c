#define _POSIX_C_SOURCE 200809L
#include "nf_ucw18c3.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
static unsigned checks;
#define CK(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);return 1;}checks++;}while(0)
int main(void){
 for(unsigned family=0;family<5;family++)for(unsigned seed=0;seed<24;seed++){
  char path[]="/tmp/ucw3_XXXXXX";int fd=mkstemp(path);CK(fd>=0);close(fd);
  Ucw3Snapshot base;memset(&base,0,sizeof base);
  ucw_init(&base.contracts,1000,1000);
  base.physical.world.revision=1;
  Ucw3Journal j;CK(ucw3_open(&j,path,&base)==UCW_OK);
  for(unsigned i=0;i<3;i++){
   UcwProposal p={0};
   p.actor=i+1;p.action=100+i;p.object=500+i;p.contract=700+i;
   p.read_revision=j.contracts.revision;p.authority_epoch=j.contracts.revision;
   p.material_delta=-(int32_t)((seed+family+i)%7);
   p.contract_delta=-1;
   UcwCandidate candidate={0};
   CK(ucw_stage(&j.contracts,&p,1,&candidate)==UCW_OK);
   Ucw3Snapshot next={0};
   next.physical=j.physical;next.physical.world.revision++;
   next.physical.world.tick++;
   next.physical.committed_receipts++;
   next.contracts=candidate.staged;
   CK(ucw3_commit(&j,&next)==UCW_OK);
   uint64_t hash=next.contracts.history_hash;
   ucw3_close(&j);
   CK(ucw3_open(&j,path,&base)==UCW_OK);
   CK(j.contracts.history_hash==hash);
   CK(j.physical.committed_receipts==i+1);
   CK(j.contracts.receipts[i].action==p.action);
   CK(ucw3_commit(&j,&next)==UCW_STALE);
  }
  ucw3_close(&j);
  if(family==3){
   fd=open(path,O_WRONLY|O_APPEND);CK(fd>=0);
   CK(write(fd,"BAD",3)==3);close(fd);
   CK(ucw3_open(&j,path,&base)==UCW_UNCERTAIN&&j.quarantine);
   ucw3_close(&j);
  }
  if(family==4){
   fd=open(path,O_RDWR);CK(fd>=0);
   unsigned char v=0;CK(pread(fd,&v,1,32)==1);
   v^=1;CK(pwrite(fd,&v,1,32)==1);close(fd);
   CK(ucw3_open(&j,path,&base)==UCW_UNCERTAIN&&j.quarantine);
   ucw3_close(&j);
  }
  unlink(path);
 }
 printf("UCW3_JOINT_A5_CONTRACT_H0 scenarios=120 checks=%u PASS\n",checks);
 return 0;
}
