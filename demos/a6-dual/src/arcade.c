/* nightfall!punk v1.8A terminal/X11 two-game diagnostic; no upstream GZDoom,
   Quake, LibreQuake code or assets included. Not an engine-compatible port. */
#include "arcade.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static const char *CINDER[AH]={
"########################",
"#P..#.....#...#......e##",
"#.#.#.###.#.#.#.####.#.#",
"#.#...#.....#.....#....#",
"#.###.#.#####.###.###..#",
"#...#...#...#...#...#..#",
"###.#####.#.###.#.#.##.#",
"#...#.....#.....#.#....#",
"#.#.#.#####.#####.#.##.#",
"#.#...#...K.....#...#..#",
"#.#####.#.###.#.###.#..#",
"#...#...#...#.#...#....#",
"###.#.###.#.#.###.####.#",
"#...#.....#...D.....#..#",
"#.######.######.###.#..#",
"#...o....#..o....#.....#",
"########################"};
static const char *SLIP[AH]={
"########################",
"#P...#....H....#......##",
"#.#..#.###.###.#.####.##",
"#.#......#.#.....#..#..#",
"#.#.####.#.#.#####..#..#",
"#...#....#.#......#....#",
"###.#.####.#####.#.##..#",
"#....#....B.....#....#.#",
"#.##.#.#########.#.#.#.#",
"#....#...#...#...#.#...#",
"#.######.#.#.#.###.##..#",
"#...#....#.#...#.....#.#",
"###.#.####.#####.#.##..#",
"#...#..S...#.....#.....#",
"#.#####.##.#.###.#####.#",
"#...o....#...H.....o...#",
"########################"};
static const int DX[4]={0,1,0,-1};
static const int DZ[4]={-1,0,1,0};
static uint32_t randstep(uint32_t *s){uint32_t v=*s;v^=v<<13;v^=v>>17;v^=v<<5;return *s=v?v:0x12345678u;}
static int in_bounds(int x,int z){return x>=0&&x<AW&&z>=0&&z<AH;}
static char tile(const Arcade *g,int x,int z){return in_bounds(x,z)?g->cells[z][x]:'#';}
static int is_wall(char t){return t=='#';}
static int is_gate(char t){return t=='D';}
static int is_solid(const Arcade *g,int x,int z){char c=tile(g,x,z);return is_wall(c)||(is_gate(c)&&!(g->switches&1));}
static int key_id(int x,int z){return z*AW+x+1;}
static unsigned int mix(unsigned int h,unsigned int x){return (h^x)*16777619u;}
uint32_t arcade_digest(const Arcade *g){
 uint32_t h=2166136261u;h=mix(h,(unsigned)g->kind);h=mix(h,(unsigned)g->policy);h=mix(h,g->seed);h=mix(h,(unsigned)g->tick);
 const int v[]={g->x,g->z,g->score,g->remaining,g->health,g->switches,g->push_count,g->purple_count,g->trace_events};
 for(size_t j=0;j<sizeof v/sizeof *v;j++)h=mix(h,(unsigned)v[j]);
 for(int i=0;i<GHOSTS;i++){h=mix(h,(unsigned)g->ghosts[i].x);h=mix(h,(unsigned)g->ghosts[i].z);h=mix(h,(unsigned)lroundf(g->ghosts[i].energy*100));}
 for(int y=0;y<AH;y++)for(int x=0;x<AW;x++)h=mix(h,(unsigned)(unsigned char)g->cells[y][x]);
 return h;
}
const char *arcade_kind_name(GameKind g){return g==GAME_CINDER?"Cinder Circuit (Doom-inspired)":"Slipgate Circuit (LibreQuake-inspired)";}
const char *arcade_ai_name(AiPolicy p){return p==AI_CLASSIC?"baseline pursuit":"systemic bounded utility";}
void arcade_init(Arcade *g,GameKind kind,AiPolicy policy,uint32_t seed){
 memset(g,0,sizeof(*g));g->kind=kind;g->policy=policy;g->seed=seed;g->rng=seed?seed:0x9e3779b9u;g->player_energy=85.f;g->health=4;
 for(int z=0;z<AH;z++){
  const char *row=kind==GAME_CINDER?CINDER[z]:SLIP[z];
  size_t len=strlen(row);if(len!=AW){fprintf(stderr,"MAP ERROR row %d length=%zu\n",z,len);exit(3);}
  memcpy(g->cells[z],row,AW+1);
  for(int x=0;x<AW;x++){
   char c=g->cells[z][x];if(c=='P'){g->x=x;g->z=z;g->cells[z][x]=' ';}
   else if(c=='.')g->remaining++;
  }
 }
 const int starts[GHOSTS][2]={{AW-2,1},{AW-2,AH-2},{2,AH-2}};
 for(int i=0;i<GHOSTS;i++){
  Ghost *s=&g->ghosts[i];s->x=starts[i][0];s->z=starts[i][1];if(is_solid(g,s->x,s->z)){s->x=1;s->z=AH-2;}
  s->energy=45.f+(float)i*6;s->last_x=s->x;s->last_z=s->z;s->last_tick=-100;
 }
 nf18a5_history_init(&g->history,NF18A5_CUMULATIVE_DURABLE);
 nf18a5_grid_init(&g->grid,12,NF18A5_HYSTERETIC_FINE);
 g->history.peak_threshold=12.f;g->history.cumulative_threshold=30.f;g->history.duration_ticks=20;g->hash=arcade_digest(g);
}
/* This is the exact translating upright capsule-vs-AABB solver from 1.8A.3,
   used as a guard on each proposed arcade step. The grid itself stays 2D. */
