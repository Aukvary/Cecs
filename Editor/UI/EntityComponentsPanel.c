#include <scheduler/RuntimeScheduler.h>
#include "DtAllocators.h"
#include "EditorApi.h"
#include "UI.h"

extern DtEntity selected_entity;

static DrawSystem* component_panel_new();
static void component_panel_draw(void* _);

DT_REGISTER_DRAW(ComponentPanelDraw, component_panel_new)

static DrawSystem* component_panel_new() {
    DrawSystem* system = DT_MALLOC(sizeof(DrawSystem));
    *system = (DrawSystem) {
        .data = NULL,
        .init = NULL,
        .draw = component_panel_draw,
        .destroy = NULL,
    };

    return system;
}

static void component_panel_draw(void* _) {
    float width = (float) GetScreenWidth();
    float height = (float) GetScreenHeight();

    if (selected_entity == DT_ENTITY_NULL)
        return;

    char title[64];
    snprintf(title, sizeof(title), "Components (ID: %u)", selected_entity);

    if (nk_begin(ctx, title, nk_rect(width - width / 5, height / 30, width / 5, height),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_TITLE)) {

        DtEntityInfo info = dt_ecs_manager_get_entity(game_scene->manager, selected_entity);

        for (int i = 0; i < info.component_count; i++) {
            const char* comp_name = game_scene->manager->pools[info.components[i]]->name;
            const DtComponentData* component_data = dt_component_get_data_by_name(comp_name);

            if (component_data == NULL)
                continue;

            DtEcsPool* pool = dt_ecs_manager_get_pool(game_scene->manager, comp_name);
            void* component_ptr = dt_ecs_pool_get(pool, selected_entity);

            if (component_ptr == NULL)
                continue;

            if (nk_tree_push_id(ctx, NK_TREE_TAB, comp_name, NK_MAXIMIZED, i)) {
                for (int j = 0; j < component_data->field_count; j++) {
                    u8* field_addr = (u8*) component_ptr + component_data->field_offsets[j];

                    dte_inspector_field_draw(component_data->field_types[j],
                                                   component_data->field_names[j], field_addr);
                }

                nk_layout_row_dynamic(ctx, 25, 1);
                if (nk_button_label(ctx, "Remove Component")) {
                    dt_ecs_manager_entity_remove_component(game_scene->manager, selected_entity,
                                                           comp_name);
                }

                nk_tree_pop(ctx);
            }
        }
    }
    nk_end(ctx);
}
