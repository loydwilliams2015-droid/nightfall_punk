#include "nf_security.h"

#include <string.h>
#include <time.h>

#ifdef NF_HAVE_SODIUM
#include <sodium.h>
#else
static uint32_t g_state = 0x9e3779b9u;
static uint32_t fallback_u32(void) {
    g_state ^= g_state << 13;
    g_state ^= g_state >> 17;
    g_state ^= g_state << 5;
    return g_state;
}
#endif

bool nf_security_init(void) {
#ifdef NF_HAVE_SODIUM
    return sodium_init() >= 0;
#else
    g_state ^= (uint32_t)time(NULL);
    return true;
#endif
}

bool nf_security_is_strong(void) {
#ifdef NF_HAVE_SODIUM
    return true;
#else
    return false;
#endif
}

void nf_security_random(uint8_t *out, size_t size) {
    if (out == NULL) return;
#ifdef NF_HAVE_SODIUM
    randombytes_buf(out, size);
#else
    for (size_t i = 0; i < size; ++i) {
        if ((i & 3u) == 0u) g_state = fallback_u32();
        out[i] = (uint8_t)(g_state >> (8u * (i & 3u)));
    }
#endif
}

void nf_security_derive_token(const uint8_t *client_nonce, const uint8_t *server_nonce, uint8_t *out) {
#ifdef NF_HAVE_SODIUM
    uint8_t material[NF_NET_NONCE_BYTES * 2];
    memcpy(material, client_nonce, NF_NET_NONCE_BYTES);
    memcpy(material + NF_NET_NONCE_BYTES, server_nonce, NF_NET_NONCE_BYTES);
    crypto_generichash(out, NF_NET_TOKEN_BYTES, material, sizeof(material), NULL, 0);
    sodium_memzero(material, sizeof(material));
#else
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < NF_NET_NONCE_BYTES; ++i) {
        h ^= client_nonce[i];
        h *= 16777619u;
        h ^= server_nonce[i];
        h *= 16777619u;
    }
    for (size_t i = 0; i < NF_NET_TOKEN_BYTES; ++i) {
        h ^= h << 13;
        h ^= h >> 17;
        h ^= h << 5;
        out[i] = (uint8_t)(h >> ((i & 3u) * 8u));
    }
#endif
}

bool nf_security_token_is_zero(const uint8_t *token) {
    uint8_t acc = 0;
    for (size_t i = 0; i < NF_NET_TOKEN_BYTES; ++i) acc |= token[i];
    return acc == 0;
}

bool nf_security_token_equal(const uint8_t *a, const uint8_t *b) {
#ifdef NF_HAVE_SODIUM
    return sodium_memcmp(a, b, NF_NET_TOKEN_BYTES) == 0;
#else
    uint8_t diff = 0;
    for (size_t i = 0; i < NF_NET_TOKEN_BYTES; ++i) diff |= (uint8_t)(a[i] ^ b[i]);
    return diff == 0;
#endif
}
