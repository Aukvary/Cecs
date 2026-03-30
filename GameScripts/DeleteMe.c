#include <stdio.h>
#include "GameScripts.h"
#include "scheduler/RuntimeScheduler.h"
#include "EditorApi.h"

DT_REGISTER_TAG(TestTag)
DT_REGISTER_COMPONENT(GameType, GAME_TYPE)

#ifdef EDITOR
DECLARE_EDITOR_FUNC_TABLE
#endif

DT_DEFINE_MODULE("game", NULL, NULL)

DT_EXPORT
void test() {
#ifdef EDITOR
    func_table.log("init");
#else
    printf("init\n");
#endif
}
