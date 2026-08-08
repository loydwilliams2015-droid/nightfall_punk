#define _POSIX_C_SOURCE 200809L

#include "nf_agent.h"
#include "nf_ai.h"
#include "nf_combat.h"
#include "nf_net.h"
#include "nf_prediction.h"
#include "nf_relations.h"
#include "nf_security.h"
#include "nf_semantics.h"
#include "nf_world.h"

#include <float.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NF_SERVER_CLIENTS NF_NET_MAX_PLAYERS
#define NF_HISTORY_FRAMES 64u

typedef struct NfServerClient {
    bool occupied;
    bool connected;
    void *peer;
    NfEntityId entity_id;
    uint8_t session_token[NF_NET_TOKEN_BYTES];
    uint32_t last_input_sequence;
    uint64_t current_client_tick;
    NfMoveInput current_input;
    NfCombatInput combat_input;
    uint32_t reserved_until_ms;
} NfServerClient;

typedef struct NfHistoryActor {
    NfEntityId id;
    NfVec3 position;
    bool crouched;
    bool alive;
} NfHistoryActor;

typedef struct NfHistoryFrame {
    bool valid;
    uint64_t tick;
    uint8_t count;
    NfHistoryActor actors[NF_NET_MAX_SNAPSHOT_ACTORS];
} NfHistoryFrame;

static volatile sig_atomic_t g_running = 1;
static uint32_t g_event_sequence = 1u;

static void on_signal(int sig) { (void)sig; g_running=0; }
static void sleep_ms(unsigned ms) { struct timespec ts={(time_t)(ms/1000u),(long)(ms%1000u)*1000000L}; nanosleep(&ts,NULL); }
static bool seq_newer(uint32_t a,uint32_t b) { return (int32_t)(a-b)>0; }
static NfVec3 vsub(NfVec3 a,NfVec3 b) { return (NfVec3){a.x-b.x,a.y-b.y,a.z-b.z}; }
static float vdot(NfVec3 a,NfVec3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }

static bool input_valid(const NfInputCommand *cmd) {
    return cmd!=NULL && isfinite(cmd->move.forward) && isfinite(cmd->move.strafe) && isfinite(cmd->move.yaw_radians) &&
           isfinite(cmd->combat.aim_pitch_radians) && fabsf(cmd->move.forward)<=1.01f && fabsf(cmd->move.strafe)<=1.01f &&
           fabsf(cmd->move.yaw_radians)<100000.0f && fabsf(cmd->combat.aim_pitch_radians)<=1.50f && cmd->combat.weapon_slot<NF_WEAPON_COUNT;
}

static NfServerClient *by_peer(NfServerClient clients[],void *peer) {
    for(size_t i=0;i<NF_SERVER_CLIENTS;++i) if(clients[i].occupied&&clients[i].connected&&clients[i].peer==peer) return &clients[i];
    return NULL;
}
static NfServerClient *by_token(NfServerClient clients[],const uint8_t token[NF_NET_TOKEN_BYTES],uint32_t now) {
    for(size_t i=0;i<NF_SERVER_CLIENTS;++i) if(clients[i].occupied&&!clients[i].connected&&(int32_t)(clients[i].reserved_until_ms-now)>0&&nf_security_token_equal(clients[i].session_token,token)) return &clients[i];
    return NULL;
}
static NfServerClient *free_slot(NfServerClient clients[]) {
    for(size_t i=0;i<NF_SERVER_CLIENTS;++i) if(!clients[i].occupied) return &clients[i];
    return NULL;
}
static size_t slot_index(NfServerClient clients[],NfServerClient *c) { return (size_t)(c-clients); }
static NfVec3 spawn_for_slot(size_t slot) {
    static const NfVec3 p[NF_SERVER_CLIENTS]={{-3,0.05f,-18},{3,0.05f,18},{3,0.05f,-18},{-3,0.05f,18}};
    return p[slot%NF_SERVER_CLIENTS];
}
static NfFaction faction_for_slot(size_t slot) { return slot==0u?NF_FACTION_PLAYER:NF_FACTION_TEAMMATE; }