static int capsule_blocked(const Arcade *g,int x,int z,int nx,int nz){
 Nf18a2ShapePolicy sh={.radius=.24f,.height=1.6f,.foot_flat_radius=.12f,.step_height=.4f,.walkable_normal_y=.7f};
 NfVec3 feet={(float)x,0.f,(float)z},step={(float)(nx-x),0.f,(float)(nz-z)};
 int minx=(x<nx?x:nx)-1,maxx=(x>nx?x:nx)+1;
 int minz=(z<nz?z:nz)-1,maxz=(z>nz?z:nz)+1;
 for(int yy=minz;yy<=maxz;yy++)for(int xx=minx;xx<=maxx;xx++){
  if(!is_solid(g,xx,yy))continue;
  Nf18aCollider box={0};box.body_id=(uint32_t)key_id(xx,yy);
  box.min=(NfVec3){xx-.5f,-.1f,yy-.5f};box.max=(NfVec3){xx+.5f,2.1f,yy+.5f};
  Nf18aContact contact={0};
  if(nf18a3_capsule_sweep(feet,sh,step,box,1.f,&contact)&&contact.kind!=NF18A_CONTACT_NONE)
   return 1;
 }
 return 0;
}
static int floor_access(const Arcade *g,int x,int z,int nx,int nz,int authorized){
 /* Simultaneous geometry G and traversal T: a switch is authorization only;
    never makes the floor plan's solid walls disappear. */
 if(!in_bounds(nx,nz)||is_solid(g,nx,nz))return 0;
 if(tile(g,nx,nz)=='H' && !authorized)return 0;
 return !capsule_blocked(g,x,z,nx,nz);
}
static int bfs_distance_known(const Arcade *g,const Ghost *observer,int sx,int sz,int tx,int tz){
 int dist[AH][AW],qx[AW*AH],qz[AW*AH],head=0,tail=0;
 for(int z=0;z<AH;z++)for(int x=0;x<AW;x++)dist[z][x]=-1;
 if(!in_bounds(sx,sz)||!in_bounds(tx,tz)||is_solid(g,tx,tz))return 999;
 if(observer && !observer->seen[tz][tx])return 999;
 dist[sz][sx]=0;qx[tail]=sx;qz[tail++]=sz;
 while(head<tail){int x=qx[head],z=qz[head++];if(x==tx&&z==tz)return dist[z][x];
  for(int d=0;d<4;d++){int xx=x+DX[d],zz=z+DZ[d];if(in_bounds(xx,zz)&&dist[zz][xx]<0&&!is_solid(g,xx,zz)&&(!observer||observer->seen[zz][xx])){
   dist[zz][xx]=dist[z][x]+1;qx[tail]=xx;qz[tail++]=zz;
  }}
 }
 return 999;
}
static int can_see(const Arcade *g,int sx,int sz,int x,int z,int radius){
 if(abs(x-sx)+abs(z-sz)>radius)return 0;
 /* Here visibility is short-range local sensing; a corridor occludes straight rays. */
 int steps=abs(x-sx)>abs(z-sz)?abs(x-sx):abs(z-sz);
 for(int i=1;i<steps;i++){
  int xx=sx+(x-sx)*i/steps,zz=sz+(z-sz)*i/steps;
  if(is_solid(g,xx,zz))return 0;
 }
 return 1;
}
static int choose_ghost(Arcade *g,int i){
 Ghost *a=&g->ghosts[i];int best=-1;float bestv=-1.e15f;
 int visible=can_see(g,a->x,a->z,g->x,g->z,g->kind==GAME_CINDER?6:7);
 /* Opponent-local grid observations; do not secretly copy the global maze
    into a situated actor's pathfinder. Physical collision remains WORLD truth. */
 if(g->policy==AI_SYSTEMIC)for(int rz=-3;rz<=3;rz++)for(int rx=-3;rx<=3;rx++){
   int px=a->x+rx,pz=a->z+rz;
   if(in_bounds(px,pz)&&can_see(g,a->x,a->z,px,pz,4))a->seen[pz][px]=1;
 }
 if(visible){a->last_x=g->x;a->last_z=g->z;a->last_tick=g->tick;}
 int remembered=g->tick-a->last_tick<=8;
 int goalx=(g->policy==AI_CLASSIC)?g->x:a->last_x;
 int goalz=(g->policy==AI_CLASSIC)?g->z:a->last_z;
 for(int d=0;d<4;d++){
  int xx=a->x+DX[d],zz=a->z+DZ[d];if(!floor_access(g,a->x,a->z,xx,zz,1))continue;
  float v=0.f;int dist=bfs_distance_known(g,g->policy==AI_SYSTEMIC?a:NULL,xx,zz,goalx,goalz);
  if(g->policy==AI_CLASSIC){v=100.f-(float)dist*8.f;}
  else {
   float pursuit=(visible?1.f:(remembered?.55f:0.f));
   v= pursuit*(100.f-(float)dist*9.f);
   /* Local frontier exploration when target is unobserved. */
   if(!remembered){int frontier=0;
    for(int n=0;n<4;n++){
      int fx=xx+DX[n],fz=zz+DZ[n];
      if(in_bounds(fx,fz)&&!a->seen[fz][fx])frontier++;
    }
    v=(float)frontier*11.f+20.f;
   }
   if(a->energy<17.f){ /* energy economy: retreat and rest when necessary */
    int sx=g->kind==GAME_CINDER?5:3,sz=AH-2;
    int retreat=bfs_distance_known(g,a,xx,zz,sx,sz);
    if(retreat<999)v=100.f-(float)retreat*7.f;
   }
   v-=(float)(tile(g,xx,zz)=='S'?8:0);
   v-= (a->energy<10.f?30.f:0.f);
  }
  v+=(float)(randstep(&g->rng)%101u)*.001f;
  if(v>bestv){bestv=v;best=d;}
 }
 if(visible)g->ghost_chases++;
 if(g->policy==AI_SYSTEMIC&&!visible&&remembered)g->stale_chases++;
 return best;
}

