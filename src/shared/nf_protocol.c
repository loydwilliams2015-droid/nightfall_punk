#include "nf_protocol.h"

#include <string.h>

#define NF_HEADER_BYTES 8u
#define NF_COMMAND_BYTES 31u
#define NF_ACTOR_BYTES 52u
#define NF_COMBAT_EVENT_BYTES 49u

static void put_u16(uint8_t *p, uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void put_u32(uint8_t *p,uint32_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);p[2]=(uint8_t)(v>>16);p[3]=(uint8_t)(v>>24);}
static void put_u64(uint8_t *p,uint64_t v){for(unsigned i=0;i<8;++i)p[i]=(uint8_t)(v>>(8u*i));}
static uint16_t get_u16(const uint8_t *p){return (uint16_t)p[0]|((uint16_t)p[1]<<8);}
static uint32_t get_u32(const uint8_t *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static uint64_t get_u64(const uint8_t *p){uint64_t v=0;for(unsigned i=0;i<8;++i)v|=(uint64_t)p[i]<<(8u*i);return v;}
static void put_f32(uint8_t *p,float v){uint32_t u;memcpy(&u,&v,sizeof(u));put_u32(p,u);}
static float get_f32(const uint8_t *p){uint32_t u=get_u32(p);float v;memcpy(&v,&u,sizeof(v));return v;}

static size_t write_header(uint8_t *out,size_t cap,NfMessageType type){if(out==NULL||cap<NF_HEADER_BYTES)return 0;put_u32(out,NF_PROTOCOL_MAGIC);put_u16(out+4,NF_PROTOCOL_VERSION);out[6]=(uint8_t)type;out[7]=0;return NF_HEADER_BYTES;}
static bool read_header(const uint8_t *data,size_t size,NfMessageType expected){return data!=NULL&&size>=NF_HEADER_BYTES&&get_u32(data)==NF_PROTOCOL_MAGIC&&get_u16(data+4)==NF_PROTOCOL_VERSION&&(expected==NF_MSG_NONE||data[6]==(uint8_t)expected);}

NfMessageType nf_protocol_peek_type(const uint8_t *data,size_t size){if(!read_header(data,size,NF_MSG_NONE))return NF_MSG_NONE;return (NfMessageType)data[6];}

size_t nf_protocol_encode_hello(uint8_t *out,size_t cap,const NfHelloMessage *msg){const size_t need=NF_HEADER_BYTES+NF_NET_NONCE_BYTES+NF_NET_TOKEN_BYTES;if(msg==NULL||cap<need)return 0;write_header(out,cap,NF_MSG_HELLO);memcpy(out+8,msg->client_nonce,NF_NET_NONCE_BYTES);memcpy(out+8+NF_NET_NONCE_BYTES,msg->resume_token,NF_NET_TOKEN_BYTES);return need;}
bool nf_protocol_decode_hello(const uint8_t *data,size_t size,NfHelloMessage *out){const size_t need=NF_HEADER_BYTES+NF_NET_NONCE_BYTES+NF_NET_TOKEN_BYTES;if(out==NULL||size!=need||!read_header(data,size,NF_MSG_HELLO))return false;memcpy(out->client_nonce,data+8,NF_NET_NONCE_BYTES);memcpy(out->resume_token,data+8+NF_NET_NONCE_BYTES,NF_NET_TOKEN_BYTES);return true;}

size_t nf_protocol_encode_welcome(uint8_t *out,size_t cap,const NfWelcomeMessage *msg){const size_t need=NF_HEADER_BYTES+4+8+4+NF_NET_TOKEN_BYTES+1;if(msg==NULL||cap<need)return 0;write_header(out,cap,NF_MSG_WELCOME);size_t o=8;put_u32(out+o,msg->entity_id);o+=4;put_u64(out+o,msg->server_tick);o+=8;put_u32(out+o,msg->world_seed);o+=4;memcpy(out+o,msg->session_token,NF_NET_TOKEN_BYTES);o+=NF_NET_TOKEN_BYTES;out[o++]=msg->strong_crypto?1u:0u;return o;}
bool nf_protocol_decode_welcome(const uint8_t *data,size_t size,NfWelcomeMessage *out){const size_t need=NF_HEADER_BYTES+4+8+4+NF_NET_TOKEN_BYTES+1;if(out==NULL||size!=need||!read_header(data,size,NF_MSG_WELCOME))return false;size_t o=8;out->entity_id=get_u32(data+o);o+=4;out->server_tick=get_u64(data+o);o+=8;out->world_seed=get_u32(data+o);o+=4;memcpy(out->session_token,data+o,NF_NET_TOKEN_BYTES);o+=NF_NET_TOKEN_BYTES;out->strong_crypto=data[o]!=0;return true;}

size_t nf_protocol_encode_reject(uint8_t *out,size_t cap,uint8_t reason){if(cap<NF_HEADER_BYTES+1)return 0;write_header(out,cap,NF_MSG_REJECT);out[8]=reason;return 9;}
bool nf_protocol_decode_reject(const uint8_t *data,size_t size,uint8_t *reason){if(reason==NULL||size!=9||!read_header(data,size,NF_MSG_REJECT))return false;*reason=data[8];return true;}

static size_t encode_command(uint8_t *out,size_t cap,const NfInputCommand *c){
    if(cap<NF_COMMAND_BYTES||c==NULL)return 0;size_t o=0;
    put_u32(out+o,c->sequence);o+=4;put_u64(out+o,c->client_tick);o+=8;
    put_f32(out+o,c->move.forward);o+=4;put_f32(out+o,c->move.strafe);o+=4;put_f32(out+o,c->move.yaw_radians);o+=4;
    uint8_t move_flags=0;if(c->move.jump_pressed)move_flags|=1u;if(c->move.crouch_held)move_flags|=2u;if(c->move.sprint_held)move_flags|=4u;if(c->move.interact_held)move_flags|=8u;out[o++]=move_flags;
    uint8_t combat_flags=0;if(c->combat.fire_held)combat_flags|=1u;if(c->combat.fire_pressed)combat_flags|=2u;if(c->combat.reload_pressed)combat_flags|=4u;out[o++]=combat_flags;
    out[o++]=c->combat.weapon_slot;put_f32(out+o,c->combat.aim_pitch_radians);o+=4;
    return o;
}
static bool decode_command(const uint8_t *data,size_t size,NfInputCommand *c){
    if(size<NF_COMMAND_BYTES||c==NULL)return false;size_t o=0;memset(c,0,sizeof(*c));
    c->sequence=get_u32(data+o);o+=4;c->client_tick=get_u64(data+o);o+=8;
    c->move.forward=get_f32(data+o);o+=4;c->move.strafe=get_f32(data+o);o+=4;c->move.yaw_radians=get_f32(data+o);o+=4;
    uint8_t f=data[o++];c->move.jump_pressed=(f&1u)!=0;c->move.crouch_held=(f&2u)!=0;c->move.sprint_held=(f&4u)!=0;c->move.interact_held=(f&8u)!=0;
    f=data[o++];c->combat.fire_held=(f&1u)!=0;c->combat.fire_pressed=(f&2u)!=0;c->combat.reload_pressed=(f&4u)!=0;c->combat.weapon_slot=data[o++];c->combat.aim_pitch_radians=get_f32(data+o);return true;
}

size_t nf_protocol_encode_input(uint8_t *out,size_t cap,const NfInputBundle *msg){if(msg==NULL||msg->count==0||msg->count>NF_NET_INPUT_REDUNDANCY)return 0;const size_t need=NF_HEADER_BYTES+NF_NET_TOKEN_BYTES+1+(size_t)msg->count*NF_COMMAND_BYTES;if(cap<need)return 0;write_header(out,cap,NF_MSG_INPUT);size_t o=8;memcpy(out+o,msg->session_token,NF_NET_TOKEN_BYTES);o+=NF_NET_TOKEN_BYTES;out[o++]=msg->count;for(uint8_t i=0;i<msg->count;++i)o+=encode_command(out+o,cap-o,&msg->commands[i]);return o;}
bool nf_protocol_decode_input(const uint8_t *data,size_t size,NfInputBundle *out){if(out==NULL||size<NF_HEADER_BYTES+NF_NET_TOKEN_BYTES+1||!read_header(data,size,NF_MSG_INPUT))return false;size_t o=8;memset(out,0,sizeof(*out));memcpy(out->session_token,data+o,NF_NET_TOKEN_BYTES);o+=NF_NET_TOKEN_BYTES;out->count=data[o++];if(out->count==0||out->count>NF_NET_INPUT_REDUNDANCY||size!=o+(size_t)out->count*NF_COMMAND_BYTES)return false;for(uint8_t i=0;i<out->count;++i){if(!decode_command(data+o,size-o,&out->commands[i]))return false;o+=NF_COMMAND_BYTES;}return true;}

static size_t encode_actor(uint8_t *out,size_t cap,const NfActorNetState *a){
    if(cap<NF_ACTOR_BYTES||a==NULL)return 0;size_t o=0;
    put_u32(out+o,a->id);o+=4;out[o++]=(uint8_t)a->faction;
    put_f32(out+o,a->position.x);o+=4;put_f32(out+o,a->position.y);o+=4;put_f32(out+o,a->position.z);o+=4;
    put_f32(out+o,a->velocity.x);o+=4;put_f32(out+o,a->velocity.y);o+=4;put_f32(out+o,a->velocity.z);o+=4;
    out[o++]=(uint8_t)a->mode;uint8_t flags=(a->grounded?1u:0u)|(a->crouched?2u:0u);out[o++]=flags;out[o++]=a->jumps_used;
    put_u32(out+o,(uint32_t)a->attached_collider);o+=4;out[o++]=(uint8_t)a->candidate_type;put_u32(out+o,(uint32_t)a->candidate_feature);o+=4;
    put_f32(out+o,a->health);o+=4;out[o++]=a->alive?1u:0u;out[o++]=(uint8_t)a->weapon;out[o++]=(uint8_t)a->weapon_state;put_u16(out+o,a->ammo_mag);o+=2;put_u16(out+o,a->reserve_ammo);o+=2;
    return o;
}
static bool decode_actor(const uint8_t *data,size_t size,NfActorNetState *a){
    if(size<NF_ACTOR_BYTES||a==NULL)return false;size_t o=0;memset(a,0,sizeof(*a));
    a->id=get_u32(data+o);o+=4;a->faction=(NfFaction)data[o++];
    a->position.x=get_f32(data+o);o+=4;a->position.y=get_f32(data+o);o+=4;a->position.z=get_f32(data+o);o+=4;
    a->velocity.x=get_f32(data+o);o+=4;a->velocity.y=get_f32(data+o);o+=4;a->velocity.z=get_f32(data+o);o+=4;
    a->mode=(NfMovementMode)data[o++];uint8_t f=data[o++];a->grounded=(f&1u)!=0;a->crouched=(f&2u)!=0;a->jumps_used=data[o++];
    a->attached_collider=(int32_t)get_u32(data+o);o+=4;a->candidate_type=(NfTraversalType)data[o++];a->candidate_feature=(int32_t)get_u32(data+o);o+=4;
    a->health=get_f32(data+o);o+=4;a->alive=data[o++]!=0;a->weapon=(NfWeaponId)data[o++];a->weapon_state=(NfWeaponState)data[o++];a->ammo_mag=get_u16(data+o);o+=2;a->reserve_ammo=get_u16(data+o);return true;
}

size_t nf_protocol_encode_snapshot(uint8_t *out,size_t cap,const NfSnapshotMessage *msg){if(msg==NULL||msg->actor_count>NF_NET_MAX_SNAPSHOT_ACTORS)return 0;const size_t need=NF_HEADER_BYTES+8+4+1+(size_t)msg->actor_count*NF_ACTOR_BYTES;if(cap<need)return 0;write_header(out,cap,NF_MSG_SNAPSHOT);size_t o=8;put_u64(out+o,msg->server_tick);o+=8;put_u32(out+o,msg->acknowledged_input);o+=4;out[o++]=msg->actor_count;for(uint8_t i=0;i<msg->actor_count;++i)o+=encode_actor(out+o,cap-o,&msg->actors[i]);return o;}
bool nf_protocol_decode_snapshot(const uint8_t *data,size_t size,NfSnapshotMessage *out){if(out==NULL||size<NF_HEADER_BYTES+13||!read_header(data,size,NF_MSG_SNAPSHOT))return false;size_t o=8;memset(out,0,sizeof(*out));out->server_tick=get_u64(data+o);o+=8;out->acknowledged_input=get_u32(data+o);o+=4;out->actor_count=data[o++];if(out->actor_count>NF_NET_MAX_SNAPSHOT_ACTORS||size!=o+(size_t)out->actor_count*NF_ACTOR_BYTES)return false;for(uint8_t i=0;i<out->actor_count;++i){if(!decode_actor(data+o,size-o,&out->actors[i]))return false;o+=NF_ACTOR_BYTES;}return true;}

size_t nf_protocol_encode_ping(uint8_t *out,size_t cap,NfMessageType type,const NfPingMessage *msg){if(msg==NULL||(type!=NF_MSG_PING&&type!=NF_MSG_PONG)||cap<NF_HEADER_BYTES+4)return 0;write_header(out,cap,type);put_u32(out+8,msg->stamp_ms);return 12;}
bool nf_protocol_decode_ping(const uint8_t *data,size_t size,NfMessageType expected,NfPingMessage *out){if(out==NULL||size!=12||!read_header(data,size,expected))return false;out->stamp_ms=get_u32(data+8);return true;}

size_t nf_protocol_encode_combat_event(uint8_t *out,size_t cap,const NfCombatEventMessage *msg){
    if(msg==NULL||cap<NF_COMBAT_EVENT_BYTES)return 0;write_header(out,cap,NF_MSG_COMBAT_EVENT);const NfCombatEvent *e=&msg->event;size_t o=8;
    put_u32(out+o,e->sequence);o+=4;put_u64(out+o,e->server_tick);o+=8;out[o++]=(uint8_t)e->type;put_u32(out+o,e->source);o+=4;put_u32(out+o,e->target);o+=4;out[o++]=(uint8_t)e->weapon;out[o++]=(uint8_t)e->hit_zone;put_f32(out+o,e->amount);o+=4;put_f32(out+o,e->position.x);o+=4;put_f32(out+o,e->position.y);o+=4;put_f32(out+o,e->position.z);o+=4;put_u16(out+o,e->rewind_ms);o+=2;return o;
}
bool nf_protocol_decode_combat_event(const uint8_t *data,size_t size,NfCombatEventMessage *out){
    if(out==NULL||size!=NF_COMBAT_EVENT_BYTES||!read_header(data,size,NF_MSG_COMBAT_EVENT))return false;memset(out,0,sizeof(*out));NfCombatEvent *e=&out->event;size_t o=8;
    e->sequence=get_u32(data+o);o+=4;e->server_tick=get_u64(data+o);o+=8;e->type=(NfCombatEventType)data[o++];e->source=get_u32(data+o);o+=4;e->target=get_u32(data+o);o+=4;e->weapon=(NfWeaponId)data[o++];e->hit_zone=(NfHitZone)data[o++];e->amount=get_f32(data+o);o+=4;e->position.x=get_f32(data+o);o+=4;e->position.y=get_f32(data+o);o+=4;e->position.z=get_f32(data+o);o+=4;e->rewind_ms=get_u16(data+o);return true;
}

void nf_actor_to_net_state(const NfActor *actor,NfActorNetState *out){
    if(actor==NULL||out==NULL)return;memset(out,0,sizeof(*out));
    *out=(NfActorNetState){.id=actor->id,.faction=actor->faction,.position=actor->transform.position,.velocity=actor->transform.velocity,.mode=actor->movement.mode,.grounded=actor->movement.grounded,.crouched=actor->movement.crouched,.jumps_used=actor->movement.jumps_used,.attached_collider=actor->movement.attached_collider,.candidate_type=actor->movement.candidate.active?actor->movement.candidate.type:NF_TRAVERSAL_NONE,.candidate_feature=actor->movement.candidate.active?actor->movement.candidate.feature_index:-1,.health=actor->health,.alive=actor->combat.alive,.weapon=actor->combat.weapon,.weapon_state=actor->combat.state,.ammo_mag=actor->combat.ammo_mag[actor->combat.weapon],.reserve_ammo=actor->combat.reserve_ammo[actor->combat.weapon]};
}
void nf_actor_apply_combat_net_state(NfActor *actor,const NfActorNetState *s){if(actor==NULL||s==NULL)return;actor->faction=s->faction;actor->health=s->health;actor->combat.alive=s->alive;actor->combat.weapon=s->weapon;actor->combat.state=s->weapon_state;if(s->weapon>NF_WEAPON_NONE&&s->weapon<NF_WEAPON_COUNT){actor->combat.ammo_mag[s->weapon]=s->ammo_mag;actor->combat.reserve_ammo[s->weapon]=s->reserve_ammo;}}
void nf_actor_apply_net_state(NfActor *actor,const NfActorNetState *s,const NfMovementConfig *cfg){if(actor==NULL||s==NULL||cfg==NULL)return;actor->transform.position=s->position;actor->transform.velocity=s->velocity;actor->movement.mode=s->mode;actor->movement.grounded=s->grounded;actor->movement.crouched=s->crouched;actor->movement.jumps_used=s->jumps_used;actor->movement.attached_collider=s->attached_collider;actor->movement.body_height=s->crouched?cfg->crouch_height:cfg->stand_height;actor->movement.eye_height=s->crouched?cfg->crouch_eye_height:cfg->stand_eye_height;actor->movement.candidate.active=s->candidate_type!=NF_TRAVERSAL_NONE;actor->movement.candidate.type=s->candidate_type;actor->movement.candidate.feature_index=s->candidate_feature;nf_actor_apply_combat_net_state(actor,s);}
