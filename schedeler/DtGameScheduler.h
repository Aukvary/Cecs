#ifndef DT_GAME_SCHEDULER_H
#define DT_GAME_SCHEDULER_H

#include <stdbool.h>
#include "Ecs/DtEcs.h"

typedef struct {
    char* name;
    u16 id;
    bool loaded;

    DtEcsManagerConfig manager_cfg;
    u16 updater_count;
    u16 drawer_count;

    DtEcsManager* manager;
    UpdateHandler* update_handler;
    DrawHandler* draw_handler;
} DtScene;

void dt_game_initialize(void);
bool dt_game_was_closed(void);
void dt_game_destroy(void);

const DtScene* dt_scenes_get_active(void);
const DtScene* dt_scenes_set_active_without_free(int idx);
void dt_scenes_set_active_with_free(int idx);
int dt_scenes_scene_is_load(int idx);

void dt_serialize_prefab(DtEntityInfo* info, const char* path);
DtEntityInfo dt_deserialize_entity_from_prefab(const char* path);
void dt_serialize_game_object(DtEntityInfo* info, const char* path);
DtEntityInfo dt_deserialize_entity_from_game_object(const char* path);

#endif /*DT_GAME_SCHEDULER_H*/
