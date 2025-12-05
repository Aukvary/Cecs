#include "EcsManager.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Collections/Iterator.h"

static int cmp_pools(const void*, const void*);


static EcsFilter* filter_new(EcsManager*, EcsMask);
static void filter_add_entity(EcsFilter*, Entity);
static void filter_remove_entity(EcsFilter*, Entity);
static void filter_resize(EcsFilter*, size_t);
static void filter_free(EcsFilter*);


static void ecs_manager_resize_filters(EcsManager*);
static void ecs_manager_resize_pools(EcsManager*);
static EcsFilter* get_filter(EcsManager*, EcsMask);

static int is_mask_compatible(const EcsManager*, EcsMask, Entity);
static int is_mask_compatible_without(const EcsManager*, EcsMask, Entity, int);

EcsMask mask_new(const EcsManager* manager, const size_t inc_size, const size_t exc_size,
                 const PoolInfo pool) {
    EcsMask mask = (EcsMask) {
        .manager = manager,

        .include_pools = calloc(inc_size, sizeof(PoolInfo)),
        .include_size = inc_size,
        .include_count = 0,

        .exclude_pools = calloc(inc_size, sizeof(PoolInfo)),
        .exclude_size = exc_size,
        .exclude_count = 0,
    };

    mask_inc(&mask, pool);

    return mask;
}

void mask_inc(EcsMask* mask, const PoolInfo data) {
    if (mask->include_count == mask->include_size) {
        mask->include_count *= 2;
        void* tmp = realloc(mask->include_pools, mask->include_size * sizeof(PoolInfo));

        if (!tmp) {
            printf("mask realloc failed\n");
            exit(1);
        }

        mask->include_pools = tmp;
    }

    mask->include_pools[mask->include_count++] = data;
}

void mask_exc(EcsMask* mask, const PoolInfo data) {
    if (mask->exclude_count == mask->exclude_size) {
        mask->exclude_size *= 2;
        void* tmp = realloc(mask->exclude_pools, mask->exclude_size * sizeof(PoolInfo));

        if (!tmp) {
            printf("mask realloc failed\n");
            exit(1);
        }

        mask->include_pools = tmp;
    }

    mask->exclude_pools[mask->exclude_count++] = data;
}

static int cmp_pools(const void* p1, const void* p2) {
    return ((PoolInfo*) p1)->id - ((PoolInfo*) p2)->id;
}

EcsFilter* mask_end(EcsMask mask) {
    qsort(mask.include_pools, mask.include_count, sizeof(PoolInfo), cmp_pools);
    qsort(mask.exclude_pools, mask.exclude_count, sizeof(PoolInfo), cmp_pools);

    mask.hash = 314519;
    for (int i = 0; i < mask.include_count; i++) {
        mask.hash += mask.include_pools[i].id;
        mask.hash ^= mask.include_pools[i].hash;
    }

    for (int i = 0; i < mask.exclude_count; i++) {
        mask.hash -= mask.exclude_pools[i].id;
        mask.hash ^= mask.exclude_pools[i].hash;
    }

    return get_filter(mask.manager, mask);
}

static EcsFilter* filter_new(EcsManager* manager, EcsMask const mask) {
    EcsFilter* new_filter = malloc(sizeof(EcsFilter));

    *new_filter = (EcsFilter) {
        .manager = manager,
        .entities = entity_container_new(sizeof(Entity), manager->cfg_dense_size,
                                         manager->sparse_size, manager->cfg_recycle_size,
                                         NULL, NULL),
        .mask = mask,
    };


    return new_filter;
}

static void filter_add_entity(EcsFilter* filter, Entity entity) {
    entity_container_add(&filter->entities, entity, &entity);
}

static void filter_remove_entity(EcsFilter* filter, const Entity entity) {
    entity_container_remove(&filter->entities, entity);
}

static void filter_resize(EcsFilter* filter, const size_t new_size) {
    entity_container_resize(&filter->entities, new_size);
}

static void filter_free(EcsFilter* filter) {
    entity_container_free(&filter->entities);
    free(filter);
}


