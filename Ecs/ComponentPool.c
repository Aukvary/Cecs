#include <stdio.h>
#include <stdlib.h>
#include "DtEcs.h"

static void component_pool_add(void* pool, Entity entity, const void* data);

static void* component_pool_get_item(const void* pool, Entity entity);

static bool component_pool_has(const void* pool, Entity entity);

static void component_pool_remove(void* pool, Entity entity);

static void component_pool_resize(void* pool, size_t new_size);

static void component_pool_free(void* pool);

struct ComponentPool {
    EcsPool pool;
    EntityContainer entities;
};

EcsPool* component_pool_new(const DtEcsManager* manager, const char* name, u16 item_size,
                            ResetItemHandler reset_handler,
                            CopyItemHandler copy_handler) {
    ComponentPool* pool = malloc(sizeof(ComponentPool));

    const int id = component_get_data_by_name(name)->id;

    *pool = (ComponentPool) {
        .pool =
            (EcsPool) {
                .manager = manager,
                .name = name,
                .component_id = id,
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
                                 manager->recycled_size, reset_handler, copy_handler),
    };

    return &pool->pool;
}

static void component_pool_add(void* pool, Entity entity, const void* data) {
    ComponentPool* component_pool = pool;
    entity_container_add(&component_pool->entities, entity, data);
}

static void* component_pool_get_item(const void* pool, Entity entity) {
    const ComponentPool* component_pool = pool;
    return entity_container_get(&component_pool->entities, entity);
}

static bool component_pool_has(const void* pool, const Entity entity) {
    const ComponentPool* component_pool = pool;
    return entity_container_has(&component_pool->entities, entity);
}

static void component_pool_remove(void* pool, const Entity entity) {
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
