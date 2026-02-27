#include <stdlib.h>

#include "DtAllocators.h"
#include "DtEcs.h"

static int cmp_updaters(const void* s1, const void* s2) {
    return ((UpdateSystem*) s1)->priority - ((UpdateSystem*) s2)->priority;
}

static int cmp_drawers(const void* s1, const void* s2) {
    return ((DrawSystem*) s1)->priority - ((DrawSystem*) s2)->priority;
}

UpdateHandler* dt_update_handler_new(DtEcsManager* manager, u16 updater_count) {
    UpdateHandler* system_handler = DT_MALLOC(sizeof(UpdateHandler));

    *system_handler = (UpdateHandler) {
        .manager = manager,
        .systems = DT_VEC_NEW(UpdateSystem*, updater_count),
    };

    return system_handler;
}

void dt_update_handler_add(UpdateHandler* handler, UpdateSystem* system) {
    DT_VEC_ADD(handler->systems, system);
}

void dt_update_handler_init(const UpdateHandler* handler) {
    qsort(handler->systems, dt_vec_count(handler->systems), sizeof(UpdateSystem), cmp_updaters);

    FOREACH(UpdateSystem*, system, DT_VEC_ITERATOR(handler->systems),
            { system->init(handler->manager, system->data); });
}

void dt_update_handler_update(const UpdateHandler* handler, DtUpdateContext* ctx) {
    FOREACH(UpdateSystem*, system, DT_VEC_ITERATOR(handler->systems),
            { system->update(system->data, ctx); });
}

void dt_update_handler_destroy(const UpdateHandler* handler) {
    FOREACH(UpdateSystem*, system, DT_VEC_ITERATOR(handler->systems),
            { system->destroy(system->data); });
}

void dt_update_handler_free(UpdateHandler* handler) {
    dt_vec_free(handler->systems);
    free(handler);
}

DrawHandler* dt_draw_handler_new(DtEcsManager* manager, u16 drawers_count) {
    DrawHandler* handler = DT_MALLOC(sizeof(DrawHandler));

    *handler = (DrawHandler) {
        .manager = manager,
        .systems = DT_VEC_NEW(DrawSystem*, drawers_count),
    };

    return handler;
}

void dt_draw_handler_add(DrawHandler* handler, const DrawSystem* system) {
    DT_VEC_ADD(handler->systems, system);
}

void dt_draw_handler_init(const DrawHandler* handler) {
    qsort(handler->systems, dt_vec_count(handler->systems), sizeof(DrawSystem*), cmp_drawers);
    FOREACH(DrawSystem*, system, DT_VEC_ITERATOR(handler->systems),
            { system->init(handler->manager, system->data); });
}

void dt_draw_handler_draw(const DrawHandler* handler) {
    FOREACH(DrawSystem*, system, DT_VEC_ITERATOR(handler->systems),
            { system->draw(system->data); });
}

void dt_draw_handler_destroy(const DrawHandler* handler) {
    FOREACH(DrawSystem*, system, DT_VEC_ITERATOR(handler->systems),
            { system->destroy(system->data); });
}

void dt_draw_handler_free(DrawHandler* handler) {
    dt_vec_free(handler->systems);
    free(handler);
}