EcsManager* ecs_manager_new(const EcsConfig* cfg) {
    EcsManager* manager = malloc(sizeof(EcsManager));
    *manager = (EcsManager) {
        .sparce_entities = calloc(cfg->sparce_size, sizeof(EntityInfo)),
        .sparse_size = cfg->sparce_size,
        .entities_ptr = 0,

        .cfg_dense_size = cfg->dense_size,
        .cfg_recycle_size = cfg->recycle_size,

        .components = calloc(cfg->components_count, sizeof(PoolInfo)),
        .components_count = cfg->components_count,

        .recycled_entities = calloc(cfg->recycle_size, sizeof(Entity)),
        .recycled_size = cfg->recycle_size,
        .recycled_ptr = 0,

        .pools = calloc(cfg->pools_size, sizeof(EcsPool*)),
        .pools_size = cfg->pools_size,
        .pools_count = 0,

        .masks = VEC_NEW(EcsMask, cfg->masks_size),
        .include_mask_count = cfg->include_mask_count,
        .exclude_mask_count = cfg->exclude_mask_count,

        .filters = calloc(cfg->filters_size, sizeof(EcsFilter*)),
        .filters_size = cfg->filters_size,
        .filters_count = 0,

        .filter_by_include = calloc(cfg->pools_size, sizeof(VEC(EcsFilter*))),
        .filter_by_exclude = calloc(cfg->pools_size, sizeof(VEC(EcsFilter*))),
    };

    return manager;
}

Entity ecs_manager_new_entity(EcsManager* manager) {
    Entity entity;

    if (manager->recycled_ptr > 0) {
        entity = manager->recycled_entities[manager->recycled_ptr--];
        manager->sparce_entities[entity].gen =
            abs(manager->sparce_entities[entity].gen) + 1;
        printf("[DEBUG]\t recycled entity \"%d\" was created\n", entity);
    } else {
        entity = manager->entities_ptr++;
        if (entity == manager->sparse_size) {
            manager->sparse_size *= 2;
            void* tmp = realloc(manager->sparce_entities,
                                manager->sparse_size * sizeof(EntityInfo));

            if (!tmp) {
                printf("entity add realloc failed\n");
                exit(1);
            }

            manager->sparce_entities = tmp;

            for (int i = 0; i < manager->pools_size; i++) {
                if (manager->pools[i])
                    ecs_pool_resize(manager->pools[i], manager->sparse_size);
            }

            for (int i = 0; i < manager->filters_size; i++) {
                if (manager->filters[i])
                    filter_resize(manager->filters[i], manager->sparse_size);
            }
        }
        printf("[DEBUG]\t new entity \"%d\" was created\n", entity);
    }

    return entity;
}

void ecs_manager_kill_entity(EcsManager* manager, const Entity entity) {
    if (manager->entities_ptr < entity || entity < 0)
        return;
    if (manager->sparce_entities[entity].gen < 0)
        return;

    if (manager->recycled_ptr == manager->recycled_size) {
        manager->recycled_size *= 2;
        void* tmp =
            realloc(manager->recycled_entities, manager->recycled_size * sizeof(Entity));

        if (!tmp) {
            printf("entity kill realloc failed\n");
            exit(1);
        }

        manager->recycled_entities = tmp;
    }

    manager->recycled_entities[manager->recycled_ptr++] = entity;
    manager->sparce_entities[entity].gen *= -1;

    for (int i = 0; i < manager->pools_size; i++) {
        if (manager->pools[i] == NULL)
            continue;

        if (ecs_pool_has(manager->pools[i], entity))
            ecs_pool_remove(manager->pools[i], entity);
    }

    for (int i = 0; i < manager->filters_size; i++) {
        if (manager->filters[i] == NULL)
            continue;

        if (entity_container_has(&manager->filters[i]->entities, entity))
            filter_remove_entity(manager->filters[i], entity);
    }

    printf("[DEBUG]\t entity \"%d\" was killed\n", entity);
}

EntityInfo ecs_manager_get_entity(const EcsManager* manager, const Entity entity) {
    if (manager->entities_ptr < entity || entity < 0)
        return ENTITY_INFO_NULL;
    if (manager->sparce_entities[entity].gen < 0)
        return ENTITY_INFO_NULL;

    return manager->sparce_entities[entity];
}

size_t ecs_manager_get_entity_components_count(const EcsManager* manager,
                                               const Entity entity) {
    if (manager->entities_ptr < entity || entity < 0)
        return 0;
    if (manager->sparce_entities[entity].gen < 0)
        return 0;

    return manager->sparce_entities[entity].component_count;
}

uint16_t ecs_manager_get_entity_gen(const EcsManager* manager, const Entity entity) {
    if (manager->entities_ptr < entity || entity < 0)
        return 0;
    if (manager->sparce_entities[entity].gen < 0)
        return 0;

    return manager->sparce_entities[entity].gen;
}

void ecs_manager_copy_entity(const EcsManager* manager, const Entity dst,
                             const Entity src) {
    if (manager->entities_ptr < dst || dst < 0)
        return;
    if (manager->sparce_entities[dst].gen < 0)
        return;

    if (manager->entities_ptr < src || src < 0)
        return;
    if (manager->sparce_entities[src].gen < 0)
        return;

    const size_t count = manager->sparce_entities[src].component_count =
        manager->sparce_entities[dst].component_count;
    memcpy(&manager->components[src], &manager->components[src],
           count * sizeof(PoolInfo));
}

