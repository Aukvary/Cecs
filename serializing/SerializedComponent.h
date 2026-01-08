#ifndef SERIALIZED_COMPONENT_H
#define SERIALIZED_COMPONENT_H

#include "Ecs/Entity/Entity.h"
#include "SizeTypes.h"

#pragma pack(push, 1)
typedef struct {
    u32 magic;
    u16 size;
    u8 name_size;
} FieldHeader;

typedef struct {
    u32 magic;
    u8 name_size;
    u8 field_count;
} ComponentHeader;

typedef struct {
    u32 magic;
    u16 component_count;
} PrefabHeader;

typedef struct {
    u32 magic;
    u16 component_count;
    Entity parent;
    u16 children_count;
} SceneObjectHeader;
#pragma pack(pop)

EntityInfo deserialize_entity_from_prefab(const char* path);

#endif
