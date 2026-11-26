#include "spiral09_bundle.h"
#include <stdio.h>
#include <string.h>
static unsigned checks=0;
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL at line %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
int main(void) {
    const NfS08Frame a[]={ {7,501,1,11,0,0,0},{7,501,1,12,3,0,0} };
    const NfS08Frame prev[]={ {7,500,1,11,0,0,0},{7,500,1,12,2,0,0} };
    const uint64_t ev[]={101,102};
    const uint64_t duplicate[]={101,101};
    const size_t tiles[]={120,130};
    const NfS08Cursor prior={0x1234,7,500,1};
    NfS09Batch b={a,2,prev,2.0f,2,ev,2,100,tiles,2,3,250,{0x1234,7,501,1},0x1234};
    NfS09Plan out={0};
    CHECK(nf_s09_plan(&b,&prior,&out));
    CHECK(out.actors==2 && out.events==2 && out.tiles==2 && out.resident_bytes==250);
    CHECK(out.cursor.tick==501 && out.last_event_id==102);
    const NfS09Plan saved=out;
#define REJECT(expr) do { expr; CHECK(!nf_s09_plan(&b,&prior,&out)); CHECK(memcmp(&out,&saved,sizeof(out))==0); b=original; }while(0)
    const NfS09Batch original=b;
    REJECT(b.expected_source_digest=0);
    REJECT(b.proposed_cursor.generation=8);
    REJECT(b.proposed_cursor.tick=500);
    REJECT(b.proposed_cursor.tick=502);
    REJECT(b.actor_count=0);
    REJECT(b.actor_count=33);
    REJECT(b.actors=NULL);
    REJECT(b.max_pose_step=0.5f);
    REJECT(b.max_pose_ticks=0);
    REJECT(b.event_ids=duplicate);
    REJECT(b.last_event_id=101);
    REJECT(b.tile_count=3);
    REJECT(b.max_tile_bytes=249);
    REJECT(b.max_tiles=1);
    REJECT(b.tile_bytes=NULL);
    REJECT(b.event_count=33);
    REJECT(b.event_ids=NULL);
    REJECT(b.proposed_cursor.source_digest=99);
    b.previous_poses=NULL;CHECK(nf_s09_plan(&b,&prior,&out));b=original;
    b.event_count=0;b.event_ids=NULL;CHECK(nf_s09_plan(&b,&prior,&out));b=original;
    b.tile_count=0;b.tile_bytes=NULL;CHECK(nf_s09_plan(&b,&prior,&out));b=original;
    CHECK(!nf_s09_plan(NULL,&prior,&out));
    CHECK(!nf_s09_plan(&b,&prior,NULL));
    puts("Spiral 09 native C bundle: PASS");
    printf("Assertions: %u\n",checks);
    return 0;
}
