#include "nf_combat.h"
#include "nf_contamination.h"
#include "nf_hitbox.h"
#include "nf_lifeworld.h"
#include "nf_net.h"
#include "nf_prediction.h"
#include "nf_region.h"
#include "nf_security.h"
#include "nf_world.h"
#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NF_REMOTE_SLOTS NF_NET_MAX_SNAPSHOT_ACTORS
#define NF_CLIENT_STASIS_TRACES 16u
#define NF_CLIENT_TRACE_LIFETIME_MS 60000u

typedef struct RemoteActor {
    bool active;
    bool seen;
    NfEntityId id;
    NfFaction faction;
    Vector3 from;
    Vector3 to;
    uint32_t received_ms;
    NfMovementMode mode;
    bool alive;
    NfAgencyState agency;
    float health;
    float contamination_systemic;
    float contamination_locomotor;
    float contamination_manipulator;
    float contamination_sensory;
    NfWeaponId weapon;
} RemoteActor;

typedef struct NfClientStasisTrace {
    bool active;
    Vector3 position;
    uint32_t created_ms;
    NfEntityId actor_id;
} NfClientStasisTrace;

typedef struct NfMouseCapture {
    bool requested;
    bool captured;
    bool was_focused;
    bool position_valid;
    uint32_t capture_after_ms;
    Vector2 previous_position;
    Vector2 last_delta;
    unsigned absolute_fallback_frames;
} NfMouseCapture;

static float clampf_local(float v,float lo,float hi) { return v<lo?lo:(v>hi?hi:v); }

/* Normal graybox identity colors are sacrosanct:
   white=player, blue=ally, red=Rival, orange=Cattler, yellow=Cattler weak point. */
static Color faction_color(NfFaction faction) {
    switch (faction) {
        case NF_FACTION_PLAYER: return (Color){242,244,247,255};
        case NF_FACTION_TEAMMATE: return (Color){46,168,255,255};
        case NF_FACTION_RIVAL: return (Color){214,40,40,255};
        case NF_FACTION_RANCHER: return (Color){242,140,40,255};
        default: return (Color){112,117,125,255};
    }
}

static Color collider_color(const NfCollider *c) {
    if(c->kind==NF_COLLIDER_LADDER)return(Color){72,191,199,145};
    if(c->kind==NF_COLLIDER_MOVING_PLATFORM)return(Color){112,106,154,255};
    return(Color){112,117,125,255};
}

static void draw_collider(const NfCollider *c) {
    Vector3 center={0.5f*(c->min.x+c->max.x),0.5f*(c->min.y+c->max.y),0.5f*(c->min.z+c->max.z)};
    float sx=c->max.x-c->min.x,sy=c->max.y-c->min.y,sz=c->max.z-c->min.z;
    DrawCube(center,sx,sy,sz,collider_color(c));
    DrawCubeWires(center,sx,sy,sz,(Color){185,191,202,180});
}

static void tri(Vector3 a,Vector3 b,Vector3 c,Color color) { DrawTriangle3D(a,b,c,color); }

static void draw_ramp(const NfRamp *r) {
    const Color color=(Color){158,164,172,255};
    Vector3 low_a,low_b,high_a,high_b;
    if(r->axis==NF_RAMP_POS_Z||r->axis==NF_RAMP_NEG_Z){
        bool pos=r->axis==NF_RAMP_POS_Z;float low_z=pos?r->min.z:r->max.z,high_z=pos?r->max.z:r->min.z;
        low_a=(Vector3){r->min.x,r->min.y,low_z};low_b=(Vector3){r->max.x,r->min.y,low_z};
        high_a=(Vector3){r->min.x,r->max.y,high_z};high_b=(Vector3){r->max.x,r->max.y,high_z};
    }else{
        bool pos=r->axis==NF_RAMP_POS_X;float low_x=pos?r->min.x:r->max.x,high_x=pos?r->max.x:r->min.x;
        low_a=(Vector3){low_x,r->min.y,r->min.z};low_b=(Vector3){low_x,r->min.y,r->max.z};
        high_a=(Vector3){high_x,r->max.y,r->min.z};high_b=(Vector3){high_x,r->max.y,r->max.z};
    }
    tri(low_a,low_b,high_b,color);tri(low_a,high_b,high_a,color);
    DrawLine3D(low_a,low_b,LIGHTGRAY);DrawLine3D(low_a,high_a,LIGHTGRAY);
    DrawLine3D(low_b,high_b,LIGHTGRAY);DrawLine3D(high_a,high_b,LIGHTGRAY);
}

/* Traversal debug avoids normal actor identity colors. */
static Color candidate_color(NfTraversalType t) {
    switch(t){
        case NF_TRAVERSAL_STEP:return(Color){83,207,165,255};
        case NF_TRAVERSAL_VAULT:return(Color){215,68,184,255};
        case NF_TRAVERSAL_MANTLE:return(Color){80,216,215,255};
        case NF_TRAVERSAL_LADDER:return(Color){72,191,199,255};
        default:return(Color){170,170,180,255};
    }
}

