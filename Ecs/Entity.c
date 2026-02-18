#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DtEcs.h"

static void entity_container_start(void*);
static void* entity_container_current(void*);
static int entity_container_next(void*);

static void default_entity_item_reset(void* data, size_t);
static void default_entity_item_copy(void* dst, const void* src, size_t);

DtEntityInfo dt_entity_info_new(DtEcsManager* manager, const DtEntity id,
                                const u16 component_count, const DtEntity children_size) {
    DtEntityInfo info = (DtEntityInfo) {
        .manager = manager,

        .id = id,

        .components = calloc(component_count, sizeof(int)),
        .component_size = component_count,
        .component_count = 0,

        .parent = NULL,

        .children = NULL,
        .children_size = children_size,
        .base_children_size = children_size,
        .children_count = 0,

        .gen = 1,
    };

    return info;
}

void dt_entity_info_reuse(DtEntityInfo* info) {
    if (info->gen > 0)
        return;

    info->gen = -info->gen + 1;

    info->component_count = info->component_count;
    info->children_count = 0;
}

void dt_entity_info_set_parent(DtEntityInfo* info, DtEntityInfo* parent) {
    if (info->parent != NULL)
        dt_entity_info_remove_child(info->parent, info);

    if (parent == NULL)
        info->parent = NULL;
    else
        dt_entity_info_add_child(parent, info);
}

void dt_entity_info_add_child(DtEntityInfo* info, DtEntityInfo* child) {
    if (info->children == NULL) {
        info->children = calloc(info->base_children_size, sizeof(DtEntityInfo*));
    }

    for (int i = 0; i < info->children_count + 1; i++) {
        if (i == info->children_size) {
            info->children_size = info->children_size ? info->children_size * 2 : 10;
            void* tmp =
                realloc(info->children, info->children_size * sizeof(DtEntityInfo*));

            if (!tmp) {
                printf("[DEBUG] Memory allocation exception");
            }

            info->children = tmp;
        }

        if (i == info->children_count) {
            info->children[i] = child;
            child->parent = info;
            info->children_count++;
            return;
        }

        if (info->children[i]->id == child->id)
            return;
    }
}

void dt_entity_info_remove_child(DtEntityInfo* info, DtEntityInfo* child) {
    for (int i = 0; i < info->children_count; i++) {
        if (info->children[i]->id != child->id)
            continue;

        info->children[i] = info->children[--info->children_count];
        child->parent = NULL;
    }
}

void dt_entity_info_remove_all_children(DtEntityInfo* info) {
    for (int i = 0; i < info->children_count; i++) {
        info->children[i]->parent = NULL;
    }

    info->children_count = 0;
}

void dt_entity_info_add_component(DtEntityInfo* info, u16 id) {
    for (int i = 0; i < info->component_count + 1; i++) {
        if (info->components[i] == id)
            return;

        DtEntityInfo inf = *info;


        if (i == info->component_size) {
            info->component_size = info->component_size ? info->component_size * 2 : 10;
            void* tmp = realloc(info->components, info->component_size * sizeof(int));

            if (!tmp) {
                printf("[DEBUG] entity info realloc exception\n");
            }

            info->components = tmp;
        }

        if (i == info->component_count) {
            info->components[i] = id;
            return;
        }
    }
}

void dt_entity_info_remove_component(DtEntityInfo* info, u16 id) {
    for (int i = 0; i < info->component_count; i++) {
        if (info->components[i] != id)
            continue;

        info->components[i] = info->components[--info->component_count];
    }
}

void dt_entity_info_reset(DtEntityInfo* info) {
    if (info->gen < 0)
        return;

    for (int i = 0; i < info->component_count; i++) {
        DtEcsPool* pool = info->manager->pools_table[info->components[i]];
        dt_ecs_pool_reset(pool, info->id);
    }
}

void dt_entity_info_clear(DtEntityInfo* info) {
    if (info->gen < 0)
        return;

    for (int i = 0; i < info->component_count; i++) {
        DtEcsPool* pool = info->manager->pools_table[info->components[i]];
        dt_ecs_pool_remove(pool, info->id);
    }

    info->component_count = 0;
}

void dt_entity_info_copy(DtEntityInfo* dst, const DtEntityInfo* src) {
    if (dst->gen < 0)
        return;
    if (src->gen < 0)
        return;

    if (dst->component_size < src->component_size) {
        dst->component_size = src->component_size;

        void* tmp = realloc(dst->components, dst->component_size * sizeof(int));

        if (!tmp) {
            printf("[DEBUG] entity info realloc exception]\n");
        }

        dst->components = tmp;
    }

    memcpy(dst->components, src->components, dst->component_size * sizeof(int));

    for (int i = 0; i < dst->component_count; i++) {
        DtEcsPool* pool = dst->manager->pools_table[dst->components[i]];
        dt_ecs_pool_copy(pool, dst->id, src->id);
    }
}

void dt_entity_info_kill(DtEntityInfo* info) {
    if (info->gen < 0)
        return;

    dt_entity_info_remove_all_children(info);
    dt_entity_info_set_parent(info, NULL);
    info->gen *= -1;
}

