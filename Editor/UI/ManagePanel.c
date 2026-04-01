#include "../GameLib.h"
#include "DtAllocators.h"
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

    if (nk_begin(nk_ctx, "MainMenuBar", nk_rect(0, 0, width, panel_height),
                 NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND | NK_WINDOW_BORDER)) {

        nk_menubar_begin(nk_ctx);

        nk_layout_row_template_begin(nk_ctx, 25);
        nk_layout_row_template_push_static(nk_ctx, 45);
        nk_layout_row_template_push_static(nk_ctx, 80);
        nk_layout_row_template_end(nk_ctx);

        if (nk_menu_begin_label(nk_ctx, "File", NK_TEXT_LEFT, nk_vec2(120, 200))) {
            nk_layout_row_dynamic(nk_ctx, 25, 1);
            if (nk_menu_item_label(nk_ctx, "New Scene", NK_TEXT_LEFT)) {
            }
            if (nk_menu_item_label(nk_ctx, "Save", NK_TEXT_LEFT)) {
                save_game_scene();
            }
            nk_menu_end(nk_ctx);
        }

        if (nk_menu_begin_label(nk_ctx, "Game Lib", NK_TEXT_LEFT, nk_vec2(150, 200))) {
            nk_layout_row_dynamic(nk_ctx, 25, 1);
            if (nk_menu_item_label(nk_ctx, "Rebuild Lib", NK_TEXT_LEFT)) {
                save_game_scene();
                reload_game_lib(true);
                reload_game_scene();
            }
            if (nk_menu_item_label(nk_ctx, "Reload Lib", NK_TEXT_LEFT)) {
                save_game_scene();
                reload_game_lib(false);
                reload_game_scene();
            }
            nk_menu_end(nk_ctx);
        }

        nk_menubar_end(nk_ctx);
    }
    nk_end(nk_ctx);
}
