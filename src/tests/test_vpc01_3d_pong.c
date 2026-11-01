/* SPDX-License-Identifier: GPL-3.0-or-later
 * H63 canonical, randomized, high-speed, boundary, negative/held-out Pong.
 */
#include "nf_retc_pong.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
static unsigned checks;
#define CK(x) do{if(!(x)){fprintf(stderr,"H63 FAIL line %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static float clampf(float x,float a,float b){return x<a?a:(x>b?b:x);}
static float bot(const NfPongWorld *w,unsigned i){
 float target=0.f,v=w->ball.velocity.x;
 if((i==0u&&v<0.f)||(i==1u&&v>0.f)){
  float x=i==0u?-NF_PONG_PADDLE_X:NF_PONG_PADDLE_X;
  float t=fabsf(v)>1.e-5f?clampf((x-w->ball.center.x)/v,0.f,1.5f):0.f;
  target=w->ball.center.z+w->ball.velocity.z*t;
  for(unsigned k=0;k<8u&&fabsf(target)>NF_PONG_COURT_Z;k++)
   target=target>NF_PONG_COURT_Z?
     2.f*NF_PONG_COURT_Z-target:-2.f*NF_PONG_COURT_Z-target;
 }
 float d=target-w->paddle_z[i];
 return fabsf(d)<.06f?0.f:clampf(3.f*d,-1.f,1.f);
}
static int rally(unsigned seed){
 NfPongWorld a={0},b={0};nf_pong_init(&a);nf_pong_init(&b);
 float drift=(float)((int)(seed%21u)-10)*.08f;
 a.ball.velocity.z+=drift;b.ball.velocity.z+=drift;
 for(unsigned tick=0;tick<480u;tick++){
  NfPongInput in={{bot(&a,0u),bot(&a,1u)}};
  const NfPongWorld frozen=a;
  NfPongCandidate ca={0},cb={0};
  CK(nf_pong_stage(&a,&in,1.f/60.f,&ca)==NF_PONG_OK);
  CK(nf_pong_stage(&b,&in,1.f/60.f,&cb)==NF_PONG_OK);
  CK(memcmp(&ca,&cb,sizeof ca)==0);
  CK(memcmp(&frozen,&a,sizeof a)==0);
  CK(nf_pong_publish(&a,&ca)==NF_PONG_OK);
  CK(nf_pong_publish(&b,&cb)==NF_PONG_OK);
  CK(memcmp(&a,&b,sizeof a)==0);
 }
 CK(a.tick==480u&&a.revision==481u);
 CK(a.paddle_hits>0u);
 return 0;
}
static int high_speed(void){
 NfPongWorld w={0};nf_pong_init(&w);
 w.ball.velocity=(NfVec3){1000.f,0,0};
 NfPongInput in={0};NfPongCandidate c={0};
 CK(nf_pong_stage(&w,&in,1.f/60.f,&c)==NF_PONG_OK);
 CK(c.impulses_applied>=1u);
 CK(c.next.paddle_hits>=1u);
 CK(c.next.score[0]==0u&&c.next.score[1]==0u);
 CK(c.next.ball.velocity.x<0.f);
 CK(nf_pong_publish(&w,&c)==NF_PONG_OK);
 CK(w.ball.center.x<6.75f);
 return 0;
}
static int walls_and_scoring(void){
 NfPongWorld w={0};nf_pong_init(&w);
 w.ball.center.z=4.14f;
 w.ball.velocity=(NfVec3){0,0,30.f};
 NfPongInput in={0};NfPongCandidate c={0};
 CK(nf_pong_stage(&w,&in,1.f/60.f,&c)==NF_PONG_OK);
 CK(c.next.wall_hits==1u&&c.next.ball.velocity.z<0.f);
 CK(nf_pong_publish(&w,&c)==NF_PONG_OK);
 nf_pong_init(&w);
 w.ball.center.z=3.25f;
 w.ball.velocity=(NfVec3){1100.f,0,0};
 w.paddle_z[1]=-3.f;
 CK(nf_pong_stage(&w,&in,1.f/60.f,&c)==NF_PONG_OK);
 CK(c.scored==1u&&c.next.score[0]==1u&&c.next.score[1]==0u);
 CK(c.next.serve_delay==36u);
 CK(nf_pong_publish(&w,&c)==NF_PONG_OK);
 for(unsigned i=0u;i<36u;i++){
  CK(nf_pong_stage(&w,&in,1.f/60.f,&c)==NF_PONG_OK);
  CK(nf_pong_publish(&w,&c)==NF_PONG_OK);
 }
 CK(w.serve_delay==0u&&w.ball.velocity.x>0.f);
 return 0;
}
static int bad_inputs(void){
 NfPongWorld w={0};nf_pong_init(&w);
 NfPongWorld before=w;
 NfPongCandidate c={0};
 NfPongInput bad={{NAN,0}};
 CK(nf_pong_stage(&w,&bad,1.f/60.f,&c)==NF_PONG_INVALID);
 bad.paddle_axis[0]=2.f;
 CK(nf_pong_stage(&w,&bad,1.f/60.f,&c)==NF_PONG_INVALID);
 bad=(NfPongInput){0};
 CK(nf_pong_stage(&w,&bad,.2f,&c)==NF_PONG_INVALID);
 CK(nf_pong_stage(&w,&bad,1.f/60.f,&c)==NF_PONG_OK);
 CK(memcmp(&before,&w,sizeof w)==0);
 CK(nf_pong_publish(&w,&c)==NF_PONG_OK);
 CK(nf_pong_publish(&w,&c)==NF_PONG_STALE);
 CK(nf_pong_publish(NULL,&c)==NF_PONG_STALE);
 nf_pong_init(&w);
 w.ball.center.z=0.f;
 w.ball.velocity=(NfVec3){0,0,4000.f};
 CK(nf_pong_stage(&w,&bad,1.f/60.f,&c)==NF_PONG_PENDING);
 CK(w.tick==0u&&w.wall_hits==0u);
 return 0;
}
int main(void){
 for(unsigned seed=0;seed<32u;seed++)if(rally(seed))return 1;
 if(high_speed()||walls_and_scoring()||bad_inputs())return 1;
 printf("PONG_H63 PASS: 32x480 replay ticks, A4 paddle/wall impulse, swept 1000-unit/s paddle, score/serve, pathological pending, invalid/stale; assertions=%u\n",checks);
 return 0;
}
