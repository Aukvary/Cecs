#ifndef LAZY_LOAD_H
#define LAZY_LOAD_H
#include "DtGameScheduler.h"
#include "Ecs/RegisterHandler.h"

//TODO: add comment
typedef struct {
    char* name;
    void (*init)(void);
    const DtComponentData* components;
    const DtUpdateData* updaters;
    const DtDrawData* drawers;
} ModuleInfo;

//TODO: add comment
ModuleInfo* dt_load_module(const char* directory);
//TODO: add comment
void dt_unload_module(void* module);


//TODO: comments
void dt_add_all_scenes(const char* directory);
//TODO: comments
DtScene dt_add_extension_scene(void);


//TODO: comments
DtEntityInfo dt_load_prefab(const char* path);

#endif /*LAZY_LOAD_H*/
