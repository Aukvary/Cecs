#ifndef DT_ECS_H
#define DT_ECS_H

/*=============================================================================
 *                               Включение заголовков
 *============================================================================*/

#include <DtNumericalTypes.h>
#include <stdbool.h>
#include <stdio.h>
#include "Collections/Collections.h"

/*=============================================================================
 *                              Базовые определения
 *============================================================================*/

typedef u16 DtEntity;
#define DT_ENTITY_NULL (0xFFFF)

/*=============================================================================
 *                        Определения типов и структур
 *============================================================================*/

/**
 * @brief Указатель на функцию для сброса элемента в EntityContainer
 * @param data Указатель на данные
 * @param size Размер данных
 */
typedef void (*DtResetItemHandler)(void* data, size_t size);

/**
 * @brief Указатель на функцию для копирования элемента в EntityContainer
 * @param dst Указатель на назначение
 * @param src Указатель на источник
 * @param size Размер данных
 */
typedef void (*DtCopyItemHandler)(void* dst, const void* src, size_t size);

/**
 * @brief Основной обработчик ECS
 */
typedef struct DtEcsManager DtEcsManager;
typedef struct DtEcsFilter DtEcsFilter;

/**
 * @brief Информация о сущности
 */
typedef struct DtEntityInfo {
    DtEntity id;
    int* components;
    int component_size;
    int component_count;

    struct DtEntityInfo* parent;
    struct DtEntityInfo** children;
    int children_size;
    int base_children_size;
    int children_count;

    int gen;
} DtEntityInfo;

/*=============================================================================
 *                             Макросы для сущностей
 *============================================================================*/

/**
 * @brief Информация о сущности с NULL id
 */
#define DT_ENTITY_INFO_NULL \
    (DtEntityInfo) { .id = DT_ENTITY_NULL }

/*=============================================================================
 *                          Функции для работы с EntityInfo
 *============================================================================*/

DtEntityInfo dt_entity_info_new(DtEntity id, u16 component_count, DtEntity children_size);
void dt_entity_info_reuse(DtEntityInfo* info);
void dt_entity_info_set_parent(DtEntityInfo* info, DtEntityInfo* parent);
void dt_entity_info_add_child(DtEntityInfo* info, DtEntityInfo* child);
void dt_entity_info_remove_child(DtEntityInfo* info, DtEntityInfo* child);
void dt_entity_info_remove_all_children(DtEntityInfo* info);

void dt_entity_info_add_component(DtEntityInfo* info, u16 id);
void dt_entity_info_remove_component(DtEntityInfo* info, u16 id);

void dt_entity_info_reset(DtEntityInfo* info);
void dt_entity_info_copy(DtEntityInfo* dst, DtEntityInfo* src);
void dt_entity_info_kill(DtEntityInfo* info);

/*=============================================================================
 *                         Контейнер сущностей (EntityContainer)
 *============================================================================*/

/**
 * @brief Контейнер для хранения данных сущностей
 * @note Используйте dense_items/dense_entities и count для итерации
 */
typedef struct DtEntityContainer {
    void* dense_items;
    DtEntity* dense_entities;
    u32 item_size;
    DtEntity dense_ptr;
    DtEntity dense_size;
    DtEntity count;

    DtEntity* sparce_entities;
    DtEntity sparse_size;

    DtEntity* recycle_entities;
    DtEntity recycle_ptr;
    DtEntity recycle_size;

    Iterator iterator;
    DtEntity iterator_ptr;

    DtResetItemHandler auto_reset;
    DtCopyItemHandler auto_copy;
} DtEntityContainer;

DtEntityContainer dt_entity_container_new(u32 item_size, DtEntity dense_size, DtEntity sparse_size,
                                     DtEntity recycle_size, DtResetItemHandler auto_reset,
                                     DtCopyItemHandler auto_copy);
void dt_entity_container_add(DtEntityContainer* container, DtEntity entity, const void* data);
int dt_entity_container_has(const DtEntityContainer* container, DtEntity entity);
void* dt_entity_container_get(const DtEntityContainer* container, DtEntity entity);
void dt_entity_container_remove(DtEntityContainer* container, DtEntity entity);
void dt_entity_container_resize(DtEntityContainer* container, u16 size);
void dt_entity_container_free(DtEntityContainer* container);

/*=============================================================================
 *                         Конфигурация ECS менеджера
 *============================================================================*/

/**
 * @brief Конфигурация для инициализации ECS менеджера
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

/*=============================================================================
 *                               Пул компонентов (EcsPool)
 *============================================================================*/

/**
 * @brief Пул компонентов с данными
 */
typedef struct DtComponentPool DtComponentPool;

/**
 * @brief Пул компонентов без данных (теги)
 */
typedef struct DtTagPool DtTagPool;

/**
 * @brief Пул данных компонентов ECS
 */
