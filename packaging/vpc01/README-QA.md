# nightfall!punk — VPC-01 two-actor Linux engineering evaluation

**Status: INTERNAL ENGINEERING QA CANDIDATE, NOT an eligible public human playtest.**  
This package intentionally does not claim 1.8C.2 completion, networking, production readiness or verified A6/B2/B3 two-actor authority.

## Contents
- `bin/nightfall_vpc01_two_actor` — actual raylib viewer, rendered from the bounded VPC-01 physical reference model.
- `run.sh` — launch from the extracted folder.
- `SOURCE_SHA` and `SHA256SUMS` — exact source and package member provenance.
- `docs/` — unprimed observation protocol and acceptance contract.
- `LICENSE` — unmodified GPLv3 text; original nightfall!punk software is `GPL-3.0-or-later`.
- `docs/LICENSING.md` — licensing scope, third-party notices and pending ownership checks.
- `source/nightfall_punk-source.tar.gz` — exact Git-tracked source snapshot used for this build (includes build scripts).
- `source/raylib-5.5-source.tar.gz` — raylib 5.5 upstream corresponding source for the statically linked rendering library.
- `third_party/` — raylib attribution/license and source commit for the linked build.

## Local evaluation (Pop!_OS or compatible x86_64 Linux)
Download the workflow **artifact ZIP** and extract it first. The ZIP contains a `.tar.gz` engineering candidate. Run from a local folder:

```bash
tar -xzf nightfall-vpc01-h61-linux-x86_64-*.tar.gz
cd nightfall-vpc01-qa
sha256sum -c SHA256SUMS
./run.sh
```

Controls: WASD — actor A; Tab toggles actor B between scripted and arrow-key input; arrow keys — actor B (manual mode); R — reset; Esc — close. The viewer shows `STAGED` or `PENDING / REFUSED`. A refused tick is an *engineering gate*, not permission to invent an authoritative result. Use R to reset, and report the inputs and outcome.

The world has two upright bodies and one movable pod, using the actual A4 reciprocal contact impulse in a bounded horizontal swept-footprint prototype. Its visual geometry is an exploratory postcyberpunk/biopunk stand-in, not the finalized art benchmark. There is no live A6/B2/B3 joint-world tick, B2 source-backed eligibility, A5 material-world consequence, durable contract receipt or gameplay server cutover in the viewer.

On compatible Linux machines, raylib may require X11, OpenGL, ALSA and related runtime libraries. If the launcher cannot initialize a display, first confirm desktop graphics drivers and the dynamic dependencies listed by `ldd bin/nightfall_vpc01_two_actor`.

## Evaluation protocol
For any observational session, first use the neutral prompt in `docs/VPC01_HUMAN_OBSERVATION_PILOT.md`, label the build *engineering research only*, and do not coach observations or collect identifiable/sensitive data. No public volunteer recruitment is authorized by this bundle. The original older graybox remains a regression control.

## Mandatory unclosed gates
1. Source-exact GCC/Clang full and headless acceptance, installed-bundle graphical smoke, repeatable binary and third-party dependency audit.
2. Multi-actor A6 sole-owner state, B2 eligibility, A5 material truth, B3 typed receipt and joint durability; source-backed interaction observed in the actual client, not just the bounded A4 viewer.
3. Whole-engine lifetime, concurrent actor/world, transport and shutdown safety; exact player-facing VPC-01 acceptance.
4. **GPL-3.0-or-later is selected and the root LICENSE is included.** Verify project copyright-holder permissions, compatibility of all externally sourced materials, and final attribution before public distribution. Raylib's license is separate.
5. Voluntary pilot consent, explicit evaluator instructions, representative frame-time and reproducibility gates, and a release-owner approval decision.

**No p-value** is inferred from deterministic tests or one qualitative pilot.

## License and corresponding source

The project's original software is designated `GPL-3.0-or-later` (see `LICENSE`). This archive includes a source snapshot and separately linked raylib source; their exact provenance is recorded in `SOURCE_SHA` and `third_party/raylib-SOURCE_SHA`. The build is a bounded research viewer, and this package does not claim that adding a license establishes ownership of historic contributions. Respect the included third-party notices and the independent release holds.
