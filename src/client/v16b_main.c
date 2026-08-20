#include "nf_net.h"
#include "nf_prediction.h"
#include "nf_security.h"
#include "nf_weapon_solution.h"
#include "nf_world.h"
#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define V16B_REMOTE_MAX NF_NET_MAX_SNAPSHOT_ACTORS

typedef struct Remote {
    bool active;
    NfEntityId id;
    NfFaction faction;
    NfVec3 position;
    bool alive;
} Remote;

static float clampf_local(float v,float lo,float hi){return v<lo?lo:(v>hi?hi:v);}
static Vector3 rv(NfVec3 v){return(Vector3){v.x,v.y,v.z};}

static void send_hello(NfNetHost *net,void *peer,const uint8_t resume[NF_NET_TOKEN_BYTES]){
    NfHelloMessage hello={0};nf_security_random(hello.client_nonce,sizeof(hello.client_nonce));memcpy(hello.resume_token,resume,NF_NET_TOKEN_BYTES);uint8_t buffer[128];size_t n=nf_protocol_encode_hello(buffer,sizeof(buffer),&hello);if(n)nf_net_send(net,peer,NF_NET_CHANNEL_RELIABLE,buffer,n,true);
}

static Color faction_color(NfFaction f){
    switch(f){case NF_FACTION_PLAYER:return RAYWHITE;case NF_FACTION_TEAMMATE:return(Color){46,168,255,255};case NF_FACTION_RIVAL:return(Color){214,40,40,255};case NF_FACTION_RANCHER:return(Color){242,140,40,255};default:return GRAY;}
}

static Remote *remote_slot(Remote remotes[],NfEntityId id){
    for(size_t i=0;i<V16B_REMOTE_MAX;++i)if(remotes[i].active&&remotes[i].id==id)return &remotes[i];
    for(size_t i=0;i<V16B_REMOTE_MAX;++i)if(!remotes[i].active){remotes[i]=(Remote){.active=true,.id=id};return &remotes[i];}
    return NULL;
}

static void draw_collider(const NfCollider *c){
    Vector3 center={0.5f*(c->min.x+c->max.x),0.5f*(c->min.y+c->max.y),0.5f*(c->min.z+c->max.z)};
    const float sx=c->max.x-c->min.x,sy=c->max.y-c->min.y,sz=c->max.z-c->min.z;
    Color color=c->kind==NF_COLLIDER_LADDER?(Color){72,191,199,170}:(c->kind==NF_COLLIDER_MOVING_PLATFORM?(Color){112,106,154,255}:(Color){112,117,125,255});
    DrawCube(center,sx,sy,sz,color);DrawCubeWires(center,sx,sy,sz,LIGHTGRAY);
}

