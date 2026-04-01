#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <nuklear.h>
#include "raylib-nuklear.h"

#include <stdbool.h>
#include "../GameLib.h"
#include "DtAllocators.h"
#include "Ecs/RegisterHandler.h"
#include "UI.h"
#include "scheduler/RuntimeScheduler.h"

DtEntity selected_entity = 0;

static void draw_entity_node(DtEntity e);

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

    if (nk_begin(nk_ctx, "Hierarchy", nk_rect(0, height / 30, width / 6, height - height / 4),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(nk_ctx, 20, 1);

        for (int i = 0; i < game_scene->manager->entities_ptr; i++) {
            if (game_scene->manager->sparse_entities[i].alive &&
                dt_ecs_manager_get_parent(game_scene->manager, i).id == DT_ENTITY_NULL) {
                draw_entity_node(i);
            }
        }
    }
    nk_end(nk_ctx);
}

static void draw_entity_node(DtEntity e) {
    DtEntityInfo info = dt_ecs_manager_get_entity(game_scene->manager, e);
    enum nk_tree_type type = info.children_count > 0 ? NK_TREE_NODE : NK_TREE_TAB;

    char label[32];
    snprintf(label, sizeof(label), "%d", e);

    int is_selected = selected_entity == e;
    int old_selected = is_selected;

    if (nk_tree_element_push_id(nk_ctx, type, label, NK_MINIMIZED, &is_selected, e)) {

        if (is_selected != old_selected) {
            selected_entity = e;
        }

        DtEntity count;
        const DtEntity* children = dt_ecs_manager_get_children(game_scene->manager, e, &count);
        for (int i = 0; i < count; i++) {
            draw_entity_node(children[i]);
        }

        nk_tree_pop(nk_ctx);
    } else {
        if (is_selected != old_selected) {
            selected_entity = e;
        }
    }
}
