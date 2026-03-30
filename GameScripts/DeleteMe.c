#include <stdio.h>
#include "GameScripts.h"
#include "scheduler/RuntimeScheduler.h"

DT_REGISTER_TAG(TestTag)
DT_REGISTER_COMPONENT(GameType, GAME_TYPE)

#ifdef EDITOR
#include "EditorApi.h"
DT_EXPORT
DtELogFuncTable func_table;
#endif

void initialize_func(DtEnvironment* game) {
    printf("initialize start\n");
#ifdef EDITOR

#else
    printf("default mode\n");
#endif
    printf("initialize end\n");
}

void deinitialize_func(DtEnvironment* game) { printf("deinit\n"); }

DT_DEFINE_MODULE("game", initialize_func, deinitialize_func)

DT_EXPORT
void test() {
#ifdef EDITOR
    func_table.log("init");
#else
    printf("init\n");
#endif
}
