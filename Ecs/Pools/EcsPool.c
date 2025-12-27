#include "ECSPool.h"
#include "../EcsManager/EcsManager.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* component_names[100] = {NULL};

EcsPool* ecs_pool_new(const EcsManager* manager, const char* name, size_t component_size) {
    return component_size != 0
               ? component_pool_new(manager, name, component_size, NULL, NULL)
               : tag_pool_new(manager, name);
}

uint64_t ecs_pool_get_hash(const char* name) {
    uint64_t hash = 14695981039346656037ULL;
    while (*name) {
        hash ^= (uint64_t) (*name++);
        hash *= 1099511628211ULL;
    }
    return hash;
}

void ecs_pool_add(EcsPool* pool, const Entity entity, const void* data) {
    pool->count++;
    pool->add(pool->data, entity, data);
    on_entity_change(pool->manager, entity, pool->info.id, 1);
    printf("[DEBUG]\t entity \"%d\" was added to %s pool\n", entity, pool->info.name);
}

inline void* ecs_pool_get(const EcsPool* pool, Entity entity) {
    return pool->get(pool->data, entity);
}

inline int ecs_pool_has(const EcsPool* pool, const Entity entity) {
    return pool->has(pool->data, entity);
}

inline void ecs_pool_remove(EcsPool* pool, Entity entity) {
    pool->count--;
    on_entity_change(pool->manager, entity, pool->info.id, 0);
    pool->remove(pool->data, entity);
    printf("[DEBUG]\t entity \"%d\" was removed from %s pool\n", entity, pool->info.name);
}

void ecs_pool_resize(EcsPool* pool, const size_t new_size) {
    pool->resize(pool->data, new_size);
}

void ecs_pool_free(EcsPool* pool) { pool->free(pool->data); }


void register_component_id(int* id, const char* name) {
    static int id_counter = 0;
    *id = id_counter++;

    component_names[*id] = name;
}

const char* get_component_name_by_id(int id) {
    return component_names[id];
}

int get_component_id_by_name(const char* name) {
    for (int i = 0; i < sizeof(component_names) / sizeof(component_names[0]); i++) {
        if (component_names[i] == NULL) return -1;

        if (strcmp(component_names[i], name) == 0) {
            return i;
        }
    }

    return -1;
}