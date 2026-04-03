# EcsManager

## сигнатура
```C
typedef u16 DtEntity;
#define DT_ENTITY_NULL (0xFFFF)
#define DT_ENTITY_INFO_NULL ((DtEntityInfo) { .id = DT_ENTITY_NULL })
    

struct DtEntityInfo{
    DtEcsManager* manager;

    DtEntity id;
    u16* components;
    u16 component_size;
    u16 component_count;

    DtEntity parent;
    DtEntity* children;
    u16 children_size;
    u16 base_children_size;
    u16 children_count;
    DtIterator children_iterator;
    u16 children_iterator_ptr;

    bool alive;
    u16 gen;
};

struct DtEcsManagerConfig { //конфиг для инициализации менеджера 
    u16 dense_size;
    u16 sparse_size;
    u16 recycle_size;
    u16 children_size;
    u16 components_count;
    u16 pools_size;
    u32 masks_size;
    u32 include_mask_count;
    u32 exclude_mask_count;
    u32 filters_size;
};

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

    DT_VEC(DtEcsPool*) pools;
    DtEcsPool** pools_table;
    size_t pools_table_size;

    size_t include_mask_count;
    size_t exclude_mask_count;

    DtEcsFilter** filters;
    size_t filters_size;
    size_t filters_count;

    DT_VEC(DtEcsFilter*) * filter_by_include;
    DT_VEC(DtEcsFilter*) * filter_by_exclude;

    DtEcsPool* hierarchy_dirty_pool;
};
```

## Функции/макросы
## создание менеджера
### сигнатура
```C
DtEcsManager* dt_ecs_manager_new(DtEcsManagerConfig cfg); 
```
### использование
```C
DtEcsManager* manager = dt_ecs_manager_new(cfg);
```
## Создание
- возвращает ID сущности для дальнейшего использования
- при добавлении смещает указатель всех сущностей и возвращает предыдущуюю позицию или берёт значение из стека переработки
- если используется сущность из стека переработки, то у неё увеличивается поколение
### Сигнатура 
```C
DtEntity dt_ecs_manager_new_entity(DtEcsManager* manager);
```
### Использование
```C
DtEntity e = dt_ecs_manager_new_entity(manager);
```

## получение информации о сущности
- возвращает данные из общего массива сущностей менеджера
- если сущность мертва или её индекс выходит за границы, то возвращает DT_ENTITY_INFO_NULL
### Сигнатура
```C
DtEntityInfo dt_ecs_manager_get_entity(const DtEcsManager* manager, DtEntity entity);
```
### использование
```C
//пусть у нас 10 сущностей, где 5 мы удалили
DtEntityInfo info = dt_ecs_manager_get_entity(manager, 0); // всё сработает корректно
DtEntityInfo info = dt_ecs_manager_get_entity(manager, 5); // вернёт DT_ENTITY_INFO_NULL
DtEntityInfo info = dt_ecs_manager_get_entity(manager, 12); // вернёт DT_ENTITY_INFO_NULL
```

## получение информации о родителе сущности
- возвращает информацию из общего массива сущностей менеджера
- если у сущности нет родителя, сущность мертва или её индекс выходит за границы, то возвращает DT_ENTITY_INFO_NULL
### Сигнатура
```C
DtEntityInfo dt_ecs_manager_get_parent(const DtEcsManager* manager, DtEntity entity);
```
### использование
```C
//пусть у нас 10 сущностей
//0 родитель 1
//0 родитель 5, но мы удалили 5
DtEntityInfo info = dt_ecs_manager_get_entity(manager, 1); // всё сработает корректно
DtEntityInfo info = dt_ecs_manager_get_entity(manager, 5); // вернёт DT_ENTITY_INFO_NULL
DtEntityInfo info = dt_ecs_manager_get_entity(manager, 12); // вернёт DT_ENTITY_INFO_NULL
```

## получение информации о дочерних сущностей 
- возвращает массив дочерних сущностей
- в аргументы функции передаётся указатель на размер возвращаемого массива
- если сущность мертва или её индекс выходит за границы, то возвращает NULL и количество 0
### Сигнатура
```C
const DtEntity* dt_ecs_manager_get_children(const DtEcsManager* manager, DtEntity entity, u16* count);
```
### Использование
```C
//пусть 1, 2, 3 дочерние к 0

u16 count;
const DtEntity* children = dt_ecs_manager_get_children(manager, 0, &count);
```

## изменени родителя у сущности 
### Сигнатура
``` C
void dt_ecs_manager_set_parent(const DtEcsManager* manager, DtEntity child, DtEntity parent);
```
### использование 
```C
dt_ecs_manager_set_parent(manager, e0, e1); 
dt_ecs_manager_set_parent(manager, e0, DT_ENTITY_NULL); 
```

