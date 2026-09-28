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