#ifndef GAME_SCRIPTS_H
#define GAME_SCRIPTS_H

#include <raylib.h>


#include "Ecs/RegisterHandler.h"

#define GAME_TYPE(X, name)\
    X(Vector2, field1, name)\
    X(int, num, name)
DT_DEFINE_COMPONENT(GameType, GAME_TYPE)

#endif /*GAME_SCRIPTS_H*/
