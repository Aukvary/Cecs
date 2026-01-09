#include "DtEcs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef COMPONENT_TABLE_SIZE
#define COMPONENT_TABLE_SIZE 107
#endif

static int id_counter = 0;
static const ComponentData* component_data_by_id[COMPONENT_TABLE_SIZE] = {NULL};
static const ComponentData* component_data_by_name[COMPONENT_TABLE_SIZE] = {NULL};

static int component_get_hash(const char* name);

EcsPool* ecs_pool_new(const DtEcsManager* manager, const char* name, const u16 size) {
    return size != 0 ? component_pool_new(manager, name, size, NULL, NULL)
                     : tag_pool_new(manager, name);
}

EcsPool* ecs_pool_new_by_id(const DtEcsManager* manager, const u16 id) {
    const ComponentData* data = component_get_data_by_id(id);

    return data->component_size != 0
               ? component_pool_new(manager, data->name, data->component_size, NULL, NULL)
               : tag_pool_new(manager, data->name);
}

void ecs_pool_add(EcsPool* pool, const Entity entity, const void* data) {
    if (pool->has(pool->data, entity))
        return;

    pool->count++;
    pool->add(pool->data, entity, data);
    dt_on_entity_change(pool->manager, entity, pool->ecs_manager_id, 1);
    printf("[DEBUG]\t entity \"%d\" was added to %s pool\n", entity, pool->name);
}

inline void* ecs_pool_get(const EcsPool* pool, const Entity entity) {
    return pool->get(pool->data, entity);
}

inline int ecs_pool_has(const EcsPool* pool, const Entity entity) {
    return pool->has(pool->data, entity);
}

inline void ecs_pool_remove(EcsPool* pool, const Entity entity) {
    if (!pool->has(pool->data, entity))
        return;

    pool->count--;
    dt_on_entity_change(pool->manager, entity, pool->ecs_manager_id, 0);
    pool->remove(pool->data, entity);
    printf("[DEBUG]\t entity \"%d\" was removed from %s pool\n", entity, pool->name);
}

void ecs_pool_resize(EcsPool* pool, const u64 size) { pool->resize(pool->data, size); }

void ecs_pool_free(EcsPool* pool) { pool->free(pool->data); }


static int component_get_hash(const char* name) {
    int hash = 2147483647;
    while (*name) {
        hash ^= *name++;
        hash *= 314159;
    }
    return hash;
}

void register_component(ComponentData* data) {
    if (component_get_data_by_name(data->name) != NULL) return;

    data->id = id_counter++;

    component_data_by_id[data->id] = data;

    int idx = component_get_hash(data->name) % COMPONENT_TABLE_SIZE;
    const int start = idx;
    while (component_data_by_name[idx] != NULL) {
        idx = (idx + 1) % COMPONENT_TABLE_SIZE;
        if (idx == start) {
            printf("[DEBUG]component count out of range");
            exit(1);
        }
    }

    component_data_by_name[idx] = data;
    printf("[DEBUD]%s component was registred with id %d\n", data->name, data->id);
}

const ComponentData* component_get_data_by_id(const u16 id) {
    if (id < 0 || id >= COMPONENT_TABLE_SIZE) {
        return NULL;
    }

    if (component_data_by_id[id] == NULL) {
        return NULL;
    }

    return component_data_by_id[id];
}

const ComponentData* component_get_data_by_name(const char* name) {
    int idx = component_get_hash(name) % COMPONENT_TABLE_SIZE;
    const int start = idx;

    while (component_data_by_name[idx] != NULL &&
           strcmp(component_data_by_name[idx]->name, name) != 0) {
        idx = (idx + 1) % COMPONENT_TABLE_SIZE;
        if (idx == start) {
            return NULL;
        }
    }

    if (component_data_by_name[idx] == NULL) {
        return NULL;
    }

    return component_data_by_name[idx];
}
