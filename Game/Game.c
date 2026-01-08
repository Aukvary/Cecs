// #include "Game.h"
//
// #include <stdio.h>
//
// #include "Components/Components.h"
// #include "Systems/GameSystems.h"
// #include "raymath.h"
//
// static EcsManager* manager;
// static SystemHandler* update_handler;
// static SystemHandler* draw_handler;
//
//
//
// static void init_entities();
// static void init_systems();
// static void init_player(EcsPool*, EcsPool*, EcsPool*, EcsPool*);
//
// void init_game() {
//     manager = ecs_manager_new(&manager_cfg);
//
//     init_entities();
//     init_systems();
// }
//
// static void init_entities() {
//     EcsPool* transform_pool = ECS_MANAGER_GET_POOL(manager, Transform2D);
//     EcsPool* sprite_pool = ECS_MANAGER_GET_POOL(manager, Sprite);
//     EcsPool* velocity_pool = ECS_MANAGER_GET_POOL(manager, Velocity);
//     EcsPool* player_pool = ECS_MANAGER_GET_POOL(manager, Player);
//
//     init_player(transform_pool, sprite_pool, velocity_pool, player_pool);
// }
//
// static void init_player(EcsPool* transform_pool, EcsPool* sprite_pool, EcsPool* velocity_pool, EcsPool* player_pool) {
//     Entity cup = ecs_manager_new_entity(manager);
//
//     Transform2D transform = (Transform2D) {
//         .position =
//             (Vector2) {
//                 (float) GetScreenWidth() * 0.5f,
//                 (float) GetScreenHeight() * 0.8f,
//             },
//         .rotation = 0,
//         .scale = (Vector2) {2.f, 2.f},
//
//         .was_transformed = 1,
//     };
//
//     Sprite sprite = (Sprite) {
//         .texture = LoadTexture("source/cup.png"),
//         .color = WHITE,
//
//         .row_source = (Rectangle) {.0f, .0f, 1.f, 1.f},
//         .row_destination = (Rectangle) {0, 0, 1, 1},
//         .row_pivot = (Vector2) {.5f, .5f},
//
//         .was_changed = 0,
//     };
//
//     Rectangle source_rect = ({
//         (Rectangle){
//             Lerp(0, (float) sprite.texture.width, sprite.row_source.x),
//             Lerp(0, (float) sprite.texture.height, sprite.row_source.y),
//             Lerp(0, (float) sprite.texture.width, sprite.row_source.width),
//             Lerp(0, (float) sprite.texture.height, sprite.row_source.height),
//         };
//     });
//
//     Rectangle dest_rect = ({
//         (Rectangle){
//             transform.position.x,
//             transform.position.y,
//             (float) sprite.texture.width * transform.scale.x,
//             (float) sprite.texture.height * transform.scale.y,
//         };
//     });
//
//     Vector2 pivot = ({
//         (Vector2){
//             Lerp(0, (float) sprite.texture.width, sprite.row_pivot.x * transform.scale.x),
//             Lerp(0, (float) sprite.texture.height,
//                  sprite.row_pivot.y * transform.scale.y),
//         };
//     });
//
//
//     sprite.source = source_rect;
//     sprite.destination = dest_rect;
//     sprite.pivot = pivot;
//
//     ECS_POOL_ADD(transform_pool, cup, transform);
//
//     ECS_POOL_ADD(sprite_pool, cup, sprite);
//
//     ECS_POOL_ADD(velocity_pool, cup, 500.0f);
//
//     ECS_POOL_ADD(player_pool, cup, (Player) {});
// }
//
// static void init_systems() {
//     update_handler = system_handler_new(manager, &systems_cfg);
//     system_handler_add(update_handler, player_move_system_new());
//
//     draw_handler = system_handler_new(manager, &systems_cfg);
//     system_handler_add(draw_handler, draw_sprite_system_new());
//
//     system_handler_init(update_handler);
//     system_handler_init(draw_handler);
// }
//
// void pre_update() {
//     system_handler_pre_update(update_handler);
// }
//
// void update() {
//     system_handler_update(update_handler);
// }
//
// void post_update() {
//     system_handler_post_update(update_handler);
// }
//
// void pre_draw() {
//     system_handler_pre_update(draw_handler);
// }
//
// void draw() {
//     system_handler_update(draw_handler);
// }
//
// void post_draw() {
//     system_handler_post_update(draw_handler);
//     ecs_manager_post_update(manager);
// }
//
// void destroy_game() {
//     ecs_manager_free(manager);
//
//     system_handler_destroy(update_handler);
//     system_handler_destroy(draw_handler);
//     system_handler_free(update_handler);
//     system_handler_free(draw_handler);
// }
