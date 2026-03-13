#include "Ecs/DtEcs.h"
#include "raylib-nuklear.h"
#include "raylib.h"
#include "scheduler/RuntimeScheduler.h"

#define MAIN_SCENE_PATH "./editor_main_scene.dt.scene"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
struct nk_context* ctx;
struct nk_colorf bg;

const DtScene* scene;

static void initialize_main_scene();
static void initialize_window();

static void deinitialize_ecs_manager();
static void deinitialize_window();

int main(void) {
    initialize_main_scene();
    initialize_window();

    while (!WindowShouldClose()) {

    }
    UnloadNuklear(ctx);
    CloseWindow();

    return 0;
}

static void initialize_main_scene() {

}

static void initialize_window() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dt Editor");
    SetTargetFPS(60);

    const int fontSize = 18;
    Font font = LoadFontFromNuklear(fontSize);
    ctx = InitNuklearEx(font, fontSize);
    bg = (struct nk_colorf) {
        0.1f,
        0.18f,
        0.24f,
        1.0f,
    };
}