static void reset_movement_after_respawn(NfWorld *world,NfActor *a) {
    a->movement=(NfMovementState){0}; a->movement.mode=NF_MOVE_AIR; a->movement.body_height=world->movement.stand_height;
    a->movement.eye_height=world->movement.stand_eye_height; a->movement.attached_collider=-1; a->movement.ground_collider=-1; a->movement.candidate.feature_index=-1;
}

static bool respawn_position(const NfServerClient clients[],const NfAiSystem *ai,NfEntityId id,NfVec3 *out) {
    for(size_t i=0;i<NF_SERVER_CLIENTS;++i) if(clients[i].occupied&&clients[i].entity_id==id) { *out=spawn_for_slot(i); return true; }
    return nf_ai_spawn_for(ai,id,out);
}

static void expire_reservations(NfWorld *world,NfServerClient clients[],uint32_t now) {
    for(size_t i=0;i<NF_SERVER_CLIENTS;++i) {
        NfServerClient *c=&clients[i];
        if(c->occupied&&!c->connected&&(int32_t)(now-c->reserved_until_ms)>=0) { nf_world_despawn_actor(world,c->entity_id); memset(c,0,sizeof(*c)); }
    }
}
static void send_reject(NfNetHost *net,void *peer,uint8_t reason) {
    uint8_t buf[32]; size_t n=nf_protocol_encode_reject(buf,sizeof(buf),reason); if(n) nf_net_send(net,peer,NF_NET_CHANNEL_RELIABLE,buf,n,true);
}

static void broadcast_event(NfNetHost *net,NfServerClient clients[],NfSemanticBus *semantics,NfCombatEvent event) {
    event.sequence=g_event_sequence++;
    NfSemanticAlert alert=nf_semantic_from_combat(&event); if(alert.type!=NF_SEMANTIC_NONE) nf_semantic_bus_publish(semantics,alert);
    NfCombatEventMessage msg={.event=event}; uint8_t buf[96]; size_t n=nf_protocol_encode_combat_event(buf,sizeof(buf),&msg);
    if(!n) return;
    bool reliable=event.type==NF_COMBAT_EVENT_DEATH||event.type==NF_COMBAT_EVENT_RESPAWN||event.type==NF_COMBAT_EVENT_RELOAD||event.type==NF_COMBAT_EVENT_WEAPON_SWITCH;
    for(size_t i=0;i<NF_SERVER_CLIENTS;++i) if(clients[i].occupied&&clients[i].connected) nf_net_send(net,clients[i].peer,reliable?NF_NET_CHANNEL_RELIABLE:NF_NET_CHANNEL_STATE,buf,n,reliable);
}

static void handle_hello(NfNetHost *net,NfWorld *world,NfServerClient clients[],void *peer,const uint8_t *data,size_t size) {
    NfHelloMessage hello;
    if(!nf_protocol_decode_hello(data,size,&hello)) { send_reject(net,peer,1); return; }
    uint32_t now=nf_net_now_ms(); NfServerClient *c=NULL; bool resumed=false;
    if(!nf_security_token_is_zero(hello.resume_token)) { c=by_token(clients,hello.resume_token,now); resumed=c!=NULL; }
    if(c==NULL) c=free_slot(clients);
    if(c==NULL) { send_reject(net,peer,2); return; }
    if(!resumed) {
        size_t idx=slot_index(clients,c); memset(c,0,sizeof(*c)); c->occupied=true; c->entity_id=nf_world_spawn_actor(world,faction_for_slot(idx),spawn_for_slot(idx));
        uint8_t server_nonce[NF_NET_NONCE_BYTES]; nf_security_random(server_nonce,sizeof(server_nonce)); nf_security_derive_token(hello.client_nonce,server_nonce,c->session_token);
    }
    c->connected=true; c->peer=peer; c->reserved_until_ms=0;
    NfWelcomeMessage welcome={.entity_id=c->entity_id,.server_tick=world->tick,.world_seed=world->seed,.strong_crypto=nf_security_is_strong()};
    memcpy(welcome.session_token,c->session_token,NF_NET_TOKEN_BYTES); uint8_t buf[128]; size_t n=nf_protocol_encode_welcome(buf,sizeof(buf),&welcome);
    nf_net_send(net,peer,NF_NET_CHANNEL_RELIABLE,buf,n,true);
    const NfActor *a=nf_world_find_actor_const(world,c->entity_id);
    printf("[server] %s entity=%u faction=%u actors=%zu\n",resumed?"resumed":"welcome",c->entity_id,a?(unsigned)a->faction:0u,nf_world_active_actor_count(world));
}

