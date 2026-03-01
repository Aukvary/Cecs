#ifndef LAZY_LOAD_H
#define LAZY_LOAD_H
#include "DtScheduler.h"
#include "Ecs/RegisterHandler.h"

//TODO: add comment
struct ModuleInfo {
    char* name;

    const DtComponentData* components;
    const DtUpdateData* updaters;
    const DtDrawData* drawers;

    void (*initialize)(DtScheduler* scheduler);
    void (*destroy)(DtScheduler* scheduler);
};

//TODO: add comment
ModuleInfo* dt_load_module(const char* path);
//TODO: add comment
void dt_unload_module(void* module);


//TODO: comments
void dt_add_scenes(const char* directory);
//TODO: comments
void dt_add_scene(const char* path);


//TODO: comments
DtRawEntity* dt_load_prefab(const char* path);

#endif /*LAZY_LOAD_H*/
