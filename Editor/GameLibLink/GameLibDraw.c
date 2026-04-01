#include "DtAllocators.h"
#include "EditorApi.h"
#include "GameLib.h"
#include "UI.h"

static DrawHandler* handler = NULL;

DrawSystem* game_lib_draw_new();
void game_lib_draw_init(DtEcsManager* manager, void* data);
void game_lib_draw(void* data);
void game_lib_draw_destroy(void* data);

DT_REGISTER_DRAW(GameLibDraw, game_lib_draw_new)

DrawSystem* game_lib_draw_new() {
    DrawSystem* sys = DT_MALLOC(sizeof(DrawSystem));

    *sys = (DrawSystem) {
        .data = NULL,

        .init = game_lib_draw_init,
        .draw = game_lib_draw,
        .destroy = game_lib_draw_destroy,

        .priority = -1,
    };

    handler = dt_draw_handler_new(game_scene->manager,
                                    dt_vec_count(game_scene->draw_handler->systems));

    return sys;
}

void game_lib_draw_init(DtEcsManager* manager, void* data) {
    FOREACH(char*, name, DT_VEC_ITERATOR(game_scene->draw_handler->names), {
        const DtDrawData* draw = dt_draw_get_data_by_name(name);

        for (int i = 0; i < draw->attribute_count; i++) {
            if (strcmp(DTE_EDIT_DRAW_NAME, draw->name) != 0)
                continue;

            dt_draw_handler_add(handler, ((DrawSystem*(*)())draw->attributes->data)(), draw->name);
        }
    });

    dt_draw_handler_init(handler);
}

void game_lib_draw(void* data) {
    dt_draw_handler_draw(handler);
}

void game_lib_draw_destroy(void* data) {
    dt_draw_handler_destroy(handler);
}

void load_draw_game_systems() {
    if (handler) {
        dt_draw_handler_free(handler);
    }

    FOREACH(char*, name, DT_VEC_ITERATOR(game_scene->draw_handler->names), {
        const DtDrawData* draw = dt_draw_get_data_by_name(name);

        for (int i = 0; i < draw->attribute_count; i++) {
            if (strcmp(DTE_EDIT_DRAW_NAME, draw->name) != 0)
                continue;

            dt_draw_handler_add(handler, ((DrawSystem*(*)())draw->attributes->data)(), draw->name);
        }
    });

    dt_draw_handler_init(handler);
}