#include "nf_lifeworld_spatial.h"

static bool tactical_override(const NfAiAgent *agent,const NfActor *body) {
    if(agent==NULL||body==NULL)return true;
    if(!body->combat.alive||body->health<32.0f)return true;
    if(agent->knowledge.visible_now)return true;
    return agent->mode==NF_AGENT_ENGAGE||
        agent->mode==NF_AGENT_SEEK_COVER||
        agent->mode==NF_AGENT_RETREAT||
        agent->mode==NF_AGENT_RELOAD;
}

static NfSpatialTask task_for(
    const NfAttentionChoice *choice,const NfAiAgent *agent) {
    if(choice==NULL)return NF_SPATIAL_TASK_ROAM;
    if(choice->hotspot==NF_HOTSPOT_CONTESTED_OCCUPATION) {
        if(agent!=NULL&&(agent->role==NF_SQUAD_FLANK_LEFT||agent->role==NF_SQUAD_FLANK_RIGHT)) {
            return NF_SPATIAL_TASK_FLANK;
        }
        return NF_SPATIAL_TASK_CONTEST;
    }
    if(choice->hotspot==NF_HOTSPOT_RECOVERY_EDGE||choice->activity==NF_LIFE_MAINTAIN) {
        return NF_SPATIAL_TASK_OCCUPY;
    }
    return NF_SPATIAL_TASK_INVESTIGATE;
}

void nf_lifeworld_apply_attention_bias(
    NfSpatialSystem *spatial,
    const NfLifeworldSystem *lifeworld,
    const NfAiSystem *ai,
    const NfWorld *world) {
    if(spatial==NULL||lifeworld==NULL||ai==NULL||world==NULL)return;

    for(size_t i=0u;i<lifeworld->attention_count;++i) {
        const NfAttentionChoice *choice=&lifeworld->attention[i];
        const NfAiAgent *agent=nf_ai_find_agent_const(ai,choice->actor_id);
        const NfActor *body=nf_world_find_actor_const(world,choice->actor_id);
        if(tactical_override(agent,body)) {
            nf_spatial_clear_attention_hint(spatial,choice->actor_id);
            continue;
        }

        nf_spatial_set_attention_hint(
            spatial,choice->actor_id,choice->region,
            task_for(choice,agent),world->tick+NF_TICK_RATE*2u);
    }
}
