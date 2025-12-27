#ifndef ECS_MANAGER_H
#define ECS_MANAGER_H

#include <stddef.h>
#include <stdint.h>
#include "../../Collections/Collections.h"
#include "../../Collections/Iterator.h"
#include "../EcsTypes.h"
#include "../Entity/Entity.h"
#include "../Pools/ECSPool.h"

/**
 * @brief contains data to initialize ecs manager
 */
typedef struct {
    size_t dense_size;
    size_t sparce_size;
    size_t recycle_size;

    size_t children_size;

    size_t components_count;

    size_t pools_size;

    size_t masks_size;

    size_t include_mask_count;
    size_t exclude_mask_count;

    size_t filters_size;
} EcsManagerConfig;

/**
 * @brief mask with data about ecs filter
 */
struct ECSMask {
    const EcsManager* manager;

    int* include_pools;
    size_t include_size;
    int include_count;

    int* exclude_pools;
    size_t exclude_size;
    int exclude_count;
    uint64_t hash;
};

/**
 * @brief return new mask
 *
 * @param manager manager that contains this mask
 * @param inc_size size of include array
 * @param exc_size size of exclude array
 * @param id id of default pool
 */
EcsMask mask_new(const EcsManager* manager, size_t inc_size, size_t exc_size, int id);

/**
 * @brief add pool to include pool
 *
 * @param mask pointer to mask
 * @param id id of component type that we add
 *
 * @warning don't call after mask_end else it throws exception
 */
void mask_inc(EcsMask* mask, int id);

/**
 * @brief add pool to exclude pool
 *
 * @param mask pointer to mask
 * @param id id of component type that we add
 *
 * @warning don't call after mask_end else it throws exception
 */
void mask_exc(EcsMask* mask, int id);

/**
 * @brief build filter from mask and return it
 *
 * @param mask pointer to mask
 */
EcsFilter* mask_end(EcsMask mask);

/**
 * @brief archetype container for entities with identical components
 */
struct ECSFilter {
    EcsManager* manager;
    EcsMask mask;

    EntityContainer entities;
};

/**
 * @brief main entity handler
 */
struct EcsManager {
    EntityInfo* sparse_entities;
    size_t sparse_size;
    Entity entities_ptr;

    Entity* parents;
    Entity* children;
    Entity children_size;

    size_t cfg_dense_size;
    size_t cfg_recycle_size;

    int* components;
    size_t components_count;

    Entity* recycled_entities;
    size_t recycled_size;
    size_t recycled_ptr;

    EcsPool** pools;
    size_t pools_size;
    size_t pools_count;

    VEC(EcsMask) masks;
    size_t include_mask_count;
    size_t exclude_mask_count;

    EcsFilter** filters;
    size_t filters_size;
    size_t filters_count;

    VEC(EcsFilter*) * filter_by_include;
    VEC(EcsFilter*) * filter_by_exclude;

    EcsPool* hierarchy_dirty_pool;
};

/**
 * @brief return ecs pool type of T from manager
 *
 * @param manager manager where we tried to find pool
 * @param T type of component
 *
 * @note if manager hasn't pool with type T create new pool and return it
 */
#define ECS_MANAGER_GET_POOL(manager, T)                                                 \
    ({                                                                                   \
        EcsPool* pool = ecs_manager_get_pool((manager), POOL_DATA(T));                   \
        if (pool == NULL) {                                                              \
            pool = ECS_POOL_NEW(T, manager);                                             \
            ecs_manager_add_pool(manager, pool);                                         \
        }                                                                                \
        pool;                                                                            \
    })

/**
 * @brief return mask where default type is T
 *
 * @param manager manager that contains this mask
 * @param T default mask type
 */
#define GET_MASK(manager, T)                                                             \
    mask_new(manager, manager->include_mask_count, manager->exclude_mask_count,          \
             (ECS_MANAGER_GET_POOL(manager, T))->info);

/**
 * @brief include type T in mask
 *
 * @param mask mask where we're including T
 * @param T type that we include
 */
#define MASK_INC(mask, T) mask_inc(&(mask), (ECS_MANAGER_GET_POOL(manager, T))->info)

/**
 * @brief exclude type T in mask
 *
 * @param mask mask where we're excluding T
 * @param T type that we exclude
 */
#define MASK_EXC(mask, T) mask_exc(&(mask), (ECS_MANAGER_GET_POOL(manager, T))->info)

/**
 * @brief return new ecs manager
 *
 * @param cfg config to initialize ecs manager
 */
EcsManager* ecs_manager_new(const EcsManagerConfig* cfg);

/**
 * @brief return new entity
 *
 * @param manager manager from we will get entity
 *
 * @note entity can be restored from recycled array
 */
Entity ecs_manager_new_entity(EcsManager* manager);

