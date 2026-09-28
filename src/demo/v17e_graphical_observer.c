#include "nf_observe17e.h"
#include "nf_spatial_logic.h"
#include "raylib.h"
#include "raymath.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NF17E_MAX_FRAME_SAMPLES 8192u
#define NF17E_CELL_SIZE 1.0f

typedef struct ObserverState {
    NfSpatialWorld world;
    Nf17bAuthoritativeState authority;
    uint8_t known[NF_SPATIAL_MAX_CELLS];
    int actor_cell;
    int believed_goal;
    int selected_cell;
    Nf17dViewPreset preset;
    Nf17eOverlayPolicy overlay_policy;
    Nf17eOverlayContribution contributions[NF17E_MAX_OVERLAY_CONTRIBUTIONS];
    Nf17eOverlayObject objects[NF17E_MAX_OVERLAY_OBJECTS];
    size_t contribution_count;
    size_t object_count;
    uint32_t state_hash;
} ObserverState;

typedef struct BenchAccumulator {
    double frame_ms[NF17E_MAX_FRAME_SAMPLES];
    double compose_us[NF17E_MAX_FRAME_SAMPLES];
    double render_us[NF17E_MAX_FRAME_SAMPLES];
    size_t count;
    unsigned hash_mismatches;
    size_t object_sum;
    size_t contribution_sum;
} BenchAccumulator;

static Color dim_color(Nf17eOverlayDimension d) {
    switch (d) {
        case NF17E_DIM_WORLD: return (Color){115, 120, 132, 255};
        case NF17E_DIM_MATERIAL: return (Color){70, 190, 210, 255};
        case NF17E_DIM_EVIDENCE: return (Color){255, 215, 80, 255};
        case NF17E_DIM_BELIEF: return (Color){85, 150, 255, 255};
        case NF17E_DIM_CONFIDENCE: return (Color){80, 220, 170, 255};
        case NF17E_DIM_CONTRACT: return (Color){255, 145, 70, 255};
        case NF17E_DIM_TRANSACTION: return (Color){245, 245, 245, 255};
        case NF17E_DIM_CONFLICT: return (Color){235, 70, 70, 255};
        case NF17E_DIM_PURPLE: return (Color){180, 85, 235, 255};
        case NF17E_DIM_FRONTIER: return (Color){255, 95, 175, 255};
        case NF17E_DIM_REFINEMENT: return (Color){90, 230, 110, 255};
        case NF17E_DIM_PENDING: return (Color){255, 180, 40, 255};
        case NF17E_DIM_INVARIANT: return (Color){255, 50, 50, 255};
        default: return GRAY;
    }
}

static Color composite_color(const Nf17eOverlayObject *o) {
    if (o == NULL) return GRAY;
    if (o->composite_kind == (uint8_t)NF17E_COMPOSITE_RAINBOW) {
        return (Color){205, 105, 230, 215};
    }
    uint32_t mask = o->visible_primary_mask;
    for (int d = 0; d < NF17E_DIM_COUNT; ++d) {
        if ((mask & (1u << d)) != 0u) {
            Color c = dim_color((Nf17eOverlayDimension)d);
            c.a = 205;
            return c;
        }
    }
    return (Color){160, 160, 170, 180};
}

static Vector3 cell_center(const NfSpatialWorld *w, int cell, float y) {
    const int x = cell % w->width;
    const int z = cell / w->width;
    return (Vector3){
        ((float)x - (float)w->width * 0.5f + 0.5f) * NF17E_CELL_SIZE,
        y,
        ((float)z - (float)w->height * 0.5f + 0.5f) * NF17E_CELL_SIZE
    };
}

static int world_point_to_cell(const NfSpatialWorld *w, Vector3 p) {
    const float fx = p.x / NF17E_CELL_SIZE + (float)w->width * 0.5f;
    const float fz = p.z / NF17E_CELL_SIZE + (float)w->height * 0.5f;
    const int x = (int)floorf(fx);
    const int z = (int)floorf(fz);
    return nf_spatial_cell_xy(w, x, z);
}

