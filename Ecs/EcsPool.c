#include "DtEcs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ComponentsHandler.h"

static int component_get_hash(const char* name);

DtEcsPool* dt_ecs_pool_new(const DtEcsManager* manager, const char* name, const u16 size) {
    return size != 0 ? dt_component_pool_new(manager, name, size, NULL, NULL)
                     : dt_tag_pool_new(manager, name);
}

DtEcsPool* dt_ecs_pool_new_by_id(const DtEcsManager* manager, const u16 id) {
    const DtComponentData* data = component_get_data_by_id(id);

    return data->component_size != 0
               ? dt_component_pool_new(manager, data->name, data->component_size, NULL, NULL)
               : dt_tag_pool_new(manager, data->name);
}

void dt_ecs_pool_add(DtEcsPool* pool, const DtEntity entity, const void* data) {
    if (pool->has(pool->data, entity))
        return;

    pool->count++;
    pool->add(pool->data, entity, data);
    dt_on_entity_change(pool->manager, entity, pool->ecs_manager_id, 1);
    printf("[DEBUG]\t entity \"%d\" was added to %s pool\n", entity, pool->name);
}

inline void* dt_ecs_pool_get(const DtEcsPool* pool, const DtEntity entity) {
    return pool->get(pool->data, entity);
}

inline int dt_ecs_pool_has(const DtEcsPool* pool, const DtEntity entity) {
    return pool->has(pool->data, entity);
}

inline void dt_ecs_pool_remove(DtEcsPool* pool, const DtEntity entity) {
    if (!pool->has(pool->data, entity))
        return;

    pool->count--;
    dt_on_entity_change(pool->manager, entity, pool->ecs_manager_id, 0);
    pool->remove(pool->data, entity);
    printf("[DEBUG]\t entity \"%d\" was removed from %s pool\n", entity, pool->name);
}

void dt_ecs_pool_resize(DtEcsPool* pool, const u64 size) { pool->resize(pool->data, size); }

void dt_ecs_pool_free(DtEcsPool* pool) { pool->free(pool->data); }
