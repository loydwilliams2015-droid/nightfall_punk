/* MA03G / MA02-8 H1 negative-gate laboratory, NOT live A4/A6 authority. */
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
static uint32_t rng=0x3a17b2c5u;
static double random01(void){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return (rng&0xffffffu)/16777216.;}
typedef enum {NONE,SINGLE,SIMULTANEOUS,INVALID} Contact;
static Contact classify(double g0,double g1,double v0,double v1,double dt){
 if(!isfinite(g0)||!isfinite(g1)||!isfinite(v0)||!isfinite(v1)||!isfinite(dt)||dt<=0||g0<=0||g1<=0)return INVALID;
 double t0=v0>0?g0/v0:INFINITY,t1=v1>0?g1/v1:INFINITY;
 if(fmin(t0,t1)>dt)return NONE;
 return fabs(t0-t1)<=1e-10*fmax(1.,dt)?SIMULTANEOUS:SINGLE;
}
typedef struct{uintptr_t owner;uint32_t actor,tick,revision,material,action;bool active;} Snapshot;
static bool same_owner(Snapshot a,Snapshot b,bool in_tick){
 return in_tick&&a.active&&b.active&&a.owner&&a.owner==b.owner&&a.actor&&a.actor==b.actor&&
 a.tick==b.tick&&a.revision==b.revision&&a.material==b.material&&a.action&&a.action==b.action;
}
int main(void){
 for(unsigned i=0;i<20000;i++){double x=.001+100*random01(),y=.001+100*random01();
  if(classify(.1*x,.1*y,x,y,1.+random01())!=SIMULTANEOUS)return 1;}
 if(classify(1,2,2,2,1)!=SINGLE||classify(10,10,1,1,1)!=NONE||
 classify(-1,1,1,1,1)!=INVALID||classify(1,1,NAN,1,1)!=INVALID)return 2;
 Snapshot a={1,7,5,12,9,3,true},b=a;
 if(!same_owner(a,a,true))return 3;
 b.tick++;if(same_owner(a,b,true))return 4;b=a;b.revision++;
 if(same_owner(a,b,true))return 5;
 b=a;b.material++;
 if(same_owner(a,b,true))return 6;
 b=a;b.owner++;
 if(same_owner(a,b,true))return 7;
 b=a;b.action++;
 if(same_owner(a,b,true))return 8;
 b=a;b.actor++;
 if(same_owner(a,b,true))return 9;
 b=a;b.active=false;
 if(same_owner(a,b,true)||same_owner(a,a,false))return 10;
 puts("MA03G/MA02-8 H1 gate corpus PASS: 20000 simultaneous seeds, 4 collision controls, 9 owner controls");return 0;
}