typedef struct DtEcsPool {
    const DtEcsManager* manager;

    u16 count;

    const char* name;
    u16 component_id;
    u16 ecs_manager_id;

    void* data;

    void (*add)(void*, DtEntity, const void*);
    void* (*get)(const void*, DtEntity);
    bool (*has)(const void*, DtEntity);
    void (*remove)(void*, DtEntity);
    void (*resize)(void*, size_t);
    void (*free)(void*);
} DtEcsPool;

/*=============================================================================
 *                          Макросы для создания пулов
 *============================================================================*/

/**
 * @brief Создает новый пул ECS
 * @param T Тип пула
 * @param manager ECS менеджер, содержащий пул
 * @return Новый пул ECS
 */
#define DT_ECS_POOL_NEW(T, manager) ecs_pool_new(manager, #T, sizeof(T))

/*=============================================================================
 *                         Функции для работы с пулами
 *============================================================================*/

DtEcsPool* dt_ecs_pool_new(const DtEcsManager* manager, const char* name, u16 size);
DtEcsPool* dt_ecs_pool_new_by_id(const DtEcsManager* manager, u16 id);
DtEcsPool* dt_component_pool_new(const DtEcsManager* manager, const char* name, u16 size,
                            DtResetItemHandler reset_handler, DtCopyItemHandler copy_handler);
DtEcsPool* dt_tag_pool_new(const DtEcsManager* manager, const char* name);

void dt_ecs_pool_add(DtEcsPool* pool, DtEntity entity, const void* data);
void* dt_ecs_pool_get(const DtEcsPool* pool, DtEntity entity);
int dt_ecs_pool_has(const DtEcsPool* pool, DtEntity entity);
void dt_ecs_pool_remove(DtEcsPool* pool, DtEntity entity);
void dt_ecs_pool_resize(DtEcsPool* pool, u64 size);
void dt_ecs_pool_free(DtEcsPool* pool);

/*=============================================================================
 *                              Маски ECS (EcsMask)
 *============================================================================*/

/**
 * @brief Маска с данными о фильтре ECS
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

EcsMask dt_mask_new(DtEcsManager* manager, u16 inc_size, u16 exc_size);
void dt_mask_inc(EcsMask* mask, u16 id);
void dt_mask_exc(EcsMask* mask, u16 id);
DtEcsFilter* dt_mask_end(EcsMask mask);

/*=============================================================================
 *                              Фильтр ECS (DtEcsFilter)
 *============================================================================*/

/**
 * @brief Архетип-контейнер для сущностей с идентичными компонентами
 */
struct DtEcsFilter {
    DtEcsManager* manager;
    EcsMask mask;
    DtEntityContainer entities;
};

/*=============================================================================
 *                              ECS Менеджер (DtEcsManager)
 *============================================================================*/

struct DtEcsManager {
    DtEntityInfo* sparse_entities;
    DtEntity sparse_size;
    DtEntity entities_ptr;

    DtEntity cfg_dense_size;
    DtEntity cfg_recycle_size;
    u16 component_count;
    DtEntity children_size;

    DtEntity* recycled_entities;
    DtEntity recycled_size;
    DtEntity recycled_ptr;

    DtEcsPool** pools;
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

    DtEcsPool* hierarchy_dirty_pool;
};

/*=============================================================================
 *                        Макросы для работы с менеджером
 *============================================================================*/

/**
 * @brief Возвращает пул типа T из менеджера
 * @param manager Менеджер, в котором ищем пул
 * @param T Тип компонента
 * @note Если менеджер не имеет пула типа T, создается новый пул
 */
