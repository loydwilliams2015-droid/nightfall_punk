#ifndef NF_RETC_NETWORK_H
#define NF_RETC_NETWORK_H
/* H53: opt-in, non-authoritative RET-C observation over protocol v6.
   This is NOT the gameplay snapshot, authenticated channel or full reader cutover. */
#include "nf_retc_external.h"
#include "nf_protocol.h"
#define NF_RETC_NETWORK_PACKET_BYTES (8u + NF_RETC_EXTERNAL_WIRE_BYTES)
/* Returns exact packet length, 0 on missing/non-joint accepted frame. */
size_t nf_retc_network_emit(const NfRetcPublisher *publisher,
                           uint8_t *out,size_t capacity);
/* One external consumer: on failure output is empty and cursor unchanged. */
bool nf_retc_network_consume(const uint8_t *packet,size_t size,
                            NfRetcExternalCursor *cursor,
                            NfRetcExternalRecord *out);
#endif
