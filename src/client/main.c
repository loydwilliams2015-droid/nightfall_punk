#include "nf_world.h"
#include "raylib.h"

int main(void) {
    const int width = 1280;
    const int height = 720;

    InitWindow(width, height, "nightfall!punk v0.1 graybox client");
    SetTargetFPS(144);

    NfWorld predicted_world;
    nf_world_init(&predicted_world, 20260807u);
    nf_world_spawn_actor(&predicted_world, NF_FACTION_PLAYER, (NfVec3){0});

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color){18, 20, 24, 255});
        DrawText("nightfall!punk v0.1", 40, 40, 32, RAYWHITE);
        DrawText("graybox client / presentation shell", 40, 85, 20, GRAY);
        DrawRectangle(width / 2 - 180, height / 2 - 90, 360, 180, (Color){68, 72, 80, 255});
        DrawRectangleLines(width / 2 - 180, height / 2 - 90, 360, 180, LIGHTGRAY);
        DrawText("SERVER TRUTH -> SNAPSHOTS -> CLIENT", width / 2 - 160, height / 2 - 10, 16, RAYWHITE);
        DrawText(TextFormat("local predicted actors: %i", (int)nf_world_active_actor_count(&predicted_world)), 40, height - 60, 18, GREEN);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
