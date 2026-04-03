#if EDITOR
#include "EditorApi.h"
#include "scheduler/RuntimeScheduler.h"

DECLARE_EDITOR_FUNC_TABLE
DT_DEFINE_MODULE("game", NULL, NULL)

void dte_init() { func_table.log("init: %s", "init"); }
void dte_deinit() { func_table.log("deinit: %s", "deinit"); }
DTE_DECLARE_EDITOR_FUNC(dte_init, dte_deinit)

#else


int main() {


    return 0;
}
#endif