static RemoteActor *remote_slot(RemoteActor remotes[],NfEntityId id) {
    for(size_t i=0;i<NF_REMOTE_SLOTS;++i)if(remotes[i].active&&remotes[i].id==id)return &remotes[i];
    for(size_t i=0;i<NF_REMOTE_SLOTS;++i){
        if(!remotes[i].active){memset(&remotes[i],0,sizeof(remotes[i]));remotes[i].active=true;remotes[i].id=id;return &remotes[i];}
    }
    return NULL;
}

static void send_hello(NfNetHost *net,void *peer,const uint8_t resume[NF_NET_TOKEN_BYTES]) {
    NfHelloMessage h={0};nf_security_random(h.client_nonce,sizeof(h.client_nonce));memcpy(h.resume_token,resume,NF_NET_TOKEN_BYTES);
    uint8_t buf[128];size_t n=nf_protocol_encode_hello(buf,sizeof(buf),&h);nf_net_send(net,peer,NF_NET_CHANNEL_RELIABLE,buf,n,true);
}

static void draw_weapon_hud(const NfActor *player,int width,int height,bool muzzle) {
    if(player==NULL)return;
    Color body=player->combat.weapon==NF_WEAPON_PISTOL?(Color){180,184,193,255}:(Color){118,126,139,255};
    int w=player->combat.weapon==NF_WEAPON_PISTOL?150:260;int h=player->combat.weapon==NF_WEAPON_PISTOL?40:58;
    DrawRectangle(width-w-70,height-h-72,w,h,body);DrawRectangleLines(width-w-70,height-h-72,w,h,RAYWHITE);
    if(muzzle){DrawCircle(width-66,height-h-51,15,(Color){255,222,86,255});DrawCircle(width-66,height-h-51,7,(Color){240,196,92,255});}
}

static void draw_hit_marker(int cx,int cy,Color color) {
    DrawLine(cx-13,cy-13,cx-5,cy-5,color);DrawLine(cx+13,cy-13,cx+5,cy-5,color);
    DrawLine(cx-13,cy+13,cx-5,cy+5,color);DrawLine(cx+13,cy+13,cx+5,cy+5,color);
}

static void draw_region_graph(const NfRegionGraph *graph) {
    if(graph==NULL)return;
    for(size_t i=0;i<graph->count;++i){
        const NfRegion *r=&graph->regions[i];Vector3 a={r->center.x,0.18f,r->center.z};
        DrawSphere(a,1.25f,(Color){70,205,210,210});
        for(uint8_t n=0;n<r->neighbor_count;++n){
            uint8_t j=r->neighbors[n];if(j<=i||j>=graph->count)continue;
            const NfRegion *b=&graph->regions[j];DrawLine3D(a,(Vector3){b->center.x,0.18f,b->center.z},(Color){70,205,210,135});
        }
    }
}

static void draw_remote_actor(Vector3 p,const RemoteActor *remote) {
    if(remote==NULL)return;
    const bool cattler=remote->faction==NF_FACTION_RANCHER;
    const bool stasis=remote->agency==NF_AGENCY_STASIS||!remote->alive;
    float body_h=stasis?0.28f:(cattler?NF_CATTLER_BODY_HEIGHT:1.80f);
    float body_w=cattler?NF_CATTLER_BODY_WIDTH:0.72f;
    Color identity=faction_color(remote->faction);
    Vector3 center={p.x,p.y+body_h*0.5f,p.z};
    DrawCube(center,body_w,body_h,body_w,identity);
    DrawCubeWires(center,body_w,body_h,body_w,RAYWHITE);

    const float contam=clampf_local(remote->contamination_systemic,0.0f,1.0f);
    if(contam>0.01f||stasis){
        const unsigned char alpha=(unsigned char)clampf_local(35.0f+contam*150.0f+(stasis?45.0f:0.0f),0.0f,220.0f);
        DrawCube(center,body_w+0.05f,body_h+0.04f,body_w+0.05f,(Color){36,21,46,alpha});
    }

    if(cattler&&!stasis){
        Color weak=(Color){255,212,59,240};
        DrawCube((Vector3){p.x,p.y+NF_CATTLER_KNEE_BAND_CENTER_Y,p.z},NF_CATTLER_WEAK_WIDTH,NF_CATTLER_KNEE_BAND_HEIGHT,NF_CATTLER_WEAK_WIDTH,weak);
        DrawCube((Vector3){p.x,p.y+NF_CATTLER_FOOT_BAND_CENTER_Y,p.z},NF_CATTLER_WEAK_WIDTH,NF_CATTLER_FOOT_BAND_HEIGHT,NF_CATTLER_WEAK_WIDTH,weak);
    }
}

