#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "../ECSManager/EcsManager.h"
#include "../EcsTypes.h"

typedef void (*Action)(void*);

typedef struct System {
    void* data;

    void (*init)(const EcsManager*, void*);

    Action pre_update;
    Action update;
    Action post_update;

    Action destroy;
} EcsSystem;

EcsSystem* ecs_system_new(void (*)(const EcsManager*, void*), Action, Action,
                          Action, Action);

struct SystemHandler {
    const EcsManager* manager;
    VEC(EcsSystem*) systems;
};

SystemHandler* system_handler_new(const EcsManager*, size_t);

inline void system_handler_add(SystemHandler*, EcsSystem*);

inline void system_handler_init(const SystemHandler*);

inline void system_handler_pre_update(const SystemHandler*);

inline void system_handler_update(const SystemHandler*);

inline void system_handler_post_update(const SystemHandler*);

inline void system_handler_destroy(const SystemHandler*);

inline void system_handler_free(SystemHandler*);

#endif
