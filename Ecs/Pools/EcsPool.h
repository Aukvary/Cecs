#ifndef ECS_POOL
#define ECS_POOL

#include <stddef.h>
#include <stdint.h>
#include "../EcsTypes.h"
#include "../Entity/Entity.h"

typedef struct ComponentPool ComponentPool;
typedef struct TagPool TagPool;

typedef enum { Component, Tag } PoolType;

typedef struct PoolInfo {
    const char* name;
    uint64_t hash;
    size_t id;
} PoolInfo;

struct EcsPool {
    const EcsManager* manager;
    size_t count;
    PoolInfo info;
    void* data;

    void (*add)(void*, Entity, const void*);
    void* (*get)(const void*, Entity);
    int (*has)(const void*, Entity);
    void (*remove)(void*, Entity);
    void (*resize)(void*, size_t);
    void (*free)(void*);
};

#define COMPONENT_POOL_NEW(T, manager, dense, sparse, recycle)                           \
    (component_pool_new(manager, (#T), sizeof(T), dense, sparse, recycle, NULL, NULL))

#define TAG_POOL_NEW(T, manager, sparse)                                                 \
    ({                                                                                   \
        T* _;                                                                            \
        (tag_pool_new(manager, (#T), sparse));                                           \
    })

#define POOL_HASH(component)                                                             \
    ({                                                                                   \
        component* _;                                                                     \
        ecs_pool_hash(#component);                                                       \
    })

#define POOL_DATA(component)                                                             \
    ({                                                                                   \
        component* _;                                                                     \
        (PoolInfo){.name = #component, .hash = ecs_pool_get_hash(#component)};           \
    })

EcsPool* component_pool_new(const EcsManager*, const char*, size_t, size_t, size_t,
                            size_t, ResetItemHandler, CopyItemHandler);

EcsPool* tag_pool_new(const EcsManager*, const char*, size_t);

uint64_t ecs_pool_get_hash(const char*);

void ecs_pool_add_item(EcsPool*, Entity, const void*);

void* ecs_pool_get_item(const EcsPool*, Entity);

int ecs_pool_has(const EcsPool*, Entity);

void ecs_pool_remove(EcsPool*, Entity);

void ecs_pool_resize(EcsPool*, size_t);

void ecs_pool_free(EcsPool*);

#endif
