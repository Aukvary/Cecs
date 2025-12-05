#include "systems.h"
#include <stdlib.h>

EcsSystem* ecs_system_new(void (*init)(const EcsManager*, void*), const Action pre_update,
                          const Action update, const Action post_update,
                          const Action destroy) {
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

SystemHandler* system_handler_new(const EcsManager* manager,
                                  const SystemHandlerConfig* cfg) {
    SystemHandler* system_handler = malloc(sizeof(SystemHandler));

    *system_handler = (SystemHandler) {
        .manager = manager,

        .inits = VEC_NEW(InitDataPair, cfg->init_size),

        .pre_updates = VEC_NEW(ActionDataPair, cfg->pre_update_size),
        .updates = VEC_NEW(ActionDataPair, cfg->update_size),
        .post_updates = VEC_NEW(ActionDataPair, cfg->post_update_size),

        .destroys = VEC_NEW(ActionDataPair, cfg->destroy_size),
    };

    return system_handler;
}

inline void system_handler_add(SystemHandler* handler, EcsSystem* system) {
    if (system->init)
        VEC_ADD(handler->inits, ((InitDataPair) {
                                     .data = system->data,
                                     .init = system->init,
                                 }));

    if (system->pre_update)
        VEC_ADD(handler->pre_updates, ((ActionDataPair) {
                                           .data = system->data,
                                           .action = system->pre_update,
                                       }));

    if (system->update)
        VEC_ADD(handler->updates, ((ActionDataPair) {
                                       .data = system->data,
                                       .action = system->update,
                                   }));

    if (system->post_update)
        VEC_ADD(handler->post_updates, ((ActionDataPair) {
                                            .data = system->data,
                                            .action = system->post_update,
                                        }));

    if (system->destroy)
        VEC_ADD(handler->destroys, ((ActionDataPair) {
                                        .data = system->data,
                                        .action = system->destroy,
                                    }));
}

inline void system_handler_init(const SystemHandler* handler) {
    FOREACH(InitDataPair, p, *VEC_ITERATOR(handler->inits),
            { p.init(handler->manager, p.data); });
}

inline void system_handler_pre_update(const SystemHandler* handler) {
    FOREACH(ActionDataPair, p, *VEC_ITERATOR(handler->pre_updates),
            { p.action(p.data); });
}

inline void system_handler_update(const SystemHandler* handler) {
    FOREACH(ActionDataPair, p, *VEC_ITERATOR(handler->updates), { p.action(p.data); });
}

inline void system_handler_post_update(const SystemHandler* handler) {
    FOREACH(ActionDataPair, p, *VEC_ITERATOR(handler->post_updates),
            { p.action(p.data); });
}

inline void system_handler_destroy(const SystemHandler* handler) {
    FOREACH(ActionDataPair, p, *VEC_ITERATOR(handler->destroys), { p.action(p.data); });
}

inline void system_handler_free(SystemHandler* handler) {
    free(handler);
}
