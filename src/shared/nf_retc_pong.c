/* SPDX-License-Identifier: GPL-3.0-or-later
 * H63 deterministic XZ Pong: real A4 contact impulses, single staging owner.
 * Restricted moving paddle slabs, fixed arena walls, and bounded events.
 * NOT a substitute for full A6/B2/A5/B3 1.8C.x authority or wall-riding.
 */
#include "nf_retc_pong.h"
#include <math.h>
#include <string.h>
#include <limits.h>
#define PADDLE_HALF_X .28f
#define PADDLE_MAX_Z (NF_PONG_COURT_Z-NF_PONG_PADDLE_HALF_Z)
#define PADDLE_SPEED 7.2f
#define PONG_MAX_EVENTS 8u
#define EPS 1.e-5f
typedef enum {P_NONE=0,P_LEFT,P_RIGHT,P_NORTH,P_SOUTH,P_GOAL_LEFT,P_GOAL_RIGHT} EventKind;
typedef struct {float t;EventKind kind;bool tied;} Event;
static float clampf(float v,float a,float b){return v<a?a:(v>b?b:v);}
static void try_event(Event *e,float t,EventKind kind,float limit){
 if(!isfinite(t)||t< -EPS||t>limit+EPS)return;
 if(t<0.f)t=0.f;
 if(e->kind==P_NONE||t<e->t-EPS){e->t=t;e->kind=kind;e->tied=false;}
 else if(fabsf(t-e->t)<=EPS&&kind!=e->kind){e->tied=true;}
}
void nf_pong_init(NfPongWorld *w){
 if(!w)return;
 *w=(NfPongWorld){0};
 w->revision=1u;
 w->ball=(Nf18a4Body){.id=1u,.center={0.f,.65f,0.f},
   .velocity={6.25f,0.f,1.35f},.radius=NF_PONG_BALL_RADIUS,
   .inverse_mass=1.f};
 w->serve_direction=1;
}
static bool valid(const NfPongWorld *w){
 return w&&nf18a4_body_valid(&w->ball)&&w->ball.id==1u&&
   fabsf(w->ball.radius-NF_PONG_BALL_RADIUS)<EPS&&
   fabsf(w->ball.center.y-.65f)<EPS&&
   fabsf(w->ball.center.x)<NF_PONG_COURT_X+EPS&&
   fabsf(w->ball.center.z)<=NF_PONG_COURT_Z+EPS&&
   fabsf(w->ball.velocity.y)<EPS&&
   isfinite(w->paddle_z[0])&&isfinite(w->paddle_z[1])&&
   fabsf(w->paddle_z[0])<=PADDLE_MAX_Z+EPS&&
   fabsf(w->paddle_z[1])<=PADDLE_MAX_Z+EPS&&
   (w->serve_direction==1||w->serve_direction==-1);
}
static Event discover(const NfPongWorld *w,float remaining){
 Event e={0};e.t=remaining+1.f;
 float x=w->ball.center.x,z=w->ball.center.z;
 float vx=w->ball.velocity.x,vz=w->ball.velocity.z;
 if(vx< -EPS){
  float t=(-NF_PONG_PADDLE_X+PADDLE_HALF_X+NF_PONG_BALL_RADIUS-x)/vx;
  if(t>=-EPS&&t<=remaining+EPS&&
     fabsf(z+vz*t-w->paddle_z[0])<=NF_PONG_PADDLE_HALF_Z+NF_PONG_BALL_RADIUS)
    try_event(&e,t,P_LEFT,remaining);
  try_event(&e,(-NF_PONG_COURT_X-x)/vx,P_GOAL_LEFT,remaining);
 }else if(vx>EPS){
  float t=(NF_PONG_PADDLE_X-PADDLE_HALF_X-NF_PONG_BALL_RADIUS-x)/vx;
  if(t>=-EPS&&t<=remaining+EPS&&
     fabsf(z+vz*t-w->paddle_z[1])<=NF_PONG_PADDLE_HALF_Z+NF_PONG_BALL_RADIUS)
    try_event(&e,t,P_RIGHT,remaining);
  try_event(&e,(NF_PONG_COURT_X-x)/vx,P_GOAL_RIGHT,remaining);
 }
 if(vz>EPS)try_event(&e,(NF_PONG_COURT_Z-z)/vz,P_NORTH,remaining);
 if(vz< -EPS)try_event(&e,(-NF_PONG_COURT_Z-z)/vz,P_SOUTH,remaining);
 return e;
}
static bool impulse(NfPongWorld *w,EventKind e,uint32_t tick){
 NfVec3 n={0};Nf18a4Body solid={0};
 Nf18a4Constraint c={0};
 if(e==P_LEFT||e==P_RIGHT){
  unsigned index=e==P_LEFT?0u:1u;
  n.x=e==P_LEFT?1.f:-1.f;
  solid.id=100u+index;solid.center=(NfVec3){
    e==P_LEFT?-NF_PONG_PADDLE_X:NF_PONG_PADDLE_X,.65f,w->paddle_z[index]};
  solid.velocity=(NfVec3){0,0,w->paddle_velocity_z[index]};
  solid.box_half=(NfVec3){PADDLE_HALF_X,.55f,NF_PONG_PADDLE_HALF_Z};
  c.friction=.32f;
 }else if(e==P_NORTH||e==P_SOUTH){
  n.z=e==P_NORTH?-1.f:1.f;
  solid.id=e==P_NORTH?201u:202u;
  solid.center=(NfVec3){0,.65f,e==P_NORTH?NF_PONG_COURT_Z:-NF_PONG_COURT_Z};
 }else return false;
 c.body_a=w->ball.id;c.body_b=solid.id;
 c.feature_id=(uint32_t)e;
 c.normal_b_to_a=n;
 c.contact_point=(NfVec3){w->ball.center.x-n.x*w->ball.radius,
                           .65f,w->ball.center.z-n.z*w->ball.radius};
 c.restitution=1.f;
 Nf18a4Receipt r=nf18a4_apply_contact(&w->ball,&solid,c,
                   NF18A4_FRICTION_FIXED6,tick,w->revision);
 if(r.status!=NF18A4_ACCEPTED||r.impulses_applied!=1u)return false;
 if(e==P_LEFT||e==P_RIGHT)w->paddle_hits++;
 else w->wall_hits++;
 return true;
}
NfPongStatus nf_pong_stage(const NfPongWorld *w,const NfPongInput *in,
 float dt,NfPongCandidate *out){
 if(!out)return NF_PONG_INVALID;
 *out=(NfPongCandidate){0};
 if(!valid(w)||!in||w->tick==UINT32_MAX||w->revision==UINT32_MAX||
    !isfinite(dt)||dt<=0.f||dt>1.f/20.f)return NF_PONG_INVALID;
 for(unsigned i=0;i<2u;i++)
  if(!isfinite(in->paddle_axis[i])||fabsf(in->paddle_axis[i])>1.f)
   return NF_PONG_INVALID;
 NfPongWorld next=*w;
 for(unsigned i=0;i<2u;i++){
  float previous=next.paddle_z[i];
  next.paddle_z[i]=clampf(previous+in->paddle_axis[i]*PADDLE_SPEED*dt,
                          -PADDLE_MAX_Z,PADDLE_MAX_Z);
  next.paddle_velocity_z[i]=(next.paddle_z[i]-previous)/dt;
 }
 if(next.serve_delay){
  next.serve_delay--;
  if(next.serve_delay==0u)
   next.ball.velocity=(NfVec3){6.25f*(float)next.serve_direction,0,
     next.serve_direction>0?1.35f:-1.35f};
 }else{
  float left=dt;
  for(unsigned k=0;k<PONG_MAX_EVENTS&&left>1.e-7f;k++){
   Event e=discover(&next,left);
   if(e.tied)return NF_PONG_PENDING; /* unsupported same-TOI manifold */
   if(e.kind==P_NONE){
    next.ball.center.x+=next.ball.velocity.x*left;
    next.ball.center.z+=next.ball.velocity.z*left;
    left=0.f;break;
   }
   next.ball.center.x+=next.ball.velocity.x*e.t;
   next.ball.center.z+=next.ball.velocity.z*e.t;
   left-=e.t;
   if(left<0.f)left=0.f;
   if(e.kind==P_GOAL_LEFT||e.kind==P_GOAL_RIGHT){
    unsigned scorer=e.kind==P_GOAL_LEFT?1u:0u;
    if(next.score[scorer]==UINT32_MAX)return NF_PONG_INVALID;
    next.score[scorer]++;
    next.serve_direction=e.kind==P_GOAL_LEFT?-1:1;
    next.ball.center=(NfVec3){0,.65f,0};
    next.ball.velocity=(NfVec3){0};
    next.serve_delay=36u;
    out->scored=1u;left=0.f;break;
   }
   if(!impulse(&next,e,w->tick+1u))return NF_PONG_PENDING;
   out->impulses_applied++;
  }
  if(left>1.e-7f)return NF_PONG_PENDING; /* too many contacts/tick */
 }
 if(!valid(&next))return NF_PONG_INVALID;
 next.tick++;next.revision++;
 out->from_tick=w->tick;out->from_revision=w->revision;
 out->next=next;
 return NF_PONG_OK;
}
NfPongStatus nf_pong_publish(NfPongWorld *w,const NfPongCandidate *c){
 if(!w||!c||!valid(w)||!valid(&c->next)||w->tick==UINT32_MAX||
    w->revision==UINT32_MAX||w->tick!=c->from_tick||
    w->revision!=c->from_revision||
    c->next.tick!=w->tick+1u||c->next.revision!=w->revision+1u||
    c->next.score[0]<w->score[0]||c->next.score[1]<w->score[1]||
    c->next.paddle_hits<w->paddle_hits||c->next.wall_hits<w->wall_hits)
    return NF_PONG_STALE;
 *w=c->next;
 return NF_PONG_OK;
}