static void client_trace_add(NfClientStasisTrace traces[],NfEntityId actor,Vector3 position,uint32_t now) {
    size_t slot=NF_CLIENT_STASIS_TRACES;
    uint32_t oldest=UINT32_MAX;size_t oldest_slot=0u;
    for(size_t i=0;i<NF_CLIENT_STASIS_TRACES;++i){
        if(!traces[i].active){slot=i;break;}
        if(traces[i].created_ms<oldest){oldest=traces[i].created_ms;oldest_slot=i;}
    }
    if(slot==NF_CLIENT_STASIS_TRACES)slot=oldest_slot;
    traces[slot]=(NfClientStasisTrace){.active=true,.position=position,.created_ms=now,.actor_id=actor};
}

static void draw_client_traces(NfClientStasisTrace traces[],uint32_t now) {
    for(size_t i=0;i<NF_CLIENT_STASIS_TRACES;++i){
        NfClientStasisTrace *t=&traces[i];if(!t->active)continue;
        uint32_t age=now-t->created_ms;if(age>=NF_CLIENT_TRACE_LIFETIME_MS){t->active=false;continue;}
        float remain=1.0f-(float)age/(float)NF_CLIENT_TRACE_LIFETIME_MS;
        unsigned char alpha=(unsigned char)(55.0f+remain*105.0f);
        float scale=0.8f+remain*0.7f;
        DrawCube((Vector3){t->position.x,0.045f,t->position.z},1.8f*scale,0.06f,1.2f*scale,(Color){36,21,46,alpha});
        DrawCube((Vector3){t->position.x+0.35f,0.078f,t->position.z-0.18f},0.55f*scale,0.025f,1.25f*scale,(Color){74,39,100,(unsigned char)(alpha*0.72f)});
    }
}

static bool in_lifeworld_window(Vector3 p){
    return fabsf(p.x-NF_LIFEWORLD_DEMO_CENTER_X)<=NF_LIFEWORLD_DEMO_HALF_X&&
        fabsf(p.z-NF_LIFEWORLD_DEMO_CENTER_Z)<=NF_LIFEWORLD_DEMO_HALF_Z;
}

static unsigned replicated_cattlers_in_window(const RemoteActor remotes[]){
    unsigned count=0u;
    for(size_t i=0u;i<NF_REMOTE_SLOTS;++i){
        const RemoteActor *r=&remotes[i];
        if(r->active&&r->alive&&r->agency!=NF_AGENCY_STASIS&&r->faction==NF_FACTION_RANCHER&&in_lifeworld_window(r->to))++count;
    }
    return count;
}

static void draw_demo_water(void) {
    DrawCube((Vector3){NF_CONTAM_DEMO_WATER_CENTER_X,0.018f,NF_CONTAM_DEMO_WATER_CENTER_Z},
        NF_CONTAM_DEMO_WATER_HALF_X*2.0f,0.035f,NF_CONTAM_DEMO_WATER_HALF_Z*2.0f,(Color){50,106,140,150});
    DrawCubeWires((Vector3){NF_CONTAM_DEMO_WATER_CENTER_X,0.02f,NF_CONTAM_DEMO_WATER_CENTER_Z},
        NF_CONTAM_DEMO_WATER_HALF_X*2.0f,0.04f,NF_CONTAM_DEMO_WATER_HALF_Z*2.0f,(Color){120,175,195,130});
}

