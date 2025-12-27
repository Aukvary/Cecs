#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "../include/raylib.h"
#include "../../Ecs/Pools/EcsPool.h"

REGISTER_COMPONENT(Transform2D)
REGISTER_COMPONENT(Sprite)
REGISTER_COMPONENT(Collider)
REGISTER_COMPONENT(Player)
REGISTER_COMPONENT(Enemy)

//
// typedef struct {
//     Vector2 position;
//     Vector2 local_position;
//
//     Vector2 scale;
//     Vector2 local_scale;
//
//     float rotation;
//     float local_rotation;
//
//     int was_transformed;
// } Transform2D;
//
// typedef float Velocity;
//
// typedef struct {
//     Texture2D texture;
//     Color color;
//
//     Vector2 pivot;
//     Vector2 row_pivot;
//
//     Rectangle source;
//     Rectangle row_source;
//
//     Rectangle destination;
//     Rectangle row_destination;
//
//     int was_changed;
// } Sprite;
//
// typedef struct {
//     Rectangle bound;
//     Vector2 pivot;
// } Collider;
//
// typedef struct {
//
// } Player;
//
// typedef struct {
//
// } Enemy;

#endif
