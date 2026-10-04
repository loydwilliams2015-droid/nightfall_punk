#ifndef NF_ARCADE_H
#define NF_ARCADE_H
#include <stdint.h>
#include "nf_contact18a5.h"
#include "nf_embody18a6.h"
#define AW 24
#define AH 17
#define GHOSTS 3
#define MAX_TICKS 300

typedef enum { GAME_CINDER=0, GAME_SLIPGATE=1 } GameKind;
typedef enum { AI_CLASSIC=0, AI_SYSTEMIC=1 } AiPolicy;
typedef struct {int x,z; float energy; int last_x,last_z,last_tick; int stun; int decisions; uint8_t seen[AH][AW]; } Ghost;
typedef struct {
 GameKind kind; AiPolicy policy; uint32_t rng, seed; int tick, x,z,score,remaining,health,won,lost;
 int push_count,contact_count,pending_count,cache_loads,purple_count,blue_count,ghost_steps,ghost_chases,energy_retreats,stale_chases,blocked_moves,invalid_moves;
 int portal_count,gate_rejects,trace_raw,trace_events,history_fail,actors_delta,win_tick;
 float player_energy;
 char cells[AH][AW+1]; Ghost ghosts[GHOSTS];
 Nf18a5History history;
 Nf18a5Grid grid;
 uint32_t hash;
 int switches; int ladder_climbs;
} Arcade;
void arcade_init(Arcade *g, GameKind kind, AiPolicy policy, uint32_t seed);
int arcade_tick(Arcade *g, int action);
int arcade_bot_action(const Arcade *g);
void arcade_ascii(const Arcade *g);
uint32_t arcade_digest(const Arcade *g);
int arcade_selftest(void);
int arcade_scenario(const char *name);
const char *arcade_kind_name(GameKind kind);
const char *arcade_ai_name(AiPolicy policy);
#endif
