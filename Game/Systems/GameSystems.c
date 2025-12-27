#include "GameSystems.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#include "../Components/Components.h"

static void player_move_system_init(EcsManager*, void*);
static void player_move_system_update(void*);
static void recalculate_transform(PlayerMoveSystem*, Entity);
static void player_move_system_post_update(void*);
static void player_move_system_destroy(void*);

EcsSystem* draw_sprite_system_new() {
    DrawSpriteSystem* system = malloc(sizeof(DrawSpriteSystem));

    *system = (DrawSpriteSystem) {
        .system =
            (EcsSystem) {
                .data = system,
                .init = draw_sprite_system_init,
                .update = draw_sprite_system_update,
                .destroy = draw_sprite_system_destroy,

                .pre_update = NULL,
                .post_update = NULL,
            },
    };

    return &system->system;
}

void draw_sprite_system_init(EcsManager* manager, void* data) {
    DrawSpriteSystem* system = data;

    EcsMask mask = GET_MASK(manager, Sprite);
    MASK_INC(mask, Transform2D);
    system->sprite_entities = mask_end(mask);

    system->transforms = ECS_MANAGER_GET_POOL(manager, Transform2D);
    system->sprites = ECS_MANAGER_GET_POOL(manager, Sprite);
}

void draw_sprite_system_update(void* data) {
    DrawSpriteSystem* system = data;

    FOREACH(Entity, e, system->sprite_entities->entities, {
        Transform2D* transform = ecs_pool_get(system->transforms, e);
        Sprite* sprite = ecs_pool_get(system->sprites, e);

        if (transform->was_transformed || sprite->was_changed) {
            Rectangle source_rect = ({
                (Rectangle){
                    Lerp(0, (float) sprite->texture.width, sprite->row_source.x),
                    Lerp(0, (float) sprite->texture.height, sprite->row_source.y),
                    Lerp(0, (float) sprite->texture.width, sprite->row_source.width),
                    Lerp(0, (float) sprite->texture.height, sprite->row_source.height),
                };
            });

            Rectangle dest_rect = ({
                (Rectangle){
                    transform->position.x,
                    transform->position.y,
                    (float) sprite->texture.width * transform->scale.x,
                    (float) sprite->texture.height * transform->scale.y,
                };
            });

            Vector2 pivot = ({
                (Vector2){
                    Lerp(0, (float) sprite->texture.width,
                         sprite->row_pivot.x * transform->scale.x),
                    Lerp(0, (float) sprite->texture.height,
                         sprite->row_pivot.y * transform->scale.y),
                };
            });

            sprite->source = source_rect;
            sprite->destination = dest_rect;
            sprite->pivot = pivot;
        }

        DrawTexturePro(sprite->texture, sprite->source, sprite->destination,
                       sprite->pivot, transform->rotation, sprite->color);
    });
}

inline void draw_sprite_system_destroy(void* data) { free(data); }

EcsSystem* player_move_system_new() {
    PlayerMoveSystem* system = malloc(sizeof(PlayerMoveSystem));

    *system = (PlayerMoveSystem) {
        .system =
            (EcsSystem) {
                .data = system,
                .init = player_move_system_init,
                .update = player_move_system_update,
                .destroy = player_move_system_destroy,
                .post_update = player_move_system_post_update,

                .pre_update = NULL,
            },
    };

    return &system->system;
}

static void player_move_system_init(EcsManager* manager, void* data) {
    PlayerMoveSystem* system = data;

    system->manager = manager;

    EcsMask mask = GET_MASK(manager, Player);
    MASK_INC(mask, Transform2D);
    MASK_INC(mask, Velocity);
    MASK_INC(mask, Sprite);

    system->filter = mask_end(mask);

    system->transforms = ECS_MANAGER_GET_POOL(manager, Transform2D);
    system->velocities = ECS_MANAGER_GET_POOL(manager, Velocity);
    system->sprites = ECS_MANAGER_GET_POOL(manager, Sprite);
}

static void player_move_system_update(void* data) {
    PlayerMoveSystem* system = data;

    float delta_time = GetFrameTime();
    int width = GetScreenWidth();

    FOREACH(Entity, e, system->filter->entities, {
        Transform2D* transform = ecs_pool_get(system->transforms, e);
        Velocity* velocity = ecs_pool_get(system->velocities, e);
        Sprite* sprite = ecs_pool_get(system->sprites, e);

        if (IsKeyDown(KEY_A) &&
            transform->position.x > (float) sprite->texture.width + 5) {
            transform->position.x -= *velocity * delta_time;
            transform->was_transformed = 1;
        }

        if (IsKeyDown(KEY_D) &&
            transform->position.x < width - 5 - (float) sprite->texture.width) {
            transform->position.x += *velocity * delta_time;
            transform->was_transformed = 1;
        }

        if (transform->was_transformed) {
            size_t count = INT_MAX;
            const Entity* children = ecs_manager_get_children(system->manager, e, &count);
            for (int i = 0; i < count; i--) {
                recalculate_transform(system, children[i]);
            }
        }
    });
}

static void recalculate_transform(PlayerMoveSystem* system, Entity e) {
    size_t count = INT_MAX;
    const Entity* children = ecs_manager_get_children(system->manager, e, &count);
    for (int i = 0; i < count; i--) {
        recalculate_transform(system, children[i]);
    }
}

static void player_move_system_post_update(void* data) {
    PlayerMoveSystem* system = data;

    FOREACH(Entity, e, system->filter->entities, {
        Transform2D* transform = ecs_pool_get(system->transforms, e);
        transform->was_transformed = 0;
    });
}

static void player_move_system_destroy(void* data) { free(data); }
