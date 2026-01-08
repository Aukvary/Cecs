#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>
#include "../EcsTypes.h"

#include "../../Collections/Iterator.h"

/**
 * @brief id of NULL entity
 */
#define ENTITY_NULL (-1)

/**
 * @brief Entity info with NULL id
 */
#define ENTITY_INFO_NULL                                                                 \
    (EntityInfo) { .id = ENTITY_NULL }

/**
 * @brief entity data in ecs manager
 *
 * @note if gen < 0 entity was killed
 */

typedef struct EntityInfo{
    Entity id;
    int* components;
    int component_size;
    int component_count;

    struct EntityInfo* parent;
    struct EntityInfo** children;
    int children_size;
    int base_children_size;
    int children_count;

    int gen;
} EntityInfo;

EntityInfo entity_info_new(Entity id, int component_count, int children_size);
void entity_info_reuse(EntityInfo* info);
void entity_info_set_parent(EntityInfo* info, EntityInfo* parent);
void entity_info_add_child(EntityInfo* info, EntityInfo* child);
void entity_info_remove_child(EntityInfo* info, EntityInfo* child);
void entity_info_remove_all_children(EntityInfo* info);

void entity_info_add_component(EntityInfo* info, int id);
void entity_info_remove_component(EntityInfo* info, int id);

void entity_info_reset(EntityInfo* info);
void entity_info_copy(EntityInfo* dst, EntityInfo* src);

void entity_info_kill(EntityInfo* info);

/**
 * @brief data struct for contains entity
 *
 * @note use dense_items/dense_entities and count to iteration
 * @note implements an ID recycled system for reuse
 */
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

/**
 * @brief return new entity container
 *
 * @param item_size size of item
 * @param dense_size size of dense array
 * @param sparse_size size of sparse array
 * @param recycle_size size of recycled array
 * @param auto_reset func to reset data
 * @param auto_copy func to copy data
 *
 * @return new entity container
 *
 * @note dense size can be smaller than sparse size
 * @note sparse_size must be equals sparse_size of ecs manager else it can throw
 * exceptions
 * @note component data is zero-initialized by default
 * @note component data is copied using memcpy func by default
 */
EntityContainer entity_container_new(size_t item_size, size_t dense_size,
                                     size_t sparse_size, size_t recycle_size,
                                     ResetItemHandler auto_reset,
                                     CopyItemHandler auto_copy);

/**
 * @brief add entity to container
 *
 * @param container container with data
 * @param entity entity that we will add
 * @param data data to write in container
 *
 * @note data can be NULL
 * @note if data isn't NULL it override reset func
 */
void entity_container_add(EntityContainer* container, Entity entity, const void* data);

/**
 * @brief containing entity in container
 *
 * @param container container with data
 * @param entity entity
 *
 * @return 1 if container has entity else return 0
 */
int entity_container_has(const EntityContainer* container, Entity entity);

/**
 * @brief return entity data
 *
 * @param container container with data
 * @param entity entity that we try found
 *
 * @return pointer to entity data
 *
 * @warning return NULL if container hasn't entity
 */
void* entity_container_get(const EntityContainer* container, Entity entity);

/**
 * @brief return entity data
 *
 * @param container container with data
 * @param entity entity that we try get
 */
void entity_container_remove(EntityContainer* container, Entity entity);

/**
 * @brief resize container when entity count out of range
 *
 * @param container container with data
 * @param size new container size
 *
 * @warning func must be called by wrapper else it can throw exceptions
 */
void entity_container_resize(EntityContainer* container, size_t size);

/**
 * @brief free container memory
 *
 * @param container container with data
 *
 * @warning func must be called by wrapper else it can throw exceptions
 */
void entity_container_free(EntityContainer* container);

#endif
