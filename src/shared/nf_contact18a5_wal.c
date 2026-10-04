#define _POSIX_C_SOURCE 200809L
#include "nf_contact18a5_close.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/file.h>

/* Testable single-process append-only snapshot WAL. ABI-local struct snapshots
   (including float layout) are deliberately not portable production records. */
typedef struct Rec {
    uint32_t magic,bytes,checksum,version;
} Rec;
static uint32_t sum(const void *data,size_t n){
    uint32_t h=2166136261u;const unsigned char *p=data;
    for(size_t i=0;i<n;++i){h^=p[i];h*=16777619u;}
    return h;
}
static bool sync_parent_directory(const char *path){
    char parent[4096];
    size_t length=strlen(path);
    if(length==0||length>=sizeof(parent))return false;
    memcpy(parent,path,length+1u);
    char *slash=strrchr(parent,'/');
    if(!slash)strcpy(parent,".");
    else if(slash==parent)parent[1]='\0';
    else *slash='\0';
    int fd=open(parent,O_RDONLY);
    if(fd<0)return false;
    bool ok=fsync(fd)==0;
    if(close(fd)!=0)ok=false;
    return ok;
}
static bool scan(FILE *f,Nf18a5Integrated *latest,uint32_t *last){
    *last=0;
    for(;;){
        Rec r={0};size_t n=fread(&r,1,sizeof(r),f);
        if(!n){if(ferror(f))return false;return true;}
        if(n!=sizeof(r)||r.magic!=0x1845a5d0u||r.bytes!=sizeof(*latest)||
           r.version==0||r.version<=*last)return false;
        Nf18a5Integrated candidate;
        if(fread(&candidate,1,sizeof(candidate),f)!=sizeof(candidate)||
           sum(&candidate,sizeof(candidate))!=r.checksum ||
           candidate.world.revision!=r.version)return false;
        *last=r.version;*latest=candidate;
    }
}
bool nf18a5_restore(const char *path,Nf18a5Integrated *s){
    if(!path||!s)return false;
    FILE *f=fopen(path,"rb");if(!f)return false;
    Nf18a5Integrated last={0};uint32_t v=0;
    bool ok=flock(fileno(f),LOCK_SH)==0;
    if(ok)ok=scan(f,&last,&v);
    if(fclose(f)!=0)ok=false;
    if(ok&&v)*s=last;
    return ok&&v!=0;
}
bool nf18a5_checkpoint(const char *path,const Nf18a5Integrated *s){
    if(!path||!s||s->world.revision==0u)return false;
    /* r+b opens an existing log; w+b creates a new one. No blind append to a
       torn/corrupt log or a conflicting higher-version snapshot. */
    FILE *f=fopen(path,"r+b");
    if(!f)f=fopen(path,"w+b");
    if(!f)return false;
    if(flock(fileno(f),LOCK_EX)!=0){fclose(f);return false;}
    Nf18a5Integrated prev={0};uint32_t last=0;
    bool ok=scan(f,&prev,&last);
    if(ok && last==s->world.revision){
        /* Replaying an already fsynced but not yet published snapshot is
           idempotent only for an identical byte-level candidate. */
        ok=(memcmp(&prev,s,sizeof(*s))==0);
        if(fclose(f)!=0)ok=false;
        return ok;
    }
    ok=ok && (!last || s->world.revision==last+1u);
    if(ok&&fseek(f,0,SEEK_END)!=0)ok=false;
    const Rec r={0x1845a5d0u,(uint32_t)sizeof(*s),sum(s,sizeof(*s)),s->world.revision};
    if(ok&&(fwrite(&r,1,sizeof(r),f)!=sizeof(r)||
            fwrite(s,1,sizeof(*s),f)!=sizeof(*s)))ok=false;
    if(ok&&(fflush(f)!=0||fsync(fileno(f))!=0))ok=false;
    if(fclose(f)!=0)ok=false;
    if(ok)ok=sync_parent_directory(path);
    return ok;
}
