#include "nf_combat.h"
#include "nf_net.h"
#include "nf_prediction.h"
#include "nf_relations.h"
#include "nf_security.h"
#include "nf_world.h"
#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NF_REMOTE_SLOTS NF_NET_MAX_SNAPSHOT_ACTORS

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
    float health;
    NfWeaponId weapon;
} RemoteActor;

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
static Color collider_color(const NfCollider *c) { if(c->kind==NF_COLLIDER_LADDER)return(Color){235,154,64,105}; if(c->kind==NF_COLLIDER_MOVING_PLATFORM)return(Color){143,104,220,255}; return(Color){95,101,112,255}; }
static void draw_collider(const NfCollider *c) { Vector3 center={0.5f*(c->min.x+c->max.x),0.5f*(c->min.y+c->max.y),0.5f*(c->min.z+c->max.z)}; float sx=c->max.x-c->min.x,sy=c->max.y-c->min.y,sz=c->max.z-c->min.z; DrawCube(center,sx,sy,sz,collider_color(c)); DrawCubeWires(center,sx,sy,sz,(Color){185,191,202,180}); }
static void tri(Vector3 a,Vector3 b,Vector3 c,Color color) { DrawTriangle3D(a,b,c,color); }
static void draw_ramp(const NfRamp *r) { const Color color=(Color){82,119,132,255}; Vector3 low_a,low_b,high_a,high_b; if(r->axis==NF_RAMP_POS_Z||r->axis==NF_RAMP_NEG_Z){bool pos=r->axis==NF_RAMP_POS_Z;float low_z=pos?r->min.z:r->max.z,high_z=pos?r->max.z:r->min.z;low_a=(Vector3){r->min.x,r->min.y,low_z};low_b=(Vector3){r->max.x,r->min.y,low_z};high_a=(Vector3){r->min.x,r->max.y,high_z};high_b=(Vector3){r->max.x,r->max.y,high_z};}else{bool pos=r->axis==NF_RAMP_POS_X;float low_x=pos?r->min.x:r->max.x,high_x=pos?r->max.x:r->min.x;low_a=(Vector3){low_x,r->min.y,r->min.z};low_b=(Vector3){low_x,r->min.y,r->max.z};high_a=(Vector3){high_x,r->max.y,r->min.z};high_b=(Vector3){high_x,r->max.y,r->max.z};} tri(low_a,low_b,high_b,color);tri(low_a,high_b,high_a,color);DrawLine3D(low_a,low_b,LIGHTGRAY);DrawLine3D(low_a,high_a,LIGHTGRAY);DrawLine3D(low_b,high_b,LIGHTGRAY);DrawLine3D(high_a,high_b,LIGHTGRAY); }
static Color candidate_color(NfTraversalType t) { switch(t){case NF_TRAVERSAL_STEP:return GREEN;case NF_TRAVERSAL_VAULT:return YELLOW;case NF_TRAVERSAL_MANTLE:return SKYBLUE;case NF_TRAVERSAL_LADDER:return ORANGE;default:return RED;} }
static RemoteActor *remote_slot(RemoteActor remotes[],NfEntityId id) { for(size_t i=0;i<NF_REMOTE_SLOTS;++i)if(remotes[i].active&&remotes[i].id==id)return &remotes[i]; for(size_t i=0;i<NF_REMOTE_SLOTS;++i){if(!remotes[i].active){memset(&remotes[i],0,sizeof(remotes[i]));remotes[i].active=true;remotes[i].id=id;return &remotes[i];}} return NULL; }
static void send_hello(NfNetHost *net,void *peer,const uint8_t resume[NF_NET_TOKEN_BYTES]) { NfHelloMessage h={0};nf_security_random(h.client_nonce,sizeof(h.client_nonce));memcpy(h.resume_token,resume,NF_NET_TOKEN_BYTES);uint8_t buf[128];size_t n=nf_protocol_encode_hello(buf,sizeof(buf),&h);nf_net_send(net,peer,NF_NET_CHANNEL_RELIABLE,buf,n,true); }
static Color actor_color(NfFaction local,NfFaction remote,bool alive) { if(!alive)return(Color){70,72,78,255}; if(remote==NF_FACTION_RANCHER)return ORANGE; NfRelationship relation=nf_relation_between(local,remote,NF_RELATION_HOSTILE); if(relation==NF_RELATION_COOPERATIVE)return(Color){65,145,235,255}; if(relation==NF_RELATION_TRUCE||relation==NF_RELATION_CONTESTED)return(Color){224,188,70,255}; return(Color){220,72,72,255}; }
static void draw_weapon_hud(const NfActor *player,int width,int height,bool muzzle) { if(player==NULL)return; Color body=player->combat.weapon==NF_WEAPON_PISTOL?(Color){180,184,193,255}:(Color){118,126,139,255};int w=player->combat.weapon==NF_WEAPON_PISTOL?150:260;int h=player->combat.weapon==NF_WEAPON_PISTOL?40:58;DrawRectangle(width-w-70,height-h-72,w,h,body);DrawRectangleLines(width-w-70,height-h-72,w,h,RAYWHITE);if(muzzle){DrawCircle(width-66,height-h-51,15,YELLOW);DrawCircle(width-66,height-h-51,7,ORANGE);} }
static void draw_hit_marker(int cx,int cy,Color color) { DrawLine(cx-13,cy-13,cx-5,cy-5,color);DrawLine(cx+13,cy-13,cx+5,cy-5,color);DrawLine(cx-13,cy+13,cx-5,cy+5,color);DrawLine(cx+13,cy+13,cx+5,cy+5,color); }

