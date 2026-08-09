#include "nf_spatial.h"

static NfSpatialAgentState *state_for(
    NfSpatialSystem *spatial,NfEntityId actor_id) {
    if(spatial==NULL)return NULL;
    for(size_t i=0u;i<spatial->count;++i) {
        if(spatial->agents[i].actor_id==actor_id)return &spatial->agents[i];
    }
    return NULL;
}

void nf_spatial_set_attention_hint(
    NfSpatialSystem *spatial,
    NfEntityId actor_id,
    uint8_t region,
    NfSpatialTask task,
    uint64_t until_tick) {
    if(spatial==NULL||region>=spatial->graph.count)return;
    NfSpatialAgentState *state=state_for(spatial,actor_id);
    if(state==NULL)return;

    state->attention_hint_active=true;
    state->attention_region=region;
    state->attention_task=task;
    state->attention_until_tick=until_tick;

    /* This is a bias into the existing spatial machinery, not a movement
       command. Invalidate the local sample so nf_spatial_tick() and Fuzzy Rail
       still own the actual traversable goal and embodiment on the next tick. */
    state->target_region=region;
    state->next_region=state->current_region==region
        ? region
        : nf_region_next_hop(&spatial->graph,state->current_region,region);
    if(state->next_region==NF_REGION_INVALID)state->next_region=region;
    state->task=state->current_region==region?task:NF_SPATIAL_TASK_ROAM;
    state->commit_until_tick=until_tick;
    state->claim_until_tick=until_tick;
    state->next_region_tick=until_tick;
    state->next_local_tick=0u;
    state->local_goal_valid=false;
    state->interrupt=NF_SPATIAL_INTERRUPT_REEVALUATE;
}

void nf_spatial_clear_attention_hint(
    NfSpatialSystem *spatial,NfEntityId actor_id) {
    NfSpatialAgentState *state=state_for(spatial,actor_id);
    if(state==NULL)return;
    state->attention_hint_active=false;
    state->attention_region=NF_REGION_INVALID;
    state->attention_until_tick=0u;
}
