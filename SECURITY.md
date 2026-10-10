# Security policy

## Reporting

If GitHub private vulnerability reporting is enabled for this repository, use **Security → Report a vulnerability**. If it is unavailable, open a minimal public issue requesting a private contact channel **without publishing exploit details, credentials, or sensitive logs**.

## Scope

Networking, prediction/reconciliation, authentication/session-token handling, parsers/importers, persistence/WAL code, and dependency updates should receive security review before production claims.

## Secrets

Never commit API tokens, private keys, passwords, personal access tokens, server credentials, or production certificates. Rotate any secret that is accidentally committed; deleting a file from the latest commit alone is not sufficient.

## Dependency posture

Track ENet, raylib, libsodium, compiler/toolchain, and operating-system package provenance for releases. Security status is separate from gameplay/scientific evidence grade.