void ecs_manager_entity_add_component(const EcsManager* manager, const Entity entity,
                                      const PoolInfo pool_hash, void* data) {
    if (manager->entities_ptr < entity || entity < 0)
        return;
    if (manager->sparce_entities[entity].gen < 0)
        return;

    EcsPool* pool = ecs_manager_get_pool(manager, pool_hash);

    ecs_pool_add(pool, entity, data);
}

void ecs_manager_entity_remove_component(const EcsManager* manager, const Entity entity,
                                         const PoolInfo pool_hash) {
    if (manager->entities_ptr < entity || entity < 0)
        return;
    if (manager->sparce_entities[entity].gen < 0)
        return;

    EcsPool* pool = ecs_manager_get_pool(manager, pool_hash);
    ecs_pool_remove(pool, entity);
}

void ecs_manager_add_pool(EcsManager* manager, EcsPool* pool) {
    size_t idx = pool->info.hash % manager->pools_size;
    size_t start = idx;
    while (manager->pools[idx] != NULL) {
        if (pool->info.id == manager->pools[idx]->info.id)
            return;

        idx = (idx + 1) % manager->pools_size;
        if (idx == start) {
            ecs_manager_resize_pools(manager);
            break;
        }
    }

    idx = pool->info.hash % manager->pools_size;

    while (manager->pools[idx] != NULL) {
        idx = (idx + 1) % manager->pools_size;
    }

    manager->pools[idx] = pool;

    pool->info.id = idx;
}

EcsPool* ecs_manager_get_pool(const EcsManager* manager, const PoolInfo pool_hash) {
    size_t idx = pool_hash.hash % manager->pools_size;
    size_t start = idx;
    while (manager->pools[idx] &&
           strcmp(pool_hash.name, manager->pools[idx]->info.name) != 0) {
        idx = (idx + 1) % manager->pools_size;
        if (idx == start) {
            return NULL;
        }
    }

    return manager->pools[idx];
}

static void ecs_manager_resize_pools(EcsManager* manager) {
    manager->pools_size *= 2;
    EcsPool** pools = calloc(manager->pools_size, sizeof(EcsPool*));

    if (pools == NULL) {
        printf("Failed to allocate memory for pools\n");
        exit(1);
    }

    void* tmp =
        realloc(manager->filter_by_include, manager->pools_size * sizeof(EcsFilter*));

    if (!tmp) {
        printf("[DEBUG]\t Failed to allocate memory for filters by include\n]");
        exit(1);
    }

    manager->filter_by_include = tmp;

    tmp = realloc(manager->filter_by_exclude, manager->pools_size * sizeof(EcsFilter*));

    if (!tmp) {
        printf("[DEBUG]\t Failed to allocate memory for filters by exclude\n]");
        exit(1);
    }

    manager->filter_by_exclude = tmp;

    for (int i = 0; i < manager->pools_count; i++) {
        size_t idx = manager->pools[i]->info.hash % manager->pools_size;

        while (manager->pools[idx] != NULL) {
            idx = (idx + 1) % manager->pools_size;
        }

        pools[idx] = manager->pools[i];
        manager->pools[i]->info.id = idx;
    }

    free(manager->pools);
    manager->pools = pools;
}

static EcsFilter* get_filter(EcsManager* manager, const EcsMask mask) {
    uint64_t hash = mask.hash;

    EcsFilter* filter = NULL;

    size_t idx = hash % manager->filters_size;
    size_t start = idx;

    while (manager->filters[idx] != NULL && manager->filters[idx]->mask.hash != hash) {
        idx = (idx + 1) % manager->filters_size;
        if (idx == start) {
            ecs_manager_resize_filters(manager);
        }
    }

    filter = manager->filters[idx];

    if (filter)
        return filter;

    filter = filter_new(manager, mask);

    while (manager->filters[idx] != NULL) {
        idx = (idx + 1) % manager->filters_size;
    }

    manager->filters[idx] = filter;


    for (int i = 0; i < mask.include_count; i++) {
        VEC(EcsFilter*) list = manager->filter_by_include[mask.include_pools[i].id];

        if (!list) {
            list = VEC_NEW(EcsFilter*, mask.include_count);
            manager->filter_by_include[mask.include_pools[i].id] = list;
        }

        VEC_ADD(list, filter);
    }

    for (int i = 0; i < mask.exclude_count; i++) {
        VEC(EcsFilter*) list = manager->filter_by_exclude[mask.exclude_pools[i].id];

        if (!list) {
            list = VEC_NEW(EcsFilter*, mask.exclude_count);
            manager->filter_by_exclude[mask.include_pools[i].id] = list;
        }

        VEC_ADD(list, filter);
    }

    for (Entity i = 0; i < manager->entities_ptr; i++) {
        if (manager->sparce_entities[i].gen > -1 &&
            is_mask_compatible(manager, mask, i)) {
            filter_add_entity(filter, i);
        }
    }

    return filter;
}

