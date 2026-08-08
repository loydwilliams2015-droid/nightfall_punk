#ifndef NF_SECURITY_H
#define NF_SECURITY_H
#include "nf_protocol.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool nf_security_init(void);
bool nf_security_is_strong(void);
void nf_security_random(uint8_t *out, size_t size);
void nf_security_derive_token(const uint8_t *client_nonce, const uint8_t *server_nonce, uint8_t *out);
bool nf_security_token_is_zero(const uint8_t *token);
bool nf_security_token_equal(const uint8_t *a, const uint8_t *b);
#endif
