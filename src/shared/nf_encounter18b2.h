#ifndef NF_ENCOUNTER18B2_H
#define NF_ENCOUNTER18B2_H
#include <stdbool.h>
#include <stdint.h>
typedef enum { NF_E_PROPOSED=0,NF_E_ELIGIBLE,NF_E_RESERVED,NF_E_COMMITTED,NF_E_EXECUTING,NF_E_COMPLETED,NF_E_BLOCKED,NF_E_PENDING,NF_E_EXPIRED,NF_E_CANCELLED,NF_E_FAILED,NF_E_INTERRUPTED } NfEncounterPhase;
typedef enum { NF_E_OK=0,NF_E_DENIED,NF_E_STALE,NF_E_DUPLICATE,NF_E_BAD_INPUT } NfEncounterResult;
typedef struct {uint32_t tick,actor_id,object_id,action_id,world_revision,material_epoch,contract_revision;NfEncounterPhase phase;uint8_t published;} NfEncounter;
typedef struct {uint32_t world_revision,material_epoch,contract_revision;} NfEncounterEpochs;
void nf_encounter_init(NfEncounter *,uint32_t,uint32_t,uint32_t,uint32_t,NfEncounterEpochs);
NfEncounterResult nf_encounter_eligible(NfEncounter *,bool,bool,bool,NfEncounterEpochs);
NfEncounterResult nf_encounter_reserve(NfEncounter *,NfEncounterEpochs);
NfEncounterResult nf_encounter_commit(NfEncounter *,NfEncounterEpochs,bool);
NfEncounterResult nf_encounter_execute(NfEncounter *);
NfEncounterResult nf_encounter_complete(NfEncounter *,bool);
NfEncounterResult nf_encounter_cancel(NfEncounter *);
#endif