static void draw_lifeworld_surface(float darkness){
    const float d=clampf_local(darkness,0.0f,1.0f);
    const unsigned char stain_alpha=(unsigned char)(22.0f+d*164.0f);
    const unsigned char violet_alpha=(unsigned char)(8.0f+d*72.0f);
    const unsigned char smoke_alpha=(unsigned char)(6.0f+d*72.0f);
    const unsigned char filament_alpha=(unsigned char)(10.0f+d*112.0f);

    /* Cattler infestation is environmental deep-violet/cyan phenotype, never orange actor identity. */
    DrawCube((Vector3){NF_LIFEWORLD_DEMO_CENTER_X,0.025f,NF_LIFEWORLD_DEMO_CENTER_Z},
        NF_LIFEWORLD_DEMO_HALF_X*2.0f,0.05f,NF_LIFEWORLD_DEMO_HALF_Z*2.0f,(Color){8,10,28,stain_alpha});
    DrawCube((Vector3){-51.0f,0.055f,-11.0f},17.0f,0.035f,10.0f,(Color){53,19,79,violet_alpha});

    if(d>0.02f){
        const double time=GetTime();
        static const Vector3 anchors[6]={
            {-61.0f,0.55f,-17.0f},{-56.0f,0.85f,-8.0f},{-49.0f,0.45f,-14.0f},
            {-43.0f,0.72f,-5.0f},{-35.0f,0.52f,-12.0f},{-29.0f,0.88f,-4.0f}
        };
        for(int i=0;i<6;++i){
            const float phase=(float)time*(0.18f+0.025f*(float)i)+(float)i*1.37f;
            Vector3 p=anchors[i];p.x+=sinf(phase)*1.2f;p.z+=cosf(phase*0.73f)*0.9f;p.y+=0.18f+sinf(phase*0.51f)*0.16f;
            const float radius=(0.55f+0.10f*(float)(i%3))*d;DrawSphere(p,radius,(Color){18,15,34,smoke_alpha});
        }
        DrawCube((Vector3){-64.0f,1.10f,-18.7f},0.18f,2.20f,2.20f,(Color){23,10,32,filament_alpha});
        DrawCube((Vector3){-57.0f,1.35f,-0.9f},0.16f,2.70f,1.60f,(Color){18,9,30,filament_alpha});
        DrawCube((Vector3){-28.0f,0.85f,-8.0f},0.12f,1.70f,2.60f,(Color){20,10,34,filament_alpha});
        DrawCube((Vector3){-51.5f,0.075f,-14.2f},2.3f,0.035f,0.42f,(Color){53,19,79,(unsigned char)(d*126.0f)});
        DrawCube((Vector3){-45.0f,0.078f,-7.0f},0.55f,0.04f,2.0f,(Color){80,216,215,(unsigned char)(d*74.0f)});
        DrawCube((Vector3){-36.0f,0.076f,-11.0f},1.5f,0.035f,0.35f,(Color){46,18,54,(unsigned char)(d*98.0f)});
        const float anomaly=sinf((float)time*0.63f)*0.35f*d;
        DrawLine3D((Vector3){-58.0f,1.9f,-13.0f},(Vector3){-48.0f+anomaly,2.05f,-8.0f},(Color){80,216,215,(unsigned char)(d*72.0f)});
        DrawLine3D((Vector3){-48.0f,2.05f,-8.0f},(Vector3){-39.0f-anomaly,1.75f,-5.5f},(Color){80,190,200,(unsigned char)(d*58.0f)});
    }

    const float activity=1.0f-d*0.62f;
    const unsigned char green=(unsigned char)(80.0f+activity*150.0f);
    DrawCube((Vector3){-54.0f,0.65f,-16.0f},1.6f,1.3f,1.6f,(Color){40,green,105,255});
    DrawCubeWires((Vector3){-54.0f,0.65f,-16.0f},1.6f,1.3f,1.6f,(Color){180,220,220,255});
    DrawCube((Vector3){-38.0f,0.45f,-4.0f},1.8f,0.9f,1.8f,(Color){45,110,(unsigned char)(100.0f+activity*120.0f),255});
    DrawCubeWires((Vector3){-38.0f,0.45f,-4.0f},1.8f,0.9f,1.8f,(Color){180,220,220,255});
    DrawLine3D((Vector3){-53.2f,0.35f,-15.4f},(Vector3){-38.9f,0.35f,-4.6f},(Color){80,160,180,180});
}

static Vector2 update_mouse_capture(NfMouseCapture *state,uint32_t now) {
    bool focused=IsWindowFocused();
    if(focused&&!state->was_focused){state->capture_after_ms=now+80u;state->position_valid=false;}
    if(IsKeyPressed(KEY_F10)){state->requested=!state->requested;state->capture_after_ms=now;state->position_valid=false;}
    if(!focused||!state->requested){if(state->captured){EnableCursor();state->captured=false;}state->position_valid=false;state->last_delta=(Vector2){0};state->was_focused=focused;return state->last_delta;}
    if(!state->captured&&(int32_t)(now-state->capture_after_ms)>=0){DisableCursor();state->captured=true;state->position_valid=false;}
    Vector2 delta={0};
    if(state->captured){
        delta=GetMouseDelta();Vector2 position=GetMousePosition();
        if(state->position_valid){
            Vector2 absolute_delta={position.x-state->previous_position.x,position.y-state->previous_position.y};
            bool relative_zero=fabsf(delta.x)<0.001f&&fabsf(delta.y)<0.001f;
            bool absolute_moved=fabsf(absolute_delta.x)>=0.001f||fabsf(absolute_delta.y)>=0.001f;
            if(relative_zero&&absolute_moved){delta=absolute_delta;++state->absolute_fallback_frames;}
        }
        state->previous_position=position;state->position_valid=true;
    }
    state->last_delta=delta;state->was_focused=focused;return delta;
}

