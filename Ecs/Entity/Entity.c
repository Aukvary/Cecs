#include "Entity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../EcsTypes.h"

static void entity_container_start(void*);
static void* entity_container_current(void*);
static int entity_container_next(void*);

static void default_entity_item_reset(void* data, size_t);
static void default_entity_item_copy(void* dst, const void* src, size_t);

EntityInfo entity_info_new(const Entity id, const int component_count,
                           const int children_size) {
    return (EntityInfo) {
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
}

void entity_info_reuse(EntityInfo* info) {
    if (info->gen > 0)
        return;

    info->gen = -info->gen + 1;

    info->component_count = info->component_count;
    info->children_count = 0;
}

void entity_info_set_parent(EntityInfo* info, EntityInfo* parent) {
    if (info->parent != NULL)
        entity_info_remove_child(info->parent, info);

    if (parent == NULL)
        info->parent = NULL;
    else
        entity_info_add_child(parent, info);
}

void entity_info_add_child(EntityInfo* info, EntityInfo* child) {
    if (info->children == NULL) {
        info->children = calloc(info->base_children_size, sizeof(EntityInfo*));
    }

    for (int i = 0; i < info->children_count + 1; i++) {
        if (info->children[i]->id == child->id)
            return;
        if (i == info->children_size) {
            info->children_size *= 2;
            void* tmp =
                realloc(info->children, info->children_size * sizeof(EntityInfo*));

            if (!tmp) {
            }

            info->children = tmp;
        }

        if (i == info->children_count) {
            info->children[i] = child;
            return;
        }
    }
}

void entity_info_remove_child(EntityInfo* info, EntityInfo* child) {
    for (int i = 0; i < info->children_count; i++) {
        if (info->children[i]->id != child->id)
            continue;

        info->children[i] = info->children[--info->children_count];
        child->parent = NULL;
    }
}

void entity_info_remove_all_children(EntityInfo* info) {
    for (int i = 0; i < info->children_count; i++) {
        info->children[i]->parent = NULL;
    }

    info->children_count = 0;
}

void entity_info_add_component(EntityInfo* info, int id) {
    for (int i = 0; i < info->component_count + 1; i++) {
        if (info->components[i] == id)
            return;

        if (i == info->component_size) {
            info->component_size *= 2;
            void* tmp = realloc(info->components, info->component_size * sizeof(int));

            if (!tmp) {
            }

            info->components = tmp;
        }

        if (i == info->component_count) {
            info->components[i] = id;
            return;
        }
    }
}

void entity_info_remove_component(EntityInfo* info, int id) {
    for (int i = 0; i < info->component_count; i++) {
        if (info->components[i] != id)
            continue;

        info->components[i] = info->components[--info->component_count];
    }
}

void entity_info_reset(EntityInfo* info) {
    if (info->gen < 0) return;

    entity_info_remove_all_children(info);
    entity_info_set_parent(info, NULL);
    info->component_count = 0;
    info->gen++;
}

void entity_info_copy(EntityInfo* dst, EntityInfo* src) {
    if (dst->gen < 0) return;
    if (src->gen < 0) return;

    if (dst->component_size < src->component_size) {
        dst->component_size = src->component_size;

        void* tmp = realloc(dst->components, dst->component_size * sizeof(int));

        if (!tmp) { }

        dst->components = tmp;
    }

    memcpy(dst->components, src->components, dst->component_size * sizeof(int));
}

void entity_info_kill(EntityInfo* info) {
    if (info->gen < 0) return;

    entity_info_remove_all_children(info);
    entity_info_set_parent(info, NULL);
    info->gen *= -1;
}

EntityContainer entity_container_new(const size_t item_size, const size_t dense_size,
                                     const size_t sparse_size, const size_t recycle_size,
                                     const ResetItemHandler auto_reset,
                                     const CopyItemHandler auto_copy) {
    EntityContainer ec = {
        .dense_items = calloc(dense_size, item_size),
        .dense_entities = calloc(dense_size, sizeof(Entity)),
        .item_size = item_size,
        .dense_ptr = 0,
        .dense_size = dense_size,
        .count = 0,

        .sparce_entities = calloc(sparse_size, sizeof(Entity)),
        .sparse_size = sparse_size,

        .recycle_entities = calloc(recycle_size, sizeof(Entity)),
        .recycle_ptr = 0,
        .recycle_size = recycle_size,

        .auto_reset = auto_reset ? auto_reset : default_entity_item_reset,
        .auto_copy = auto_copy ? auto_copy : default_entity_item_copy,

        .iterator =
            (Iterator) {
                .start = entity_container_start,
                .current = entity_container_current,
                .next = entity_container_next,
            },
    };

    for (size_t i = 0; i < sparse_size; i++) {
        ec.sparce_entities[i] = ENTITY_NULL;
    }

    return ec;
}

void entity_container_add(EntityContainer* container, const Entity entity,
                          const void* data) {
    if (entity > container->sparse_size)
        return;
    if (entity_container_has(container, entity))
        return;

    Entity e;

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
                          container->dense_size * sizeof(Entity));

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

void entity_container_remove(EntityContainer* container, const Entity entity) {
    if (entity > container->sparse_size)
        return;
    if (!entity_container_has(container, entity))
        return;

    Entity dense_index = container->sparce_entities[entity];
    Entity last_index = (Entity) (container->dense_ptr - 1);

    if (dense_index != last_index) {
        Entity last_entity = container->dense_entities[last_index];

        container->auto_copy(
            (char*) container->dense_items + dense_index * container->item_size,
            (char*) container->dense_items + last_index * container->item_size,
            container->item_size);

        container->dense_entities[dense_index] = last_entity;
        container->sparce_entities[last_entity] = dense_index;
    }

    container->sparce_entities[entity] = ENTITY_NULL;
    container->dense_ptr--;
    container->count--;

    if (container->recycle_ptr >= container->recycle_size) {
        container->recycle_size =
            container->recycle_size ? 2 * container->recycle_size : 8;
        void* tmp = realloc(container->recycle_entities,
                            container->recycle_size * sizeof(Entity));

        if (!tmp) {
            printf("[DEBUG] entity container realloc exception]\n");
            exit(1);
        }

        container->recycle_entities = tmp;
    }

    container->recycle_entities[container->recycle_ptr++] = entity;
}

inline int entity_container_has(const EntityContainer* container, const Entity entity) {
    return entity < container->sparse_size &&
           container->sparce_entities[entity] != ENTITY_NULL;
}

void* entity_container_get(const EntityContainer* container, const Entity entity) {
    if (entity > container->sparse_size)
        return NULL;

    if (container->sparce_entities[entity] == ENTITY_NULL) {
        return NULL;
    }

    return (char*) container->dense_items +
           container->sparce_entities[entity] * container->item_size;
}

void entity_container_resize(EntityContainer* container, const size_t new_size) {
    void* tmp = realloc(container->sparce_entities, new_size);

    if (!tmp) {
        printf("[DEBUG]memory allocation exception");
        exit(1);
    }

    container->sparce_entities = tmp;
}

static void default_entity_item_reset(void* data, size_t size) { memset(data, 0, size); }

void entity_container_free(EntityContainer* container) {
    free(container->dense_items);
    free(container->dense_entities);
    free(container->sparce_entities);
    free(container->recycle_entities);
}

static void default_entity_item_copy(void* dst, const void* src, size_t size) {
    memcpy(dst, src, size);
}

static void entity_container_start(void* data) {
    ((EntityContainer*) data)->iterator_ptr = 0;
}

static void* entity_container_current(void* data) {
    EntityContainer* container = data;
    return &container->dense_entities[container->iterator_ptr];
}

static int entity_container_next(void* data) {
    EntityContainer* container = data;
    return container->iterator_ptr++ < container->count; // lil popA
}
