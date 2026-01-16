#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ComponentsHandler.h"
#include "DtEcs.h"

/**
 * @brief compair two pools by id
 */
static int cmp_pools(const void* id1, const void* id2);

/**
 * @brief return new filter
 *
 * @param manager manager that contains filters
 * @param mask mask from will be generated filter
 */
static DtEcsFilter* filter_new(DtEcsManager* manager, EcsMask mask);

/**
 * @brief add entity to filter
 */
static void filter_add_entity(DtEcsFilter* filter, DtEntity entity);

/**
 * @brief remove entity from filter
 */
static void filter_remove_entity(DtEcsFilter* filter, DtEntity entity);

/**
 * @brief resize filter when entities out of range
 */
static void filter_resize(DtEcsFilter* filter, size_t new_size);

/**
 * @brief free filter memory
 */
static void filter_free(DtEcsFilter*);

/**
 * @brief resize filters container if filter count out of range
 */
static void ecs_manager_resize_filters(DtEcsManager* manager);

/**
 * @brief return index of pool with id_component id
 */
static int ecs_manager_get_pool_index_by_id(DtEcsManager* manager, int id);

/**
 * @brief return index of pool with this name
 */
static int ecs_manager_get_pool_index_by_name(DtEcsManager* manager, const char* name);

/**
 * @brief resize pool container if pool count out of range
 */
static void ecs_manager_resize_pools(DtEcsManager* manager);
/**
 * @brief return filter from manager pool or create new
 */
static DtEcsFilter* get_filter(DtEcsManager* manager, EcsMask mask);

/**
 * @brief remove HierarchyDirty component from entities
 */
static void remove_hierarchy_dirty_tag(const DtEcsManager* manager);

/**
 * @brief return 1 if entity has all components pool and hasn't all exclude components
 * else return 0
 */
static int is_mask_compatible(const DtEcsManager* manager, EcsMask mask, DtEntity entity);

/**
 * @brief return 1 if entity hasn't all components pool and has all exclude components
 * else return 0
 */
static int is_mask_compatible_without(const DtEcsManager*, EcsMask, DtEntity, int);

EcsMask dt_mask_new(DtEcsManager* manager, u16 inc_size, u16 exc_size) {
    return (EcsMask) {
        .manager = manager,

        .include_pools = calloc(inc_size, sizeof(int)),
        .include_size = inc_size,
        .include_count = 0,

        .exclude_pools = calloc(inc_size, sizeof(int)),
        .exclude_size = exc_size,
        .exclude_count = 0,
    };
}

void dt_mask_inc(EcsMask* mask, u16 id) {
    if (mask->include_count == mask->include_size) {
        mask->include_count *= 2;
        void* tmp = realloc(mask->include_pools, mask->include_size * sizeof(int));

        if (!tmp) {
            printf("mask realloc failed\n");
            exit(1);
        }

        mask->include_pools = tmp;
    }

    mask->include_pools[mask->include_count++] = id;
}

void dt_mask_exc(EcsMask* mask, const u16 id) {
    if (mask->exclude_count == mask->exclude_size) {
        mask->exclude_size *= 2;
        void* tmp = realloc(mask->exclude_pools, mask->exclude_size * sizeof(int));

        if (!tmp) {
            printf("mask realloc failed\n");
            exit(1);
        }

        mask->include_pools = tmp;
    }

    mask->exclude_pools[mask->exclude_count++] = id;
}

static int cmp_pools(const void* id1, const void* id2) {
    return *(const int*) id1 - *(const int*) id2;
}

DtEcsFilter* dt_mask_end(EcsMask mask) {
    qsort(mask.include_pools, mask.include_count, sizeof(int), cmp_pools);
    qsort(mask.exclude_pools, mask.exclude_count, sizeof(int), cmp_pools);

    mask.hash = 314519;
    for (int i = 0; i < mask.include_count; i++) {
        mask.hash += mask.include_pools[i];
        mask.hash ^= mask.manager->pools[mask.include_pools[i]]->component_id;
    }

    for (int i = 0; i < mask.exclude_count; i++) {
        mask.hash -= mask.exclude_pools[i];
        mask.hash ^= mask.manager->pools[mask.exclude_pools[i]]->component_id;
    }

    return get_filter(mask.manager, mask);
}