static void add_contribution(
    ObserverState *s,
    int cell,
    Nf17eOverlayDimension dim,
    uint16_t priority,
    uint32_t source,
    bool pending,
    bool committed) {

    if (s->contribution_count >= NF17E_MAX_OVERLAY_CONTRIBUTIONS || cell < 0) return;
    Nf17eOverlayContribution *c = &s->contributions[s->contribution_count++];
    memset(c, 0, sizeof(*c));
    c->anchor_id = (uint32_t)cell + 1u;
    c->source_id = source;
    c->authority_hash = s->state_hash;
    c->payload_hash = (uint32_t)cell * 2654435761u ^ ((uint32_t)dim << 24) ^ source;
    c->priority = priority;
    c->anchor_kind = (uint8_t)NF17E_ANCHOR_CELL;
    c->dimension = (uint8_t)dim;
    c->pending = pending ? 1u : 0u;
    c->committed = committed ? 1u : 0u;
}

static void build_observability(ObserverState *s) {
    s->contribution_count = 0u;
    s->object_count = 0u;
    s->state_hash = nf17b_state_hash(&s->authority);

    const uint32_t mask = nf17d_overlay_mask(s->preset);
    const int total = s->world.width * s->world.height;

    if ((mask & (NF17D_OVERLAY_CELLS | NF17D_OVERLAY_MATERIAL)) != 0u) {
        for (int c = 0; c < total && s->contribution_count < NF17E_MAX_OVERLAY_CONTRIBUTIONS; ++c) {
            if (!nf_spatial_is_valid_cell(&s->world, c)) continue;
            add_contribution(s, c, NF17E_DIM_WORLD, 0u, 100u + (uint32_t)c, false, true);
            if (s->world.cells[c].exposure > 0.62f || s->world.cells[c].ecological_risk > 0.62f) {
                add_contribution(s, c, NF17E_DIM_MATERIAL, 0u, 500u + (uint32_t)c, false, true);
            }
        }
    }

    if ((mask & NF17D_OVERLAY_EVIDENCE) != 0u) {
        for (int c = 0; c < total && s->contribution_count < NF17E_MAX_OVERLAY_CONTRIBUTIONS; ++c) {
            if (s->known[c] != 0u) add_contribution(s, c, NF17E_DIM_EVIDENCE, 0u, 1000u + (uint32_t)c, false, true);
        }
    }
    if ((mask & NF17D_OVERLAY_BELIEF) != 0u && s->believed_goal >= 0) {
        add_contribution(s, s->believed_goal, NF17E_DIM_BELIEF, 0u, 2001u, false, true);
    }
    if ((mask & NF17D_OVERLAY_CONFIDENCE) != 0u && s->believed_goal >= 0) {
        add_contribution(s, s->believed_goal, NF17E_DIM_CONFIDENCE, 3u, 2002u, false, true);
    }
    if ((mask & NF17D_OVERLAY_CONTRACTS) != 0u) {
        for (int c = 0; c < total && s->contribution_count < NF17E_MAX_OVERLAY_CONTRIBUTIONS; ++c) {
            if (s->world.cells[c].contract_memory > 0.25f) {
                add_contribution(s, c, NF17E_DIM_CONTRACT, 0u, 3000u + (uint32_t)c, false, true);
            }
        }
    }

    const int door = nf_spatial_cell_xy(&s->world, 6, 5);
    const int purple = nf_spatial_cell_xy(&s->world, 7, 5);
    const int pending = nf_spatial_cell_xy(&s->world, 8, 5);
    const int refine = nf_spatial_cell_xy(&s->world, 5, 5);

    if ((mask & NF17D_OVERLAY_TRANSACTIONS) != 0u) {
        add_contribution(s, door, NF17E_DIM_TRANSACTION, 0u, 4001u, false, false);
    }
    if ((mask & NF17D_OVERLAY_CONFLICTS) != 0u) {
        add_contribution(s, door, NF17E_DIM_CONFLICT, 0u, 4002u, false, false);
    }
    if ((mask & NF17D_OVERLAY_PURPLE) != 0u) {
        add_contribution(s, purple, NF17E_DIM_PURPLE, 0u, 5001u, false, false);
        add_contribution(s, purple, NF17E_DIM_FRONTIER, 5u, 5002u, false, false);
    }
    if ((mask & NF17D_OVERLAY_FRONTIER) != 0u) {
        add_contribution(s, nf_spatial_cell_xy(&s->world, 7, 6), NF17E_DIM_FRONTIER, 0u, 6001u, false, false);
        add_contribution(s, nf_spatial_cell_xy(&s->world, 8, 6), NF17E_DIM_FRONTIER, 0u, 6002u, false, false);
    }
    if ((mask & NF17D_OVERLAY_REFINEMENT) != 0u) {
        add_contribution(s, refine, NF17E_DIM_REFINEMENT, 0u, 7001u, false, false);
    }
    if ((mask & NF17D_OVERLAY_REASON_TRACE) != 0u) {
        add_contribution(s, s->actor_cell, NF17E_DIM_TRANSACTION, 2u, 8001u, false, true);
    }
    if (s->preset == NF17D_VIEW_CAUSAL || s->preset == NF17D_VIEW_FULL) {
        add_contribution(s, pending, NF17E_DIM_PENDING, 0u, 9001u, true, false);
        add_contribution(s, door, NF17E_DIM_INVARIANT, 0u, 9002u, false, false);
    }

    s->object_count = nf17e_compose_overlays(
        &s->overlay_policy,
        s->contributions,
        s->contribution_count,
        s->objects,
        NF17E_MAX_OVERLAY_OBJECTS);
}

