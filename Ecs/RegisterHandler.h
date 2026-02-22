#ifndef COMPONENTS_HANDLER_H
#define COMPONENTS_HANDLER_H

#include <DtNumericalTypes.h>
#include <stdio.h>
#include "DtEcs.h"

/**
 * @brief metadata of component and components field
 */
typedef struct {
    const char* name;
    unsigned long long data_size;
    void* data;
} DtComponentAttribute;

/**
 * @brief data of serialized component
 */
typedef struct DtComponentData {
    char* name;
    unsigned short id;

    int field_count;
    char** field_names;
    int* field_offsets;

    u64 component_size;

    unsigned long long attribute_count;
    DtComponentAttribute* attributes;
} DtComponentData;

#define DT_FIELD_DECL(type, name, struct_name, ...) type name;
#define DT_FIELD_ENUM_DECL(type, name, struct_name, ...) DT_COMPONENT_FIELD_##struct_name##_##name,
#define DT_FIELD_COUNT(type, name, struct_name, ...) +1
#define DT_FIELD_NAME(type, name, struct_name, ...) #name,
#define DT_FIELD_OFFSET(type, name, struct_name, ...) offsetof(struct_name, name),
#define DT_FIELD_TYPE(type, name, struct_name, ...) #type,
#define DT_FIELD_SIZE(type, name, struct_name, ...) sizeof(type),

#define DT_DEFINE_COMPONENT(component_name, fields)                                                \
    typedef struct component_name {                                                                \
        fields(DT_FIELD_DECL, component_name)                                                      \
    } component_name;                                                                              \
    typedef struct component_name##Fields {                                                        \
        enum { fields(DT_FIELD_ENUM_DECL, component_name) };                                       \
    } component_name##_Fields;                                                                     \
    DtComponentData component_name##_data();

#define DT_DEFINE_EMPTY_COMPONENT(component_name)                                                  \
    typedef struct component_name {                                                                \
    } component_name;                                                                              \
    DtComponentData component_name##_data();

#define DT_REGISTER_COMPONENT(component_name, fields, ...)                                         \
    static DtComponentAttribute component_name##_attrs[] = {__VA_ARGS__};                          \
    static char* component_name##_field_names[] = {fields(DT_FIELD_NAME, component_name)};         \
    static int component_name##_field_offsets[] = {fields(DT_FIELD_OFFSET, component_name)};       \
    static int component_name##_attrs_count =                                                      \
        sizeof(component_name##_attrs) / sizeof(component_name##_attrs[0]);                        \
    static DtComponentData local_##component_name##_data;                                          \
    static __attribute__((constructor)) void component_name##_register_component(void) {           \
        local_##component_name##_data = (DtComponentData) {                                        \
            .name = #component_name,                                                               \
            .attributes = component_name##_attrs,                                                  \
            .attribute_count = component_name##_attrs_count,                                       \
            .field_count = 0 fields(DT_FIELD_COUNT, component_name),                               \
            .field_names = component_name##_field_names,                                           \
            .field_offsets = component_name##_field_offsets,                                       \
            .component_size = sizeof(component_name),                                              \
        };                                                                                         \
        dt_register_component(&local_##component_name##_data);                                     \
    }                                                                                              \
    DtComponentData component_name##_data() { return local_##component_name##_data; }

/**
 * @brief register component in global pool
 *
 * @param data pointer to generated data variable
 *
 * @note this func must be called by __attribute__((constructor)) func else may be
 * exceptions
 */
void dt_register_component(DtComponentData* data);

/**
 * @brief get component name by id
 *
 * @param id id of component
 *
 * @note if global pool hasn't this id return NULL
 */
const DtComponentData* dt_component_get_data_by_id(u16 id);

/**
 * @brief get component id by name
 *
 * @param name name of component
 *
 * @note if global pool hasn't this name return -1
 */
const DtComponentData* dt_component_get_data_by_name(const char* name);

typedef struct {
    char* name;
    u16 id;

    UpdateSystem* (*new)(void);
    Init init;
    CtxAction update;
    Action destroy;

    i16 priority;
} DtUpdateData;

#define DT_DEFINE_UPDATE(system_name) DtUpdateData system_name##_data();

#define DT_REGISTER_UPDATE(system_name, new_func, system)                                          \
    static DtUpdateData local_##system_name##_data;                                                \
    static __attribute__((constructor)) void dt_##system_name##_register_update(void) {            \
        local_##system_name##_data = (DtUpdateData) {                                              \
            .name = #system_name,                                                                  \
            .new = new_func,                                                                       \
            .init = (system).init,                                                                 \
            .update = (system).update,                                                             \
            .destroy = (system).destroy,                                                           \
            .priority = (system).priority,                                                         \
        };                                                                                         \
        dt_update_register(&local_##system_name##_data);                                           \
    }                                                                                              \
    DtUpdateData system_name##_data() { return local_##system_name##_data; }

void dt_update_register(DtUpdateData* data);
const DtUpdateData* dt_update_get_data_by_name(const char* name);
const DtUpdateData* dt_update_get_data_by_id(u16 id);

typedef struct {
    char* name;
    u16 id;

    DrawSystem* (*new)(void);
    Init init;
    Action draw;
    Action destroy;

    i16 priority;
} DtDrawData;

#define DT_DEFINE_DRAW(system_name) DtDrawData system_name##_data();

#define DT_REGISTER_DRAW(system_name, new_func, system)                                            \
    static DtDrawData local_##system_name##_data;                                                \
    static __attribute__((constructor)) void dt_##system_name##_register_draw(void) {              \
        local_##system_name##_data = (DtDrawData) {                                                \
            .name = #system_name,                                                                  \
            .new = new_func,                                                                       \
            .init = (system).init,                                                                 \
            .draw = (system).draw,                                                                 \
            .destroy = (system).destroy,                                                           \
            .priority = (system).priority,                                                         \
        };                                                                                         \
        dt_draw_register(&local_##system_name##_data);                                             \
    }                                                                                              \
    DtDrawData system_name##_data() { return local_##system_name##_data; }

void dt_draw_register(DtDrawData* data);
const DtDrawData* dt_draw_get_data_by_name(const char* name);
const DtDrawData* dt_draw_get_data_by_id(u16 id);

#endif /*COMPONENTS_HANDLER_H*/
