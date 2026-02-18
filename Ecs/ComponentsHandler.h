#ifndef COMPONENTS_HANDLER_H
#define COMPONENTS_HANDLER_H

#include <DtNumericalTypes.h>
#include <stddef.h>
#include <stdio.h>

/**
 * @brief metadata of component and components field
 */
typedef struct DtComponentAttribute {
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

#define DT_DEFINE_EMPTY_COMPONENT(component_name, fields)                                          \
    typedef struct component_name {                                                                \
        fields(DT_FIELD_DECL, component_name)                                                      \
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

#endif
