#ifndef NF_PROTOCOL_H
#define NF_PROTOCOL_H

#include "nf_world.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_PROTOCOL_MAGIC 0x3330464eu /* "NF03" little-endian */
#define NF_PROTOCOL_VERSION 3u
#define NF_NET_DEFAULT_PORT 7777u
#define NF_NET_MAX_PACKET_BYTES 1200u
#define NF_NET_MAX_PLAYERS 4u
#define NF_NET_MAX_SNAPSHOT_ACTORS 16u
#define NF_NET_INPUT_REDUNDANCY 3u
#define NF_NET_HISTORY 256u
#define NF_NET_TOKEN_BYTES 16u
#define NF_NET_NONCE_BYTES 16u
#define NF_NET_SNAPSHOT_HZ 30u
#define NF_NET_RECONNECT_WINDOW_MS 20000u

typedef enum NfMessageType {
    NF_MSG_NONE = 0,
    NF_MSG_HELLO = 1,
    NF_MSG_WELCOME = 2,
    NF_MSG_REJECT = 3,
    NF_MSG_INPUT = 4,
    NF_MSG_SNAPSHOT = 5,
    NF_MSG_PING = 6,
    NF_MSG_PONG = 7,
    NF_MSG_DISCONNECT = 8
} NfMessageType;

typedef struct NfHelloMessage {
    uint8_t client_nonce[NF_NET_NONCE_BYTES];
    uint8_t resume_token[NF_NET_TOKEN_BYTES];
} NfHelloMessage;

typedef struct NfWelcomeMessage {
    NfEntityId entity_id;
    uint64_t server_tick;
    uint32_t world_seed;
    uint8_t session_token[NF_NET_TOKEN_BYTES];
    bool strong_crypto;
} NfWelcomeMessage;

typedef struct NfInputCommand {
    uint32_t sequence;
    uint64_t client_tick;
    NfMoveInput move;
} NfInputCommand;

typedef struct NfInputBundle {
    uint8_t session_token[NF_NET_TOKEN_BYTES];
    uint8_t count;
    NfInputCommand commands[NF_NET_INPUT_REDUNDANCY];
} NfInputBundle;

typedef struct NfActorNetState {
    NfEntityId id;
    NfVec3 position;
    NfVec3 velocity;
    NfMovementMode mode;
    bool grounded;
    bool crouched;
    uint8_t jumps_used;
    int32_t attached_collider;
    NfTraversalType candidate_type;
    int32_t candidate_feature;
} NfActorNetState;

typedef struct NfSnapshotMessage {
    uint64_t server_tick;
    uint32_t acknowledged_input;
    uint8_t actor_count;
    NfActorNetState actors[NF_NET_MAX_SNAPSHOT_ACTORS];
} NfSnapshotMessage;

typedef struct NfPingMessage { uint32_t stamp_ms; } NfPingMessage;

NfMessageType nf_protocol_peek_type(const uint8_t *data, size_t size);
size_t nf_protocol_encode_hello(uint8_t *out, size_t cap, const NfHelloMessage *msg);
bool nf_protocol_decode_hello(const uint8_t *data, size_t size, NfHelloMessage *out);
size_t nf_protocol_encode_welcome(uint8_t *out, size_t cap, const NfWelcomeMessage *msg);
bool nf_protocol_decode_welcome(const uint8_t *data, size_t size, NfWelcomeMessage *out);
size_t nf_protocol_encode_reject(uint8_t *out, size_t cap, uint8_t reason);
bool nf_protocol_decode_reject(const uint8_t *data, size_t size, uint8_t *reason);
size_t nf_protocol_encode_input(uint8_t *out, size_t cap, const NfInputBundle *msg);
bool nf_protocol_decode_input(const uint8_t *data, size_t size, NfInputBundle *out);
size_t nf_protocol_encode_snapshot(uint8_t *out, size_t cap, const NfSnapshotMessage *msg);
bool nf_protocol_decode_snapshot(const uint8_t *data, size_t size, NfSnapshotMessage *out);
size_t nf_protocol_encode_ping(uint8_t *out, size_t cap, NfMessageType type, const NfPingMessage *msg);
bool nf_protocol_decode_ping(const uint8_t *data, size_t size, NfMessageType expected, NfPingMessage *out);

void nf_actor_to_net_state(const NfActor *actor, NfActorNetState *out);
void nf_actor_apply_net_state(NfActor *actor, const NfActorNetState *state, const NfMovementConfig *config);

#endif
