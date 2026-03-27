#include <stdbool.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#include "Ecs/RegisterHandler.h"
#include "GameLibLink/GameLib.h"
#include "UI.h"
#include "raylib-nuklear.h"

extern struct nk_context* ctx;
extern const DtComponentData** components;
extern u16 components_count;

static void components_tree_draw();
static void console_draw();

DT_REGISTER_UPDATE(HotKeyReloadSystem, reload_system_new)
UpdateSystem* reload_system_new() {
    UpdateSystem* system = DT_MALLOC(sizeof(UpdateSystem));

    *system = (UpdateSystem) {
        .init = NULL,
        .update = reload_system_update,
        .destroy = NULL,
    };

    return system;
}

void reload_system_update(void* data, DtUpdateContext* update_ctx) {
    if (!IsKeyPressed(KEY_C))
        return;
    printf("call reload game lib");
    reload_game_lib(true);
}

DT_REGISTER_DRAW(HierarchyTreeSystem, hierarchy_tree_new)
DrawSystem* hierarchy_tree_new() {
    DrawSystem* system = DT_MALLOC(sizeof(DrawSystem));
    *system = (DrawSystem) {
        .init = NULL,
        .draw = hierarchy_tree_draw,
        .destroy = NULL,
    };

    return system;
}

void hierarchy_tree_draw(void* data) {
    float width = (float) GetScreenWidth();
    float height = (float) GetScreenHeight();

    if (nk_begin(ctx, "Hierarchy tree", nk_rect(0, 0, width / 6, height - height / 4),
                 NK_WINDOW_BORDER | NK_WINDOW_SCALE_LEFT | NK_WINDOW_TITLE)) {
        nk_layout_row_static(ctx, 30, (int) width / 12, 1);


        if (nk_button_label(ctx, "+")) {
        }
    }
    nk_end(ctx);

    components_tree_draw();
    console_draw();
}

static void components_tree_draw() {
    float width = (float) GetScreenWidth();
    float height = (float) GetScreenHeight();

    if (nk_begin(ctx, "components", nk_rect(width - width / 5, 0, width / 5, height),
                 NK_WINDOW_BORDER | NK_WINDOW_SCALE_LEFT | NK_WINDOW_TITLE)) {
        nk_layout_row_static(ctx, 30, (int) width / 12, 1);


        if (nk_button_label(ctx, "transform")) {
        }
    }
    nk_end(ctx);
}

static void console_draw() {
    float width = (float) GetScreenWidth();
    float height = (float) GetScreenHeight();

    if (nk_begin(ctx, "Terminal", nk_rect(0, height - height / 4, width - width / 5, height / 4),
                 NK_WINDOW_BORDER | NK_WINDOW_SCALE_LEFT | NK_WINDOW_TITLE)) {
        nk_layout_row_static(ctx, 30, (int) width / 12, 1);


        if (nk_button_label(ctx, "terminal")) {
        }
    }
    nk_end(ctx);
}