## Добавление дочерней сущности
### Сигнатура
```C
void dt_ecs_manager_add_child(const DtEcsManager* manager, DtEntity parent, DtEntity child);
```
### Использование
```C
dt_ecs_manager_add_child(manager, 0, 1);
```
## удаление дочерней сущности
### Сигнатура
```C
void dt_ecs_manager_remove_child(const DtEcsManager* manager, DtEntity parent, DtEntity child);
```
### Использование
```C
dt_ecs_manager_remove_child(manager, e0, e1);
```

## Получение компонентов у сущности 
### Сигнатура
```C
size_t dt_ecs_manager_get_entity_components_count(const DtEcsManager* manager, DtEntity entity);
```
### Использование
```C
size_t count = dt_ecs_manager_get_entity_components_count(manager, entity);
```
## получение поколения сущности
### Сигнатура
```C
u16 dt_ecs_manager_get_entity_gen(const DtEcsManager* manager, DtEntity entity);
```
### Использование
```C
u16 gen = dt_ecs_manager_get_entity_gen(manager, entity);
```

## копирование и перенос компонентов 
- добавляет компоненты, которых не хватает сущности
- копирует данные компонентов 
- если хотя бы одна сущность мертва или её индекс выходит за границы
### Сигнатура
```C
void dt_ecs_manager_copy_entity(const DtEcsManager* manager, DtEntity dst, DtEntity src);
```
### Использование
```C
dt_ecs_manager_copy_entity(manager, e0, e1);
```
## сброс компонентов
- значения всех компонентов сущности сбрасываются 
### Сигнатура
```C
void dt_ecs_manager_reset_entity(const DtEcsManager* manager, DtEntity entity);
```
### Использование 
```C
dt_ecs_manager_reset_entity(manager, entity);
```
## очистка компонентов 
- удаляет все компоненты с сунщности
### Сигнатура
```C
void dt_ecs_manager_clear_entity(const DtEcsManager* manager, DtEntity entity);
```
### Использование
```C
dt_ecs_manager_clear_entity(manager, entity);
```
## добавление компонентов
- передавать необходимо тип нужного компонента
- использует имя компонента, чтобы найти метаданные для работы с компонентом
- можно передать NULL, чтобы инициализировать компонент по умолчанию
- добавляет необходимый набор компонентов, если его нет 
### Сигнатура
```C
#define DT_ECS_MANAGER_ADD_TO_POOL(manager, T, entity, data)                                       \
    ({ dt_ecs_manager_entity_add_component(manager, entity, #T, data); })
```
### Использование
```C
//добавление компанента с данными
DT_ECS_MANAGER_ADD_TO_POOL(manager, MyComponent, e0, &(MyComponent){/*data*/})

//добавление компанента по умолчанию
DT_ECS_MANAGER_ADD_TO_POOL(manager, MyComponent, e1, NULL)

//добавление тега
DT_ECS_MANAGER_ADD_TO_POOL(manager, MyTag, e2, NULL)
```

## удаление компонента 
### Сигнатура
```C
#define DT_ECS_MANAGER_REMOVE_FROM_POOL(manager, T, entity)                                        \
    ({ dt_ecs_manager_entity_remove_component(manager, entity, #T); })
```
### Использование
```C
DT_ECS_MANAGER_REMOVE_FROM_POOL(manager, MyComponent, entity)
```
## удаление сущности
- добавляет индекс сущности в стек переработки
- удаляет сущность из всех наборов компонентов и фильтров 
### Сигнатура
```C
void dt_ecs_manager_kill_entity(DtEcsManager* manager, DtEntity entity);
```
### Использование
```C
dt_ecs_manager_kill_entity(manager, entity);
```
## Добавление набора компонентов 
### Сигнатура
```C
void dt_ecs_manager_add_pool(DtEcsManager* manager, DtEcsPool* pool);
```
### Использование 
```C
DtEcsPool* pool = dt_ecs_pool_new_by_name(manager, "MyComponent");
void dt_ecs_manager_add_pool(manager, pool);
```

## Получение набора компонентов
- если набора компонентов нет, то добавляет его
- необходимо передавать тип компонента 
### Сигнатура
```C
#define DT_ECS_MANAGER_GET_POOL(manager, T) ({ dt_ecs_manager_get_pool((manager), #T); })
```
### Использование
```C
DtEcsPool* pool = DT_ECS_MANAGER_GET_POOL(manager, MyComponent);
```

## Освобождение памяти
### сигнатура
```C
dt_ecs_manager_free(DtEcsManager* manager);
```
### использование
```C
DtEcsManager* manager = dt_ecs_manager_new(cfg);
dt_ecs_manager_free(manager);
```