static Vector2 update_mouse_capture(NfMouseCapture *state,uint32_t now) {
    bool focused=IsWindowFocused();
    if(focused&&!state->was_focused){state->capture_after_ms=now+80u;state->position_valid=false;}
    if(IsKeyPressed(KEY_F10)){state->requested=!state->requested;state->capture_after_ms=now;state->position_valid=false;}
    if(!focused||!state->requested){if(state->captured){EnableCursor();state->captured=false;}state->position_valid=false;state->last_delta=(Vector2){0};state->was_focused=focused;return state->last_delta;}
    if(!state->captured&&(int32_t)(now-state->capture_after_ms)>=0){DisableCursor();state->captured=true;state->position_valid=false;}
    Vector2 delta={0};
    if(state->captured){
        delta=GetMouseDelta();
        Vector2 position=GetMousePosition();
        if(state->position_valid){Vector2 absolute_delta={position.x-state->previous_position.x,position.y-state->previous_position.y};bool relative_zero=fabsf(delta.x)<0.001f&&fabsf(delta.y)<0.001f;bool absolute_moved=fabsf(absolute_delta.x)>=0.001f||fabsf(absolute_delta.y)>=0.001f;if(relative_zero&&absolute_moved){delta=absolute_delta;++state->absolute_fallback_frames;}}
        state->previous_position=position;
        state->position_valid=true;
    }
    state->last_delta=delta;
    state->was_focused=focused;
    return delta;
}

