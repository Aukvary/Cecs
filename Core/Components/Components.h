#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <raylib.h>
#include "Ecs/RegisterHandler.h"
#include <stddef.h>

#define DT_TRANSFORM_2D(X, name)                                                                   \
    X(Vector3, position, name)                                                                     \
    X(Vector3, scale, name)                                                                        \
    X(float, rotation, name)
DT_DEFINE_COMPONENT(DtTransform2D, DT_TRANSFORM_2D);




#endif /*COMPONENTS_H*/