static DtEcsFilter* filter_new(DtEcsManager* manager, EcsMask const mask) {
    DtEcsFilter* new_filter = malloc(sizeof(DtEcsFilter));

    *new_filter = (DtEcsFilter) {
        .manager = manager,
        .entities = dt_entity_container_new(sizeof(DtEntity), manager->cfg_dense_size,
                                         manager->sparse_size, manager->cfg_recycle_size,
                                         NULL, NULL),
        .mask = mask,
    };


    return new_filter;
}

static void filter_add_entity(DtEcsFilter* filter, DtEntity entity) {
    dt_entity_container_add(&filter->entities, entity, &entity);
}

static void filter_remove_entity(DtEcsFilter* filter, const DtEntity entity) {
    dt_entity_container_remove(&filter->entities, entity);
}

static void filter_resize(DtEcsFilter* filter, const size_t new_size) {
    dt_entity_container_resize(&filter->entities, new_size);
}

static void filter_free(DtEcsFilter* filter) {
    dt_entity_container_free(&filter->entities);
    free(filter);
}


DtEcsManager* dt_ecs_manager_new(const DtEcsManagerConfig* cfg) {
    DtEcsManager* manager = malloc(sizeof(DtEcsManager));
    *manager = (DtEcsManager) {
        .sparse_entities = calloc(cfg->sparce_size, sizeof(DtEntityInfo)),
        .sparse_size = cfg->sparce_size,
        .entities_ptr = 0,

        .cfg_dense_size = cfg->dense_size,
        .cfg_recycle_size = cfg->recycle_size,
        .children_size = cfg->children_size,
        .component_count = cfg->components_count,

        .recycled_entities = calloc(cfg->recycle_size, sizeof(DtEntity)),
        .recycled_size = cfg->recycle_size,
        .recycled_ptr = 0,

        .pools = calloc(cfg->pools_size, sizeof(DtEcsPool*)),
        .pools_size = cfg->pools_size,
        .pools_count = 0,

        .masks = DT_VEC_NEW(EcsMask, cfg->masks_size),
        .include_mask_count = cfg->include_mask_count,
        .exclude_mask_count = cfg->exclude_mask_count,

        .filters = calloc(cfg->filters_size, sizeof(DtEcsFilter*)),
        .filters_size = cfg->filters_size,
        .filters_count = 0,

        .filter_by_include = calloc(cfg->pools_size, sizeof(DT_VEC(DtEcsFilter*))),
        .filter_by_exclude = calloc(cfg->pools_size, sizeof(DT_VEC(DtEcsFilter*))),
    };

    manager->hierarchy_dirty_pool = DT_ECS_MANAGER_GET_POOL(manager, HierarchyDirty);

    return manager;
}

DtEntity dt_ecs_manager_new_entity(DtEcsManager* manager) {
    DtEntity entity;

    if (manager->recycled_ptr > 0) {
        entity = manager->recycled_entities[--manager->recycled_ptr];
        dt_entity_info_reuse(&manager->sparse_entities[entity]);
        printf("[DEBUG]\t recycled entity \"%d\" was created\n", entity);
    } else {
        entity = manager->entities_ptr++;
        if (entity == manager->sparse_size) {
            printf("[DEBUG]\t need to resize sparse array\n");
            manager->sparse_size = manager->sparse_size ? 2 * manager->sparse_size : 8;
            void* tmp = realloc(manager->sparse_entities,
                                manager->sparse_size * sizeof(DtEntityInfo));

            if (!tmp) {
                printf("entity add realloc failed\n");
                exit(1);
            }

            manager->sparse_entities = tmp;

            for (int i = 0; i < manager->pools_size; i++) {
                if (manager->pools[i])
                    dt_ecs_pool_resize(manager->pools[i], manager->sparse_size);
            }

            for (int i = 0; i < manager->filters_size; i++) {
                if (manager->filters[i])
                    filter_resize(manager->filters[i], manager->sparse_size);
            }
        }
        manager->sparse_entities[entity] =
            dt_entity_info_new(
            manager, entity, manager->component_count, manager->children_size);
        printf("[DEBUG]\t new entity \"%d\" was created\n", entity);
    }

    return entity;
}

