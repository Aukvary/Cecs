#ifndef ECS_MANAGER_H
#define ECS_MANAGER_H

#include <stddef.h>
#include <stdint.h>
#include "../../Collections/Collections.h"
#include "../../Collections/Iterator.h"
#include "../EcsTypes.h"
#include "../Entity/Entity.h"
#include "../Pools/ECSPool.h"

typedef struct {
    size_t dense_size;
    size_t sparce_size;
    size_t recycle_size;

    size_t components_count;

    size_t pools_size;

    size_t masks_size;

    size_t include_mask_count;
    size_t exclude_mask_count;

    size_t filters_size;
} EcsConfig;

struct ECSMask {
    EcsManager* manager;

    PoolData* include_pools;
    size_t include_size;
    int include_count;

    PoolData* exclude_pools;
    size_t exclude_size;
    int exclude_count;
    uint64_t hash;
};

EcsMask mask_new(EcsManager*, size_t, size_t);

void mask_inc(EcsMask*, PoolData);

void mask_exc(EcsMask*, PoolData);

EcsFilter* mask_end(EcsMask);

struct ECSFilter {
    EcsManager* manager;
    EcsMask mask;

    EntityContainer entities;
};

struct EcsManager {
    EntityInfo* sparce_entities;
    size_t sparse_size;
    Entity entities_ptr;

    size_t cfg_dense_size;
    size_t cfg_recycle_size;

    PoolData* components;
    size_t components_count;

    Entity* recycled_entities;
    size_t recycled_size;
    size_t recycled_ptr;

    EcsPool** pools;
    size_t pools_size;
    size_t pools_count;

    VEC(EcsMask) masks;
    size_t include_mask_count;
    size_t exclude_mask_count;

    EcsFilter** filters;
    size_t filters_size;
    size_t filters_count;

    VEC(EcsFilter*) * filter_by_include;
    VEC(EcsFilter*) * filter_by_exclude;
};

#define GET_POOL(manager, ComponentType)                                                                               \
    ({                                                                                                                 \
        EcsPool* res_pool = ecs_manager_get_pool((manager), POOL_DATA(ComponentType));                                 \
        if (res_pool == NULL) {                                                                                        \
            res_pool = POOL_NEW(ComponentType, (manager), (manager)->cfg_dense_size, (manager)->sparse_size,           \
                                (manager)->cfg_recycle_size);                                                          \
            ecs_manager_add_pool((manager), res_pool);                                                                  \
        }                                                                                                              \
        res_pool;                                                                                                      \
    })

#define GET_MASK(manager, ComponentType)                                                                               \
    ({                                                                                                                 \
        ComponentType _v;                                                                                              \
        mask_new(manager, manager->include_mask_count, manager->exclude_mask_count);                                   \
    })

EcsManager* ecs_manager_new(const EcsConfig*);

Entity ecs_manager_new_entity(EcsManager*);

void ecs_manager_kill_entity(EcsManager*, Entity);

EntityInfo ecs_manager_get_entity(const EcsManager*, Entity);

size_t ecs_manager_get_entity_components_count(const EcsManager*, Entity);

uint16_t ecs_manager_get_entity_gen(const EcsManager*, Entity);

void ecs_manager_copy_entity(const EcsManager*, Entity, Entity);

void ecs_manager_entity_add_component(const EcsManager*, Entity, PoolData, void*);

void ecs_manager_entity_remove_component(const EcsManager*, Entity, PoolData);

void ecs_manager_add_pool(EcsManager*, EcsPool*);

EcsPool* ecs_manager_get_pool(const EcsManager*, PoolData);

void on_entity_change(const EcsManager*, Entity, size_t, int);

#endif
