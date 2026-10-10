# Third-party dependencies and comparative references

This file distinguishes actual build/runtime dependencies from design references. It is not a substitute for including upstream license texts when a distribution requires them.

## Build/runtime dependencies observed in CMake

| Dependency | Current project reference | Upstream license | Relationship |
|---|---|---|---|
| ENet | 1.3.18 | MIT | system package or FetchContent networking dependency |
| raylib | 5.5 | zlib/libpng | system package or FetchContent client/rendering dependency |
| libsodium | optional system dependency | ISC | authenticated token derivation when present |

Preserve upstream notices when redistributing covered source/binaries. If fetched dependencies bundle further third-party code, follow their upstream distribution notices as well.

## Comparative/reference projects

GZDoom, LibreQuake/Quake, Tomb Raider, Unreal Engine, and other named games/engines may appear in design notes as mechanism precedents or counterpoints. Such references **do not mean their code or proprietary assets are incorporated**. Any future compatibility/import work must be audited separately under the applicable licenses.

## Verified upstream license references

- ENet 1.3.18: upstream MIT license.
- raylib 5.5: upstream zlib/libpng license.
- libsodium: upstream ISC license.

The project does not vendor these dependencies in the reviewed tree; CMake resolves them from system packages or upstream FetchContent where configured.

## Audit note — 2026-10-10

The reviewed nightfall!punk tree did not contain an obvious vendored `vendor/`, `external/`, or third-party source directory for the named comparison projects. CMake presently resolves the dependencies above from system packages or upstream fetches. Re-run this audit before release.