#include "ECSPool.h"
#include "../EcsManager/EcsManager.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

EcsPool* ecs_pool_new(const EcsManager* manger, const char* name, const size_t item_size, const size_t dense_size,
                      const size_t sparse_size, const size_t recycle_size) {

    EcsPool* pool = malloc(sizeof(EcsPool));

    *pool = (EcsPool) {
            .manager = manger,
            .data = (PoolData) {.hash = ecs_pool_get_hash(name), .name = name},
            .component_container = entity_container_new(item_size, dense_size, recycle_size, sparse_size, NULL, NULL),
    };

    printf("[DEBUG]\tpool %s was created\n", name);

    return pool;
}

uint64_t ecs_pool_get_hash(const char* name) {
    uint64_t hash = 14695981039346656037ULL;
    while (*name) {
        hash ^= (uint64_t) (*name++);
        hash *= 1099511628211ULL;
    }
    return hash;
}

inline void ecs_pool_add_item(EcsPool* pool, const Entity entity, const void* data) {
    pool->count++;
    entity_container_add(&pool->component_container, entity, data);
    on_entity_change(pool->manager, entity, pool->data.id, 1);

    printf("[DEBUG]\t entity \"%d\" was added to %s pool\n", entity, pool->data.name);
}

inline void* ecs_pool_get_item(const EcsPool* pool, Entity entity) {
    return entity_container_get(&pool->component_container, entity);
}

inline int ecs_pool_has(const EcsPool* pool, const Entity entity) {
    return entity_container_has(&pool->component_container, entity);
}

inline void ecs_pool_remove(EcsPool* pool, Entity entity) {
    pool->count--;
    entity_container_remove(&pool->component_container, entity);
    on_entity_change(pool->manager, entity, pool->data.id, 1);
}

void ecs_pool_resize(EcsPool* pool, const size_t new_size) {
    entity_container_resize(&pool->component_container, new_size);
}

void ecs_pool_free(EcsPool* pool) {

}