#define DT_ECS_MANAGER_GET_POOL(manager, T) \
    ({ dt_ecs_manager_get_pool_by_name((manager), #T); })

/**
 * @brief Возвращает маску с типом T по умолчанию
 * @param manager Менеджер, содержащий маску
 * @param T Тип маски по умолчанию
 */
#define DT_GET_MASK(manager, T) \
    dt_mask_new(manager, manager->include_mask_count, manager->exclude_mask_count, \
                (DT_ECS_MANAGER_GET_POOL(manager, T))->info)

/**
 * @brief Включает тип T в маску
 * @param mask Маска, в которую включаем T
 * @param T Тип, который включаем
 */
#define DT_MASK_INC(mask, T) \
    dt_mask_inc(&(mask), (DT_ECS_MANAGER_GET_POOL(manager, T))->ecs_manager_id)

/**
 * @brief Исключает тип T из маски
 * @param mask Маска, из которой исключаем T
 * @param T Тип, который исключаем
 */
#define DT_MASK_EXC(mask, T) \
    dt_mask_exc(&(mask), (DT_ECS_MANAGER_GET_POOL(manager, T))->ecs_manager_id)

/*=============================================================================
 *                     Функции для работы с ECS менеджером
 *============================================================================*/

DtEcsManager* dt_ecs_manager_new(const DtEcsManagerConfig* cfg);
DtEntity dt_ecs_manager_new_entity(DtEcsManager* manager);
DtEntityInfo dt_ecs_manager_get_entity(const DtEcsManager* manager, DtEntity entity);
DtEntity dt_ecs_manager_get_parent(const DtEcsManager* manager, DtEntity entity);
void dt_ecs_manager_set_parent(const DtEcsManager* manager, DtEntity child, DtEntity parent);
void dt_ecs_manager_add_child(const DtEcsManager* manager, DtEntity parent, DtEntity child);
void dt_ecs_manager_remove_child(const DtEcsManager* manager, DtEntity parent,
                                 DtEntity child);
DtEntityInfo** dt_ecs_manager_get_children(const DtEcsManager* manager, DtEntity entity,
                                         u16* count);
void dt_ecs_manager_kill_entity(DtEcsManager* manager, DtEntity entity);
size_t dt_ecs_manager_get_entity_components_count(const DtEcsManager* manager,
                                                  DtEntity entity);
uint16_t dt_ecs_manager_get_entity_gen(const DtEcsManager* manager, DtEntity entity);
void dt_ecs_manager_copy_entity(const DtEcsManager* manager, DtEntity dst, DtEntity src);
void dt_ecs_manager_entity_add_component(DtEcsManager* manager, DtEntity entity, u16 id,
                                         const void* data);
void dt_ecs_manager_entity_remove_component(DtEcsManager* manager, DtEntity entity, u16 id);
void dt_ecs_manager_add_pool(DtEcsManager* manager, DtEcsPool* pool);
DtEcsPool* dt_ecs_manager_get_pool_by_id(DtEcsManager* manager, u16 id);
DtEcsPool* dt_ecs_manager_get_pool_by_name(DtEcsManager* manager, const char* name);
void dt_on_entity_change(const DtEcsManager* manager, DtEntity entity, u16 id, bool added);
void dt_ecs_manager_free(DtEcsManager* manager);
void dt_remove_tool_components(const DtEcsManager* manager);

/*=============================================================================
 *                         Определения для систем
 *============================================================================*/

typedef void (*Action)(void*);
typedef void (*Init)(DtEcsManager*, void*);

/**
 * @brief Пара данных системы и функции инициализации
 */
typedef struct InitDataPair {
    void* data;
    Init init;
} InitDataPair;

/**
 * @brief Пара данных системы и функции действия
 */
typedef struct ActionDataPair {
    void* data;
    Action action;
} ActionDataPair;

/**
 * @brief Конфигурация обработчика системы
 */
typedef struct SystemHandlerConfig {
    size_t init_size;
    size_t pre_update_size;
    size_t update_size;
    size_t post_update_size;
    size_t tag_remove_size;
    size_t destroy_size;
} SystemHandlerConfig;

/*=============================================================================
 *                              Система ECS (EcsSystem)
 *============================================================================*/

/**
 * @brief Контейнер для функций системы
 * @warning Если система не имеет какой-либо функции, она должна быть инициализирована NULL
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

EcsSystem* ecs_system_new(Init init, Action pre_update, Action update, Action post_update,
                          Action tag_remove, Action destroy);

/*=============================================================================
 *                         Обработчик систем (SystemHandler)
 *============================================================================*/

/**
 * @brief Контейнер, в котором системы сгруппированы по функциям
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

SystemHandler* dt_system_handler_new(DtEcsManager* manager, const SystemHandlerConfig* cfg);
void dt_system_handler_add(SystemHandler* handler, const EcsSystem* system);
void dt_system_handler_init(const SystemHandler* handler);
void dt_system_handler_pre_update(const SystemHandler* handler);
void dt_system_handler_update(const SystemHandler* handler);
void dt_system_handler_post_update(const SystemHandler* handler);
void dt_system_handler_remove_tag(const SystemHandler* handler);
void dt_system_handler_destroy(const SystemHandler* handler);
void dt_system_handler_free(SystemHandler* handler);

/*=============================================================================
 *                         Система отрисовки (DrawSystem)
 *============================================================================*/

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

/*=============================================================================
 *                        Обработчик отрисовки (DrawHandler)
 *============================================================================*/

typedef struct DrawHandler {
    DtEcsManager* manager;
    DrawHandlerConfig stats;

    void (*begin_draw)(void);
    void (*end_draw)(void);

    DrawSystem* systems;
    u16 system_count;
} DrawHandler;

DrawHandler* dt_draw_handler_new(DtEcsManager* manager, const DrawHandlerConfig* cfg);
void dt_draw_handler_add(DrawHandler* handler, const DrawSystem* system);
void dt_draw_handler_init(const DrawHandler* handler);
void dt_draw_handler_draw(const DrawHandler* handler);
void dt_draw_handler_destroy(const DrawHandler* handler);

#endif /* DT_ECS_H */