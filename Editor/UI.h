#ifndef UI_H
#define UI_H
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <nuklear.h>
#include "raylib-nuklear.h"
#include "Ecs/DtEcs.h"
#include "scheduler/RuntimeScheduler.h"

extern struct nk_context* nk_ctx;
extern const DtComponentData** components;
extern u16 components_count;
const extern DtScene* game_scene;

UpdateSystem* reload_system_new();
void reload_system_update(void* data, DtUpdateContext* ctx);

DrawSystem* hierarchy_tree_new();
void hierarchy_tree_draw(void* data);

#endif /*UI_H*/
