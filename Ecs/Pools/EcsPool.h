#ifndef ECS_POOL
#define ECS_POOL

#include <stddef.h>
#include <stdint.h>
#include "../EcsTypes.h"
#include "../Entity/Entity.h"


typedef struct PoolData {
    const char* name;
    uint64_t hash;
    size_t id;
} PoolData;

struct EcsPool {
    const EcsManager* manager;
    PoolData data;
    size_t count;
    EntityContainer component_container;
};

#define POOL_NEW(T, manager, dense, sparse, recycle) (ecs_pool_new(manager, (#T), sizeof(T), dense, sparse, recycle))

#define POOL_HASH(component)                                                                                           \
    ({                                                                                                                 \
        component _;                                                                                                   \
        ecs_pool_hash(#component);                                                                                     \
    })

#define POOL_DATA(component)                                                                                           \
    ({                                                                                                                 \
        component _;                                                                                                   \
        (PoolData){.name = #component, .hash = ecs_pool_get_hash(#component)};                                         \
    })

#define POOL_GET(T, pool, entity) (T*) (__ecs_pool_get_item(pool, entity))

EcsPool* ecs_pool_new(const EcsManager*, const char*, size_t, size_t, size_t, size_t);

uint64_t ecs_pool_get_hash(const char*);

void ecs_pool_add_item(EcsPool*, Entity, const void*);

void* ecs_pool_get_item(const EcsPool*, Entity);

int ecs_pool_has(const EcsPool*, Entity);

void ecs_pool_remove(EcsPool*, Entity);

void ecs_pool_resize(EcsPool*, size_t);

void ecs_pool_free(EcsPool*);

#endif
