#include <stdlib.h>

#include "DtAllocators.h"
#include "DtEcs.h"

static int cmp_systems(void* s1, void* s2) {
    ((EcsSystem*) s1)->priority - ((EcsSystem*) s2)->priority;
}

static int cmp_drawrs(const void* s1, const void* s2) {
    ((DrawSystem*) s1)->priority - ((DrawSystem*) s2)->priority;
}

EcsSystem ecs_system_new(Init init, Action update, Action destroy, i16 priority) {
    return (EcsSystem) {.init = init, .update = update, .destroy = destroy, .priority = priority};
}

SystemHandler* dt_system_handler_new(DtEcsManager* manager, u16 updater_count) {
    SystemHandler* system_handler = DT_MALLOC(sizeof(SystemHandler));

    *system_handler = (SystemHandler) {
        .manager = manager,
        .ecs_systems = DT_VEC_NEW(EcsSystem, updater_count),
    };

    return system_handler;
}

inline void dt_system_handler_add(SystemHandler* handler, const EcsSystem system) {
    DT_VEC_ADD(handler->ecs_systems, system);
}

inline void dt_system_handler_init(const SystemHandler* handler) {
    qsort(handler->ecs_systems, dt_vec_count(handler->ecs_systems), sizeof(EcsSystem), cmp_drawrs);

    FOREACH(EcsSystem, system, DT_VEC_ITERATOR(handler->ecs_systems),
            { system.init(handler->manager, system.data); });
}

inline void dt_system_handler_update(const SystemHandler* handler) {
    FOREACH(EcsSystem, system, DT_VEC_ITERATOR(handler->ecs_systems),
            { system.update(system.data); });
}

inline void dt_system_handler_destroy(const SystemHandler* handler) {
    FOREACH(EcsSystem, system, DT_VEC_ITERATOR(handler->ecs_systems),
            { system.destroy(system.data); });
}

inline void dt_system_handler_free(SystemHandler* handler) { free(handler); }

DrawHandler* dt_draw_handler_new(DtEcsManager* manager, const DrawHandlerConfig* cfg) {
    DrawHandler* handler = DT_MALLOC(sizeof(DrawHandler));

    *handler = (DrawHandler) {
        .manager = manager,
        .begin_draw = cfg->begin_draw,
        .end_draw = cfg->end_draw,
        .systems = DT_VEC_NEW(DrawSystem, cfg->drawers_count),
    };

    return handler;
}

void dt_draw_handler_add(DrawHandler* handler, const DrawSystem* system) {}

void dt_draw_handler_init(const DrawHandler* handler) {}

void dt_draw_handler_draw(const DrawHandler* handler) {}

void dt_draw_handler_destroy(const DrawHandler* handler) {}