int main(int argc,char **argv){
    const char *host="127.0.0.1";uint16_t port=NF_NET_DEFAULT_PORT;
    for(int i=1;i<argc;++i){if(strcmp(argv[i],"--host")==0&&i+1<argc)host=argv[++i];else if(strcmp(argv[i],"--port")==0&&i+1<argc)port=(uint16_t)atoi(argv[++i]);}
    if(!nf_security_init()||!nf_net_global_init()){fprintf(stderr,"nightfall v1.6B: network init failed\n");return 1;}NfNetHost net;if(!nf_net_client_open(&net)){nf_net_global_shutdown();return 1;}void *peer=nf_net_connect(&net,host,port);
    const int width=1280,height=720;InitWindow(width,height,"nightfall!punk v1.6B network client — Weapon Authority / Mobility");SetTargetFPS(120);DisableCursor();
    bool connected=false,welcomed=false;NfEntityId player_id=0;uint8_t token[NF_NET_TOKEN_BYTES]={0};NfWorld world;nf_world_init(&world,20260819u);nf_world_build_movement_lab(&world);NfPrediction prediction;nf_prediction_init(&prediction);Remote remotes[V16B_REMOTE_MAX]={0};
    float yaw=0.0f,pitch=0.0f,displayed_reticle=7.0f;double accumulator=0.0;const double fixed=1.0/(double)NF_TICK_RATE;uint32_t last_snapshot_ms=0,last_event_ms=0;NfCombatEvent last_event={0};bool jump_latched=false,fire_latched=false,reload_latched=false;uint8_t weapon_latched=0u;
    while(!WindowShouldClose()){
        const uint32_t now=nf_net_now_ms();NfNetEvent ev;
        while(nf_net_service(&net,&ev,0)>0){
            if(ev.type==NF_NET_EVENT_CONNECT){connected=true;peer=ev.peer;send_hello(&net,peer,token);}
            else if(ev.type==NF_NET_EVENT_DISCONNECT){connected=false;welcomed=false;peer=NULL;}
            else if(ev.type==NF_NET_EVENT_RECEIVE){const NfMessageType type=nf_protocol_peek_type(ev.data,ev.size);
                if(type==NF_MSG_WELCOME){NfWelcomeMessage w;if(nf_protocol_decode_welcome(ev.data,ev.size,&w)){player_id=w.entity_id;memcpy(token,w.session_token,NF_NET_TOKEN_BYTES);nf_world_init(&world,w.world_seed);nf_world_build_movement_lab(&world);nf_world_spawn_actor_with_id(&world,player_id,NF_FACTION_PLAYER,(NfVec3){0,0.05f,-18});world.tick=w.server_tick;nf_prediction_init(&prediction);welcomed=true;}}
                else if(type==NF_MSG_SNAPSHOT&&welcomed){NfSnapshotMessage snapshot;if(nf_protocol_decode_snapshot(ev.data,ev.size,&snapshot)){last_snapshot_ms=now;for(size_t i=0;i<V16B_REMOTE_MAX;++i)remotes[i].active=false;for(uint8_t i=0;i<snapshot.actor_count;++i){const NfActorNetState *a=&snapshot.actors[i];if(a->id==player_id){nf_prediction_reconcile_v16b(&prediction,&world,player_id,a,snapshot.owner_authority_valid?&snapshot.owner_authority:NULL,snapshot.acknowledged_input,snapshot.server_tick,0.035f,0.08f);}else{Remote *r=remote_slot(remotes,a->id);if(r){r->faction=a->faction;r->position=a->position;r->alive=a->alive;}}}}}
                else if(type==NF_MSG_COMBAT_EVENT){NfCombatEventMessage m;if(nf_protocol_decode_combat_event(ev.data,ev.size,&m)){last_event=m.event;last_event_ms=now;}}
            }
        }

        Vector2 mouse=GetMouseDelta();yaw-=mouse.x*0.0022f;pitch=clampf_local(pitch-mouse.y*0.0022f,-1.45f,1.45f);
        NfMoveInput move={0};move.forward=(IsKeyDown(KEY_W)?1.0f:0.0f)-(IsKeyDown(KEY_S)?1.0f:0.0f);move.strafe=(IsKeyDown(KEY_D)?1.0f:0.0f)-(IsKeyDown(KEY_A)?1.0f:0.0f);move.yaw_radians=yaw;move.crouch_held=IsKeyDown(KEY_LEFT_CONTROL);move.sprint_held=IsKeyDown(KEY_LEFT_SHIFT);move.interact_held=IsKeyDown(KEY_E);if(IsKeyPressed(KEY_SPACE))jump_latched=true;move.jump_pressed=jump_latched;if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))fire_latched=true;if(IsKeyPressed(KEY_R))reload_latched=true;if(IsKeyPressed(KEY_ONE))weapon_latched=1u;if(IsKeyPressed(KEY_TWO))weapon_latched=2u;
        NfCombatInput combat={.fire_held=IsMouseButtonDown(MOUSE_BUTTON_LEFT),.fire_pressed=fire_latched,.reload_pressed=reload_latched,.focus_held=IsMouseButtonDown(MOUSE_BUTTON_RIGHT)||IsKeyDown(KEY_F),.weapon_slot=weapon_latched,.aim_pitch_radians=pitch};
        float frame_dt=clampf_local(GetFrameTime(),0.0f,0.10f);accumulator+=frame_dt;bool first_tick=true;
        while(accumulator>=fixed){if(!first_tick){move.jump_pressed=false;combat.fire_pressed=false;combat.reload_pressed=false;combat.weapon_slot=0u;}if(welcomed&&player_id!=0){NfInputCommand cmd=nf_prediction_make_command(&prediction,world.tick,move,combat);nf_prediction_simulate_command(&world,player_id,&cmd);NfActor *local=nf_world_find_actor(&world,player_id);if(local)nf_prediction_record(&prediction,&cmd,local);NfInputBundle bundle={0};memcpy(bundle.session_token,token,NF_NET_TOKEN_BYTES);nf_prediction_build_redundant_bundle(&prediction,cmd.sequence,&bundle);uint8_t packet[NF_NET_MAX_PACKET_BYTES];size_t n=nf_protocol_encode_input(packet,sizeof(packet),&bundle);if(n&&peer)nf_net_send(&net,peer,NF_NET_CHANNEL_STATE,packet,n,false);if(first_tick){jump_latched=false;fire_latched=false;reload_latched=false;weapon_latched=0u;}}accumulator-=fixed;first_tick=false;}

        const NfActor *player=player_id?nf_world_find_actor_const(&world,player_id):NULL;NfWeaponEnvelope envelope={0};float fov=82.0f;if(player){envelope=nf_weapon_authority_envelope(player,yaw,pitch);fov=82.0f-player->weapon_authority.focus_amount*7.0f;const float target=nf_weapon_reticle_radius_px_for_envelope(&envelope,fov,(float)height);const float alpha=1.0f-expf(-frame_dt/0.055f);displayed_reticle+=(target-displayed_reticle)*alpha;}
        Vector3 eye={0,2,-18},look={0,0,1};if(player){eye=(Vector3){player->transform.position.x,player->transform.position.y+player->movement.eye_height,player->transform.position.z};look=(Vector3){cosf(pitch)*sinf(yaw),sinf(pitch),cosf(pitch)*cosf(yaw)};}Camera3D camera={.position=eye,.target={eye.x+look.x,eye.y+look.y,eye.z+look.z},.up={0,1,0},.fovy=fov,.projection=CAMERA_PERSPECTIVE};
        BeginDrawing();ClearBackground((Color){18,20,24,255});BeginMode3D(camera);DrawGrid(60,1);for(size_t i=0;i<world.collider_count;++i)draw_collider(&world.colliders[i]);for(size_t i=0;i<V16B_REMOTE_MAX;++i)if(remotes[i].active&&remotes[i].alive){const Vector3 p=rv(remotes[i].position);DrawCube((Vector3){p.x,p.y+0.9f,p.z},0.72f,1.8f,0.72f,faction_color(remotes[i].faction));}EndMode3D();
        const int cx=width/2,cy=height/2,r=(int)displayed_reticle;DrawLine(cx-r,cy,cx-r/2,cy,RAYWHITE);DrawLine(cx+r/2,cy,cx+r,cy,RAYWHITE);DrawLine(cx,cy-r,cx,cy-r/2,RAYWHITE);DrawLine(cx,cy+r/2,cx,cy+r,RAYWHITE);DrawCircle(cx,cy,2,(Color){80,220,220,255});
        DrawText("nightfall!punk v1.6B — NETWORK WEAPON AUTHORITY",20,18,22,RAYWHITE);DrawText(TextFormat("NET %s | snapshot age %u ms | cmd %u ack %u",welcomed?"AUTH+PREDICTED":(connected?"HANDSHAKE":"DISCONNECTED"),last_snapshot_ms?now-last_snapshot_ms:0,prediction.next_sequence?prediction.next_sequence-1u:0u,prediction.last_acknowledged),20,48,16,welcomed?GREEN:YELLOW);DrawText(TextFormat("PRED error %.3fm max %.3fm | corrections %u resyncs %u | authority err %.3f max %.3f corrections %u",prediction.last_error,prediction.max_error,prediction.corrections,prediction.resyncs,prediction.last_authority_error,prediction.max_authority_error,prediction.authority_corrections),20,70,16,RAYWHITE);if(player){DrawText(TextFormat("NOW focus %.3f cone %.3f | redirect %.3f support %.3f recoil %.3f | shot ancestry %u",player->weapon_authority.focus_amount,envelope.total_cone_deg,envelope.redirect_cone_deg,envelope.support_cone_deg,envelope.recoil_cone_deg,player->weapon_authority.accepted_shot_sequence),20,92,16,(Color){90,220,220,255});DrawText(TextFormat("INPUT RMB %s LMB %s | %s %u/%u",combat.focus_held?"DOWN":"UP",(combat.fire_held||combat.fire_pressed)?"DOWN":"UP",nf_weapon_name(player->combat.weapon),player->combat.ammo_mag[player->combat.weapon],player->combat.reserve_ammo[player->combat.weapon]),20,114,16,RAYWHITE);}DrawText(TextFormat("LAST AUTH EVENT %s seq %u blocked %s age %ums",nf_combat_event_name(last_event.type),last_event.shot_sequence,last_event.shot_blocked?"YES":"NO",last_event_ms?now-last_event_ms:0),20,136,16,last_event.shot_blocked?ORANGE:GRAY);EndDrawing();
    }
    EnableCursor();CloseWindow();if(peer)nf_net_disconnect(peer,0);nf_net_flush(&net);nf_net_close(&net);nf_net_global_shutdown();return 0;
}
