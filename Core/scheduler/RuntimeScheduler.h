#ifndef LAZY_LOAD_H
#define LAZY_LOAD_H
#include <cjson/cJSON.h>
#include "Ecs/RegisterHandler.h"
#include "FileHandle.h"

#define DT_MODULE_INITIALIZE dt_module_initialize
#define DT_MODULE_INITIALIZE_STR "dt_module_initialize"

#define DT_MODULE_DEINITIALIZE dt_module_deinitialize
#define DT_MODULE_DEINITIALIZE_STR "dt_module_deinitialize"

#define DT_DEFINE_MODULE(name, init, deinit)                                                       \
    DT_EXPORT const char* dt_module_name = name;                                                   \
    DT_EXPORT void (*DT_MODULE_INITIALIZE)(DtEnvironment * env) = init;                            \
    DT_EXPORT void (*DT_MODULE_DEINITIALIZE)(DtEnvironment * env) = deinit;

// TODO: add comment
typedef struct DtEnvironment DtEnvironment;

// TODO: comment
typedef void (*TypeParser)(cJSON* src, void* dst);

// TODO: comment
typedef cJSON* (*TypeSerializer)(const void* src);

// TODO: comments
typedef struct {
    DtEnvironment* environment;
    char* name;

    DtEcsManager* manager;
    UpdateHandler* update_handler;
    DrawHandler* draw_handler;
} DtScene;

// TODO: comments
void dt_add_scenes(const char* directory);
// TODO: comments
const DtScene* dt_add_scene(const char* path);
// TODO: comments
const DtScene* dt_add_scene_from_json(const char* json, const char* name);
// TODO: comments
const DtScene* dt_scenes_get_active(void);
// TODO: comments
const DtScene* dt_scenes_set_active(const char* name);
// TODO: comments
const DtScene* dt_scenes_set_active_by(const DtScene* scene);
// TODO: comments
void dt_scene_unload(const char* name);
// TODO: comments
void dt_scene_unload_by(const DtScene* scene);
// TODO: comments
bool dt_scenes_scene_is_loaded(const char* name);

// TODO: add comment
typedef struct {
    char* name;

    void (*initialize)(DtEnvironment* env);
    void (*deinitialize)(DtEnvironment* env);

    DtEnvironment* environment;

    DT_LIB_HANDLE handle;
} ModuleInfo;
// TODO: comment
struct DtEnvironment {
    const DtScene* active_scene;
    DtRbTree modules;
    DtRbTree scenes;

    const DtComponentData** components;
    u16 components_count;
    const DtUpdateData** updaters;
    u16 updaters_count;
    const DtDrawData** drawers;
    u16 drawers_count;

    void (*add_scenes)(const char* directory);
    void (*add_scene)(const char* path);

    const DtComponentData* (*get_component)(const char* name);
    const DtUpdateData* (*get_update)(const char* name);
    const DtDrawData* (*get_draw)(const char* name);
};

// TODO: comments
typedef void (*DtEnvironmentHandle)(DtEnvironment*);

// TODO: comments
ModuleInfo* dt_module_load(DtEnvironment* env, const char* path);

// TODO: comments
void dt_module_unload(DtEnvironment* env, ModuleInfo* info);

// TODO: comments
DT_EXPORT DtEnvironment* dt_environment_instance(void);

// TODO: comments
void dt_add_parser_json_to_type(const char* type, TypeParser parser);

// TODO: comments
void dt_link_parser_json_to_type(const char* type, const char* base_type);

// TODO: comments
void dt_parse_json_to_type(const char* type, cJSON* src, void* dst);

// TODO: comments
void dt_add_serializer_type_to_json(const char* type, TypeSerializer serializer);

//TODO: comments
cJSON* dt_serialize_type_to_json(const char* type, const void* src);

#endif /*LAZY_LOAD_H*/
