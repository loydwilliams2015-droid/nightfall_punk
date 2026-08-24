#include "nf_spatial_review.h"

#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Selection {
    NfSpatialLabKind lab;
    char lab_name[32];
    char category[32];
    uint32_t seed;
} Selection;

static NfSpatialLabKind parse_lab(const char *name) {
    for (int lab = 0; lab < NF_SPATIAL_LAB_COUNT; ++lab) {
        if (strcmp(name, nf_spatial_lab_name((NfSpatialLabKind)lab)) == 0) {
            return (NfSpatialLabKind)lab;
        }
    }
    return NF_SPATIAL_LAB_COUNT;
}

static int load_selections(const char *path, Selection *items, int max_items) {
    FILE *in = fopen(path, "r");
    if (in == NULL) return -1;
    char line[512];
    if (fgets(line, sizeof(line), in) == NULL) {
        fclose(in);
        return -1;
    }
    int count = 0;
    while (count < max_items && fgets(line, sizeof(line), in) != NULL) {
        char lab_name[32];
        char category[32];
        unsigned seed = 0u;
        double selection_score = 0.0;
        if (sscanf(line, "%31[^,],%31[^,],%u,%lf", lab_name, category, &seed, &selection_score) != 4) {
            fclose(in);
            return -1;
        }
        (void)selection_score;
        NfSpatialLabKind lab = parse_lab(lab_name);
        if (lab == NF_SPATIAL_LAB_COUNT) {
            fclose(in);
            return -1;
        }
        items[count].lab = lab;
        snprintf(items[count].lab_name, sizeof(items[count].lab_name), "%s", lab_name);
        snprintf(items[count].category, sizeof(items[count].category), "%s", category);
        items[count].seed = (uint32_t)seed;
        ++count;
    }
    fclose(in);
    return count;
}

static Color cell_color(const NfSpatialCell *cell) {
    if (cell->solid != 0u) return (Color){36, 39, 45, 255};
    int shade = 244 - (int)(112.0f * cell->exposure);
    if (shade < 112) shade = 112;
    return (Color){(unsigned char)shade, (unsigned char)shade, (unsigned char)shade, 255};
}

static void draw_path(
    const NfSpatialReviewTrace *trace,
    int origin_x,
    int origin_y,
    int cell_px,
    Color color) {
    const NfSpatialWorld *world = &trace->world_final;
    for (uint16_t i = 1u; i < trace->path_len; ++i) {
        int a = trace->path[i - 1u];
        int b = trace->path[i];
        float ax = (float)(origin_x + (a % world->width) * cell_px + cell_px / 2);
        float ay = (float)(origin_y + (a / world->width) * cell_px + cell_px / 2);
        float bx = (float)(origin_x + (b % world->width) * cell_px + cell_px / 2);
        float by = (float)(origin_y + (b / world->width) * cell_px + cell_px / 2);
        DrawLineEx((Vector2){ax, ay}, (Vector2){bx, by}, 3.0f, color);
    }
}

static void draw_world_panel(
    const NfSpatialReviewTrace *trace,
    int origin_x,
    int origin_y,
    int cell_px,
    Color path_color,
    const char *mode_label) {
    const NfSpatialWorld *world = &trace->world_final;
    const NfSpatialReviewResult *r = &trace->result;
    DrawText(mode_label, origin_x, origin_y - 28, 20, path_color);

    for (int y = 0; y < world->height; ++y) {
        for (int x = 0; x < world->width; ++x) {
            int cell = y * world->width + x;
            const NfSpatialCell *c = &world->cells[cell];
            int px = origin_x + x * cell_px;
            int py = origin_y + y * cell_px;
            DrawRectangle(px, py, cell_px - 1, cell_px - 1, cell_color(c));
            if (c->solid == 0u) {
                if (c->ecological_risk > 0.62f) {
                    DrawCircle(px + 6, py + 6, 3.0f, (Color){174, 86, 58, 255});
                }
                if (c->affordance > 0.35f && c->authored_forbid_affordance == 0u) {
                    DrawRectangleLines(px + 2, py + 2, cell_px - 5, cell_px - 5, (Color){64, 121, 180, 255});
                }
                if (c->contract_memory > 0.10f) {
                    int h = (int)((float)(cell_px - 4) * c->contract_memory);
                    DrawRectangle(px + cell_px - 5, py + cell_px - 2 - h, 3, h, (Color){132, 82, 156, 255});
                }
                if (trace->known_final[cell] == 0u) {
                    DrawRectangle(px, py, cell_px - 1, cell_px - 1, (Color){20, 24, 30, 82});
                }
            }
        }
    }

    draw_path(trace, origin_x, origin_y, cell_px, path_color);

    int start = world->start_cell;
    int goal = world->goal_cell;
    DrawCircle(
        origin_x + (start % world->width) * cell_px + cell_px / 2,
        origin_y + (start / world->width) * cell_px + cell_px / 2,
        6.0f,
        (Color){48, 150, 82, 255});
    DrawCircle(
        origin_x + (goal % world->width) * cell_px + cell_px / 2,
        origin_y + (goal / world->width) * cell_px + cell_px / 2,
        6.0f,
        (Color){215, 169, 64, 255});

    char line[256];
    snprintf(line, sizeof(line), "success %u  steps %u  health %.1f", r->success, r->steps, r->health_remaining);
    DrawText(line, origin_x, origin_y + world->height * cell_px + 10, 16, RAYWHITE);
    snprintf(line, sizeof(line), "exposure %.2f  ecology %.2f  revisits %u", r->exposure_accum, r->ecology_cost_accum, r->revisits);
    DrawText(line, origin_x, origin_y + world->height * cell_px + 31, 16, RAYWHITE);
    snprintf(line, sizeof(line), "discover %u  cue-disagree %u  invalid %u", r->route_discovery_step, r->cue_disagreement_steps, r->path_invalid_steps);
    DrawText(line, origin_x, origin_y + world->height * cell_px + 52, 16, RAYWHITE);
}

