#define _POSIX_C_SOURCE 200809L
#include "nf_contact18a5.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

/* POSIX laboratory append journal. A line stores the event DATA and a checksum;
   an unacknowledged append may be replayed idempotently. This is not a WAL for
   the entire authoritative world and cannot self-repair a torn record. */
static uint32_t checksum(const char *p){
    uint32_t x=2166136261u;
    for(;*p;++p){x^=(unsigned char)*p;x*=16777619u;}
    return x;
}
static bool payload(const Nf18a5Event *e,char *buf,size_t cap){
    int n=snprintf(buf,cap,
       "%" PRIu64 "|%" PRIu32 "|%" PRIu32 "|%" PRIu32 "|%" PRIu32 "|%" PRIu32
       "|%u|%.17g|%.17g|%.9g|%.9g|%" PRIu32,
       e->sequence,e->contact_id,e->first_tick,e->last_tick,e->samples,
       e->material_epoch,(unsigned)e->reason_mask,e->impulse_sum,e->force_time,
       (double)e->impulse_peak,(double)e->force_peak,e->digest);
    return n>0 && (size_t)n<cap;
}
bool nf18a5_persist_outbox(Nf18a5History *h,const char *path){
    if(!h||!path||!*path)return false;
    if(!h->outbox_count)return true;
    FILE *f=fopen(path,"a+");if(!f)return false;
    rewind(f);
    char line[512],data[480];uint64_t last=0u;
    uint64_t recent_seq[NF18A5_OUTBOX]={0};uint32_t recent_crc[NF18A5_OUTBOX]={0};
    bool ok=true;
    while(fgets(line,sizeof(line),f)){
        size_t n=strlen(line);
        if(!n || line[n-1u]!='\n'){ok=false;break;}
        char *last_pipe=strrchr(line,'|');
        if(!last_pipe){ok=false;break;}
        uint32_t claimed=0u,id=0u;uint64_t seq=0u;
        char tail='x';
        if(sscanf(last_pipe+1,"%" SCNx32 "%c",&claimed,&tail)!=2 || tail!='\n'){
            ok=false;break;
        }
        *last_pipe='\0';
        if(checksum(line)!=claimed ||
           sscanf(line,"%" SCNu64 "|%" SCNu32,&seq,&id)!=2 ||
           !id || seq!=last+1u){ok=false;break;}
        last=seq;
        recent_seq[seq%NF18A5_OUTBOX]=seq;recent_crc[seq%NF18A5_OUTBOX]=claimed;
    }
    if(ferror(f))ok=false;
    if(ok && h->outbox[0].sequence>last+1u)ok=false;
    for(unsigned i=0u;ok && i<h->outbox_count;++i){
        const Nf18a5Event *e=&h->outbox[i];
        if(!payload(e,data,sizeof(data))){ok=false;break;}
        if(e->sequence<=last){
            unsigned slot=(unsigned)(e->sequence%NF18A5_OUTBOX);
            if(recent_seq[slot]!=e->sequence || recent_crc[slot]!=checksum(data))ok=false;
        }
    }
    if(ok && fseek(f,0,SEEK_END)!=0)ok=false;
    for(unsigned i=0u;ok && i<h->outbox_count;++i){
        const Nf18a5Event *e=&h->outbox[i];
        if(e->sequence<=last)continue;
        if(e->sequence!=last+1u || !payload(e,data,sizeof(data)) ||
           fprintf(f,"%s|%08" PRIx32 "\n",data,checksum(data))<0){ok=false;break;}
        last=e->sequence;
    }
    if(ok && (fflush(f)!=0 || fsync(fileno(f))!=0))ok=false;
    if(fclose(f)!=0)ok=false;
    if(!ok)return false;
    return nf18a5_ack(h,h->outbox[h->outbox_count-1u].sequence);
}
