#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include <cjson/cJSON.h>
#include "Tests/TestEcs.h"

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

int main() {
    test_filter();
    // test_parent_children_relations();
    // test_create_remove_entity();

    return 0;
}
