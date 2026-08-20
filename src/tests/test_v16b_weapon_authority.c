#include "nf_prediction.h"
#include "nf_protocol.h"
#include "nf_weapon_geometry.h"
#include "nf_weapon_solution.h"
#include "nf_world.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static void protocol_focus_and_packet_budget(void) {
    uint8_t buffer[NF_NET_MAX_PACKET_BYTES];
    NfInputBundle input={.count=1};
    memset(input.session_token,0x42,sizeof(input.session_token));
    input.commands[0].sequence=9u;
    input.commands[0].client_tick=77u;
    input.commands[0].combat.focus_held=true;
    NfInputBundle decoded={0};
    const size_t input_size=nf_protocol_encode_input(buffer,sizeof(buffer),&input);
    assert(input_size>0u);
    assert(nf_protocol_decode_input(buffer,input_size,&decoded));
    assert(decoded.commands[0].combat.focus_held);

    NfSnapshotMessage snapshot={.server_tick=500u,.acknowledged_input=9u,.actor_count=NF_NET_MAX_SNAPSHOT_ACTORS,.owner_authority_valid=true};
    for(uint8_t i=0;i<snapshot.actor_count;++i){
        snapshot.actors[i].id=(NfEntityId)(i+1u);
        snapshot.actors[i].faction=NF_FACTION_RIVAL;
        snapshot.actors[i].weapon=NF_WEAPON_CARBINE;
        snapshot.actors[i].alive=true;
    }
    snapshot.owner_authority.actor_id=1u;
    snapshot.owner_authority.focus_amount=0.625f;
    snapshot.owner_authority.instability_deg[NF_WEAPON_CARBINE]=0.41f;
    snapshot.owner_authority.accepted_shot_sequence=27u;
    snapshot.owner_authority.weapon=NF_WEAPON_CARBINE;
    snapshot.owner_authority.weapon_state=NF_WEAPON_READY;
    snapshot.owner_authority.ammo_mag[NF_WEAPON_CARBINE]=18u;
    snapshot.owner_authority.reserve_ammo[NF_WEAPON_CARBINE]=90u;
    const size_t snapshot_size=nf_protocol_encode_snapshot(buffer,sizeof(buffer),&snapshot);
    assert(snapshot_size==1186u);
    assert(snapshot_size<=NF_NET_MAX_PACKET_BYTES);
    NfSnapshotMessage snapshot2={0};
    assert(nf_protocol_decode_snapshot(buffer,snapshot_size,&snapshot2));
    assert(snapshot2.owner_authority_valid);
    assert(snapshot2.owner_authority.accepted_shot_sequence==27u);
    assert(fabsf(snapshot2.owner_authority.focus_amount-0.625f)<1e-6f);
}

static void accepted_shot_ancestry(void) {
    NfWorld world;nf_world_init(&world,20260819u);
    const NfEntityId id=nf_world_spawn_actor_with_id(&world,7u,NF_FACTION_PLAYER,(NfVec3){0,0,0});
    NfActor *actor=nf_world_find_actor(&world,id);
    NfCombatInput fire={.fire_pressed=true};
    NfWeaponSolution shot={0};
    assert(nf_weapon_authority_try_fire(actor,&fire,100u,world.tick,world.seed,0.0f,&shot,NULL));
    assert(actor->weapon_authority.accepted_shot_sequence==1u);
    assert(shot.shot_sequence==1u);

    /* Cooldown rejection and arbitrary input ancestry do not consume ballistic ancestry. */
    assert(!nf_weapon_authority_try_fire(actor,&fire,401u,world.tick,world.seed,0.0f,NULL,NULL));
    assert(actor->weapon_authority.accepted_shot_sequence==1u);
    for(int i=0;i<12;++i)nf_combat_step_actor(actor,1.0f/(float)NF_TICK_RATE);
    assert(nf_weapon_authority_try_fire(actor,&fire,999u,world.tick,world.seed,0.0f,&shot,NULL));
    assert(actor->weapon_authority.accepted_shot_sequence==2u);
    assert(shot.shot_sequence==2u);
}

static void reload_focus_timing(void) {
    NfWorld world;nf_world_init(&world,1u);
    const NfEntityId id=nf_world_spawn_actor_with_id(&world,2u,NF_FACTION_PLAYER,(NfVec3){0,0,0});
    NfActor *actor=nf_world_find_actor(&world,id);
    actor->combat.state=NF_WEAPON_RELOADING;
    actor->combat.reload_total=1.0f;
    actor->combat.action_timer=1.0f;
    const float dt=1.0f/(float)NF_TICK_RATE;
    int acquire=0;
    while(actor->weapon_authority.focus_amount<0.95f&&acquire<120){nf_weapon_authority_step_actor(actor,true,dt);++acquire;}
    const float acquire_s=(float)acquire*dt;
    assert(acquire_s>=0.35f&&acquire_s<=0.50f);
    int release=0;
    while(actor->weapon_authority.focus_amount>0.05f&&release<120){nf_weapon_authority_step_actor(actor,false,dt);++release;}
    const float release_s=(float)release*dt;
    assert(release_s>=0.10f&&release_s<=0.20f);
}

