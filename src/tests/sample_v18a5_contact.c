#include "nf_contact18a5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static unsigned seed_state=0x12345678u;
static unsigned rnd(void){seed_state^=seed_state<<13;seed_state^=seed_state>>17;seed_state^=seed_state<<5;return seed_state;}
static Nf18a5Sample mk(uint32_t t,uint32_t id,float j,float load){
    return (Nf18a5Sample){t,id,42u,7u,j,load,0.0f,1u};
}
static void field(uint8_t *coarse,uint8_t *fine,unsigned seed){
    memset(coarse,0,64u);coarse[0]=NF18A5_MIXED;coarse[1]=NF18A5_SOLID;
    memset(fine,0,4096u);
    unsigned plane=1u+seed%3u;
    for(unsigned y=0;y<4u;++y)for(unsigned z=0;z<4u;++z)for(unsigned x=0;x<4u;++x){
        fine[(y*16u+z)*16u+x]=x<plane?NF18A5_SOLID:NF18A5_FREE;
        fine[(y*16u+z)*16u+x+4u]=NF18A5_SOLID;
    }
}
static const char *grid_name(int i){const char *n[]={"canonical_only","eager_fine","selective_fine","hysteretic_fine","unsafe_missing_fine"};return n[i];}
static const char *hist_name(int i){const char *n[]={"raw_all","peak_only","windows_only","cumulative_ack","overwrite_control"};return n[i];}
static void grid_samples(FILE *out){
    uint8_t coarse[64],fine[4096];
    for(unsigned fixture=0;fixture<1000u;++fixture){
        unsigned cohort=fixture/200u, seed=fixture%200u;
        seed_state=0x5a5a1234u^fixture*0x9e3779b9u;
        field(coarse,fine,seed);
        for(unsigned model=0;model<5u;++model){
            Nf18a5Grid grid;nf18a5_grid_init(&grid,1,(Nf18a5GridPolicy)model);
            if(!nf18a5_load_canonical(&grid,0,0,0,coarse,1u)){fprintf(stderr,"grid load fail\n");exit(3);}
            unsigned false_free=0,false_block=0,pending=0,accurate=0,loaded=0;
            size_t mem_total=0;
            unsigned last_relevant=0u;
            for(unsigned t=1;t<=12u;++t){
                /* A consequential mixed-voxel query at t=1, t=5.  The
                   selective policy drops fine when the demand disappears. */
                bool relevant=t==1u||t==5u;
                if(relevant) last_relevant=t;
                if(model==2u && !relevant) (void)nf18a5_release_fine(&grid,0,0,0);
                if(model==3u && last_relevant && t-last_relevant>3u)
                    (void)nf18a5_release_fine(&grid,0,0,0);
                if(model!=4u && nf18a5_should_refine(&grid,0,0,0,relevant,t,3u) && !grid.chunks[0].fine_valid){
                    if(!nf18a5_load_fine(&grid,0,0,0,4,fine,4096,1u,t)){fprintf(stderr,"fine load fail\n");exit(4);}
                    ++loaded;
                }
                unsigned plane=1u+seed%3u;
                unsigned cell=t%3u;
                float x=(cell==0u ? (float)plane/4.0f+0.005f+(float)(rnd()%180u)/10000.0f : cell==1u?0.01f+(float)(rnd()%100u)/10000.0f:1.1f);
                if(x>=1.0f&&cell==0u)x=0.95f;
                int expected=cell==2u?1 : cell==1u?1 : 0;
                Nf18a5Query q=nf18a5_query(&grid,x,0.1f,0.1f,t,true);
                if(q==NF18A5_Q_PENDING)++pending;
                else if(q==NF18A5_Q_FREE && expected)++false_free;
                else if(q==NF18A5_Q_SOLID && !expected)++false_block;
                else if((q==NF18A5_Q_FREE && !expected)||(q==NF18A5_Q_SOLID && expected))++accurate;
                else {fprintf(stderr,"unexpected invalid query\n");exit(5);}
                mem_total+=nf18a5_resident_bytes(&grid);
            }
            fprintf(out,"grid,%u,%u,%s,%u,%u,%u,%u,%u,%zu,0,0,0,0\n",fixture,cohort,grid_name((int)model),
                    accurate,false_free,false_block,pending,loaded,mem_total/12u);
        }
    }
}
static void history_samples(FILE *out){
    for(unsigned fixture=0;fixture<1000u;++fixture){
        unsigned cohort=fixture/200u;
        seed_state=0x3417ae15u^fixture*0x85ebca6bu;
        unsigned kind=fixture%5u;
        unsigned count=kind==0u?10u:kind==1u?48u:kind==2u?45u:kind==3u?75u:kind==4u?30u:10u;
        float j=kind==0u?25.0f+(float)(rnd()%40u)/10.0f:
                kind==1u?2.0f+(float)(rnd()%20u)/100.0f:
                kind==2u?0.0f:kind==3u?0.1f+(float)(rnd()%15u)/100.0f:0.3f;
        float f=kind==2u?120.0f+(float)(rnd()%60u)/10.0f:0.0f;
        bool ground_truth=kind<=3u;
        for(unsigned model=0u;model<5u;++model){
            Nf18a5History h;nf18a5_history_init(&h,(Nf18a5HistoryPolicy)model);
            bool okay=true;
            for(unsigned tick=1u;tick<=count;++tick){
                Nf18a5Sample s=mk(tick,10u+fixture,j,f);
                if(!nf18a5_history_commit(&h,tick,tick,42u,&s,1u)){okay=false;break;}
            }
            if(okay)okay=nf18a5_history_close(&h,count+1u,count+1u,42u,10u+fixture);
            if(!okay){fprintf(stderr,"history commit failure fixture=%u model=%u\n",fixture,model);exit(6);}
            bool detected=h.outbox_count>0u;
            /* This corpus ends each episode and consumes ALL committed samples. */
            bool correct=(ground_truth==detected);
            unsigned retained=h.raw_retained;
            unsigned durable=0u;
            if(model==3u){
                /* Simulate a durable sink acknowledging the complete contiguous
                   prefix; this test does not itself implement file persistence. */
                if(h.outbox_count>0u && !nf18a5_ack(&h,h.outbox[h.outbox_count-1u].sequence))exit(7);
                durable=h.generated_events;
            }
            fprintf(out,"history,%u,%u,%s,%u,%u,%u,%u,%u,%zu,%u,%u,%u,%u\n",fixture,cohort,hist_name((int)model),
                    correct?1u:0u,detected?1u:0u,ground_truth?1u:0u,h.lost_records,retained,
                    sizeof(h),h.summary_count,h.generated_events,durable,h.raw_samples);
        }
    }
}
int main(int argc,char **argv){
    const char *name=argc>1?argv[1]:"build/v18a5/model_samples.csv";
    FILE *out=fopen(name,"w");if(!out){perror(name);return 2;}
    fprintf(out,"family,fixture,cohort,model,correct_or_accurate,false_free_or_detected,false_block_or_truth,pending_or_loss,loaded_or_retained,resident_bytes,summary_count,events_generated,durable_acked,sample_count\n");
    grid_samples(out);history_samples(out);
    fclose(out);
    puts("v1.8A.5 grid/history 10000-model-evaluation corpus: generated");return 0;
}