DtEntity dt_ecs_manager_get_parent(const DtEcsManager* manager, const DtEntity entity) {
    if (entity < 0 || entity > manager->entities_ptr)
        return DT_ENTITY_NULL;
    if (manager->sparse_entities[entity].parent == NULL)
        return DT_ENTITY_NULL;
    return manager->sparse_entities[entity].parent->id;
}

void dt_ecs_manager_set_parent(const DtEcsManager* manager, const DtEntity child,
                               const DtEntity parent) {
    if (child < 0 || child > manager->entities_ptr)
        return;

    if (parent > manager->entities_ptr && parent != DT_ENTITY_NULL)
        return;

    if (parent == child)
        return;

    if (manager->sparse_entities[child].parent != NULL &&
        manager->sparse_entities[child].parent->id == parent)
        return;

    DtEntityInfo* parent_ptr =
        parent == DT_ENTITY_NULL ? NULL : &manager->sparse_entities[parent];

    dt_entity_info_set_parent(&manager->sparse_entities[child], parent_ptr);
}

void dt_ecs_manager_add_child(const DtEcsManager* manager, const DtEntity parent,
                              const DtEntity child) {
    if (child < 0 && child > manager->entities_ptr)
        return;
    if (parent < 0 && parent > manager->entities_ptr)
        return;

    if (parent == child)
        return;

    if (manager->sparse_entities[child].parent != NULL &&
        manager->sparse_entities[child].parent->id == parent)
        return;

    dt_entity_info_add_child(&manager->sparse_entities[parent],
                             &manager->sparse_entities[child]);
}

void dt_ecs_manager_remove_child(const DtEcsManager* manager, const DtEntity parent,
                                 const DtEntity child) {
    if (child < 0 && child > manager->entities_ptr)
        return;
    if (parent < 0 && parent > manager->entities_ptr)
        return;

    if (parent == child)
        return;

    if (manager->sparse_entities[child].parent != NULL &&
        manager->sparse_entities[child].parent->id == parent)
        return;

    dt_entity_info_remove_child(&manager->sparse_entities[parent],
                                &manager->sparse_entities[child]);
}

DtEntityInfo** dt_ecs_manager_get_children(const DtEcsManager* manager, const DtEntity entity,
                                         u16* count) {
    *count = manager->sparse_entities[entity].children_count;

    return manager->sparse_entities[entity].children;
}

void dt_ecs_manager_kill_entity(DtEcsManager* manager, const DtEntity entity) {
    if (manager->entities_ptr <= entity || entity < 0)
        return;
    if (manager->sparse_entities[entity].gen < 0)
        return;

    if (manager->recycled_ptr == manager->recycled_size) {
        printf("[DEBUG]\t need to resize recycle array\n");

        manager->recycled_size = manager->recycled_size ? 2 * manager->recycled_size : 8;
        void* tmp =
            realloc(manager->recycled_entities, manager->recycled_size * sizeof(DtEntity));

        if (!tmp) {
            printf("[DEBUG]entity kill realloc failed\n");
            exit(1);
        }

        manager->recycled_entities = tmp;
    }

    manager->recycled_entities[manager->recycled_ptr++] = entity;
    manager->sparse_entities[entity].gen *= -1;

    dt_entity_info_kill(&manager->sparse_entities[entity]);

    for (int i = 0; i < manager->pools_size; i++) {
        if (manager->pools[i] == NULL)
            continue;

        if (dt_ecs_pool_has(manager->pools[i], entity))
            dt_ecs_pool_remove(manager->pools[i], entity);
    }

    for (int i = 0; i < manager->filters_size; i++) {
        if (manager->filters[i] == NULL)
            continue;

        if (dt_entity_container_has(&manager->filters[i]->entities, entity))
            filter_remove_entity(manager->filters[i], entity);
    }

    printf("[DEBUG]\t entity \"%d\" was killed\n", entity);
}

