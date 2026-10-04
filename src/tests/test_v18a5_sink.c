#define _POSIX_C_SOURCE 200809L
#include "nf_contact18a5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static int failure=0;
static void check(int truth,const char *name){printf("%s,%s\n",name,truth?"PASS":"FAIL");if(!truth)failure=1;}
static Nf18a5Sample s(uint32_t t){return (Nf18a5Sample){t,55,42,7,25.0f,0,0,1};}
int main(void){
  char name[]="/tmp/nf18a5_sink_XXXXXX";
  int fd=mkstemp(name);if(fd<0)return 2;close(fd);
  Nf18a5History h;nf18a5_history_init(&h,NF18A5_CUMULATIVE_DURABLE);
  Nf18a5Sample first=s(1);
  check(nf18a5_history_commit(&h,1,1,42,&first,1),"P01_commit_prior_to_durable_event");
  check(nf18a5_history_close(&h,2,2,42,55),"P02_summary_generates_event");
  Nf18a5History replay=h; /* power loss after file sync before ACK */
  check(nf18a5_persist_outbox(&h,name),"P03_fsync_and_ack");
  check(h.ack_sequence==1&&h.outbox_count==0,"P04_ack_only_after_disk_write");
  struct stat before,after;stat(name,&before);
  check(nf18a5_persist_outbox(&replay,name),"P05_replayed_unacknowledged_event_idempotent");
  stat(name,&after);
  check(before.st_size==after.st_size,"P06_no_duplicate_disk_entry");
  FILE *f=fopen(name,"a");if(!f)return 3;fputs("torn",f);fclose(f);
  Nf18a5History fresh;nf18a5_history_init(&fresh,NF18A5_CUMULATIVE_DURABLE);
  Nf18a5Sample v=s(1);check(nf18a5_history_commit(&fresh,1,1,42,&v,1),"P07_new_branch_event");
  check(nf18a5_history_close(&fresh,2,2,42,55),"P08_branch_summary");
  check(!nf18a5_persist_outbox(&fresh,name)&&fresh.outbox_count==1,"P09_torn_record_fail_closed");
  unlink(name);
  printf("SINK,%s\n",failure?"FAIL":"PASS");return failure;
}