int main(int argc,char **argv) {
    const char *host="127.0.0.1"; uint16_t port=NF_NET_DEFAULT_PORT; uint32_t sim_latency=0,sim_jitter=0; float sim_loss=0.0f;
    for(int i=1;i<argc;++i){if(strcmp(argv[i],"--host")==0&&i+1<argc)host=argv[++i];else if(strcmp(argv[i],"--port")==0&&i+1<argc)port=(uint16_t)atoi(argv[++i]);else if(strcmp(argv[i],"--sim-latency")==0&&i+1<argc)sim_latency=(uint32_t)atoi(argv[++i]);else if(strcmp(argv[i],"--sim-jitter")==0&&i+1<argc)sim_jitter=(uint32_t)atoi(argv[++i]);else if(strcmp(argv[i],"--sim-loss")==0&&i+1<argc)sim_loss=(float)atof(argv[++i]);}
    if(!nf_security_init()||!nf_net_global_init()){fprintf(stderr,"nightfall: network init failed\n");return 1;}
    NfNetHost net; if(!nf_net_client_open(&net)){nf_net_global_shutdown();return 1;} nf_net_set_simulation(&net,sim_latency,sim_jitter,sim_loss); void *peer=nf_net_connect(&net,host,port);
    const int width=1280,height=720; InitWindow(width,height,"nightfall!punk v0.6 encounter intelligence lab"); SetTargetFPS(144);
    NfMouseCapture mouse_capture={.requested=true,.capture_after_ms=nf_net_now_ms()+80u};
    NfWorld world;nf_world_init(&world,20260807u);nf_world_build_movement_lab(&world); NfPrediction prediction;nf_prediction_init(&prediction); NfEntityId player_id=0; uint8_t session_token[NF_NET_TOKEN_BYTES]={0}; bool welcomed=false,connected=false,strong_crypto=false; uint32_t next_reconnect_ms=0,last_snapshot_ms=0,last_ping_ms=0,last_pong_ms=0; uint64_t last_server_tick=0; uint32_t candidate_disagreements=0; RemoteActor remotes[NF_REMOTE_SLOTS]={0};
    NfCombatEvent last_event={0}; uint32_t last_event_ms=0,hit_marker_until=0,damage_flash_until=0,muzzle_until=0,last_auto_flash=0; unsigned combat_events=0; float yaw=0,pitch=0; double accumulator=0; const double fixed_dt=1.0/(double)NF_TICK_RATE; bool jump_latched=false,fire_latched=false,reload_latched=false; uint8_t weapon_latched=0;
    while(!WindowShouldClose()) {
        uint32_t now=nf_net_now_ms(); NfNetEvent ev;
        while(nf_net_service(&net,&ev,0)>0) {
            if(ev.type==NF_NET_EVENT_CONNECT){connected=true;peer=ev.peer;send_hello(&net,peer,session_token);}
            else if(ev.type==NF_NET_EVENT_DISCONNECT){connected=false;welcomed=false;peer=NULL;next_reconnect_ms=nf_net_now_ms()+1500u;}
            else if(ev.type==NF_NET_EVENT_RECEIVE) {
                NfMessageType t=nf_protocol_peek_type(ev.data,ev.size);
                if(t==NF_MSG_WELCOME){NfWelcomeMessage w;if(nf_protocol_decode_welcome(ev.data,ev.size,&w)){bool first=player_id==0;player_id=w.entity_id;memcpy(session_token,w.session_token,NF_NET_TOKEN_BYTES);strong_crypto=w.strong_crypto;if(first||nf_world_find_actor(&world,player_id)==NULL){nf_world_init(&world,w.world_seed);nf_world_build_movement_lab(&world);nf_world_spawn_actor_with_id(&world,player_id,NF_FACTION_PLAYER,(NfVec3){0,0.05f,-18});}world.tick=w.server_tick;nf_world_sync_dynamic_geometry(&world);nf_prediction_init(&prediction);welcomed=true;}}
                else if(t==NF_MSG_SNAPSHOT&&welcomed){NfSnapshotMessage s;if(nf_protocol_decode_snapshot(ev.data,ev.size,&s)){last_snapshot_ms=nf_net_now_ms();last_server_tick=s.server_tick;for(size_t i=0;i<NF_REMOTE_SLOTS;++i)remotes[i].seen=false;for(uint8_t i=0;i<s.actor_count;++i){const NfActorNetState *a=&s.actors[i];if(a->id==player_id){NfActor *local=nf_world_find_actor(&world,player_id);if(local!=NULL){int predicted_feature=local->movement.candidate.active?local->movement.candidate.feature_index:-1;bool traversal_active=local->movement.mode==NF_MOVE_VAULT||local->movement.mode==NF_MOVE_MANTLE||local->movement.mode==NF_MOVE_LADDER;if(traversal_active&&predicted_feature!=a->candidate_feature)++candidate_disagreements;}nf_prediction_reconcile(&prediction,&world,player_id,a,s.acknowledged_input,s.server_tick,0.035f);}else{RemoteActor *r=remote_slot(remotes,a->id);if(r!=NULL){Vector3 p={a->position.x,a->position.y,a->position.z};if(r->received_ms==0)r->from=p;else r->from=r->to;r->to=p;r->received_ms=last_snapshot_ms;r->mode=a->mode;r->faction=a->faction;r->alive=a->alive;r->health=a->health;r->weapon=a->weapon;r->seen=true;}}}for(size_t i=0;i<NF_REMOTE_SLOTS;++i)if(remotes[i].active&&!remotes[i].seen&&remotes[i].received_ms!=0&&last_snapshot_ms-remotes[i].received_ms>500u)remotes[i].active=false;}}
                else if(t==NF_MSG_COMBAT_EVENT){NfCombatEventMessage m;if(nf_protocol_decode_combat_event(ev.data,ev.size,&m)){last_event=m.event;last_event_ms=nf_net_now_ms();++combat_events;if(m.event.source==player_id&&(m.event.type==NF_COMBAT_EVENT_DAMAGE||m.event.type==NF_COMBAT_EVENT_DEATH))hit_marker_until=last_event_ms+160u;if(m.event.target==player_id&&(m.event.type==NF_COMBAT_EVENT_DAMAGE||m.event.type==NF_COMBAT_EVENT_DEATH))damage_flash_until=last_event_ms+220u;}}
                else if(t==NF_MSG_PONG){NfPingMessage p;if(nf_protocol_decode_ping(ev.data,ev.size,NF_MSG_PONG,&p))last_pong_ms=nf_net_now_ms()-p.stamp_ms;}
                else if(t==NF_MSG_REJECT){uint8_t reason=0;nf_protocol_decode_reject(ev.data,ev.size,&reason);fprintf(stderr,"nightfall: server rejected connection reason=%u\n",reason);}
            }
        }
        now=nf_net_now_ms();
        if(!connected&&peer==NULL&&(int32_t)(now-next_reconnect_ms)>=0){peer=nf_net_connect(&net,host,port);next_reconnect_ms=now+1500u;}
        if(connected&&welcomed&&now-last_ping_ms>1000u){NfPingMessage p={.stamp_ms=now};uint8_t buf[32];size_t n=nf_protocol_encode_ping(buf,sizeof(buf),NF_MSG_PING,&p);nf_net_send(&net,peer,NF_NET_CHANNEL_RELIABLE,buf,n,true);last_ping_ms=now;}
        Vector2 mouse=update_mouse_capture(&mouse_capture,now);yaw-=mouse.x*0.0022f;pitch=clampf_local(pitch-mouse.y*0.0022f,-1.45f,1.45f);
        NfMoveInput input={0};input.forward=(IsKeyDown(KEY_W)?1.0f:0.0f)-(IsKeyDown(KEY_S)?1.0f:0.0f);input.strafe=(IsKeyDown(KEY_A)?1.0f:0.0f)-(IsKeyDown(KEY_D)?1.0f:0.0f);input.yaw_radians=yaw;if(IsKeyPressed(KEY_SPACE))jump_latched=true;input.jump_pressed=jump_latched;input.crouch_held=IsKeyDown(KEY_LEFT_CONTROL);input.sprint_held=IsKeyDown(KEY_LEFT_SHIFT);input.interact_held=IsKeyDown(KEY_E);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){fire_latched=true;muzzle_until=now+55u;} if(IsKeyPressed(KEY_R))reload_latched=true; if(IsKeyPressed(KEY_ONE))weapon_latched=1u; if(IsKeyPressed(KEY_TWO))weapon_latched=2u;
        const NfActor *pre_player=player_id?nf_world_find_actor_const(&world,player_id):NULL; if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)&&pre_player!=NULL&&pre_player->combat.weapon==NF_WEAPON_CARBINE&&now-last_auto_flash>95u){muzzle_until=now+45u;last_auto_flash=now;}
        NfCombatInput combat={.fire_held=IsMouseButtonDown(MOUSE_BUTTON_LEFT),.fire_pressed=fire_latched,.reload_pressed=reload_latched,.weapon_slot=weapon_latched,.aim_pitch_radians=pitch};
        double frame_dt=GetFrameTime();if(frame_dt>0.10)frame_dt=0.10;accumulator+=frame_dt;bool first_tick=true;
        while(accumulator>=fixed_dt){if(!first_tick){input.jump_pressed=false;combat.fire_pressed=false;combat.reload_pressed=false;combat.weapon_slot=0u;}if(welcomed&&player_id!=0){NfInputCommand cmd=nf_prediction_make_command(&prediction,world.tick,input,combat);nf_world_set_input(&world,player_id,cmd.move);nf_world_step(&world,(float)fixed_dt);NfActor *local=nf_world_find_actor(&world,player_id);nf_prediction_record(&prediction,&cmd,local);NfInputBundle bundle={0};memcpy(bundle.session_token,session_token,NF_NET_TOKEN_BYTES);nf_prediction_build_redundant_bundle(&prediction,cmd.sequence,&bundle);uint8_t buf[NF_NET_MAX_PACKET_BYTES];size_t n=nf_protocol_encode_input(buf,sizeof(buf),&bundle);if(n)nf_net_send(&net,peer,NF_NET_CHANNEL_STATE,buf,n,false);if(first_tick){if(input.jump_pressed)jump_latched=false;if(combat.fire_pressed)fire_latched=false;if(combat.reload_pressed)reload_latched=false;if(combat.weapon_slot)weapon_latched=0u;}}accumulator-=fixed_dt;first_tick=false;}
        const NfActor *player=player_id?nf_world_find_actor_const(&world,player_id):NULL; Vector3 eye={0,2,-18},look={0,0,1}; if(player!=NULL){eye=(Vector3){player->transform.position.x,player->transform.position.y+player->movement.eye_height,player->transform.position.z};look=(Vector3){cosf(pitch)*sinf(yaw),sinf(pitch),cosf(pitch)*cosf(yaw)};} Camera3D camera={.position=eye,.target={eye.x+look.x,eye.y+look.y,eye.z+look.z},.up={0,1,0},.fovy=82,.projection=CAMERA_PERSPECTIVE};
        BeginDrawing();ClearBackground((Color){18,20,24,255});BeginMode3D(camera);DrawGrid(60,1);for(size_t i=0;i<world.collider_count;++i)draw_collider(&world.colliders[i]);for(size_t i=0;i<world.ramp_count;++i)draw_ramp(&world.ramps[i]);NfFaction local_faction=player?player->faction:NF_FACTION_PLAYER;for(size_t i=0;i<NF_REMOTE_SLOTS;++i){if(!remotes[i].active)continue;float a=clampf_local((float)(now-remotes[i].received_ms)/(1000.0f/(float)NF_NET_SNAPSHOT_HZ),0,1);Vector3 p={remotes[i].from.x+(remotes[i].to.x-remotes[i].from.x)*a,remotes[i].from.y+(remotes[i].to.y-remotes[i].from.y)*a,remotes[i].from.z+(remotes[i].to.z-remotes[i].from.z)*a};float body_h=remotes[i].alive?1.8f:0.25f;Color color=actor_color(local_faction,remotes[i].faction,remotes[i].alive);DrawCube((Vector3){p.x,p.y+body_h*0.5f,p.z},0.72f,body_h,0.72f,color);DrawCubeWires((Vector3){p.x,p.y+body_h*0.5f,p.z},0.72f,body_h,0.72f,RAYWHITE);}if(player!=NULL&&player->movement.candidate.active){const NfTraversalCandidate *c=&player->movement.candidate;Vector3 p={c->point.x,c->point.y+0.15f,c->point.z};DrawSphere(p,0.14f,candidate_color(c->type));}EndMode3D();
        DrawText("nightfall!punk v0.6 - ENCOUNTER INTELLIGENCE LAB",24,18,23,RAYWHITE);DrawText("red actors = Human Rival AI | WASD move | Shift sprint | Ctrl crouch | Space jump | E ladder | LMB fire | R reload | 1/2 weapons",24,47,15,GRAY);const char *status=welcomed?"AUTHORITATIVE + PREDICTED":(connected?"HANDSHAKE":"RECONNECTING");DrawText(TextFormat("NET %s | ENet RTT %u ms | effective ping %u ms | snapshot age %u ms",status,peer?nf_net_peer_rtt_ms(peer):0,last_pong_ms,last_snapshot_ms?now-last_snapshot_ms:0),24,79,16,welcomed?GREEN:YELLOW);DrawText(TextFormat("server tick %llu | client tick %llu | cmd %u ack %u pending %u",(unsigned long long)last_server_tick,(unsigned long long)world.tick,prediction.next_sequence?prediction.next_sequence-1:0,prediction.last_acknowledged,(prediction.next_sequence-1)-prediction.last_acknowledged),24,101,16,RAYWHITE);DrawText(TextFormat("prediction %.3fm | max %.3fm | visible corrections %u | resyncs %u | traversal disagreements %u",prediction.last_error,prediction.max_error,prediction.corrections,prediction.resyncs,candidate_disagreements),24,123,16,RAYWHITE);
        if(player!=NULL){float speed=sqrtf(player->transform.velocity.x*player->transform.velocity.x+player->transform.velocity.z*player->transform.velocity.z);DrawText(TextFormat("MOVE %s %.2f m/s | health %.0f | %s %u/%u | %s",nf_movement_mode_name(player->movement.mode),speed,player->health,nf_weapon_name(player->combat.weapon),player->combat.ammo_mag[player->combat.weapon],player->combat.reserve_ammo[player->combat.weapon],nf_weapon_state_name(player->combat.state)),24,145,16,RAYWHITE);if(!player->combat.alive)DrawText("DEAD - authoritative respawn pending",width/2-190,height/2+45,20,RED);}
        DrawText(TextFormat("combat events %u | last %s src %u tgt %u zone %u dmg %.0f rewind %ums",combat_events,nf_combat_event_name(last_event.type),last_event.source,last_event.target,(unsigned)last_event.hit_zone,last_event.amount,last_event.rewind_ms),24,167,16,last_event_ms&&now-last_event_ms<1500u?YELLOW:GRAY);DrawText(TextFormat("crypto %s | sim %ums +/- %ums %.1f%% loss",strong_crypto?"libsodium":"scaffold",sim_latency,sim_jitter,sim_loss),24,189,16,strong_crypto?SKYBLUE:ORANGE);DrawText(TextFormat("mouse %s | capture %s | delta %.1f %.1f | abs-fallback %u | F10 toggle",IsWindowFocused()?"FOCUSED":"UNFOCUSED",mouse_capture.captured?"ON":(mouse_capture.requested?"WAIT":"OFF"),mouse_capture.last_delta.x,mouse_capture.last_delta.y,mouse_capture.absolute_fallback_frames),24,211,16,mouse_capture.captured?GREEN:YELLOW);DrawCircle(width/2,height/2,2,RAYWHITE);if(now<hit_marker_until)draw_hit_marker(width/2,height/2,GREEN);draw_weapon_hud(player,width,height,now<muzzle_until);if(now<damage_flash_until)DrawRectangleLinesEx((Rectangle){4,4,width-8,height-8},8,RED);DrawText("v0.6 bounds squad pressure and aim tracking while preserving authoritative movement and combat truth",24,height-32,15,(Color){160,166,178,255});EndDrawing();
    }
    if(peer!=NULL) nf_net_disconnect(peer,0);
    nf_net_flush(&net);
    EnableCursor();
    CloseWindow();
    nf_net_close(&net);
    nf_net_global_shutdown();
    return 0;
}
