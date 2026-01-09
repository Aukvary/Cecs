#ifndef GAME_INITIALIZER_H
#define GAME_INITIALIZER_H

#include <stdbool.h>
#include "Ecs/DtEcs.h"

typedef struct DtGameScene {
    DtEcsManager* ecs_manager;
    SystemHandler* system_handler;
    DrawHandler* draw_handler;
    unsigned short id;
} DtGameScene;

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

typedef struct {
    u32 magic;
    u16 id;
    DtEcsManagerConfig manager_config;
    SystemHandlerConfig system_handler_config;
    u16 scene_object_count;
} SceneHeader;
#pragma pack(pop)

void dt_game_initialize(void);
bool dt_game_was_closed(void);
void dt_game_destroy(void);

const DtGameScene* dt_scenes_get_active(void);
const DtGameScene* dt_scenes_set_active_without_free(int idx);
void dt_scenes_set_active_with_free(int idx);
int dt_scenes_scene_is_load(int idx);

void dt_serialize_prefab(EntityInfo* info, const char* path);
EntityInfo dt_deserialize_entity_from_prefab(const char* path);
void dt_serialize_game_object(EntityInfo* info, const char* path);
EntityInfo dt_deserialize_entity_from_game_object(const char* path);

#endif