static void draw_comparison(const Selection *selection, int index) {
    NfSpatialReviewTrace instrumented;
    NfSpatialReviewTrace perceptual;
    (void)nf_spatial_review_run(selection->lab, NF_SPATIAL_REVIEW_INSTRUMENTED, selection->seed, 0u, &instrumented);
    (void)nf_spatial_review_run(selection->lab, NF_SPATIAL_REVIEW_PERCEPTUAL, selection->seed, 0u, &perceptual);

    BeginDrawing();
    ClearBackground((Color){18, 21, 26, 255});
    char title[256];
    snprintf(title, sizeof(title), "v1.7A referee replay  %02d/40  %s / %s  seed %u", index + 1, selection->lab_name, selection->category, selection->seed);
    DrawText(title, 36, 22, 24, RAYWHITE);
    DrawText("Referee truth shown after play: grey=exposure, red dot=ecological risk, blue box=affordance, purple bar=contract memory; dark veil=unobserved", 36, 54, 15, (Color){190, 195, 205, 255});

    draw_world_panel(&instrumented, 74, 116, 32, (Color){83, 154, 225, 255}, "INSTRUMENTED — bounded exact observed values");
    draw_world_panel(&perceptual, 716, 116, 32, (Color){214, 107, 181, 255}, "PERCEPTUAL — quantized player-legible cues");

    DrawText("start", 44, 650, 14, (Color){48, 150, 82, 255});
    DrawText("goal", 104, 650, 14, (Color){215, 169, 64, 255});
    DrawText("AI never receives this referee overlay during the run.", 716, 650, 16, (Color){190, 195, 205, 255});
    EndDrawing();
}

int main(int argc, char **argv) {
    if (argc != 4 || strcmp(argv[1], "--batch") != 0) {
        fprintf(stderr, "usage: %s --batch selections.csv frames_dir\n", argv[0]);
        return 2;
    }

    Selection selections[64];
    int count = load_selections(argv[2], selections, (int)(sizeof(selections) / sizeof(selections[0])));
    if (count != 40) {
        fprintf(stderr, "expected 40 selected seeds, got %d\n", count);
        return 2;
    }

    if (!DirectoryExists(argv[3]) && !MakeDirectory(argv[3])) {
        fprintf(stderr, "could not create frames directory: %s\n", argv[3]);
        return 2;
    }
    if (!ChangeDirectory(argv[3])) {
        fprintf(stderr, "could not enter frames directory: %s\n", argv[3]);
        return 2;
    }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "nightfall!punk v1.7A — Spatial Logic Referee Viewer");
    SetTargetFPS(60);

    for (int i = 0; i < count; ++i) {
        draw_comparison(&selections[i], i);
        char filename[192];
        snprintf(filename, sizeof(filename), "v17a_%02d_%s_%s_%u.png", i + 1, selections[i].lab_name, selections[i].category, selections[i].seed);
        TakeScreenshot(filename);
    }

    CloseWindow();
    printf("nightfall v1.7A graphical referee viewer: 40 comparison frames rendered\n");
    return 0;
}