/**
 * @brief return entity data
 *
 * @param manager manager that contains entities
 * @param entity entity that we tried find
 *
 * @warning return ENTITY_INFO_NULL if entity was killed or out of range
 */
EntityInfo ecs_manager_get_entity(const EcsManager* manager, Entity entity);

/**
 * @brief return id of entity parent
 *
 * @param manager manager that contains entity
 * @param entity entity that parent we tried to find
 *
 * @warning return ENTITY_NULL(-1) if entity hasn't parent
 */
Entity ecs_manager_get_parent(const EcsManager* manager, Entity entity);

/**
 * @brief return id of entity parent
 *
 * @param manager manager that contains entities
 * @param parent new parent of child
 * @param child new child of parent
 *
 * @note add HierarchyDirty component to parent, child and old child parent if it exists
 * @note if parent already is parent of child func do nothing
 */
void ecs_manager_set_parent(EcsManager* manager, Entity parent, Entity child);

/**
 * @brief add child to parent child pool
 *
 * @param manager manager that contains entities
 * @param parent new parent of child
 * @param child new child of parent
 *
 * @note add HierarchyDirty component to parent, child and old child parent if it exists
 * @note if parent already is parent of child func do nothing
 */
void ecs_manager_add_child(EcsManager* manager, Entity parent, Entity child);

/**
 * @brief add child to parent child pool
 *
 * @param manager manager that contains entities
 * @param parent old parent of child
 * @param child old child of parent
 *
 * @note add HierarchyDirty component to parent and child
 * @note if parent already isn't parent of child func do nothing
 */
void ecs_manager_remove_child(EcsManager* manager, Entity parent, Entity child);

/**
 * @brief return pointer to child pool of entity
 *
 * @param manager manager that contains entities
 * @param entity parent of returned entities
 * @param count pointer to count value
 *
 * @note func always override count value
 */
const Entity* ecs_manager_get_children(const EcsManager* manager, Entity entity,
                                       size_t* count);

/**
 * @brief move entity to recycle array
 *
 * @param manager manager that contains entities
 * @param entity entity that we want to kill
 *
 * @note multiply gen by -1
 */
void ecs_manager_kill_entity(EcsManager* manager, Entity entity);

/**
 * @brief return count of components on entity
 *
 * @param manager manager that contains entities
 * @param entity entity that we tried find
 *
 * @note return 0 if entity was killed or out of range
 */
size_t ecs_manager_get_entity_components_count(const EcsManager* manager, Entity entity);

/**
 * @brief return count of components on entity
 *
 * @param manager manager that contains entities
 * @param entity entity that we tried find
 *
 * @note return -1 if entity was killed or out of range
 */
uint16_t ecs_manager_get_entity_gen(const EcsManager* manager, Entity entity);

/**
 * @brief copy all component data from src to dst
 *
 * @param manager manager that contains entities
 * @param dst destination
 * @param src source
 */
void ecs_manager_copy_entity(const EcsManager* manager, Entity dst, Entity src);

/**
 * @brief add entity to ecs pool with this component id
 *
 * @param manager manager that contains entities
 * @param entity entity that we're adding
 * @param id id of component type
 * @param data component data
 *
 * @note if manager hasn't ecs pool of this component, pool will be created and added to
 * manager
 */
void ecs_manager_entity_add_component(const EcsManager* manager, Entity entity, int id,
                                      void* data);

/**
 * @brief remove entity from ecs pool with this component id
 *
 * @param manager manager that contains entities
 * @param entity destination entity
 * @param id id of component type
 *
 * @note if manager hasn't ecs pool of this component func do nothing
 */
void ecs_manager_entity_remove_component(const EcsManager* manager, Entity entity,
                                         int id);

/**
 * @brief add pool to mager pools
 *
 * @param manager manager that contains pools
 * @param pool pool that we're adding
 */
void ecs_manager_add_pool(EcsManager* manager , EcsPool* pool);

/**
 * @brief return pool from manager with this id
 *
 * @param manager manager that contains pools
 * @param id id of component type
 *
 * @note if manager hasn't ecs pool of this component, pool will be created and added to
 * manager
 */
EcsPool* ecs_manager_get_pool(const EcsManager* manager, int id);

/**
 * @brief recalculate filter entities when entity was changed
 *
 * @param manager manager that contains pools
 * @param entity entity that was changed
 * @param id id of changed pool
 * @param added 1 if entity was added else 0
 *
 * @warning func must be called by changed pool else it may throw exceptions
 */
void on_entity_change(const EcsManager* manager, Entity entity, int id, int added);

/**
 * @brief free all manager data
 */
void ecs_manager_free(EcsManager* manager);

/**
 * @brief remove tool components
 *
 * @note tool components: HierarchyDirty
 */
void remove_tool_components(EcsManager* manager);

#endif