int main(int argc,char **argv) {
    const char *host="127.0.0.1";uint16_t port=NF_NET_DEFAULT_PORT;uint32_t sim_latency=0,sim_jitter=0;float sim_loss=0.0f;
    for(int i=1;i<argc;++i){
        if(strcmp(argv[i],"--host")==0&&i+1<argc)host=argv[++i];
        else if(strcmp(argv[i],"--port")==0&&i+1<argc)port=(uint16_t)atoi(argv[++i]);
        else if(strcmp(argv[i],"--sim-latency")==0&&i+1<argc)sim_latency=(uint32_t)atoi(argv[++i]);
        else if(strcmp(argv[i],"--sim-jitter")==0&&i+1<argc)sim_jitter=(uint32_t)atoi(argv[++i]);
        else if(strcmp(argv[i],"--sim-loss")==0&&i+1<argc)sim_loss=(float)atof(argv[++i]);
    }
    if(!nf_security_init()||!nf_net_global_init()){fprintf(stderr,"nightfall: network init failed\n");return 1;}
    NfNetHost net;if(!nf_net_client_open(&net)){nf_net_global_shutdown();return 1;}
    nf_net_set_simulation(&net,sim_latency,sim_jitter,sim_loss);void *peer=nf_net_connect(&net,host,port);
    const int width=1280,height=720;InitWindow(width,height,"nightfall!punk v1.1 contamination / relational physics lab");SetTargetFPS(144);
    NfMouseCapture mouse_capture={.requested=true,.capture_after_ms=nf_net_now_ms()+80u};
    NfRegionGraph region_graph;nf_region_graph_init_spatial_lab(&region_graph);bool spatial_debug=false;
    NfWorld world;nf_world_init(&world,20260807u);nf_world_build_movement_lab(&world);
    NfPrediction prediction;nf_prediction_init(&prediction);NfEntityId player_id=0;uint8_t session_token[NF_NET_TOKEN_BYTES]={0};
    bool welcomed=false,connected=false,strong_crypto=false;uint32_t next_reconnect_ms=0,last_snapshot_ms=0,last_ping_ms=0,last_pong_ms=0;
    uint64_t last_server_tick=0;uint32_t candidate_disagreements=0;RemoteActor remotes[NF_REMOTE_SLOTS]={0};
    NfClientStasisTrace stasis_traces[NF_CLIENT_STASIS_TRACES]={0};
    NfCombatEvent last_event={0};uint32_t last_event_ms=0,hit_marker_until=0,damage_flash_until=0,muzzle_until=0,last_auto_flash=0;
    unsigned combat_events=0;float yaw=0,pitch=0;double accumulator=0;const double fixed_dt=1.0/(double)NF_TICK_RATE;
    bool jump_latched=false,fire_latched=false,reload_latched=false;uint8_t weapon_latched=0;float predicted_darkness=0.0f;

    while(!WindowShouldClose()) {
        uint32_t now=nf_net_now_ms();NfNetEvent ev;
        while(nf_net_service(&net,&ev,0)>0) {
            if(ev.type==NF_NET_EVENT_CONNECT){connected=true;peer=ev.peer;send_hello(&net,peer,session_token);}
            else if(ev.type==NF_NET_EVENT_DISCONNECT){connected=false;welcomed=false;peer=NULL;next_reconnect_ms=nf_net_now_ms()+1500u;}
            else if(ev.type==NF_NET_EVENT_RECEIVE) {
                NfMessageType t=nf_protocol_peek_type(ev.data,ev.size);
                if(t==NF_MSG_WELCOME){
                    NfWelcomeMessage w;
                    if(nf_protocol_decode_welcome(ev.data,ev.size,&w)){
                        bool first=player_id==0;player_id=w.entity_id;memcpy(session_token,w.session_token,NF_NET_TOKEN_BYTES);strong_crypto=w.strong_crypto;
                        if(first||nf_world_find_actor(&world,player_id)==NULL){nf_world_init(&world,w.world_seed);nf_world_build_movement_lab(&world);nf_world_spawn_actor_with_id(&world,player_id,NF_FACTION_PLAYER,(NfVec3){0,0.05f,-18});}
                        world.tick=w.server_tick;nf_world_sync_dynamic_geometry(&world);nf_prediction_init(&prediction);welcomed=true;
                    }
                } else if(t==NF_MSG_SNAPSHOT&&welcomed){
                    NfSnapshotMessage s;
                    if(nf_protocol_decode_snapshot(ev.data,ev.size,&s)){
                        last_snapshot_ms=nf_net_now_ms();last_server_tick=s.server_tick;
                        for(size_t i=0;i<NF_REMOTE_SLOTS;++i)remotes[i].seen=false;
                        for(uint8_t i=0;i<s.actor_count;++i){
                            const NfActorNetState *a=&s.actors[i];
                            if(a->id==player_id){
                                NfActor *local=nf_world_find_actor(&world,player_id);
                                if(local!=NULL){
                                    int predicted_feature=local->movement.candidate.active?local->movement.candidate.feature_index:-1;
                                    bool traversal_active=local->movement.mode==NF_MOVE_VAULT||local->movement.mode==NF_MOVE_MANTLE||local->movement.mode==NF_MOVE_LADDER;
                                    if(traversal_active&&predicted_feature!=a->candidate_feature)++candidate_disagreements;
                                }
                                nf_prediction_reconcile(&prediction,&world,player_id,a,s.acknowledged_input,s.server_tick,0.035f);
                            } else {
                                RemoteActor *r=remote_slot(remotes,a->id);
                                if(r!=NULL){
                                    Vector3 p={a->position.x,a->position.y,a->position.z};if(r->received_ms==0)r->from=p;else r->from=r->to;r->to=p;
                                    r->received_ms=last_snapshot_ms;r->mode=a->mode;r->faction=a->faction;r->alive=a->alive;r->agency=a->agency;r->health=a->health;
                                    r->contamination_systemic=a->contamination_systemic;r->contamination_locomotor=a->contamination_locomotor;
                                    r->contamination_manipulator=a->contamination_manipulator;r->contamination_sensory=a->contamination_sensory;r->weapon=a->weapon;r->seen=true;
                                }
                            }
                        }
                        for(size_t i=0;i<NF_REMOTE_SLOTS;++i)if(remotes[i].active&&!remotes[i].seen&&remotes[i].received_ms!=0&&last_snapshot_ms-remotes[i].received_ms>500u)remotes[i].active=false;
                    }
                } else if(t==NF_MSG_COMBAT_EVENT){
                    NfCombatEventMessage m;
                    if(nf_protocol_decode_combat_event(ev.data,ev.size,&m)){
                        last_event=m.event;last_event_ms=nf_net_now_ms();++combat_events;
                        if(m.event.source==player_id&&(m.event.type==NF_COMBAT_EVENT_DAMAGE||m.event.type==NF_COMBAT_EVENT_STASIS))hit_marker_until=last_event_ms+160u;
                        if(m.event.target==player_id&&(m.event.type==NF_COMBAT_EVENT_DAMAGE||m.event.type==NF_COMBAT_EVENT_STASIS))damage_flash_until=last_event_ms+260u;
                        if(m.event.type==NF_COMBAT_EVENT_STASIS){Vector3 p={m.event.position.x,m.event.position.y,m.event.position.z};client_trace_add(stasis_traces,m.event.target,p,last_event_ms);}
                    }
                } else if(t==NF_MSG_PONG){NfPingMessage p;if(nf_protocol_decode_ping(ev.data,ev.size,NF_MSG_PONG,&p))last_pong_ms=nf_net_now_ms()-p.stamp_ms;}
                else if(t==NF_MSG_REJECT){uint8_t reason=0;nf_protocol_decode_reject(ev.data,ev.size,&reason);fprintf(stderr,"nightfall: server rejected connection reason=%u\n",reason);}
            }
        }

        now=nf_net_now_ms();if(IsKeyPressed(KEY_F4))spatial_debug=!spatial_debug;
        if(!connected&&peer==NULL&&(int32_t)(now-next_reconnect_ms)>=0){peer=nf_net_connect(&net,host,port);next_reconnect_ms=now+1500u;}
        if(connected&&welcomed&&now-last_ping_ms>1000u){NfPingMessage p={.stamp_ms=now};uint8_t buf[32];size_t n=nf_protocol_encode_ping(buf,sizeof(buf),NF_MSG_PING,&p);nf_net_send(&net,peer,NF_NET_CHANNEL_RELIABLE,buf,n,true);last_ping_ms=now;}

        Vector2 mouse=update_mouse_capture(&mouse_capture,now);yaw-=mouse.x*0.0022f;pitch=clampf_local(pitch-mouse.y*0.0022f,-1.45f,1.45f);
        NfMoveInput input={0};input.forward=(IsKeyDown(KEY_W)?1.0f:0.0f)-(IsKeyDown(KEY_S)?1.0f:0.0f);input.strafe=(IsKeyDown(KEY_A)?1.0f:0.0f)-(IsKeyDown(KEY_D)?1.0f:0.0f);input.yaw_radians=yaw;
        if(IsKeyPressed(KEY_SPACE))jump_latched=true;input.jump_pressed=jump_latched;input.crouch_held=IsKeyDown(KEY_LEFT_CONTROL);input.sprint_held=IsKeyDown(KEY_LEFT_SHIFT);input.interact_held=IsKeyDown(KEY_E);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){fire_latched=true;muzzle_until=now+55u;}if(IsKeyPressed(KEY_R))reload_latched=true;if(IsKeyPressed(KEY_ONE))weapon_latched=1u;if(IsKeyPressed(KEY_TWO))weapon_latched=2u;
        const NfActor *pre_player=player_id?nf_world_find_actor_const(&world,player_id):NULL;
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)&&pre_player!=NULL&&pre_player->combat.weapon==NF_WEAPON_CARBINE&&now-last_auto_flash>95u){muzzle_until=now+45u;last_auto_flash=now;}
        NfCombatInput combat={.fire_held=IsMouseButtonDown(MOUSE_BUTTON_LEFT),.fire_pressed=fire_latched,.reload_pressed=reload_latched,.weapon_slot=weapon_latched,.aim_pitch_radians=pitch};
        double frame_dt=GetFrameTime();if(frame_dt>0.10)frame_dt=0.10;

        const unsigned local_cattlers=replicated_cattlers_in_window(remotes);
        const float darkness_target=local_cattlers>=2u?0.82f:(local_cattlers==1u?0.38f:0.0f);
        const float darkness_rate=darkness_target>predicted_darkness?0.14f:0.015f;
        if(darkness_target>predicted_darkness){predicted_darkness+=darkness_rate*(float)frame_dt;if(predicted_darkness>darkness_target)predicted_darkness=darkness_target;}
        else if(darkness_target<predicted_darkness){predicted_darkness-=darkness_rate*(float)frame_dt;if(predicted_darkness<darkness_target)predicted_darkness=darkness_target;}

        accumulator+=frame_dt;bool first_tick=true;
        while(accumulator>=fixed_dt){
            if(!first_tick){input.jump_pressed=false;combat.fire_pressed=false;combat.reload_pressed=false;combat.weapon_slot=0u;}
            if(welcomed&&player_id!=0){
                NfInputCommand cmd=nf_prediction_make_command(&prediction,world.tick,input,combat);nf_world_set_input(&world,player_id,cmd.move);nf_world_step(&world,(float)fixed_dt);
                NfActor *local=nf_world_find_actor(&world,player_id);nf_prediction_record(&prediction,&cmd,local);NfInputBundle bundle={0};memcpy(bundle.session_token,session_token,NF_NET_TOKEN_BYTES);
                nf_prediction_build_redundant_bundle(&prediction,cmd.sequence,&bundle);uint8_t buf[NF_NET_MAX_PACKET_BYTES];size_t n=nf_protocol_encode_input(buf,sizeof(buf),&bundle);if(n)nf_net_send(&net,peer,NF_NET_CHANNEL_STATE,buf,n,false);
                if(first_tick){if(input.jump_pressed)jump_latched=false;if(combat.fire_pressed)fire_latched=false;if(combat.reload_pressed)reload_latched=false;if(combat.weapon_slot)weapon_latched=0u;}
            }
            accumulator-=fixed_dt;first_tick=false;
        }

        const NfActor *player=player_id?nf_world_find_actor_const(&world,player_id):NULL;
        Vector3 eye={0,2,-18},look={0,0,1};
        if(player!=NULL){eye=(Vector3){player->transform.position.x,player->transform.position.y+player->movement.eye_height,player->transform.position.z};look=(Vector3){cosf(pitch)*sinf(yaw),sinf(pitch),cosf(pitch)*cosf(yaw)};}
        Camera3D camera={.position=eye,.target={eye.x+look.x,eye.y+look.y,eye.z+look.z},.up={0,1,0},.fovy=82,.projection=CAMERA_PERSPECTIVE};

        BeginDrawing();ClearBackground((Color){18,20,24,255});BeginMode3D(camera);
        DrawGrid(60,1);draw_demo_water();draw_lifeworld_surface(predicted_darkness);draw_client_traces(stasis_traces,now);
        if(spatial_debug)draw_region_graph(&region_graph);
        for(size_t i=0;i<world.collider_count;++i)draw_collider(&world.colliders[i]);
        for(size_t i=0;i<world.ramp_count;++i)draw_ramp(&world.ramps[i]);
        for(size_t i=0;i<NF_REMOTE_SLOTS;++i){
            if(!remotes[i].active)continue;float a=clampf_local((float)(now-remotes[i].received_ms)/(1000.0f/(float)NF_NET_SNAPSHOT_HZ),0,1);
            Vector3 p={remotes[i].from.x+(remotes[i].to.x-remotes[i].from.x)*a,remotes[i].from.y+(remotes[i].to.y-remotes[i].from.y)*a,remotes[i].from.z+(remotes[i].to.z-remotes[i].from.z)*a};draw_remote_actor(p,&remotes[i]);
        }
        if(player!=NULL&&player->movement.candidate.active){const NfTraversalCandidate *c=&player->movement.candidate;Vector3 p={c->point.x,c->point.y+0.15f,c->point.z};DrawSphere(p,0.14f,candidate_color(c->type));}
        EndMode3D();

        DrawText("nightfall!punk v1.1 - CONTAMINATION / RELATIONAL PHYSICS LAB",24,18,23,RAYWHITE);
        DrawText("white player | blue ally | RED Rival | ORANGE Dream Cattler | YELLOW Cattler weak point | black-violet contamination",24,47,15,GRAY);
        const char *status=welcomed?"AUTHORITATIVE + PREDICTED":(connected?"HANDSHAKE":"RECONNECTING");
        DrawText(TextFormat("NET %s | ENet RTT %u ms | effective ping %u ms | snapshot age %u ms",status,peer?nf_net_peer_rtt_ms(peer):0,last_pong_ms,last_snapshot_ms?now-last_snapshot_ms:0),24,79,16,welcomed?GREEN:YELLOW);
        DrawText(TextFormat("server tick %llu | client tick %llu | cmd %u ack %u pending %u",(unsigned long long)last_server_tick,(unsigned long long)world.tick,prediction.next_sequence?prediction.next_sequence-1:0,prediction.last_acknowledged,(prediction.next_sequence-1)-prediction.last_acknowledged),24,101,16,RAYWHITE);
        DrawText(TextFormat("prediction %.3fm | max %.3fm | visible corrections %u | resyncs %u | traversal disagreements %u",prediction.last_error,prediction.max_error,prediction.corrections,prediction.resyncs,candidate_disagreements),24,123,16,RAYWHITE);
        if(player!=NULL){
            float speed=sqrtf(player->transform.velocity.x*player->transform.velocity.x+player->transform.velocity.z*player->transform.velocity.z);
            DrawText(TextFormat("MOVE %s %.2f m/s | AGENCY %s | CONTAM %.0f%% [sys %.0f loc %.0f man %.0f sen %.0f]",
                nf_movement_mode_name(player->movement.mode),speed,nf_agency_state_name(player->agency),player->contamination.systemic*100.0f,
                player->contamination.systemic*100.0f,player->contamination.locomotor*100.0f,player->contamination.manipulator*100.0f,player->contamination.sensory*100.0f),24,145,16,RAYWHITE);
            DrawText(TextFormat("EQUIP %s %u/%u | %s",nf_weapon_name(player->combat.weapon),player->combat.ammo_mag[player->combat.weapon],player->combat.reserve_ammo[player->combat.weapon],nf_weapon_state_name(player->combat.state)),24,167,16,RAYWHITE);
            if(player->agency==NF_AGENCY_STASIS){
                DrawRectangle(0,0,width,height,(Color){20,10,28,82});
                DrawText("STASIS — agency suspended / revival pending",width/2-230,height/2+45,20,(Color){190,150,215,255});
            } else if(player->agency==NF_AGENCY_RETURNING){
                DrawText("RETURNING — agency restored; locality history persists",width/2-250,height/2+45,18,(Color){150,210,220,255});
            }
        }
        DrawText(TextFormat("events %u | last %s src %u tgt %u zone %u dose %.0f rewind %ums",combat_events,nf_combat_event_name(last_event.type),last_event.source,last_event.target,(unsigned)last_event.hit_zone,last_event.amount,last_event.rewind_ms),24,189,16,last_event_ms&&now-last_event_ms<1500u?(Color){210,180,110,255}:GRAY);
        DrawText(TextFormat("crypto %s | sim %ums +/- %ums %.1f%% loss",strong_crypto?"libsodium":"scaffold",sim_latency,sim_jitter,sim_loss),24,211,16,strong_crypto?SKYBLUE:(Color){180,160,110,255});
        DrawText(TextFormat("mouse %s | capture %s | delta %.1f %.1f | abs-fallback %u | F10 toggle",IsWindowFocused()?"FOCUSED":"UNFOCUSED",mouse_capture.captured?"ON":(mouse_capture.requested?"WAIT":"OFF"),mouse_capture.last_delta.x,mouse_capture.last_delta.y,mouse_capture.absolute_fallback_frames),24,233,16,mouse_capture.captured?GREEN:YELLOW);
        uint8_t local_region=player?nf_region_nearest(&region_graph,player->transform.position):NF_REGION_INVALID;
        DrawText(TextFormat("SPATIAL F4 %s | local region %u %s | graph %zu | graybox 0.40 km^2",spatial_debug?"ON":"OFF",local_region,nf_region_name(local_region),region_graph.count),24,255,16,spatial_debug?SKYBLUE:GRAY);
        DrawCircle(width/2,height/2,2,RAYWHITE);if(now<hit_marker_until)draw_hit_marker(width/2,height/2,GREEN);draw_weapon_hud(player,width,height,now<muzzle_until);
        if(now<damage_flash_until)DrawRectangleLinesEx((Rectangle){4,4,width-8,height-8},8,(Color){74,39,100,255});
        DrawText("v1.1: CONTAMINATION = universal damage/body horror | INFESTATION = Dream Cattler-specific habitat ecology",24,height-32,15,(Color){160,166,178,255});
        EndDrawing();
    }

    if(peer!=NULL)nf_net_disconnect(peer,0);nf_net_flush(&net);EnableCursor();CloseWindow();nf_net_close(&net);nf_net_global_shutdown();return 0;
}