/* Canonical world witness is the level data, NOT the resident cache. This
   loader only reconstructs exact 1m parent cells. The capsule CCD remains
   authoritative for swept volume and never substitutes a point-free result. */
static void canonical_chunk(const Arcade *g,int cx,int cz,uint8_t data[NF18A5_CANON_VOXELS]){
 for(int iy=0;iy<4;iy++)for(int iz=0;iz<4;iz++)for(int ix=0;ix<4;ix++){
  data[(iy*4+iz)*4+ix]=(uint8_t)((iy<=1 && is_solid(g,cx*4+ix,cz*4+iz))?NF18A5_SOLID:NF18A5_FREE);
 }
}
static int canonical_query(Arcade *g,int nx,int nz){
 uint32_t tick=(uint32_t)g->tick+1;
 Nf18a5Query q=nf18a5_query(&g->grid,(float)nx+.5f,.5f,(float)nz+.5f,tick,true);
 if(q==NF18A5_Q_PENDING){
  g->pending_count++;
  uint8_t data[NF18A5_CANON_VOXELS];int cx=nx/4,cz=nz/4;
  canonical_chunk(g,cx,cz,data);
  if(g->grid.count==g->grid.capacity)nf18a5_evict(&g->grid,tick,0u);
  if(!nf18a5_load_canonical(&g->grid,cx,0,cz,data,tick))return 0;
  g->cache_loads++;
  q=nf18a5_query(&g->grid,(float)nx+.5f,.5f,(float)nz+.5f,tick,true);
 }
 return q==NF18A5_Q_FREE;
}
static void material_gate_revision(Arcade *g){
 /* Unlocking an actual door modifies world geometry. Refresh a resident
    canonical chunk as a material edit, not as a mere cache-load revision. */
 const int x=14,z=13,cx=x/4,cz=z/4;
 for(unsigned i=0;i<g->grid.count;i++){
  if(g->grid.chunks[i].x==cx&&g->grid.chunks[i].y==0&&g->grid.chunks[i].z==cz){
   uint8_t data[NF18A5_CANON_VOXELS];canonical_chunk(g,cx,cz,data);
   (void)nf18a5_load_canonical(&g->grid,cx,0,cz,data,(uint32_t)g->tick+1);
   break;
  }
 }
}

