#include "DtAllocators.h"
#include "GameLibLink/GameLib.h"
#include "UI.h"
#include "scheduler/RuntimeScheduler.h"
static DrawSystem* manager_panel_new();
static void manage_panel_draw(void* data);

DT_REGISTER_DRAW(ManagerPanel, manager_panel_new)

static DrawSystem* manager_panel_new() {
    DrawSystem* system = DT_MALLOC(sizeof(DrawSystem));
    *system = (DrawSystem) {
        .data = NULL,
        .init = NULL,
        .draw = manage_panel_draw,
        .destroy = NULL,
    };

    return system;
}

static void manage_panel_draw(void* data) {
    float width = (float) GetScreenWidth();
    float panel_height = 30;

    if (nk_begin(ctx, "MainMenuBar", nk_rect(0, 0, width, panel_height),
                 NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND | NK_WINDOW_BORDER)) {

        nk_menubar_begin(ctx);

        nk_layout_row_template_begin(ctx, 25);
        nk_layout_row_template_push_static(ctx, 45);
        nk_layout_row_template_push_static(ctx, 80);
        nk_layout_row_template_end(ctx);

        if (nk_menu_begin_label(ctx, "File", NK_TEXT_LEFT, nk_vec2(120, 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_menu_item_label(ctx, "New Scene", NK_TEXT_LEFT)) {
            }
            if (nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT)) {
                save_game_scene(game_scene, "./game_scene.dt.scene");
            }
            nk_menu_end(ctx);
        }

        if (nk_menu_begin_label(ctx, "Game Lib", NK_TEXT_LEFT, nk_vec2(150, 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_menu_item_label(ctx, "Rebuild Lib", NK_TEXT_LEFT)) {
                save_game_scene(game_scene, "./game_scene.dt.scene");
                reload_game_lib(true);
                dt_scene_unload_by(game_scene);
                game_scene = dt_add_scene("./game_scene.dt.scene");
            }
            if (nk_menu_item_label(ctx, "Reload Lib", NK_TEXT_LEFT)) {
                save_game_scene(game_scene, "./game_scene.dt.scene");
                reload_game_lib(false);
                dt_scene_unload_by(game_scene);
                game_scene = dt_add_scene("./game_scene.dt.scene");
            }
            nk_menu_end(ctx);
        }

        nk_menubar_end(ctx);
    }
    nk_end(ctx);
}
