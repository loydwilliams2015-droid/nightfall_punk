# nightfall!punk — minimalist 3D Pong, VPC-01 physics descendant

**Internal engineering QA / playable research demo.** This is a deliberate small 3D game using real A4 contact impulses. It is not the completed A6/B2/A5/B3 authoritative multiplayer game, does not certify wall-riding/grinding, and does not itself satisfy all 1.8C.x production gates.

## Play
From the extracted `nightfall-vpc01-pong-qa` folder, execute:
```bash
sha256sum -c SHA256SUMS
./run.sh
```
- W / S: left paddle movement
- Right paddle: simple automated defender by default
- Tab: toggle the right paddle between automated and manual (up/down arrows)
- R: reset scores and serve
- Esc: exit

The ball, walls and paddles occupy an XZ-plane 3D scene. Contact response is the actual A4 impulse solver, with limited swept collision times. The game displays paddle/rail contact counters and a two-sided scoreboard. Unsupported simultaneous collision configurations fail closed instead of inventing a result.

## Reproducibility
- `SOURCE_SHA`: exact repository commit for this binary.
- `SHA256SUMS`: integrity of all packaged files.
- `LICENSE`: unmodified GNU GPLv3; the project elects `GPL-3.0-or-later`.
- `docs/LICENSING.md`: copyright attribution currently provisional; no registered copyright, future ownership decision or fictitious entity asserted.
- `source/nightfall_punk-source.tar.gz`: complete Git-tracked source at build SHA, including build scripts and test harness.
- `source/raylib-5.5-source.tar.gz` and `third_party/raylib-LICENSE`: bundled corresponding source and attribution for linked raylib.
- `docs/VPC01_FINAL_18CX_3D_PONG.md`: intentional demonstration charter and unresolved engineering gates.

Linux x86-64 with graphical session and working OpenGL/X11 drivers required; dynamically linked system libraries are not guaranteed portable between distros. This archive is built and tested with Ubuntu 22.04 and is intended for owner QA of a bounded demo, not production distribution. Preserve licenses and source archives if sharing in compliance with applicable terms and verified ownership rights.

## Human observation
One tester reported positive early responsiveness and delicacy from the earlier VPC-01 physics viewer. That observation helped select the Pong presentation but is **not** a source-exact performance measurement or proof of future momentum wall-riding. Give any new evaluator the open-ended, unprimed prompt in `docs/VPC01_HUMAN_OBSERVATION_PILOT.md`; record independent testimony and exact source/hardware information.
