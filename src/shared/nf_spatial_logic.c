#include "nf_spatial_logic.h"

#include <math.h>
#include <string.h>

static uint32_t mix32(uint32_t x) {
    x ^= x >> 16; x *= 0x7feb352du; x ^= x >> 15; x *= 0x846ca68bu; x ^= x >> 16;
    return x;
}
static uint32_t rng_next(uint32_t *s) { *s = mix32(*s + 0x9e3779b9u); return *s; }
static float rng01(uint32_t *s) { return (float)(rng_next(s) & 0x00ffffffu) / 16777215.0f; }
static float clamp01(float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }
static int cx(const NfSpatialWorld *w, int c) { return c % w->width; }
static int cy(const NfSpatialWorld *w, int c) { return c / w->width; }
static int manhattan(const NfSpatialWorld *w, int a, int b) {
    int dx = cx(w, a) - cx(w, b), dy = cy(w, a) - cy(w, b);
    return (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
}
static int neighbor(const NfSpatialWorld *w, int c, int k) {
    static const int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
    return nf_spatial_cell_xy(w, cx(w, c) + dx[k], cy(w, c) + dy[k]);
}
static void set_solid(NfSpatialWorld *w, int x, int y, bool v) {
    int c = nf_spatial_cell_xy(w, x, y); if (c >= 0) w->cells[c].solid = v ? 1u : 0u;
}

bool nf_spatial_is_valid_cell(const NfSpatialWorld *w, int c) {
    return w != NULL && c >= 0 && c < w->width * w->height && w->cells[c].solid == 0u;
}
int nf_spatial_cell_xy(const NfSpatialWorld *w, int x, int y) {
    if (w == NULL || x < 0 || y < 0 || x >= w->width || y >= w->height) return -1;
    return y * w->width + x;
}
int nf_spatial_neighbor_count(const NfSpatialWorld *w, int c) {
    if (!nf_spatial_is_valid_cell(w, c)) return 0;
    int n = 0; for (int k = 0; k < 4; ++k) if (nf_spatial_is_valid_cell(w, neighbor(w, c, k))) ++n;
    return n;
}
int nf_spatial_shortest_path(const NfSpatialWorld *w, int start, int goal) {
    int q[NF_SPATIAL_MAX_CELLS], d[NF_SPATIAL_MAX_CELLS], head = 0, tail = 0;
    if (!nf_spatial_is_valid_cell(w, start) || !nf_spatial_is_valid_cell(w, goal)) return -1;
    for (int i = 0; i < NF_SPATIAL_MAX_CELLS; ++i) d[i] = -1;
    d[start] = 0; q[tail++] = start;
    while (head < tail) {
        int c = q[head++]; if (c == goal) return d[c];
        for (int k = 0; k < 4; ++k) {
            int n = neighbor(w, c, k);
            if (nf_spatial_is_valid_cell(w, n) && d[n] < 0) { d[n] = d[c] + 1; q[tail++] = n; }
        }
    }
    return -1;
}
bool nf_spatial_line_of_sight(const NfSpatialWorld *w, int a, int b) {
    if (!nf_spatial_is_valid_cell(w, a) || !nf_spatial_is_valid_cell(w, b)) return false;
    int x0 = cx(w, a), y0 = cy(w, a), x1 = cx(w, b), y1 = cy(w, b);
    int dx = x1 - x0, dy = y1 - y0, sx = dx >= 0 ? 1 : -1, sy = dy >= 0 ? 1 : -1;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    int err = dx - dy;
    for (;;) {
        int c = nf_spatial_cell_xy(w, x0, y0);
        if (c < 0) return false;
        if (c != a && c != b && w->cells[c].solid != 0u) return false;
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
    return true;
}
size_t nf_spatial_observe_local(const NfSpatialWorld *w, int origin, int radius, uint8_t *known, size_t count) {
    if (w == NULL || known == NULL || count < (size_t)(w->width * w->height) || !nf_spatial_is_valid_cell(w, origin)) return 0u;
    size_t added = 0u;
    for (int c = 0; c < w->width * w->height; ++c) {
        if (manhattan(w, origin, c) <= radius && nf_spatial_line_of_sight(w, origin, c) && known[c] == 0u) {
            known[c] = 1u; ++added;
        }
    }
    return added;
}
float nf_spatial_affordance_candidate(const NfSpatialWorld *w, int from, int to) {
    if (!nf_spatial_is_valid_cell(w, from) || !nf_spatial_is_valid_cell(w, to)) return 0.0f;
    if (manhattan(w, from, to) != 1 || w->cells[to].authored_forbid_affordance != 0u) return 0.0f;
    return clamp01(w->cells[to].affordance);
}
void nf_spatial_mark_dirty(NfSpatialWorld *w, int center, int radius) {
    if (w == NULL || center < 0) return;
    for (int c = 0; c < w->width * w->height; ++c) if (manhattan(w, center, c) <= radius) w->cells[c].dirty = 1u;
}
uint16_t nf_spatial_recompute_dirty(NfSpatialWorld *w, uint16_t budget) {
    uint16_t done = 0u; if (w == NULL || budget == 0u) return 0u;
    for (int c = 0; c < w->width * w->height && done < budget; ++c) {
        if (w->cells[c].dirty != 0u) { w->cells[c].dirty = 0u; ++done; }
    }
    if (done > 0u) ++w->topology_revision;
    return done;
}

static void base_world(NfSpatialWorld *w, NfSpatialLabKind lab, uint32_t seed) {
    memset(w, 0, sizeof(*w)); w->width = 12; w->height = 12; w->seed = seed; w->lab = lab;
    w->start_cell = nf_spatial_cell_xy(w, 1, 10); w->goal_cell = nf_spatial_cell_xy(w, 10, 1);
    for (int c = 0; c < w->width * w->height; ++c) w->cells[c].support_kind = (uint8_t)NF_SUPPORT_GROUND;
    for (int i = 0; i < NF_SPATIAL_MAX_OBJECTS; ++i) { w->object_authority[i] = NF_SPATIAL_UNASSIGNED; w->object_cell[i] = -1; }
    for (int y = 0; y < w->height; ++y) for (int x = 0; x < w->width; ++x)
        if (x == 0 || y == 0 || x == w->width - 1 || y == w->height - 1) set_solid(w, x, y, true);
}
static void carve_open(NfSpatialWorld *w) {
    for (int y = 1; y < w->height - 1; ++y) for (int x = 1; x < w->width - 1; ++x) set_solid(w, x, y, false);
}
static void carve_loop(NfSpatialWorld *w) {
    for (int y = 1; y < w->height - 1; ++y) for (int x = 1; x < w->width - 1; ++x)
        set_solid(w, x, y, !(x == 2 || x == 9 || y == 2 || y == 9 || y == 5 || x == 6));
}
static void carve_lab(NfSpatialWorld *w, uint32_t *rng) {
    if (w->lab == NF_SPATIAL_LAB_CORRIDOR) {
        carve_loop(w); int gx = 4 + (int)(rng_next(rng) % 4u);
        for (int y = 3; y <= 8; ++y) set_solid(w, gx, y, true);
        set_solid(w, gx, 5 + (int)(rng_next(rng) % 2u), false);
    } else if (w->lab == NF_SPATIAL_LAB_LOOP || w->lab == NF_SPATIAL_LAB_CONTRACT) {
        carve_loop(w);
    } else if (w->lab == NF_SPATIAL_LAB_DYNAMIC) {
        carve_loop(w); for (int y = 3; y <= 8; ++y) set_solid(w, 6, y, true); set_solid(w, 6, 5, false);
    } else {
        carve_open(w);
    }
    if (w->lab == NF_SPATIAL_LAB_VERTICAL) {
        for (int y = 2; y <= 9; ++y) { int c = nf_spatial_cell_xy(w, 5, y); if (c >= 0) { w->cells[c].support_kind = NF_SUPPORT_LADDER; w->cells[c].affordance = 0.9f; } }
        for (int x = 6; x <= 9; ++x) { int c = nf_spatial_cell_xy(w, x, 4); if (c >= 0) { w->cells[c].support_kind = NF_SUPPORT_PLATFORM; w->cells[c].affordance = 0.6f; } }
    }
    if (w->lab == NF_SPATIAL_LAB_COVER) {
        const int p[5][2] = {{4,4},{7,4},{4,7},{7,7},{6,6}};
        for (int i = 0; i < 5; ++i) { int c = nf_spatial_cell_xy(w,p[i][0],p[i][1]); if (c >= 0) { w->cells[c].cover = 0.65f + 0.25f*rng01(rng); w->cells[c].exposure = 0.15f; } }
    }
    if (w->lab == NF_SPATIAL_LAB_SNAP) {
        for (int y = 1; y < w->height - 1; ++y) for (int x = 1; x < w->width - 1; ++x)
            if ((x + y) % 3 == 0) { int c = nf_spatial_cell_xy(w,x,y); if (c >= 0) w->cells[c].affordance = 0.45f + 0.5f*rng01(rng); }
        int forbidden = nf_spatial_cell_xy(w,6,6); if (forbidden >= 0) w->cells[forbidden].authored_forbid_affordance = 1u;
    }
    if (w->lab == NF_SPATIAL_LAB_ECOLOGY) {
        for (int y = 1; y < w->height - 1; ++y) for (int x = 1; x < w->width - 1; ++x) {
            int c=nf_spatial_cell_xy(w,x,y); float nx=(float)x/(float)(w->width-1), ny=(float)y/(float)(w->height-1);
            if (c >= 0) { w->cells[c].ecological_risk=clamp01(0.2f+0.55f*nx+0.2f*rng01(rng)); w->cells[c].resource=clamp01(0.75f*ny+0.2f*rng01(rng)); }
        }
    }
}

bool nf_spatial_apply_contract_event(NfSpatialWorld *w, const NfContractExchangeEvent *e) {
    if (w == NULL || e == NULL || e->object_id >= NF_SPATIAL_MAX_OBJECTS || !nf_spatial_is_valid_cell(w,e->cell) || e->radius < 0) return false;
    if (w->object_authority[e->object_id] != e->from_actor && e->from_actor != NF_SPATIAL_UNASSIGNED) return false;
    w->object_authority[e->object_id] = e->to_actor; w->object_cell[e->object_id] = e->cell;
    for (int c = 0; c < w->width * w->height; ++c) {
        if (!nf_spatial_is_valid_cell(w,c)) continue;
        int d = nf_spatial_shortest_path(w,e->cell,c);
        float retained = w->cells[c].contract_memory * clamp01(e->memory_decay);
        if (d < 0 || d > e->radius) { w->cells[c].contract_memory = retained; continue; }
        float sw = e->radius == 0 ? 1.0f : 1.0f - (float)d/(float)(e->radius+1);
        float dep = clamp01(e->assigned_weight) * clamp01(sw); w->cells[c].contract_memory = clamp01(retained + dep*(1.0f-retained));
    }
    return true;
}
void nf_spatial_world_init(NfSpatialWorld *w, NfSpatialLabKind lab, uint32_t seed) {
    uint32_t rng = mix32(seed ^ ((uint32_t)lab * 0x45d9f3bu)); base_world(w,lab,seed); carve_lab(w,&rng);
    for (int c = 0; c < w->width*w->height; ++c) if (w->cells[c].solid == 0u) {
        float open=(float)nf_spatial_neighbor_count(w,c)/4.0f;
        w->cells[c].exposure=clamp01(w->cells[c].exposure+0.65f*open-0.55f*w->cells[c].cover+0.08f*rng01(&rng));
        if (lab != NF_SPATIAL_LAB_ECOLOGY) { w->cells[c].ecological_risk=clamp01(0.08f+0.34f*rng01(&rng)+0.18f*open); w->cells[c].resource=clamp01(0.08f+0.42f*rng01(&rng)); }
    }
    if (w->start_cell >= 0) w->cells[w->start_cell].solid=0u;
    if (w->goal_cell >= 0) w->cells[w->goal_cell].solid=0u;
    if (lab == NF_SPATIAL_LAB_CONTRACT) {
        w->object_authority[0]=NF_SPATIAL_ACTOR_COMMONS; w->object_cell[0]=nf_spatial_cell_xy(w,6,5);
        NfContractExchangeEvent e={0u,NF_SPATIAL_ACTOR_COMMONS,NF_SPATIAL_ACTOR_RIVAL,0.85f,0.92f,w->object_cell[0],4,1u}; (void)nf_spatial_apply_contract_event(w,&e);
    }
}

static uint32_t round_hash(const NfSpatialRoundResult *r) {
    uint32_t h=2166136261u; const uint32_t vals[]={r->seed,r->lab,r->policy,r->success,r->steps,r->unique_cells,r->revisits,r->retreats,r->support_transitions,r->affordance_accepts,r->topology_updates,(uint32_t)lroundf(r->health_remaining*1000.0f),(uint32_t)lroundf(r->exposure_accum*1000.0f),(uint32_t)lroundf(r->ecology_cost_accum*1000.0f),(uint32_t)lroundf(r->resource_collected*1000.0f),(uint32_t)lroundf(r->contract_context_seen*1000.0f)};
    for (size_t i=0;i<sizeof(vals)/sizeof(vals[0]);++i){h^=vals[i];h*=16777619u;} return h;
}
static float score_cell(const NfSpatialWorld *w,int cur,int cand,int goal,const uint8_t *known,const uint8_t *vis,NfSpatialPolicyKind p,uint32_t *rng){
    if(p==NF_SPATIAL_POLICY_RANDOM) return rng01(rng);
    float s=-1.55f*(float)manhattan(w,cand,goal);
    if(p==NF_SPATIAL_POLICY_GREEDY) return s+0.02f*rng01(rng);
    if(known[cand]){const NfSpatialCell*c=&w->cells[cand];s-=4.2f*c->exposure+3.6f*c->ecological_risk;s+=0.55f*c->resource+0.12f*(float)nf_spatial_neighbor_count(w,cand)+0.45f*nf_spatial_affordance_candidate(w,cur,cand)+0.10f*c->contract_memory;}else{s-=0.35f;}
    if(vis[cand]) s-=1.20f*(float)vis[cand];
    return s+0.02f*rng01(rng);
}
NfSpatialRoundResult nf_spatial_run_round(NfSpatialLabKind lab,NfSpatialPolicyKind policy,uint32_t seed,uint8_t replicate){
    NfSpatialWorld w; NfSpatialRoundResult r; uint8_t known[NF_SPATIAL_MAX_CELLS]={0},vis[NF_SPATIAL_MAX_CELLS]={0},taken[NF_SPATIAL_MAX_CELLS]={0}; uint32_t rng=mix32(seed^((uint32_t)lab<<16)^((uint32_t)policy<<24)); float health=100.0f; int cur; uint16_t updates=0u;
    memset(&r,0,sizeof(r));nf_spatial_world_init(&w,lab,seed);cur=w.start_cell;r.seed=seed;r.lab=(uint8_t)lab;r.policy=(uint8_t)policy;r.replicate=replicate;
    if(!nf_spatial_is_valid_cell(&w,cur)||nf_spatial_shortest_path(&w,cur,w.goal_cell)<0){r.deterministic_hash=round_hash(&r);return r;}vis[cur]=1u;r.unique_cells=1u;
    for(uint16_t step=0u;step<NF_SPATIAL_MAX_STEPS&&health>0.0f;++step){
        (void)nf_spatial_observe_local(&w,cur,3,known,sizeof(known)); if(cur==w.goal_cell){r.success=1u;break;}
        if(lab==NF_SPATIAL_LAB_DYNAMIC&&step>0u&&step%12u==0u){
            int g=nf_spatial_cell_xy(&w,6,5);
            if(g>=0){w.cells[g].solid=w.cells[g].solid?0u:1u;nf_spatial_mark_dirty(&w,g,2);updates=(uint16_t)(updates+nf_spatial_recompute_dirty(&w,12u));}
        }
        int cand[4],n=0;for(int k=0;k<4;++k){int q=neighbor(&w,cur,k);if(nf_spatial_is_valid_cell(&w,q))cand[n++]=q;}if(n==0)break;
        int chosen=cand[0];float best=-1000000.0f;for(int k=0;k<n;++k){float s=score_cell(&w,cur,cand[k],w.goal_cell,known,vis,policy,&rng);if(s>best){best=s;chosen=cand[k];}}
        if(manhattan(&w,chosen,w.goal_cell)>manhattan(&w,cur,w.goal_cell)) ++r.retreats;
        if(vis[chosen]){++r.revisits;if(vis[chosen]<255u)++vis[chosen];}else{vis[chosen]=1u;++r.unique_cells;}
        if(w.cells[cur].support_kind!=w.cells[chosen].support_kind) ++r.support_transitions;
        if(nf_spatial_affordance_candidate(&w,cur,chosen)>0.3f) ++r.affordance_accepts;
        cur=chosen;
        const NfSpatialCell*c=&w.cells[cur];r.exposure_accum+=c->exposure;r.ecology_cost_accum+=c->ecological_risk;r.contract_context_seen+=c->contract_memory;health-=1.75f*c->exposure+1.45f*c->ecological_risk;
        if(!taken[cur]&&c->resource>0.25f){float gain=3.0f*c->resource;health+=gain;if(health>100.0f)health=100.0f;r.resource_collected+=c->resource;taken[cur]=1u;}r.steps=(uint16_t)(step+1u);
    }
    if(cur==w.goal_cell) r.success=1u;
    r.health_remaining=health>0.0f?health:0.0f;
    r.topology_updates=updates;
    r.deterministic_hash=round_hash(&r);
    return r;
}
const char *nf_spatial_lab_name(NfSpatialLabKind lab){static const char*n[NF_SPATIAL_LAB_COUNT]={"corridor","loop","vertical","cover","snap","dynamic","ecology","contract"};return lab>=0&&lab<NF_SPATIAL_LAB_COUNT?n[lab]:"unknown";}
const char *nf_spatial_policy_name(NfSpatialPolicyKind p){static const char*n[NF_SPATIAL_POLICY_COUNT]={"informed","greedy","random"};return p>=0&&p<NF_SPATIAL_POLICY_COUNT?n[p]:"unknown";}
