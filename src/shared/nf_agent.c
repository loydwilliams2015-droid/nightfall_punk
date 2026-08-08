#include "nf_agent.h"

const char *nf_agent_mode_name(NfAgentMode mode) {
    switch (mode) {
        case NF_AGENT_IDLE: return "IDLE";
        case NF_AGENT_INVESTIGATE: return "INVESTIGATE";
        case NF_AGENT_ADVANCE: return "ADVANCE";
        case NF_AGENT_ENGAGE: return "ENGAGE";
        case NF_AGENT_SEEK_COVER: return "SEEK_COVER";
        case NF_AGENT_RETREAT: return "RETREAT";
        case NF_AGENT_RELOAD: return "RELOAD";
        case NF_AGENT_TRUCE_HOLD: return "TRUCE_HOLD";
        default: return "UNKNOWN";
    }
}

const char *nf_squad_role_name(NfSquadRole role) {
    switch (role) {
        case NF_SQUAD_PRESSURE: return "PRESSURE";
        case NF_SQUAD_FLANK_LEFT: return "FLANK_LEFT";
        case NF_SQUAD_FLANK_RIGHT: return "FLANK_RIGHT";
        case NF_SQUAD_HOLD: return "HOLD";
        default: return "UNKNOWN";
    }
}
