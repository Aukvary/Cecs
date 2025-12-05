#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>
#include "../EcsTypes.h"

#include "../../Collections/Iterator.h"

#define ENTITY_NULL (-1)
#define ENTITY_INFO_NULL                                                                 \
    (EntityInfo) { .id = ENTITY_NULL }

typedef struct {
    Entity id;
    size_t component_count;
    int gen;
} EntityInfo;

struct EntityContainer {
    void* dense_items;
    Entity* dense_entities;
    size_t item_size;
    Entity dense_ptr;
    size_t dense_size;
    size_t count;

    Entity* sparce_entities;
    size_t sparse_size;

    Entity* recycle_entities;
    size_t recycle_ptr;
    size_t recycle_size;

    Iterator iterator;
    size_t iterator_ptr;

    ResetItemHandler auto_reset;
    CopyItemHandler auto_copy;
};

EntityContainer entity_container_new(size_t, size_t, size_t, size_t, ResetItemHandler,
                                     CopyItemHandler);

Entity entity_container_push(EntityContainer*);

void entity_container_add(EntityContainer*, Entity, const void*);

void entity_container_remove(EntityContainer*, Entity);

int entity_container_has(const EntityContainer*, Entity);

void* entity_container_get(const EntityContainer*, Entity);

int get_container_entities(EntityContainer*, Entity*, size_t);

int get_container_items(EntityContainer*, void**, size_t);

void entity_container_resize(EntityContainer*, size_t);

void entity_container_free(EntityContainer*);

#endif
