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
    reload_game_lib();
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
    if (nk_begin(ctx, "Components", nk_rect(50, 50, 230, 250),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE |
                     NK_WINDOW_TITLE)) {
        nk_layout_row_static(ctx, 50, 150, 1);

        if (nk_button_label(ctx, "rebuild")) {

            reload_game_lib();
        }
        nk_layout_row_static(ctx, 50, 150, 1);

        for (int i = 0; i < components_count; i++) {
            if (nk_button_label(ctx, components[i]->name)) {
                printf("\n%s's fields:\n", components[i]->name);
                for (int j = 0; j < components[i]->field_count; j++) {
                    printf("\t%s %s\n", components[i]->field_types[j], components[i]->field_names[j]);
                }
            }
        }
    }
    nk_end(ctx);
}
