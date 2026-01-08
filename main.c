#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include "Game/Game.h"
#include "include/raylib.h"

void func() {
    //
    //
    // const int screenWidth = 1200;
    // const int screenHeight = 625;
    //
    // InitWindow(screenWidth, screenHeight, "Double Troubbell");
    // SetWindowState(FLAG_WINDOW_RESIZABLE);
    //
    // SetTargetFPS(60);
    //
    // init_game();
    //
    // while (!WindowShouldClose()) {
    //     pre_update();
    //     pre_draw();
    //
    //     BeginDrawing();
    //     ClearBackground(SKYBLUE);
    //     update();
    //     draw();
    //     EndDrawing();
    //
    //     post_update();
    //     post_draw();
    // }
    //
    // destroy_game();
    // CloseWindow();
}



static const SystemHandlerConfig systems_cfg = {
    .init_size = 2,
    .pre_update_size = 0,
    .update_size = 2,
    .post_update_size = 0,
    .destroy_size = 0,
};

#include "Tests/EcsManagerTests.h"

int main() {
    test_create_remove_entity();

    return 0;
}