static int motor_guard(int dir){
 Nf18a4Body a={0};a.id=1;a.inverse_mass=1.f/80.f;
 Nf18a4Motor motor={.target_velocity={(float)DX[dir]*4.f,0.f,(float)DZ[dir]*4.f},.max_force=10000.f,.max_accel=100.f};
 Nf18a4MotorReceipt rec={0};
 return nf18a4_motor_drive(&a,motor,.25f,&rec)&&rec.applied_force>0.f;
}
static int move_crate(Arcade *g,int d){
 int xx=g->x+DX[d],zz=g->z+DZ[d],nx=xx+DX[d],nz=zz+DZ[d];
 if(!floor_access(g,xx,zz,nx,nz,1)||tile(g,nx,nz)=='B')return 0;
 /* Separate two-body angular/friction impulse solve from 1.8A.4;
    a successful receipt is required before changing either grid occupancy. */
 Nf18a4Body actor={0},crate={0};actor.id=1;actor.inverse_mass=1.f/80.f;
 actor.center=(NfVec3){(float)g->x,.9f,(float)g->z};
 actor.velocity=(NfVec3){(float)DX[d]*8.f,0.f,(float)DZ[d]*8.f};
 actor.inverse_inertia=(NfVec3){.012f,.012f,.012f};
 crate.id=2;crate.inverse_mass=1.f/30.f;crate.inverse_inertia=(NfVec3){.04f,.04f,.04f};
 crate.center=(NfVec3){(float)xx,.9f,(float)zz};crate.box_half=(NfVec3){.3f,.5f,.3f};
 Nf18a4Constraint c={0};c.body_a=1;c.body_b=2;c.feature_id=(uint32_t)key_id(xx,zz);
 c.normal_b_to_a=(NfVec3){(float)-DX[d],0.f,(float)-DZ[d]};
 c.contact_point=(NfVec3){g->x+DX[d]*.55f,.9f,g->z+DZ[d]*.55f};
 c.friction=.35f;
 Nf18a4Receipt rc=nf18a4_apply_contact(&actor,&crate,c,NF18A4_FRICTION_ADAPTIVE,(uint32_t)g->tick+1,1u);
 float mv=crate.velocity.x*(float)DX[d]+crate.velocity.z*(float)DZ[d];
 if(!rc.impulses_applied||mv<.01f)return 0;
 Nf18a5Sample sample={0};if(!nf18a5_receipt_sample(&rc,1,2,0.f,1u,&sample)){g->history_fail++;return 0;}
 Nf18a5History proposed=g->history;
 if(!nf18a5_history_commit(&proposed,(uint32_t)g->tick+1,(uint32_t)g->tick+2,1,&sample,1)){
  g->history_fail++;return 0;
 }
 /* One bounded two-body transaction. Graphical grid occupancy is only
    a projection, not a claim of continuous rigid-body translation. */
 g->history=proposed;g->trace_raw++;
 g->cells[nz][nx]='B';g->cells[zz][xx]=' ';
 g->push_count++;g->contact_count++;
 Nf18a6TickTrace tr=nf18a6_classify_tick((uint32_t)g->tick+1,(uint32_t)c.feature_id,
  (uint32_t)c.feature_id,true,true,1u,1u);
 if(tr.purple_present&&tr.red_proposed&&tr.blue_material_checked){
  /* Mark committed only AFTER the reciprocal impulse and history commit. */
  tr.purple_status=NF17C_PURPLE_COMMITTED;
  g->purple_count++;
 }
 return 1;
}
static void collect(Arcade *g){
 char *t=&g->cells[g->z][g->x];
 if(*t=='.'){*t=' ';g->score+=10;g->remaining--;g->player_energy=fminf(100.f,g->player_energy+1.25f);}
 else if(*t=='o'){*t=' ';g->score+=50;g->player_energy=fminf(100.f,g->player_energy+20.f);for(int i=0;i<GHOSTS;i++)g->ghosts[i].stun=4;}
 else if(*t=='K'){*t=' ';g->switches|=1;g->score+=35;material_gate_revision(g);}
 else if(*t=='S'){*t=' ';g->switches|=2;g->score+=20;}
 else if(*t=='e'){*t=' ';g->score+=75;}
}
static int step_player(Arcade *g,int d){
 if(d<0||d>=4)return 0;
 int nx=g->x+DX[d],nz=g->z+DZ[d];
 int geom= floor_access(g,g->x,g->z,nx,nz,1);
 int allowed=(tile(g,nx,nz)!='D'||(g->switches&1))&&(tile(g,nx,nz)!='H'||(g->switches&2));
 /* Geometry and contract independent: the closed door is a material obstacle,
    not erased until key state gives authority and updates effective geometry. */
 if(tile(g,nx,nz)=='D' && (g->switches&1))geom=1;
 if(!geom||!allowed){g->gate_rejects+=!allowed;g->blocked_moves++;g->blue_count++;return 0;}
 if(!canonical_query(g,nx,nz)){g->blocked_moves++;g->blue_count++;return 0;}
 if(!motor_guard(d)){g->invalid_moves++;return 0;}
 if(tile(g,nx,nz)=='B'&&!move_crate(g,d)){g->blocked_moves++;return 0;}
 g->x=nx;g->z=nz;g->player_energy=fmaxf(0.f,g->player_energy-.45f);
 if(tile(g,nx,nz)=='H')g->ladder_climbs++;
 collect(g);return 1;
}
int arcade_tick(Arcade *g,int action){
 if(g->lost||g->won)return 0;
 if(action>=0&&action<=3)step_player(g,action);
 else if(action==4){if(g->player_energy>=8.f){g->player_energy-=8.f;for(int i=0;i<GHOSTS;i++){
  Ghost *a=&g->ghosts[i];if(abs(a->x-g->x)+abs(a->z-g->z)<=2)a->stun=4;}
 }}
 for(int i=0;i<GHOSTS;i++){
  Ghost *a=&g->ghosts[i];if(a->stun){a->stun--;continue;}
  if(g->policy==AI_SYSTEMIC&&a->energy<8.f){a->energy=fminf(50.f,a->energy+3.f);g->energy_retreats++;continue;}
  int d=choose_ghost(g,i);if(d>=0){a->x+=DX[d];a->z+=DZ[d];a->energy-=g->policy==AI_SYSTEMIC?1.f:.35f;g->ghost_steps++;}
  if(g->policy==AI_SYSTEMIC)a->energy=fminf(65.f,a->energy+.55f);
  if(a->x==g->x&&a->z==g->z){g->health--;g->player_energy=fmaxf(0.f,g->player_energy-12.f);a->stun=4;}
 }
 g->tick++;
 if(g->health<=0||g->tick>=MAX_TICKS)g->lost=1;
 if(g->remaining==0){g->won=1;g->win_tick=g->tick;}
 g->trace_events=(int)g->history.generated_events;
 g->hash=arcade_digest(g);
 return !g->lost&&!g->won;
}
static int player_route_goal(const Arcade *g,int x,int z){
 char c=tile(g,x,z);
 return c=='.'||c=='o'||c=='K'||c=='S'||c=='e';
}
int arcade_bot_action(const Arcade *g){
 /* A bounded BFS player with no ghost omniscience: the player is allowed a
    learned static maze map as an experimental control; dynamic enemy locations
    are only consulted at local Manhattan radius <=3. */
 int dist[AH][AW],first[AH][AW],qx[AW*AH],qz[AW*AH],h=0,t=0;
 for(int z=0;z<AH;z++)for(int x=0;x<AW;x++){dist[z][x]=-1;first[z][x]=4;}
 dist[g->z][g->x]=0;qx[t]=g->x;qz[t++]=g->z;
 int best=4,bscore=-100000;
 while(h<t){int x=qx[h],z=qz[h++];
  if(dist[z][x]>0&&player_route_goal(g,x,z)){
   int score=500-dist[z][x]*16;
   if(tile(g,x,z)=='K' && !(g->switches&1))score+=110;
   if(tile(g,x,z)=='S' && !(g->switches&2))score+=110;
   if(tile(g,x,z)=='o')score+=20;
   int dir=first[z][x];
   for(int i=0;i<GHOSTS;i++){
    const Ghost *gh=&g->ghosts[i];if(gh->stun)continue;
    int nx=g->x+DX[dir],nz=g->z+DZ[dir];int dd=abs(gh->x-nx)+abs(gh->z-nz);
    if(dd<=3)score-=(4-dd)*25;
   }
   if(score>bscore){bscore=score;best=dir;}
  }
  if(dist[z][x]>=35)continue;
  for(int d=0;d<4;d++){
   int nx=x+DX[d],nz=z+DZ[d];
   if(!in_bounds(nx,nz)||dist[nz][nx]>=0||is_solid(g,nx,nz))continue;
   if(tile(g,nx,nz)=='H'&&!(g->switches&2))continue;
   if(tile(g,nx,nz)=='B'){
    int ax=nx+DX[d],az=nz+DZ[d];
    if(!in_bounds(ax,az)||is_solid(g,ax,az)||tile(g,ax,az)=='B')continue;
   }
   dist[nz][nx]=dist[z][x]+1;
   first[nz][nx]=dist[z][x]==0?d:first[z][x];
   qx[t]=nx;qz[t++]=nz;
  }
 }
 return best;
}
void arcade_ascii(const Arcade *g){
 printf("\n%s | %s | tick=%d score=%d health=%d energy=%.1f pending=%d impulse=%d purple=%d\n",
  arcade_kind_name(g->kind),arcade_ai_name(g->policy),g->tick,g->score,g->health,g->player_energy,g->pending_count,g->contact_count,g->purple_count);
 for(int z=0;z<AH;z++){for(int x=0;x<AW;x++){
  char c=tile(g,x,z);for(int i=0;i<GHOSTS;i++)if(g->ghosts[i].x==x&&g->ghosts[i].z==z)c='A'+i;
  if(g->x==x&&g->z==z)c='@';
  putchar(c);
 }putchar('\n');}
}
int arcade_selftest(void){
 int failures=0;Arcade a,b;arcade_init(&a,GAME_SLIPGATE,AI_SYSTEMIC,7);
 arcade_init(&b,GAME_SLIPGATE,AI_SYSTEMIC,7);
 for(int k=0;k<50;k++){int act=arcade_bot_action(&a);int act2=arcade_bot_action(&b);
  if(act!=act2)failures++;
  arcade_tick(&a,act);arcade_tick(&b,act2);if(a.hash!=b.hash)failures++;
 }
 if(a.invalid_moves||a.history_fail)failures++;
 if(a.cache_loads==0||a.pending_count==0)failures++;
 int bx=a.x,bz=a.z;
 a.x=1;a.z=1;a.cells[1][2]='#';
 if(step_player(&a,1)||a.x!=1||a.z!=1)failures++;
 a.x=bx;a.z=bz;
 Nf18a4Body body={0};body.id=1;body.inverse_mass=.0125f;
 Nf18a4Motor m={.target_velocity={2,0,0},.max_force=2000,.max_accel=50};
 Nf18a4MotorReceipt rec={0};if(!nf18a4_motor_drive(&body,m,.05f,&rec)||body.velocity.x<=0.f)failures++;
 Arcade q;arcade_init(&q,GAME_SLIPGATE,AI_SYSTEMIC,91u);
 /* Physical reachable crate (x=10,z=7), open next tile (11,7). */
 q.x=9;q.z=7;q.tick=1;
 if(!step_player(&q,1)||q.push_count!=1||q.contact_count!=1||
    q.purple_count!=1||q.history.raw_samples!=1||q.history_fail!=0)failures++;
 /* Permit a valid empty cell on demand, but a wall must never become FREE. */
 Arcade f;arcade_init(&f,GAME_SLIPGATE,AI_SYSTEMIC,92u);
 if(!canonical_query(&f,2,1)||f.cache_loads!=1||f.pending_count!=1)failures++;
 if(canonical_query(&f,0,1))failures++;
 /* Ordinary unknown data cannot generate reciprocal Purple. */
 Nf18a6TickTrace negative=nf18a6_classify_tick(1,31,32,true,true,1,1);
 if(negative.purple_present)failures++;
 printf("SELFTEST,%d,%s\n",failures,failures?"FAIL":"PASS");return failures?1:0;
}