DtEntityInfo dt_ecs_manager_get_entity(const DtEcsManager* manager, const DtEntity entity) {
    if (manager->entities_ptr <= entity || entity < 0)
        return DT_ENTITY_INFO_NULL;

    return manager->sparse_entities[entity];
}

size_t dt_ecs_manager_get_entity_components_count(const DtEcsManager* manager,
                                                  const DtEntity entity) {
    if (manager->entities_ptr < entity || entity < 0)
        return 0;
    if (manager->sparse_entities[entity].gen < 0)
        return 0;

    return manager->sparse_entities[entity].component_count;
}

uint16_t dt_ecs_manager_get_entity_gen(const DtEcsManager* manager, const DtEntity entity) {
    if (manager->entities_ptr < entity || entity < 0)
        return 0;
    if (manager->sparse_entities[entity].gen < 0)
        return 0;

    return manager->sparse_entities[entity].gen;
}

void dt_ecs_manager_copy_entity(const DtEcsManager* manager, const DtEntity dst,
                                const DtEntity src) {
    if (manager->entities_ptr < dst || dst < 0)
        return;
    if (manager->sparse_entities[dst].gen < 0)
        return;

    if (manager->entities_ptr < src || src < 0)
        return;
    if (manager->sparse_entities[src].gen < 0)
        return;

    const u16 count = manager->sparse_entities[src].component_count =
        manager->sparse_entities[dst].component_count;

    for (u16 i = 0; i < count; i++) {

    }
}

void dt_ecs_manager_entity_add_component(DtEcsManager* manager, const DtEntity entity,
                                         u16 id, const void* data) {
    if (manager->entities_ptr < entity || entity < 0)
        return;
    if (manager->sparse_entities[entity].gen < 0)
        return;

    DtEcsPool* pool = dt_ecs_manager_get_pool_by_id(manager, id);

    dt_ecs_pool_add(pool, entity, data);
}

void dt_ecs_manager_entity_remove_component(DtEcsManager* manager, const DtEntity entity,
                                            u16 id) {
    if (manager->entities_ptr < entity || entity < 0)
        return;
    if (manager->sparse_entities[entity].gen < 0)
        return;

    DtEcsPool* pool = dt_ecs_manager_get_pool_by_id(manager, id);
    dt_ecs_pool_remove(pool, entity);
}

void dt_ecs_manager_add_pool(DtEcsManager* manager, DtEcsPool* pool) {
    size_t idx = pool->component_id % manager->pools_size;
    size_t start = idx;
    while (manager->pools[idx] != NULL) {
        if (pool->component_id == manager->pools[idx]->component_id)
            return;

        idx = (idx + 1) % manager->pools_size;
        if (idx == start) {
            ecs_manager_resize_pools(manager);
            idx = pool->component_id % manager->pools_size;
        }
    }

    idx = pool->component_id % manager->pools_size;

    manager->pools[idx] = pool;
    pool->ecs_manager_id = (int) idx;
}

DtEcsPool* dt_ecs_manager_get_pool_by_id(DtEcsManager* manager, u16 id) {
    size_t idx = id % manager->pools_size;
    const size_t start = idx;

    while (manager->pools[idx] != NULL && id != manager->pools[idx]->component_id) {
        idx = (idx + 1) % manager->pools_size;
        if (idx == start) {
            ecs_manager_resize_pools(manager);
            dt_ecs_manager_add_pool(manager, dt_ecs_pool_new_by_id(manager, id));
        }
    }

    if (manager->pools[idx] == NULL) {
        DtEcsPool* pool = dt_ecs_pool_new_by_id(manager, id);
        dt_ecs_manager_add_pool(manager, pool);
        return pool;
    }

    return manager->pools[idx];
}

