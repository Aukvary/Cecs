#ifndef DT_GAME_SCHEDULER_H
#define DT_GAME_SCHEDULER_H

#include <stdbool.h>
#include "Ecs/DtEcs.h"

//TODO: comments
typedef struct {
    char* name;
    u16 id;
    bool loaded;

    DtEcsManagerConfig manager_cfg;
    u16 updater_count;
    u16 drawer_count;
    char** systems;

    DtEcsManager* manager;
    UpdateHandler* update_handler;
    DrawHandler* draw_handler;
} DtScene;

//TODO:comment
DtScene* dt_scene;
//TODO: comments
const DtScene* dt_scenes_get_active(void);
//TODO: comments
const DtScene* dt_scenes_set_active_without_unload(int idx);
//TODO: comments
void dt_scenes_set_active_with_unload(int idx);
//TODO: comments
void dt_scene_load_by_id(int idx);
//TODO: comments
void dt_scene_unload_by_id(int idx);
//TODO: comments
bool dt_scenes_scene_is_load(int idx);
//TODO: comments
void dt_add_all_scenes(void);
//TODO: comments
DtScene dt_add_extension_scene(void );

//TODO: comments
void dt_serialize_prefab(DtEntityInfo* info, const char* path);
//TODO: comments
DtEntityInfo dt_deserialize_entity_from_prefab(const char* path);
//TODO: comments
void dt_serialize_game_object(DtEntityInfo* info, const char* path);
//TODO: comments
DtEntityInfo dt_deserialize_entity_from_game_object(const char* path);

#endif /*DT_GAME_SCHEDULER_H*/
