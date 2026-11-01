/* SPDX-License-Identifier: GPL-3.0-or-later
 * H63: bounded minimalist 3D Pong physics reference.
 * Uses A4 real normal/tangential impulses; independent research game model,
 * NOT an A6/B2/A5/B3 authoritative transaction.
 */
#ifndef NF_RETC_PONG_H
#define NF_RETC_PONG_H
#include "nf_contact18a4.h"
#include <stdint.h>
#include <stdbool.h>
#define NF_PONG_SCORE_LIMIT UINT32_MAX
#define NF_PONG_COURT_X 8.5f
#define NF_PONG_COURT_Z 4.2f
#define NF_PONG_PADDLE_X 7.25f
#define NF_PONG_PADDLE_HALF_Z .95f
#define NF_PONG_BALL_RADIUS .26f
typedef struct {
 Nf18a4Body ball;
 float paddle_z[2], paddle_velocity_z[2];
 uint32_t score[2]; /* left, right */
 uint32_t tick, revision, paddle_hits, wall_hits;
 uint32_t serve_delay; /* deterministic pause after score */
 int serve_direction;  /* -1 toward left, +1 toward right */
} NfPongWorld;
typedef struct {float paddle_axis[2];} NfPongInput;
typedef enum { NF_PONG_OK=0, NF_PONG_PENDING, NF_PONG_STALE, NF_PONG_INVALID } NfPongStatus;
typedef struct {
 NfPongWorld next;
 uint32_t from_tick, from_revision;
 uint32_t impulses_applied, scored;
} NfPongCandidate;
void nf_pong_init(NfPongWorld *world);
NfPongStatus nf_pong_stage(const NfPongWorld *world,const NfPongInput *input,
                          float dt,NfPongCandidate *candidate);
NfPongStatus nf_pong_publish(NfPongWorld *world,const NfPongCandidate *candidate);
#endif
