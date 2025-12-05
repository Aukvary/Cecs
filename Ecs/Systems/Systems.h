#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "../EcsManager/EcsManager.h"
#include "../EcsTypes.h"

typedef void (*Action)(void*);
typedef void (*Init)(EcsManager*, void*);

typedef struct {
    void* data;
    Init init;
} InitDataPair;

typedef struct {
    void* data;
    Action action;
} ActionDataPair;

typedef struct {
    size_t init_size;

    size_t pre_update_size;
    size_t update_size;
    size_t post_update_size;

    size_t destroy_size;
} SystemHandlerConfig;

typedef struct EcsSystem {
    void* data;

    Init init;

    Action pre_update;
    Action update;
    Action post_update;

    Action destroy;
} EcsSystem;

EcsSystem* ecs_system_new(Init init, Action pre_update,
                          Action update, Action post_update, Action destroy);

struct SystemHandler {
    const EcsManager* manager;

    VEC(InitDataPair) inits;

    VEC(ActionDataPair) pre_updates;
    VEC(ActionDataPair) updates;
    VEC(ActionDataPair) post_updates;

    VEC(ActionDataPair) destroys;
};

SystemHandler* system_handler_new(const EcsManager*, const SystemHandlerConfig*);

void system_handler_add(SystemHandler*, EcsSystem*);

void system_handler_init(const SystemHandler*);

void system_handler_pre_update(const SystemHandler*);

void system_handler_update(const SystemHandler*);

void system_handler_post_update(const SystemHandler*);

void system_handler_destroy(const SystemHandler*);

void system_handler_free(SystemHandler*);

#endif