static void ecs_manager_resize_filters(EcsManager* manager) {
    manager->filters_size *= 2;
    EcsFilter** filters = calloc(manager->filters_size, sizeof(EcsFilter*));

    if (filters == NULL) {
        printf("[DEBUG]Failed to allocate memory for filters\n");
        exit(1);
    }

    for (int i = 0; i < manager->filters_count; i++) {
        size_t idx = manager->filters[i]->mask.hash % manager->filters_size;

        while (manager->filters[idx] != NULL) {
            idx = (idx + 1) % manager->filters_size;
        }

        filters[idx] = manager->filters[i];
    }

    free(manager->filters);
    manager->filters = filters;
}

void on_entity_change(const EcsManager* manager, const Entity entity, const size_t id,
                      const int added) {
    VEC(EcsFilter*) include_list = manager->filter_by_include[id];
    VEC(EcsFilter*) exclude_list = manager->filter_by_exclude[id];


    if (added) {
        if (include_list != NULL) {
            FOREACH(EcsFilter*, filter, *VEC_ITERATOR(include_list), {
                if (is_mask_compatible(manager, filter->mask, entity)) {
                    filter_add_entity(filter, entity);
                }
            });
        }
        if (exclude_list != NULL) {
            printf("exc\n");

            FOREACH(EcsFilter*, filter, *VEC_ITERATOR(exclude_list), {
                if (is_mask_compatible_without(manager, filter->mask, entity, id)) {
                    filter_remove_entity(filter, entity);
                }
            });
        }
    } else {
        if (include_list != NULL) {
            FOREACH(EcsFilter*, filter, *VEC_ITERATOR(include_list), {
                if (is_mask_compatible(manager, filter->mask, entity)) {
                    filter_remove_entity(filter, entity);
                }
            });
        }
        if (exclude_list != NULL) {
            FOREACH(EcsFilter*, filter, *VEC_ITERATOR(exclude_list), {
                if (is_mask_compatible_without(manager, filter->mask, entity, id)) {
                    filter_add_entity(filter, entity);
                }
            });
        }
    }
}

static int is_mask_compatible(const EcsManager* manager, const EcsMask filterMask,
                              const Entity entity) {
    EcsPool* pool1 = manager->pools[filterMask.include_pools[0].id];
    EcsPool* pool2 = manager->pools[filterMask.include_pools[1].id];

    for (int i = 0; i < filterMask.include_count; i++) {
        if (!ecs_pool_has(manager->pools[filterMask.include_pools[i].id], entity)) {
            return 0;
        }
    }
    for (int i = 0; i < filterMask.exclude_count; i++) {
        if (ecs_pool_has(manager->pools[filterMask.exclude_pools[i].id], entity)) {
            return 0;
        }
    }
    return 1;
}

static int is_mask_compatible_without(const EcsManager* manager, const EcsMask filterMask,
                                      const Entity entity, int id) {
    for (int i = 0; i < filterMask.include_count; i++) {
        size_t typeId = filterMask.include_pools[i].id;
        if (typeId == id || !ecs_pool_has(manager->pools[typeId], entity)) {
            return 0;
        }
    }

    for (int i = 0; i < filterMask.exclude_count; i++) {
        size_t typeId = filterMask.exclude_pools[i].id;
        if (typeId != id && !ecs_pool_has(manager->pools[typeId], entity)) {
            return 0;
        }
    }
    return 1;
}

void ecs_manager_free(EcsManager* manager) {
    free(manager->sparce_entities);
    free(manager->components);
    free(manager->recycled_entities);

    for (int i = 0; i < manager->pools_count; i++) {
        if (manager->pools[i] == NULL)
            continue;
        ecs_pool_free(manager->pools[i]);
        vec_free(manager->filter_by_include[i]);
        vec_free(manager->filter_by_exclude[i]);
    }

    vec_free(manager->masks);

    for (int i = 0; i < manager->filters_size; i++) {
        if (manager->filters[i] != NULL)
            filter_free(manager->filters[i]);
    }

    free(manager);
}
