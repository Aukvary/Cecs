#ifndef DT_ECS_H
#define DT_ECS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef STACK_ALLOC
#if defined(_WIN32) || defined(_WIN64)
#include <malloc.h>
#else
#include <alloca.h>
#endif

#define STACK_ALLOC(size) alloca((size))
#endif

#include "Collections/Collections.h"

#ifndef DT_INT_TYPES
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
#endif

typedef u16 Entity;

/**
 * @brief func pointer to handle reset operation in EntityContainer
 *
 * @param data pointer to data
 * @param size size of data
 */
typedef void (*ResetItemHandler)(void* data, size_t size);

/**
 * @brief func pointer to handle copy operation in EntityContainer
 *
 * @param dst pointer to destination
 * @param src pointer to source
 * @param size size of data
 */
typedef void (*CopyItemHandler)(void* dst, const void* src, size_t size);

#define ENTITY_NULL (0xFFFF)

/**
 * @brief Entity info with NULL id
 */
#define ENTITY_INFO_NULL                                                                 \
    (EntityInfo) { .id = ENTITY_NULL }

/**
 * @brief main entity handler
 */
typedef struct DtEcsManager DtEcsManager;
typedef struct DtEcsFilter DtEcsFilter;

typedef struct EntityInfo {
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

EntityInfo dt_entity_info_new(Entity id, u16 component_count, Entity children_size);
void dt_entity_info_reuse(EntityInfo* info);
void dt_entity_info_set_parent(EntityInfo* info, EntityInfo* parent);
void dt_entity_info_add_child(EntityInfo* info, EntityInfo* child);
void dt_entity_info_remove_child(EntityInfo* info, EntityInfo* child);
void dt_entity_info_remove_all_children(EntityInfo* info);

void dt_entity_info_add_component(EntityInfo* info, u16 id);
void dt_entity_info_remove_component(EntityInfo* info, u16 id);

void dt_entity_info_reset(EntityInfo* info);
void dt_entity_info_copy(EntityInfo* dst, EntityInfo* src);

void dt_entity_info_kill(EntityInfo* info);

/**
 * @brief data struct for contains entity
 *
 * @note use dense_items/dense_entities and count to iteration
 * @note implements an ID recycled system for reuse
 */
typedef struct EntityContainer {
    void* dense_items;
    Entity* dense_entities;
    u32 item_size;
    Entity dense_ptr;
    Entity dense_size;
    Entity count;

    Entity* sparce_entities;
    Entity sparse_size;

    Entity* recycle_entities;
    Entity recycle_ptr;
    Entity recycle_size;

    Iterator iterator;
    Entity iterator_ptr;

    ResetItemHandler auto_reset;
    CopyItemHandler auto_copy;
} EntityContainer;

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
EntityContainer entity_container_new(u32 item_size, Entity dense_size, Entity sparse_size,
                                     Entity recycle_size, ResetItemHandler auto_reset,
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
void entity_container_resize(EntityContainer* container, u16 size);

/**
 * @brief free container memory
 *
 * @param container container with data
 *
 * @warning func must be called by wrapper else it can throw exceptions
 */
void entity_container_free(EntityContainer* container);


/**
 * @brief contains data to initialize ecs manager
 */
typedef struct DtEcsManagerConfig {
    u16 dense_size;
    u16 sparce_size;
    u16 recycle_size;

    u16 children_size;

    u16 components_count;

    u16 pools_size;

    u32 masks_size;

    u32 include_mask_count;
    u32 exclude_mask_count;

    u32 filters_size;
} DtEcsManagerConfig;

/**
 * @brief contains components with data
 */
typedef struct ComponentPool ComponentPool;

/**
 * @brief contains components without data
 */
typedef struct TagPool TagPool;

/**
 * @brief contains entity component data
 */
typedef struct EcsPool {
    const DtEcsManager* manager;

    u16 count;

    const char* name;
    u16 component_id;
    u16 ecs_manager_id;

    void* data;

    void (*add)(void*, Entity, const void*);
    void* (*get)(const void*, Entity);
    bool (*has)(const void*, Entity);
    void (*remove)(void*, Entity);
    void (*resize)(void*, size_t);
    void (*free)(void*);
} EcsPool;

/**
 * @brief create new ecs pool
 *
 * @param T type of pool
 * @param manager ecs manager that contains this pool
 * @return new ecs pool
 */
#define ECS_POOL_NEW(T, manager) ecs_pool_new(manager, #T, sizeof(T))

/**
 * @brief create new ecs pool
 *
 * @param manager ecs manager that contains this pool
 * @param name name of pool type
 * @param size size of pool type
 * @return new ecs pool
 */
EcsPool* ecs_pool_new(const DtEcsManager* manager, const char* name, u16 size);

/**
 * @brief return new pool with this type id
 *
 * @param manager ecs manager that contains this pool
 * @param id id of component type
 * @return new ecs pool
 */
EcsPool* ecs_pool_new_by_id(const DtEcsManager* manager, u16 id);

/**
 * @brief return new component pool
 *
 * @param manager ecs manager that contains this pool
 * @param name name of pool type
 * @param size size of pool type
 * @param reset_handler func to handle reset component
 * @param copy_handler func to handle copy component
 *
 * @return new component pool
 *
 * @note component data is zero-initialized by default
 * @note component data is copied using memcpy func by default
 */
EcsPool* component_pool_new(const DtEcsManager* manager, const char* name, u16 size,
                            ResetItemHandler reset_handler, CopyItemHandler copy_handler);

/**
 * @brief return component pool
 *
 * @param manager ecs manager that contains this pool
 * @param name name of pool type
 *
 * @return new tag pool
 */
EcsPool* tag_pool_new(const DtEcsManager* manager, const char* name);

/**
 * @brief add entity to ecs pool
 *
 * @param pool pool where will be added entity
 * @param entity entity that will be added to pool
 * @param data pointer to component data
 *
 * @note if ecs pool type is tag pool, data must be NULL
 */
void ecs_pool_add(EcsPool* pool, Entity entity, const void* data);

/**
 * @brief return pointer to component data
 *
 * @param pool pool that contains entity component data
 * @param entity entity that will be added to pool
 *
 * @return pointer to component data
 *
 * @note return NULL if pool haven't entity
 */
void* ecs_pool_get(const EcsPool* pool, Entity entity);

/**
 * @brief return 1 if pool contains entity else 0
 *
 * @param pool pool that contains(or not) entity
 * @param entity entity that will be added to pool
 *
 * @return 1 if pool contains entity else 0
 */
int ecs_pool_has(const EcsPool* pool, Entity entity);

/**
 * @brief remove entity from pool
 *
 * @param pool pool that contains entity component data
 * @param entity entity that will be added to pool
 */
void ecs_pool_remove(EcsPool* pool, Entity entity);

/**
 * @brief resize pool container if count entity count out of range
 *
 * @param pool pool from ecs manager
 * @param size new pool size
 *
 * @return pointer to component data
 *
 * @note this func must be called by ecs manager else it can throw exception
 */
void ecs_pool_resize(EcsPool* pool, u64 size);

/**
 * @brief free pool memory
 *
 * @param pool pool from ecs manager
 *
 * @return pointer to component data
 *
 * @note this func must be called by ecs manager else it can throw exception
 */
void ecs_pool_free(EcsPool* pool);

/**
 * @brief metadata of component and components field
 */
typedef struct ComponentAttribute {
    const char* name;
    u64 data_size;
    void* data;
} ComponentAttribute;

/**
 * @brief data of serialized component
 */
typedef struct ComponentData {
    char* name;
    u16 id;

    u64 component_size;

    u16 attribute_count;
    ComponentAttribute* attributes;
} ComponentData;


/**
 * @brief register component in global pool
 *
 * @param data pointer to generated data variable
 *
 * @note this func must be called by __attribute__((constructor)) func else may be
 * exceptions
 */
void register_component(ComponentData* data);

/**
 * @brief get component name by id
 *
 * @param id id of component
 *
 * @note if global pool hasn't this id return NULL
 */
const ComponentData* component_get_data_by_id(u16 id);

/**
 * @brief get component id by name
 *
 * @param name name of component
 *
 * @note if global pool hasn't this name return -1
 */
const ComponentData* component_get_data_by_name(const char* name);

/**
 * @brief register component in global pool
 *
 * @param component_name name of component type
 * @param ... attributes of component type
 *
 * generate funcs for register component before called main func and static variables for
 * store component metadata
 */
#define REGISTER_COMPONENT(component_name, ...)                                          \
    typedef struct component_name component_name;                                        \
    static ComponentAttribute component_name##_attrs[] = {__VA_ARGS__};                  \
    static int component_name##_attrs_count =                                            \
        sizeof(component_name##_attrs) / sizeof(component_name##_attrs[0]);              \
    static ComponentData _##component_name##_data;                                       \
    static __attribute__((constructor)) void component_name##_register_component(void) { \
        _##component_name##_data = (ComponentData) {                                     \
            .name = #component_name,                                                     \
            .attributes = component_name##_attrs,                                        \
            .attribute_count = component_name##_attrs_count,                             \
        };                                                                               \
        register_component(&_##component_name##_data);                                   \
    }                                                                                    \
    inline ComponentData component_name##_data() { return _##component_name##_data; }

struct HierarchyDirty {};

REGISTER_COMPONENT(HierarchyDirty)

/**
 * @brief mask with data about ecs filter
 */
typedef struct DtEcsMask {
    DtEcsManager* manager;

    u16* include_pools;
    u16 include_size;
    u16 include_count;

    u16* exclude_pools;
    u16 exclude_size;
    u16 exclude_count;
    u64 hash;
} EcsMask;

/**
 * @brief return new mask
 *
 * @param manager manager that contains this mask
 * @param inc_size size of include array
 * @param exc_size size of exclude array
 */
EcsMask dt_mask_new(DtEcsManager* manager, u16 inc_size, u16 exc_size);

/**
 * @brief add pool to include pool
 *
 * @param mask pointer to mask
 * @param id index of pool in ecs manager
 *
 * @warning don't call after mask_end else it throws exception
 */
void dt_mask_inc(EcsMask* mask, u16 id);

/**
 * @brief add pool to exclude pool
 *
 * @param mask pointer to mask
 * @param id index of pool in ecs manager
 *
 * @warning don't call after mask_end else it throws exception
 */
void dt_mask_exc(EcsMask* mask, u16 id);

/**
 * @brief build filter from mask and return it
 */
DtEcsFilter* dt_mask_end(EcsMask mask);

/**
 * @brief archetype container for entities with identical components
 */
struct DtEcsFilter {
    DtEcsManager* manager;
    EcsMask mask;

    EntityContainer entities;
};

struct DtEcsManager {
    EntityInfo* sparse_entities;
    Entity sparse_size;
    Entity entities_ptr;

    Entity cfg_dense_size;
    Entity cfg_recycle_size;
    u16 component_count;
    Entity children_size;

    Entity* recycled_entities;
    Entity recycled_size;
    Entity recycled_ptr;

    EcsPool** pools;
    size_t pools_size;
    size_t pools_count;

    DT_VEC(EcsMask) masks;
    size_t include_mask_count;
    size_t exclude_mask_count;

    DtEcsFilter** filters;
    size_t filters_size;
    size_t filters_count;

    DT_VEC(DtEcsFilter*) * filter_by_include;
    DT_VEC(DtEcsFilter*) * filter_by_exclude;

    EcsPool* hierarchy_dirty_pool;
};

/**
 * @brief return ecs pool type of T from manager
 *
 * @param manager manager where we tried to find pool
 * @param T type of component
 *
 * @note if manager didn't have pool with type T create new pool and return it
 */
#define DT_ECS_MANAGER_GET_POOL(manager, T)                                              \
    ({ dt_ecs_manager_get_pool_by_name((manager), #T); })

/**
 * @brief return mask where default type is T
 *
 * @param manager manager that contains this mask
 * @param T default mask type
 */
#define DT_GET_MASK(manager, T)                                                          \
    dt_mask_new(manager, manager->include_mask_count, manager->exclude_mask_count,       \
                (DT_ECS_MANAGER_GET_POOL(manager, T))->info);

/**
 * @brief include type T in mask
 *
 * @param mask mask where we're including T
 * @param T type that we include
 */
#define DT_MASK_INC(mask, T)                                                             \
    dt_mask_inc(&(mask), (DT_ECS_MANAGER_GET_POOL(manager, T))->ecs_manager_id)

/**
 * @brief exclude type T in mask
 *
 * @param mask mask where we're excluding T
 * @param T type that we exclude
 */
#define DT_MASK_EXC(mask, T)                                                             \
    dt_mask_exc(&(mask), (DT_ECS_MANAGER_GET_POOL(manager, T))->ecs_manager_id)

/**
 * @brief return new ecs manager
 *
 * @param cfg config to initialize ecs manager
 */
DtEcsManager* dt_ecs_manager_new(const DtEcsManagerConfig* cfg);

/**
 * @brief return new entity
 *
 * @param manager manager from we will get entity
 *
 * @note entity can be restored from recycled array
 */
Entity dt_ecs_manager_new_entity(DtEcsManager* manager);

/**
 * @brief return entity data
 *
 * @param manager manager that contains entities
 * @param entity entity that we tried find
 *
 * @warning return ENTITY_INFO_NULL if entity was killed or out of range
 */
EntityInfo dt_ecs_manager_get_entity(const DtEcsManager* manager, Entity entity);

/**
 * @brief return id of entity parent
 *
 * @param manager manager that contains entity
 * @param entity entity that parent we tried to find
 *
 * @warning return ENTITY_NULL(-1) if entity hasn't parent
 */
Entity dt_ecs_manager_get_parent(const DtEcsManager* manager, Entity entity);

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
void dt_ecs_manager_set_parent(const DtEcsManager* manager, Entity child, Entity parent);

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
void dt_ecs_manager_add_child(const DtEcsManager* manager, Entity parent, Entity child);

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
void dt_ecs_manager_remove_child(const DtEcsManager* manager, Entity parent,
                                 Entity child);

/**
 * @brief return pointer to child pool of entity
 *
 * @param manager manager that contains entities
 * @param entity parent of returned entities
 * @param count pointer to count value
 *
 * @note func always override count value
 */
EntityInfo** dt_ecs_manager_get_children(const DtEcsManager* manager, Entity entity,
                                         u16* count);

/**
 * @brief move entity to recycle array
 *
 * @param manager manager that contains entities
 * @param entity entity that we want to kill
 *
 * @note multiply gen by -1
 */
void dt_ecs_manager_kill_entity(DtEcsManager* manager, Entity entity);

/**
 * @brief return count of components on entity
 *
 * @param manager manager that contains entities
 * @param entity entity that we tried find
 *
 * @note return 0 if entity was killed or out of range
 */
size_t dt_ecs_manager_get_entity_components_count(const DtEcsManager* manager,
                                                  Entity entity);

/**
 * @brief return count of components on entity
 *
 * @param manager manager that contains entities
 * @param entity entity that we tried find
 *
 * @note return -1 if entity was killed or out of range
 */
uint16_t dt_ecs_manager_get_entity_gen(const DtEcsManager* manager, Entity entity);

/**
 * @brief copy all component data from src to dst
 *
 * @param manager manager that contains entities
 * @param dst destination
 * @param src source
 */
void dt_ecs_manager_copy_entity(const DtEcsManager* manager, Entity dst, Entity src);

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
void dt_ecs_manager_entity_add_component(DtEcsManager* manager, Entity entity, u16 id,
                                         const void* data);

/**
 * @brief remove entity from ecs pool with this component id
 *
 * @param manager manager that contains entities
 * @param entity destination entity
 * @param id id of component type
 *
 * @note if manager hasn't ecs pool of this component func do nothing
 */
void dt_ecs_manager_entity_remove_component(DtEcsManager* manager, Entity entity, u16 id);

/**
 * @brief add pool to mager pools
 *
 * @param manager manager that contains pools
 * @param pool pool that we're adding
 */
void dt_ecs_manager_add_pool(DtEcsManager* manager, EcsPool* pool);

/**
 * @brief return pool from manager with this id
 *
 * @param manager manager that contains pools
 * @param id id of component type
 *
 * @note if manager hasn't ecs pool of this component, pool will be created and added to
 * manager
 */
EcsPool* dt_ecs_manager_get_pool_by_id(DtEcsManager* manager, u16 id);

/**
 * @brief return pool from manager with this name
 *
 * @param manager manager that contains pools
 * @param name name of component type
 *
 * @note if manager hasn't ecs pool of this component, pool will be created and added to
 * manager
 */
EcsPool* dt_ecs_manager_get_pool_by_name(DtEcsManager* manager, const char* name);

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
void dt_on_entity_change(const DtEcsManager* manager, Entity entity, u16 id, bool added);

/**
 * @brief free all manager data
 */
void dt_ecs_manager_free(DtEcsManager* manager);

/**
 * @brief remove tool components
 *
 * @note tool components: HierarchyDirty
 */
void dt_remove_tool_components(const DtEcsManager* manager);

typedef void (*Action)(void*);
typedef void (*Init)(DtEcsManager*, void*);

/**
 * @brief pair of system data and init func
 */
typedef struct InitDataPair {
    void* data;
    Init init;
} InitDataPair;

/**
 * @brief pair of system data and some action func
 */
typedef struct ActionDataPair {
    void* data;
    Action action;
} ActionDataPair;

/**
 * @brief pair of system data and some update func
 */
typedef struct SystemHandlerConfig {
    size_t init_size;

    size_t pre_update_size;
    size_t update_size;
    size_t post_update_size;

    size_t tag_remove_size;

    size_t destroy_size;
} SystemHandlerConfig;


/**
 * @brief container for system funcs
 *
 * @warning if system hasn't any func it must be initialized by NULL
 */
typedef struct EcsSystem {
    void* data;

    Init init;

    Action pre_update;
    Action update;
    Action post_update;

    Action tag_remove;

    Action destroy;
} EcsSystem;

/**
 * @brief return new system
 *
 * @param init called on system handler initialize
 * @param pre_update called before update func
 * @param update called on main update cycle
 * @param post_update called after update func
 * @param tag_remove use for remove tags after update funcs
 * @param destroy called on system handler destroy
 *
 * @warning if system hasn't any func it must be initialized by NULL
 */
EcsSystem* ecs_system_new(Init init, Action pre_update, Action update, Action post_update,
                          Action tag_remove, Action destroy);

/**
 * @brief container where systems grouped by funcs
 */
typedef struct SystemHandler {
    DtEcsManager* manager;

    DT_VEC(InitDataPair) inits;

    DT_VEC(ActionDataPair) pre_updates;
    DT_VEC(ActionDataPair) updates;
    DT_VEC(ActionDataPair) post_updates;

    DT_VEC(ActionDataPair) tag_removes;

    DT_VEC(ActionDataPair) destroys;
} SystemHandler;

/**
 * @brief return new system handler
 *
 * @param manager ecs manager that handle pools
 * @param cfg config for initialize
 *
 * @return return pointer to new system handler
 */
SystemHandler* system_handler_new(DtEcsManager* manager, const SystemHandlerConfig* cfg);

/**
 * @brief add system in pools if system has needed func
 *
 * @param handler handler where we add system
 * @param system added system
 *
 * @return return pointer to new system handler
 */
void system_handler_add(SystemHandler* handler, const EcsSystem* system);

/**
 * @brief call all init systems funcs
 */
void system_handler_init(const SystemHandler* handler);

/**
 * @brief call all pre_update systems funcs
 */
void system_handler_pre_update(const SystemHandler* handler);

/**
 * @brief call all update systems funcs
 */
void system_handler_update(const SystemHandler* handler);

/**
 * @brief call all post_update systems funcs
 */
void system_handler_post_update(const SystemHandler* handler);

/**
 * @brief call all remove_tag systems funcs
 */
void system_handler_remove_tag(const SystemHandler* handler);

/**
 * @brief call all destroy systems funcs
 */
void system_handler_destroy(const SystemHandler* handler);

/**
 * @brief free all systems and handler
 */
void system_handler_free(SystemHandler* handler);

typedef struct {
    int bg_far_count;
    int bg_near_count;
    int world_back_count;
    int world_count;
    int world_front_count;
    int particles_count;
    int ui_count;
} DrawHandlerConfig;

typedef enum : char {
    LAYER_BG_FAR = 0,
    LAYER_BG_NEAR,
    LAYER_WORLD_BACK,
    LAYER_WORLD,
    LAYER_WORLD_FRONT,
    LAYER_PARTICLES,
    LAYER_UI,
    LAYER_DEBUG,
} DrawLayer;

typedef struct {
    void* data;
    Init init;
    Action draw;
    Action destroy;
    DrawLayer layer;
} DrawSystem;

typedef struct DrawHandler {
    DtEcsManager* manager;
    DrawHandlerConfig stats;

    void (*begin_draw)(void);
    void (*end_draw)(void);

    DrawSystem* systems;
    u16 system_count;
} DrawHandler;

DrawHandler* draw_handler_new(DtEcsManager* manager, const DrawHandlerConfig* cfg);
void draw_handler_add(DrawHandler* handler, const DrawSystem* system);
void draw_handler_init(const DrawHandler* handler);
void draw_handler_draw(const DrawHandler* handler);
void draw_handler_destroy(const DrawHandler* handler);

#endif
