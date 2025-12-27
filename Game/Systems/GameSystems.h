#ifndef GAME_SYSTEMS_H
#define GAME_SYSTEMS_H

#include <../Ecs/Ecs.h>

typedef struct {
    EcsSystem system;
    EcsFilter* sprite_entities;
    EcsFilter* transformed_entities;

    EcsPool* transforms;
    EcsPool* sprites;
    EcsPool* transformed;
} DrawSpriteSystem;

EcsSystem* draw_sprite_system_new();
void draw_sprite_system_init(EcsManager*, void*);
void draw_sprite_system_pre_update(void* data);
void draw_sprite_system_update(void*);
void draw_sprite_system_post_update(void*);
void draw_sprite_system_destroy(void*);

typedef struct {
    EcsSystem system;
    EcsManager* manager;
    EcsFilter* filter;

    EcsPool* transforms;
    EcsPool* velocities;
    EcsPool* sprites;
} PlayerMoveSystem;

EcsSystem* player_move_system_new();

#endif
