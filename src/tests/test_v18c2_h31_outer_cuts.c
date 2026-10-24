#define _POSIX_C_SOURCE 200809L
#include "nf_c2_h22_envelope.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CK(v) do{if(!(v)){fprintf(stderr,"H31 FAIL line %d\n",__LINE__);return 1;}}while(0)
int main(int argc,char **argv){
 CK(argc==2);
 const char *cuts[]={"before_world_publish","after_physical_copy",
   "after_world_publish","before_b3_realize","after_b3_realize"};
 for(unsigned i=0;i<5;i++){
  pid_t pid=fork();CK(pid>=0);
  if(!pid){
   setenv("NF_H31_KILL_BOUNDARY",cuts[i],1);
   execl(argv[1],argv[1],(char *)NULL);
   _exit(99);
  }
  int status=0;
  CK(waitpid(pid,&status,0)==pid);
  CK(WIFEXITED(status)&&WEXITSTATUS(status)==94);
  /* The checkpoint itself may already exist before live-world publication.
     A valid checkpoint does NOT establish that the stopped writer exposed
     its physical and contractual domains atomically. */
  char path[200];
  snprintf(path,sizeof path,"/tmp/nightfall_h24_check_%ld.bin",(long)pid);
  pid_t recover=fork();CK(recover>=0);
  if(!recover){
   Nf18a5Integrated *s=calloc(1,sizeof *s);Nf18b3Event e={0};
   if(!s)_exit(8);
   const int ok=nf_c2_h23_restore_full(path,s,&e);
   if(ok && (s->world.tick!=e.tick||s->actor.id!=e.actor||
              e.phase!=NF18B3_APPLIED||!e.applied))_exit(7);
   free(s);_exit(0);
  }
  CK(waitpid(recover,&status,0)==recover&&WIFEXITED(status)&&WEXITSTATUS(status)==0);
  remove(path);
 }
 puts("H31 5 outer cutpoints terminated; fresh-process envelope integrity/refusal PASS");
 puts("WHOLE-WORLD ATOMICITY NOT PROVEN: recovery does not rebuild live A6 and B3");
 return 0;
}