int arcade_scenario(const char *name){
 Arcade g;arcade_init(&g,GAME_SLIPGATE,AI_SYSTEMIC,913u);
 if(strcmp(name,"crate")==0){
  g.x=9;g.z=7;g.tick=1;
  for(int n=0;n<3;n++){
   int step=step_player(&g,1);g.tick++;
   printf("SCENARIO,crate,iteration=%d,move=%d,contact=%d,purple=%d,raw=%u,events=%u,remaining=%d\n",
    n+1,step,g.contact_count,g.purple_count,g.history.raw_samples,g.history.generated_events,g.remaining);
  }
  return g.contact_count>0 && g.purple_count==g.contact_count && !g.history_fail ? 0:1;
 }
 if(strcmp(name,"ladder")==0){
  g.x=9;g.z=1;g.tick=1;
  int denied=step_player(&g,1);
  g.switches|=2;g.tick++;
  int approved=step_player(&g,1);
  printf("SCENARIO,ladder,unauthorized_move=%d,authorized_move=%d,contract_rejections=%d,accesses=%d\n",
   denied,approved,g.gate_rejects,g.ladder_climbs);
  return !denied&&approved&&g.gate_rejects>0&&g.ladder_climbs==1?0:1;
 }
 if(strcmp(name,"pending")==0){
  int missing=nf18a5_query(&g.grid,2.5f,.5f,1.5f,1u,true)==NF18A5_Q_PENDING;
  int loaded=canonical_query(&g,2,1);
  int obstruction=canonical_query(&g,0,1);
  printf("SCENARIO,pending,initial_missing=%d,loaded_free=%d,obstacle_false_free=%d,cache_misses=%d,loads=%d\n",
   missing,loaded,obstruction,g.pending_count,g.cache_loads);
  return missing&&loaded&&!obstruction&&g.pending_count>0?0:1;
 }
 if(strcmp(name,"door")==0){
  arcade_init(&g,GAME_CINDER,AI_SYSTEMIC,93);
  g.x=13;g.z=13;g.tick=1;
  int denied=step_player(&g,1);
  g.switches|=1;material_gate_revision(&g);g.tick++;
  int approved=step_player(&g,1);
  printf("SCENARIO,door,unauthorized_move=%d,authorized_move=%d,contract_rejections=%d\n",
    denied,approved,g.gate_rejects);
  return !denied&&approved&&g.gate_rejects>0?0:1;
 }
 fprintf(stderr,"Unknown scenario: %s (crate|ladder|pending|door)\n",name);
 return 2;
}