static void handle_input(NfServerClient *c,const uint8_t *data,size_t size) {
    NfInputBundle b;
    if(c==NULL||!nf_protocol_decode_input(data,size,&b)||!nf_security_token_equal(c->session_token,b.session_token)) return;
    bool jump=false,fire_pressed=false,reload_pressed=false; uint8_t weapon_slot=0;
    for(uint8_t i=0;i<b.count;++i) {
        NfInputCommand *cmd=&b.commands[i];
        if(!input_valid(cmd)||!seq_newer(cmd->sequence,c->last_input_sequence)) continue;
        jump=jump||cmd->move.jump_pressed; fire_pressed=fire_pressed||cmd->combat.fire_pressed; reload_pressed=reload_pressed||cmd->combat.reload_pressed;
        if(cmd->combat.weapon_slot) weapon_slot=cmd->combat.weapon_slot;
        c->current_input=cmd->move; c->combat_input=cmd->combat; c->current_client_tick=cmd->client_tick; c->last_input_sequence=cmd->sequence;
    }
    if(jump) c->current_input.jump_pressed=true;
    if(fire_pressed) c->combat_input.fire_pressed=true;
    if(reload_pressed) c->combat_input.reload_pressed=true;
    if(weapon_slot) c->combat_input.weapon_slot=weapon_slot;
}

static void send_snapshot(NfNetHost *net,const NfWorld *world,NfServerClient *recipient) {
    NfSnapshotMessage s={.server_tick=world->tick,.acknowledged_input=recipient->last_input_sequence};
    for(size_t i=0;i<NF_MAX_ENTITIES&&s.actor_count<NF_NET_MAX_SNAPSHOT_ACTORS;++i) { const NfActor *a=&world->actors[i]; if(a->active) nf_actor_to_net_state(a,&s.actors[s.actor_count++]); }
    uint8_t buf[NF_NET_MAX_PACKET_BYTES]; size_t n=nf_protocol_encode_snapshot(buf,sizeof(buf),&s); if(n) nf_net_send(net,recipient->peer,NF_NET_CHANNEL_STATE,buf,n,false);
}

static void history_record(NfHistoryFrame history[],const NfWorld *world) {
    NfHistoryFrame *f=&history[world->tick%NF_HISTORY_FRAMES]; memset(f,0,sizeof(*f)); f->valid=true; f->tick=world->tick;
    for(size_t i=0;i<NF_MAX_ENTITIES&&f->count<NF_NET_MAX_SNAPSHOT_ACTORS;++i) {
        const NfActor *a=&world->actors[i]; if(!a->active) continue;
        f->actors[f->count++]=(NfHistoryActor){.id=a->id,.position=a->transform.position,.crouched=a->movement.crouched,.alive=a->combat.alive};
    }
}
static const NfHistoryFrame *history_find(NfHistoryFrame history[],uint64_t desired,uint64_t now) {
    uint64_t minimum=now>NF_NET_MAX_REWIND_TICKS?now-NF_NET_MAX_REWIND_TICKS:0; if(desired<minimum)desired=minimum; if(desired>now)desired=now;
    const NfHistoryFrame *best=NULL;
    for(size_t i=0;i<NF_HISTORY_FRAMES;++i) { const NfHistoryFrame *f=&history[i]; if(!f->valid||f->tick>desired)continue; if(best==NULL||f->tick>best->tick)best=f; }
    return best;
}

