#ifndef GAME_COMPONENTS_H
#define GAME_COMPONENTS_H
#include <raylib.h>
#include "EditorApi.h"
#include "scheduler/RuntimeScheduler.h"
#include "Ecs/RegisterHandler.h"

void on_change_path_to_sprite(DtEcsPool* data, DtEntity entity);
void on_change_field_test(DtEcsPool* data, DtEntity entity);

#define SPRITE(X, name)                                                                            \
    X(char*, path, name, DTE_ON_FIELD_CHANGE(on_change_path_to_sprite))                            \
    X(Texture2D, texture, name)                                                                    \
    X(Vector2, origin, name)                                                                       \
    X(Color, color, name)                                                                          \
    X(Rectangle, source, name)                                                                     \
    X(bool, horizontal_flip, name)                                                                 \
    X(bool, vertical_flip, name, DTE_ON_FIELD_CHANGE(on_change_field_test))
DT_DEFINE_COMPONENT(Sprite, SPRITE)

#define COLLIDER_GRID(X, name)                                                                     \
    X(int, cell_size, name)                                                                        \
    X(Vector2, cell_count, name)                                                                   \
    X(bool, show, name)                                                                            \
    X(Color, grid_color, name)\
    X(DT_VEC(DtEntity)**, cells, name)
DT_DEFINE_COMPONENT(ColliderGrid, COLLIDER_GRID)

#define GAME_COLLIDER_2D(X, name)                                                                          \
    X(Rectangle, source, name)                                                                     \
    X(DtEntity, grid, name)                                                                        \
    X(bool, show, name)\
    X(Vector2, cell, name, DTE_INSPECTOR_HIDE)
DT_DEFINE_COMPONENT(GameCollider2D, GAME_COLLIDER_2D)

#define GAME_CAMERA_2D(X, name)                                                                    \
    X(Vector2, target, name)                                                                       \
    X(float, rotation, name)                                                                       \
    X(float, zoom, name)
DT_DEFINE_COMPONENT(GameCamera2D, GAME_CAMERA_2D)


#endif /*GAME_COMPONENTS_H*/
