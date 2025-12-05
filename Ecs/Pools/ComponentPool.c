#include <stdio.h>
#include <stdlib.h>
#include "../EcsManager/EcsManager.h"
#include "EcsPool.h"

static void component_pool_add(void* pool, Entity entity, const void* data);

static void* component_pool_get_item(const void* pool, Entity entity);

static int component_pool_has(const void* pool, Entity entity);

static void component_pool_remove(void* pool, Entity entity);

static void component_pool_resize(void* pool, size_t new_size);

static void component_pool_free(void* pool);

struct ComponentPool {
    EcsPool pool;
    EntityContainer entities;
};

EcsPool* component_pool_new(const EcsManager* manager, const char* name, size_t item_size,
                            ResetItemHandler auto_reset, CopyItemHandler auto_copy) {
    ComponentPool* pool = malloc(sizeof(ComponentPool));

    *pool = (ComponentPool) {
        .pool =
            (EcsPool) {
                .manager = manager,
                .info = (PoolInfo) {.name = name, .hash = ecs_pool_get_hash(name)},
                .count = 0,

                .data = pool,

                .add = component_pool_add,
                .get = component_pool_get_item,
                .has = component_pool_has,
                .remove = component_pool_remove,
                .resize = component_pool_resize,
                .free = component_pool_free,
            },

        .entities =
            entity_container_new(item_size, manager->cfg_dense_size, manager->sparse_size,
                                 manager->recycled_size, auto_reset, auto_copy),
    };

    return &pool->pool;
}

static void component_pool_add(void* pool, Entity entity, const void* data) {
    ComponentPool* component_pool = pool;
    entity_container_add(&component_pool->entities, entity, data);
    printf("[DEBUG]\t entity \"%d\" was added to %s pool\n", entity,
           component_pool->pool.info.name);
}

static void* component_pool_get_item(const void* pool, Entity entity) {
    ComponentPool* component_pool = pool;
    return entity_container_get(&component_pool->entities, entity);
}

static int component_pool_has(const void* pool, const Entity entity) {
    ComponentPool* component_pool = pool;
    return entity_container_has(&component_pool->entities, entity);
}

static void component_pool_remove(void* pool, Entity entity) {
    ComponentPool* component_pool = pool;
    entity_container_remove(&component_pool->entities, entity);
}

static void component_pool_resize(void* pool, const size_t new_size) {
    ComponentPool* component_pool = pool;
    entity_container_resize(&component_pool->entities, new_size);
}

static void component_pool_free(void* pool) {
    entity_container_free(&((ComponentPool*) pool)->entities);
    free(pool);
}