static bool ray_aabb(NfVec3 o,NfVec3 d,NfVec3 mn,NfVec3 mx,float *distance) {
    float tmin=0.0f,tmax=FLT_MAX; const float ov[3]={o.x,o.y,o.z},dv[3]={d.x,d.y,d.z},av[3]={mn.x,mn.y,mn.z},bv[3]={mx.x,mx.y,mx.z};
    for(int i=0;i<3;++i) {
        if(fabsf(dv[i])<1e-6f){if(ov[i]<av[i]||ov[i]>bv[i])return false;continue;}
        float t1=(av[i]-ov[i])/dv[i],t2=(bv[i]-ov[i])/dv[i];
        if(t1>t2){float tmp=t1;t1=t2;t2=tmp;}
        if(t1>tmin)tmin=t1;
        if(t2<tmax)tmax=t2;
        if(tmin>tmax)return false;
    }
    if(tmax<0.0f) return false;
    if(distance) *distance=tmin>=0.0f?tmin:tmax;
    return true;
}
static bool ray_sphere(NfVec3 o,NfVec3 d,NfVec3 center,float radius,float *distance) {
    NfVec3 oc=vsub(o,center); float b=vdot(oc,d),c=vdot(oc,oc)-radius*radius,disc=b*b-c; if(disc<0.0f)return false; float s=sqrtf(disc),t=-b-s; if(t<0.0f)t=-b+s; if(t<0.0f)return false; if(distance)*distance=t; return true;
}
static float world_block_distance(const NfWorld *world,NfVec3 origin,NfVec3 dir,float range) {
    float nearest=range;
    for(size_t i=0;i<world->collider_count;++i) { const NfCollider *c=&world->colliders[i]; if(c->kind==NF_COLLIDER_LADDER)continue; float t; if(ray_aabb(origin,dir,c->min,c->max,&t)&&t>0.01f&&t<nearest)nearest=t; }
    return nearest;
}

static bool hitscan_target(const NfWorld *world,const NfHistoryFrame *frame,NfEntityId shooter,NfFaction shooter_faction,NfVec3 origin,NfVec3 dir,float range,NfRelationship rival_relation,bool friendly_fire,NfEntityId *target_out,NfHitZone *zone_out,float *distance_out) {
    float nearest=world_block_distance(world,origin,dir,range); NfEntityId target=0; NfHitZone zone=NF_HIT_NONE;
    if(frame==NULL) return false;
    for(uint8_t i=0;i<frame->count;++i) {
        const NfHistoryActor *h=&frame->actors[i]; if(h->id==shooter||!h->alive)continue;
        const NfActor *current=nf_world_find_actor_const(world,h->id); if(current==NULL)continue;
        NfRelationship relation=nf_relation_between(shooter_faction,current->faction,rival_relation); if(!nf_relation_can_damage(relation,friendly_fire))continue;
        float height=h->crouched?world->movement.crouch_height:world->movement.stand_height; float t_head=FLT_MAX,t_body=FLT_MAX;
        NfVec3 head={h->position.x,h->position.y+height-0.20f,h->position.z}; bool head_hit=ray_sphere(origin,dir,head,0.23f,&t_head);
        NfVec3 mn={h->position.x-world->movement.radius,h->position.y,h->position.z-world->movement.radius}; NfVec3 mx={h->position.x+world->movement.radius,h->position.y+height*0.78f,h->position.z+world->movement.radius};
        bool body_hit=ray_aabb(origin,dir,mn,mx,&t_body); if(head_hit&&t_head<nearest){nearest=t_head;target=h->id;zone=NF_HIT_HEAD;} if(body_hit&&t_body<nearest){nearest=t_body;target=h->id;zone=NF_HIT_BODY;}
    }
    if(target==0) return false;
    *target_out=target; *zone_out=zone; *distance_out=nearest; return true;
}

