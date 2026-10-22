/* MA03H/MA02-9 safety laboratory: no live A4 or A6 authority. */
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
typedef enum {PENDING,SINGLE,MISS} Decision;
static Decision classify(double g0,double g1,double v0,double v1,double dt){
 if(!isfinite(g0)||!isfinite(g1)||!isfinite(v0)||!isfinite(v1)||!isfinite(dt)||dt<=0||g0<=0||g1<=0)return PENDING;
 double a=v0>0?g0/v0:INFINITY,b=v1>0?g1/v1:INFINITY;
 if(a>dt&&b>dt)return MISS;
 if(a<=dt&&b<=dt&&fabs(a-b)<=1e-9*fmax(1.,dt))return PENDING;
 return SINGLE;
}
typedef struct {uint64_t owner;unsigned actor,tick,revision,material,action;bool in_tick;} Token;
static bool allowed(Token x,Token y){return x.owner&&x.actor&&x.action&&y.in_tick&&x.owner==y.owner&&x.actor==y.actor&&x.tick==y.tick&&x.revision==y.revision&&x.material==y.material&&x.action==y.action;}
static uint32_t rng=0x018c1031u;
static double draw(void){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return (rng&0xffffffu)/16777216.;}
int main(void){
 for(unsigned i=0;i<20000;i++){double v=1.+100.*draw(),t=.00001+.01*draw(),g=v*t;
  if(classify(g,g,v,v,1.)!=PENDING)return 1;}
 if(classify(1,2,2,2,1.)!=SINGLE||classify(10,10,1,1,1)!=MISS||
    classify(-1,1,1,1,1)!=PENDING||classify(NAN,1,1,1,1)!=PENDING)return 2;
 Token e={1,2,3,4,5,6,true},n=e;
 if(!allowed(e,n))return 3;
 n=e;n.tick++;if(allowed(e,n))return 4;
 n=e;n.revision++;if(allowed(e,n))return 5;
 n=e;n.material++;if(allowed(e,n))return 6;
 n=e;n.actor++;if(allowed(e,n))return 7;
 n=e;n.owner++;if(allowed(e,n))return 8;
 n=e;n.action++;if(allowed(e,n))return 9;
 n=e;n.in_tick=false;if(allowed(e,n))return 10;
 puts("PASS: 20000 simultaneous guards, 4 collision controls, 8 owner controls");return 0;
}
