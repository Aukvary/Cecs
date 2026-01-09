#include "DtEcs.h"
#include <stdlib.h>

EcsSystem* ecs_system_new(void (*init)(DtEcsManager*, void*), const Action pre_update,
                          const Action update, const Action post_update,
                          const Action tag_remove, const Action destroy) {
    EcsSystem* system = malloc(sizeof(EcsSystem));

    *system = (EcsSystem) {
        .init = init,
        .pre_update = pre_update,
        .update = update,
        .post_update = post_update,
        .tag_remove = tag_remove,
        .destroy = destroy,
    };

    return system;
}

SystemHandler* system_handler_new(DtEcsManager* manager, const SystemHandlerConfig* cfg) {
    SystemHandler* system_handler = malloc(sizeof(SystemHandler));

    *system_handler = (SystemHandler) {
        .manager = manager,

        .inits = DT_VEC_NEW(InitDataPair, cfg->init_size),

        .pre_updates = DT_VEC_NEW(ActionDataPair, cfg->pre_update_size),
        .updates = DT_VEC_NEW(ActionDataPair, cfg->update_size),
        .post_updates = DT_VEC_NEW(ActionDataPair, cfg->post_update_size),

        .tag_removes = DT_VEC_NEW(ActionDataPair, cfg->tag_remove_size),

        .destroys = DT_VEC_NEW(ActionDataPair, cfg->destroy_size),
    };

    return system_handler;
}

inline void system_handler_add(SystemHandler* handler, const EcsSystem* system) {
    if (system->init)
        DT_VEC_ADD(handler->inits, ((InitDataPair) {
                                    .data = system->data,
                                    .init = system->init,
                                }));

    if (system->pre_update)
        DT_VEC_ADD(handler->pre_updates, ((ActionDataPair) {
                                          .data = system->data,
                                          .action = system->pre_update,
                                      }));

    if (system->update)
        DT_VEC_ADD(handler->updates, ((ActionDataPair) {
                                      .data = system->data,
                                      .action = system->update,
                                  }));

    if (system->post_update)
        DT_VEC_ADD(handler->post_updates, ((ActionDataPair) {
                                           .data = system->data,
                                           .action = system->post_update,
                                       }));

    if (system->tag_remove)
        DT_VEC_ADD(handler->tag_removes, ((ActionDataPair) {
                                          .data = system->data,
                                          .action = system->tag_remove,
                                      }));

    if (system->destroy)
        DT_VEC_ADD(handler->destroys, ((ActionDataPair) {
                                       .data = system->data,
                                       .action = system->destroy,
                                   }));
}

inline void system_handler_init(const SystemHandler* handler) {
    FOREACH(InitDataPair, p, DT_VEC_ITERATOR(handler->inits),
            { p.init(handler->manager, p.data); });
}

inline void system_handler_pre_update(const SystemHandler* handler) {
    FOREACH(ActionDataPair, p, DT_VEC_ITERATOR(handler->pre_updates), { p.action(p.data); });
}

inline void system_handler_update(const SystemHandler* handler) {
    FOREACH(ActionDataPair, p, DT_VEC_ITERATOR(handler->updates), { p.action(p.data); });
}

inline void system_handler_post_update(const SystemHandler* handler) {
    FOREACH(ActionDataPair, p, DT_VEC_ITERATOR(handler->post_updates),
            { p.action(p.data); });
}

inline void system_handler_remove_tag(const SystemHandler* handler) {
    FOREACH(ActionDataPair, p, DT_VEC_ITERATOR(handler->tag_removes), { p.action(p.data); });
}

inline void system_handler_destroy(const SystemHandler* handler) {
    FOREACH(ActionDataPair, p, DT_VEC_ITERATOR(handler->destroys), { p.action(p.data); });
}

inline void system_handler_free(SystemHandler* handler) { free(handler); }