static void process_combat_control(NfNetHost *net,NfWorld *world,NfServerClient clients[],NfSemanticBus *semantics,const NfControlFrame *control,NfHistoryFrame history[],NfRelationship rival_relation,bool friendly_fire) {
    NfActor *actor=nf_world_find_actor(world,control->actor); if(actor==NULL||!actor->combat.alive)return;
    if((control->move.sprint_held||control->move.interact_held)&&actor->combat.state==NF_WEAPON_RELOADING) nf_combat_cancel_reload(actor);
    if(control->combat.weapon_slot>0u&&control->combat.weapon_slot<NF_WEAPON_COUNT&&nf_combat_select_weapon(actor,(NfWeaponId)control->combat.weapon_slot)) {
        NfCombatEvent e={.server_tick=world->tick,.type=NF_COMBAT_EVENT_WEAPON_SWITCH,.source=actor->id,.weapon=(NfWeaponId)control->combat.weapon_slot,.position=actor->transform.position}; broadcast_event(net,clients,semantics,e);
    }
    if(control->combat.reload_pressed&&nf_combat_start_reload(actor)) { NfCombatEvent e={.server_tick=world->tick,.type=NF_COMBAT_EVENT_RELOAD,.source=actor->id,.weapon=actor->combat.weapon,.position=actor->transform.position}; broadcast_event(net,clients,semantics,e); }
    NfCombatEvent fire_event={0};
    if(nf_combat_try_fire(actor,&control->combat,control->sequence,world->tick,&fire_event)) {
        broadcast_event(net,clients,semantics,fire_event); const NfWeaponSpec *spec=nf_weapon_spec(actor->combat.weapon); float yaw=control->move.yaw_radians,pitch=control->combat.aim_pitch_radians,cp=cosf(pitch);
        NfVec3 origin={actor->transform.position.x,actor->transform.position.y+actor->movement.eye_height,actor->transform.position.z}; NfVec3 dir={cp*sinf(yaw),sinf(pitch),cp*cosf(yaw)};
        const NfHistoryFrame *frame=history_find(history,control->perceived_tick,world->tick); NfEntityId target_id=0; NfHitZone zone=NF_HIT_NONE; float distance=0.0f;
        if(hitscan_target(world,frame,actor->id,actor->faction,origin,dir,spec->range,rival_relation,friendly_fire,&target_id,&zone,&distance)) {
            (void)distance; NfActor *target=nf_world_find_actor(world,target_id); float damage=spec->damage*(zone==NF_HIT_HEAD?spec->head_multiplier:1.0f); NfCombatEvent hit_event={0};
            if(nf_combat_apply_damage(target,actor->id,actor->combat.weapon,zone,damage,world->tick,&hit_event)) {
                uint64_t rewind=frame&&world->tick>frame->tick?world->tick-frame->tick:0; uint64_t rewind_ms=(rewind*1000u)/NF_TICK_RATE; if(rewind_ms>65535u)rewind_ms=65535u; hit_event.rewind_ms=(uint16_t)rewind_ms; broadcast_event(net,clients,semantics,hit_event);
            }
        }
    }
}

static void process_respawns(NfNetHost *net,NfWorld *world,NfServerClient clients[],NfAiSystem *ai,NfSemanticBus *semantics) {
    for(size_t i=0;i<NF_MAX_ENTITIES;++i) {
        NfActor *a=&world->actors[i]; if(!a->active||a->combat.alive||a->combat.respawn_timer>0.0f)continue; NfVec3 spawn; if(!respawn_position(clients,ai,a->id,&spawn))continue;
        NfCombatEvent e={0}; nf_combat_respawn(a,spawn,world->tick,&e); reset_movement_after_respawn(world,a); nf_ai_on_respawn(ai,a->id); broadcast_event(net,clients,semantics,e);
    }
}

static NfControlFrame client_control(const NfServerClient *c) {
    return (NfControlFrame){.actor=c->entity_id,.source=NF_CONTROL_NETWORK,.move=c->current_input,.combat=c->combat_input,.perceived_tick=c->current_client_tick,.sequence=c->last_input_sequence};
}

