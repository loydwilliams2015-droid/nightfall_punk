# 1.8C.2 H53 — Source-bound NF04 protocol observer (draft evidence)

Base: H52 `500a95a52678a754961ecfbc332e5257a29af85d`.
Purpose: exercise an actual protocol-framed **read-only** consumer from the real
A6/B2/B3 two-tick fixture without forging missing full-game state.

## Source changes

- `NF_MSG_RETC_OBSERVATION=10` reserved as a diagnostic-only NF04 v6 type.
- `nf_retc_network_emit` checks one *accepted* immutable RET-C frame,
  translates the H52 external projection to a 100-byte packet (8-byte NF04
  framing + 92-byte NFEX V1 record). No server world mutation.
- `nf_retc_network_consume` validates the existing NF04 header,
  diagnostic type, reserved byte, payload size, checksum, typed invariants,
  and strict transaction/epoch monotonicity.
- Real A6 test: prepublication refusal, two authoritative consecutive ticks,
  actual `nf_protocol_peek_type`, successful receiver consumption, duplicate
  refusal, truncation, header spoof, reserved-byte change, corrupt payload.
- Rejected A6 transaction: cannot emit a newer packet; exact bytes remain
  those of the accepted tick-2 observation, which the receiver rejects as replay.

## Critical nonclaims

This tests a **real serialisation/consumer boundary within the existing NF04
protocol framing**, but NOT a live network socket, delivered game snapshot or
full remote actor state. The current game server's snapshot still reads
`NfWorld` and includes up to 16 actors, weapon authority and other fields
absent from the RET-C single-actor prototype. Cutting it over now would be
incorrect. No gameplay client handles the new diagnostic type yet.

NFEX checksum is noncryptographic. An attacker can forge a plausible packet.
Do not use the diagnostic record for player actions, permission decisions or
trusted server state. Sender/session authentication and lifecycle safety
remain independent gates.

Unclosed: actual server A6 owner, complete multi-actor frame, the entire
network/AI/prediction/camera reader inventory, concurrent publisher disposal,
portable durable commit/recovery, ThreadSanitizer runtime qualification,
remaining B3 typed realizers, VPC-01 in-game visual-physical acceptance.

## Acceptance and human evaluation

CMake Debug GCC + Clang, focused A6/RET-C tests, full headless CTest,
GitHub PR-wide workflows and source SHA must be recorded as executed evidence.
Separate survey process in `docs/VPC01_HUMAN_OBSERVATION_PILOT.md`.
2–3 volunteers are exploratory independent observations, not statistical
significance; ML p-values close to 1 remain undetermined.

Production release, PR merge, and promotion to 1.8C.3: **HOLD**.
