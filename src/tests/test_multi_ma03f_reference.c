/* MA03F independent, intentionally narrow simultaneous three-body oracle.
 * Equal masses, equal radii, collinear symmetric approaching speeds only.
 * Not A4, not full multi-actor runtime; unsupported cases fail closed. */
#include <math.h>
#include <stdio.h>
#include <stdint.h>
typedef struct { double x[3],v[3]; int valid; } Ref;
static Ref oracle(const double x[3],const double v[3],double m,double r,double dt){
 Ref o={0};if(!x||!v||!isfinite(m)||m<=0||!isfinite(r)||r<=0||!isfinite(dt)||dt<=0)return o;
 for(int k=0;k<3;k++)if(!isfinite(x[k])||!isfinite(v[k]))return o;
 double g0=x[1]-x[0]-2*r,g1=x[2]-x[1]-2*r,a0=v[0]-v[1],a1=v[1]-v[2];
 if(g0<=0||g1<=0||a0<=0||a1<=0)return o;
 double t0=g0/a0,t1=g1/a1;
 if(t0>dt||t1>dt||fabs(t0-t1)>1e-11*fmax(1.,dt)||
    fabs(a0-a1)>1e-10*fmax(1.,fabs(a0)))return o;
 /* From equal-mass conservation and zero normal relative velocity reversal:
    simultaneous impulses on the two contacts each equal 2*m*a0. */
 double impulse=2*m*a0;
 o.v[0]=v[0]-impulse/m;o.v[1]=v[1];o.v[2]=v[2]+impulse/m;
 for(int k=0;k<3;k++)o.x[k]=x[k]+v[k]*t0+o.v[k]*(dt-t0);
 o.valid=1;return o;
}
static uint32_t state=0x4d413033u;
static double rand01(void){state^=state<<13;state^=state>>17;state^=state<<5;return (state&0xffffffu)/16777216.;}
int main(void){
 for(int k=0;k<20000;k++){
  double m=.1+100*rand01(),r=.1+.5*rand01(),s=.01+100*rand01();
  double center=-100+200*rand01(),base=-5+10*rand01();
  double t=.0001+.01*rand01(),dt=t+.01+rand01();
  double x[]={center-2*r-s*t,center,center+2*r+s*t},v[]={base+s,base,base-s};
  Ref z=oracle(x,v,m,r,dt);if(!z.valid)return 1;
  double p0=m*(v[0]+v[1]+v[2]),p1=m*(z.v[0]+z.v[1]+z.v[2]);
  double e0=m*(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
  double e1=m*(z.v[0]*z.v[0]+z.v[1]*z.v[1]+z.v[2]*z.v[2]);
  if(fabs(p0-p1)>1e-8*fmax(1,fabs(p0))||
     fabs(e0-e1)>1e-8*fmax(1,fabs(e0))||
     z.v[0]>z.v[1]+1e-9||z.v[1]>z.v[2]+1e-9)return 2;
 }
 {double x[]={-3,0,4},v[]={2,0,-2};if(oracle(x,v,1,1,1).valid)return 3;}
 {double x[]={-3,0,3},v[]={1,0,-1};if(oracle(x,v,1,1,.1).valid)return 4;}
 {double x[]={-1,0,1},v[]={1,0,-1};if(oracle(x,v,1,1,1).valid)return 5;}
 puts("MA03F reference: 20000 randomized symmetric cases + 3 rejection controls PASS");return 0;
}
