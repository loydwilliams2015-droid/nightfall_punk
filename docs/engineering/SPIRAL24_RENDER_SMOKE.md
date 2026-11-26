# Spiral 24 — legacy client MAP-D02 smoke (draft integration)

This is a **visual-only, opt-in** integration against the historical `main` C/raylib client. It is **not** an A6/PHYSICS integration and does not establish production RENDER 7-gate acceptance.

## Dependencies
- raylib 5.5 Linux development package
- ENet 1.3.18 source tree
- CMake, C11 toolchain and optionally Xvfb and Mesa/OpenGL for headless captures

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DNF_LOCAL_ENET_ROOT=/absolute/path/to/enet-1.3.18 \
  -DNF_LOCAL_RAYLIB_ROOT=/absolute/path/to/raylib-5.5_linux_amd64 \
  -DNF_FETCH_RAYLIB=OFF -DNF_FETCH_ENET=OFF
cmake --build build -j4
ctest --test-dir build --output-on-failure
```

Start `build/nightfall_server` in one terminal. In a second terminal:

```sh
LD_LIBRARY_PATH=/absolute/path/to/raylib-5.5_linux_amd64/lib \
  build/nightfall_client --render-smoke-frames 110 \
  --render-capture client_connected.png \
  --map-d02-visual assets/map_d02/map_d02_visual.obj
```

A capture is refused with nonzero exit status if no welcome and nonzero server snapshot tick have been observed at the target frame. This is a **smoke check** for connection and rendering, not a cryptographic authentication or A6 provenance check. Without `--map-d02-visual`, the map overlay is disabled. An invalid map asset is logged and drawing proceeds without the overlay.

The OBJ is **synthetic**, not actual Los Angeles GIS. It carries no collision or gameplay authority. The original simulation and networking data paths are unchanged.

## Local evidence (2026-11-26)
- Real `nightfall_client` and `nightfall_server` compiled using supplied raylib 5.5 and ENet 1.3.18 sources
- Original eight CTest cases passed
- Connected 110-frame smoke saved 1280x720 screenshot and reported received server tick > 0
- Deliberately disconnected 12-frame smoke refused screenshot and returned nonzero
- Mesa llvmpipe under Xvfb (software OpenGL), **not a GPU-hardware benchmark**

**HOLD** for review: source-exact A6 publication, PHYSICS morphology, GIS provenance, sound, streaming, production RENDER formal acceptance. CI/independent validation of this GitHub commit pending.
