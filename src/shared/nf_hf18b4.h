#ifndef NF_HF18B4_H
#define NF_HF18B4_H
#include <stdbool.h>
#include <stdint.h>
typedef enum { NF_HF_IDLE, NF_HF_ATTEMPT, NF_HF_PENDING, NF_HF_CONTESTED, NF_HF_REFUSED, NF_HF_APPLIED, NF_HF_INTERRUPTED } NfHfState;
typedef enum { NF_HF_INPUT_PRESS, NF_HF_INPUT_RELEASE, NF_HF_SERVER_PENDING, NF_HF_SERVER_CONTESTED, NF_HF_SERVER_REFUSED, NF_HF_SERVER_APPLIED, NF_HF_SERVER_INTERRUPTED } NfHfEvent;
typedef struct { uint32_t actor, action, seq; NfHfState state; uint8_t toggle_mode, holding, confirmed; } NfHfClient;
/* Presentation only. No authority over physics, grants, debits or other actors. */
bool nf_hf_input(NfHfClient *c, NfHfEvent e, uint32_t seq);
bool nf_hf_server(NfHfClient *c, NfHfEvent e, uint32_t seq);
const char *nf_hf_label(NfHfState s);
#endif