int main(int argc,char **argv) {
    uint16_t port=NF_NET_DEFAULT_PORT; uint32_t sim_latency=0,sim_jitter=0; float sim_loss=0.0f; double duration=0.0; bool friendly_fire=false; size_t ai_count=4u; NfRelationship rival_relation=NF_RELATION_HOSTILE;
    for(int i=1;i<argc;++i) {
        if(strcmp(argv[i],"--port")==0&&i+1<argc)port=(uint16_t)atoi(argv[++i]);
        else if(strcmp(argv[i],"--sim-latency")==0&&i+1<argc)sim_latency=(uint32_t)atoi(argv[++i]);
        else if(strcmp(argv[i],"--sim-jitter")==0&&i+1<argc)sim_jitter=(uint32_t)atoi(argv[++i]);
        else if(strcmp(argv[i],"--sim-loss")==0&&i+1<argc)sim_loss=(float)atof(argv[++i]);
        else if(strcmp(argv[i],"--duration")==0&&i+1<argc)duration=atof(argv[++i]);
        else if(strcmp(argv[i],"--ai-count")==0&&i+1<argc){long v=strtol(argv[++i],NULL,10);ai_count=v<0?0u:(size_t)v;if(ai_count>NF_AI_MAX_AGENTS)ai_count=NF_AI_MAX_AGENTS;}
        else if(strcmp(argv[i],"--rival-truce")==0)rival_relation=NF_RELATION_TRUCE;
        else if(strcmp(argv[i],"--friendly-fire")==0)friendly_fire=true;
    }
    signal(SIGINT,on_signal); signal(SIGTERM,on_signal);
    if(!nf_security_init()||!nf_net_global_init()) { fprintf(stderr,"nightfall: network/security init failed\n"); return 1; }
    NfNetHost net; if(!nf_net_server_open(&net,port,NF_SERVER_CLIENTS)) { fprintf(stderr,"nightfall: could not open UDP port %u\n",port); nf_net_global_shutdown(); return 1; }
    nf_net_set_simulation(&net,sim_latency,sim_jitter,sim_loss);
    NfWorld world; nf_world_init(&world,20260807u); nf_world_build_movement_lab(&world); NfServerClient clients[NF_SERVER_CLIENTS]={0}; NfHistoryFrame history[NF_HISTORY_FRAMES]={0}; NfSemanticBus semantics; nf_semantic_bus_init(&semantics); NfAiSystem ai; nf_ai_init(&ai,&world,ai_count,world.seed^0xA105u); nf_ai_set_rival_relationship(&ai,rival_relation);
    history_record(history,&world);
    printf("nightfall!punk dedicated server v0.5 agent intelligence\n");
    printf("port=%u tick=%u snapshot=%u max_players=%u ai=%zu relation=%s crypto=%s friendly_fire=%s sim=%ums +/- %ums %.1f%% loss\n",port,NF_TICK_RATE,NF_NET_SNAPSHOT_HZ,NF_NET_MAX_PLAYERS,ai.count,nf_relationship_name(rival_relation),nf_security_is_strong()?"libsodium":"scaffold",friendly_fire?"on":"off",sim_latency,sim_jitter,sim_loss);
    printf("[ai] semantic perception + memory + utility + cover affordances + squad reports enabled\n");
    const double fixed_ms=1000.0/(double)NF_TICK_RATE; uint32_t last=nf_net_now_ms(),start=last; double acc=0.0; uint64_t next_ai_log_tick=0u;
    while(g_running) {
        NfNetEvent ev;
        while(nf_net_service(&net,&ev,0)>0) {
            if(ev.type==NF_NET_EVENT_RECEIVE) {
                NfMessageType t=nf_protocol_peek_type(ev.data,ev.size); NfServerClient *c=by_peer(clients,ev.peer);
                if(t==NF_MSG_HELLO)handle_hello(&net,&world,clients,ev.peer,ev.data,ev.size);
                else if(t==NF_MSG_INPUT)handle_input(c,ev.data,ev.size);
                else if(t==NF_MSG_PING&&c!=NULL) { NfPingMessage p; if(nf_protocol_decode_ping(ev.data,ev.size,NF_MSG_PING,&p)){uint8_t b[32];size_t n=nf_protocol_encode_ping(b,sizeof(b),NF_MSG_PONG,&p);nf_net_send(&net,c->peer,NF_NET_CHANNEL_RELIABLE,b,n,true);} }
            } else if(ev.type==NF_NET_EVENT_DISCONNECT) {
                NfServerClient *c=by_peer(clients,ev.peer); if(c!=NULL){c->connected=false;c->peer=NULL;c->current_input=(NfMoveInput){0};c->combat_input=(NfCombatInput){0};c->reserved_until_ms=nf_net_now_ms()+NF_NET_RECONNECT_WINDOW_MS;printf("[server] reserved entity=%u for reconnect\n",c->entity_id);}
            }
        }
        uint32_t now=nf_net_now_ms(); uint32_t elapsed=now-last; last=now; if(elapsed>250u)elapsed=250u; acc+=(double)elapsed; expire_reservations(&world,clients,now);
        while(acc>=fixed_ms) {
            NfControlFrame ai_controls[NF_AI_MAX_AGENTS]; size_t ai_controls_count=nf_ai_tick(&ai,&world,&semantics,ai_controls,NF_AI_MAX_AGENTS);
            for(size_t i=0;i<NF_SERVER_CLIENTS;++i) {
                NfServerClient *c=&clients[i]; if(!c->occupied)continue; NfMoveInput move=c->connected?c->current_input:(NfMoveInput){0}; nf_world_set_input(&world,c->entity_id,move);
                if(c->connected){NfControlFrame control=client_control(c);process_combat_control(&net,&world,clients,&semantics,&control,history,rival_relation,friendly_fire);c->current_input.jump_pressed=false;c->combat_input.fire_pressed=false;c->combat_input.reload_pressed=false;c->combat_input.weapon_slot=0u;}
            }
            for(size_t i=0;i<ai_controls_count;++i){nf_world_set_input(&world,ai_controls[i].actor,ai_controls[i].move);process_combat_control(&net,&world,clients,&semantics,&ai_controls[i],history,rival_relation,friendly_fire);}
            nf_world_step(&world,1.0f/(float)NF_TICK_RATE); process_respawns(&net,&world,clients,&ai,&semantics); history_record(history,&world);
            if(world.tick%(NF_TICK_RATE/NF_NET_SNAPSHOT_HZ)==0u) for(size_t i=0;i<NF_SERVER_CLIENTS;++i) if(clients[i].occupied&&clients[i].connected) send_snapshot(&net,&world,&clients[i]);
            if(world.tick>=next_ai_log_tick){for(size_t i=0;i<ai.count;++i){const NfAiAgent*a=&ai.agents[i];const NfActor*body=nf_world_find_actor_const(&world,a->actor_id);printf("[ai] id=%u role=%s mode=%s target=%u confidence=%.2f visible=%s health=%.0f cover=%d\n",a->actor_id,nf_squad_role_name(a->role),nf_agent_mode_name(a->mode),a->knowledge.target,a->knowledge.confidence,a->knowledge.visible_now?"yes":"no",body?body->health:0.0f,a->selected_affordance);}next_ai_log_tick=world.tick+NF_TICK_RATE*2u;}
            acc-=fixed_ms;
        }
        nf_net_flush(&net); if(duration>0.0&&(double)(now-start)>=duration*1000.0)break; sleep_ms(1);
    }
    for(size_t i=0;i<NF_SERVER_CLIENTS;++i) if(clients[i].connected) nf_net_disconnect(clients[i].peer,0);
    nf_net_flush(&net); nf_net_close(&net); nf_net_global_shutdown(); printf("[server] shutdown tick=%llu\n",(unsigned long long)world.tick); return 0;
}
