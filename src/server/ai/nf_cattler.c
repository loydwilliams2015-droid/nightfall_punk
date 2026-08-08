#include "nf_cattler.h"

#include "nf_combat.h"
#include "nf_movement.h"

#include <float.h>
#include <math.h>
#include <string.h>

#define NF_CATTLER_PERCEPTION_INTERVAL 6u
#define NF_CATTLER_DECISION_INTERVAL 18u
#define NF_CATTLER_MEMORY_TICKS (NF_TICK_RATE * 8u)
#define NF_CATTLER_REPORT_TICKS (NF_TICK_RATE * 5u)
#define NF_CATTLER_EVENT_CHECK_TICKS (NF_TICK_RATE * 12u)
#define NF_CATTLER_SURGE_TICKS (NF_TICK_RATE * 8u)
#define NF_CATTLER_SURGE_COOLDOWN_TICKS (NF_TICK_RATE * 30u)
#define NF_CATTLER_VIEW_RANGE 58.0f
#define NF_CATTLER_TALL_HEIGHT 2.80f

static float clamp01(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

static float absf_local(float value) {
    return value < 0.0f ? -value : value;
}

static uint32_t hash_u32(uint32_t value) {
    value ^= value >> 16;
    value *= 0x7feb352du;
    value ^= value >> 15;
    value *= 0x846ca68bu;
    value ^= value >> 16;
    return value;
}

static float dist_xz(NfVec3 a, NfVec3 b) {
    const float dx = a.x - b.x;
    const float dz = a.z - b.z;
    return sqrtf(dx*dx + dz*dz);
}

static NfVec3 vsub(NfVec3 a, NfVec3 b) {
    return (NfVec3){a.x-b.x,a.y-b.y,a.z-b.z};
}

static float vdot(NfVec3 a, NfVec3 b) {
    return a.x*b.x+a.y*b.y+a.z*b.z;
}

static float vlen(NfVec3 v) {
    return sqrtf(vdot(v,v));
}

static bool ray_aabb(
    NfVec3 origin, NfVec3 direction, NfVec3 minimum, NfVec3 maximum,
    float *distance) {
    float tmin=0.0f;
    float tmax=FLT_MAX;
    const float ov[3]={origin.x,origin.y,origin.z};
    const float dv[3]={direction.x,direction.y,direction.z};
    const float mn[3]={minimum.x,minimum.y,minimum.z};
    const float mx[3]={maximum.x,maximum.y,maximum.z};
    for(int i=0;i<3;++i) {
        if(fabsf(dv[i])<1e-6f) {
            if(ov[i]<mn[i]||ov[i]>mx[i]) return false;
            continue;
        }
        float t1=(mn[i]-ov[i])/dv[i];
        float t2=(mx[i]-ov[i])/dv[i];
        if(t1>t2){float tmp=t1;t1=t2;t2=tmp;}
        if(t1>tmin)tmin=t1;
        if(t2<tmax)tmax=t2;
        if(tmin>tmax)return false;
    }
    if(tmax<0.0f)return false;
    if(distance!=NULL)*distance=tmin>=0.0f?tmin:tmax;
    return true;
}

static bool line_blocked(const NfWorld *world,NfVec3 from,NfVec3 to) {
    const NfVec3 delta=vsub(to,from);
    const float distance=vlen(delta);
    if(distance<0.001f)return false;
    const NfVec3 direction={delta.x/distance,delta.y/distance,delta.z/distance};
    for(size_t i=0;i<world->collider_count;++i) {
        const NfCollider *collider=&world->colliders[i];
        if(collider->kind==NF_COLLIDER_LADDER)continue;
        float hit=0.0f;
        if(ray_aabb(from,direction,collider->min,collider->max,&hit)&&
           hit>0.02f&&hit<distance-0.08f) return true;
    }
    return false;
}

static bool is_prey(const NfActor *actor) {
    return actor!=NULL&&actor->active&&actor->combat.alive&&
        actor->faction!=NF_FACTION_NONE&&actor->faction!=NF_FACTION_RANCHER;
}

static NfCattlerAgent *find_mutable(NfCattlerSystem *system,NfEntityId actor_id) {
    if(system==NULL)return NULL;
    for(size_t i=0;i<system->count;++i) {
        if(system->agents[i].actor_id==actor_id)return &system->agents[i];
    }
    return NULL;
}

const NfCattlerAgent *nf_cattler_find_const(
    const NfCattlerSystem *system,NfEntityId actor_id) {
    if(system==NULL)return NULL;
    for(size_t i=0;i<system->count;++i) {
        if(system->agents[i].actor_id==actor_id)return &system->agents[i];
    }
    return NULL;
}

const NfCattlerRegionState *nf_cattler_region_state_const(
    const NfCattlerSystem *system,uint8_t region) {
    if(system==NULL||region>=system->graph.count)return NULL;
    return &system->regions[region];
}

static bool actor_is_cattler(const NfCattlerSystem *system,NfEntityId actor_id) {
    return nf_cattler_find_const(system,actor_id)!=NULL;
}

static NfVec3 spawn_for_index(const NfRegionGraph *graph,size_t index) {
    static const uint8_t anchors[NF_CATTLER_MAX_AGENTS]={23u,3u,12u,20u,7u};
    uint8_t region=anchors[index%NF_CATTLER_MAX_AGENTS];
    if(graph==NULL||region>=graph->count)return (NfVec3){0.0f,0.05f,0.0f};
    NfVec3 p=graph->regions[region].center;
    p.y=0.05f;
    p.x += (index&1u)?-8.0f:8.0f;
    p.z += (index&1u)?5.0f:-5.0f;
    return p;
}

static void update_region_pressure(NfCattlerSystem *system,const NfWorld *world) {
    for(size_t r=0;r<system->graph.count;++r)system->regions[r].territorial_pressure=0.0f;
    for(size_t i=0;i<system->count;++i) {
        const NfActor *body=nf_world_find_actor_const(world,system->agents[i].actor_id);
        if(body==NULL||!body->combat.alive)continue;
        uint8_t region=nf_region_nearest(&system->graph,body->transform.position);
        if(region<system->graph.count)system->regions[region].territorial_pressure+=0.34f;
    }
    for(size_t r=0;r<system->graph.count;++r) {
        system->regions[r].territorial_pressure=clamp01(system->regions[r].territorial_pressure);
    }
}

static void set_region_static_values(NfCattlerSystem *system) {
    for(size_t i=0;i<system->graph.count;++i) {
        const NfRegion *region=&system->graph.regions[i];
        NfCattlerRegionState *state=&system->regions[i];
        state->resource_value=clamp01(
            0.18f+region->objective_value*0.44f+region->structural_exposure*0.38f);
    }
}

static float continuity_score(
    const NfCattlerSystem *system,const NfCattlerAgent *agent,uint8_t region) {
    if(region>=system->graph.count)return 0.0f;
    const NfRegion *candidate=&system->graph.regions[region];
    float neighboring=0.0f;
    for(uint8_t i=0;i<candidate->neighbor_count;++i) {
        uint8_t n=candidate->neighbors[i];
        if(n<system->graph.count&&system->regions[n].infestation>neighboring) {
            neighboring=system->regions[n].infestation;
        }
    }
    float continuity=system->regions[region].infestation*0.72f+neighboring*0.28f;
    if(region==agent->home_region)continuity+=0.10f;
    return clamp01(continuity);
}

float nf_cattler_habitat_score(
    const NfCattlerSystem *system,const NfCattlerAgent *agent,
    const NfWorld *world,uint8_t region) {
    if(system==NULL||agent==NULL||world==NULL||region>=system->graph.count)return 0.0f;
    const NfCattlerRegionState *state=&system->regions[region];
    const NfActor *body=nf_world_find_actor_const(world,agent->actor_id);
    const uint8_t current=body?nf_region_nearest(&system->graph,body->transform.position):agent->current_region;
    const int steps=current<NF_REGION_MAX?nf_region_path_steps(&system->graph,current,region):-1;
    if(steps<0)return 0.0f;

    const float continuity=continuity_score(system,agent,region);
    const float resource=state->resource_value;
    const float prey=state->prey_activity;
    const float disturbance=agent->profile.disposition==NF_RANCHER_PREDATORY
        ? clamp01(1.0f-absf_local(state->disturbance-0.55f)/0.65f)
        : 1.0f-state->disturbance;
    float territorial=1.0f-state->territorial_pressure;
    if(system->event_mode==NF_CATTLER_EVENT_HUNT_SURGE&&agent->social==NF_CATTLER_PACK) {
        territorial=1.0f-state->territorial_pressure*0.15f;
    }
    const float access=clamp01(1.0f-(float)steps/8.0f);

    float score=continuity*0.30f+resource*0.22f+prey*0.18f+
        disturbance*0.12f+territorial*0.10f+access*0.08f;
    if(region==current&&state->infestation>0.45f) {
        score+=0.08f+0.10f*state->infestation;
    }
    if(agent->locomotor_integrity<0.55f&&steps>1) {
        score-=0.16f*(1.0f-agent->locomotor_integrity)*(float)(steps-1);
    }
    return clamp01(score);
}

static void add_report(
    NfCattlerSystem *system,const NfCattlerAgent *agent,NfEntityId prey,
    NfVec3 position,uint8_t region,NfCattlerEvidenceType evidence,float confidence,
    uint64_t tick) {
    if(system==NULL||agent==NULL||agent->social!=NF_CATTLER_PACK||prey==0u)return;

    NfCattlerReport *previous=NULL;
    uint64_t previous_tick=0u;
    for(size_t i=0;i<NF_CATTLER_BLACKBOARD_REPORTS;++i) {
        NfCattlerReport *candidate=&system->reports[i];
        if(candidate->active&&candidate->prey==prey&&candidate->tick>=previous_tick) {
            previous=candidate;
            previous_tick=candidate->tick;
        }
    }
    if(previous!=NULL&&previous->region!=region&&previous->region<system->graph.count&&
       system->regions[previous->region].infestation>0.35f) {
        system->ecology_score.displacement+=0.25f;
    }

    NfCattlerReport *report=&system->reports[system->next_report%NF_CATTLER_BLACKBOARD_REPORTS];
    system->next_report=(system->next_report+1u)%NF_CATTLER_BLACKBOARD_REPORTS;
    *report=(NfCattlerReport){
        .active=true,.reporter=agent->actor_id,.prey=prey,.position_hint=position,
        .region=region,.evidence=evidence,.confidence=clamp01(confidence),.tick=tick
    };
}

static const NfCattlerReport *best_report_for(
    const NfCattlerSystem *system,const NfCattlerAgent *agent,uint64_t now) {
    if(agent->social!=NF_CATTLER_PACK)return NULL;
    const NfCattlerReport *best=NULL;
    float best_score=0.0f;
    for(size_t i=0;i<NF_CATTLER_BLACKBOARD_REPORTS;++i) {
        const NfCattlerReport *report=&system->reports[i];
        if(!report->active||report->reporter==agent->actor_id||now<report->tick||
           now-report->tick>NF_CATTLER_REPORT_TICKS)continue;
        const float age=1.0f-(float)(now-report->tick)/(float)NF_CATTLER_REPORT_TICKS;
        const float score=report->confidence*age;
        if(score>best_score){best_score=score;best=report;}
    }
    return best;
}

static void perceive(
    NfCattlerSystem *system,NfCattlerAgent *agent,NfWorld *world,
    const NfSemanticBus *semantics) {
    NfActor *self=nf_world_find_actor(world,agent->actor_id);
    if(self==NULL||!self->combat.alive)return;

    NfEntityId best=0u;
    NfVec3 best_position={0};
    float best_distance=FLT_MAX;
    const float facing_x=sinf(agent->yaw);
    const float facing_z=cosf(agent->yaw);
    const NfVec3 eye={self->transform.position.x,self->transform.position.y+2.35f,self->transform.position.z};

    for(size_t i=0;i<NF_MAX_ENTITIES;++i) {
        const NfActor *other=&world->actors[i];
        if(!is_prey(other))continue;
        const float dx=other->transform.position.x-self->transform.position.x;
        const float dz=other->transform.position.z-self->transform.position.z;
        const float range=sqrtf(dx*dx+dz*dz);
        if(range>NF_CATTLER_VIEW_RANGE||range<0.001f)continue;
        const float facing=(facing_x*dx+facing_z*dz)/range;
        if(facing<-0.35f)continue;
        const NfVec3 target_eye={
            other->transform.position.x,
            other->transform.position.y+other->movement.eye_height*0.75f,
            other->transform.position.z
        };
        if(line_blocked(world,eye,target_eye))continue;
        if(range<best_distance){best_distance=range;best=other->id;best_position=other->transform.position;}
    }

    agent->knowledge.visible_now=best!=0u;
    if(best!=0u) {
        const uint8_t region=nf_region_nearest(&system->graph,best_position);
        agent->knowledge=(NfCattlerKnowledge){
            .prey=best,.last_position=best_position,.region=region,
            .evidence=NF_CATTLER_EVIDENCE_VISUAL,.confidence=1.0f,
            .evidence_tick=world->tick,.visible_now=true
        };
        if(region<system->graph.count) {
            system->regions[region].prey_activity=clamp01(system->regions[region].prey_activity+0.22f);
            system->regions[region].last_prey_tick=world->tick;
        }
        add_report(system,agent,best,best_position,region,NF_CATTLER_EVIDENCE_VISUAL,0.86f,world->tick);
        return;
    }

    NfSemanticAlert heard[8];
    const size_t heard_count=nf_semantic_collect_audible(
        semantics,self->transform.position,world->tick,heard,8u);
    for(size_t i=0;i<heard_count;++i) {
        const NfSemanticAlert *event=&heard[i];
        const NfActor *source=nf_world_find_actor_const(world,event->source);
        if(!is_prey(source))continue;
        if(event->type==NF_SEMANTIC_GUNFIRE||event->type==NF_SEMANTIC_DAMAGE_TAKEN||
           event->type==NF_SEMANTIC_ACTOR_DIED) {
            uint8_t region=nf_region_nearest(&system->graph,event->position);
            agent->knowledge.prey=event->source;
            agent->knowledge.last_position=event->position;
            agent->knowledge.region=region;
            agent->knowledge.evidence=event->type==NF_SEMANTIC_GUNFIRE
                ? NF_CATTLER_EVIDENCE_GUNFIRE:NF_CATTLER_EVIDENCE_DAMAGE;
            agent->knowledge.confidence=agent->knowledge.confidence<0.66f?0.66f:agent->knowledge.confidence;
            agent->knowledge.evidence_tick=world->tick;
            add_report(system,agent,event->source,event->position,region,agent->knowledge.evidence,0.62f,world->tick);
            break;
        }
    }

    const NfCattlerReport *report=best_report_for(system,agent,world->tick);
    if(report!=NULL&&agent->knowledge.confidence<report->confidence*0.72f) {
        agent->knowledge.prey=report->prey;
        agent->knowledge.last_position=report->position_hint;
        agent->knowledge.region=report->region;
        agent->knowledge.evidence=NF_CATTLER_EVIDENCE_PACK_REPORT;
        agent->knowledge.confidence=report->confidence*0.72f;
        agent->knowledge.evidence_tick=report->tick;
    }

    if(agent->knowledge.evidence_tick==0u||world->tick<agent->knowledge.evidence_tick||
       world->tick-agent->knowledge.evidence_tick>NF_CATTLER_MEMORY_TICKS) {
        agent->knowledge=(NfCattlerKnowledge){0};
    } else {
        agent->knowledge.confidence=clamp01(agent->knowledge.confidence-0.032f);
    }
}

static uint8_t choose_region(
    const NfCattlerSystem *system,const NfCattlerAgent *agent,const NfWorld *world) {
    const NfActor *body=nf_world_find_actor_const(world,agent->actor_id);
    if(body==NULL)return agent->current_region;
    const uint8_t current=nf_region_nearest(&system->graph,body->transform.position);
    if(current>=system->graph.count)return agent->home_region;

    float best_score=nf_cattler_habitat_score(system,agent,world,current);
    uint8_t best=current;
    const bool surge=system->event_mode==NF_CATTLER_EVENT_HUNT_SURGE&&
        agent->social==NF_CATTLER_PACK&&agent->profile.disposition==NF_RANCHER_PREDATORY;

    if(surge&&agent->knowledge.region<system->graph.count&&agent->knowledge.confidence>0.15f) {
        return agent->knowledge.region;
    }

    for(uint8_t region=0u;region<(uint8_t)system->graph.count;++region) {
        const int steps=nf_region_path_steps(&system->graph,current,region);
        if(steps<0||steps>5)continue;
        float score=nf_cattler_habitat_score(system,agent,world,region);
        const uint32_t tie=hash_u32(system->seed^agent->actor_id^((uint32_t)region<<8)^((uint32_t)(world->tick/NF_TICK_RATE)));
        score+=(float)(tie%17u)*0.001f;
        if(agent->knowledge.region==region&&agent->knowledge.confidence>0.25f) {
            const float drive=agent->profile.disposition==NF_RANCHER_PREDATORY
                ? agent->profile.predation_drive:0.15f;
            score+=agent->knowledge.confidence*drive*0.16f;
        }
        if(score>best_score){best_score=score;best=region;}
    }

    const float current_infestation=system->regions[current].infestation;
    if(best!=current&&current_infestation>0.55f) {
        const float current_score=nf_cattler_habitat_score(system,agent,world,current);
        const float required=current_score+0.08f+current_infestation*0.10f;
        if(best_score<required)best=current;
    }
    return best;
}

static void decide(NfCattlerSystem *system,NfCattlerAgent *agent,NfWorld *world) {
    NfActor *body=nf_world_find_actor(world,agent->actor_id);
    if(body==NULL)return;
    if(!body->combat.alive) {
        agent->mode=NF_CATTLER_RECOVER;
        return;
    }

    agent->current_region=nf_region_nearest(&system->graph,body->transform.position);
    const bool low_locomotion=agent->locomotor_integrity<0.42f;
    const bool predatory=agent->profile.disposition==NF_RANCHER_PREDATORY;
    const float prey_distance=agent->knowledge.prey!=0u
        ? dist_xz(body->transform.position,agent->knowledge.last_position):FLT_MAX;

    if(low_locomotion&&agent->knowledge.visible_now&&prey_distance<10.0f) {
        agent->mode=NF_CATTLER_WITHDRAW;
        agent->target_region=agent->current_region;
        return;
    }

    if(!predatory&&agent->knowledge.confidence>0.25f) {
        agent->mode=NF_CATTLER_AVOID;
        agent->target_region=choose_region(system,agent,world);
        return;
    }

    const bool surge=system->event_mode==NF_CATTLER_EVENT_HUNT_SURGE&&agent->social==NF_CATTLER_PACK;
    if(agent->knowledge.visible_now&&predatory) {
        if(prey_distance>=22.0f) {
            agent->mode=NF_CATTLER_SNIPE;
            agent->target_region=agent->current_region;
            return;
        }
        if(prey_distance>=7.0f&&world->tick>=agent->swoop_ready_tick&&
           agent->locomotor_integrity>0.52f) {
            agent->mode=NF_CATTLER_SWOOP;
            agent->target_region=agent->knowledge.region<system->graph.count
                ? agent->knowledge.region:agent->current_region;
            agent->swoop_ready_tick=world->tick+(surge?NF_TICK_RATE*2u:NF_TICK_RATE*5u);
            return;
        }
        agent->mode=NF_CATTLER_SNIPE;
        agent->target_region=agent->current_region;
        return;
    }

    if(agent->knowledge.confidence>0.32f&&predatory) {
        agent->target_region=agent->knowledge.region<system->graph.count
            ? agent->knowledge.region:choose_region(system,agent,world);
        agent->mode=surge?NF_CATTLER_STALK:
            (agent->knowledge.evidence==NF_CATTLER_EVIDENCE_PACK_REPORT?NF_CATTLER_LURK:NF_CATTLER_STALK);
        return;
    }

    agent->target_region=choose_region(system,agent,world);
    if(agent->target_region!=agent->current_region) {
        agent->mode=system->regions[agent->current_region].infestation>0.50f
            ? NF_CATTLER_EXPAND:NF_CATTLER_STALK;
    } else {
        agent->mode=system->regions[agent->current_region].infestation>0.24f
            ? NF_CATTLER_INFEST:NF_CATTLER_LURK;
    }
}

static NfVec3 region_goal(
    const NfCattlerSystem *system,const NfCattlerAgent *agent,const NfWorld *world) {
    const NfActor *body=nf_world_find_actor_const(world,agent->actor_id);
    if(body==NULL)return (NfVec3){0};
    uint8_t current=nf_region_nearest(&system->graph,body->transform.position);
    uint8_t target=agent->target_region<system->graph.count?agent->target_region:current;
    uint8_t next=nf_region_next_hop(&system->graph,current,target);
    if(next==NF_REGION_INVALID)next=target;
    if(next>=system->graph.count)return body->transform.position;
    NfVec3 goal=system->graph.regions[next].center;
    const uint32_t variation=hash_u32(system->seed^agent->actor_id^((uint32_t)next<<12));
    goal.x+=(float)((int)(variation%13u)-6)*1.3f;
    goal.z+=(float)((int)((variation>>8)%13u)-6)*1.3f;
    return goal;
}

static NfControlFrame control_for(
    NfCattlerSystem *system,NfCattlerAgent *agent,NfWorld *world) {
    NfControlFrame frame={.actor=agent->actor_id,.source=NF_CONTROL_AI};
    NfActor *body=nf_world_find_actor(world,agent->actor_id);
    if(body==NULL||!body->combat.alive)return frame;
    frame.sequence=++agent->control_sequence;
    frame.perceived_tick=world->tick;

    NfVec3 goal=region_goal(system,agent,world);
    bool move_to_goal=false;

    if(agent->mode==NF_CATTLER_STALK||agent->mode==NF_CATTLER_EXPAND||
       agent->mode==NF_CATTLER_WITHDRAW||agent->mode==NF_CATTLER_AVOID) {
        move_to_goal=true;
    }

    if(agent->mode==NF_CATTLER_STALK&&agent->knowledge.confidence>0.25f) {
        goal=agent->knowledge.last_position;
        if(agent->knowledge.evidence==NF_CATTLER_EVIDENCE_PACK_REPORT) {
            const NfRegion *r=agent->knowledge.region<system->graph.count
                ? &system->graph.regions[agent->knowledge.region]:NULL;
            if(r!=NULL) {
                const uint32_t side=hash_u32(system->seed^agent->actor_id^(uint32_t)world->tick/NF_TICK_RATE);
                goal=r->center;
                goal.x+=(side&1u)?18.0f:-18.0f;
                goal.z+=(side&2u)?14.0f:-14.0f;
            }
        }
        move_to_goal=true;
    }

    if(agent->mode==NF_CATTLER_SWOOP&&agent->knowledge.prey!=0u) {
        goal=agent->knowledge.last_position;
        move_to_goal=true;
    }

    if(agent->mode==NF_CATTLER_WITHDRAW&&agent->knowledge.prey!=0u) {
        const NfVec3 away={
            body->transform.position.x-(agent->knowledge.last_position.x-body->transform.position.x),
            body->transform.position.y,
            body->transform.position.z-(agent->knowledge.last_position.z-body->transform.position.z)
        };
        goal=away;
        move_to_goal=true;
    }

    if(agent->mode==NF_CATTLER_AVOID&&agent->knowledge.prey!=0u) {
        goal=(NfVec3){
            body->transform.position.x-(agent->knowledge.last_position.x-body->transform.position.x),
            body->transform.position.y,
            body->transform.position.z-(agent->knowledge.last_position.z-body->transform.position.z)
        };
        move_to_goal=true;
    }

    if(move_to_goal) {
        const float dx=goal.x-body->transform.position.x;
        const float dz=goal.z-body->transform.position.z;
        const float distance=sqrtf(dx*dx+dz*dz);
        if(distance>0.6f) {
            agent->yaw=atan2f(dx,dz);
            frame.move.forward=1.0f;
            frame.move.sprint_held=(agent->mode==NF_CATTLER_SWOOP)||
                (distance>8.0f&&agent->locomotor_integrity>0.58f);
            if(agent->mode==NF_CATTLER_SWOOP&&body->movement.grounded&&distance>4.0f) {
                frame.move.jump_pressed=true;
            }
        }
    }

    if((agent->mode==NF_CATTLER_LURK||agent->mode==NF_CATTLER_INFEST)&&
       agent->knowledge.confidence<0.20f) {
        const uint32_t watch=hash_u32(system->seed^agent->actor_id^((uint32_t)(world->tick/(NF_TICK_RATE*2u))));
        agent->yaw+=(watch&1u)?0.035f:-0.035f;
    }

    if((agent->mode==NF_CATTLER_SNIPE||agent->mode==NF_CATTLER_SWOOP)&&
       agent->knowledge.visible_now&&agent->knowledge.prey!=0u) {
        const float dx=agent->knowledge.last_position.x-body->transform.position.x;
        const float dz=agent->knowledge.last_position.z-body->transform.position.z;
        const float horizontal=sqrtf(dx*dx+dz*dz);
        const float dy=agent->knowledge.last_position.y+0.9f-(body->transform.position.y+2.35f);
        agent->yaw=atan2f(dx,dz);
        agent->pitch=atan2f(dy,horizontal>0.01f?horizontal:0.01f);
        frame.combat.fire_held=true;
        frame.combat.fire_pressed=true;
    }

    frame.move.yaw_radians=agent->yaw;
    frame.combat.aim_pitch_radians=agent->pitch;
    return frame;
}

static void update_event_mode(NfCattlerSystem *system,const NfWorld *world) {
    if(system->debug_profile==NF_CATTLER_PROFILE_HORDE) {
        system->event_mode=NF_CATTLER_EVENT_HUNT_SURGE;
        system->event_until_tick=world->tick+NF_TICK_RATE;
        return;
    }
    if(system->event_mode==NF_CATTLER_EVENT_HUNT_SURGE&&world->tick>=system->event_until_tick) {
        system->event_mode=NF_CATTLER_EVENT_NORMAL;
        system->event_cooldown_until_tick=world->tick+NF_CATTLER_SURGE_COOLDOWN_TICKS;
    }
    if(world->tick<system->next_event_check_tick)return;
    system->next_event_check_tick=world->tick+NF_CATTLER_EVENT_CHECK_TICKS;
    if(system->count<3u||world->tick<system->event_cooldown_until_tick)return;
    const uint32_t epoch=(uint32_t)(world->tick/NF_CATTLER_EVENT_CHECK_TICKS);
    const uint32_t roll=hash_u32(system->seed^epoch^0xD09Fu)%20u;
    if(roll==0u) {
        bool ecology_ready=false;
        for(size_t r=0;r<system->graph.count;++r) {
            if(system->regions[r].infestation>0.35f||system->regions[r].prey_activity>0.55f) {
                ecology_ready=true;
                break;
            }
        }
        if(ecology_ready) {
            system->event_mode=NF_CATTLER_EVENT_HUNT_SURGE;
            system->event_until_tick=world->tick+NF_CATTLER_SURGE_TICKS;
        }
    }
}

static void update_region_ecology(NfCattlerSystem *system,const NfWorld *world) {
    bool occupied[NF_REGION_MAX]={0};
    NfEntityId occupant[NF_REGION_MAX]={0};
    for(size_t i=0;i<system->count;++i) {
        const NfActor *body=nf_world_find_actor_const(world,system->agents[i].actor_id);
        if(body==NULL||!body->combat.alive)continue;
        uint8_t region=nf_region_nearest(&system->graph,body->transform.position);
        if(region<system->graph.count){occupied[region]=true;occupant[region]=body->id;}
    }

    for(size_t r=0;r<system->graph.count;++r) {
        NfCattlerRegionState *state=&system->regions[r];
        state->prey_activity=clamp01(state->prey_activity-0.0010f);
        state->disturbance=clamp01(state->disturbance-0.0008f);
        const float before=state->infestation;
        if(occupied[r]) {
            state->infestation=clamp01(state->infestation+0.00072f);
            state->last_occupied_tick=world->tick;
        } else {
            state->infestation=clamp01(state->infestation-0.00010f);
        }
        if(state->infestation>before) {
            const float gain=(state->infestation-before)*10.0f;
            system->ecology_score.infestation+=gain;
            NfCattlerAgent *agent=find_mutable(system,occupant[r]);
            if(agent!=NULL)agent->score.infestation+=gain;
        }
        if(before<0.25f&&state->infestation>=0.25f&&occupied[r]) {
            ++state->expansion_count;
            system->ecology_score.expansion+=1.0f;
            NfCattlerAgent *agent=find_mutable(system,occupant[r]);
            if(agent!=NULL)agent->score.expansion+=1.0f;
        }
    }

    if(world->tick%NF_TICK_RATE==0u) {
        for(size_t i=0;i<system->count;++i) {
            const NfActor *body=nf_world_find_actor_const(world,system->agents[i].actor_id);
            if(body!=NULL&&body->combat.alive) {
                system->agents[i].score.survival+=0.05f;
                system->ecology_score.survival+=0.05f;
            }
        }
    }
}

void nf_cattler_init(
    NfCattlerSystem *system,NfWorld *world,size_t count,
    NfCattlerDebugProfile profile,uint32_t seed) {
    if(system==NULL||world==NULL)return;
    memset(system,0,sizeof(*system));
    nf_region_graph_init_spatial_lab(&system->graph);
    system->seed=seed;
    system->debug_profile=profile;
    system->event_mode=profile==NF_CATTLER_PROFILE_HORDE
        ? NF_CATTLER_EVENT_HUNT_SURGE:NF_CATTLER_EVENT_NORMAL;
    if(count>NF_CATTLER_MAX_AGENTS)count=NF_CATTLER_MAX_AGENTS;
    system->count=count;
    set_region_static_values(system);

    for(size_t i=0;i<count;++i) {
        NfCattlerAgent *agent=&system->agents[i];
        const NfVec3 spawn=spawn_for_index(&system->graph,i);
        NfEntityId actor_id=nf_world_spawn_actor(world,NF_FACTION_RANCHER,spawn);
        const uint32_t personality=hash_u32(seed^(uint32_t)i*0x9E3779B9u);
        NfCattlerSocialMode social=(personality%20u==0u)?NF_CATTLER_LONER:NF_CATTLER_PACK;
        if(profile==NF_CATTLER_PROFILE_LONER)social=NF_CATTLER_LONER;
        else if(profile==NF_CATTLER_PROFILE_PACK||profile==NF_CATTLER_PROFILE_HORDE)social=NF_CATTLER_PACK;
        NfRancherProfile rancher=nf_rancher_profile_from_roll(personality%1000u,10u,1000u);
        uint8_t home=nf_region_nearest(&system->graph,spawn);
        *agent=(NfCattlerAgent){
            .actor_id=actor_id,.mode=NF_CATTLER_LURK,.social=social,.profile=rancher,
            .spawn=spawn,.locomotor_integrity=1.0f,.commitment=1.0f,
            .current_region=home,.target_region=home,.next_region=home,
            .home_region=home,.claimed_watch_region=NF_REGION_INVALID,
            .next_perception_tick=(uint64_t)(i%NF_CATTLER_PERCEPTION_INTERVAL),
            .next_decision_tick=(uint64_t)(i%NF_CATTLER_DECISION_INTERVAL),
            .control_sequence=1000u+(uint32_t)i*100u
        };
        NfActor *body=nf_world_find_actor(world,actor_id);
        if(body!=NULL) {
            body->movement.body_height=NF_CATTLER_TALL_HEIGHT;
            body->movement.eye_height=2.35f;
        }
    }
    update_region_pressure(system,world);
}

size_t nf_cattler_tick(
    NfCattlerSystem *system,NfWorld *world,const NfSemanticBus *semantics,
    NfControlFrame *out,size_t cap) {
    if(system==NULL||world==NULL||semantics==NULL||out==NULL)return 0u;
    update_event_mode(system,world);
    update_region_pressure(system,world);
    update_region_ecology(system,world);

    size_t written=0u;
    for(size_t i=0;i<system->count&&written<cap;++i) {
        NfCattlerAgent *agent=&system->agents[i];
        NfActor *body=nf_world_find_actor(world,agent->actor_id);
        if(body==NULL)continue;
        agent->current_region=nf_region_nearest(&system->graph,body->transform.position);
        if(agent->current_region<system->graph.count) {
            system->regions[agent->current_region].last_occupied_tick=world->tick;
        }
        if(world->tick>=agent->next_perception_tick) {
            perceive(system,agent,world,semantics);
            agent->next_perception_tick=world->tick+NF_CATTLER_PERCEPTION_INTERVAL;
        }
        if(world->tick>=agent->next_decision_tick) {
            const uint8_t previous=agent->current_region;
            decide(system,agent,world);
            agent->next_decision_tick=world->tick+NF_CATTLER_DECISION_INTERVAL;
            agent->next_region=nf_region_next_hop(&system->graph,agent->current_region,agent->target_region);
            if(agent->next_region==NF_REGION_INVALID)agent->next_region=agent->target_region;
            if(previous!=agent->current_region)agent->last_region_change_tick=world->tick;
        }
        out[written++]=control_for(system,agent,world);
    }
    return written;
}

void nf_cattler_on_combat_event(
    NfCattlerSystem *system,const NfWorld *world,const NfCombatEvent *event) {
    if(system==NULL||world==NULL||event==NULL)return;
    uint8_t region=nf_region_nearest(&system->graph,event->position);
    if(region<system->graph.count) {
        float prey_gain=0.0f;
        float disturbance_gain=0.0f;
        switch(event->type) {
            case NF_COMBAT_EVENT_GUNFIRE: prey_gain=0.10f; disturbance_gain=0.16f; break;
            case NF_COMBAT_EVENT_DAMAGE: prey_gain=0.18f; disturbance_gain=0.22f; break;
            case NF_COMBAT_EVENT_DEATH: prey_gain=0.28f; disturbance_gain=0.34f; break;
            default: break;
        }
        system->regions[region].prey_activity=clamp01(system->regions[region].prey_activity+prey_gain);
        system->regions[region].disturbance=clamp01(system->regions[region].disturbance+disturbance_gain);
        if(prey_gain>0.0f)system->regions[region].last_prey_tick=event->server_tick;
    }

    NfCattlerAgent *source=find_mutable(system,event->source);
    if(source!=NULL&&(event->type==NF_COMBAT_EVENT_DAMAGE||event->type==NF_COMBAT_EVENT_DEATH)&&
       !actor_is_cattler(system,event->target)) {
        float prey_value=event->type==NF_COMBAT_EVENT_DEATH?2.0f:clamp01(event->amount/100.0f)*0.75f;
        source->score.prey+=prey_value;
        system->ecology_score.prey+=prey_value;
    }

    NfCattlerAgent *target=find_mutable(system,event->target);
    if(target!=NULL&&(event->type==NF_COMBAT_EVENT_DAMAGE||event->type==NF_COMBAT_EVENT_DEATH)) {
        float locomotor_loss=0.0f;
        if(event->hit_zone==NF_HIT_KNEE)locomotor_loss=0.34f;
        else if(event->hit_zone==NF_HIT_FOOT)locomotor_loss=0.26f;
        else if(event->hit_zone==NF_HIT_BODY)locomotor_loss=0.035f;
        target->locomotor_integrity=clamp01(target->locomotor_integrity-locomotor_loss);
        if(event->type==NF_COMBAT_EVENT_DEATH)target->mode=NF_CATTLER_RECOVER;
    }
}

void nf_cattler_on_respawn(
    NfCattlerSystem *system,NfEntityId actor_id,const NfWorld *world) {
    if(system==NULL||world==NULL)return;
    NfCattlerAgent *agent=find_mutable(system,actor_id);
    if(agent==NULL)return;
    agent->knowledge=(NfCattlerKnowledge){0};
    agent->mode=NF_CATTLER_LURK;
    agent->locomotor_integrity=1.0f;
    agent->target_region=agent->home_region;
    agent->next_region=agent->home_region;
    agent->swoop_ready_tick=world->tick+NF_TICK_RATE*2u;
    agent->next_perception_tick=world->tick+1u;
    agent->next_decision_tick=world->tick+1u;
}

bool nf_cattler_spawn_for(
    const NfCattlerSystem *system,NfEntityId actor_id,NfVec3 *out) {
    const NfCattlerAgent *agent=nf_cattler_find_const(system,actor_id);
    if(agent==NULL||out==NULL)return false;
    *out=agent->spawn;
    return true;
}

const char *nf_cattler_mode_name(NfCattlerMode mode) {
    switch(mode) {
        case NF_CATTLER_LURK:return "LURK";
        case NF_CATTLER_STALK:return "STALK";
        case NF_CATTLER_SNIPE:return "SNIPE";
        case NF_CATTLER_SWOOP:return "SWOOP";
        case NF_CATTLER_INFEST:return "INFEST";
        case NF_CATTLER_EXPAND:return "EXPAND";
        case NF_CATTLER_RECOVER:return "RECOVER";
        case NF_CATTLER_WITHDRAW:return "WITHDRAW";
        case NF_CATTLER_AVOID:return "AVOID";
        default:return "UNKNOWN";
    }
}

const char *nf_cattler_social_name(NfCattlerSocialMode mode) {
    return mode==NF_CATTLER_LONER?"LONER":"PACK";
}

const char *nf_cattler_event_name(NfCattlerEventMode mode) {
    return mode==NF_CATTLER_EVENT_HUNT_SURGE?"HUNT_SURGE":"NORMAL";
}
