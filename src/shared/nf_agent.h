#ifndef NF_AGENT_H
#define NF_AGENT_H

#include "nf_world.h"

#include <stdint.h>

typedef enum NfControlSource {
    NF_CONTROL_NETWORK = 0,
    NF_CONTROL_AI = 1
} NfControlSource;

typedef enum NfAgentMode {
    NF_AGENT_IDLE = 0,
    NF_AGENT_INVESTIGATE,
    NF_AGENT_ADVANCE,
    NF_AGENT_ENGAGE,
    NF_AGENT_SEEK_COVER,
    NF_AGENT_RETREAT,
    NF_AGENT_RELOAD,
    NF_AGENT_TRUCE_HOLD,
    NF_AGENT_MODE_COUNT
} NfAgentMode;

typedef enum NfSquadRole {
    NF_SQUAD_PRESSURE = 0,
    NF_SQUAD_FLANK_LEFT,
    NF_SQUAD_FLANK_RIGHT,
    NF_SQUAD_HOLD
} NfSquadRole;

typedef struct NfControlFrame {
    NfEntityId actor;
    NfControlSource source;
    NfMoveInput move;
    NfCombatInput combat;
    uint64_t perceived_tick;
    uint32_t sequence;
} NfControlFrame;

const char *nf_agent_mode_name(NfAgentMode mode);
const char *nf_squad_role_name(NfSquadRole role);

#endif