static void observer_init(ObserverState *s, uint32_t seed) {
    memset(s, 0, sizeof(*s));
    nf_spatial_world_init(&s->world, NF_SPATIAL_LAB_CONTRACT, seed);
    nf17b_state_init(&s->authority);
    nf17e_overlay_policy_default(&s->overlay_policy);
    s->actor_cell = s->world.start_cell;
    s->believed_goal = nf_spatial_cell_xy(&s->world, 9, 2);
    s->selected_cell = s->actor_cell;
    s->preset = NF17D_VIEW_WORLD;
    (void)nf_spatial_observe_local(&s->world, s->actor_cell, 4, s->known, NF_SPATIAL_MAX_CELLS);
    build_observability(s);
}

static void draw_world_base(const ObserverState *s) {
    const int total = s->world.width * s->world.height;
    for (int c = 0; c < total; ++c) {
        Vector3 p = cell_center(&s->world, c, 0.0f);
        if (s->world.cells[c].solid != 0u) {
            DrawCube((Vector3){p.x, 0.35f, p.z}, 0.94f, 0.70f, 0.94f, (Color){55, 58, 66, 255});
            DrawCubeWires((Vector3){p.x, 0.35f, p.z}, 0.94f, 0.70f, 0.94f, (Color){100, 105, 118, 255});
        } else {
            DrawCubeWires((Vector3){p.x, 0.01f, p.z}, 0.92f, 0.02f, 0.92f, (Color){45, 48, 56, 120});
        }
    }

    Vector3 actor = cell_center(&s->world, s->actor_cell, 0.55f);
    DrawCube(actor, 0.45f, 1.10f, 0.45f, (Color){240, 240, 245, 255});
    Vector3 goal = cell_center(&s->world, s->world.goal_cell, 0.18f);
    DrawSphere(goal, 0.18f, GOLD);
}

static void draw_overlay_object(const ObserverState *s, const Nf17eOverlayObject *o) {
    if (o == NULL || o->anchor_id == 0u) return;
    const int cell = (int)o->anchor_id - 1;
    if (cell < 0 || cell >= s->world.width * s->world.height) return;

    const Color main = composite_color(o);
    Vector3 p = cell_center(&s->world, cell, 0.16f);
    DrawCube((Vector3){p.x, 0.12f, p.z}, 0.76f, 0.10f, 0.76f, main);

    if (o->composite_kind != (uint8_t)NF17E_COMPOSITE_SINGLE) {
        DrawCubeWires((Vector3){p.x, 0.21f, p.z}, 0.86f, 0.28f, 0.86f, RAYWHITE);
    }

    int band = 0;
    for (int d = 0; d < NF17E_DIM_COUNT; ++d) {
        if ((o->dimension_mask & (1u << d)) == 0u) continue;
        Color c = dim_color((Nf17eOverlayDimension)d);
        const float y = 0.31f + 0.055f * (float)band;
        DrawLine3D(
            (Vector3){p.x - 0.34f, y, p.z - 0.34f},
            (Vector3){p.x + 0.34f, y, p.z + 0.34f},
            c);
        ++band;
        if (band >= 6) break;
    }

    if (o->has_pending != 0u) {
        DrawSphere((Vector3){p.x, 0.75f, p.z}, 0.11f, ORANGE);
    }
}

static void draw_overlays(const ObserverState *s) {
    if (s->preset == NF17D_VIEW_PLAY) return;
    for (size_t i = 0u; i < s->object_count; ++i) draw_overlay_object(s, &s->objects[i]);
}

