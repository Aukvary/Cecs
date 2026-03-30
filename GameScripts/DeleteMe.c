#include <stdio.h>
#include "GameScripts.h"
#include "scheduler/RuntimeScheduler.h"

DT_REGISTER_TAG(TestTag)
DT_REGISTER_COMPONENT(GameType, GAME_TYPE)

#ifdef EDITOR
#include "EditorApi.h"
DECLARE_EDITOR_FUNC_TABLE
#endif

void initialize_func(DtEnvironment* game) {}

void deinitialize_func(DtEnvironment* game) {}

DT_DEFINE_MODULE("game", initialize_func, deinitialize_func)

DT_EXPORT
void test() {
#ifdef EDITOR
    func_table.log("init");
#else
    printf("init\n");
#endif
}
