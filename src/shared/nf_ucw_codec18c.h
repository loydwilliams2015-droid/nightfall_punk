#ifndef NF_UCW_CODEC18C_H
#define NF_UCW_CODEC18C_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
/* Bounded portable mechanic TLV contract: independent of native C ABI.
 * All integers are explicit big-endian; versions are per mechanic.
 * This is a reusable codec substrate, NOT a full A5 serializer. */
#define NFUC_CODEC_MAX 16u
#define NFUC_CODEC_BYTES 4096u
#define NFUC_CODEC_REQUIRED 1u
typedef enum {NFUC_CODEC_OK, NFUC_CODEC_UNKNOWN_REQUIRED,
 NFUC_CODEC_BAD_VERSION, NFUC_CODEC_CORRUPT, NFUC_CODEC_FULL} NfucCodecStatus;
typedef bool (*NfucEncode)(const void *mechanic,unsigned char *out,size_t capacity,size_t *length);
typedef bool (*NfucDecode)(void *mechanic,const unsigned char *src,size_t length);
typedef struct {
 uint32_t id,version;
 bool required;
 void *state;
 NfucEncode encode;
 NfucDecode decode;
} NfucCodec;
typedef struct {NfucCodec items[NFUC_CODEC_MAX];size_t count;} NfucRegistry;
bool nfuc_codec_register(NfucRegistry *registry,NfucCodec codec);
NfucCodecStatus nfuc_codec_write(const NfucRegistry *registry,unsigned char *buf,size_t capacity,size_t *out_len);
NfucCodecStatus nfuc_codec_read(const NfucRegistry *registry,const unsigned char *buf,size_t length);
#endif