static int pick_cell(const ObserverState *s, Camera3D camera) {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return s->selected_cell;
    Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
    if (fabsf(ray.direction.y) < 0.0001f) return s->selected_cell;
    const float t = -ray.position.y / ray.direction.y;
    if (t <= 0.0f) return s->selected_cell;
    Vector3 p = {
        ray.position.x + ray.direction.x * t,
        0.0f,
        ray.position.z + ray.direction.z * t
    };
    const int cell = world_point_to_cell(&s->world, p);
    return cell >= 0 ? cell : s->selected_cell;
}

static const Nf17eOverlayObject *object_for_cell(const ObserverState *s, int cell) {
    const uint32_t anchor = cell >= 0 ? (uint32_t)cell + 1u : 0u;
    for (size_t i = 0u; i < s->object_count; ++i) {
        if (s->objects[i].anchor_id == anchor) return &s->objects[i];
    }
    return NULL;
}

static void draw_inspector(const ObserverState *s, int width, int height) {
    const Nf17eOverlayObject *o = object_for_cell(s, s->selected_cell);
    DrawRectangle(width - 405, 12, 393, 184, (Color){12, 14, 18, 220});
    DrawRectangleLines(width - 405, 12, 393, 184, (Color){105, 110, 125, 255});
    DrawText("SELECTED DIAGNOSTIC OBJECT", width - 390, 24, 17, RAYWHITE);
    DrawText(TextFormat("cell %d  preset %s", s->selected_cell, nf17d_view_preset_name(s->preset)),
             width - 390, 50, 15, GRAY);
    if (o == NULL) {
        DrawText("no overlay object at anchor", width - 390, 76, 15, GRAY);
        return;
    }

    DrawText(TextFormat("kind %s  sources %u", nf17e_composite_kind_name((Nf17eCompositeKind)o->composite_kind), o->source_count),
             width - 390, 76, 15, RAYWHITE);
    DrawText(TextFormat("dimensions 0x%04X primary 0x%04X", o->dimension_mask, o->visible_primary_mask),
             width - 390, 98, 15, RAYWHITE);
    DrawText(TextFormat("suppressed 0x%04X pending %s committed %s",
                        o->suppressed_mask, o->has_pending ? "YES" : "NO", o->has_committed ? "YES" : "NO"),
             width - 390, 120, 15, o->has_pending ? ORANGE : GRAY);
    DrawText(TextFormat("priority %u signature %08X", o->highest_priority, o->composite_signature),
             width - 390, 142, 15, GRAY);
    DrawText(TextFormat("authority %08X", s->state_hash), width - 390, 164, 15, SKYBLUE);
    (void)height;
}

static int compare_double(const void *a, const void *b) {
    const double da = *(const double *)a;
    const double db = *(const double *)b;
    return da < db ? -1 : (da > db ? 1 : 0);
}

static double percentile(double *v, size_t n, double p) {
    if (n == 0u) return 0.0;
    qsort(v, n, sizeof(v[0]), compare_double);
    size_t idx = (size_t)floor(p * (double)(n - 1u));
    if (idx >= n) idx = n - 1u;
    return v[idx];
}

static void bench_write_row(
    FILE *out,
    const char *preset,
    const BenchAccumulator *a) {

    double frame[NF17E_MAX_FRAME_SAMPLES];
    double compose[NF17E_MAX_FRAME_SAMPLES];
    double render[NF17E_MAX_FRAME_SAMPLES];
    memcpy(frame, a->frame_ms, a->count * sizeof(double));
    memcpy(compose, a->compose_us, a->count * sizeof(double));
    memcpy(render, a->render_us, a->count * sizeof(double));

    double frame_sum = 0.0, compose_sum = 0.0, render_sum = 0.0;
    for (size_t i = 0u; i < a->count; ++i) {
        frame_sum += a->frame_ms[i];
        compose_sum += a->compose_us[i];
        render_sum += a->render_us[i];
    }
    const double n = a->count ? (double)a->count : 1.0;
    const double median = percentile(frame, a->count, 0.50);
    const double p95 = percentile(frame, a->count, 0.95);
    const double p99 = percentile(frame, a->count, 0.99);
    const double cp95 = percentile(compose, a->count, 0.95);
    const double rp95 = percentile(render, a->count, 0.95);

    fprintf(out,
        "%s,%zu,%.6f,%.6f,%.6f,%.6f,%.3f,%.3f,%.3f,%.3f,%.2f,%.2f,%u\n",
        preset,
        a->count,
        frame_sum / n,
        median,
        p95,
        p99,
        compose_sum / n,
        cp95,
        render_sum / n,
        rp95,
        (double)a->contribution_sum / n,
        (double)a->object_sum / n,
        a->hash_mismatches);
}

