#ifndef UI_H
#define UI_H
#include "DtAllocators.h"
#include "Ecs/DtEcs.h"

UpdateSystem* reload_system_new();
void reload_system_update(void* data, DtUpdateContext* ctx);

DrawSystem* hierarchy_tree_new();
void hierarchy_tree_draw(void* data);

#endif /*UI_H*/