DtEntityContainer dt_entity_container_new(u32 item_size, DtEntity dense_size,
                                          DtEntity sparse_size, DtEntity recycle_size,
                                          const DtResetItemHandler auto_reset,
                                          const DtCopyItemHandler auto_copy) {
    DtEntityContainer ec = {
        .dense_items = calloc(dense_size, item_size),
        .dense_entities = calloc(dense_size, sizeof(DtEntity)),
        .item_size = item_size,
        .dense_ptr = 0,
        .dense_size = dense_size,
        .count = 0,

        .sparce_entities = calloc(sparse_size, sizeof(DtEntity)),
        .sparse_size = sparse_size,

        .recycle_entities = calloc(recycle_size, sizeof(DtEntity)),
        .recycle_ptr = 0,
        .recycle_size = recycle_size,

        .auto_reset = auto_reset ? auto_reset : default_entity_item_reset,
        .auto_copy = auto_copy ? auto_copy : default_entity_item_copy,

        .iterator =
            (DtIterator) {
                .start = entity_container_start,
                .current = entity_container_current,
                .next = entity_container_next,
            },
    };

    for (size_t i = 0; i < sparse_size; i++) {
        ec.sparce_entities[i] = DT_ENTITY_NULL;
    }

    return ec;
}

void dt_entity_container_add(DtEntityContainer* container, const DtEntity entity,
                             const void* data) {
    if (entity > container->sparse_size)
        return;
    if (dt_entity_container_has(container, entity))
        return;

    DtEntity e;

    if (container->recycle_ptr) {
        e = container->recycle_entities[container->recycle_ptr--];
    } else {
        if (container->dense_ptr == container->dense_size) {
            container->dense_size *= 2;
            void* tmp = realloc(container->dense_items,
                                container->dense_size * container->item_size);

            if (!tmp) {
                printf("[DEBUG] entity container realloc exception]\n");
                exit(1);
            }

            container->dense_items = tmp;

            tmp = realloc(container->dense_entities,
                          container->dense_size * sizeof(DtEntity));

            if (!tmp) {
                printf("[DEBUG] entity container realloc exception]\n");
                exit(1);
            }

            container->dense_entities = tmp;
        }

        e = container->dense_ptr++;
    }

    void* target = (char*) container->dense_items + e * container->item_size;

    if (data) {
        container->auto_copy(target, data, container->item_size);
    } else {
        container->auto_reset(target, container->item_size);
    }

    container->sparce_entities[entity] = e;
    container->dense_entities[e] = entity;
    container->count++;
}

void dt_entity_container_remove(DtEntityContainer* container, const DtEntity entity) {
    if (entity > container->sparse_size)
        return;
    if (!dt_entity_container_has(container, entity))
        return;

    DtEntity dense_index = container->sparce_entities[entity];
    DtEntity last_index = (DtEntity) (container->dense_ptr - 1);

    if (dense_index != last_index) {
        DtEntity last_entity = container->dense_entities[last_index];

        container->auto_copy(
            (u8*) container->dense_items + dense_index * container->item_size,
            (u8*) container->dense_items + last_index * container->item_size,
            container->item_size);

        container->dense_entities[dense_index] = last_entity;
        container->sparce_entities[last_entity] = dense_index;
    }

    container->sparce_entities[entity] = DT_ENTITY_NULL;
    container->dense_ptr--;
    container->count--;

    if (container->recycle_ptr >= container->recycle_size) {
        container->recycle_size =
            container->recycle_size ? 2 * container->recycle_size : 8;
        void* tmp = realloc(container->recycle_entities,
                            container->recycle_size * sizeof(DtEntity));

        if (!tmp) {
            printf("[DEBUG] entity container realloc exception]\n");
            exit(1);
        }

        container->recycle_entities = tmp;
    }

    container->recycle_entities[container->recycle_ptr++] = entity;
}

inline int dt_entity_container_has(const DtEntityContainer* container,
                                   const DtEntity entity) {
    return entity < container->sparse_size &&
           container->sparce_entities[entity] != DT_ENTITY_NULL;
}

void* dt_entity_container_get(const DtEntityContainer* container, const DtEntity entity) {
    if (entity > container->sparse_size)
        return NULL;

    if (container->sparce_entities[entity] == DT_ENTITY_NULL) {
        return NULL;
    }

    return (char*) container->dense_items +
           container->sparce_entities[entity] * container->item_size;
}

void dt_entity_container_reset(DtEntityContainer* container, const DtEntity entity) {
    if (!dt_entity_container_has(container, entity))
        return;

    container->auto_reset(&container->dense_items[container->sparce_entities[entity]],
                          container->item_size);
}

void dt_entity_container_copy(DtEntityContainer* container, const DtEntity dst,
                              const DtEntity src) {
    if (!dt_entity_container_has(container, src))
        return;

    if (dt_entity_container_has(container, dst))
        container->auto_copy(&container->dense_items[container->sparce_entities[dst]],
                             &container->dense_items[container->sparce_entities[src]],
                             container->item_size);
    else
        dt_entity_container_add(container, dst,
                                &container->dense_items[container->sparce_entities[src]]);
}

void dt_entity_container_resize(DtEntityContainer* container, u16 new_size) {
    void* tmp = realloc(container->sparce_entities, new_size);

    if (!tmp) {
        printf("[DEBUG]memory allocation exception");
        exit(1);
    }

    container->sparce_entities = tmp;
}

static void default_entity_item_reset(void* data, const size_t size) {
    memset(data, 0, size);
}

void dt_entity_container_free(DtEntityContainer* container) {
    free(container->dense_items);
    free(container->dense_entities);
    free(container->sparce_entities);
    free(container->recycle_entities);
}

static void default_entity_item_copy(void* dst, const void* src, const size_t size) {
    memcpy(dst, src, size);
}

static void entity_container_start(void* data) {
    ((DtEntityContainer*) data)->iterator_ptr = 0;
}

static void* entity_container_current(void* data) {
    const DtEntityContainer* container = data;
    return &container->dense_entities[container->iterator_ptr];
}

static int entity_container_next(void* data) {
    DtEntityContainer* container = data;
    return container->iterator_ptr++ < container->count; // lil popA
}
