#include <stdlib.h>


#include "systems.h"

EcsSystem* ecs_system_new(void (*init)(const EcsManager*, void*),
                          const Action pre_update, const Action update,
                          const Action post_update, const Action destroy) {
    EcsSystem* system = malloc(sizeof(EcsSystem));

    *system = (EcsSystem) {
            .init = init,
            .pre_update = pre_update,
            .update = update,
            .post_update = post_update,
            .destroy = destroy,
    };

    return system;
}

SystemHandler* system_handler_new(const EcsManager* manager, const size_t size) {
    SystemHandler* system_handler = malloc(sizeof(SystemHandler));

    *system_handler = (SystemHandler) {
            .manager = manager,
            .systems = VEC_NEW(EcsSystem*, size),
    };


    return system_handler;
}

inline void system_handler_add(SystemHandler* handler, EcsSystem* system) {
    VEC_ADD(handler->systems, system);
}

inline void system_handler_init(const SystemHandler* handler) {
    FOREACH(EcsSystem*, s, *VEC_ITERATOR(handler->systems),
            { s->init(handler->manager, s->data); });
}

inline void system_handler_pre_update(const SystemHandler* handler) {
    FOREACH(EcsSystem*, s, *VEC_ITERATOR(handler->systems), { s->pre_update(s->data); });
}

inline void system_handler_update(const SystemHandler* handler) {
    FOREACH(EcsSystem*, s, *VEC_ITERATOR(handler->systems), { s->update(s->data); });
}

inline void system_handler_post_update(const SystemHandler* handler) {
    FOREACH(EcsSystem*, s, *VEC_ITERATOR(handler->systems), { s->post_update(s->data); });
}

inline void system_handler_destroy(const SystemHandler* handler) {
    FOREACH(EcsSystem*, s, *VEC_ITERATOR(handler->systems), { s->destroy(s->data); });
}

void system_handler_free(SystemHandler* handler) {
    FOREACH(EcsSystem*, s, *VEC_ITERATOR(handler->systems), { free(s); });

    free(handler);
}
