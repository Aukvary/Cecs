#include <stdio.h>
#include "scheduler/RuntimeScheduler.h"
#include "GameScripts.h"

DT_REGISTER_TAG(TestTag)
DT_REGISTER_COMPONENT(GameType, GAME_TYPE)

void initialize_func(DtEnvironment* game) {
    printf("initialize start\n");
#ifdef EDITOR
    printf("editor mode\n");
#else
    printf("default mode\n");
#endif
    printf("initialize end\n");
}

void deinitialize_func(DtEnvironment* game) {
    printf("deinit\n");
}

DT_DEFINE_MODULE("game", initialize_func, deinitialize_func)

void test() {
#ifdef  EDITOR
    printf("test\n");
#else
    printf("test\n");
#endif
}