DtEcsPool* dt_ecs_manager_get_pool_by_name(DtEcsManager* manager, const char* name) {
    return dt_ecs_manager_get_pool_by_id(manager, dt_component_get_data_by_name(name)->id);
}

static int ecs_manager_get_pool_index_by_id(DtEcsManager* manager, const int id) {
    size_t idx = id % manager->pools_size;
    const size_t start = idx;
    while (manager->pools[idx] != NULL && id != manager->pools[idx]->component_id) {
        idx = (idx + 1) % manager->pools_size;
        if (idx == start) {
            ecs_manager_resize_pools(manager);
            dt_ecs_manager_add_pool(manager, dt_ecs_pool_new_by_id(manager, id));
            return ecs_manager_get_pool_index_by_id(manager, id);
        }
    }

    if (manager->pools[idx] == NULL) {
        dt_ecs_manager_add_pool(manager, dt_ecs_pool_new_by_id(manager, id));
    }

    return (int) idx;
}

static int ecs_manager_get_pool_index_by_name(DtEcsManager* manager, const char* name) {
    return ecs_manager_get_pool_index_by_id(manager,
                                            dt_component_get_data_by_name(name)->id);
}

static void ecs_manager_resize_pools(DtEcsManager* manager) {
    manager->pools_size *= 2;
    DtEcsPool** pools = calloc(manager->pools_size, sizeof(DtEcsPool*));

    if (pools == NULL) {
        printf("Failed to allocate memory for pools\n");
        exit(1);
    }

    void* tmp =
        realloc(manager->filter_by_include, manager->pools_size * sizeof(DtEcsFilter*));

    if (!tmp) {
        printf("[DEBUG]\t Failed to allocate memory for filters by include\n]");
        exit(1);
    }

    manager->filter_by_include = tmp;

    tmp = realloc(manager->filter_by_exclude, manager->pools_size * sizeof(DtEcsFilter*));

    if (!tmp) {
        printf("[DEBUG]\t Failed to allocate memory for filters by exclude\n]");
        exit(1);
    }

    manager->filter_by_exclude = tmp;

    for (int i = 0; i < manager->pools_count; i++) {
        size_t idx = manager->pools[i]->component_id % manager->pools_size;

        while (manager->pools[idx] != NULL) {
            idx = (idx + 1) % manager->pools_size;
        }

        pools[idx] = manager->pools[i];
        pools[idx]->ecs_manager_id = (int) idx;
    }

    free(manager->pools);
    manager->pools = pools;
}

static DtEcsFilter* get_filter(DtEcsManager* manager, const EcsMask mask) {
    const uint64_t hash = mask.hash;

    DtEcsFilter* filter = NULL;

    size_t idx = hash % manager->filters_size;
    const size_t start = idx;

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
        DT_VEC(DtEcsFilter*) list = manager->filter_by_include[mask.include_pools[i]];

        if (!list) {
            list = DT_VEC_NEW(DtEcsFilter*, mask.include_count);
            manager->filter_by_include[mask.include_pools[i]] = list;
        }

        DT_VEC_ADD(list, filter);
    }

    for (int i = 0; i < mask.exclude_count; i++) {
        DT_VEC(DtEcsFilter*) list = manager->filter_by_exclude[mask.exclude_pools[i]];

        if (!list) {
            list = DT_VEC_NEW(DtEcsFilter*, mask.exclude_count);
            manager->filter_by_exclude[mask.include_pools[i]] = list;
        }

        DT_VEC_ADD(list, filter);
    }

    for (DtEntity i = 0; i < manager->entities_ptr; i++) {
        if (manager->sparse_entities[i].gen > -1 &&
            is_mask_compatible(manager, mask, i)) {

            filter_add_entity(filter, i);
        }
    }

    return filter;
}