static Nf17dViewPreset preset_from_index(int i) {
    static const Nf17dViewPreset p[5] = {
        NF17D_VIEW_PLAY, NF17D_VIEW_WORLD, NF17D_VIEW_ACTOR, NF17D_VIEW_CAUSAL, NF17D_VIEW_FULL
    };
    return p[i < 0 ? 0 : (i > 4 ? 4 : i)];
}

static const char *bench_mode_name(int i) {
    static const char *names[6] = {"off", "play", "world", "actor", "causal", "full"};
    return names[i < 0 ? 0 : (i > 5 ? 5 : i)];
}

static Nf17dViewPreset bench_mode_preset(int i) {
    if (i <= 0) return NF17D_VIEW_PLAY;
    return preset_from_index(i - 1);
}

int main(int argc, char **argv) {
    bool benchmark = false;
    unsigned frames_per_preset = 360u;
    const char *csv_path = "v17e_benchmark.csv";
    uint32_t seed = 217017u;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--benchmark") == 0) benchmark = true;
        else if (strcmp(argv[i], "--frames") == 0 && i + 1 < argc) frames_per_preset = (unsigned)strtoul(argv[++i], NULL, 10);
        else if (strcmp(argv[i], "--csv") == 0 && i + 1 < argc) csv_path = argv[++i];
        else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) seed = (uint32_t)strtoul(argv[++i], NULL, 10);
    }
    if (frames_per_preset > NF17E_MAX_FRAME_SAMPLES) frames_per_preset = NF17E_MAX_FRAME_SAMPLES;

    const int width = 1280, height = 760;
    InitWindow(width, height, "nightfall!punk v1.7E — Graphical Observability");
    SetTargetFPS(benchmark ? 0 : 120);

    ObserverState state;
    observer_init(&state, seed);

    Camera3D camera = {
        .position = {10.0f, 13.5f, -13.0f},
        .target = {0.0f, 0.0f, 0.0f},
        .up = {0.0f, 1.0f, 0.0f},
        .fovy = 48.0f,
        .projection = CAMERA_PERSPECTIVE
    };

    FILE *csv = NULL;
    BenchAccumulator acc[6];
    memset(acc, 0, sizeof(acc));
    static const int bench_order[12] = {0, 1, 2, 3, 4, 5, 5, 4, 3, 2, 1, 0};
    int bench_block = 0;
    int bench_mode = bench_order[0];
    unsigned bench_frame = 0u;
    unsigned bench_warmup = 0u;
    const unsigned warmup_per_block = 30u;
    unsigned measured_per_block = frames_per_preset / 2u;
    bool bench_off = true;
    if (measured_per_block == 0u) measured_per_block = 1u;

    if (benchmark) {
        csv = fopen(csv_path, "w");
        if (csv == NULL) {
            fprintf(stderr, "could not open benchmark CSV: %s\n", csv_path);
            CloseWindow();
            return 2;
        }
        fprintf(csv,
            "preset,frames,frame_avg_ms,frame_median_ms,frame_p95_ms,frame_p99_ms,"
            "compose_avg_us,compose_p95_us,render_avg_us,render_p95_us,"
            "contributions_avg,objects_avg,hash_mismatches\n");
        bench_mode = bench_order[bench_block];
        bench_off = bench_mode == 0;
        state.preset = bench_mode_preset(bench_mode);
    }

    while (!WindowShouldClose()) {
        if (!benchmark) {
            if (IsKeyPressed(KEY_F1)) state.preset = NF17D_VIEW_PLAY;
            if (IsKeyPressed(KEY_F2)) state.preset = NF17D_VIEW_WORLD;
            if (IsKeyPressed(KEY_F3)) state.preset = NF17D_VIEW_ACTOR;
            if (IsKeyPressed(KEY_F4)) state.preset = NF17D_VIEW_CAUSAL;
            if (IsKeyPressed(KEY_F5)) state.preset = NF17D_VIEW_FULL;

            const float wheel = GetMouseWheelMove();
            if (wheel != 0.0f) {
                Vector3 dir = Vector3Subtract(camera.position, camera.target);
                float length = Vector3Length(dir);
                length = fmaxf(5.0f, fminf(35.0f, length - wheel));
                dir = Vector3Scale(Vector3Normalize(dir), length);
                camera.position = Vector3Add(camera.target, dir);
            }
            if (IsKeyDown(KEY_LEFT)) {
                camera.position.x -= 0.10f;
                camera.target.x -= 0.10f;
            }
            if (IsKeyDown(KEY_RIGHT)) {
                camera.position.x += 0.10f;
                camera.target.x += 0.10f;
            }
            if (IsKeyDown(KEY_UP)) {
                camera.position.z += 0.10f;
                camera.target.z += 0.10f;
            }
            if (IsKeyDown(KEY_DOWN)) {
                camera.position.z -= 0.10f;
                camera.target.z -= 0.10f;
            }
            state.selected_cell = pick_cell(&state, camera);
        }

        const uint32_t before_hash = nf17b_state_hash(&state.authority);
        const double frame_begin = GetTime();

        const double compose_begin = GetTime();
        if (!benchmark || !bench_off) {
            build_observability(&state);
        } else {
            state.contribution_count = 0u;
            state.object_count = 0u;
            state.state_hash = nf17b_state_hash(&state.authority);
        }
        const double compose_end = GetTime();

        BeginDrawing();
        ClearBackground((Color){16, 18, 23, 255});
        BeginMode3D(camera);
        draw_world_base(&state);
        draw_overlays(&state);
        if (state.selected_cell >= 0) {
            Vector3 p = cell_center(&state.world, state.selected_cell, 0.03f);
            DrawCubeWires((Vector3){p.x, 0.22f, p.z}, 0.96f, 0.44f, 0.96f, YELLOW);
        }
        EndMode3D();

        DrawText("nightfall!punk v1.7E — GRAPHICAL OBSERVABILITY", 18, 14, 21, RAYWHITE);
        DrawText("F1 PLAY  F2 WORLD  F3 ACTOR  F4 CAUSAL  F5 FULL | arrows pan | wheel zoom | LMB select",
                 18, 42, 14, GRAY);
        DrawText(TextFormat("preset %s | contributions %zu | multidimensional objects %zu | authority %08X",
                            nf17d_view_preset_name(state.preset), state.contribution_count,
                            state.object_count, state.state_hash),
                 18, 64, 15, SKYBLUE);
        DrawText("priority suppression is visual only; click an object to inspect retained dimensions",
                 18, height - 28, 14, GRAY);
        draw_inspector(&state, width, height);
        EndDrawing();

        const double render_end = GetTime();
        const uint32_t after_hash = nf17b_state_hash(&state.authority);

        if (benchmark) {
            const double frame_end = GetTime();
            if (bench_warmup < warmup_per_block) {
                ++bench_warmup;
            } else {
                BenchAccumulator *a = &acc[bench_mode];
                if (a->count < NF17E_MAX_FRAME_SAMPLES) {
                    a->frame_ms[a->count] = (frame_end - frame_begin) * 1000.0;
                    a->compose_us[a->count] = (compose_end - compose_begin) * 1000000.0;
                    a->render_us[a->count] = (render_end - compose_end) * 1000000.0;
                    ++a->count;
                }
                a->contribution_sum += state.contribution_count;
                a->object_sum += state.object_count;
                if (before_hash != after_hash || before_hash != state.state_hash) ++a->hash_mismatches;

                ++bench_frame;
                if (bench_frame >= measured_per_block) {
                    bench_frame = 0u;
                    bench_warmup = 0u;
                    ++bench_block;
                    if (bench_block >= 12) {
                        for (int mode = 0; mode < 6; ++mode) {
                            bench_write_row(csv, bench_mode_name(mode), &acc[mode]);
                        }
                        fflush(csv);
                        break;
                    }
                    bench_mode = bench_order[bench_block];
                    bench_off = bench_mode == 0;
                    state.preset = bench_mode_preset(bench_mode);
                }
            }
        }
    }

    if (csv != NULL) {
        fclose(csv);
        printf("nightfall v1.7E graphical benchmark -> %s\n", csv_path);
    }
    CloseWindow();
    return 0;
}