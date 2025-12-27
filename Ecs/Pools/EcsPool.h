#ifndef ECS_POOL
#define ECS_POOL

#include <stddef.h>
#include <stdint.h>
#include "../EcsTypes.h"
#include "../Entity/Entity.h"

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
struct EcsPool {
    const EcsManager* manager;

    size_t count;

    char* name;
    int id;

    void* data;

    void (*add)(void*, Entity, const void*);
    void* (*get)(const void*, Entity);
    int (*has)(const void*, Entity);
    void (*remove)(void*, Entity);
    void (*resize)(void*, size_t);
    void (*free)(void*);
};

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
EcsPool* ecs_pool_new(const EcsManager* manager, const char* name, size_t size);

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
EcsPool* component_pool_new(const EcsManager* manager, const char* name, size_t size,
                            ResetItemHandler reset_handler, CopyItemHandler copy_handler);

/**
 * @brief return component pool
 *
 * @param manager ecs manager that contains this pool
 * @param name name of pool type
 *
 * @return new tag pool
 */
EcsPool* tag_pool_new(const EcsManager* manager, const char* name);

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
void ecs_pool_resize(EcsPool* pool, size_t size);

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
    size_t data_size;
    void* data;
} ComponentAttribute;

/**
 * @brief data of serialized component
 */
typedef struct {
    char* name;
    size_t component_size;

    int id;

    int attribute_count;
    ComponentAttribute* attributes;
} ComponentData;


/**
 * @brief register component in global pool
 *
 * @param id pointer to id field in component data struct
 * @param name name of component
 *
 * @note this func must be called by __attribute__((constructor)) func else may be
 * exceptions
 */
void register_component_id(int* id, const char* name);

/**
 * @brief get component name by id
 *
 * @param id id of component
 *
 * @note if global pool hasn't this id return NULL
 */
const char* get_component_name_by_id(int id);

/**
 * @brief get component id by name
 *
 * @param name name of component
 *
 * @note if global pool hasn't this name return -1
 */
int get_component_id_by_name(const char* name);

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
    __attribute__((constructor)) static void component_name##_register_component(void) { \
        _##component_name##_data = (ComponentData) {                                     \
            .name = #component_name,                                                     \
            .attributes = component_name##_attrs,                                        \
            .attribute_count = component_name##_attrs_count,                             \
        };                                                                               \
        register_component_id(&_##component_name##_data.id, #component_name);            \
    }                                                                                    \
    inline ComponentData component_name##_data() { return _##component_name##_data; }

/**
 * @brief data of serialized component field
 */
typedef struct {
    char* name;
    size_t type_size;

    int attribute_count;
    ComponentAttribute* attributes;
} ComponentField;


#endif