static void ecs_manager_resize_filters(DtEcsManager* manager) {
    manager->filters_size *= 2;
    DtEcsFilter** filters = calloc(manager->filters_size, sizeof(DtEcsFilter*));

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

void dt_on_entity_change(const DtEcsManager* manager, const DtEntity entity, const u16 id,
                         const bool added) {

    manager->sparse_entities[entity].component_count += added ? 1 : -1;

    DT_VEC(DtEcsFilter*) include_list = manager->filter_by_include[id];
    DT_VEC(DtEcsFilter*) exclude_list = manager->filter_by_exclude[id];

    if (added)
        dt_entity_info_add_component(&manager->sparse_entities[entity], id);
    else
        dt_entity_info_remove_component(&manager->sparse_entities[entity], id);


    if (added) {
        if (include_list != NULL) {
            FOREACH(DtEcsFilter*, filter, DT_VEC_ITERATOR(include_list), {
                if (is_mask_compatible(manager, filter->mask, entity)) {
                    filter_add_entity(filter, entity);
                }
            });
        }
        if (exclude_list != NULL) {
            FOREACH(DtEcsFilter*, filter, DT_VEC_ITERATOR(exclude_list), {
                if (is_mask_compatible_without(manager, filter->mask, entity, id)) {
                    filter_remove_entity(filter, entity);
                }
            });
        }
    } else {
        if (include_list != NULL) {
            FOREACH(DtEcsFilter*, filter, DT_VEC_ITERATOR(include_list), {
                if (is_mask_compatible(manager, filter->mask, entity)) {
                    filter_remove_entity(filter, entity);
                }
            });
        }
        if (exclude_list != NULL) {
            FOREACH(DtEcsFilter*, filter, DT_VEC_ITERATOR(exclude_list), {
                if (is_mask_compatible_without(manager, filter->mask, entity, id)) {
                    filter_add_entity(filter, entity);
                }
            });
        }
    }
}

static int is_mask_compatible(const DtEcsManager* manager, const EcsMask mask,
                              const DtEntity entity) {
    for (int i = 0; i < mask.include_count; i++) {
        if (!dt_ecs_pool_has(manager->pools[mask.include_pools[i]], entity)) {
            return 0;
        }
    }
    for (int i = 0; i < mask.exclude_count; i++) {
        if (dt_ecs_pool_has(manager->pools[mask.exclude_pools[i]], entity)) {
            return 0;
        }
    }
    return 1;
}

static int is_mask_compatible_without(const DtEcsManager* manager,
                                      const EcsMask filterMask, const DtEntity entity,
                                      const int id) {
    for (int i = 0; i < filterMask.include_count; i++) {
        size_t typeId = filterMask.include_pools[i];
        if (typeId == id || !dt_ecs_pool_has(manager->pools[typeId], entity)) {
            return 0;
        }
    }

    for (int i = 0; i < filterMask.exclude_count; i++) {
        size_t typeId = filterMask.exclude_pools[i];
        if (typeId != id && !dt_ecs_pool_has(manager->pools[typeId], entity)) {
            return 0;
        }
    }
    return 1;
}

void dt_ecs_manager_free(DtEcsManager* manager) {
    free(manager->sparse_entities);
    free(manager->recycled_entities);

    for (int i = 0; i < manager->pools_count; i++) {
        if (manager->pools[i] == NULL)
            continue;
        dt_ecs_pool_free(manager->pools[i]);
        dt_vec_free(manager->filter_by_include[i]);
        dt_vec_free(manager->filter_by_exclude[i]);
    }

    FOREACH(EcsMask, m, DT_VEC_ITERATOR(manager->masks), {
        free(m.include_pools);
        free(m.exclude_pools);
    });

    dt_vec_free(manager->masks);

    for (int i = 0; i < manager->filters_size; i++) {
        if (manager->filters[i] != NULL)
            filter_free(manager->filters[i]);
    }

    free(manager);
}

void dt_remove_tool_components(const DtEcsManager* manager) {
    remove_hierarchy_dirty_tag(manager);
}

static void remove_hierarchy_dirty_tag(const DtEcsManager* manager) {
    for (DtEntity i = 0; i < manager->entities_ptr; i++) {
        if (manager->sparse_entities[i].gen < 0)
            continue;

        dt_ecs_pool_remove(manager->hierarchy_dirty_pool, i);
    }
}
