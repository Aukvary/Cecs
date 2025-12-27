// #ifndef ECS_TESTS_H
// #define ECS_TESTS_H
//
// #include <raylib.h>
// #include <stdio.h>
// #include "Ecs.h"
//
// typedef struct {
//     int x, y;
// } ObjectTransform;
//
// typedef struct {
//     Texture2D image;
//     Color color;
// } Sprite;
//
// typedef struct {
//     EcsSystem system;
//     EcsFilter* filter;
//
//     EcsPool* transform_pool;
//     EcsPool* sprite_pool;
// } DrawSpriteSystem;
//
// static void draw_sprite_init(EcsManager* manager, void* data) {
//     DrawSpriteSystem* system = data;
//
//     EcsMask mask = GET_MASK(manager, Sprite);
//     MASK_INC(mask, ObjectTransform);
//
//     system->filter = mask_end(mask);
//
//     system->transform_pool = ECS_MANAGER_GET_POOL(manager, ObjectTransform);
//     system->sprite_pool = ECS_MANAGER_GET_POOL(manager, Sprite);
// }
//
// static void draw_sprite_update(void* data) {
//     static float angle;
//
//     DrawSpriteSystem* system = data;
//
//     FOREACH(Entity, e, system->filter->entities, {
//         ObjectTransform* transform = ecs_pool_get(system->transform_pool, e);
//         Sprite* sprite = ecs_pool_get(system->sprite_pool, e);
//
//
//         if (IsKeyDown(KEY_A) && transform->x > 5) {
//             transform->x -= 10;
//         }
//
//         if (IsKeyDown(KEY_D) && transform->x < GetScreenWidth() - sprite->image.width) {
//             transform->x += 10;
//         }
//
//         DrawTexturePro(sprite->image,
//                        (Rectangle) {
//                            0,
//                            0,
//                            sprite->image.width,
//                            sprite->image.height,
//                        },
//                        (Rectangle) {
//                            transform->x,
//                            transform->y,
//                            sprite->image.width,
//                            sprite->image.height,
//                        },
//                        (Vector2) {
//                            sprite->image.width / 2,
//                            sprite->image.height / 2,
//                        },
//                        angle++, sprite->color);
//     });
// }
//
// static EcsSystem* draw_sprite_system_new() {
//     EcsSystem* system = malloc(sizeof(EcsSystem));
//
//     *system = (EcsSystem) {
//         .data = malloc(sizeof(DrawSpriteSystem)),
//
//         .init = draw_sprite_init,
//         .update = draw_sprite_update,
//
//         .destroy = NULL,
//         .pre_update = NULL,
//         .post_update = NULL,
//     };
//
//     return system;
// }
//
// EcsManager* get_test_ecs_manager() {
//     const EcsConfig config = {
//         .dense_size = 50,
//         .sparce_size = 500,
//         .recycle_size = 50,
//         .components_count = 5,
//         .pools_size = 25,
//         .masks_size = 10,
//         .include_mask_count = 5,
//         .exclude_mask_count = 5,
//         .filters_size = 10,
//     };
//
//     return ecs_manager_new(&config);
// }
//
// Entity get_test_entity(EcsManager* manager) {
//     Entity cup = ecs_manager_new_entity(manager);
//     Image image = LoadImage("source/cup.png");
//
//     Texture2D tex = LoadTextureFromImage(image);
//     tex.width /= 4;
//     tex.height /= 4;
//     UnloadImage(image);
//
//     EcsPool* pool1 = ECS_MANAGER_GET_POOL(manager, ObjectTransform);
//     EcsPool* pool2 = ECS_MANAGER_GET_POOL(manager, Sprite);
//
//     ECS_POOL_ADD(pool1, cup, ((ObjectTransform) {600, 450}));
//     ECS_POOL_ADD(pool2, cup, ((Sprite) {.image = tex, .color = WHITE}));
//
//     return cup;
// }
//
// SystemHandler* get_test_ecs_system(EcsManager* manager) {
//     SystemHandlerConfig sys_cfg = (SystemHandlerConfig) {
//         .init_size = 2,
//         .pre_update_size = 0,
//         .update_size = 2,
//         .post_update_size = 0,
//         .destroy_size = 0,
//     };
//
//     SystemHandler* handler = system_handler_new(manager, &sys_cfg);
//
//     system_handler_add(handler, draw_sprite_system_new());
//
//     return handler;
// }
//
// #endif
