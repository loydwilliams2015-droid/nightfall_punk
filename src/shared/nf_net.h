#ifndef NF_NET_H
#define NF_NET_H
#include "nf_protocol.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_NET_CHANNEL_RELIABLE 0u
#define NF_NET_CHANNEL_STATE 1u
#define NF_NET_CHANNEL_COUNT 2u
#define NF_NET_SIM_QUEUE 256u

typedef enum NfNetEventType { NF_NET_EVENT_NONE=0, NF_NET_EVENT_CONNECT, NF_NET_EVENT_RECEIVE, NF_NET_EVENT_DISCONNECT } NfNetEventType;
typedef struct NfNetEvent { NfNetEventType type; void *peer; uint8_t channel; size_t size; uint8_t data[NF_NET_MAX_PACKET_BYTES]; } NfNetEvent;
typedef struct NfQueuedPacket { bool used; void *peer; uint32_t due_ms; uint8_t channel; bool reliable; size_t size; uint8_t data[NF_NET_MAX_PACKET_BYTES]; } NfQueuedPacket;
typedef struct NfNetHost { void *host; bool server; uint32_t sim_latency_ms; uint32_t sim_jitter_ms; float sim_loss_percent; uint32_t rng; NfQueuedPacket queue[NF_NET_SIM_QUEUE]; } NfNetHost;

bool nf_net_global_init(void);
void nf_net_global_shutdown(void);
bool nf_net_server_open(NfNetHost *net, uint16_t port, size_t max_peers);
bool nf_net_client_open(NfNetHost *net);
void nf_net_close(NfNetHost *net);
void *nf_net_connect(NfNetHost *net, const char *host, uint16_t port);
void nf_net_disconnect(void *peer, uint32_t data);
int nf_net_service(NfNetHost *net, NfNetEvent *event, uint32_t timeout_ms);
bool nf_net_send(NfNetHost *net, void *peer, uint8_t channel, const uint8_t *data, size_t size, bool reliable);
void nf_net_flush(NfNetHost *net);
void nf_net_set_simulation(NfNetHost *net, uint32_t latency_ms, uint32_t jitter_ms, float loss_percent);
uint32_t nf_net_now_ms(void);
uint32_t nf_net_peer_rtt_ms(void *peer);
#endif