static void mobility_stress_is_embodied(void) {
    NfWorld world;nf_world_init(&world,2u);
    const NfEntityId id=nf_world_spawn_actor_with_id(&world,3u,NF_FACTION_PLAYER,(NfVec3){0,0,0});
    NfActor *actor=nf_world_find_actor(&world,id);
    actor->movement.grounded=true;actor->movement.mode=NF_MOVE_GROUND;
    actor->transform.velocity=(NfVec3){0,0,6};
    nf_weapon_authority_step_actor(actor,false,1.0f/(float)NF_TICK_RATE);
    const float before=actor->weapon_authority.redirect_stress_deg;
    actor->transform.velocity=(NfVec3){6,0,0};
    nf_weapon_authority_step_actor(actor,false,1.0f/(float)NF_TICK_RATE);
    assert(actor->weapon_authority.redirect_stress_deg>before+0.05f);

    const float redirected=nf_weapon_authority_envelope(actor,0.0f,0.0f).total_cone_deg;
    actor->weapon_authority.redirect_stress_deg=0.0f;
    const float straight=nf_weapon_authority_envelope(actor,0.0f,0.0f).total_cone_deg;
    assert(redirected>straight);

    /* Semantic affordance alone is not a ballistic cause. */
    actor->movement.candidate.active=true;
    actor->movement.candidate.score=0.99f;
    const float candidate=nf_weapon_authority_envelope(actor,0.0f,0.0f).total_cone_deg;
    assert(fabsf(candidate-straight)<1e-7f);
}

static void weapon_state_isolation(void) {
    NfWorld world;nf_world_init(&world,3u);
    const NfEntityId id=nf_world_spawn_actor_with_id(&world,4u,NF_FACTION_PLAYER,(NfVec3){0,0,0});
    NfActor *actor=nf_world_find_actor(&world,id);
    actor->weapon_authority.focus_amount=0.72f;
    actor->weapon_authority.instability_deg[NF_WEAPON_CARBINE]=0.80f;
    actor->weapon_authority.instability_deg[NF_WEAPON_PISTOL]=0.05f;
    actor->combat.weapon=NF_WEAPON_PISTOL;
    nf_weapon_authority_step_actor(actor,true,1.0f/(float)NF_TICK_RATE);
    assert(actor->weapon_authority.focus_amount>0.72f);
    assert(actor->weapon_authority.instability_deg[NF_WEAPON_PISTOL]<0.05f);
    assert(actor->weapon_authority.instability_deg[NF_WEAPON_CARBINE]>0.70f);
}

static void physical_muzzle_and_world_block(void) {
    NfWorld world;nf_world_init(&world,4u);
    nf_world_add_collider(&world,NF_COLLIDER_SOLID,(NfVec3){-1.0f,0.0f,0.70f},(NfVec3){1.0f,1.50f,0.80f});
    const NfEntityId id=nf_world_spawn_actor_with_id(&world,5u,NF_FACTION_PLAYER,(NfVec3){0,0,0});
    NfActor *actor=nf_world_find_actor(&world,id);
    actor->movement.eye_height=1.62f;
    const NfVec3 muzzle=nf_weapon_muzzle_origin(actor,0.0f);
    assert(muzzle.z>0.40f&&muzzle.y<1.50f);
    NfWorldRayHit hit={0};
    assert(nf_weapon_world_first_hit(&world,muzzle,(NfVec3){0,0,1},10.0f,&hit));
    assert(hit.distance<1.10f);
    /* Camera/eye line is above this obstruction: material muzzle and camera are not synonyms. */
    NfWorldRayHit eye_hit={0};
    const NfVec3 eye={actor->transform.position.x,actor->transform.position.y+actor->movement.eye_height,actor->transform.position.z};
    assert(!nf_weapon_world_first_hit(&world,eye,(NfVec3){0,0,1},10.0f,&eye_hit));
}

static void authority_reconcile(void) {
    NfWorld world;nf_world_init(&world,5u);nf_world_add_collider(&world,NF_COLLIDER_SOLID,(NfVec3){-10,-0.5f,-10},(NfVec3){10,0,10});
    const NfEntityId id=nf_world_spawn_actor_with_id(&world,6u,NF_FACTION_PLAYER,(NfVec3){0,0,0});
    NfActor *actor=nf_world_find_actor(&world,id);actor->movement.grounded=true;
    NfPrediction prediction;nf_prediction_init(&prediction);
    NfCombatInput combat={.focus_held=true};
    const NfInputCommand command=nf_prediction_make_command(&prediction,world.tick,(NfMoveInput){0},combat);
    nf_prediction_simulate_command(&world,id,&command);actor=nf_world_find_actor(&world,id);nf_prediction_record(&prediction,&command,actor);
    NfActorNetState state={0};nf_actor_to_net_state(actor,&state);
    NfWeaponAuthorityNetState authority={0};nf_weapon_authority_to_net_state(actor,&authority);authority.focus_amount=0.0f;
    assert(nf_prediction_reconcile_v16b(&prediction,&world,id,&state,&authority,command.sequence,world.tick,0.035f,0.08f));
    assert(prediction.authority_corrections==1u);
    assert(nf_world_find_actor(&world,id)->weapon_authority.focus_amount<0.001f);
}

int main(void) {
    protocol_focus_and_packet_budget();
    accepted_shot_ancestry();
    reload_focus_timing();
    mobility_stress_is_embodied();
    weapon_state_isolation();
    physical_muzzle_and_world_block();
    authority_reconcile();
    puts("nightfall v1.6B Weapon Authority / Mobility contracts: PASS");
    puts("AUTH protocol_v6=PASS accepted_shot_ancestry=PASS owner_snapshot=PASS packet_1186=PASS");
    puts("FOCUS reload_55pct=PASS release_fast=PASS weapon_isolation=PASS");
    puts("MOBILITY integrated_state=PASS redirect_named=PASS affordance_not_ballistics=PASS");
    puts("MATERIAL physical_muzzle=PASS current_world_block=PASS prediction_authority=PASS");
    return 0;
}
