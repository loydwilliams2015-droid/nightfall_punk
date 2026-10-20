#define _POSIX_C_SOURCE 200809L
#include "nf_ucw18c2.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
static int fail(int line){fprintf(stderr,"FAIL:%d\n",line);return 1;}
#define CHECK(x) do { if(!(x))return fail(__LINE__); }while(0)
int main(void){
 for(unsigned family=0;family<5;family++)for(unsigned seed=0;seed<64;seed++){
  char path[]="/tmp/nfucw2_XXXXXX";int fd=mkstemp(path);CHECK(fd>=0);close(fd);
  Ucw2Journal journal;CHECK(ucw2_open(&journal,path,1000,1000)==UCW_OK);
  for(unsigned step=0;step<4;step++){
   UcwProposal action={0};action.actor=step+1;action.action=100+step;
   action.object=200+step;action.contract=300+step;
   action.authority_epoch=journal.state.revision;
   action.read_revision=journal.state.revision;
   action.material_delta=-(int32_t)((family+seed+step)%7);
   action.contract_delta=-1;
   UcwCandidate candidate={0};CHECK(ucw_stage(&journal.state,&action,1,&candidate)==UCW_OK);
   CHECK(ucw2_commit(&journal,&candidate)==UCW_OK);
   const uint64_t hash=journal.state.history_hash;
   ucw2_close(&journal);
   CHECK(ucw2_open(&journal,path,1000,1000)==UCW_OK);
   CHECK(journal.state.count==step+1 && journal.state.history_hash==hash);
   CHECK(journal.state.receipts[step].action==action.action);
   CHECK(ucw2_commit(&journal,&candidate)==UCW_STALE);
  }
  ucw2_close(&journal);unlink(path);
 }
 puts("UCW2_320_SCENARIOS_PASS");return 0;
}
