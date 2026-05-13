#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <raylib.h>
#include <stddef.h>
#include "Ecs/RegisterHandler.h"

#define DT_TRANSFORM_2D(X, name)                                                                   \
    X(Vector2, position, name)                                                                     \
    X(Vector2, scale, name)                                                                        \
    X(float, rotation, name)
DT_DEFINE_COMPONENT(DtTransform2D, DT_TRANSFORM_2D);

#define DT_UI_TRANSFORM(X, name)                                                                   \
    X(Vector2, anchor_tl, name)                                                                    \
    X(Vector2, anchor_br, name)                                                                    \
    X(float, rotation, name)
DT_DEFINE_COMPONENT(DtUITransform, DT_UI_TRANSFORM);


#endif /*COMPONENTS_H*/
